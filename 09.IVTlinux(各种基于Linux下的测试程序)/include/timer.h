
#ifndef __TIMER_H__
#define __TIMER_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/time.h>
#include <bits/time.h>
#include <pthread.h>
#include <signal.h>
#include "glabtypes.h"

#define MAX_SYS_TIMERS 					20
/*select 精度100ms,经测试使用100ms比较准 */
#define  MIN_SYS_CLOCK_TIME	           	100    /*最小时间片，系统时钟可以设置的最大频率 */

#define CRITICAL pthread_mutex_t
#define CRITICAL_OPEN(x) pthread_mutex_init(x,NULL);
#define CRITICAL_CLOSE pthread_mutex_destroy
#define CRITICAL_LOCK pthread_mutex_lock
#define CRITICAL_UNLOCK pthread_mutex_unlock

typedef BOOL (*SYS_TIMER_HANDLER)(void* lpParameter,int nParamLen);

int sysTimerInit(void);
int trSysTimerAlloc(SYS_TIMER_HANDLER fn,void* context,int nParamLen,int nTimeOut);
BOOL trSysTimerStart(int nTimer, BOOL bLoop, int nLoopTimes);
BOOL trSysTimerCancel(int nTimer);
BOOL trSysTimerReset(int nTimer, int nTimeOut);
BOOL trSysTimerFree(int *nTimer);



#endif
