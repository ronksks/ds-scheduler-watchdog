/* META DATA
Date: 08/10/2024
Author: Ron Yalensky
Reviewer:
Description: Source file for Watch Dog project. */

/* feature test macro */
#define _POSIX_C_SOURCE (199506L)  /* pthread_sigmask, sigaction, sigsetops */

/* Header Files Used */

#include <assert.h> /* assert */
#include <unistd.h> /* fork */
#include <signal.h> /* sig_atomic_t, sigaction, kill */
#include <pthread.h>/* pid_t pthread_sigmask */
#include <sys/wait.h> /* waitpid */
#include <semaphore.h>/* sem_t */
#include <stdatomic.h> /* __atomic_store_n */


#include "utils.h"
#include "watchdog.h"
#include "watchdog_utils.h" /* wd_struct */

enum { WD_APP_INITIAL_ARGS = 4, ARG_LEN = 10 };


/* Watch Dog Static Functions Declerations */
static void* WatchdogThreadIMP(void* wd_);
static void InitialSH(int signum);
static void ReviveAppIMP(watchdog_ty* wd_);
static int ChangeToWaitIMP(watchdog_ty* wd_);
static void BlockSigUser1IMP(void);
static void UnBlockSigUser1IMP(void);
static int WaitForWatchdogAppTSK(watchdog_ty* wd_);
static void WdSchedClearIMP(watchdog_ty* wd_);
static wd_status_ty CreateArgvIMP(watchdog_ty* wd_);
static void ItoaIMP(char* string_, int num_);


/* Global Variables */
volatile sig_atomic_t g_misses = 0; 
volatile atomic_int g_should_stop = FALSE;
static sem_t g_is_wd_active;
static pthread_t g_wd_thread; /* global thread that MMI and DNR should join and access it */




/* Watch Dog API Implementation */

int MakeMeImmortal(int seconds_between_sol_, int max_missed_sol_, int argc_, char** argv_)
{
    int status = 0;
    watchdog_ty* wd = NULL;
    /* assert */
    assert(0 < seconds_between_sol_);
    assert(0 < max_missed_sol_);
    assert(0 < argc_);
    assert(NULL != argv_);

    /* Block SIGUSR1. 
       SIGUSR2 stays unblocked for the opposite process to signal it and kill this process*/
    BlockSigUser1IMP();

    /* sem init with 0 */
    status = sem_init(&g_is_wd_active, 0, 0);
    assert(SUCCESS == status);

    /* create WD */
    wd = WatchdogCreate(seconds_between_sol_, max_missed_sol_, argc_, argv_, ReviveAppIMP, ChangeToWaitIMP);
    RETURN_IF_BAD (NULL != wd, WD_CREATE_FAIL, "Failed to create Watchdof in wd_client");
    
    RETURN_IF_BAD (WD_SUCCESS == WatchdogAddTaskToSched(wd, ReviveIfNeededTSK, REVIVE), FAILURE, "Falied to add ReviveIfNeededTSK");

    DEBUG_ONLY(LOGVAR("%d: CLIENT created watchdog", getpid()));

    status = pthread_create(&g_wd_thread, NULL, WatchdogThreadIMP, wd);

    /* Handel pthread_create errors */
    if (EAGAIN == status)
    {
        DestroyWatchdog(wd);
     
        return FAILURE;
    }
    if (EINVAL == status)
    {
        DEBUG_ONLY(LOGVAR("%d: CLIENT pthread_create failed: invalid settings", getpid()));
        DestroyWatchdog(wd);
     
        return FAILURE;
    }
    else if (EPERM == status)
    {
        DEBUG_ONLY(LOGVAR("%d: CLIENT pthread_create failed: no permission to set scheduling parameters", getpid()));
        DestroyWatchdog(wd);
     
        return FAILURE;
    }
    assert(SUCCESS == status);

    errno = 0;  /* Reset errno before calling sem_wait */
    while (-1 == sem_wait(&g_is_wd_active) && errno == EINTR) /* EINTR- interrupted by signal */
    {
        DEBUG_ONLY(LOGVAR("%d: CLIENT sem_wait interrupted, retrying", getpid()));
    }

    if (errno != EINTR && errno != 0) /* means sem_wait did not succeed with other errno value*/
    {
        DEBUG_ONLY(LOGVAR("%d: CLIENT sem_wait failed", getpid()));
        DEBUG_ONLY(LOGVAR("CLIENT sem_wait failed errno: %d", errno));

        assert(errno != EINVAL);  /* Assert if it's a critical issue */
    }

    return SUCCESS;
}

