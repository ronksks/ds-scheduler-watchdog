/* Date: 3/7/2024, Author: Ron Yalensky, Reviewer: Daniel the mighty ehrenberg*/

#include <stdio.h>	/*	printf	*/
#include <stdlib.h> /*	malloc	*/
#include <assert.h>	/*	assert	*/
#include "../include/slist.h" /*	typedefs: slist_node_ty, slist*/

void TestSListCreate();
void TestSListDestroy();
void TestSListInsertBefore();
void TestSListRemove();
void TestSListCount();
void TestSListIsEmpty();
void TestSListIterEnd();
void TestSListIterNext();
void TestSListIterIsEqual();
void TestSListIterGetData();
void TestSListIterSetData();
void TestSListFind();
void TestSListForEach();
void TestSListAppend();

int IsMatched(const void *param1, const void *param2);
int OpIncrementParam(void *param1, const void *param2);

static void PrintList(slist_ty *slist, size_t length)
{
	void *res = NULL;
	size_t i = 0;
	slist_iter_ty iter = SListIterBegin(slist);

	for(i = 0; i < length; ++i)
	{
		res = SListIterGetData(iter);
		printf("%d ", *(int*)res);
		iter = SListIterNext(iter);
	}
	printf("\n");
}




int main()
{

	TestSListCreate();
	TestSListInsertBefore();
	TestSListCount();
	TestSListIsEmpty();
	TestSListIterGetData();
	TestSListIterSetData();
	TestSListIterIsEqual();
	TestSListIterEnd();
	TestSListDestroy();
	TestSListFind();
	TestSListRemove();
	TestSListForEach();
	TestSListAppend();
	/*TestSListIterNext();*/

	return(0);
}


void TestSListCreate()
{
	slist_ty *slist = NULL;
    printf("---------------SListCreate----------------\n");
	slist = SListCreate();
	SListDestroy(slist);
	slist = NULL;


}

void TestSListDestroy()
{
	slist_ty *slist = SListCreate();
    printf("---------------SListDestroy----------------\n");
	SListDestroy(slist);
	slist = NULL;

}

void TestSListInsertBefore()
{
	slist_ty *slist = SListCreate();
	slist_iter_ty iter = SListIterBegin(slist);
	int data = 10;
	int res = 0;

    printf("---------------SListInsertBefore----------------\n");

	iter = SListInsertBefore(slist, iter, &data);
	res = *(int *)SListIterGetData(SListIterBegin(slist));
	printf("Expected: %d\n", data);
	printf("Result: %d\n", res);
	if (data == res)
	{
		printf("Test SListInsertBefore succesful! \n");
	}
	else
	{
		printf("Test SListInsertBefore Failed! \n");
	}
	
	data = 20;
	iter = SListInsertBefore(slist, iter, &data);
	res = *(int *)SListIterGetData(SListIterBegin(slist));
	printf("Expected: %d\n", data);
	printf("Result: %d\n", res);
	if (data == res)
	{
		printf("Test SListInsertBefore succesful! \n");
	}
	else
	{
		printf("Test SListInsertBefore Failed! \n");
	}
	SListDestroy(slist);
	slist = NULL;

}

void TestSListCount()
{
	slist_ty *slist = SListCreate();
	slist_iter_ty iter = SListIterBegin(slist);
	size_t i = 0;
	size_t count = 0;
	/*int x = 10;
	int j = 20;*/
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};

    printf("\n---------------SListCount----------------\n");
	
	for(i = 0; i < 10; ++i)
	{
		iter = SListInsertBefore(slist, iter, &arr[i]);
	}

	count = SListCount(slist);

	printf("Expected count: %ld\n",i );
	printf("Result count: %ld\n",count );
	if(count == i)
	{
		printf("Test SListCount succesful\n");
	}
	else
		{
			printf("Test SListCount Failed\n");
		}

	SListDestroy(slist);
	slist = NULL;

}

