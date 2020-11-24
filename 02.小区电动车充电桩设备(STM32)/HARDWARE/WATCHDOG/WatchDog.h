#ifndef	_WATCHDOG_H
#define	_WATCHDOG_H

#include "sys.h"  


void WatchDogGPIO_Init(void);
void WatchDogGPIO_Enable(void);
void WatchDogGPIO_Disable(void);
void WatchDog_Feed(void);





#endif