void DoNotResuscitate(void)
{
    watchdog_ty* wd = NULL;
    int status = 0;
    int local_errno = 0;
    
    DEBUG_ONLY(LOGVAR("%d: CLIENT entered DoNotResuscitate", getpid()));
    /* g_should_stop = TRUE */
    __atomic_store_n(&g_should_stop, TRUE, __ATOMIC_SEQ_CST);

    /* clean up (join thread) wait until watchdog is finished*/
    /* ret = join_thread - returnes the wd */

    status = pthread_join(g_wd_thread, (void**)&wd);
    assert(SUCCESS == status);
        /* ret errors */

    /* destroy wd using ret */
    DestroyWatchdog(wd);

    /* unblock SIGUSR1 */
    UnBlockSigUser1IMP();

    /* DEBUG_MODE: destroy semaphore */
    DEBUG_ONLY
    (
        status = sem_destroy(&g_is_wd_active);
        local_errno = errno;
        EXIT_IF_BAD(SUCCESS == status, local_errno, "Failed destroy semaphore!\n");
        (void)status;
        
    )
}

/* Signal handlers , second handler implemented in utils*/
static void InitialSH(int signum)
{
    int status = 0;
    /* assert */
    assert(SIGUSR1 == signum);

    /* sem_post to g_is_wd_active*/
    status = sem_post(&g_is_wd_active);
    DEBUG_ONLY(
        if(-1 == status)
        {
            int local_errno = errno;
            EXIT_IF_BAD(SUCCESS == status, local_errno, "Failed to unlock semaphore!\n");
        })
       
    /* set g_misses to 0*/
    __atomic_store_n(&g_misses, 0, __ATOMIC_SEQ_CST);

    /* set signal handler SIGUSR1 to do ResetCounter with sigaction*/
        /* handle failure */
    EXIT_IF_BAD (WD_SUCCESS == SetSignalHandler(SIGUSR1, ResetCounterSH),WD_SIG_FAIL,"Failed to SetSignalHandler in wd_client");
}

static void* WatchdogThreadIMP(void* wd_)
{
    int status = 0;
    watchdog_ty* wd = (assert(wd_), (watchdog_ty*)wd_);

    BlockAllExepctUserIMP();

    EXIT_IF_BAD (WD_SUCCESS == SetSignalHandler(SIGUSR1, InitialSH), WD_SIG_FAIL, "Failed to SetSignalHandler in wd_client");

    g_misses = wd->max_missed_sol;  

    status = SchedulerRun(wd->scheduler);
    while (status == RUN_TASK_FAILURE || status == RUN_SYSTEM_FAILURE)
    {
        WdSchedClearIMP(wd);
        status = WatchdogAddTaskToSched(wd, WaitForWatchdogAppTSK, CHECK_FOR_SIGNAL);
        
        /* RETURN_IF_BAD(SUCCESS == WatchdogAddTaskToSched(wd, WaitForWatchdogAppTSK, CHECK_FOR_SIGNAL), TASK_FAILURE, "Failed to add task WaitForWatchdogAppTSK to wd scheduler"); */

        /* RETURN_IF_BAD (WD_SUCCESS == WatchdogAddTaskToSched(wd, ReviveIfNeededTSK, REVIVE), NULL, "Falied to add ReviveIfNeededTSK"); */
        status = SchedulerRun(wd->scheduler);
    }

    if (RUN_STOP == status)
    {
        DEBUG_ONLY(LOGVAR("%d: CLIENT stopped watchdog.", getpid()));

    }

    status = waitpid(wd->target_pid, NULL, 0);
    EXIT_IF_BAD(-1 != status, errno, "waitpid failed");

    DEBUG_ONLY(LOGVAR("Process %d exited successfully\n", wd->target_pid));
    return wd;
}


