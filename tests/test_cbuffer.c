/* Date: 10/7/2024, Author: Ron Yalensky, Reviewer: Oded Fisher*/

#include <stdio.h>	/*	printf	*/
#include <stdlib.h> /*	malloc	*/
#include <assert.h>	/*	assert	*/
#include <string.h>	/*	strlen	*/
#include "../include/cbuffer.h" /*	typedefs: cbuff_ty*/

/*	Status Macros		*/
#define __ILRD_FAIL     (1)
#define __ILRD_SUCCESS  (0)
#define __ILRD_TRUE     (1)
#define __ILRD_FALSE    (0)
#define __ILRD_CAPACITY (10)



void TestCbuff1();
void TestCbuff2();
void TestCbuff3();

int main()
{
	TestCbuff1();
	TestCbuff2();
	TestCbuff3();
	return(0);
}


/*******************************************************************************
 * Test Plan 1:
 * Buff capacity: 10
 * Write 8 elements
 * Read 6 elements
 * Write 8 elements
 * Result: 3, 4, 5, 6, 7, 8, 7, 8, 1, 2
 * Buffsize = 10
******************************************************************************/

void TestCbuff1()
{
	char data[] = "12345678";
	char read_data[__ILRD_CAPACITY] = {0};
	size_t written_bytes = 0;
	size_t i = 0;

	cbuff_ty *cbuff = CBuffCreate(__ILRD_CAPACITY);

	printf("\n*************************Test 1**************************\n");
	
	written_bytes = CBuffWrite(cbuff, data, strlen(data));
	printf("SIZE after write: CBuffBufSiz: %lu\n",CBuffBufSiz(cbuff));
	
	printf("Read bytes: %lu\n",CBuffRead(cbuff, read_data, 6 ));
	printf("SIZE after read: %lu\n",CBuffBufSiz(cbuff));
	
	for(i = 0; i < sizeof(read_data)/sizeof(read_data[0]); ++i)
	{
		printf("read_data: %c\n",read_data[i] );
	}
	
	written_bytes = CBuffWrite(cbuff, data, strlen(data));

	printf("Written bytes: %lu\n",written_bytes);

	printf("SIZE after second write: %lu\n",CBuffBufSiz(cbuff));
	CBuffDestroy(cbuff);
	cbuff = NULL;
}

/*******************************************************************************
 * Test Plan 2:
 * Buff capacity: 10
 * Write 5 elements
 * Read 8 elements
 * Write 8 elements
 * Result: 6, 7, 8, *, *, 1, 2, 3, 4, 5
 * Buffsize = 8
******************************************************************************/
void TestCbuff2()
{
	char data[] = "12345678";
	char read_data[__ILRD_CAPACITY] = {0};
	size_t written_bytes = 0;
	size_t i = 0;

	cbuff_ty *cbuff = CBuffCreate(__ILRD_CAPACITY);
	printf("\n*************************Test 2**************************\n");
	written_bytes = CBuffWrite(cbuff, data, 5);
	printf("SIZE after write: CBuffBufSiz: %lu\n",CBuffBufSiz(cbuff));
	
	printf("Read bytes: %lu\n",CBuffRead(cbuff, read_data, 8 ));
	printf("SIZE after read: %lu\n",CBuffBufSiz(cbuff));
	
	for(i = 0; i < sizeof(read_data)/sizeof(read_data[0]); ++i)
	{
		printf("read_data: %c\n",read_data[i] );
	}
	
	written_bytes = CBuffWrite(cbuff, data, strlen(data));

	printf("Written bytes: %lu\n",written_bytes);

	printf("SIZE after second write: %lu\n",CBuffBufSiz(cbuff));

	printf("Size left in buffsiz: %ld\n",CBuffFreeSpace(cbuff));
	CBuffDestroy(cbuff);
	cbuff = NULL;
}

/*******************************************************************************
 * Test Plan 3:
 * Buff capacity: 10
 * Write 10 elements
 * Read 9 elements
 * Write 8 elements
 * Result: 2, 3, 4, 5, 6, 7, 8, 9, *, X
 * Buffsize = 9
******************************************************************************/
void TestCbuff3()
{
	char data[] = "123456789X";
	char read_data[__ILRD_CAPACITY] = {0};
	size_t written_bytes = 0;
	size_t i = 0;

	cbuff_ty *cbuff = CBuffCreate(__ILRD_CAPACITY);
	
	printf("\n*************************Test 3**************************\n");

	written_bytes = CBuffWrite(cbuff, data, strlen(data));
	printf("SIZE after write: CBuffBufSiz: %lu\n",CBuffBufSiz(cbuff));
	
	printf("Read bytes: %lu\n",CBuffRead(cbuff, read_data, 9));
	printf("SIZE after read: %lu\n",CBuffBufSiz(cbuff));
	
	for(i = 0; i < sizeof(read_data)/sizeof(read_data[0]); ++i)
	{
		printf("read_data: %c\n",read_data[i] );
	}
	
	written_bytes = CBuffWrite(cbuff, data+1, 8);

	printf("Written bytes: %lu\n",written_bytes);

	printf("SIZE after second write: %lu\n",CBuffBufSiz(cbuff));
	CBuffDestroy(cbuff);
	cbuff = NULL;
}