/* META DATA
Date: 09/10/2024
Author: Ron Yalensky
Reviewer:
Description: Header file for Watchdog Utils project. */

#ifndef _ILRD_WATCHDOG_UTILS_H_
#define _ILRD_WATCHDOG_UTILS_H_

/* Header Files Used */
#include <sys/types.h> /* pid_t */
#include <stdatomic.h> /* atomic_int */

#include "scheduler.h" /* Scheduler API */
#include "uid.h" /* UID API */

/* Watchdog statuses */
typedef enum { WD_SUCCESS = 0, WD_CREATE_FAIL = 1, WD_SIG_FAIL = 2, WD_ADD_TASK_FAIL = 3, WD_REVIVE_FAIL = 4, WD_SYS_FAIL = 5, WD_SET_SCHED_FAIL = 6, WD_MEM_FAIL = 7 } wd_status_ty;

/* Tasks statuses */
typedef enum { TASK_FAILURE = -1, TASK_SUCCESS_NO_REPEAT, TASK_SUCCESS_REPEAT }task_op_status_ty;

/* Scheduler statuses */
typedef enum { RUN_SUCCESS, RUN_STOP, RUN_TASK_FAILURE, RUN_SYSTEM_FAILURE }scheduler_run_status_ty;

/* NUM_OF_TASKS must be last */
typedef enum { INCREMENT_COUNTER, REVIVE, SEND_SIGNAL, CHECK_FOR_SIGNAL, NUM_OF_TASKS }task_idx_ty;

typedef struct watchdog watchdog_ty;
typedef task_op_status_ty (*wd_task_op_ty)(watchdog_ty*);

typedef void (*revive_func_ty)(watchdog_ty*);
typedef task_op_status_ty (*wait_func_ty)(watchdog_ty*);

struct watchdog
{
    scheduler_ty* scheduler;
    uid_ty tasks_uids[NUM_OF_TASKS];

    pid_t target_pid;
    int seconds_between_sol; 
    int max_missed_sol; 
    char** argv; 
    
    int argc; 

	revive_func_ty revive_func;
	wait_func_ty wait_func;

};


extern volatile atomic_int g_should_stopped;

/* Watchdog Utils API */
watchdog_ty* WatchdogCreate(int seconds_between_sol_, int max_missed_sol_, int argc_, char** argv_, revive_func_ty revive_func_, wait_func_ty wait_state_func_);

void DestroyWatchdog(watchdog_ty* wd_);

/* "which_task_" should be from the enum task_idx_ty */
wd_status_ty WatchdogAddTaskToSched(watchdog_ty* wd_, wd_task_op_ty task_op_, task_idx_ty which_task_);

wd_status_ty SetSignalHandler(int sig_num_, void(*handler_)(int));


/* Scheduler TASKS */
task_op_status_ty ReviveIfNeededTSK(watchdog_ty* wd_);
task_op_status_ty IncCntTSK(watchdog_ty* wd_);
task_op_status_ty SendSignal1TSK(watchdog_ty* wd_);



/* Signal Handlers*/
void ResetCounterSH(int sig_num_); /* SIGUSER1 */

/* Extras */
int WatchdogStop(watchdog_ty* wd_);  /* SIGUSER2 */
int SetSchedulerIMP(watchdog_ty* wd_);
void BlockAllExepctUserIMP();
void KillWithSIG2IMP(pid_t to_kill_);




#endif /* _ILRD_WATCHDOG_UTILS_H_ */
