/* Date: 11/7/2024, Author: Ron Yalensky, Reviewer: GIl Livneh*/

#include <stdlib.h>	/*	malloc, free*/
#include <assert.h> /* assert 		*/


#include "../include/dlist.h" /*	typedefs: dlist_ty, dlist_iter_ty, dlist_node_ty, size_t*/


#define _TRUE_ 		(1)
#define _FALSE_		(0)
#define _SUCCESS_ 	(0)
#define _FAILED_ 	(1)
#define DLIST_MULTI_FIND_FAIL (-1)



#define ITER_TO_NODE(iter) 	   (dlist_node_ty *)iter
#define NODE_TO_ITER(node_ptr) (dlist_iter_ty)node_ptr
#define NULL_ITER 			   NODE_TO_ITER(NULL)


struct dlist_node
{
	void *data;
	dlist_node_ty *next;
	dlist_node_ty *prev;
};

struct dlist
{
	dlist_node_ty head;
	dlist_node_ty tail;
};

/*******************************************************************************
							Utils Functions
*******************************************************************************/
static dlist_node_ty *DListCreateNodeAndSetPointers(const void *data, dlist_iter_ty next, dlist_iter_ty prev);
static void DListIterSetNextToPrev(dlist_iter_ty iter, dlist_iter_ty new_next);
static void DListIterSetPrevToNext(dlist_iter_ty iter, dlist_iter_ty new_prev);
static int IncreaseCount(void *data, void *count);



/*******************************************************************************
							DList Functions
*******************************************************************************/

/*******************************************************************************
 * Creates a doubly linked list.
 * Return Value: A pointer to the created dlist, or NULL if allocation fails.
 * Notes: Undefined behavior if capacity is 0.
 *        Must call DListDestroy at the end of use
 * Complexity: O(1) + system call complexity.
 * ****************************************************************************/
dlist_ty *DListCreate(void)
{
	dlist_ty *dlist = (dlist_ty *)malloc(sizeof(dlist_ty));
	
	if(NULL == dlist)
	{
		free(dlist);
		return(NULL);
	}
	dlist->tail.data = NULL;
	dlist->tail.next = NULL;
	dlist->tail.prev = &(dlist->head);

	dlist->head.data = NULL;
	dlist->head.next = &(dlist->tail);
	dlist->head.prev = NULL;
	
	return (dlist);

}

/*******************************************************************************
 * Destroys the dlist and frees the memory.
 * Arguments:
 * Notes: Undefined behavior if dlist is NULL.
 * Complexity: O(n) + system call complexity 
 * ****************************************************************************/
void DListDestroy(dlist_ty *dlist)
{
	dlist_node_ty *cur_node = NULL;
	dlist_node_ty *next_node = NULL;
	if(NULL == dlist)
	{
		return;
	}

	cur_node = dlist->head.next; /* init node to point the head node*/

	while(NULL != cur_node->next)
	{
		next_node = cur_node->next;
		free(cur_node);
		cur_node = next_node;
	}

	free(dlist);
	dlist = NULL;


}

/*******************************************************************************
 * Inserts a new element before "iter".
 * Return Value: iterator to new element or DListEnd in case of failure 
 * Notes: undefined behaviour if dlist is NULL
 * Time Complexity: O(1)+  system call complexity 
 * ****************************************************************************/
dlist_iter_ty DListInsertBefore(dlist_ty *dlist, dlist_iter_ty iter,
	const void *data)
{
	dlist_node_ty * new_node = NULL;


    assert(NULL != dlist);
   	assert(_FALSE_ == DListIterIsEqual(iter, NULL_ITER));

	new_node = DListCreateNodeAndSetPointers(data, DListIterPrev(iter),iter);/* Allocate memory for the new node and set its data and pointers*/

	if(NULL == new_node)
		{
			return (DListIterEnd(dlist));
		}

	/*	update the iter prev and iter next to point on the new node*/
	DListIterSetNextToPrev(DListIterPrev(iter), NODE_TO_ITER(new_node));
	DListIterSetPrevToNext(iter, NODE_TO_ITER(new_node));



	return (NODE_TO_ITER(new_node));

}

