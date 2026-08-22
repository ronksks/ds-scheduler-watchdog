#include <assert.h> /* assert */
#include <signal.h> /* sig_atomic_t*/
#include <semaphore.h> /* sem_t*/
#include <unistd.h> /* exec*/
#include <stdatomic.h> /* atomic_int */

#include "utils.h" /* EXIT_IF_BAD, RETURN_IF_BAD */
#include "watchdog.h" /* watchdog */
#include "watchdog_utils.h" /* wd_struct, Scheduler API, UID APO */


/* Global Variables */
/* are global because used by signal handlers and/or DNR() which is an interface parameterless function*/
volatile sig_atomic_t g_misses = 0; /* decleared in watchdog_utils.c */
volatile atomic_int g_should_stop = FALSE;


/* Static function declerations */
static void WatchdogApp(char* argv[]);
static void ReviveIfNeededTaskIMP(watchdog_ty* wd_);
static int ChangeToWaitIMP(watchdog_ty* wd_);
static void StopWatchdogSH(int sig_num_); /* SIGUSR2 */

int main(int argc, char* argv[])
{
    /* argv should have at least 4 params: executable_name, seconds_between_sol, max_missed_sol, argv sent from client process. */
    assert(4 <= argc );
    WatchdogApp(argv);

    return 0;
}

/* this function would do exec and become the new client app */
static void WatchdogApp(char* argv_[])
{
    watchdog_ty* wd = NULL;
    int dummy_status = 0;

    /* extract parameters from argv */
    int seconds_between_sol = 0;
    int max_missed_sol = 0;
    int argc = 0;
    assert(NULL != argv_);
    
    /* block all signals except SIGUSR1 SIGUSR2 */

    
    BlockAllExepctUserIMP();

    /* register signal handlers (SIGUSR1 = RESET from utils, SIGUSR2 is StopWatchdogSH) */
    EXIT_IF_BAD (WD_SIG_FAIL != SetSignalHandler(SIGUSR1, ResetCounterSH), WD_SIG_FAIL, "Failed to SetSignalHandler in wd_app");
    EXIT_IF_BAD (WD_SIG_FAIL != SetSignalHandler(SIGUSR2, StopWatchdogSH), WD_SIG_FAIL, "Failed to SetSignalHandler in wd_app");

    seconds_between_sol = atoi(argv_[1]);
    assert(0 != seconds_between_sol);

    max_missed_sol = atoi(argv_[2]);
    assert(0 != max_missed_sol);

    argc = atoi(argv_[3]);
    assert(0 != argc);

    /* WatchdogCreate(seconds_between_sol_, max_missed_sol_, argc_, argv_, ReviveWatchdogAppIMP, WatchdogWaitStateIMP) */    /* assign parameters */
    wd = WatchdogCreate(seconds_between_sol, max_missed_sol, argc, argv_ + 4, ReviveIfNeededTaskIMP, ChangeToWaitIMP );
    
    EXIT_IF_BAD (NULL != wd, WD_CREATE_FAIL, "Failed to create Watchdog in wd_app");
    DEBUG_ONLY(LOGVAR("%d: APP WatchdogCreate SUCCESS", getpid()));
    
    /* assign parameters */
    wd->target_pid = getppid(); 

    EXIT_IF_BAD (SUCCESS == SetSchedulerIMP(wd), WD_SET_SCHED_FAIL, "Failed set tasks in Scheduler in Watchdog app");

    EXIT_IF_BAD (TASK_FAILURE != SendSignal1TSK(wd), WD_SYS_FAIL, "Failed to SendSignal1TSK in Watchdog app");
    /* This run command should never be stopped, if it stopped, the main app would exit this process */
    dummy_status = SchedulerRun(wd->scheduler);
    (void)dummy_status;
}

static void ReviveIfNeededTaskIMP(watchdog_ty* wd_)
{
    int local_errno = 0;
    /* assert */
    assert(wd_);

    DEBUG_ONLY(LOGVAR("%d: APP revives CLIENT.", getpid()));
    EXIT_IF_BAD ((local_errno = errno , SUCCESS == execv(wd_->argv[0], wd_->argv)), local_errno, "Failed to execv Watchdog app");
}

/* Signal Handlers */
static void StopWatchdogSH(int sig_num_) /* SIGUSR2 */
{
    assert(SIGUSR2 == sig_num_);

    exit(0);
}

/* Description: Whis function is an unreachable code */
static int ChangeToWaitIMP(watchdog_ty* wd_)
{
    DEBUG_ONLY(LOGSTR("Should never reach this code."));
    (void)wd_;
    return SUCCESS;
}