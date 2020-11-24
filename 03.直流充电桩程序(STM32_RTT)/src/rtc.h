#ifndef RTC_H_
#define RTC_H_


typedef struct
{
	unsigned short year;
	unsigned char  mon;
	unsigned char  day;
	unsigned char  hour;
	unsigned char  min;
	unsigned char  sec;
	unsigned char  week;
}time_t;



unsigned char TIME_Get(time_t *time);


void RTC_Init(void);

#endif
