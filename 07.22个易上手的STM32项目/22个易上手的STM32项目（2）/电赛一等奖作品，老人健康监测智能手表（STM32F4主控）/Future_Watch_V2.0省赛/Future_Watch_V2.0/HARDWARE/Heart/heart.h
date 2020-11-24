/*
**************************************************************
*
* 文件名称：time_1.h;
*
* 版本说明：V.13；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：增加输入捕获实验；
*           
* 完成日期：2016/03/04;
*
* 作者姓名：郑子烁;
*
* 其他备注：高级定时器计数实验；
*           PWM产生实验；
*           输入捕获实验；
*           编码器接口模式实验；
*      
**************************************************************
*/

#ifndef __TIME_1_H__
#define __TIME_1_H__

#include "sys.h"
#include "delay.h"
//#include "gpio.h"
//#include "led.h"

#define CNT_FREQUENCY   600000                         //计数频率*60；


#define PWM_VAL TIM1->CCR1                           //设置捕获/比较寄存器1；
#define TIM1_COUNT TIM1->CNT                         //读取定时器1的计数值；
#define GET_DIRECTION ((TIM1->CR1 & 1<<4)?1:0)       //读取计数方向；




/*
* 函数名称：Time_1_Init；
* 功能说明：高级定时器初始化；
* 入口参数：arr->自动重装值，psc->预分频数；
* 返回值  ：无；
* 备注    ：注意主从模式的时钟源；
*           默认向上边沿计数，禁止中断和DMA；
*/
void Time_1_Init(u16 arr,u16 psc);


/*
* 函数名称：Get_Haer_Rate；
* 功能说明：读心率数据；
* 入口参数：无；
* 返回值  ：心率；
* 备注    ：无；
*/
u8 Get_Haer_Rate(void);

#endif
