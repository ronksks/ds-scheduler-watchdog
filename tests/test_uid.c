/* Date: 18/7/2024, Author: Ron Yalensky, Reviewer: Oded Pisher*/

#include "uid.h"
#include <stdio.h>	/*	printf	*/
#include <string.h>	/*	strlen	*/



static void PrintUid(uid_ty uid);

int main()
{
	uid_ty uid1 = UIDCreate();
	uid_ty uid2 = UIDCreate();
	
	/*
	PrintUid(uid1);
	PrintUid(uid2);
	*/


	if(UIDIsEqual(uid1,uid2))
	{
		printf("Error in UIDIsEqual");
	}
	else
	{

		printf("Test success! UIDCreate UIDIsEqual \n");

	}

	return (0);

}


static void PrintUid(uid_ty uid)
{
    char * time_str = ctime(&uid.time);
    time_str[strlen(time_str)-1] = '\0';
    
    printf("PID: %d\n", uid.pid);
    printf("Address: %s\n", uid.ip);
    printf("Current Time : %s\n", time_str);
    printf("Count: %ld\n", uid.counter);

}
