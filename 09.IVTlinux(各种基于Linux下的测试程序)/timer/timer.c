#include "timer.h"

#define LOG_SYSTIMER 					printf
#define MAX_FUNC_ARG_LEN 				1024

/* 定时器数据结构 */
typedef struct _tagSysTimerList
{
	BOOL 				bIsUsed;                        /*定时器数据结构是否已被使用，false表示已未使用*/     
	BOOL 				bLoop;                          /*定时器是否一直循环,false表示不循环,执行次数有限*/
	int  				nInterval;                     	/*定时器的定时时间，单位毫秒 */  
	int  				nTimediff;						/*与10MS 整数时间片的偏移次数*/
	SYS_TIMER_HANDLER   pFunction;          			/*定时服务处理函数指针 */
	char    			lpParameter[MAX_FUNC_ARG_LEN];  /*定时服务处理函数参数 */
	int    				nParamLen;                      /*定时服务处理函数参数的长度 */
	int    				nLoopTimes;                     /*循环的次数 0:循环停止，n > 0:循环n次.... */
	BOOL     			bEnable;						/*定时器的使能状态,能否调用回调 */
	int    				nIndexId;                       /*正在使用的定时器的定时器数组下标 */ 
} tSysTimer;

/* 全局变量 */
typedef struct _tagSysTimer
{
	BOOL   bTimerInit;						/*初始化运行标志 */    
	int    nSysClockTime;              		/*定时器的最小定时时间，单位毫秒*/
	int    nTimers;                    		/*定时器模块使用的定时器数 */
	tSysTimer tTimer[MAX_SYS_TIMERS];   	/*定时器数据结构数组 */
	int    nFirstTimer;              		/*第一个使用的定时器 */
	int    nLastTimer;               		/*最后一个使用的定时器*/
	CRITICAL trCritical;                 	/*互斥量定义*/
} sysTimer_t;

static sysTimer_t  gSystimer;

/** \fn BOOL sysTimerRelease(INT32 nIndex)
 *  \brief  从定时器链表启用的定时器最大最小下标
 */
void setSysTimerMinMaxIndex()
{
	int index = 0 , nMin = -1 , nMax = 0;

	if(gSystimer.nTimers > 0)
	{
		for (index = 0;index < MAX_SYS_TIMERS;index++)
		{
			if(gSystimer.tTimer[index].bIsUsed)
			{
				if(nMin < 0)/*未付初值*/
				{
					nMin = index;
				}
				nMin = min(nMin ,index);
				nMax = max(nMax,index);
			}
		}	
	}
	else
	{
		nMin = 0;
		nMax = 0;
	}

	gSystimer.nFirstTimer = nMin;
	gSystimer.nLastTimer  = nMax;
}

/** \fn int systimerAdd(tSysTimer *pSysTimer)
 *  \brief  向定时器列表添加一个定时器。
 *  \param  pSysTimer    添加的定时器结构指针。
 *  \return 成功返回添加的定时器的下标，失败返回-1。
 */
int systimerAdd(tSysTimer *pSysTimer)
{
	int i = 0;
	
	for (i = 0; i < MAX_SYS_TIMERS; i++)
	{
		/* 判别定时器数组中那个定时器尚未使用 */
		if (!gSystimer.tTimer[i].bIsUsed)
		{
			memcpy(&gSystimer.tTimer[i], pSysTimer, sizeof(tSysTimer));	
			gSystimer.nTimers++;
			setSysTimerMinMaxIndex();
			return i;
		}
	}
	return -1;
}

/** \fn int trSysTimerAlloc(SYS_TIMER_HANDLER fn,
 *                               VOID* context,
 *                               INT32 nTimeOut)
 *  \brief  初始化定时器。
 *  \param  fn              定时的回调函数。
 *  \param  context         回调函数的参数。
 *  \param  nParamLen       参数长度 
 *  \param  nTimeOut        定时的时间(单位毫秒)。
 *  \return 成功返回定时器的标识，失败返回-1。
 */
