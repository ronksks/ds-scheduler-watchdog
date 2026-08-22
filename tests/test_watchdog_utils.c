/* META DATA
Date: 09/10/2024
Author: Gil Livneh
Reviewer:
Description: Test file for Watchdog Utilities project. */

/* Header Files Used */
#include <stdio.h> /* printf */
#include <unistd.h> /* getpid */
#include <assert.h> /* assert */
#include <time.h> /* time, ctime */

#include <signal.h>

#include "utils.h"
#include "watchdog_utils.h"

static void StopIfMax(watchdog_ty* watchdog_); /* revive_func_ty */
static task_op_status_ty ChangeToCheckIsMax(watchdog_ty* watchdog_); /* wait_func_ty */

static int CheckIfMaxTask(void* watchdog_);

/* Typedefs, structs, enums, globals */
volatile sig_atomic_t g_misses = 0; /* declared in watchdog_utils.c */

int main(int argc_, char** argv_)
{
	watchdog_ty* watchdog = WatchdogCreate(1/*seconds_between_SOL_*/, 1/*max_missed_SOL_*/, argc_, argv_, StopIfMax, NULL);
	uid_ty check_max_task = bad_uid;
	scheduler_run_status_ty status = 0;
	time_t now;

	(void)argc_;

	EXIT_IF_BAD(watchdog, WD_CREATE_FAIL, "Failed to create watchdog handle");

	watchdog->target_pid = getpid();

	EXIT_IF_BAD(WD_SUCCESS == SetSignalHandler(SIGUSR1, ResetCounterSH), WD_SIG_FAIL, "Failed to register signal handler");

	EXIT_IF_BAD(WD_SUCCESS == WatchdogAddTaskToSched(watchdog, (wd_task_op_ty)SendSignal1TSK, SEND_SIGNAL), WD_ADD_TASK_FAIL, "Failed to add SendSOLTask");
	EXIT_IF_BAD(WD_SUCCESS == WatchdogAddTaskToSched(watchdog, (wd_task_op_ty)IncCntTSK, INCREMENT_COUNTER), WD_ADD_TASK_FAIL, "Failed to add IncrementCounterTask");

	check_max_task = SchedulerAddTask(watchdog->scheduler, CheckIfMaxTask, 4/*interval*/, watchdog);
	EXIT_IF_BAD(!UIDIsEqual(bad_uid, check_max_task), WD_ADD_TASK_FAIL, "Failed to add CheckIfMaxTask");

	now = time(NULL);
	printf("Start: %s", ctime(&now));

	status = SchedulerRun(watchdog->scheduler);

	now = time(NULL);
	printf("End: %s", ctime(&now));

	DestroyWatchdog(watchdog);

	(void)status;
	return 0;
}

static void StopIfMax(watchdog_ty* watchdog_)
{
	SchedulerStop(watchdog_->scheduler);

	/* return WD_SUCCESS; */
}

static int CheckIfMaxTask(void* watchdog_)
{
	watchdog_ty* watchdog = (watchdog_ty*)watchdog_;
	SchedulerRemoveTask(watchdog->scheduler, watchdog->tasks_uids[SEND_SIGNAL]);

	if (g_misses >= watchdog->max_missed_sol)
	{
		StopIfMax(watchdog);
	}

	return TASK_SUCCESS_NO_REPEAT;
}
