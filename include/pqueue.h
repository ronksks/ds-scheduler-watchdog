/* Date: 17/7/2024, Author: Ron Yalensky, Reviewer: Daniel (The magic mike) Ehrenberg */

/*Description: Header file for the Priority Queue project *********************/
#ifndef _ILRD_P_QUEUE_H_
#define _ILRD_P_QUEUE_H_

#include <stddef.h>   /*size_t*/

typedef struct p_queue p_queue_ty;

/*******************************************************************************
 * Returns: positive if "new_data" has higher priority than "data_to_check", 0 
   if they have the same priority or negative if "data_to_check" has higer 
   priority than "new_data"
 * Complexity: depends on user implementation and his mood
 * ****************************************************************************/
typedef int (*priority_func_ty)(void *data_to_check, void *new_data);

/*******************************************************************************
 * Returns: True (1) if "to_erase" is associated with the same element as "data"
 * Complexity: depends on user implementation and his mood
 * ****************************************************************************/
typedef int (*is_match_func_ty)(void *data, void *to_erase);

/*******************************************************************************
 * Creates a priority queue.
 * Return Value: A pointer to the created priority queue, or NULL if allocation fails
 * Notes: Must call PQDestroy at the end of use
 * Complexity: O(1) + system call complexity.
 * ****************************************************************************/
p_queue_ty *PQCreate(priority_func_ty priority_func);

/*******************************************************************************
 * Destroys the priority queue and frees its memory.
 * Arguments:
 * Notes: TODO
 * Complexity: O(n) + system call complexity 
 * ****************************************************************************/
void PQDestroy(p_queue_ty *pq);

/*******************************************************************************
 * Inserts a new element to its relevant position according to priority function
 * Return Value: success(0)/fail(1) status
 * Notes: undefined behaviour if pq is NULL
 * Time Complexity: O(n) + system call complexity
 * ****************************************************************************/
int PQEnqueue(p_queue_ty *pq, const void *data);

/*******************************************************************************
 * Removes the element with the highest priority from the priority queue
 * Return Value: data from the element dequeued
 * Notes: undefined behaviour if "pq" is NULL or the priority queue is empty
 * Complexity: O(1) +  system call complexity 
 * ****************************************************************************/
void *PQDequeue(p_queue_ty *pq);

/*******************************************************************************
 * Returns the data of the element with the highest priority from the priority queue
 * Notes: undefined behaviour if pq is NULL or if queue is empty
 * Complexity: O(1)
 * ****************************************************************************/
void *PQPeek(const p_queue_ty *pq);
/*******************************************************************************
 * Returns True(1)/False(0) according to the queue being empty or not
 * Notes: undefined behaviour if pq is NULL
 * Complexity: O(1)
 * ****************************************************************************/
int PQIsEmpty(const p_queue_ty *pq);

/*******************************************************************************
 * Description: Erase the entire data from the queue.
 * Input: Pointer to the queue.
 * Output: .
 * Note: Erase the data without delete the queue.
 * Complexity: O(n).
*******************************************************************************/
void PQClear(p_queue_ty *p_queue);

/*******************************************************************************
 * Returns number of elements in the queue
 * Notes: undefined behaviour if pq is NULL
 * Complexity: O(n)
 * ****************************************************************************/
size_t PQCount(const p_queue_ty *pq);

/*******************************************************************************
 * Removes the first element which matches specific criteria according to match_func
 * Return Value: The data found to match the condition or NULL if no such data is found
 * Notes:        undefined behaviour if "pq" or "match_func" is NULL
 * Complexity: O(n)
 * ****************************************************************************/
void *PQErase(p_queue_ty *pq, is_match_func_ty match_func, void *match_func_param);

#endif /* _ILRD_P_QUEUE_H_ */