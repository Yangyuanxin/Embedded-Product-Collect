#ifndef __PPM_H
#define __PPM_H
#include "main.h"
#define MS20	(20000)	/* 20ms */
#define MS05	(500)	/* 0.5ms */
#define PPM_NUM chNum*2+2 //PPM数组中的个数，这里是8通道1+8*2+1
#define PPM_ON() TIM_Cmd(TIM4, ENABLE); //使能TIM4
#define PPM_OFF() TIM_Cmd(TIM4, DISABLE); //不使能TIM4
extern u16 PPM_Array[PPM_NUM];

void PPM_Init(void);

#endif

