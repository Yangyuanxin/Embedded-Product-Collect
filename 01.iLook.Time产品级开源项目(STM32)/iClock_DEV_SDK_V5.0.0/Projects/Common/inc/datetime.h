#ifndef __DATETIME_H
#define __DATETIME_H

typedef struct{
    unsigned char sec;
    unsigned char min;
    unsigned char hour;
    unsigned char wday;
    unsigned int mday;
    unsigned int month;
    unsigned int year;
    unsigned long tick;
}tm;

char IsLeapYear(unsigned int uiYear);
tm *sys_time(void *p);
char *tm2str(tm *set_t);
void UTCTime2LocalTime(tm *l_time, tm *utc_time, int iTimeZone);
int CalDays(tm *t1, tm *t2);
long CalSeconds(tm *t1, tm *t2);

#endif

