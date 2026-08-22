/* Date: 10/7/2024, Author: Ron Yalensky, Reviewer: Oded Fisher*/

#include <stdio.h>	/*	printf			*/
#include <stdlib.h> /*	malloc, free	*/
#include <assert.h>	/*	assert			*/
#include <stddef.h>	/*	ofsetoff, size_t*/

#include "../include/cbuffer.h" /*	typedefs: cbuff_ty*/

/*******************************************************************************
 * Status Macros
******************************************************************************/
#define __ILRD_FAIL     (1)
#define __ILRD_SUCCESS  (0)
#define __ILRD_TRUE     (1)
#define __ILRD_FALSE    (0)
#define __ILRD_BYTE_WRITTEN     (1)
#define __ILRD_BYTE_NOT_WRITTEN (0)

/*******************************************************************************
 * 							Macro Functions
******************************************************************************/
#define CHECK_IS_END(ptr, cbuff) (ptr == (cbuff->buffer + (cbuff->capacity))) /* Returns 1 if ptr is at the end of buffer, otherwise 0 */
#define CHECK_IF_FULL(cbuff) (cbuff->read == cbuff->write) /* Returns 1 if read and write point to the same address, otherwise 0*/
#define CHECK_IF_EMPTY(cbuff) ((cbuff->read) == (char *)cbuff) /* Returns 1 if read points to the start of the struct (address of capasity), otherwise 0*/


/*******************************************************************************
 * 							Structs Definition
******************************************************************************/
struct circular_buffer
{
    size_t capacity;
    char *write;
    char *read;
    char buffer[1];	

};

/*******************************************************************************
 * 							Static Functions
******************************************************************************/
static void ResetPointerToStartOfBuffer(char **ptr, cbuff_ty *cbuff) 
{
    assert(ptr != NULL);
    assert(cbuff != NULL);
    *ptr = cbuff->buffer;
}

static void PrintBuffer(const cbuff_ty *cbuff)
{
	size_t len = 0;
	char *ptr = NULL;
	assert(NULL != cbuff);
	assert(NULL != cbuff->buffer);
    
    ptr = (char *)&(cbuff->buffer[0]);
	len = cbuff->capacity;

	while(len)
	{
		printf(" %c,",*ptr++);
		--len;
	}
			printf("\n");

	
}


/*******************************************************************************
 * 							Functions
******************************************************************************/

cbuff_ty *CBuffCreate(size_t capacity)
{
	/*	allocate memory for the struct + add it exactly the extra memory for the wanted capasity*/

    cbuff_ty *cbuff = (cbuff_ty *)malloc(offsetof(cbuff_ty, buffer) + (assert(capacity > 0),capacity));

	if (NULL == cbuff)
	{
		free(cbuff);
		return (NULL);
	}
	cbuff->capacity = capacity;
	cbuff->write = cbuff->buffer; /* set write to point the begining of the buffer*/
	cbuff->read = (char *)cbuff; /*	set read to point the begining of the cbuffer*/
	return (cbuff);
}


void CBuffDestroy(cbuff_ty *cbuff)
{
	free(cbuff);
	cbuff = NULL;

}


size_t CBuffWrite(cbuff_ty *cbuff, const void *src, size_t n_bytes)
{
	size_t count = 0;
	assert(NULL != cbuff);
	assert(NULL != src);
	
	if(__ILRD_TRUE == CHECK_IF_FULL(cbuff))
	{
		return(__ILRD_BYTE_NOT_WRITTEN); /* return 0 bytes writetn and exits CBuffWrite*/
	}
	
	if(__ILRD_TRUE == CBuffIsEmpty(cbuff))
	{
		cbuff->read = cbuff->write;
	}
	
	while(0 != n_bytes)
	{
		*(cbuff->write) = *(char *)src; /*	write data	*/
		++count;
		++(cbuff->write); /* incerements write pointer with 1 byte within the range of the buffer*/

		if (__ILRD_TRUE == CHECK_IS_END(cbuff->write, cbuff))
		{
		    ResetPointerToStartOfBuffer(&(cbuff->write), cbuff);
		}
		if(__ILRD_TRUE == CHECK_IF_FULL(cbuff))
		{
				PrintBuffer(cbuff);

			return(count);
		}

		src = (char *)src + 1; /* incerements src pointer with 1 byte*/
		--n_bytes;
	}
	PrintBuffer(cbuff);
	return(count);

}


size_t CBuffRead(cbuff_ty *cbuff, void *dest, size_t n_bytes)
{
	size_t count = 0;
	assert(NULL != cbuff);
	assert(NULL != dest);
	
	if(__ILRD_TRUE == CBuffIsEmpty(cbuff))
	{
		return(__ILRD_BYTE_NOT_WRITTEN);
	}

	while(count < n_bytes)
	{


		*(char *)dest = *(cbuff->read);
		++(cbuff->read);
		++count;
		dest = (char *)dest + 1; /* incerements dest pointer with 1 byte*/
		
		if (__ILRD_TRUE == CHECK_IS_END(cbuff->read, cbuff))
		{
		    ResetPointerToStartOfBuffer(&(cbuff->read), cbuff);
		}

		if(cbuff->read == cbuff->write) /*	here the buffer is empty*/
		{
			cbuff->read = (char *)cbuff; /* reset read pointer to managerial struct */
			return(count);

		}
	}

	return (count);




}

size_t CBuffBufSiz(const cbuff_ty *cbuff) 
{
    size_t size = 0;
    assert(NULL != cbuff);
    assert(NULL != cbuff->read);
    assert(NULL != cbuff->write);

    if (__ILRD_TRUE == CHECK_IF_FULL(cbuff)) 
    {
        return (cbuff->capacity);
    }

    if (__ILRD_TRUE == CHECK_IF_EMPTY(cbuff)) 
    {
        return (0);
    }
    
    if (cbuff->read > cbuff->write) 
    {
        size = cbuff->capacity - (cbuff->read - cbuff->write);
    } 
    else 
    {
        size = cbuff->write - cbuff->read;
    }

    return (size);
}

size_t CBuffFreeSpace(const cbuff_ty *cbuff)
{
	assert(NULL != cbuff);
	return(cbuff->capacity - CBuffBufSiz(cbuff));

}


int CBuffIsEmpty(const cbuff_ty *cbuff)
{
	assert(NULL != cbuff);

	return((cbuff->read) == (char *)cbuff);
}

