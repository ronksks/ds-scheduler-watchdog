/* Date: 11/7/2024, Author: Ron Yalensky, Reviewer: GIl Livneh */
#ifndef _ILRD_DLIST_H_
#define _ILRD_DLIST_H_

#include <stddef.h> /* size_t */




/*******************************************************************************
							Type definitions
*******************************************************************************/
typedef struct dlist_node dlist_node_ty;
typedef struct dlist dlist_ty;

/* Do not touch, can change in the future. */
typedef dlist_node_ty *dlist_iter_ty;

/*******************************************************************************
 * Returns 1 (true) or 0 ( false) if data matches requested condition.
 * user_param is optional 
 * ****************************************************************************/
typedef int (*dlist_op_ty)(void *data, void *param); 

/*******************************************************************************
 * Perform requested operation on "data",
 * returns 0 for false or 1 for true
 * ****************************************************************************/
typedef int (*dlist_is_match_ty)(void *data, void *param);


/*******************************************************************************
						Doubly Linked List functions
*******************************************************************************/

/*******************************************************************************
 * Creates a doubly linked list.
 * Return Value: A pointer to the created dlist, or NULL if allocation fails.
 * Notes: Undefined behavior if capacity is 0.
 *        Must call DListDestroy at the end of use
 * Complexity: O(1) + system call complexity.
 * ****************************************************************************/
dlist_ty *DListCreate(void);

/*******************************************************************************
 * Destroys the dlist and frees the memory.
 * Arguments:
 * Notes: Undefined behavior if dlist is NULL.
 * Complexity: O(n) + system call complexity 
 * ****************************************************************************/
void DListDestroy(dlist_ty *dlist);

/*******************************************************************************
 * Inserts a new element before "iter".
 * Return Value: iterator to new element or DListEnd in case of failure 
 * Notes: undefined behaviour if dlist is NULL
 * Time Complexity: O(1)+  system call complexity 
 * ****************************************************************************/
dlist_iter_ty DListInsertBefore(dlist_ty *dlist, dlist_iter_ty iter, const void *data);

/*******************************************************************************
 * Removes element referred to by "to_remove"
 * Return Value: iterator to next element
 * Notes: undefined behaviour if "to_remove" is DListEnd/dummy node
 * Complexity: O(1)+  system call complexity 
 * ****************************************************************************/
dlist_iter_ty DListRemove(dlist_iter_ty to_remove);

/*******************************************************************************
 * Returns number of elements in dlist.
 * Notes: Undefined behavior if "dlist" is NULL.
 * Complexity: O(n)
 * ****************************************************************************/
size_t DListCount(const dlist_ty *dlist);

/*******************************************************************************
 * Checks if the dlist is empty.
 * Return Value: true (1) if the dlist is empty, false (0) otherwise.
 * Notes: Undefined behavior if dlist is NULL.
 * Complexity: O(n)
 * ****************************************************************************/
int DListIsEmpty(const dlist_ty *dlist);
/*******************************************************************************
 * Searches for a node within a certain range [from, to)
 * (to exclusive) that meets the requested criteria according to "is_match"
 * function.
 * Return Value: iterator to the first element which meets the criteria or "to" if element isn't found
 * Notes: undefined behaviour if "from" and "to" belong to different lists,
 * "to" comes before "from", or "is_match" is NULL.
 * Complexity: O(n)
 * ****************************************************************************/
dlist_iter_ty DListFind(dlist_iter_ty from, dlist_iter_ty to,
	dlist_is_match_ty is_match, void *param);

/*******************************************************************************
 * Executes the function op_func on each of the elements in the range from
 * "from" until "to" (excluding "to") or until first failure.
 * Return Value: 0 if "op_func" succeeded on all elements, otherwise 1
 * Notes: undefined behaviour if "from" and "to" belong to different lists,
 * "to" comes before "from", or "dlist_op_func" is NULL.
 * Complexity: O(n)
 * ****************************************************************************/
int DListForEach(dlist_iter_ty from, dlist_iter_ty to, dlist_op_ty op_func,
 void *param);

