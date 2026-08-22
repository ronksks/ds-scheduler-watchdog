/* META DATA
Date: 02/09/2024
Author: Ron Yalensky
Reviewer:
Description: Source file for Heap project. */

/* Header Files Used */

#include "utils.h"/* BAD_MEM, STATUS */
#include "heap.h"
#include "dvector.h"
#include <assert.h>/* assert */

enum {DEFAULT_CAPACITY = 100};


static void HeapifyUp(heap_ty* hp_, size_t index_);
static void HeapifyDown(heap_ty* hp_, size_t index_);
static size_t GetElementValueIMP(heap_ty* hp_, size_t index_);
static void** ElemArr(heap_ty* hp_);
static void SwapDataIMP(void** ptr1_, void** ptr2_);
static size_t BestOfThree(heap_ty* hp_, size_t index_);

struct heap
{
    dvector_ty* vec;
  
    cmp_f_ty sort_func;
    const void* param; 
};

/* Heap API Implementation */

heap_ty* HPQCreate(cmp_f_ty sort_func_, const void* prm_, size_t initial_capacity_)
{
    size_t capacity = initial_capacity_ + !initial_capacity_* DEFAULT_CAPACITY; /* if user did not provide size, we set the default */
    
    /* allocate heap*/
    heap_ty* heap = (heap_ty*)malloc(sizeof(heap_ty));
    
    /* assert */
    assert(NULL != sort_func_);
    
    /* check allocation success */
    if(!heap)
    {
        return NULL;
    }

    /* allocate vector sizeof initial_capacity */
    heap->vec = DVectorCreate(capacity, sizeof(void*));
    
    /* check allocation success */
    if(!heap->vec)
    {
        free(heap);
        DEBUG_ONLY(heap = DEAD_MEM(heap_ty*));
        return NULL;
    }

    /* intilize parameters with arguments */
    heap->sort_func = sort_func_;
    heap->param = prm_;

    /* return heap */
    return heap;
}

void HPQDestroy(heap_ty* hp_)
{
    /* if (NULL == hp_) return */
    if (NULL == hp_)
    {
        return;
    } 
    
    /* call DVectorDestroy */
    DVectorDestroy(hp_->vec);
    
    DEBUG_ONLY
    (

        hp_->vec = DEAD_MEM(dvector_ty*);
        hp_->sort_func = DEAD_MEM(cmp_f_ty);
        hp_->param = DEAD_MEM(void*);

    )

    /* free(hp_) */
    free(hp_);
}

int HPQEnqueue(heap_ty* hp_, void* element_)
{
    /* assert */
    assert(hp_);
    
    /* call  DVectorPushBack with element*/
    
    /* if failed to push */
        /* return FAILED */
    if(SUCCESS != DVectorPushBack(hp_->vec, element_))
    {
        return FAILURE;
    }

    HeapifyUp(hp_, HPQSize(hp_));
    
    /* return success */
    return SUCCESS;
}

void HPQDequeue(heap_ty* hp_)
{
    /* assert */

    /* get ElemArr */
    void** array = (assert(hp_), ElemArr(hp_));

    assert(!HPQIsEmpty(hp_));

    /* copy the last valid element to the beginning of the heap */
    array[1] = array[HPQSize(hp_)];

    /* remove from the heap */
    DVectorPopBack(hp_->vec);

        /* if there's still elements s*/
    if(!HPQIsEmpty(hp_))
    {
        /* fix the heap if have to */
        HeapifyDown(hp_, 1);
    }
}

void* HPQPeek(heap_ty* hp_)
{
    assert(hp_);

    /* return the begininng of the heap */
    return ElemArr(hp_)[1];


}

size_t HPQSize(const heap_ty* hp_)
{
    /* assert */
    assert(hp_);

    /* return DVectorGetSize */
    return(DVectorGetSize(hp_->vec));
}

int HPQIsEmpty(const heap_ty* hp_)
{
    /* assert */
    assert(hp_);
    /* return(0 == DVectorGetSize) */
    return(0 == DVectorGetSize(hp_->vec));
}

void* HPQRemove(heap_ty* hp_, is_match_ty match_func_, const void* match_param_)
{
    size_t curr = 0;
    size_t size = 0;
    void* ret = NULL;
    void **array = ElemArr(hp_);
    int status;

    /* assert */
    assert(NULL != hp_);
    assert(NULL != match_func_);

    size = HPQSize(hp_);


    /* foreach element in vector */
    for (curr = 1; curr <= size; ++curr )
    {
        /* if match_func(curr) */
        status = match_func_(array[curr], match_param_);
        if (status)
        {
            /* break */
            break;
        }
        
    }

    /* if not found */
    if (!status)
    {
        /* return */
        return;
    }
    
    /* save found position */
    /* ret = curr */
    ret = array[curr];
    
    /* copy last element to the curr element position */
    array[curr] = array[size];

    /* call DVectorPopBack */
    DVectorPopBack(hp_->vec);

    /* call HeapifyUp */
    HeapifyUp(hp_, curr);

    /* call HeapifyDown */
    HeapifyDown(hp_, curr);

    /* return ret */
    return ret;

}

static size_t GetElementValueIMP(heap_ty* hp_, size_t index_)
{
    void** ptr = ElemArr(hp_);
    size_t num = *(size_t*)(ptr + index_);
    return num;
}


static void HeapifyUp(heap_ty* hp_, size_t index_)
{
    
    size_t curr = index_;
    size_t parent = curr / 2;
    void** array = (assert(NULL != hp_), ElemArr(hp_)); 

    /* assert that index is in bounds. */
    assert(HPQSize(hp_) >= index_);

    /* if curr value is grated than parent value */
    if ((1 == index_) || (0 >= hp_->sort_func(array[curr], array[parent], hp_->param)))
    {
        return;
    }

    /* swap curr with its parent */
    SwapDataIMP(array + curr, array + parent);
        
    HeapifyUp(hp_, parent);

}

static void HeapifyDown(heap_ty* hp_, size_t index_)
{
    /* assert */
    void** array = ElemArr(hp_);
    size_t swap_idx = BestOfThree(hp_, index_);

    /* if there is no left child */
	if (index_ == swap_idx)
	{
		return;
	}
	
    SwapDataIMP(array + index_, array + swap_idx);

	HeapifyDown(hp_, swap_idx);

}

static void** ElemArr(heap_ty* hp_)
{
    return (void**)DVectorGetAccessToElem(hp_->vec, 0) -1;
}



static void SwapDataIMP(void** ptr1_, void** ptr2_)
{
	void* temp = *ptr1_;

	*ptr1_ = *ptr2_;
	*ptr2_ = temp;
}



static size_t BestOfThree(heap_ty* hp_, size_t index_)
{
    void** arr = ElemArr(hp_);
    size_t ret = index_;
    size_t left = 2 * index_;
    size_t right = 2 * index_ + 1;

    /* if right child exist and his value should be before the ret's value. */
    if(right <= HPQSize(hp_) && (0 <= hp_->sort_func(arr[right], arr[index_], hp_->param)))
    {
        ret = right;
    }
    /* if left child exist and his value should be before the ret's value. */
    if(left <= HPQSize(hp_) && (0 <= hp_->sort_func(arr[left], arr[index_], hp_->param)))
    {
        ret = left;
    }


    return ret;
}
