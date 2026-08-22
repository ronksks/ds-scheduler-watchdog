/* Date: 18/7/2024, Author: Ron Yalensky, Reviewer: Oded Pisher*/

/* Loop and take first valid IP */
/*ifaddr_cur->ifa_addr->sa_data*/

#include <arpa/inet.h>  /*  inet_ntoa    */
#include <sys/socket.h> /*  sockaddr_in  */
#include <ifaddrs.h>    /*  getifaddrs   */
#include <time.h>       /*  time         */
#include <string.h>     /*  strcpy       */
#include <stdio.h>

#include <stdatomic.h>	/* atomic_size_t */

#include "uid.h"

#define _TRUE_      (1)
#define _FALSE_     (0)
#define _SUCCESS_   (0)
#define _FAILED_    (1)


const uid_ty bad_uid = {0};

static char *GetIp();

uid_ty UIDCreate(void)
{
    static volatile atomic_size_t counter = 1;

    uid_ty uid = bad_uid;

    char *ip = GetIp();

    if(!ip)
    {
        return(bad_uid);
    }

    uid.pid = getpid();

    strcpy((char *)uid.ip,ip);

    uid.time = time(NULL);

    uid.counter = __atomic_fetch_add(&counter, 1, __ATOMIC_SEQ_CST);

    return (uid);
}

int UIDIsEqual(uid_ty uid1, uid_ty uid2)
{
    if(uid1.counter != uid2.counter || uid1.time != uid2.time || uid1.pid != uid2.pid)
    {
    
        return (_FALSE_);
    
    }

    if (strcmp((char *)uid1.ip, (char *)uid2.ip))
    {
        return (_FALSE_);
    }

    return(_TRUE_);
}


static char *GetIp()
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;
    int found = 0;

    if (-1 == getifaddrs (&ifap))
    {
        return (NULL);
    }


    for (ifa = ifap; ifa && !found; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) 
        {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            found = 1;
        }
    }

    freeifaddrs(ifap);

    if (!found) 
    {
        return (NULL);
    }

    return(addr);
}