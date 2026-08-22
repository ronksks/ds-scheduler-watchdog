/*******************************************************************************
------------------------------------META DATA-----------------------------------
Date:        22/07/2024
Author:      Ron Yalensky
Reviewer:    Gil Livneh
Description: Source File Scheduler Project. 
*******************************************************************************/

#include <stdlib.h>		/*	malloc, free		 */
#include <assert.h>		/*	assert				 */
#include <unistd.h>		/*	sleep 				 */
#include <time.h> 		/*  time_t  			 */
#include "scheduler.h"  /* 	scheduler_ty  		 */
#include "task.h" 		/* 	task API			 */
#include "pqueue.h" 	/* 	pqueue API			 */
#include "uid.h"    	/* 	uid API				 */


/*******************************************************************************
*                           Macrcos			                                   *
*******************************************************************************/

#define _TRUE_      	 	 ( 1 )
#define _FALSE_     	 	 ( 0 )
#define _SUCCESS_   	 	 ( 0 )
#define _FAILED_    	 	 ( 1 )
#define _STOP_	    	 	 ( 1 )
#define _TIME_FAIL_ 	 	 (-1 )
#define _FAILURE_   	 	 (-1 )
#define _TASK_FAIL_ 	 	 ( 2 )
#define _TASK_RESCHUDEL_ 	 ( 1 )
#define _TASK__NO_RESCHUDEL_ ( 0 )


/*******************************************************************************
*                           Structs			                                   *
*******************************************************************************/
struct scheduler
{
    p_queue_ty *pq;
    task_ty *cur_task;
    int to_remove_cur;
    int to_stop;

};


/*******************************************************************************
*                           Static Functions                                *
*******************************************************************************/

static int PriorityFunc(void *task1, void *task2);

/* Checks task exectiotion time, and put the program to sleep until the task is due.*/
static void HandleExecTimeAndSleep(task_ty *task, time_t cur_time);

/* Gets the current time, if it fails -> stops scheduler run. */
static time_t CheckAndToggleStop(scheduler_ty *scheduler);


/*******************************************************************************
*                           Scheduler Functions                                *
*******************************************************************************/

scheduler_ty *SchedulerCreate(void)
{
	scheduler_ty *scheduler = (scheduler_ty *)malloc(sizeof(scheduler_ty));

    p_queue_ty *pq = PQCreate(PriorityFunc);

	if ((NULL == scheduler) || (NULL == pq))
	{
		free(scheduler);
		scheduler = NULL;

		PQDestroy(pq);
		pq = NULL;

		return(NULL);
	}

	scheduler->pq = pq;
	scheduler->cur_task = NULL;
	scheduler->to_remove_cur = _FALSE_;
	scheduler->to_stop = _FALSE_;

	return(scheduler);
}

void SchedulerDestroy(scheduler_ty *scheduler)
{
	if (NULL == scheduler)
	{
		return;
	}

	SchedulerClear(scheduler);

	PQDestroy(scheduler->pq);
	scheduler->pq = NULL;
	
	free(scheduler);
	scheduler = NULL;
}


uid_ty SchedulerAddTask(scheduler_ty *scheduler, task_op_ty task_op, time_t interval_in_sec, void *param)
{
		task_ty *new_task = NULL;
		
		assert(NULL != scheduler);
		assert(NULL != task_op);
		assert(0 < interval_in_sec);

		new_task = TaskCreate(task_op, interval_in_sec, param);

		if(NULL == new_task)
		{
			return(bad_uid); /* task wasnt created*/
		}

		if(_FAILED_ == PQEnqueue(scheduler->pq, new_task))
		{
			TaskDestroy(new_task);
			new_task = NULL;

			return(bad_uid); /* task couldnt enqueue*/
		}

		return(TaskGetUID(new_task));
}


int SchedulerRemoveTask(scheduler_ty *scheduler, uid_ty uid_to_remove)
{
    	task_ty *removed_task = NULL;

    	assert(NULL != scheduler);
   
    	removed_task = PQErase(scheduler->pq, TaskIsMatch, &uid_to_remove);

    	if(NULL == removed_task)
    	{
    		if(NULL == scheduler->cur_task)
    		{
				return (_FAILURE_);
    		}
    		else if (_TRUE_ == UIDIsEqual(uid_to_remove, TaskGetUID(scheduler->cur_task)))
			{
				scheduler->to_remove_cur = _TRUE_;
				return (_SUCCESS_);
			}
			else
			{
				return (_FAILURE_);
			}

    	}

		TaskDestroy(removed_task);
		removed_task = NULL;

    	return(_SUCCESS_);
}


