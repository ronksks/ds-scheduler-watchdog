/* META DATA
Date: 08/10/2024
Author: Ron Yalensky
Reviewer:
Description: Header file for Watch Dog project. */

#ifndef _ILRD_WATCH_DOG_H_
#define _ILRD_WATCH_DOG_H_

/* Header Files Used */
typedef struct watchdog_ty* wd;
/* Watch Dog API */
int MakeMeImmortal(int seconds_between_SOL, int max_missed_SOL, int argc, char** argv);

void DoNotResuscitate(void);


#endif /* _ILRD_WATCH_DOG_H_ */
