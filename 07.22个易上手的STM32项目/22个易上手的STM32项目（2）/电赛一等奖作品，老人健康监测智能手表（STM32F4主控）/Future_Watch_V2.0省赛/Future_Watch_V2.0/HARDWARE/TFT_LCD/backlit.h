#ifndef _BACKLIT_H_
#define _BACKLIT_H_

#include "delay.h"
#include "sys.h"


#define LSENS_READ_TIMES	10		//定义光敏传感器读取次数,读这么多次,然后取平均值
#define LSENS_ADC_CHX		11		//定义光敏传感器所在的ADC通道编号


void Lsens_Init(void); 				//初始化光敏传感器
u8 Lsens_Get_Val(void);				//读取光敏传感器的值
void TIM3_PWM_Init(u32 arr,u32 psc);

#endif
