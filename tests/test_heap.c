		
/* META DATA
Date: 02/09/2024
Author: Ron Yalensky
Reviewer:
Description: Test file for Heap project. */

/* Header Files Used */

#include <assert.h>/* assert */
#include <time.h> /* srarand */
#include "heap.h"
#include "test.h"
#include "utils.h"



#define VECTOR_SIZE (100)
#define ARR_SIZE (10)


/*---------------FUNCTION DECLARATION---------------------*/

static test_status_t TestHPQCreate(void);
static test_status_t TestHPQEnqueue(void);
static test_status_t TestHPQPeek(void);
static test_status_t TestHPQDequeue(void);
static test_status_t TestHPQRemove(void);
static test_status_t TestHPQDestroy(void);

static int CmpInts(const void* data1_, const void* data2_, const void* param_);
static int VoidToInt(const void* ptr);
static int IsMatch(const void* data_, const void* param_);

static void InitRandomArray(int arr[], int size);

/*-------------------------MAIN---------------------------*/

int main()
{
    RUNTEST(TestHPQCreate);
    RUNTEST(TestHPQEnqueue);
    RUNTEST(TestHPQPeek);
    RUNTEST(TestHPQDequeue);
    RUNTEST(TestHPQRemove);
    /* 
     */
    RUNTEST(TestHPQDestroy);

    return (PASSED); 
}

/*--------------------------------------------------------------------------------------------*/

/* Test heap creation */
static test_status_t TestHPQCreate(void)
{
    heap_ty* heap;

    FUNC_TO_TEST(HPQCreate);

    REQUIRE(((heap = HPQCreate(CmpInts, NULL, ARR_SIZE)), NULL != heap));
    LOGSTR("Initial SIZE:");
    LOGLONG(HPQSize(heap));

    REQUIRE(HPQIsEmpty(heap));
    LOGSTR("Heap is empty after creation");

    HPQDestroy(heap);
    return (PASSED);
}

/* Test enqueuing elements into the heap */
static test_status_t TestHPQEnqueue(void)
{
    heap_ty* heap;
    size_t i = 0;
    void* data = NULL;
    int arr[ARR_SIZE] = {0};
    
    InitRandomArray(arr, ARR_SIZE);

    heap = HPQCreate(CmpInts, NULL, ARR_SIZE);

    FUNC_TO_TEST(HPQEnqueue);

    LOGSTR("Inserting numbers:");
    for (i = 0; i < ARR_SIZE; ++i)
    {
        LOGINT(arr[i]);
        data = &arr[i];
        REQUIRE(SUCCESS == HPQEnqueue(heap, &data));    
    }

    REQUIRE(0 != HPQSize(heap));
    LOGSTR("SIZE after enqueuing:");
    LOGLONG(HPQSize(heap));

    HPQDestroy(heap);
    return (PASSED);
}

/* Test peeking at the top element of the heap */
static test_status_t TestHPQPeek(void)
{
    heap_ty* heap;
    size_t i = 0;
    void* data = NULL;
    int arr[ARR_SIZE] = {0};

    InitRandomArray(arr, ARR_SIZE);
    heap = HPQCreate(CmpInts, NULL, ARR_SIZE);

    for (i = 0; i < ARR_SIZE; ++i)
    {
        data = &arr[i];
        HPQEnqueue(heap, &data);    
    }

    FUNC_TO_TEST(HPQPeek);
    REQUIRE(NULL != HPQPeek(heap));
    LOGSTR("Top element after enqueuing:");
    LOGINT(VoidToInt(HPQPeek(heap)));

    HPQDestroy(heap);
    return (PASSED);
}

/* Test dequeuing elements from the heap */
static test_status_t TestHPQDequeue(void)
{
    heap_ty* heap;
    size_t i = 0;
    void* data = NULL;
    int arr[ARR_SIZE] = {0};

    InitRandomArray(arr, ARR_SIZE);
    heap = HPQCreate(CmpInts, NULL, ARR_SIZE);

    for (i = 0; i < ARR_SIZE; ++i)
    {
        data = &arr[i];
        HPQEnqueue(heap, &data);    
    }

    FUNC_TO_TEST(HPQDequeue);
    LOGSTR("Dequeuing elements:");
    for (i = 0; i < ARR_SIZE / 2; ++i)
    {
        LOGINT(VoidToInt(HPQPeek(heap)));
        HPQDequeue(heap);
    }

    REQUIRE(HPQSize(heap) == ARR_SIZE / 2);
    LOGSTR("SIZE after dequeuing half:");
    LOGLONG(HPQSize(heap));

    HPQDestroy(heap);
    return (PASSED);
}

/* Test removing a specific element from the heap */
static test_status_t TestHPQRemove(void)
{
    heap_ty* heap;
    size_t i = 0;
    int arr[ARR_SIZE] = {0};
    void* data = NULL;
    void* removed_element = NULL;

    InitRandomArray(arr, ARR_SIZE);
    heap = HPQCreate(CmpInts, NULL, ARR_SIZE);

    for (i = 0; i < ARR_SIZE; ++i)
    {
        data = &arr[i];
        HPQEnqueue(heap, &data);    
    }

    FUNC_TO_TEST(HPQRemove);
    {
        int to_remove = arr[2];  /* Example element to remove */
        removed_element = HPQRemove(heap, IsMatch, &to_remove);
        REQUIRE(NULL != removed_element);
        LOGSTR("Removed element:");
        LOGINT(VoidToInt(removed_element));
    }

    REQUIRE(HPQSize(heap) == ARR_SIZE - 1);
    LOGSTR("SIZE after removal:");
    LOGLONG(HPQSize(heap));

    HPQDestroy(heap);
    return (PASSED);
}

/* Test destroying the heap */
static test_status_t TestHPQDestroy(void)
{
    heap_ty* heap;
    heap = HPQCreate(CmpInts, NULL, ARR_SIZE);

    FUNC_TO_TEST(HPQDestroy);
    HPQDestroy(heap);

    LOGSTR("Heap destroyed successfully");
    return (PASSED);
}

/*--------------------------------------------------------------------------------------------*/
/* Helper Functions */

static int CmpInts(const void* data1_, const void* data2_, const void* param_)
{
    (void)param_;  /* Ignore unused param */
    return (*(int*)data1_ - *(int*)data2_);
}

static int VoidToInt(const void* ptr)
{
    return *(int*)ptr;
}

static int IsMatch(const void* data_, const void* param_)
{
    return (*(int*)data_ == *(int*)param_);
}

static void InitRandomArray(int arr[], int size)
{
    int i = 0;
    srand((unsigned int)time(NULL));
    for (i = 0; i < size; ++i)
    {
        arr[i] = rand() % size;
    }
}
