#ifndef __MY_DELAY_H
#define __MY_DELAY_H

#include "sys.h"

#define		DELAY_MAX_VALUE		5000


void 	TimerDelay_Ms(u16 counter);
void 	RunDelayCounter(void);
void 	Clear(void);
u16 	GetCounterValue();

#endif