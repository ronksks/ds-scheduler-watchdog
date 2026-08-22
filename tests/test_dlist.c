/* Date: 11/7/2024, Author: Ron Yalensky, Reviewer: GIl Livneh*/

#include <stdio.h>	/*	printf	*/
#include <assert.h>	/*	assert	*/

#include "../include/dlist.h" /*	typedefs: dlist_ty, dlist_iter_ty, dlist_node_ty, */

#define _TRUE_ 		(1)
#define _FALSE_		(0)
#define _SUCCESS_ 	(0)
#define _FAILED_ 	(1)
#define _INSERT_FAILURE_ (-1)

static void PrintList(dlist_ty *dlist);
static int IsMatch(void *iter_data, void *param_to_cmp);

void TestDlist();
void TestSetData();
void TestIsEmpty();
void TestDListRemove();
void TestDListCount();
void TestSplice();
void TestPushPopBackFront();
void TestDListFind();
void TestDListMultiFind();




int main()
{

	TestDlist();
	TestSetData();
	TestDListCount();
	TestDListRemove();
	TestPushPopBackFront();
	TestIsEmpty();
	TestDListFind();
	TestSplice();
	TestDListMultiFind();
	return(0);
}


void TestDlist()
{
	dlist_ty *dlist = DListCreate();
	int i = 0;
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	dlist_iter_ty return_itr = (assert(dlist), DListIterBegin(dlist));

    printf("\n---------------START----------------\n");

	for(i = 0; i < 10; ++i)
	{
		return_itr = DListInsertBefore(dlist, return_itr, &arr[i]);
		return_itr = DListIterNext(return_itr);
	}
	PrintList(dlist);
	
	printf("SUCESS\n" );
	DListDestroy(dlist);
}

void TestDListCount()
{
	dlist_ty *dlist = DListCreate();
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	int i = 0;
	size_t temp = 10;
	dlist_iter_ty return_itr = (assert(dlist), DListIterBegin(dlist));
    printf("\n---------------TestDListCount----------------\n");

	for(i = 0; i < 10; ++i)
	{
		return_itr = DListInsertBefore(dlist, return_itr, &arr[i]);

		return_itr = DListIterNext(return_itr);

	}
	PrintList(dlist);

	printf("Expected 10 elements:\n");
	if(DListCount(dlist) != temp)
	{
		printf("Error in DListCount\n");
	}
	else
	{
		printf("Sucess! Num of elements: %lu\n",DListCount(dlist));
	}


		DListDestroy(dlist);



}
void TestDListRemove()
{
	dlist_ty *dlist = DListCreate();
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	int i = 0;
	dlist_iter_ty return_itr = (assert(dlist), DListIterBegin(dlist));
    printf("\n---------------TestDListRemove----------------\n");

	for(i = 0; i < 10; ++i)
	{
		return_itr = DListInsertBefore(dlist, return_itr, &arr[i]);
		return_itr = DListIterNext(return_itr);


	}
	PrintList(dlist);
	return_itr = DListIterBegin(dlist);
	for(i = 0; i < 10; ++i)
	{
		return_itr = DListRemove(return_itr);
	}
	PrintList(dlist);
	printf("List cleanded all elementes removed\n");



	for(i = 0; i < 10; ++i)
	{
		return_itr = DListInsertBefore(dlist, return_itr, &arr[i]);
		return_itr = DListIterNext(return_itr);


	}


	return_itr = DListIterEnd(dlist);

	return_itr = DListIterPrev(return_itr);
	printf("Last element: %d \n", *(int *)DListIterGetData(return_itr));

	DListRemove(return_itr);
	printf("The list after removing the last element:\n");

	PrintList(dlist);



	DListDestroy(dlist);

}


void TestSetData()
{
	dlist_ty *dlist = DListCreate();
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	dlist_iter_ty return_itr = (assert(dlist), DListIterBegin(dlist));
	dlist_iter_ty end =DListIterEnd(dlist);

	printf("\n---------------TestSetData----------------\n");

    return_itr = DListInsertBefore(dlist, end, &arr[0]);
	printf("First element: : %d\n", *(int *)DListIterGetData(return_itr));
	printf("New element should be 10\n");
	DListIterSetData(return_itr,&arr[9]);
	printf("New element : %d\n", *(int *)DListIterGetData(return_itr));

	DListDestroy(dlist);


}

void TestIsEmpty()
{
	dlist_ty *dlist = DListCreate();
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	int i = 0;
	dlist_iter_ty return_itr = (assert(dlist), DListIterBegin(dlist));
    printf("\n---------------TestIsEmpty----------------\n");

	for(i = 0; i < 10; ++i)
	{
		return_itr = DListInsertBefore(dlist, return_itr, &arr[i]);
		return_itr = DListIterNext(return_itr);


	}

	printf("Num Of elements: %lu\n",DListCount(dlist));


	return_itr = DListIterBegin(dlist);
	for(i = 0; i < 10; ++i)
	{
		return_itr = DListRemove(return_itr);
	}

	printf("Num Of elements after remove: %lu\n",DListCount(dlist));
	printf("1 is for empty, is empty returns: %d\n",DListIsEmpty(dlist));

	DListDestroy(dlist);

}

