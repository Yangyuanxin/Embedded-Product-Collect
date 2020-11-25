#ifndef __ALAM_H
#define	__ALAM_H

#include "modules_init.h"

#define		ALAM_RCC		RCC_APB2Periph_GPIOA
#define		ALAM_PORT		GPIOA
#define		ALAM_PIN		GPIO_Pin_8
#define		ALAM_KEY		PAin(8)


void 	AlamKeyPinInit(void);
void 	ScanAlamKey(void);

#endif