/*******************************************************************************
 * Removes element referred to by "to_remove"
 * Return Value: iterator to next element
 * Notes: undefined behaviour if "to_remove" is DListEnd/dummy node
 * Complexity: O(1)+  system call complexity 
 * ****************************************************************************/
dlist_iter_ty DListRemove(dlist_iter_ty to_remove)
{
	dlist_iter_ty next = NULL_ITER;

    assert(!DListIterIsEqual(to_remove, NULL_ITER));
    next = DListIterNext(to_remove);

	/*	update the iter prev and iter next to point on each other -> excluding the removed node*/
	DListIterSetNextToPrev(DListIterPrev(to_remove),DListIterNext(to_remove));
	DListIterSetPrevToNext(DListIterNext(to_remove),DListIterPrev(to_remove));

	free(ITER_TO_NODE(to_remove));
    to_remove = NULL_ITER;

    return(next);



}

/*******************************************************************************
 * Returns number of elements in dlist.
 * Notes: Undefined behavior if "dlist" is NULL.
 * Complexity: O(n)
 * ****************************************************************************/
size_t DListCount(const dlist_ty *dlist)
{

	size_t count = 0;
	dlist_iter_ty begin = NULL_ITER;
	dlist_iter_ty end = NULL_ITER;

	assert(NULL != dlist);
	
	begin = DListIterBegin(dlist);
	end = DListIterEnd(dlist);
	DListForEach(begin, end, IncreaseCount, &count);

	return (count);


}

/*******************************************************************************
 * Checks if the dlist is empty.
 * Return Value: true (1) if the dlist is empty, false (0) otherwise.
 * Notes: Undefined behavior if dlist is NULL.
 * Complexity: O(n)
 * ****************************************************************************/
int DListIsEmpty(const dlist_ty *dlist)
{
	assert(NULL != dlist);
    return (DListIterIsEqual(DListIterBegin(dlist), DListIterEnd(dlist)));
}

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
	dlist_is_match_ty is_match, void *param)
{
	assert(!DListIterIsEqual( NULL_ITER, from));
	assert(!DListIterIsEqual( NULL_ITER, to));
	assert(NULL != is_match);

	while(_FALSE_ == DListIterIsEqual(from,to))
	{
		if (_TRUE_ == is_match(DListIterGetData(from), param))
		{
			return(from);
		}
		from = DListIterNext(from);
	}

	return(to);
}

/*******************************************************************************
 * Executes the function op_func on each of the elements in the range from
 * "from" until "to" (excluding "to") or until first failure.
 * Return Value: 0 if "op_func" succeeded on all elements, otherwise 1
 * Notes: undefined behaviour if "from" and "to" belong to different lists,
 * "to" comes before "from", or "dlist_op_func" is NULL.
 * Complexity: O(n)
 * ****************************************************************************/
int DListForEach(dlist_iter_ty from, dlist_iter_ty to, dlist_op_ty op_func,
 void *param)
{
	assert(NULL_ITER != from);
	assert(NULL_ITER != to);
	assert(NULL != op_func);

	while(_FALSE_ == DListIterIsEqual(from, to))
	{
		if(_FAILED_ == op_func(DListIterGetData(from), param))
		{
			return (_FAILED_);
		}
		from = DListIterNext(from);
	}

	return (_SUCCESS_);

}

/*******************************************************************************
 * Extracts the list segment that starts on "src_from" and ends on "src_to" 
(excluding src_to) from its list and adds it to the "dest"'s list before "dest"
 * Notes: undefined behaviour if "src_from" and "src_to" belong to different
  lists, "src_to" comes before "src_from" or the function was implemented during
   the weekend.
 * Complexity: O(n)
 * ****************************************************************************/