void TestSListIsEmpty()
{
	
	slist_ty *slist = SListCreate();
	int res = SListIsEmpty(slist);
	slist_iter_ty iter = NULL;
	int check_res = 0;
	int i = 0;
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};

    
    printf("\n---------------SListIsEmpty----------------\n");

	printf("Expected list is empty: %d\n", check_res);
	printf("Result list is empty: %d\n",res);
	if(res == check_res)
	{
		printf("Test SListIsEmpty succesful\n");
	}
	else
		{
			printf("Test SListIsEmpty Failed\n");
		}

	/*insert nodes and check SListIsEmpty*/

	iter = SListIterBegin(slist);
	check_res = 10;
	for(i = 0; i < 10; ++i)
	{
		iter = SListInsertBefore(slist, iter, &arr[i]);
	}
	res = SListIsEmpty(slist);

	printf("\nExpected list is empty: %d\n", check_res);
	printf("Result list is empty: %d\n",res);
	if(res == check_res)
	{
		printf("Test SListIsEmpty Failed\n");
	}
	else
	{
		printf("Test SListIsEmpty succesful\n");
	}

	SListDestroy(slist);
	slist = NULL;

}




void TestSListIterEnd()
{
	slist_ty *slist = SListCreate();
	slist_iter_ty iter_end = NULL;
    
    printf("---------------SListIterEnd----------------\n");	
	
	iter_end = SListIterEnd(slist);
	if(iter_end == NULL)
	{
		printf("iter_end == NULL\n");
	}
	SListDestroy(slist);
	slist = NULL;



}

void TestSListIterNext()
{
	slist_iter_ty iter_cur = NULL;
	slist_iter_ty iter_next = NULL;
    
    printf("---------------SListIterNext----------------\n");	

	iter_next = SListIterNext(iter_cur);
	if(iter_next == NULL)
	{
		printf("iter_next == NULL\n");
	}

}



void TestSListIterGetData()
{
	slist_ty *slist = SListCreate();
	slist_iter_ty iter = SListIterBegin(slist);
	int data = 344;
	int check_res = 0;
    printf("\n---------------SListIterGetData----------------\n");	


	iter = SListInsertBefore(slist, iter, &data);
	check_res = *(int *)SListIterGetData(iter);
	printf("Expected data: %d\n",data);
	printf("Result data: %d\n",check_res);
	if(data == check_res)
	{
		printf("Test SListIterGetData Succesful\n");
	}
	else
	{
		printf("Test SListIterGetData Failed\n");
	}




    printf("---------------SListIterNext----------------\n");	

	SListDestroy(slist);
	slist = NULL;
}

void TestSListIterSetData()
{

	slist_ty *slist = SListCreate();
	slist_iter_ty iter = SListIterBegin(slist);
	int data = 325;
	int check_res = 0;

    printf("\n---------------SListIterSetData----------------\n");	

	iter = SListInsertBefore(slist, iter, &data);
	/*printf("Data: %d \n", *(int *)SListIterGetData(iter));*/
	SListIterSetData(iter,&check_res);
	printf("Data sould be 0 answer is: %d \n", *(int *)SListIterGetData(iter));
	if(data == check_res)
	{
		printf("Test SListIterSetData Failed\n");
	}
	else
	{
		printf("Test SListIterSetData Succesful\n");
	}
	SListDestroy(slist);
	slist = NULL;
}
void TestSListFind()
{

	slist_ty *slist = SListCreate();
	slist_iter_ty iter = NULL;
	void *res = NULL;
	void *to_find = NULL;
	int i = 0;
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};

    
    printf("---------------SListFind----------------\n");	
	iter = SListIterBegin(slist);

	for(i = 0; i < 10; ++i,	iter = SListIterNext(iter))
	{
		iter = SListInsertBefore(slist, iter, &arr[i]);
		if (i == 4)
		{
			to_find = &arr[i];
		}
		
	}

	iter = SListFind(SListIterBegin(slist), SListIterEnd(slist), IsMatched, to_find);
	res = SListIterGetData(iter);


	printf("Expedted data is: %d\n",*(int *)to_find);
	printf("Result data is: %d\n",*(int *)res);

	SListDestroy(slist);
	slist = NULL;
}
void TestSListForEach()
{
	slist_ty *slist = SListCreate();
	slist_iter_ty iter = NULL;
	size_t length = 0;
	int i = 0;
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};

    
    printf("---------------SListForEach----------------\n");	
	iter = SListIterBegin(slist);
	for(i = 0; i < 10; ++i, iter = SListIterNext(iter))
	{
		iter = SListInsertBefore(slist, iter, &arr[i]);
	}


	printf("Before:\n");
	length = SListCount(slist);
	PrintList(slist,length);
	
	SListForEach(SListIterBegin(slist), SListIterEnd(slist), OpIncrementParam, NULL);
	
	printf("After:\n");

	PrintList(slist,SListCount(slist));

	SListDestroy(slist);
	slist = NULL;
}




