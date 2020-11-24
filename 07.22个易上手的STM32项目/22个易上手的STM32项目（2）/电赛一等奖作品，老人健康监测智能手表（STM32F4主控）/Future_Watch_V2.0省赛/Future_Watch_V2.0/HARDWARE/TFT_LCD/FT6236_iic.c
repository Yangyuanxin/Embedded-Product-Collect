/*
**************************************************************
*
* 文件名称：touch.c;
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


#include "FT6236_iic.h"


/*
* 函数名称：FT6236_IIC_Start；
* 功能说明：FT6236起始信号；
* 入口参数：无
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_IIC_Start(void)					
{
	FT6236_SDA_OUT();   //sda线输出
	FT6236_SDA = 1;									
	FT6236_SCL = 1;		//SCL最小高电平脉宽:0.6us
	delay_us(10);		//起始信号的最小建立时间:0.6us
	FT6236_SDA = 0;		//SCL高电平期间，SDA的一个下降沿表示起始信号
	delay_us(10);		//起始信号的最小保持时间:0.6us
	FT6236_SCL = 0;		//箝住总线,为发送器件地址做准备;
}


/*
* 函数名称：FT6236_IIC_Stop；
* 功能说明：FT6236结束信号；
* 入口参数：无
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_IIC_Stop(void)							
{
	FT6236_SDA_OUT();   //sda线输出	
	FT6236_SCL = 0;		//SCL最小高电平脉宽:0.6us		0
	FT6236_SDA = 0;	
	delay_us(10);
	FT6236_SCL = 1;
	delay_us(10);
	FT6236_SDA = 1;		//SCL高电平期间，SDA的一个上升沿表示停止信号						
}


/*
* 函数名称：FT6236_Send_ACK；
* 功能说明：单片机发送应答信号；
* 入口参数：无
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_Send_ACK(void)							
{
	FT6236_SCL = 0;	
	FT6236_SDA_OUT();     		//sda线输出	
	FT6236_SDA = 0;	
	delay_us(10);																	
	FT6236_SCL = 1;		        //SCL最小高电平脉宽:0.6us
	delay_us(10);
	FT6236_SCL = 0;		        //SCL最小低电平脉宽:1.2us
}


/*
* 函数名称：FT6236_Send_NACK；
* 功能说明：单片机发送非应答信号；
* 入口参数：无
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_Send_NACK(void)
{
	FT6236_SCL = 0;	
	FT6236_SDA_OUT();     				//sda线输出	
	FT6236_SDA = 1;	
	delay_us(10);																	
	FT6236_SCL = 1;				//SCL最小高电平脉宽:0.6us
	delay_us(10);
	FT6236_SCL = 0;				//SCL最小低电平脉宽:1.2us
}


/*
* 函数名称：FT6236_Wait_Ack；
* 功能说明：等待应答信号；
* 入口参数：无
* 返回值  ：0-->接收应答成功；1-->接收应答失败；
* 备注    ：无；
*/
u8 FT6236_Wait_Ack(void)							
{
	u8 ucErrTime=0;
	FT6236_SDA_IN();      		//SDA设置为输入
	FT6236_SDA = 1;
	delay_us(1);
	FT6236_SCL = 1;				//使SDA上数据有效;SCL最小高电平脉宽:0.6us
	delay_us(1);
	while(FT6236_SDA_Read)
	{	
		ucErrTime++;
		if(ucErrTime>250)		//无应答
		{
			FT6236_IIC_Stop();	
			return 1;
		}
	}
	FT6236_SCL = 0;
	return 0;
}


/*
* 函数名称：FT6236_Send_Byte；
* 功能说明：向FT6236发送一个字节数据；
* 入口参数：data-->发送的数据；
* 返回值  ：无；
* 备注    ：无；
*/
void FT6236_Send_Byte(u8 data)						
{
	u8 i;						
	FT6236_SDA_OUT();     		//sda线输出	
	FT6236_SCL = 0;				//拉低时钟开始数据传输
	delay_us(3);
	for(i=0;i<8;i++)		//8位1B地址/数据的长度
	{
		
		FT6236_SDA=(data&0x80)>>7;
		data<<=1;
		FT6236_SCL = 1;
		delay_us(2);
		FT6236_SCL = 0;
		delay_us(2);
	}		
}


/*
* 函数名称：FT6236_Read_Byte；
* 功能说明：从FT6236读一个字节数据；
* 入口参数：无
* 返回值  ：读出的数据；
* 备注    ：无；
*/
u8 FT6236_Read_Byte(unsigned char ack)						
{
	u8 i,data = 0;				//接收数据位数和内容暂存单元
	FT6236_SDA_IN();			//SDA设置为输入
	delay_us(2);
	for(i=8;i>0;i--)
	{
		FT6236_SCL = 0;
		delay_us(30);
		FT6236_SCL = 1;
		data <<= 1;
		if(FT6236_SDA_Read)
		{data++;}
	}
	if(!ack)FT6236_Send_NACK();
	else FT6236_Send_ACK();
	return(data);				//返回1B的数据
}