void DListSplice(dlist_iter_ty dest, dlist_iter_ty src_from, dlist_iter_ty src_to)
{


    dlist_node_ty *dest_node = NULL;
    dlist_node_ty *dest_prev_node = NULL;
    dlist_node_ty *from_node = NULL;
    dlist_node_ty *from_prev_node = NULL;
    dlist_node_ty *exclusive_to_node = NULL;
    dlist_node_ty *inclusive_to_node = NULL;


    assert(_FALSE_ == DListIterIsEqual(NULL_ITER, dest));
    assert(_FALSE_ == DListIterIsEqual(NULL_ITER, src_from));
    assert(_FALSE_ == DListIterIsEqual(NULL_ITER, src_to));

    dest_node = ITER_TO_NODE(dest);
    dest_prev_node = dest->prev;

    from_node = ITER_TO_NODE(src_from);
    from_prev_node = from_node->prev;

    exclusive_to_node = ITER_TO_NODE(src_to);
    inclusive_to_node = exclusive_to_node->prev;
  

    /* Extracting segment from source list */
    from_prev_node->next = exclusive_to_node;
    exclusive_to_node->prev = from_prev_node;


    /* Adding segment into destination list: */
    /* Connecting the left edge */
    dest_prev_node->next = from_node;
    from_node->prev = dest_prev_node;


    /* Connecting the right edge */
    inclusive_to_node->next = dest_node;
    dest_node->prev = inclusive_to_node;



}

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
	 dlist_is_match_ty is_match, void *param)
{

    void *data = NULL;
    int count = 0;

    assert(NULL != dlist_dest);
    assert(_FALSE_ == DListIterIsEqual(NULL_ITER, from));
    assert(_FALSE_ == DListIterIsEqual(NULL_ITER, to));
    assert(NULL != is_match);

    while (_FALSE_ == DListIterIsEqual(from, to))
    {
        data = DListIterGetData(from);

        if (_TRUE_ == is_match(data, param))
        {
            if (_FAILED_ == DListPushBack(dlist_dest, data))
            {
                return (DLIST_MULTI_FIND_FAIL);
            }

            ++count;
        }

        from = DListIterNext(from);
    }

    return (count);

}

/*******************************************************************************
 * Inserts a new element at the end of the list.
 * Return Value: 0 on success, 1 on failure
 * Notes: undefined behaviour if dlist is NULL.
 * Complexity: O(1) + system call complexity
 * ****************************************************************************/
int DListPushBack(dlist_ty *dlist, const void *data)
{
	
	 assert(dlist);

    if (DListIterIsEqual(DListIterEnd(dlist), DListInsertBefore(dlist, DListIterEnd(dlist), data)))
    {
        return (_FAILED_);
    }
    
    return (_SUCCESS_);

}

/*******************************************************************************
 * Removes the last element from the list
 * Return Value:
 * Notes: undefined behaviour if dlist is NULL
 * Complexity: O(1)
 * ****************************************************************************/
void DListPopBack(dlist_ty *dlist)
{
	(assert(dlist),DListRemove(DListIterPrev(DListIterEnd(dlist))));

}

/*******************************************************************************
 * Inserts a new element at the beginning of the list.
 * Return Value: 0 on success, 1 on failure
 * Notes: undefined behaviour if dlist is NULL.
 * Complexity: O(1) + system call complexity
 * ****************************************************************************/
int DListPushFront(dlist_ty *dlist, const void *data)
{
	 assert(dlist);

    if (DListIterIsEqual(DListIterBegin(dlist), DListInsertBefore(dlist, DListIterBegin(dlist), data)))
    {
        return (_SUCCESS_);
    }
    
    return (_FAILED_);
}


/*******************************************************************************
 * Removes the first element from the list
 * Return Value:
 * Notes: undefined behaviour if dlist is NULL
 * Complexity: O(1)
 * ****************************************************************************/
void DListPopFront(dlist_ty *dlist)
{
	(assert(dlist),DListRemove(DListIterBegin(dlist)));

}


/*******************************************************************************
							Iterator functions
*******************************************************************************/

