/* Date: 3/7/2024, Author: Ron Yalensky, Reviewer: Daniel the mighty ehrenberg*/

#include <stdlib.h> /*	malloc, free */
#include <assert.h> /*	assert */
#include "../include/slist.h" /*	typedefs: slist_node_ty, slist, op_ty, is_match_ty*/

/*	Status Macros		*/
#define __ILRD_FAIL     (1)
#define __ILRD_SUCCESS  (0)
#define __ILRD_TRUE     (1)
#define __ILRD_FALSE    (0)

/*	Casting Macros		*/




/* Static utility functions declarations */

/****************************************************************************
 * Sets the "next" field of the node pointed to by "iter" to "next"
 * Notes:  undefined behaviour if "iter" is  SlistEnd
 * Complexity: O(1)
 * ************************************************************************/
static void SListIterSetNext(slist_iter_ty iter, const slist_node_ty *next);

/****************************************************************************
 * Copies the contents of the node pointed to by "src_iter" to the one pointed
 * to by "dest_iter"
 * Notes:  undefined behaviour if either iterator is  SlistEnd
 * Complexity: O(1)
 * ************************************************************************/
static void SListIterCopy(slist_iter_ty dest_iter,
							const slist_iter_ty src_iter);

/****************************************************************************
 * Increments the value of "param" (type cast to size_t) by 1 
 * Complexity: O(1)
 * ************************************************************************/

static int OpIncrementParam(void *data, const void *param);


/****************************************************************************
 * Structures Definitions
 * ************************************************************************/

struct slist_node {
    void *data;
     slist_node_ty *next;
};


struct slist {
    slist_node_ty *head;      
    slist_node_ty *tail;                      
};              

/****************************************************************************
 * Api Functions
 * ************************************************************************/

slist_ty *SListCreate(void)
{
	slist_node_ty *dummy = (slist_node_ty *)malloc(sizeof(slist_node_ty));
	slist_ty *slist = (slist_ty *)malloc(sizeof(slist_ty));
	
	if( NULL == dummy || NULL == slist)
	{
		free(slist);
		free(dummy);
		return(NULL);
	}
	slist->head = dummy;
	slist->tail = dummy;

	dummy->data = slist;
	dummy->next = NULL;
	return (slist);
}  

void SListDestroy(slist_ty* slist)
{
	slist_node_ty *cur_node = NULL;
	slist_node_ty *next_node = NULL;
	if(NULL == slist)
	{
		return;
	}

	cur_node = slist->head; /* init node to point the head node*/

	while(NULL != cur_node->next)
	{
		next_node = cur_node->next;
		free(cur_node);
		cur_node = next_node;
	}
	free(cur_node);
	cur_node = NULL;
	free(slist);
	slist = NULL;
}


slist_iter_ty SListInsertBefore(slist_ty *slist, slist_iter_ty iter, const void* data)
{
    /* Allocate memory for the new node */
    slist_node_ty *new_node = (slist_node_ty *)malloc(sizeof(slist_node_ty));

    assert(NULL != slist);
    assert(NULL != iter);

    if (!new_node) 
    {
        return SListIterEnd(slist);
    }

    new_node->data = iter->data;
    new_node->next = iter->next;

    iter->data = (void *)data;
    iter->next = new_node;

    if(NULL == new_node->next)
    {
        slist->tail = new_node;
    }

    return (iter);
}


slist_iter_ty SListRemove(slist_iter_ty to_remove)
{
	slist_iter_ty temp_iter = NULL;

		assert(NULL != to_remove); /* to_remove will be dereferenced */
		assert(NULL != to_remove->next); /* to_remove->next will be dereferenced */

		temp_iter = SListIterNext(to_remove);

		SListIterCopy(to_remove, temp_iter);

		free(temp_iter);
		temp_iter = NULL;

		if (NULL == SListIterNext(to_remove))
		{
			((slist_ty *)SListIterGetData(to_remove))->tail = to_remove;
		}

	return(to_remove);

} 