/*******************************************************************************
 * Extracts the list segment that starts on "src_from" and ends on "src_to" 
(excluding src_to) from its list and adds it to the "dest"'s list before "dest"
 * Notes: undefined behaviour if "src_from" and "src_to" belong to different
  lists, "src_to" comes before "src_from" or the function was implemented during
   the weekend.
 * Complexity: O(n)
 * ****************************************************************************/
void DListSplice(dlist_iter_ty dest, dlist_iter_ty src_from, dlist_iter_ty src_to);
/*******************************************************************************
 * Searches for nodes within a certain range [from, to)
 * (to exclusive) that meet the requested criteria according to "is_match"
 * function.
 * Return Value: the number of nodes found to meet the criteria of "is_match"
 * or -1 on failure.
 * Notes: undefined behaviour if "from" and "to" belong to different lists or to
 * "dlist_dest", "to" comes before "from", or "dlist_dest" or "is_match" is
 * NULL.
 * Complexity: O(n)
 * ****************************************************************************/
int DListMultiFind(dlist_ty *dlist_dest, dlist_iter_ty from, dlist_iter_ty to, 
	 dlist_is_match_ty is_match, void *param);

/*******************************************************************************
 * Inserts a new element at the end of the list.
 * Return Value: 0 on success, 1 on failure
 * Notes: undefined behaviour if dlist is NULL.
 * Complexity: O(1) + system call complexity
 * ****************************************************************************/
int DListPushBack(dlist_ty *dlist, const void *data);

/*******************************************************************************
 * Removes the last element from the list
 * Return Value:
 * Notes: undefined behaviour if dlist is NULL
 * Complexity: O(1)
 * ****************************************************************************/
void DListPopBack(dlist_ty *dlist);

/*******************************************************************************
 * Inserts a new element at the beginning of the list.
 * Return Value: 0 on success, 1 on failure
 * Notes: undefined behaviour if dlist is NULL.
 * Complexity: O(1) + system call complexity
 * ****************************************************************************/
int DListPushFront(dlist_ty *dlist, const void *data);

/*******************************************************************************
 * Removes the first element from the list
 * Return Value:
 * Notes: undefined behaviour if dlist is NULL
 * Complexity: O(1)
 * ****************************************************************************/
void DListPopFront(dlist_ty *dlist);


/*******************************************************************************
							Iterator functions
*******************************************************************************/

/*******************************************************************************
 * Returns pointer to data referred to by "iter"
 * Notes: undefined behaviour if "iter" is  DlistEnd
 * Complexity: O(1)
 * ****************************************************************************/
void *DListIterGetData(dlist_iter_ty iter);

/*******************************************************************************
 * Sets the data of "iter" to "data"
 * Notes: undefined behaviour if "iter" is DlistEnd
 * Complexity: O(1)
 * ****************************************************************************/
void DListIterSetData(dlist_iter_ty iter, const void *data);

/*******************************************************************************
 * Returns iterator to the first valid element
 * Notes: DListEnd/dummy if dlist is empty
 *        undefined behaviour if "dlist" is NULL
 * Complexity: O(1)
 * ****************************************************************************/
dlist_iter_ty DListIterBegin(const dlist_ty *dlist);

/*******************************************************************************
 * Returns iterator to the dummy element beyond the last node (invalid node)
 * Notes: undefined behaviour if "dlist" is NULL
 * Complexity: O(1)
 * ****************************************************************************/
dlist_iter_ty DListIterEnd(const dlist_ty *dlist);

/*******************************************************************************
 * Returns iterator to the element following "iter" 
 * Notes: undefined behaviour if "iter" is  DlistEnd
 * Complexity: O(1)
 * ****************************************************************************/
dlist_iter_ty DListIterNext(dlist_iter_ty iter);

/*******************************************************************************
 * Returns iterator to the element preceding "iter" 
 * Notes: undefined behaviour if "iter" is  DlistEnd
 * Complexity: O(1)
 * ****************************************************************************/
dlist_iter_ty DListIterPrev(dlist_iter_ty iter);

/*******************************************************************************
 * Returns true (0)/ false (1) representing "iter1" and "iter2" being the
 * same iterator
 * Complexity: O(1)
 * ****************************************************************************/
int DListIterIsEqual(dlist_iter_ty iter1, dlist_iter_ty iter2);

#endif /* __ILRD_DLIST_H__ */