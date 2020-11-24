/*
**************************************************************
*
* 文件名称：usart_2.h;
*
* 版本说明：V.10；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：暂无；
*           
* 完成日期：2016/05/27;
*
* 作者姓名：郑子烁;
*
* 其他备注：串口2实验；
*
**************************************************************
*/

#ifndef __USART_2_H__
#define __USART_2_H__


#include "sys.h"
#include "delay.h"
#include "led.h"
extern u16 AT_RX_STA;                        //AT模式接收状态标志；B15:接收完成标志;B14:接收到0x0d;B13~0:接收到的有效字节数目
extern u16 UT_RX_STA;                        //透传模式接收转态标志，



/*
* 函数名称：Usart_2_Init；
* 功能说明：Usart2初始化；
* 入口参数：pclk1->PCLK1时钟频率最高36，bound->波特率；
* 返回值  ：无；
* 备注    ：无；
*/
void Usart_1_Init(u32 pclk1,u32 bound);


/*
* 函数名称：Usart_2_Send；
* 功能说明：Usart2串口发送；
* 入口参数：data->发送的数据；
* 返回值  ：无；
* 备注    ：无；
*/
void Usart_1_Send(u8 data);


/*
* 函数名称：USART2_IRQHandler；
* 功能说明：Usart2接收中断服务函数；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/
void USART1_IRQHandler(void);


#endif

