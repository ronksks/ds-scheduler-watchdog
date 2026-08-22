/* META DATA
Date: 02/09/2024
Author: Ron Yalensky
Reviewer:
Description: Header file for Heap project. */

#ifndef _ILRD_HEAP_H_
#define _ILRD_HEAP_H_

#include <stddef.h> /*size_t*/

typedef struct heap heap_ty;

/* Description: is_same function used to search an element to remove.
 * Note: User may use "param_" argument which is optional. */
typedef int (*is_match_ty)(const void* data_, const void* param_);

/* Return positive, zero or negative if d2_ has lower, equal or higher priority than d1_.
    Note: param_ is optional and can be NULL if allowed by the function. */
typedef int (*cmp_f_ty)(const void* d2_, const void* d1_, const void* param_);

/* User should provide initial capacity for better preformance, if not, default capacity would be set. */
heap_ty* HPQCreate(cmp_f_ty cmp_f_, const void* prm_, size_t initial_capacity_);

/*if hp_ is NULL nothing happens*/
void HPQDestroy(heap_ty* hp_);

int HPQEnqueue(heap_ty* hp_, void* element_);

/*undefined behavior for empty heap*/
void HPQDequeue(heap_ty* hp_);

/*undefined behavior for empty heap*/
void* HPQPeek(heap_ty* hp_);

size_t HPQSize(const heap_ty* hp_);

int HPQIsEmpty(const heap_ty* hp_);


/* Removes the first (biggest according to cmp_f_) element which matches specific criteria according to match_func_.
 * Note: "param_" is optional.
 * Returns NULL if no matching element is found in the heap. */
void* HPQRemove(heap_ty* hp_, is_match_ty match_func_, const void* match_param_);

#endif /* _ILRD_HEAP_H_ */