static void ReviveAppIMP(watchdog_ty* wd_)
{
    pid_t pid = 0;

    /* assert */
    assert(wd_);

    DEBUG_ONLY(LOGVAR("%d: CLIENT revives APP.", getpid()));
    /* handel error fork */
    pid = fork();

    if(-1 == pid) /* fork failed */
    {
        wd_->target_pid = 0;
        DEBUG_ONLY(LOGVAR("%d: CLIENT Failed to create child process.", getpid()));
        
        return;
    }

    if (0 == pid) /* child process */
    {
        int local_errno = 0;

        EXIT_IF_BAD ( WD_SUCCESS == CreateArgvIMP(wd_), WD_MEM_FAIL, "Error in CreateArgvIMP");

        EXIT_IF_BAD ((local_errno = errno , SUCCESS == execv(wd_->argv[0], wd_->argv)), local_errno, "Failed to execv Watchdog client");
    }

    /* only parent process reach here */

    /* set the target pid for the parent */

    wd_->target_pid = pid;
    DEBUG_ONLY(LOGVAR("%d: CLIENT has set its target pid", getpid()));
    DEBUG_ONLY(LOGVAR("    -> target is: %d", wd_->target_pid));
}

static int WaitForWatchdogAppTSK(watchdog_ty* wd_)
{
    int kill_status =0;

    assert(wd_);
    DEBUG_ONLY(LOGVAR("%d: CLIENT entered WaitForWatchdogAppTSK", getpid()));

    if (TRUE == g_should_stop)
    {
        DEBUG_ONLY(LOGVAR("%d: CLIENT should stop = TRUE", getpid()));
        return WatchdogStop(wd_);
    }

    if (0 == g_misses)
    {
        EXIT_IF_BAD (SUCCESS == SetSchedulerIMP(wd_), WD_SET_SCHED_FAIL, "Failed set tasks in Scheduler in Watchdog client");
        
        return TASK_SUCCESS_NO_REPEAT;
    }

    kill_status = kill(wd_->target_pid,!!wd_->target_pid * SIGUSR2);
    if(-1 == kill_status)
    {
        DEBUG_ONLY
        (
        DEBUG_ONLY(LOGVAR("%d: Failed to Send signal to target:", getpid()));
        DEBUG_ONLY(LOGVAR("-> %d", wd_->target_pid));
        EXIT_IF_BAD (EINVAL != errno, EINVAL, "kill process returned EINVAL");
        )

        EXIT_IF_BAD (EPERM != errno, EPERM, "kill process returned EPERM");
    } 
    /*WNOHANG non-blocking if child is not dead */
    RETURN_IF_BAD(-1 != waitpid(wd_->target_pid, NULL, WNOHANG), TASK_FAILURE,"Failed to wait for process in WaitForWatchdogAppTSK" );

    DEBUG_ONLY(LOGVAR("Process %d exited successfully\n", wd_->target_pid));

    wd_->revive_func(wd_);

    return TASK_SUCCESS_REPEAT;
}



static int ChangeToWaitIMP(watchdog_ty* wd_)
{
    assert(wd_);

    WdSchedClearIMP(wd_);

    RETURN_IF_BAD(WD_SUCCESS == WatchdogAddTaskToSched(wd_, WaitForWatchdogAppTSK, CHECK_FOR_SIGNAL), TASK_FAILURE, "Failed to add task WaitForWatchdogAppTSK to wd scheduler");

    return TASK_SUCCESS_NO_REPEAT;
}

static void BlockSigUser1IMP(void)
{
    int status = 0;
    sigset_t sigset;

    status = sigemptyset(&sigset);
    assert(SUCCESS == status);

    status = sigaddset(&sigset, SIGUSR1);
    assert(SUCCESS == status);

    status = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
    assert(SUCCESS == status);

}

