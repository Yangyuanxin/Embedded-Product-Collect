#ifndef __motor_H
#define __motor_H	 
#include "sys.h"

#define ENA PAout(11) 		 //PA11电机使能信号
#define PUL PAout(12) 		 //PA12电机时钟信号
#define DIR PBout(0)  		 //PB0 电机正反信号
#define MCU_LOCK PBout(1)    //PB1 步进电机断电刹车

#define MCU_TEST PCout(0)    //PC0 测试

void motor_Init(void);		   //初始化
u32 motor_P(u32 MP_delay);	   //正转
u32 motor_N(u32 MN_delay);	   //反转
u8 motor_R(u32 MP_delay,u8 PN);
void motor_S(void);			   //停止转动
					    
#endif
