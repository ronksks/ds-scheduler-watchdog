#include "../../utils/test.h"
#include "task.h"



/*---------------FUNCTION DECLERATION---------------------*/

static test_status_t StageOneTest(void);
int OneTimeTask(void *param);
/*-------------------------MAIN---------------------------*/


int main()
{
	RUNTEST(StageOneTest);

	return (PASSED); 
}

/*--------------------------------------------------------------------------------------------*/

static test_status_t StageOneTest(void)
{
	int num = 5;
	task_ty *new_task1 = TaskCreate(&OneTimeTask, 15, &num);
	task_ty *new_task2 = TaskCreate(&OneTimeTask, 40, &num);
	
	FUNC_TO_TEST(TaskCreate);
	FUNC_TO_TEST(TaskExecute);
	FUNC_TO_TEST(TaskDestroy);
	FUNC_TO_TEST(TaskGetExecuteTime);
	FUNC_TO_TEST(TaskUpdateExecuteTime);
	FUNC_TO_TEST(TaskGetUID);

	REQUIRE(1 == TaskExecute(new_task1));
	REQUIRE(TaskGetExecuteTime(new_task1) < TaskGetExecuteTime(new_task2));
	REQUIRE(!UIDIsEqual(TaskGetUID(new_task1),TaskGetUID(new_task2)));
	REQUIRE(!UIDIsEqual(TaskGetUID(new_task1),bad_uid));


	REQUIRE(_SUCCESS_ ==  TaskUpdateExecuteTime(new_task1));

	REQUIRE(TaskGetExecuteTime(new_task1) < TaskGetExecuteTime(new_task2));

	TaskDestroy(new_task1);
	TaskDestroy(new_task2);

	return (PASSED);
}

int OneTimeTask(void *param)
{
	(void)param;

	return (1);
}