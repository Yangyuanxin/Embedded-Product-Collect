#ifndef _BL8025_H_
#define _BL8025_H_

#include "sys.h"



#define BL8025_ADD				0x64

#define BL8025_SECOND_ADD		0x00
#define BL8025_MINUTE_ADD		0x01
#define BL8025_HOUR_ADD			0x02
#define BL8025_WEEK_ADD			0x03
#define BL8025_DAY_ADD			0x04
#define BL8025_MONTH_ADD		0x05
#define BL8025_YEAR_ADD			0x06

extern uint8_t SetTime_Hour;
extern uint8_t SetTime_Minute;
extern uint8_t SetTime_Second;

extern uint8_t SetDate_Year;
extern uint8_t SetDate_Month;
extern uint8_t SetDate_Day;
extern uint8_t SetDate_Week;

extern uint8_t Current_Year;
extern uint8_t Current_Month;
extern uint8_t Current_Day;
extern uint8_t Current_Week;
extern uint8_t Current_Hour;
extern uint8_t Current_Minute;
extern uint8_t Current_Second;

extern uint8_t u8_TrickleCurrentTimeBuf1_8[8];
extern uint8_t u8_TrickleCurrentTimeBuf9_16[8];
extern uint8_t u8_TrickleCurrentTimeBuf17_24[8];
extern uint8_t u8_TrickleCurrentTimeBuf25_32[8];
extern uint8_t u8_JitterRunTimeBuf[32];

void BL8025_Init(void);
void BL8025WriteOneByte(uint8_t add,uint8_t dat);
uint8_t BL8025ReadOneByte(uint8_t add);


void BL8025SetDate(void);
void BL8025SetTime(void);
void BL8025ReadDateTime(void);
void BL8025_Process(void);
void BL8025_TimerMS(void);













#endif
