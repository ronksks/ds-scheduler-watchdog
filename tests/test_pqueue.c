/* Date: 17/7/2024, Author: Ron Yalensky, Reviewer: Daniel (The magic mike) Ehrenberg */

#include <stdio.h>	/*	printf	*/
#include <assert.h>	/*	assert	*/

#include "pqueue.h" /*	typedefs: p_queue_ty */
#include "sorted_list.h" /*	typedefs: sort_list_ty, sort_list_iter_ty */
#include "dlist.h" /*	typedefs: dlist_ty, dlist_iter_ty, dlist_node_ty, */

#define _TRUE_ 		(1)
#define _FALSE_		(0)
#define _SUCCESS_ 	(0)
#define _FAILED_ 	(1)

static int IsBigger(void *data_to_check, void *new_data);
static int IsDivisibleBy(void *data_to_check, void *new_data);



void TestCreate();
void TestEnqueue();
void TestPQClear();
void TestPQCount();
void TestPQCErase();


int main()
{
	TestCreate();
	TestEnqueue();
	TestPQClear();
	TestPQCount();
	TestPQCErase();






	return(0);
}

void TestCreate()
{
	p_queue_ty *pq = PQCreate(IsBigger);
	printf("\n---------------TestCreate----------------\n");

	if(NULL != pq)
	{
		printf("Test Passed!\nPqueue created succesfully!\n");
	}
	else
	{
		printf("Test Failed!\nError in PQCreate.\n");
	}
	PQDestroy(pq);
}

void TestEnqueue()
{
	p_queue_ty *pq = PQCreate(IsBigger);
	
    int arr[] = {10, 9, 8, 22, 6, 5, 4, 3, 2, 1};
    int sorted[] = {1, 2, 3, 4, 5, 6 ,8 ,9 ,10 ,22};
    int expected[] = {1, 2, 3, 4, 5, 6 ,8 ,9 ,10 ,22};
    size_t i = 0;
    int res = _SUCCESS_;
    void *return_value = NULL;
    assert(pq != NULL);

     printf("\n---------------TestEnqueue----------------\n");

    for (i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i)
    {
        /*res = *(int *)(PQEnqueue(pq, &arr[i]));*/
        res = PQEnqueue(pq, &arr[i]);
        if(res == _SUCCESS_)
        {
   	        printf("#%ld Element Enqueue: %d\n", i, arr[i]);
        }

    }
    if(_FAILED_ == res)
    {
    	printf("Test Failed! \nError in Enqueue Test\n");
    }
    else
    {
    	printf("Test Passed! \nEnqueue successful\n");
    }

     printf("\n---------------TestPQPeek----------------\n");

    return_value = PQPeek(pq);
    
    printf("Expected value to peek:%d\n",expected[0]); 
    printf("Returned value from peek: %d\n",*(int *)return_value);
    
    if(*(int *)return_value != expected[0])
    {
		printf("Test Failed!\nError in PQPeek Test \n");
    }
    else
    {
		printf("Test Passed!\n");

    }

     
     printf("\n---------------TestDequeue----------------\n");

    res = _SUCCESS_;

    for (i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i)
    {
        return_value = PQDequeue(pq);
        printf("#%ld Element Dequeue: %d\n", i, *(int *)return_value);
        if((*(int *)return_value) != expected[i])
        {
        	res = _FAILED_;
        }

    }
  if(_FAILED_ == res)
    {
    	printf("Test failed!\nError in Enqueue Test\nResult data is not as expected.\n");
    }
    else
    {
    	printf("Test Passed! \nEnqueue successful\n");
    }


     printf("\n---------------TestPQIsEmpty----------------\n");


    if(_FALSE_ == PQIsEmpty(pq))
    {
	    printf("Test failed!\nError in TestPQIsEmpty Test\nResult data is not as expected.\n");
    }
    else
    {
    	printf("Test Passed! \nTestPQIsEmpty successful\n");
    }

    (void)sorted;
	PQDestroy(pq);
}

void TestPQClear()
{
	p_queue_ty *pq = PQCreate(IsBigger);
	
    int arr[] = {10, 9, 8, 22, 6, 5, 4, 3, 2, 1};
    size_t i = 0;
    int res = _SUCCESS_;
    assert(pq != NULL);

    printf("\n---------------TestPQClear----------------\n");

    for (i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i)
    {
        res = PQEnqueue(pq, &arr[i]);
    }

    PQClear(pq);

  if(_FALSE_ == PQIsEmpty(pq))
    {
	    printf("Test failed!\nError in TestPQClear Test\nResult data is not as expected.\n");
    }
    else
    {
    	printf("Test Passed! \nTestPQClear successful\n");
    }

    (void)res;

   	PQDestroy(pq);
}

void TestPQCount()
{
	p_queue_ty *pq = PQCreate(IsBigger);
	
    int arr[] = {10, 9, 8, 22, 6, 5, 4, 3, 2, 1};
    size_t i = 0;
    size_t size_arr = sizeof(arr) / sizeof(arr[0]);
    int res = _SUCCESS_;

    assert(pq != NULL);

    printf("\n---------------TestPQCount----------------\n");

    for (i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i)
    {
        res = PQEnqueue(pq, &arr[i]);
    }

    if(size_arr != PQCount(pq))
    {
	    printf("Test failed!\nError in TestPQCount Test\nResult data is not as expected.\n");
    }
    else
    {
    	printf("Test Passed! \nTestPQCount successful, returns: %ld\n",PQCount(pq));
    }
   	
   	(void)res;
   	PQDestroy(pq);
}

void TestPQCErase()
{
	p_queue_ty *pq = PQCreate(IsBigger);
	
    int arr[] = {10, 9, 8, 22, 6, 5, 4, 3, 2, 11};
    int to_div = 77;
    size_t i = 0;
    int res = 0;
    void *return_value = NULL;

    assert(pq != NULL);

    printf("\n---------------TestPQCErase----------------\n");

    for (i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i)
    {
        res = PQEnqueue(pq, &arr[i]);
    }

    return_value = PQErase(pq, IsDivisibleBy,&to_div); 

    res = *(int *)return_value;

    if(arr[9] != res) /*11 != 11*/
   {
	    printf("Test failed!\nError in TestPQCErase Test\nResult data is not as expected.\n");
    }
    else
    {
    	printf("Test Passed! \nTestPQCErase successful, returned value: %d\n",res);
    }

   	PQDestroy(pq);
}

static int IsBigger(void *data_to_check, void *new_data)
{
	if(*(int*)data_to_check > *(int*)new_data)
	{
		return (_TRUE_);
	}
	return (_FALSE_);
}

static int IsDivisibleBy(void *data_to_check, void *new_data)
{
	int a = *(int*)new_data;
	int b = *(int*)data_to_check;
	if(0 == (a % b))
	{
		return (_TRUE_);
	}
	return (_FALSE_);
}
	/*if(0 == ((*(int*)new_data) % (*(int*)data_to_check)))*/