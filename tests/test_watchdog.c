		
/* META DATA
Date: 08/10/2024
Author: Ron Yalensky
Reviewer:
Description: Test file for Watch Dog project. */

/* Header Files Used */

#include <assert.h> /* assert */
#include <unistd.h> /*  getpid */
#include "watchdog.h"
#include "test.h"
#include "utils.h"

/*---------------FUNCTION DECLARATION---------------------*/

static test_status_t TestFunc(void);

/*-------------------------MAIN---------------------------*/

int main(int argc, char** argv)
{
    RUNTEST(TestFunc);

    FUNC_TO_TEST(MakeMeImmortal);

    REQUIRE(SUCCESS == MakeMeImmortal(3, 7,argc, argv ));
    LOGVAR("%d: I am immortal!", getpid());
    LOGSTR("working on something");
    sleep(10);
    LOGSTR("finished working on something");
    LOGVAR("%d: asked DoNotResuscitate", getpid());

    DoNotResuscitate();
    LOGSTR("finished DNR");

    return (PASSED); 

    
}

/*--------------------------------------------------------------------------------------------*/

 static test_status_t TestFunc(void)
{

    return (PASSED);
}
