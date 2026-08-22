/* Date: 10/7/2024, Author: Ron Yalensky, Reviewer: Oded Fisher*/

#ifndef __ILRD_CBUFF_H__
#define __ILRD_CBUFF_H__ 

#include <unistd.h> /* size_t*/

typedef struct circular_buffer cbuff_ty;

/*******************************************************************************
 * Creates a new circular buffer with a specified capacity.
 * Return Value: A pointer to the created circular buffer, or NULL if
 * allocation fails.
 * Notes: Undefined behavior if capacity is 0.
 *        Must call CBuffDestroy at the end of use
 * Complexity: O(1) + system call complexity.
******************************************************************************/
cbuff_ty *CBuffCreate(size_t capacity);

/*******************************************************************************
 * Destroys the circular buffer.
 * Return Value: None.
 * Notes: Nothing occurs if cbuff is NULL.
 * Complexity: O(1) + system call complexity.
******************************************************************************/
void CBuffDestroy(cbuff_ty *cbuff);

/*******************************************************************************
 * Writes at most n_bytes from src to cbuff
 * Return Value: The number of bytes actually written (0 if cbuff is FULL)
 * Notes: Undefined behavior if cbuff or src is NULL.
 *        src must be at least n_bytes size, otherwise undefined behaviour. 
 * Complexity: O(n)
******************************************************************************/
size_t CBuffWrite(cbuff_ty *cbuff, const void *src, size_t n_bytes);

/*******************************************************************************
 * Reads at most n_bytes from "cbuff" to "dest"
 * Arguments: 
 * Return Value: The number of bytes actually read ( 0 if empty)
 * Notes: Undefined behavior if dest or cbuff is NULL.
 *        dest must be at least n_bytes size, otherwise undefined behaviour.
 * Complexity: O(n)
******************************************************************************/
size_t CBuffRead(cbuff_ty *cbuff, void *dest, size_t n_bytes);

/*******************************************************************************
 * Returns the number of occupied bytes in the circular buffer.
 * Notes: Undefined behavior if cbuff is NULL.
 * Complexity: O(1)
******************************************************************************/
size_t CBuffBufSiz(const cbuff_ty *cbuff);

/*******************************************************************************
 * Returns the amount of free bytes in the circular buffer.
 * Notes: Undefined behavior if cbuff is NULL.
 * Complexity: O(1)
******************************************************************************/
size_t CBuffFreeSpace(const cbuff_ty *cbuff);

/*******************************************************************************
 * Checks if the circular buffer is empty.
 * Return Value: 1 if the circular buffer is empty, 0 otherwise.
 * Notes: Undefined behavior if cbuff is NULL.
 * Complexity: O(1)
******************************************************************************/
int CBuffIsEmpty(const cbuff_ty *cbuff);

#endif /* __ILRD_CBUFF_H__ */