int trSysTimerAlloc(SYS_TIMER_HANDLER fn,void* context,int nParamLen,int nTimeOut)
{
	int   nRet = 0;
	tSysTimer LSysTimer;

	nRet = MAX_SYS_TIMERS;
	
	if (!gSystimer.bTimerInit)
	{
		LOG_SYSTIMER("timer is not init, system can not alloc a timer!\n");
		return -1;
	}
	
	if (MIN_SYS_CLOCK_TIME >= nTimeOut)
	{
		LOG_SYSTIMER(" system can't achieve timer <10ms!\n");
		return -1;
	}
	
	CRITICAL_LOCK(&(gSystimer.trCritical));
	
	if (gSystimer.nTimers == MAX_SYS_TIMERS)
	{
		LOG_SYSTIMER("timer is reach MAX_SYS_TIMERS :%d, can not add more!\n", nRet);
		CRITICAL_UNLOCK(&(gSystimer.trCritical));
		return -1;
	}

	if (0 == gSystimer.nTimers)
	{
		gSystimer.nFirstTimer = 0;
		gSystimer.nLastTimer = 0;
	}

	/* 要设置的定时器数据结构初始化 */
	memset(&LSysTimer, 0, sizeof(tSysTimer));
	LSysTimer.bIsUsed = true;
	LSysTimer.nInterval = nTimeOut;
	LSysTimer.nTimediff = (nTimeOut/MIN_SYS_CLOCK_TIME);
	LSysTimer.pFunction = fn;
	if(nParamLen > 0)
	{
		memcpy(LSysTimer.lpParameter,context, nParamLen);
	}	
	LSysTimer.nParamLen = nParamLen;

	nRet = systimerAdd(&LSysTimer);		
	if (-1 == nRet)
	{
		LOG_SYSTIMER("timer is full!\n");
	}	
	CRITICAL_UNLOCK(&(gSystimer.trCritical));

	return nRet;
}

/** \fn BOOL trSysTimerStart(int nTimer, BOOL bLoop, int nLoopTimes)
 *  \brief  启动定时器。
 *  \param  nTimer      启动的定时器标识。
 *  \param  bLoop       定时器是否循环。
 *  \param  nLoopTimes  循环次数，0:停止，n > 0:循环n次。
 *  \return 成功返回TRUE，失败返回FALSE。
 */
BOOL trSysTimerStart(int nTimer, BOOL bLoop, int nLoopTimes)
{
	if (nTimer < 0 || nTimer >= MAX_SYS_TIMERS)
	{
		LOG_SYSTIMER("trSysTimerStart timer[%d] is not existed!\n", nTimer);
		return false;
	}
	
	CRITICAL_LOCK(&(gSystimer.trCritical));
	if (!gSystimer.tTimer[nTimer].bIsUsed) 
	{
		LOG_SYSTIMER("timer is not init, can not start!\n");
		CRITICAL_UNLOCK(&(gSystimer.trCritical));
		return false;
	}
	
	/* 设置循环标志位 */
	if (bLoop)
	{
		gSystimer.tTimer[nTimer].bLoop = true;
	}
	else
	{
		gSystimer.tTimer[nTimer].bLoop = false;
		/* 设置循环次数 */     
		if(nLoopTimes > 0)
		{
		   	gSystimer.tTimer[nTimer].nLoopTimes = nLoopTimes; 
		}
		else
		{
			LOG_SYSTIMER("Loop timer is negative!\n");
		}
	}
	/* 设置启动标志位 */	
	gSystimer.tTimer[nTimer].bEnable = true;
	CRITICAL_UNLOCK(&(gSystimer.trCritical));
	return true;
}


/** \fn BOOL trSysTimerStart(int nTimer, BOOL bLoop, INT32 nLoopTimes)
 *  \brief  暂停定时器。
 *  \param  nTimer      启动的定时器标识。
 *  \return 成功返回TRUE，失败返回FALSE。
 */
BOOL trSysTimerCancel(int nTimer)
{
	if (nTimer < 0 || nTimer >= MAX_SYS_TIMERS)
	{
		LOG_SYSTIMER("trSysTimerCancel timer[%d] is not existed!\n", nTimer);
		return false;
	}
	
	CRITICAL_LOCK(&(gSystimer.trCritical));
	if (!gSystimer.tTimer[nTimer].bIsUsed) 
	{
		LOG_SYSTIMER("trSysTimerCancel timer is not init, can not cancal!\n");
		CRITICAL_UNLOCK(&(gSystimer.trCritical));
		return false;
	}
	
	/* 设置启动暂停标志位 */	
	gSystimer.tTimer[nTimer].bEnable = false;
	CRITICAL_UNLOCK(&(gSystimer.trCritical));
	return true;
}


/** \fn BOOL trSysTimerReset(int nTimer, int nTimeOut)
 *  \brief  重新开始定时器的计时。
 *  \param  nTimer      需要重起的定时器标识。
 *  \return 成功返回TRUE，失败返回FALSE。
 */
