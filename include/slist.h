/* Date: 3/7/2024, Author: Ron Yalensky, Reviewer: Daniel the mighty ehrenberg*/
#ifndef __ILRD_SLIST_H__
#define __ILRD_SLIST_H__

#include <stddef.h> /* size_t */

typedef struct slist_node slist_node_ty;

typedef struct slist slist_ty;

/*Do not touch, can change in the future.*/ 
typedef slist_node_ty *slist_iter_ty;


/****************************************************************************
 * Returns 1 (true) or 0 ( false) if data matches requested condition.
   user_param is optional 
 ****************************************************************************/
typedef int (*is_match_ty) (const void *data, const void *user_param);

/****************************************************************************
 * Perform requested operation on "data", returns 0 for success or 1 for
   failure
 ***************************************************************************/
typedef int (*op_ty) (void *data, const void *user_param);

/****************************************************************************
 * Creates an empty Singly Linked List.
 * Return Values:  pointer to the new list or NULL in case of failure
 * Notes: must call SListDestroy at the end of use
 * Time Complexity: Determined by system call complexity
 * *************************************************************************/
slist_ty *SListCreate(void);     

/****************************************************************************
 * Destroys the slist list and frees the memory
 * Notes: if slist is NULL - nothing occurs
 * Time Complexity: O(n)+  system call complexity 
 * *************************************************************************/
void SListDestroy(slist_ty *slist);

/****************************************************************************
 * Inserts a new element before "iter"
 * Return Value: iterator to new element or SListEnd in case of failure 
 * Notes: 
 * Time Complexity: O(1)+  system call complexity 
 * *************************************************************************/
slist_iter_ty SListInsertBefore(slist_ty *slist, slist_iter_ty iter, const void* data);  

/****************************************************************************
 * Removes element referred to by "to_remove"
 * Return Value: iterator to next element
 * Notes: undefined behaviour if "to_remove" is SListEnd/dummy node
 * Complexity: O(1)+  system call complexity 
 * *************************************************************************/
slist_iter_ty SListRemove(slist_iter_ty to_remove);  

/****************************************************************************
 * Returns number of elements in Singly Linked List.
 * Notes: Undefined behavior if "slist" is NULL.
 * Complexity: O(n)
 * *************************************************************************/
size_t SListCount(const slist_ty *slist); 

/****************************************************************************
 * Returns 1 if the list is empty, 0 otherwise
 * Notes: undefined behavior if slist is empty.       
 * Complexity: O(n)
 * *************************************************************************/
int SListIsEmpty(const slist_ty *slist); 

/****************************************************************************
 * Returns iterator to the first valid element
 * Notes: SListEnd/dummy if slist is empty
 *        undefined behaviour if "slist" is NULL
 * Complexity: O(1)
 * *************************************************************************/
slist_iter_ty SListIterBegin(const slist_ty *slist);

/****************************************************************************
 * Returns iterator to the dummy element beyond the last node (unvalid node)
 * Notes:  undefined behaviour if "slist" is NULL
 * Complexity: O(1)
 * *************************************************************************/
slist_iter_ty SListIterEnd(const slist_ty *slist);

/****************************************************************************
 * Returns iterator to the element following "iter" 
 * Notes:  undefined behaviour if "iter" is  SlistEnd
 * Complexity: O(1)
 * ************************************************************************/
slist_iter_ty SListIterNext(slist_iter_ty iter);

/****************************************************************************
 * Returns true (0)/ false (1) representing "iter1" and "iter2" being the
   same iterator 
 * Complexity: O(1)
 * ************************************************************************/
int SListIterIsEqual(slist_iter_ty iter1, slist_iter_ty iter2);

/****************************************************************************
 * Returns pointer to data referred to by "iter"
 * Notes:  undefined behaviour if "iter" is  SlistEnd
 * Complexity: O(1)
 * ************************************************************************/
void *SListIterGetData(slist_iter_ty iter);              

/****************************************************************************
 * Sets the data of "iter" to "data"
 * Notes:  undefined behaviour if "iter" is  SlistEnd
 * Complexity: O(1)
 * ************************************************************************/
void SListIterSetData(slist_iter_ty iter, void *data);                                      

/****************************************************************************
 * Searches for a node within a certain range [from, to)
 * (to exclusive) that meets the requested criteria according to "is_match" function.
 * Return Value: iterator to the found element or SlistEnd if element isn't found
 * Notes:  undefined behaviour if "from" and "to" belong to different lists,
   "to" comes before "from", "is_match" is NULL or **Oded is implementing the
   function**
 * Complexity: O(n)
 * ************************************************************************/
slist_iter_ty SListFind(slist_iter_ty from, const slist_iter_ty to,
	is_match_ty is_match, const void *param);


/****************************************************************************
 * Executes the function op_func on each of the elements in the range from
   "from" until to "to" (excluding "to") or until first failure.
 * Return Value: 0 if "op_func" succeeded on all elements, otherwise 1
 * Notes:  undefined behaviour if "from" and "to" belong to different lists,
   "to" comes before "from", "op_func" is NULL or Oded is implementing the
   function
 * Complexity: O(n)
 * ************************************************************************/
int SListForEach(slist_iter_ty from, slist_iter_ty to, op_ty op_func, void *param);


/****************************************************************************
 * Appends the source list to the destination list.
 * Parameters:
 * Return Value: Pointer to the destination queue.
 * Notes: Undefined behavior if either pointer is NULL. The source list will be empty after the operation.
 * Complexity: O(1) + system call complexity
 ****************************************************************************/
void SListAppend(slist_ty *dest, slist_ty *src);




#endif /*__ILRD_SLIST_H__*/