void TestSListRemove()
{
	size_t i = 0;
	slist_ty *slist = SListCreate();
	slist_iter_ty iter = SListIterBegin(slist);

	size_t length = 0;
	/*int check_res = 0;*/
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};

	for(i = 0; i < 10; ++i)
		{
			iter = SListInsertBefore(slist, iter, &arr[i]);
		}

	length = SListCount(slist); 
	
	printf("Original List:\n");
	PrintList(slist, length);

	/* Remove First */
	printf("Remove First:\n");
	SListRemove(iter);
	length = SListCount(slist);
	PrintList(slist, length);

	/* Remove Last */
	printf("Remove Last:\n");
	iter = SListIterBegin(slist);

	for(i = 0; i < length-1; ++i)
	{
		printf("i = %ld\n",i);
		iter = SListIterNext(iter);

	}
	
	length = SListCount(slist);
	PrintList(slist, length);

	SListRemove(iter);
	length = SListCount(slist);
	PrintList(slist, length);

	/* Remove Middle */
	printf("Remove Middle:\n");
	iter = SListIterBegin(slist);
	for(i = 0; i < 3;  ++i)
	{
		iter = SListIterNext(iter);
	}
	SListRemove(iter);
	length = SListCount(slist);
	PrintList(slist, length);

	SListDestroy(slist);
	slist = NULL;
}

void TestSListIterIsEqual()
{
    slist_ty *slist = SListCreate();
	slist_iter_ty iter1 = SListIterBegin(slist);
	slist_iter_ty iter2 = SListIterBegin(slist);
	int res = 0;
	int check_res = 1;
	int data1 = 135;
	int data2 = 668;
	size_t length  = 0;

    
    printf("\n---------------SListIterIsEqual----------------\n");	

	res = SListIterIsEqual(iter1,iter2);


    printf("Expected: %d for equal\n", check_res);
	printf("Result: %d for equal \n",res);
	if(res == check_res)
	{
		printf("Test SListIterIsEqual succesful\n");
	}
	else
	{
		printf("Test SListIterIsEqual Failed\n");
	}

	iter1 = SListInsertBefore(slist, iter1, &data1);
	iter2 = SListInsertBefore(slist, iter1, &data2);
	res = SListIterIsEqual(iter1,iter2);
	check_res = 0;

    printf("\nExpected: %d for not equal\n", check_res);
	printf("Result: %d for not equal\n",!res);
	
	length = SListCount(slist);
	PrintList(slist,length);
	
	if(res == check_res)
	{
		printf("Test SListIterIsEqual Succesful\n");
	}
	else
	{
		printf("Test SListIterIsEqual Failed\n");
	}
	res = SListCount(slist);

	SListDestroy(slist);
	slist = NULL;

}

void TestSListAppend()
{
	size_t i = 0;
	size_t length  = 0;
	slist_ty *slist1 = SListCreate();
	slist_ty *slist2 = SListCreate();
	slist_iter_ty iter1 = SListIterBegin(slist1);
	slist_iter_ty iter2 = SListIterBegin(slist2);

	int arr1[10] = {1,2,3,4,5,6,7,8,9,10};
	int arr2[10] = {11,12,13,14,15,16,17,18,19,20};
    printf("\n---------------SListAppend----------------\n");	
	length = SListCount(slist1);
	PrintList(slist1,length);



	for(i = 0; i < 10; ++i,iter1 = SListIterNext(iter1), iter2 = SListIterNext(iter2))
		{
			iter1 = SListInsertBefore(slist1, iter1, &arr1[i]);
			iter2 = SListInsertBefore(slist2, iter2, &arr2[i]);
		}


	SListAppend(slist1,slist2);

	length = SListCount(slist1);
	PrintList(slist1,length);

	SListDestroy(slist1);
	SListDestroy(slist2);
	slist1 = NULL;
	slist2 = NULL;

}



int IsMatched(const void *param1, const void *param2)
{	
	return ( param1 == param2);

}


int OpIncrementParam(void *param1, const void *param2)
{
	assert(NULL != param1);
	*(int *)param1 += 1;
	(void)param2;

	return (0);
}