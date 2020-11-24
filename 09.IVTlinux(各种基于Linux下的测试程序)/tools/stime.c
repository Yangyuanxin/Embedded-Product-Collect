#include "stime.h"

int getfulltime(unit_tradetime * fulltime)
{
    time_t now;
    struct tm *tnow;
    now = time(0);
    tnow= localtime(&now);
    //  printf("nowtime is %d %d %d %d %d %d\n", 1900+tnow->tm_year,tnow->tm_mon+1,tnow->tm_mday,tnow->tm_hour,tnow->tm_min,tnow->tm_sec);
    fulltime->date.yearh=inttobcd((1900+tnow->tm_year)/100);
    fulltime->date.yearl=inttobcd((1900+tnow->tm_year)%100);
    fulltime->date.month=inttobcd(tnow->tm_mon+1);
    fulltime->date.day=inttobcd(tnow->tm_mday);
    fulltime->hour=inttobcd(tnow->tm_hour);
    fulltime->minute=inttobcd(tnow->tm_min);
    fulltime->second=inttobcd(tnow->tm_sec);
    return 1;
}

/*******************************************************************************
* 返回自系统开机以来的毫秒数（tick）
* 编译需要添加-lrt
*******************************************************************************/
ST_ULONG getTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1000 + ts.tv_nsec/1000000);
}