size_t SListCount(const slist_ty *slist)
{
	size_t count = 0;

	assert(NULL != slist); /* for dereference */

	SListForEach(SListIterBegin(slist), SListIterEnd(slist),
		OpIncrementParam, &count);

	return (count);
}

int SListIsEmpty(const slist_ty *slist)
{
	assert(NULL != slist);
	if(NULL == slist->head->next)
	{
		return (__ILRD_TRUE);
	}
	return(__ILRD_FAIL);


}

slist_iter_ty SListIterBegin(const slist_ty *slist)
{
	assert(NULL != slist);
	return(slist->head);
}

slist_iter_ty SListIterEnd(const slist_ty *slist)
{
	assert(NULL != slist);
	return(slist->tail);
}

slist_iter_ty SListIterNext(slist_iter_ty iter)
{
	assert(NULL != iter);
	return(iter->next);
}

int SListIterIsEqual(slist_iter_ty iter1, slist_iter_ty iter2)
{
		return(iter1 == iter2);
}

void* SListIterGetData(slist_iter_ty iter)
{
	assert(NULL != iter);
	return(iter->data);
}              

void SListIterSetData(slist_iter_ty iter, void *data)
{
	assert(NULL != iter);
	iter->data = data;
}                                      


slist_iter_ty SListFind(slist_iter_ty from, const slist_iter_ty to, is_match_ty is_match, const void *param)
{
	assert(NULL != from);
	assert(NULL != to);
	assert(NULL != is_match);
	assert(NULL != from);
    while (!SListIterIsEqual(from, to) && !is_match(SListIterGetData(from), param))
    {
        from = SListIterNext(from);
    }

    if (SListIterIsEqual(from, to))
    {
        while (NULL != SListIterNext(from))
        {
            from = SListIterNext(from);
        }

        return (from);  
    }
    else
    {
        return (from);
    }
}


int SListForEach(slist_iter_ty from, slist_iter_ty to, op_ty op_func,
    void *param)
{
    void *data = NULL;
   	assert(NULL != from);
   	assert(NULL != to);
   	assert(NULL != op_func);



    while (!SListIterIsEqual(from, to))
    {
        data = SListIterGetData(from);

        if (1 == op_func(data, param))
        {
            return (__ILRD_FAIL);
        }

        from = SListIterNext(from);
    }

    return (__ILRD_SUCCESS);
}

void SListAppend(slist_ty *dest, slist_ty *src)
{
	assert(NULL != dest); 
	assert(NULL != src); 


	SListIterCopy(SListIterEnd(dest),SListIterBegin(src));  /*	copy src data and pointer to dest dummy*/
	dest->tail = SListIterEnd(src);							/*	update new dummy for dest 			   */
	SListIterSetData(SListIterEnd(dest), (void *)dest); 	/*	dest dummy data -> dest list 		   */
	SListIterSetData(SListIterBegin(src), (void *)src); 	/*	src dummy data -> src list             */
	SListIterSetNext(SListIterBegin(src), NULL);			/*	src dummy->next is NULL				   */
	src->tail = SListIterBegin(src); 						/*	set new dummy for src 				   */
}


/* Static utility functions definitions */
static void SListIterSetNext(slist_iter_ty iter, const slist_node_ty *next)
{
	assert(NULL != iter); /* dereference */

	iter->next = (slist_node_ty *) next;
}

static void SListIterCopy(slist_iter_ty dest_iter,
							const slist_iter_ty src_iter)
{
	assert(NULL != dest_iter); /* dereference */
	assert(NULL != src_iter); /* dereference */

	SListIterSetData(dest_iter, SListIterGetData(src_iter));
	SListIterSetNext(dest_iter, SListIterNext(src_iter));
}


static int OpIncrementParam(void *data, const void *param)
{
	assert(NULL != param); /* for dereference */

	*(size_t *)param += 1;
	(void) data;

	return (__ILRD_SUCCESS);
}
