#include "datetime.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"

static char time_str[40];
static tm local_time;

/*
函数名称: time

函数原型: time_t sys_time(time_t *timer)
函数功能: 得到系统当前时间,或设定系统当前时间
函数返回: 系统当前时间
参数说明: t=NULL时得到机器日历时间，t为有效指针时，更新t为系统当前时间，t是一个tm类型
*/
tm *sys_time(void *p)
{
    tm *t = (tm *)p;
    
    if(t==NULL)
        return &local_time;

    local_time.year = t->year;
    local_time.month = t->month;
    local_time.mday = t->mday;
    local_time.hour = t->hour;
    local_time.min = t->min;
    local_time.sec = t->sec;
	local_time.wday = t->wday;
    
    local_time.tick = t->hour*3600+t->min*60+t->sec;
    
    return &local_time;
}

//"YYYY-MM-DDThh:mm:ss"
char *xml_time(void *set_t)
{
    tm *t = (tm *)set_t;

    if(t==NULL){
        t = &local_time;
        sprintf(time_str,"%04d%s%02d%s%02d%s%02d%s%02d%s%02d",
        t->year,"-",t->month,"-",t->mday,
        "T",
        t->hour,":",t->min,":",t->sec
        );
    }
    return time_str;
}

char *tm2str(tm *time)
{
    tm *t = (tm *)time;

    sprintf(time_str,"%04d%s%02d%s%02d%s%02d%s%02d%s%02d",
    t->year,"-",t->month,"-",t->mday,
    "T",
    t->hour,":",t->min,":",t->sec
    );

    return time_str;
}
   
void UTCTime2LocalTime(tm *l_time, tm *utc_time, int iTimeZone)
{
    int iHourInterval,iInterval,iDays=0;
    tm lc_time;
    
    memset(&lc_time,0,sizeof(tm));   
    memcpy(&lc_time, utc_time, sizeof(tm)); 


    //将UTC时间加上本地时区小时数   
    lc_time.hour += iTimeZone;
   
    //根据加上时区后的时间刷新本地时间   
    //根据小时数判断日期是否需要进位   
    iHourInterval = lc_time.hour/24;
    
    lc_time.hour %= 24;   
    if(iHourInterval>0)   
    {   
        //小时数已经大于24，日期需要加1   
        lc_time.mday += 1;   
   
        //小时数加1后继续判断天数是否需要加1   
        //要根据当前月份判断   
        switch(lc_time.month)   
        {   
        case 1:   
        case 3:   
        case 5:   
        case 7:   
        case 8:   
        case 10:   
        case 12:   
            {   
                iDays = 31;   
            }   
            break;   
        case 2:   
            {   
                //2月份比较特殊，需要根据是不是闰年来判断当月是28天还29天   
                iDays = IsLeapYear(utc_time->year)?29:28;   
            }   
            break;   
        case 4:   
        case 6:   
        case 9:   
        case 11:   
            {   
                iDays = 30;   
            }   
            break;   
        }   
   
        iInterval = lc_time.mday-iDays;
        if(iInterval>0)   
        {   
            lc_time.mday = iInterval;   
            lc_time.month += 1;   
   
            //判断月份是否大于12，如果大于12，则年份需要再加1   
            iInterval = lc_time.month/12;   
            lc_time.month %= 12;   
            if(iInterval>0)   
            {   
                lc_time.year += 1;   
            }   
        }   
    }
    
    memcpy(l_time, &lc_time, sizeof(tm)); 
   
    //printf("当前 UTC 日期时间： %04d-%02d-%02d %02d:%02d:%02d\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);   
    //printf("当前 本地日期时间： %04d-%02d-%02d %02d:%02d:%02d\n",lc_time.wYear,lc_time.wMonth,lc_time.wDay,lc_time.wHour,lc_time.wMinute,lc_time.wSecond);   
}   

//判断给定年是否闰年
/*  bool IsLeapYear(int iYear)
    {
	    if (iYear % 100 == 0)
		    return ((iYear % 400 == 0));
	    else
		    return ((iYear % 4 == 0));
    }
*/   
char IsLeapYear(unsigned int uiYear)   
{   
    return (((uiYear%4)==0)&&((uiYear%100)!=0))||((uiYear%400)==0);   
}


//判断给定日期是否合法
unsigned char mday_cnt[13]={0xff,31,28,31,30,31,30,31,31,30,31,30,31};

char IsValidDate(tm *t)
{
  char rslt = 0;
  
	if( t->year>9999 ) 
	  return 0;

	switch (t->month)
	{
  	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
  	  if( t->mday<=31 )  rslt = 1;
  		break;
  	case 4: case 6: case 9: case 11:
  	  if( t->mday<=30 )  rslt = 1;
  		break;
  	case 2:
  	  if( t->mday <= (IsLeapYear(t->year) ? 29 : 28) )
  	    rslt = 1;
  		break;
  	default:
  		return 0;
	}
	return rslt;
}

__inline void swap_int(unsigned int *a, unsigned int *b)
{
  *a ^= *b;
  *b ^= *a;
  *a ^= *b;
}

//获取2个日期之间相差的天数

int CalDays(tm *tm1, tm *tm2)
{
  int k;
  int iDays = 0;
  int i;
  tm T1,T2;
  tm *t1=&T1,*t2=&T2;
  
  //
  memcpy( &T1,tm1,sizeof(tm) );
  memcpy( &T2,tm2,sizeof(tm) );
	//1. 确保 日期1 < 日期2

	if ((t1->year * 10000 + t1->month * 100 + t1->mday)>(t2->year * 10000 + t2->month * 100 + t2->mday))
	{
	  k = -1;
	  swap_int(&t1->year,&t2->year);
	  swap_int(&t1->month,&t2->month);
	  swap_int(&t1->mday,&t2->mday);
	}else
	  k = 1;
	
	//2. 开始计算天数
	//计算 从 iYear1年1月1日 到 iYear2年1月1日前一天 之间的天数
	for (i = t1->year; i < t2->year; i++)
	{
		iDays += (IsLeapYear(i) ? 366 : 365);
	}
	
	//减去iYear1年前iMonth1月的天数
	for (i = 1; i < t1->month; i++)
	{
		if( (i==2)&&IsLeapYear(t2->year) )
		  iDays -= 29;
		else
		  iDays -= mday_cnt[i];
	}
	//减去iYear1年iMonth1月前iDay1的天数
	iDays -= (t1->mday - 1);
	
	//加上iYear2年前iMonth2月的天数
	for (i = 1; i < t2->month; i++)
	{
		if( (i==2)&&IsLeapYear(t2->year) )
		  iDays += 29;
		else
		  iDays += mday_cnt[i];
	}
	//加上iYear2年iMonth2月前iDay2的天数
	iDays += (t2->mday - 1);
	
	return iDays*k;
}


//返回相差的秒数
long CalSeconds(tm *t1, tm *t2)
{
  if( (t1->year!=t2->year)||(t1->month!=t2->month)||(t1->mday!=t2->mday) ){
    return (CalDays(t1,t2)*24*60*60);
  }else
    return (t2->tick - t1->tick);
}


