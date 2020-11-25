#ifndef __LORA_PWR_H
#define __LORA_PWR_H

#include "sys.h"

#define		LORA_PWR_RCC		RCC_APB2Periph_GPIOA
#define		LORA_PWR_PORT		GPIOA
#define		LORA_PWR_PIN		GPIO_Pin_3


//#define		LORA_PWR_PIN		PAout(3)
//#define		LORA_EN()			{ LORA_PWR_PIN=1; }


void LoRaPWR_Init(void);

#endif



