/*******************************************************************************
------------------------------------META DATA-----------------------------------
Date:        22/07/2024
Author:      Ron Yalensky
Reviewer:    
Description: Source File task Project. 
*******************************************************************************/


#include "task.h" 	/*	typedefs: task_ty 	*/
#include "uid.h"    /* 	uid_t, bad_uid  	*/
#include <stdlib.h>	/*	malloc, free		*/
#include <assert.h>	/*	assert				*/


/*******************************************************************************
                              Macros  
*******************************************************************************/


#define _TRUE_      ( 1 )
#define _FALSE_     ( 0 )
#define _FAILED_    ( 1 )
#define _TIME_FAIL_ (-1 )


/*******************************************************************************
*                           Structs			                                   *
*******************************************************************************/

struct task
{
    time_t repeat_interval_sec;
    time_t next_exec_time;
    task_op_ty task_op;
    void *param;
    uid_ty uid;
};


/*******************************************************************************
*                           Task Functions	                                   *
*******************************************************************************/
task_ty *TaskCreate(task_op_ty task_op, time_t repeat_interval_sec ,void *param)
{
	task_ty *task = (task_ty *)malloc(sizeof(task_ty));

	time_t cur_time;	/* check assigment value	*******************/
	
	task->uid = UIDCreate();

	if((NULL == task) || (_TRUE_ == UIDIsEqual(task->uid, bad_uid)))
	{
		free(task);
		task = NULL;
		return(NULL);
	}

	task->task_op = task_op;
	task->repeat_interval_sec = repeat_interval_sec;
	task->param = param;


	cur_time = time(NULL);

	if(_TIME_FAIL_ == cur_time)
	{
		return(NULL);
	}
	task->next_exec_time = cur_time + task->repeat_interval_sec;

	return(task);

}


void TaskDestroy(task_ty *task)
{
	free(task);
	task = NULL;
}


int TaskUpdateExecuteTime(task_ty *task)
{
	time_t cur_time = time(NULL);
	
	assert(NULL != task);

	if(_TIME_FAIL_ == cur_time)
	{
		return(_TIME_FAIL_);
	}

	task->next_exec_time = cur_time + task->repeat_interval_sec;

	return(_SUCCESS_);
}


time_t TaskGetExecuteTime(const task_ty *task)
{
	assert(NULL != task);

	return(task->next_exec_time);
}


int TaskExecute(task_ty *task)
{
	assert(NULL != task);
	assert(NULL != task->task_op);

	return(task->task_op(task->param));
}


uid_ty TaskGetUID(const task_ty *task)
{
	assert(NULL != task);

	return(task->uid);
}


int TaskIsMatch(void *task, void *uid)
{
	assert(NULL != task);
	assert(NULL != uid);

	return(_TRUE_ == UIDIsEqual(TaskGetUID((task_ty *)task), *(uid_ty *)uid));
}
