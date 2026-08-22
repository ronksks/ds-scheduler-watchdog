/* Date: 17/7/2024, Author: Ron Yalensky, Reviewer: Daniel (The magic mike) Ehrenberg */

#include <assert.h>	/*	assert				*/
#include <stdlib.h>	/*	malloc, free, size_t*/


#include "pqueue.h" /*	typedefs: p_queue_ty */
#include "sorted_list.h" /*	typedefs: sort_list_ty, sort_list_iter_ty */

#define _TRUE_ 		(1)
#define _FALSE_		(0)
#define _SUCCESS_ 	(0)
#define _FAILED_ 	(1)



/****************************************************************************
 * Structures Definitions
 * ************************************************************************/

struct p_queue
{
	sort_list_ty *sort_list;
};




/****************************************************************************
 * Static utility functions declarations
 * ************************************************************************/


/****************************************************************************
 * Api Functions
 * ************************************************************************/
p_queue_ty *PQCreate(priority_func_ty priority_func)
{
	p_queue_ty *pq = (p_queue_ty *)malloc(sizeof(p_queue_ty));
	
	if(NULL == pq)
	{
		return (NULL);
	}
	
	pq->sort_list = SortListCreate(priority_func);
	
	if(NULL == pq->sort_list)
	{
		free(pq);
		
		pq = NULL;

		return(NULL);
	}

	return(pq);
}

void PQDestroy(p_queue_ty *pq)
{
	if(NULL == pq)
	{
		return;
	}
	
	SortListDestroy(pq->sort_list);
	
	pq->sort_list = NULL;
	
	free(pq);
	
	pq = NULL;
}


int PQEnqueue(p_queue_ty *pq, const void *data)
{
	assert(NULL != pq);

	if (_TRUE_ == SortListIterIsEqual(SortListIterEnd(pq->sort_list), SortListInsert(pq->sort_list, data)))
	{
	
		return(_FAILED_);
	
	}

	return(_SUCCESS_);
}


void *PQDequeue(p_queue_ty *pq)
{
		assert(NULL != pq);
		assert(NULL != PQPeek(pq));

		return(SortListPopFront(pq->sort_list));
}


void *PQPeek(const p_queue_ty *pq)
{
	assert(NULL != pq);
	assert(_TRUE_ != PQIsEmpty(pq));

	return(SortListIterGetData(SortListIterBegin(pq->sort_list)));
}

int PQIsEmpty(const p_queue_ty *pq)
{
	assert(NULL != pq);
	
	return(SortListIsEmpty(pq->sort_list));
}


void PQClear(p_queue_ty *pq)
{
	assert(NULL != pq);

	while(!PQIsEmpty(pq))
	{
		PQDequeue(pq);
	}
}


size_t PQCount(const p_queue_ty *pq)
{
	assert(NULL != pq);

	return(SortListCount(pq->sort_list));
}


void *PQErase(p_queue_ty *pq, is_match_func_ty match_func, void *match_func_param)
{
		sort_list_iter_ty found;
		
		void *data_found = NULL;

		assert(NULL != pq);
		assert(NULL != match_func);

		found = SortListFindIf(SortListIterBegin(pq->sort_list), 
				SortListIterEnd(pq->sort_list),
				match_func, match_func_param);

		if(SortListIterIsEqual(found, SortListIterEnd(pq->sort_list)))
		{
			
			return(NULL);
		
		}
		data_found = SortListIterGetData(found);

		SortListRemove(found);

		return(data_found);
}


