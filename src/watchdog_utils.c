/* META DATA
Date: 09/10/2024
Author: Ron Yalensky
Reviewer:
Description: Source file for Watchdog Utils project. */

/* enables POSIX features, such as the sigaction function */
#define _POSIX_C_SOURCE (199506L) 

/* Header Files Used */

#include <limits.h> /* INT_MAX */
#include <assert.h> /* assert */
#include <stdlib.h> /*malloc*/
#include <signal.h>  /* sigaction, sigset_t, SIGUSR1, SIGUSR2, kill, sigfillset, sigdelset */
#include <stdatomic.h> /* __atomic_add_fetch */
#include <pthread.h> /* pthread_sigmask */
#include "scheduler.h" /* Scheduler API */

#include "utils.h" /* RETURN_IF_BAD EXIT_IF_BAD*/
#include "watchdog_utils.h"

/* Global Variables */
extern volatile sig_atomic_t g_misses;

extern volatile atomic_int g_should_stop;

/* Watchdog Utils Static Functions Declerations */
void BlockAllExepctUserIMP();

/* Watchdog Utils API Implementation */


watchdog_ty* WatchdogCreate(int seconds_between_sol_, int max_missed_sol_,int argc_, char** argv_, revive_func_ty  revive_func_, wait_func_ty  wait_func_)
{
    int task_idx = 0;
    watchdog_ty* ret = (watchdog_ty*)calloc(1, sizeof(watchdog_ty));
    
    RETURN_IF_BAD(NULL != ret, NULL,  "Memory allocation failed");

    assert(argv_);
    assert(0 < seconds_between_sol_);
    assert(0 < max_missed_sol_);
    assert(0 < argc_);
    assert(revive_func_);
    assert(wait_func_);

    ret->scheduler = SchedulerCreate();

    /* RETURN_IF_BAD(NULL != ret->scheduler, NULL, errno, "Memory allocation failed"); */

    if(!ret->scheduler)
    {
        DestroyWatchdog(ret);
        DEBUG_ONLY(ret = DEAD_MEM(watchdog_ty*));
        
        return NULL;
    }

    ret->seconds_between_sol = seconds_between_sol_;
    ret->max_missed_sol = max_missed_sol_;
    ret->argv = argv_;
    ret->argc = argc_;
    ret->revive_func = revive_func_;
    ret->wait_func = wait_func_;
    ret->target_pid = INT_MAX; /* field is assigned in wd_app and wd_client */
    
    for(task_idx = 0; task_idx < NUM_OF_TASKS; ++task_idx)
    {
        ret->tasks_uids[task_idx] = bad_uid;
    }

    return ret;
}

void DestroyWatchdog(watchdog_ty* wd_)
{
    if(NULL == wd_)
    {
        return;
    }

    SchedulerDestroy(wd_->scheduler);

    DEBUG_ONLY(
        wd_->scheduler = DEAD_MEM(scheduler_ty*);
        wd_->argv = DEAD_MEM(char**);
        wd_->revive_func = DEAD_MEM(revive_func_ty);
        wd_->wait_func = DEAD_MEM(wait_func_ty);

    );

    free(wd_);
}
wd_status_ty SetSignalHandler(int sig_num_, void(*handler_)(int))
{
    struct sigaction set_signal_handler = {0};

    set_signal_handler.sa_handler = handler_;

    RETURN_IF_BAD (SUCCESS == sigaction(sig_num_, &set_signal_handler, NULL),WD_SIG_FAIL, "Failed sigaction in SetSignalHandler");

    /* assert(errno != EFAULT);
    assert(errno != EINVAL); */

    return WD_SUCCESS;
}

void BlockAllExepctUserIMP()
{
    /* create a set of signals */
    sigset_t signal_set;
    int status = 0;
    
    /* status = fill the entire set */
    status = sigfillset(&signal_set);
    assert(SUCCESS == status);

    /* status = delete SIGUSR1 from the set */
    status = sigdelset(&signal_set, SIGUSR1);
    assert(SUCCESS == status);

    /* status = delete SIGUSR2 from the set */
    status = sigdelset(&signal_set, SIGUSR2);
    assert(SUCCESS == status);

    status = sigdelset(&signal_set, SIGSEGV);
    assert(SUCCESS == status);

    status = sigdelset(&signal_set, SIGABRT);
    assert(SUCCESS == status);

    /* block the set of signals */
    status = pthread_sigmask(SIG_SETMASK, &signal_set, NULL);
    assert(SUCCESS == status);

}

task_op_status_ty ReviveIfNeededTSK(watchdog_ty* wd_)
{
    assert(wd_);

    if(WatchdogStop(wd_))
    {
        return TASK_SUCCESS_NO_REPEAT;
    }

    /* if(max_missed_SOL <= g_misses ) */
    if(g_misses >= wd_->max_missed_sol)
    {
        /* kill target process */
        if(-1 == kill(wd_->target_pid, !!wd_->target_pid * SIGUSR2))
        {
            
            DEBUG_ONLY(LOGVAR("%d: Failed to send SIGUSR2 signal:", getpid()));
            /* handle failure */
            DEBUG_ONLY (EXIT_IF_BAD(EINVAL != errno, EINVAL, "Invalid signal was specified"));
            EXIT_IF_BAD (EPERM != errno, EPERM, "The calling process does not have permission to send the signal to any of the target processes." );
        }

        /* call rivive funciton */
        wd_->revive_func(wd_);

        return wd_->wait_func(wd_);
            /* handel failures and returns *************************************/
    }
    
    /* return Repeat */
    return TASK_SUCCESS_REPEAT;
}

