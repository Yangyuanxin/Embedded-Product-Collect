/*
**************************************************************
*
* 文件名称：touch.h;
*
* 版本说明：V1.0；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：无；
*           
* 完成日期：2016/06/26;
*
* 作者姓名：郑子烁;
*
* 其他备注：触屏芯片FT6236底层iic；
*          
* 修改说明：无；
*
**************************************************************
*/


#ifndef __TOUCH_H__
#define __TOUCH_H__


#include "sys.h"
#include "delay.h"
#include "gpio.h"

#define FT6236_SDA_OUT() GPIO_Init(GPIOC,P12,IO_MODE_OUT,IO_SPEED_50M,GPIO_OTYPE_PP,IO_NOT,IO_H)             //设置SDA为输出模式；
#define FT6236_SDA_IN()  GPIO_Init(GPIOC,P12,IO_MODE_IN,IO_SPEED_50M,GPIO_OTYPE_PP,IO_PULL,IO_H)            //设置SDA为输入模式；

#define FT6236_SDA PCout(12)                                                   //写SDA
#define FT6236_SCL PBout(3)                                                   //时钟控制；
#define FT6236_RST PCout(11)                                                   //复位控制；

#define FT6236_SDA_Read PCin(12)                                               //读SDA；
#define FT6236_INT      PBin(4)                                               //读INT；





/*
* 函数名称：FT6236_IIC_Start；
* 功能说明：FT6236起始信号；
* 入口参数：无
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_IIC_Start(void);


/*
* 函数名称：FT6236_IIC_Stop；
* 功能说明：FT6236结束信号；
* 入口参数：无
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_IIC_Stop(void);


/*
* 函数名称：FT6236_Send_ACK；
* 功能说明：单片机发送应答信号；
* 入口参数：无
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_Send_ACK(void);


/*
* 函数名称：FT6236_Send_NACK；
* 功能说明：单片机发送非应答信号；
* 入口参数：无
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_Send_NACK(void);


/*
* 函数名称：FT6236_Wait_Ack；
* 功能说明：等待应答信号；
* 入口参数：无
* 返回值  ：0-->接收应答成功；1-->接收应答失败；
* 备注    ：无；
*/
u8 FT6236_Wait_Ack(void);


/*
* 函数名称：FT6236_Send_Byte；
* 功能说明：向FT6236发送一个字节数据；
* 入口参数：data-->发送的数据；
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_Send_Byte(u8 data);

/*
* 函数名称：FT6236_Read_Byte；
* 功能说明：从FT6236读一个字节数据；
* 入口参数：无
* 返回值  ：读出的数据；
* 备注    ：无；
*/
u8 FT6236_Read_Byte(unsigned char ack);


#endif