/*******************************************************************************
 * Returns pointer to data referred to by "iter"
 * Notes: undefined behaviour if "iter" is  DlistEnd
 * Complexity: O(1)
 * ****************************************************************************/
void *DListIterGetData(dlist_iter_ty iter)
{
	assert(NULL_ITER != iter);
	return (ITER_TO_NODE(iter)->data);
}

/*******************************************************************************
 * Sets the data of "iter" to "data"
 * Notes: undefined behaviour if "iter" is DlistEnd
 * Complexity: O(1)
 * ****************************************************************************/
void DListIterSetData(dlist_iter_ty iter, const void *data)
{
	assert(NULL_ITER != iter);

	(ITER_TO_NODE(iter))->data = (void *)data;
}

/*******************************************************************************
 * Returns iterator to the first valid element
 * Notes: DListEnd/dummy if dlist is empty
 *        undefined behaviour if "dlist" is NULL
 * Complexity: O(1)
 * ****************************************************************************/
dlist_iter_ty DListIterBegin(const dlist_ty *dlist)
{
	assert(NULL != dlist);
	return(NODE_TO_ITER(dlist->head).next);
}

/*******************************************************************************
 * Returns iterator to the dummy element beyond the last node (invalid node)
 * Notes: undefined behaviour if "dlist" is NULL
 * Complexity: O(1)
 * ****************************************************************************/
dlist_iter_ty DListIterEnd(const dlist_ty *dlist)
{
	assert(NULL != dlist);

	return(NODE_TO_ITER(&(dlist->tail)));
}

/*******************************************************************************
 * Returns iterator to the element following "iter" 
 * Notes: undefined behaviour if "iter" is  DlistEnd
 * Complexity: O(1)
 * ****************************************************************************/
dlist_iter_ty DListIterNext(dlist_iter_ty iter)
{

	assert(NULL_ITER != iter);
	return (ITER_TO_NODE(iter)->next);

}

/*******************************************************************************
 * Returns iterator to the element preceding "iter" 
 * Notes: undefined behaviour if "iter" is  DlistEnd
 * Complexity: O(1)
 * ****************************************************************************/
dlist_iter_ty DListIterPrev(dlist_iter_ty iter)
{
	assert(NULL_ITER != iter);
	return (ITER_TO_NODE(iter)->prev);

}

/*******************************************************************************
 * Returns true (0)/ false (1) representing "iter1" and "iter2" being the
 * same iterator
 * Complexity: O(1)
 * ****************************************************************************/
int DListIterIsEqual(dlist_iter_ty iter1, dlist_iter_ty iter2)

{
	    return (ITER_TO_NODE(iter1) == ITER_TO_NODE(iter2));

}

/*******************************************************************************
							Utils functions
*******************************************************************************/

/*******************************************************************************
 * Returns new node with the data assignes and the next and prev updated.
 * Complexity: O(1)
 * ****************************************************************************/

static dlist_node_ty *DListCreateNodeAndSetPointers(const void *data, dlist_iter_ty prev, dlist_iter_ty next)
{
	 dlist_node_ty *new_node = ( dlist_node_ty *)malloc(sizeof(dlist_node_ty));

	if(NULL == new_node)
	{
		return (NULL);
	}

	new_node->data = (void *)data;
	new_node->prev = ITER_TO_NODE(prev);
	new_node->next = ITER_TO_NODE(next);
	return(NODE_TO_ITER(new_node));

}



static void DListIterSetNextToPrev(dlist_iter_ty iter, dlist_iter_ty new_next)
{
	dlist_iter_ty node = ITER_TO_NODE(iter);
	node->next = ITER_TO_NODE(new_next);

}

static void DListIterSetPrevToNext(dlist_iter_ty iter, dlist_iter_ty new_prev)
{
	dlist_iter_ty node = ITER_TO_NODE(iter);
	node->prev = ITER_TO_NODE(new_prev);

}

static int IncreaseCount(void *data, void *count)
{
	(void)data;
	++(*(size_t*)count);
	
	return (_SUCCESS_);
}

