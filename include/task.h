/*******************************************************************************
------------------------------------META DATA-----------------------------------
Date:        22/07/2024
Author:      Ron Yalensky
Reviewer:    
Description: Header File Task. 
*******************************************************************************/

/*******************************************************************************
                              Header files used  
*******************************************************************************/
#ifndef _ILRD_TASK_H_
#define _ILRD_TASK_H_


#include <stddef.h>     /* size_t          */
#include <time.h>       /* time_t          */
#include "uid.h"        /* uid_t, bad_uid  */
#include "scheduler.h"  /* task_op_ty      */


/*******************************************************************************
                              Macros  
*******************************************************************************/
#define _FAIL             (-1)
#define _TASK_REPEAT_     (1)
#define _TASK_NOT_REPEAT_ (0)


typedef struct task task_ty;



/*******************************************************************************
 * Description: Creates a Task.
 * Return Value: A pointer to the created Task, or NULL if allocation fails.
 * Notes: Must call TaskDestroy or SchedulerDestroy at the end of use.
 * Complexity: O(1) + system call complexity.
 * ****************************************************************************/
task_ty *TaskCreate(task_op_ty task_op, time_t repeat_interval_sec ,void *param);

/*******************************************************************************
 * Description: Creates a Task.
 * Return Value: A pointer to the created Task, or NULL in case of failure.
 * Notes: Must call TaskDestroy or SchedulerDestroy at the end of use.
 * Complexity: O(1) + system call complexity.
 * ****************************************************************************/
task_ty *TaskCreate(task_op_ty task_op, time_t repeat_interval ,void *param);

/*******************************************************************************
 * Description: Destroys the task and frees its memory.
 * Arguments:
 * Notes: If task is NULL, nothing occurs.
 * Complexity: O(1) + system call complexity.
 * ****************************************************************************/
void TaskDestroy(task_ty *task);

/*******************************************************************************
 * Description: Calculates the next execution time.
 * Return Value: 0 for success or 1 for failure
 * Notes: Undefined behavior if task is NULL.
 * Complexity: O(1).
 * ****************************************************************************/
int TaskUpdateExecuteTime(task_ty *task);

/*******************************************************************************
 * Description:
 * Arguments:
 * Notes: Undefined behavior if task is NULL.
 * Complexity: O(1).
 * ****************************************************************************/
time_t TaskGetExecuteTime(const task_ty *task);

/*******************************************************************************
 * Description: Executes the task.
 * Arguments:
 * Return Value: 0 for success/ no rescheduling. 
 *               -1 for failure.
 *               1 for reschudeling
 * Notes: Undefined behavior if task is NULL.
 * Complexity: O(1).
 * ****************************************************************************/
int TaskExecute(task_ty *task);

/*******************************************************************************
 * Description:
 * Arguments:
 * Return Value: The task's UID.
 * Notes: Undefined behavior if task is NULL.
 * Complexity: O(1).
 * ****************************************************************************/
uid_ty TaskGetUID(const task_ty *task);

/*******************************************************************************
 * Description: Checks if the two tasks match.
 * Arguments:
 * Return Value: TRUE (1) or FALSE (0).
 * Notes: Undefined behavior if either task is NULL.
 * Complexity: O(1).
 * ****************************************************************************/
int TaskIsMatch(void *task, void *uid);


#endif /*_ILRD_TASK_H_*/