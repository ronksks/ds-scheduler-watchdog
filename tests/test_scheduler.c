/*******************************************************************************
------------------------------------META DATA-----------------------------------
Date:        21/07/2024
Author:      Ron Yalensky
Reviewer:    Gil Livneh
Description: Test File Scheduler Project. 
*******************************************************************************/

/*******************************************************************************
                             Includes
*******************************************************************************/
#include <stdio.h>		/*	printf				 */
#include <assert.h>		/*	assert				 */
#include <assert.h>		/*	printf				 */
#include <stdlib.h>		/*	malloc,free			 */
#include "scheduler.h"  /*	typedefs: p_queue_ty */
#include "task.h" 		/*	typedefs: task_ty 	 */
#include "test.h"

/*******************************************************************************
                             Macros
*******************************************************************************/

#define _TRUE_ 		     (1)
#define _FALSE_		     (0)
#define _SUCCESS_ 	     (0)
#define _FAILED_ 	     (1)
#define _REPEAT_TASK_    (1)
#define _NO_REPEAT_TASK_ (0)

/*******************************************************************************
                             Static Functions Decleration 
*******************************************************************************/
static test_status_t StageOneTest(void);
static test_status_t TestAddRemove(void);
static test_status_t AddAndClear(void);

/*******************************************************************************
                             Functions Decleration
*******************************************************************************/
static int RepeatedTask1(void *param);
static int OneTimeTask2(void *param);
static int StopSchedTask(void *scheduler);
static int AddSchedTask(void *scheduler);
static int RemoveSchedTask(void *SchedulerWithUID);
static int ClearSchedTask(void *scheduler);
static int IsEmptySchedTask(void *scheduler);

/*******************************************************************************
                             Struct
*******************************************************************************/

typedef struct SchedulerWithUID
{
	scheduler_ty *scheduler;
	uid_ty task_uid;
}SchedulerWithUID_ty;

/*******************************************************************************
                             Main 
*******************************************************************************/

int main()
{
	RUNTEST(StageOneTest);
	RUNTEST(TestAddRemove);
	RUNTEST(AddAndClear);
	return (PASSED); 
}

/*******************************************************************************
                              Test 1  
*******************************************************************************/

static test_status_t StageOneTest(void)
{
	scheduler_ty *scheduler;

	int num = 5;
	int num2 = 10;
	time_t t1 = time(NULL);

	scheduler = SchedulerCreate();
	
	FUNC_TO_TEST(SchedulerCreate);
	FUNC_TO_TEST(SchedulerAddTask);
	FUNC_TO_TEST(SchedulerIsEmpty);
	FUNC_TO_TEST(SchedulerCount);
	FUNC_TO_TEST(SchedulerStop);


	REQUIRE(NULL != scheduler);

	REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &StopSchedTask, 5, (void *)scheduler)));
	
	REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &RepeatedTask1, 1, &num)));

	REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &OneTimeTask2, 3, &num2)));
	
	REQUIRE(_FALSE_ == SchedulerIsEmpty(scheduler));
	
	REQUIRE(3 == SchedulerCount(scheduler));

	/*	RUN	*/
	printf("Started Test... - %s\n", ctime(&t1));
	REQUIRE(_STOP_ == SchedulerRun(scheduler));

	SchedulerDestroy(scheduler);
	scheduler = NULL;
   	printf("Funished Test... - %s\n", ctime(&t1));

	return (PASSED);
}

/*******************************************************************************
                              Test 2
*******************************************************************************/

