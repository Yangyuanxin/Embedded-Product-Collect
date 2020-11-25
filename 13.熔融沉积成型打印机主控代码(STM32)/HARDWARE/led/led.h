#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"
#include "sys.h"

void LED_Init(void);
void LED_Blink(uint8_t frequency);

#define LED0 PEout(5)
#define LED1 PEout(6)

#define LED_ON  0
#define LED_OFF 1

#endif // __LED_H

