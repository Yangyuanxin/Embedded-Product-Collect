#ifndef __ADVANCE_TIMER_H
#define __ADVANCE_TIMER_H

#include "sys.h"
#include "led.h"
//#include "rtc.h"
#include "com_status_key_init.h"


void 	AdvanceTimerInit(u8 WhichTimer, u16 period, u16 prescaler);
void 	SetAdvanceTimerState(u8 which,FunctionalState state);

#endif