wd_status_ty WatchdogAddTaskToSched(watchdog_ty* wd_, wd_task_op_ty task_op_, task_idx_ty which_task_)
{
    uid_ty new_uid = bad_uid;

    assert(wd_);
    assert(task_op_);
    assert(which_task_ < NUM_OF_TASKS);

    new_uid = SchedulerAddTask(wd_->scheduler,(task_op_ty)task_op_, wd_->seconds_between_sol, wd_);

    RETURN_IF_BAD(FALSE == UIDIsEqual(bad_uid, new_uid), WD_ADD_TASK_FAIL, "Error in SchedulerAddTask");

    wd_->tasks_uids[which_task_] = new_uid;

    return WD_SUCCESS;
}


task_op_status_ty IncCntTSK(watchdog_ty* wd_)
{
    assert(wd_);
    if(WatchdogStop(wd_))
    {
        return TASK_SUCCESS_NO_REPEAT;
    }
    DEBUG_ONLY(LOGVAR("%d: Call IncCntTSK:", getpid()));
    DEBUG_ONLY(LOGVAR("Counter was value: %d", g_misses));

    /* increment counter with atomic operation  (atomic_int)
        to prevent race condition with thread & signal handler*/
    __atomic_add_fetch(&g_misses, 1, __ATOMIC_SEQ_CST);
    DEBUG_ONLY(LOGVAR("%d: Call IncCntTSK:", getpid()));
    DEBUG_ONLY(LOGVAR("Counter is value: %d", g_misses));

    return TASK_SUCCESS_REPEAT;
}

task_op_status_ty SendSignal1TSK(watchdog_ty* wd_)
{
    int kill_status = 0;
    /* assert */
    assert(wd_);
    
    if(WatchdogStop(wd_))
    {
        return TASK_SUCCESS_NO_REPEAT;
    }
    
    /* check if other process is alive with kill(wd_->target_pid,0) */
    /* res =  kill (SIGUSR1,wd_->target_pid) */
    /* kill_status = kill(wd_->target_pid, !!wd_->target_pid * SIGUSR1); */
    kill_status = kill(wd_->target_pid, SIGUSR1);
    if(-1 == kill_status)
    {
        DEBUG_ONLY(LOGVAR("%d: Failed to Send signal to target:", getpid()));
        DEBUG_ONLY(LOGVAR("-> %d", wd_->target_pid));
        DEBUG_ONLY (EXIT_IF_BAD (EINVAL != errno, EINVAL, "kill process returned EINVAL"));
        EXIT_IF_BAD (EPERM != errno, EPERM, "kill process returned EPERM");
        if(ESRCH == errno)
        {
            wd_->revive_func(wd_);
        }

        return TASK_SUCCESS_NO_REPEAT;
    }

    DEBUG_ONLY(LOGVAR("%d: Send signal:", getpid()));
    DEBUG_ONLY(LOGVAR("      -> to: %d", wd_->target_pid));

    return TASK_SUCCESS_REPEAT;
}

/* SIGNALS */
void ResetCounterSH(int sig_num_) /* SIGUSER1 */
{
    /* assert */
    assert(SIGUSR1 == sig_num_);
    /* atomic set g_misses to 0*/
    __atomic_store_n(&g_misses, 0, __ATOMIC_SEQ_CST);
}

int WatchdogStop(watchdog_ty* wd_)  /* SIGUSER2 */
{
    assert(wd_);
    if (TRUE == __atomic_load_n(&g_should_stop, __ATOMIC_SEQ_CST))
    {
        int target_pid = wd_->target_pid;
        RETURN_IF_BAD(SUCCESS == kill(target_pid, SIGUSR2 * !!target_pid), TASK_FAILURE, "falied to kill SIGUSR2 to target in WatchdogStop");
        SchedulerStop(wd_->scheduler);
        
        DEBUG_ONLY(LOGVAR("%d: Sent SIGUSR2 Succesfully:", getpid()));

        return TRUE;
    }

    return FALSE;
}

int SetSchedulerIMP(watchdog_ty* wd_)
{
    assert(wd_);

    RETURN_IF_BAD(WD_SUCCESS == WatchdogAddTaskToSched(wd_, SendSignal1TSK, SEND_SIGNAL), WD_ADD_TASK_FAIL, "Falied to add SendSignal1TSK");
    RETURN_IF_BAD(WD_SUCCESS == WatchdogAddTaskToSched(wd_, ReviveIfNeededTSK, REVIVE), WD_ADD_TASK_FAIL, "Falied to add ReviveIfNeededTSK");
    RETURN_IF_BAD(WD_SUCCESS == WatchdogAddTaskToSched(wd_, IncCntTSK, INCREMENT_COUNTER), WD_ADD_TASK_FAIL, "Falied to add IncCntTSK");

    return SUCCESS;
}