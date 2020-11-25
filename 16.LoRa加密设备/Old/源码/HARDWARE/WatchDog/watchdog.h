#ifndef __WATCHDOG_H
#define __WATCHDOG_H
#include "sys.h"
 

 void IWDG_Init(u8 prer,u16 rlr);
 void IWDG_Feed(void);

#endif

