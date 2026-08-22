/* Date: 18/7/2024, Author: Ron Yalensky, Reviewer: Oded Pisher */
#ifndef _ILRD_UID_H_
#define _ILRD_UID_H_

#include <sys/types.h> /* size_t, getifaddrs*/
#include <unistd.h>	   /* pid_t				*/
#include <time.h> 	   /* time_t			*/


/* This is liable to change and is not part of the API, use at your own risk */
typedef struct ilrd_uid
{
    pid_t pid;
    time_t time;
    size_t counter;
    unsigned char ip[14];
}uid_ty;

/* Used to indicate errors */
extern const uid_ty bad_uid;

/*******************************************************************************
 * Provides a Unique Identifier.
 * Return Value: The new UID or "bad_uid" if an error occured.
 * Notes:        
 * Complexity: O(1)
 * ****************************************************************************/
uid_ty UIDCreate(void);

/*******************************************************************************
 * Checks if uid1 and uid2 are equal.
 * Return Value: "true" (1) or "false" (0).
 * Notes:        
 * Complexity: O(1)
 * ****************************************************************************/
int UIDIsEqual(uid_ty uid1, uid_ty uid2);

#endif /*_ILRD_UID_H_*/