static test_status_t TestAddRemove(void)
{
    int num = 5;
    int num2 = 999999;
    time_t t1 = time(NULL);
    uid_ty to_remove_uid = bad_uid;
    scheduler_ty *scheduler;
    
    SchedulerWithUID_ty *sc = (SchedulerWithUID_ty*)malloc(sizeof(SchedulerWithUID_ty));  
    
    if (!sc) {
        printf("Memory allocation failed for sc\n");
        return (_FAILED_);
    }

    scheduler = SchedulerCreate();
    sc->scheduler = scheduler;
    
    FUNC_TO_TEST(SchedulerCreate);
    FUNC_TO_TEST(SchedulerAddTask);
    FUNC_TO_TEST(SchedulerRemove);
    FUNC_TO_TEST(SchedulerRun);

    REQUIRE(NULL != scheduler);

    /* Adding a task using the new AddSchedTask function */
    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &AddSchedTask, 2, (void *)scheduler)));
    
    /* for external remove - OneTimeTask2 not executed in scheduler removed before run.*/
    to_remove_uid =  SchedulerAddTask(scheduler, &OneTimeTask2, 1,  &num2);
    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, to_remove_uid));

    REQUIRE(_SUCCESS_ == SchedulerRemoveTask(scheduler, to_remove_uid));

    /* internal remove */
    sc->task_uid =  SchedulerAddTask(scheduler, &RepeatedTask1, 1,  &num);
    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, sc->task_uid));
    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &RemoveSchedTask, 6, (void *)sc)));

    /* self remove */
    to_remove_uid =  SchedulerAddTask(scheduler, &RemoveSchedTask, 8, (void *)sc);
    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, to_remove_uid));
    sc->task_uid = to_remove_uid;

    /* STOP task */
    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &StopSchedTask, 10, (void *)scheduler)));
    
    /* RUN */
    printf("Started Test... - %s\n", ctime(&t1));

    REQUIRE(_STOP_ == SchedulerRun(scheduler));

    free(sc);
    SchedulerDestroy(scheduler);
    scheduler = NULL;
    printf("Finished Test... - %s\n", ctime(&t1));

    return (PASSED);
}



static test_status_t AddAndClear(void) 
{
    scheduler_ty *scheduler;
   	time_t t1 = time(NULL);

    int arr[] = {1,2,3,4,5,6,7,8,9,10};
    size_t i = 0;


    scheduler = SchedulerCreate();

	FUNC_TO_TEST(SchedulerCreate);
	FUNC_TO_TEST(SchedulerAddTask);
	
    REQUIRE(NULL != scheduler);

    for(i = 0; i< sizeof(arr)/sizeof(arr[0]); ++i)
    {
 	    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &RepeatedTask1, i/2 + 1, &arr[i])));
    }
    
    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &ClearSchedTask,6, (void *)scheduler)));
    
    REQUIRE(_FALSE_ == UIDIsEqual(bad_uid, SchedulerAddTask(scheduler, &IsEmptySchedTask,7, (void *)scheduler)));

   /* Run the scheduler*/
   	printf("Started Test... - %s\n", ctime(&t1));

    REQUIRE(_SUCCESS_ == SchedulerRun(scheduler));

    SchedulerDestroy(scheduler);
    scheduler = NULL;
	printf("Finished Test... - %s\n", ctime(&t1));

    return (PASSED);
}
/*******************************************************************************
                              Tasks for Scheduler  
*******************************************************************************/

static int RepeatedTask1(void *param)
{
	(void)param;

	return (_REPEAT_TASK_);
}

static int OneTimeTask2(void *param)
{
	(void)param;

	return (_NO_REPEAT_TASK_);
}

static int StopSchedTask(void *scheduler)
{
	SchedulerStop((scheduler_ty *) scheduler);

	return(_NO_REPEAT_TASK_);
}


static int AddSchedTask(void *scheduler)
{
	SchedulerAddTask((scheduler_ty *) scheduler, &OneTimeTask2, 3, NULL);

	return(_NO_REPEAT_TASK_);
}

static int RemoveSchedTask(void *SchedulerWithUID)
{
	SchedulerWithUID_ty *sc = (SchedulerWithUID_ty *)SchedulerWithUID;
	SchedulerRemoveTask(sc->scheduler, sc->task_uid);

	return(_NO_REPEAT_TASK_);
}

static int ClearSchedTask(void *scheduler)
{
	SchedulerClear((scheduler_ty *) scheduler);

	return(_NO_REPEAT_TASK_);
}

static int IsEmptySchedTask(void *scheduler)
{
	SchedulerIsEmpty((scheduler_ty *) scheduler);

	return(_NO_REPEAT_TASK_);
}
