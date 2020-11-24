#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "sys.h"
#include "delay.h"
#include "gpio.h"

#define MOTOR_STOP		GPIOB->ODR &= ~(1<<1)		//马达停止转动 PB1 = 0
#define MOTOR_START		GPIOB->ODR |= (1<<1)		//马达开始转 PB1 = 1
#define MOTOR_WORK	PBout(1)

void Motor_Init(void);		//马达初始化


#endif
