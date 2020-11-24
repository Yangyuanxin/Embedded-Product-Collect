#ifndef GPRS_H_
#define GPRS_H_

#include "stm32f10x.h"

void task_gprs(void);

int gprs_send(u8 *buf, u16 len);

#endif /* LCD_H_ */
