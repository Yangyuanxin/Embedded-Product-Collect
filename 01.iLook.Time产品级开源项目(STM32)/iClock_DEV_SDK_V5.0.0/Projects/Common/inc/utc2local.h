#ifndef _UTC_2_LOCAL_H_
#define _UTC_2_LOCAL_H_

typedef struct _LOCAL_TIME 
{
	unsigned short	Year;
	unsigned short	Month;
	unsigned short	Date;
	unsigned short	DayOfWeek;
	unsigned short	Hour;
	unsigned short	Minute;
	unsigned short	Second;
}LOCAL_TIME,*P_LOCAL_TIME;
void Utc2LocalTime(unsigned int TIME,P_LOCAL_TIME pTime);
unsigned char IsRunNian(unsigned int year);
unsigned int  LocalTime2UTC(P_LOCAL_TIME ptTime);
#endif