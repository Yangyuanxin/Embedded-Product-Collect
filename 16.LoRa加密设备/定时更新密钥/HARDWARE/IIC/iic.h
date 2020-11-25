#ifndef __IIC_H
#define __IIC_H

#include "sys.h"

//IO方向设置
#define	SDA_IN()		{ GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28; }
#define	SDA_OUT()		{ GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28; }

//IO操作函数(使用IIC2 -- PB10/11)	 
#define	IIC_SCL			PBout(6) 	//SCL
#define	IIC_SDA			PBout(7) 	//SDA	 
#define	READ_SDA()		PBin(7)  	//输入SDA 

#define	IIC_SCL_RCC		RCC_APB2Periph_GPIOB
#define	IIC_SCL_PORT	GPIOB
#define	IIC_SCL_PIN		GPIO_Pin_6

#define	IIC_SDA_RCC		RCC_APB2Periph_GPIOB
#define	IIC_SDA_PORT	GPIOB
#define	IIC_SDA_PIN		GPIO_Pin_7


/******************************************************************************/
////IO方向设置
//#define	SDA_IN()  		{ GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=(u32)8<<4;}
//#define	SDA_OUT() 		{ GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=(u32)3<<4;}

////IO操作函数(使用IIC2 -- PB10/11)	 
//#define	IIC_SCL			PBout(8) 				//SCL
//#define	IIC_SDA    		PBout(9) 				//SDA	 
//#define	READ_SDA()		PBin(9)  				//输入SDA 

//#define	IIC_SCL_RCC		RCC_APB2Periph_GPIOB	//SCL时钟
//#define	IIC_SCL_PORT	GPIOB					//SCL端口
//#define	IIC_SCL_PIN		GPIO_Pin_8				//SCL引脚

//#define	IIC_SDA_RCC		RCC_APB2Periph_GPIOB	//SDA时钟
//#define	IIC_SDA_PORT	GPIOB					//SDA端口
//#define	IIC_SDA_PIN		GPIO_Pin_9				//SDA引脚
/******************************************************************************/


//IIC所有操作函数
void 	IIC_Init(void);                					//初始化IIC的IO口				 
void 	IIC_Start(void);								//发送IIC开始信号
void 	IIC_Stop(void);	  								//发送IIC停止信号
void 	IIC_Send_Byte(u8 txd);							//IIC发送一个字节
u8 		IIC_Read_Byte(unsigned char ack);				//IIC读取一个字节
u8 		IIC_Wait_Ack(void); 							//IIC等待ACK信号
void 	IIC_Ack(void);									//IIC发送ACK信号
void 	IIC_NAck(void);									//IIC不发送ACK信号

#endif