int SchedulerRun(scheduler_ty *scheduler)
{
    time_t cur_time = 0; 
    int task_res = 0;

    assert(NULL != scheduler);

	scheduler->to_stop = _FALSE_;
	scheduler->to_remove_cur = _FALSE_;

    while ((_TRUE_ != SchedulerIsEmpty(scheduler)) && (_TRUE_ != scheduler->to_stop)) 
    {
	    cur_time = CheckAndToggleStop(scheduler);

	    if (_TIME_FAIL_ == cur_time) 	
	    {
	        return (_SYSTEM_FAIL_); /* check return code */
	    }

        scheduler->cur_task = PQDequeue(scheduler->pq);

        HandleExecTimeAndSleep(scheduler->cur_task, cur_time);

        task_res = TaskExecute(scheduler->cur_task);

        if (_TRUE_ == scheduler->to_remove_cur) /*	checks if the running task ask to remove itself*/
		{
			TaskDestroy(scheduler->cur_task);
			scheduler->cur_task = NULL;
			scheduler->to_remove_cur = _FALSE_;
		}

        if (_FAILURE_ ==  task_res) /* failure*/
        {

			TaskDestroy(scheduler->cur_task);
			scheduler->cur_task = NULL;

            return(_TASK_FAIL_); /* stop flag*/
        }
        else if(_TASK_RESCHUDEL_ == task_res ) /* task rescheduel*/
        {

        	if(_SUCCESS_ == TaskUpdateExecuteTime(scheduler->cur_task)) 
        	{
        		if(_FAILED_ == PQEnqueue(scheduler->pq, scheduler->cur_task))
        		{
        			TaskDestroy(scheduler->cur_task);
        			scheduler->cur_task = NULL;

        			return(_SYSTEM_FAIL_);
        		}
       			scheduler->cur_task = NULL;
        	}
        	else
        	{
				return(_SYSTEM_FAIL_); /* check return value in test*/
        	}
        }
        else if(_TASK__NO_RESCHUDEL_ == task_res)	/* do not repeat */
        {
			TaskDestroy(scheduler->cur_task);
			scheduler->cur_task = NULL;
        }

		if (_TRUE_ == scheduler->to_stop)
		{
			scheduler->to_stop = _FALSE_;

			return(_STOP_);
		}
	}
    
    return(_SUCCESS_);
}


void SchedulerStop(scheduler_ty *scheduler)
{
	assert(NULL != scheduler);

	scheduler->to_stop = _TRUE_;
}


int SchedulerIsEmpty(const scheduler_ty *scheduler)
{
	assert(NULL != scheduler);

	return(_TRUE_== PQIsEmpty(scheduler->pq) && (NULL == scheduler->cur_task));
}


void SchedulerClear(scheduler_ty *scheduler)
{
	task_ty *task = NULL;

	assert(NULL != scheduler);
	
	while(_TRUE_ != PQIsEmpty(scheduler->pq))
	{
		task = PQDequeue(scheduler->pq);
		TaskDestroy(task);
		task = NULL;
	}

	if (NULL != scheduler->cur_task)
    {
        scheduler->to_remove_cur = _TRUE_;
    }
}


size_t SchedulerCount(const scheduler_ty *scheduler)
{
	assert(NULL != scheduler);

	return(PQCount(scheduler->pq) + (NULL != scheduler->cur_task));
}


/*******************************************************************************
                             Static Functions Defenitions
*******************************************************************************/

static int PriorityFunc(void *task1, void *task2)
{
	assert(NULL != task1);
	assert(NULL != task2);

	return(TaskGetExecuteTime((task_ty *)task1) - TaskGetExecuteTime((task_ty *)task2));
}


static void HandleExecTimeAndSleep(task_ty *task, time_t cur_time) 
{
    time_t exec_time = TaskGetExecuteTime(task);
    int time_to_sleep = exec_time - cur_time;

 	if (exec_time > cur_time)
 	{
		while( 0 < time_to_sleep)
		 {
		 	time_to_sleep = sleep((unsigned int)time_to_sleep);
		 }
	}
}


static time_t CheckAndToggleStop(scheduler_ty *scheduler) 
{
    time_t cur_time = time(NULL);

    if (_TIME_FAIL_ == cur_time) 
    {
        SchedulerStop(scheduler);

        return (_TIME_FAIL_); /* check return code */
    }

    return (cur_time);
}