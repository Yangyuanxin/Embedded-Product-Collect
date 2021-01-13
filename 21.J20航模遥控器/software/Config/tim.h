#ifndef __TIM_H
#define __TIM_H
#include "stm32f10x.h"
#include "sys.h"

extern volatile u32 nowTime;//程序运行时间，单位10us
extern volatile u32 clockTime;//闹钟定时时间，单位10us
void TIM4_Counter_Init(u16 arr, u16 psc);
void drawClockTime(void);
#endif