BOOL trSysTimerReset(int nTimer, int nTimeOut)
{
	if (nTimer < 0 || nTimer >= MAX_SYS_TIMERS)
	{
		LOG_SYSTIMER("timer[%d] is not existed trSysTimerReset!\n", nTimer);
		return false;
	}
	/* 设置定时器计数器的值为初始值 */
	CRITICAL_LOCK(&(gSystimer.trCritical));
	if(nTimeOut>MIN_SYS_CLOCK_TIME)
	{	
		gSystimer.tTimer[nTimer].nInterval = nTimeOut;
		gSystimer.tTimer[nTimer].nTimediff = nTimeOut%MIN_SYS_CLOCK_TIME;
	}
	CRITICAL_UNLOCK(&(gSystimer.trCritical));
	return true;
}

/** \fn BOOL sysTimerRelease(INT32 nIndex)
 *  \brief  从定时器链表删除一个定时器。
 *  \param  nIndex      需删除的定时器的下标。
 *  \return 成功返回TRUE，失败返回FALSE。
 */
BOOL sysTimerRelease(int nIndex)
{
	if(gSystimer.nTimers<=0)
	{
		return false;
	}
	/* 判别要删除的定时器是否存在 */
	if (!gSystimer.tTimer[nIndex].bIsUsed)
	{
		return false;
	}

	gSystimer.tTimer[nIndex].bIsUsed = false;
	gSystimer.nTimers--;

	/*需要重新设置第一个使用的定时器和最后一个使用的定时器*/
	setSysTimerMinMaxIndex();
	return true;
}

/** \fn BOOL trSysTimerFree(int nTimer)
 *  \brief  释放定时器。
 *  \param  nTimer      释放的定时器标识。
 *  \return 成功返回TRUE，失败返回FALSE。
 */
BOOL trSysTimerFree(int *nTimer)
{
	BOOL    bRet;
	
	CRITICAL_LOCK(&(gSystimer.trCritical));
	if(*nTimer < 0 || *nTimer >= MAX_SYS_TIMERS)
	{
		CRITICAL_UNLOCK(&(gSystimer.trCritical));
		return false;
	}
	/* 释放一个定时器 */
	bRet = sysTimerRelease(*nTimer);
	*nTimer = -1;
	CRITICAL_UNLOCK(&(gSystimer.trCritical));	
	return bRet;
}

void printSysTimer()
{
	int    i = 0,j = 0;
	
	LOG_SYSTIMER("SystimerLib have %d timers.\n", gSystimer.nTimers);
	
	for (i = 0; i < MAX_SYS_TIMERS; ++i)
	{
		if (gSystimer.tTimer[i].bIsUsed)
		{
			LOG_SYSTIMER("Timer[%d] index = %d nInterval=%d nIndexId=%d\n",j,i,gSystimer.tTimer[i].nInterval,gSystimer.tTimer[i].nIndexId);
			j++;
		}
	}
}

/*定时器处理线程*/
void* trSysTimerThread(void* param)
{
	struct timeval tv,tv_old,tv_now;
	int nIndex=0,nTimers=0,expires=0,time_diff=0,nSec=0,nUsec=0;
	
	nSec = gSystimer.nSysClockTime/1000;
	nUsec = 1000*(gSystimer.nSysClockTime%1000);
	gettimeofday(&tv_old, 0);
	gettimeofday(&tv_now, 0);
	
	while(1)
	{
		if(gSystimer.bTimerInit)
		{
			time_diff = 0;
			nTimers = gSystimer.nTimers;
			if (nTimers > 0)
			{
				/*增加执行的精度*/
				gettimeofday(&tv_now, 0);
				expires = ((tv_now.tv_sec-tv_old.tv_sec)*1000000+(tv_now.tv_usec-tv_old.tv_usec));
				/*程序执行的时间*/
				if(expires > gSystimer.nSysClockTime*3*1000)
				{
					time_diff = expires/(gSystimer.nSysClockTime);
				}
				CRITICAL_LOCK(&(gSystimer.trCritical));
				for(nIndex=gSystimer.nFirstTimer;nIndex<=gSystimer.nLastTimer;nIndex++)/*定时器列表*/
				{
					if((gSystimer.tTimer[nIndex].bEnable)&&(gSystimer.tTimer[nIndex].bIsUsed))/*有效的定时器*/
					{
						if(time_diff>0)/*程序执行时间或其他原因，造成时间差别太大*/
						{
							gSystimer.tTimer[nIndex].nTimediff -= time_diff;
						}
						else
						{
							gSystimer.tTimer[nIndex].nTimediff--;
						}
						if(0>=gSystimer.tTimer[nIndex].nTimediff)/*定时到时*/
						{
							gSystimer.tTimer[nIndex].nTimediff = gSystimer.tTimer[nIndex].nInterval/(gSystimer.nSysClockTime);	
							if(gSystimer.tTimer[nIndex].bLoop)/*一直循环*/
							{
								gSystimer.tTimer[nIndex].pFunction(gSystimer.tTimer[nIndex].lpParameter,gSystimer.tTimer[nIndex].nParamLen);
							}
							else if(gSystimer.tTimer[nIndex].nLoopTimes > 0)/*循环次数*/
							{
								if(true==gSystimer.tTimer[nIndex].pFunction(gSystimer.tTimer[nIndex].lpParameter,gSystimer.tTimer[nIndex].nParamLen))
								{
									gSystimer.tTimer[nIndex].nLoopTimes--;
								}
							}
							else
							{
								sysTimerRelease(nIndex);/*从定时器列表里删除*/
							}
						}
					}
					
				}
				CRITICAL_UNLOCK(&(gSystimer.trCritical));
				tv_old.tv_sec = tv_now.tv_sec;
				tv_old.tv_usec = tv_now.tv_usec;
				
			}	
		}
		
		tv.tv_sec = nSec;
		tv.tv_usec = nUsec;
		if(-1 == select(0, NULL, NULL, NULL, &tv))
		{
			perror("sysclockmain select");
			continue;		
		}
		
	}
	pthread_exit(NULL);
	
	return NULL;
}