void TestPushPopBackFront()
{
	dlist_ty *dlist = DListCreate();
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	dlist_iter_ty return_itr = (assert(dlist), DListIterBegin(dlist));
	int data1 = 0;
	int data2 = 0;
	int i = 0;
	int res = 0;

	printf("\n---------------TestPushPopBackFront----------------\n");
	for(i = 0; i < 10; ++i)
	{
		return_itr = DListInsertBefore(dlist, return_itr, &arr[i]);
		return_itr = DListIterNext(return_itr);


	}
	PrintList(dlist);
	data1 = *(int *)DListIterGetData(DListIterBegin(dlist));
	
	printf("\n---------------PopFirst----------------\n");
	
	DListPopFront(dlist);

	printf("First element: : %d\n",data1);
	printf("First element poped\n");
	PrintList(dlist);
	data1 = *(int *)DListIterGetData(DListIterPrev(DListIterEnd(dlist)));

	printf("\n---------------PopBack----------------\n");
	printf("Last element: : %d\n",data1);

	DListPopBack(dlist);
	printf("Last element poped\n");
	PrintList(dlist);

	printf("\n---------------PushBack----------------\n");

	data1 = 33;
	printf("Element to push to back: %d\n",data1);
	res = DListPushBack(dlist,&data1);
	if(res == 0)
	{
		printf ("Success push from back: \n");

	}
	else
	{

		printf ("Error push from back: \n");
	}
	PrintList(dlist);

	printf("\n---------------PushFront----------------\n");
	printf("Element to push to front: %d\n",data2);
	data2 = -25;
	if(0 == DListPushFront(dlist,&data2))
	{
		printf("Sucees");
	}
	else
	{
		printf("Failed PushFront");
	}
	PrintList(dlist);
    
	DListDestroy(dlist);
}

void TestDListFind()
{

	dlist_ty *dlist = DListCreate();
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
	dlist_iter_ty return_itr = DListIterEnd(dlist);
	dlist_iter_ty last_itr = DListIterEnd(dlist);
	int i = 0;
	int data = 10;
	printf("\n---------------TestDListFind----------------\n");

	for(i = 0; i < 10; ++i)
	{
		return_itr = DListInsertBefore(dlist, return_itr, &arr[i]);
		return_itr = DListIterNext(return_itr);
	}
	last_itr = DListIterPrev(DListIterEnd(dlist));
	if(_TRUE_ == DListIterIsEqual(last_itr, DListFind(DListIterBegin(dlist),DListIterEnd(dlist),IsMatch,(void *)&data)))
	{
		printf("Sucees DListFind!\n");

	}else{

		printf("NO Success DListFind\n");
	}

	DListDestroy(dlist);


}

void TestSplice()
{

	dlist_ty *dlist1 = DListCreate();
	dlist_ty *dlist2 = DListCreate();
	int arr1[10] = {1,2,3,4,5,6,7,8,9,10};
	int arr2[10] = {0,9,9,9,9,9,9,9,9,0};
	dlist_iter_ty return_itr1 = DListIterEnd(dlist1);
	dlist_iter_ty return_itr2 = DListIterEnd(dlist2);
	int i = 0;
	printf("\n---------------TestSplice----------------\n");

	for(i = 0; i < 10; ++i)
	{
		return_itr1 = DListInsertBefore(dlist1, return_itr1, &arr1[i]);
		return_itr1 = DListIterNext(return_itr1);
		return_itr2 = DListInsertBefore(dlist2, return_itr2, &arr2[i]);
		return_itr2 = DListIterNext(return_itr2);
	}
	printf("List1: \n");
	PrintList(dlist1);
	
	printf("\nList2: \n");
	PrintList(dlist2);

	DListSplice(DListIterNext(DListIterBegin(dlist1)), DListIterNext(DListIterBegin(dlist2)), DListIterPrev(DListIterEnd(dlist2))); 

	printf("Expected list 1: 1999999992345678910 \n");
	PrintList(dlist1);

	
	printf("Expected list 2: 00 \n");
	PrintList(dlist2);

	DListDestroy(dlist1);
	DListDestroy(dlist2);

}

void TestDListMultiFind()
{
	dlist_ty *dlist = DListCreate();
	int arr[10] = {1,2,3,2,5,6,7,2,9,10};
	dlist_iter_ty return_itr = DListIterEnd(dlist);
	int i = 0;
	int res = 0;
	int temp = 2;;

	printf("\n---------------TestDListFind----------------\n");

	for(i = 0; i < 10; ++i)
	{
		return_itr = DListInsertBefore(dlist, return_itr, &arr[i]);
		return_itr = DListIterNext(return_itr);
	}
	res = DListMultiFind(dlist, DListIterBegin(dlist), DListIterPrev(DListIterEnd(dlist)), *IsMatch,&temp);


	printf("Expected: %d\n",3);
	if(res != 3)
	{
		printf("Failed, RES is: %d\n",res);

	}
	printf("Success!  RES: %d\n",res);

	DListDestroy(dlist);

}


int IsMatch(void *iter_data, void *param_to_cmp)
{
	assert(iter_data);
	assert(param_to_cmp);
	return (*(int *)iter_data == *(int *)param_to_cmp);
}

static void PrintList(dlist_ty *dlist)
{
	dlist_iter_ty runner =(assert(dlist), DListIterBegin(dlist));
	dlist_iter_ty end = DListIterEnd(dlist);

	printf("\n");

	while (!DListIterIsEqual(runner, end))
	{
		printf("%d", *(int *)DListIterGetData(runner));
		runner = DListIterNext(runner);
	}
	
	printf("\n");

}
