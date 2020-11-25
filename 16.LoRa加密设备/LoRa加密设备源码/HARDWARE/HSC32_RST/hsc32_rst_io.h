#ifndef __HSC32_RST_IO_H
#define __HSC32_RST_IO_H	

#include "sys.h"
#include "delay.h"

#define		HSC32IO_RCC			RCC_APB2Periph_GPIOB
#define		HSC32IO_PORT		GPIOB
#define		HSC32IO_PIN			GPIO_Pin_5
#define  	HSC_RST_PIN			PBout(5)


void 	HSC32RstInit(void);
void 	RestHSC32Module(void);

#endif
