/*
**************************************************************
*
* 文件名称：mpu_iic.h;
*
* 版本说明：V.10；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：暂无；
*           
* 完成日期：2016/04/24;
*
* 作者姓名：郑子烁;
*
* 其他备注：模拟mpu6050 的iic接口；
*
**************************************************************
*/
#ifndef __MPU_IIC_H__
#define __MPU_IIC_H__

#include "sys.h"
#include "delay.h"
#include "gpio.h"



#define MPU_IIC_SDA  PCout(4)             //SDA信号线；
#define MPU_IIC_SCL  PCout(5)             //SCL时钟线；
#define MPU_READ_SDA PCin(4)              //读SDA信号；



#define MPU_IIC_SDA_OUT()  GPIO_Init(GPIOC,P4,IO_MODE_OUT,IO_SPEED_2M,IO_OTYPER_PP,IO_PULL,IO_H)
#define MPU_IIC_SDA_IN()  GPIO_Init(GPIOC,P4,IO_MODE_IN,IO_SPEED_2M,IO_OTYPER_PP,IO_NOT,IO_H)

//#define MPU_IIC_SDA_OUT() GPIO_Init(GPIOB,P11,OUT_10M,G_PUSH,SET_L)                //SDA为输出模式；
//#define MPU_IIC_SDA_IN()  GPIO_Init(GPIOB,P11,IN_MODE,PULL,PULL_UP)                //SDA为输入模式；



/*
* 函数名称：MPU_IIC_Init；
* 功能说明：模拟IIC初始化；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：PC12和PC11分别模拟SCL和SDA；
*/
void MPU_IIC_Init(void);


/*
* 函数名称：MPU_IIC_Start；
* 功能说明：产生起始信号；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/
void MPU_IIC_Start(void);


/*
* 函数名称：MPU_IIC_Spot；
* 功能说明：产生结束信号；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/
void MPU_IIC_Stop(void);


/*
* 函数名称：MPU_IIC_Wait_Ack；
* 功能说明：等待应答信号；
* 入口参数：无；
* 返回值  ：0->接收应答成功，1->接收应答失败；
* 备注    ：无；
*/
u8 MPU_IIC_Wait_Ack(void);



/*
* 函数名称：MPU_IIC_Ack；
* 功能说明：发送ACK信号；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：在应答脉冲期间保持稳定低电平；
*/
void MPU_IIC_Ack(void);


/*
* 函数名称：MPU_IIC_NAck；
* 功能说明：发送NACK信号；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：在应答脉冲期间保持稳定高电平；
*/
void MPU_IIC_NAck(void);


/*
* 函数名称：MPU_IIC_Send_Byte；
* 功能说明：送出一个字节；
* 入口参数：tx_data->发送的一字节数据；
* 返回值  ：无；
* 备注    ：每个时钟传送一位数据，SCL为高时SDA必须保持稳定；
*           因为此时SDA的改变会被认为是控制信号(开始、结束或者应答)；
*           这也是控制信号和数据信号的区别；
*/	  
void MPU_IIC_Send_Byte(u8 tx_data);	


/*
* 函数名称：MPU_IIC_Read_Byte；
* 功能说明：读取一个字节；
* 入口参数：ack->0：发送NACK，ack->1：发送ACK；
* 返回值  ：读取到的8位数据；
* 备注    ：无；
*/	
u8 MPU_IIC_Read_Byte(u8 ack);


#endif



