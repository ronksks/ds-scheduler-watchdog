/*******************************************************************************
------------------------------------META DATA-----------------------------------
Date:        21/07/2024
Author:      Ron Yalensky
Reviewer:    Gil Livneh
Description: Header File Scheduler Project. 
*******************************************************************************/

/*******************************************************************************
                              Header files used  
*******************************************************************************/
#ifndef _ILRD_SCHEDULER_H_
#define _ILRD_SCHEDULER_H_

#include <stddef.h> /* size_t  */
#include <time.h> /* time_t  */
#include "uid.h"    /* uid_t, bad_uid  */

/*******************************************************************************
                              Macros & Typedefs
*******************************************************************************/

enum Status {_SUCCESS_ = 0, 
              _STOP_  = 1, 
              _TASK_FAIL_ = 2,
              _SYSTEM_FAIL_ = 3

            };
              /*_TIME_FAIL_ = 4*/



typedef struct scheduler scheduler_ty;

/*******************************************************************************
 * Description: Function prototype for tasks.
 * Return: 0 for success/ no rescheduling. 
 *        -1 for failure.
 *         1 for reschudeling
 * Notes: Should be a short call back function
 * Complexity: Depends on user implementation and his mood
 * ****************************************************************************/
typedef int (*task_op_ty)(void *param);

/*******************************************************************************
*                           Scheduler Functions                                *
*******************************************************************************/
/*******************************************************************************
 * Description: Creates an empty Scheduler.
 * Return Value: A pointer to the created Scheduler, or NULL if allocation fails.
 * Notes: Must call SchedulerDestroy at the end of use.
 * Complexity: O(1) + system call complexity.
 * ****************************************************************************/
scheduler_ty *SchedulerCreate(void);

/*******************************************************************************
 * Description: Destroys the scheduler and frees its memory.
 * Arguments: Pointer to the scheduler.
 * Notes: If scheduler is NULL, nothing occurs.
 * Complexity: O(n) + system call complexity.
 * ****************************************************************************/
void SchedulerDestroy(scheduler_ty *scheduler);

/*******************************************************************************
 * Description: Inserts a new task to its correct position according to its interval
 * Arguments: Pointer to scheduler, interval time, pointer to task function, param. 
 * Return Value: In case of success the UID associated with the task_op is
   returned. In case of failuer bad_uid.
 * Notes: Undefined behaviour if task_op is NULL. 
 * Time Complexity: O(n) + system call complexity.
 * ****************************************************************************/
uid_ty SchedulerAddTask(scheduler_ty *scheduler, task_op_ty task_op, time_t interval_in_sec, void *param);

/*******************************************************************************
 * Description: Removes the task which is associated with "uid_to_remove" from
   the scheduler.
 * Return Value: "0" In case of success, "1" in case of failure.
 * Arguments: Pointer to the scheduler, uid_to_remove.
 * Notes: Undefined behaviour if "scheduler" is NULL or the scheduler is empty.
   If no task associated with "uid_to_remove" is found, nothing happens.
 * Complexity: O(n) + system call complexity.
 * ****************************************************************************/
int SchedulerRemoveTask(scheduler_ty *scheduler, uid_ty uid_to_remove);

/*******************************************************************************
 * Description: Runs the tasks in the scheduler until no more tasks are
   scheduled, a task called "SchedulerStop" or a task returned failure.
 * Arguments: Pointer to the scheduler.
 * Return Value: one of the following:
   0 for success
   1 for stopped by task
   2 for task fail
   3 for system fail
 * Notes: If there are task whose execution time has already passed, they will
   be executed immediately. Also their author will be executed immediately.
 * Undefined behaviour if scheduler is NULL.
   If the scheduler is empty, nothing happens.
 * Complexity: O(n).
 * ****************************************************************************/
int SchedulerRun(scheduler_ty *scheduler);

/*******************************************************************************
 * Description: Stops the scheduler run. /////
 * Arguments: Pointer to the scheduler.
 * Complexity: O(1).
 * ****************************************************************************/
void SchedulerStop(scheduler_ty *scheduler);

/*******************************************************************************
 * Description: Checks if the scheduler is empty.
 * Return Value: Returns True (1) / False (0) according to the scheduler being 
   empty or not.    
 * Notes: Undefined behaviour if "scheduler" is NULL.
 * Complexity: O(1).
 * ****************************************************************************/
int SchedulerIsEmpty(const scheduler_ty *scheduler);

/*******************************************************************************
 * Description: Erase all the tasks from the scheduler.
 * Note: Erase the tasks without deleting the scheduler. 
   Undefined behaviour if scheduler is NULL.
 * Complexity: O(n).
*******************************************************************************/
void SchedulerClear(scheduler_ty *scheduler);

/*******************************************************************************
 * Description: Returns number of tasks in the scheduler.
 * Arguments: Pointer to the scheduler.
 * Notes: Undefined behaviour if scheduler is NULL.
 * Complexity: O(n).
 * ****************************************************************************/
size_t SchedulerCount(const scheduler_ty *scheduler);

#endif /* _ILRD_SCHEDULER_H_ */