static void UnBlockSigUser1IMP(void)
{
    int status = 0;
    sigset_t sigset;

    status = sigemptyset(&sigset);
    assert(SUCCESS == status);

    status = sigaddset(&sigset, SIGUSR1);
    assert(SUCCESS == status);
    
    status = pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);
    assert(SUCCESS == status); 
}


static void WdSchedClearIMP(watchdog_ty* wd_)
{
    size_t task_id = 0;
    assert(wd_);
    SchedulerClear(wd_->scheduler);

    /* reset uid's in tasks_uids */
    for (task_id = 0; task_id < NUM_OF_TASKS; ++task_id)
    {
        wd_->tasks_uids[task_id] = bad_uid;
    }
}

static wd_status_ty CreateArgvIMP(watchdog_ty* wd_)
{
    #ifdef _DEBUG
    static const char* WD_APP_PATHNAME = "/home/ron/git/ds/bin/debug/WD";
	#else /* NDEBUG */
	static const char* WD_APP_PATHNAME = "/home/ron/git/ds/bin/release/WD";
	#endif

    int new_argc = 0;
    char** new_argv = NULL;
    int curr = 0;

    char* buffer = (char*)calloc(ARG_LEN, sizeof(char));
    RETURN_IF_BAD (NULL != buffer, WD_MEM_FAIL, "Failed to allocate space for watchdog app's argv");

    assert(wd_);

    /* calculate the required num of pointers */
    new_argc = wd_->argc + WD_APP_INITIAL_ARGS;

    /* allocate space and store in wd_object */
    new_argv = (char**)calloc(new_argc + 1, sizeof(char*)); /* execv requires an extra NULL-pointer at the end of argv */

    RETURN_IF_BAD (NULL != new_argv, WD_MEM_FAIL, "Failed to allocate space for watchdog app's argv");
    
    /* store WD_APP_PATHNAME in first pointer */
    new_argv[0] = (char*)WD_APP_PATHNAME;

    new_argv[1] = (char*)calloc(ARG_LEN, sizeof(char));
    ItoaIMP(new_argv[1] ,wd_->seconds_between_sol);
    RETURN_IF_BAD (NULL != new_argv[1], WD_MEM_FAIL, "Error allocating memory in argv");
    
    new_argv[2] = (char*)calloc(ARG_LEN, sizeof(char));
    ItoaIMP(new_argv[2] ,wd_->max_missed_sol); 
    RETURN_IF_BAD (NULL != new_argv[2], WD_MEM_FAIL, "Error allocating memory in argv");
    
    new_argv[3] = (char*)calloc(ARG_LEN, sizeof(char));
    ItoaIMP(new_argv[3] ,wd_->argc); 
    RETURN_IF_BAD (NULL != new_argv[3], WD_MEM_FAIL, "Error allocating memory in argv");

    /* copy the client's arguments from wd_->client_argv to wd_->argv starting at index 4 */
    for (curr = 0; curr < wd_->argc; ++curr)
    {
        new_argv[curr + WD_APP_INITIAL_ARGS] = wd_->argv[curr];
    }
    
    new_argv[curr + WD_APP_INITIAL_ARGS] = NULL;
    
   wd_->argv = new_argv;

   return WD_SUCCESS;
}

/* void DestroyArgvIMP(char** argv_, size_t length_)
{
    size_t curr = 0;

    for(curr = 0; curr < length_; ++curr)
    {
        free(argv_[curr]);
        DEBUG_ONLY( argv_[curr] = DEAD_MEM(char*); )
    }

    free(argv_);
} */


static void ItoaIMP(char* string_, int num_)
{
	int num_copy = num_;
	int num_len = 0;

	/* assert */
	assert(string_);
	assert(0 < num_);

	/* compute length of number's decimal expansion */
	while (num_copy)
	{
		++num_len;
		num_copy /= 10;
	}

	string_[num_len] = 0;
	--num_len;

	/* for each digit in number */
	for ( ; 0 <= num_len; --num_len)
	{
		/* store corresponding character in string */
		string_[num_len] = num_ % 10 + '0';
		num_ /= 10;
	}
}