int sysTimerInit(void)
{
	int ret;
	pthread_t pthrSysTimer;

	if (gSystimer.bTimerInit)
	{
		return false;
	}
	gSystimer.nSysClockTime = MIN_SYS_CLOCK_TIME;
	/* 创建定时器模块需要的互斥量   */
	CRITICAL_OPEN(&(gSystimer.trCritical));	
	/* create the systimer thread */
	ret = pthread_create(&pthrSysTimer, NULL, trSysTimerThread, NULL);
	if (ret)
	{
		printf("ERROR; return code from pthread_create() is:%d\n", ret);
		exit(-1);
	}
	gSystimer.bTimerInit = true;
	return 0;
}

#if 0

#define TIMER_TEST
#ifdef TIMER_TEST
unsigned int getTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec/1000000);
}

BOOL SLSysTimerLog1(void * lpParam,int nParamLen);
BOOL SLSysTimerLog2(void *lpParam,int nParamLen);
BOOL SLSysTimerLog3(void *lpParam,int nParamLen);

int systimerindex1;
int systimerindex2;
int systimerindex3;

BOOL SLSysTimerLog1(void * lpParam,int nParamLen)
{
	static unsigned int now;
	LOG_SYSTIMER("SLSysTimerLog1 %u\n",getTickCount()-now);
	now = getTickCount();
	return true;
}

BOOL SLSysTimerLog2(void *lpParam,int nParamLen)
{
	static unsigned int now;
	char buf[1024] = {0};

	memcpy(buf,lpParam,nParamLen);
	LOG_SYSTIMER("SLSysTimerLog2 %u nParamLen(%d) param:%s\n",getTickCount()-now,nParamLen,buf);
	now = getTickCount();
	return true;
}

BOOL SLSysTimerLog3(void *lpParam,int nParamLen)
{
	static int times;
	static unsigned int now;

	times++; 
	LOG_SYSTIMER("times %d SLSysTimerLog3 %u\n",times,getTickCount()-now);
	now = getTickCount();
	usleep(3000);
	return true;
}

void setsystimer1(void)
{
	systimerindex1 = trSysTimerAlloc((SYS_TIMER_HANDLER)SLSysTimerLog1,0,0,500);
	trSysTimerStart(systimerindex1, 1, 0);
}

void setsystimer2(void)
{
	char string[] = "setsystimer2testmytimer";
	
	systimerindex2 = trSysTimerAlloc((SYS_TIMER_HANDLER)SLSysTimerLog2,(void*)string ,sizeof(string), 600);
	trSysTimerStart(systimerindex2, 0, 10);
}

void setsystimer3(void)
{
	systimerindex3 = trSysTimerAlloc((SYS_TIMER_HANDLER)SLSysTimerLog3, 0, 0, 900);
	trSysTimerStart(systimerindex3, 0, 100);
}

int main()
{
	struct timeval tv;
	sysTimerInit();
	setsystimer1();
	setsystimer2();
	setsystimer3();
	while(1)
	{
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);
		trSysTimerReset(systimerindex1,1000);
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);
		trSysTimerCancel(systimerindex1);
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);
		trSysTimerStart(systimerindex1,1, 0);
		tv.tv_sec = 60;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);
		trSysTimerFree(&systimerindex1);
	}
	return 0;	
}

#endif
#endif
