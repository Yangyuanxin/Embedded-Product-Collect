#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#include "sys.h"

#define		WATCHDOG_RCC		RCC_APB2Periph_GPIOA
#define		WATCHDOG_PORT		GPIOA
#define		WATCHDOG_PIN		GPIO_Pin_0
#define		WATCH_DOG			PAout(0)
#define		FEED_DOG_CYCLE		200


void 	WatchDogPinInit(void);
void 	RunWatchDogTimer(void);
void 	WatchDogHandle(void);

#endif

