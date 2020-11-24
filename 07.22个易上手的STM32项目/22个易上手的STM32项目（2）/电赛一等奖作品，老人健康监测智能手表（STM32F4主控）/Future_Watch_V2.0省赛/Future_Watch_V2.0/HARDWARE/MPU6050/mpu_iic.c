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
* 其他备注：模拟mpu6050的iic接口；
*
**************************************************************
*/
#include "mpu_iic.h"

/*
* 函数名称：MPU_IIC_Init；
* 功能说明：模拟IIC初始化；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：PC12和PC11分别模拟SCL和SDA；
*/
void MPU_IIC_Init(void)
{
	GPIO_Clock_Set(IOPCEN);                                   //先使能外设IO PORTC时钟 	
	
	 GPIO_Init(GPIOC,P4,IO_MODE_OUT,IO_SPEED_2M,IO_OTYPER_PP,IO_PULL,IO_H);	// SDA
	 GPIO_Init(GPIOC,P5,IO_MODE_OUT,IO_SPEED_2M,IO_OTYPER_PP,IO_PULL,IO_H);	//	SCL
}



/*
* 函数名称：MPU_IIC_Start；
* 功能说明：产生起始信号；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：根据iic协议的交流电气特性；
*/
void MPU_IIC_Start(void)
{
	MPU_IIC_SDA_OUT();
	
	MPU_IIC_SCL=1;
	
	delay_us(2);               //此处保险起见可延时2us；
	MPU_IIC_SDA=1;             //参考时序图(重要!)
	delay_us(2);
	MPU_IIC_SDA=0;
	delay_us(2);
	
	MPU_IIC_SCL=0;
}


/*
* 函数名称：MPU_IIC_Spot；
* 功能说明：产生结束信号；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：根据iic协议的交流电气特性；
*           应该注意SDA=0要在SCL=1之前完成，之后再进行SDA=1的变化；
*           否则在SCL为高时执行SDA=0一般会先检测到下降沿(因为前面可能拉高了SDA)，
*           器件会误认为是开始信号；
*/
void MPU_IIC_Stop(void)
{
	MPU_IIC_SDA_OUT();
	
	MPU_IIC_SDA=0;                          //参考时序图(重要!)
	MPU_IIC_SCL=0;
	delay_us(2);                            //此处保险起见可延时2us；
	MPU_IIC_SCL=1;
	delay_us(1);
	MPU_IIC_SDA=1;
}


/*
* 函数名称：MPU_IIC_Wait_Ack；
* 功能说明：等待应答信号；
* 入口参数：无；
* 返回值  ：0->接收应答成功，1->接收应答失败；
* 备注    ：无；
*/
u8 MPU_IIC_Wait_Ack(void)
{
	u8 nack_overtime=0;
	MPU_IIC_SDA_IN();
	
	MPU_IIC_SDA=1;
	delay_us(2);
	MPU_IIC_SCL=1;                          //根据时序图，在SCL为高等待应答信号；
	delay_us(2);                            //此处延时可参考iic应答信号的时序；
	while(MPU_READ_SDA)                        //根据时序图，读到1表示收到NACK；
	{
		nack_overtime++;
		if(nack_overtime>250)               //从机没有应答超时(这里是从机没有应答而不是非应答)，
		{
			MPU_IIC_Stop();                 //所以主机发送stop；
			return 1;
		}
	}
	MPU_IIC_SCL=0;                          //结束应答信号时钟；
	return 0;                               //接收到ACK或者NACK；
}


/*
* 函数名称：MPU_IIC_Ack；
* 功能说明：发送ACK信号；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：在应答脉冲期间保持稳定低电平；
*/
void MPU_IIC_Ack(void)
{
	MPU_IIC_SCL=0;
	MPU_IIC_SDA_OUT();
	MPU_IIC_SDA=0;                        //拉低SDA送ACK；
	delay_us(2);
	MPU_IIC_SCL=1;                        //在SCL为高保持稳定；
	delay_us(2);
	MPU_IIC_SCL=0;                        //结束应答时钟；
}


/*
* 函数名称：MPU_IIC_NAck；
* 功能说明：发送NACK信号；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：在应答脉冲期间保持稳定高电平；
*/
void MPU_IIC_NAck(void)
{
	MPU_IIC_SCL=0;
	MPU_IIC_SDA_OUT();
	MPU_IIC_SDA=1;                        //拉高SDA送NACK；
	delay_us(2);
	MPU_IIC_SCL=1;                        //在SCL为高保持稳定；
	delay_us(2);
	MPU_IIC_SCL=0;                        //结束应答时钟；
}


/*
* 函数名称：MPU_IIC_Send_Byte；
* 功能说明：送出一个字节；
* 入口参数：tx_data->发送的一字节数据；
* 返回值  ：无；
* 备注    ：每个时钟传送一位数据，SCL为高时SDA必须保持稳定；
*           因为此时SDA的改变会被认为是控制信号(开始、结束或者应答)；
*           这也是控制信号和数据信号的区别；
*/	  
void MPU_IIC_Send_Byte(u8 tx_data)
{                        
    u8 i;   
	MPU_IIC_SDA_OUT(); 	    

    for(i=0;i<8;i++)
    {           
		MPU_IIC_SCL=0;                  //拉低时钟开始数据传输；
        MPU_IIC_SDA=(tx_data&0x80)>>7;
        tx_data<<=1; 	  
		delay_us(2);                    //保持稳定；
		MPU_IIC_SCL=1;
		delay_us(2); 
    }	 
	MPU_IIC_SCL=0;                       //根据根据时序图，最后要多个时钟；
} 	


/*
* 函数名称：MPU_IIC_Read_Byte；
* 功能说明：读取一个字节；
* 入口参数：ack->0：发送NACK，ack->1：发送ACK；
* 返回值  ：读取到的8位数据；
* 备注    ：无；
*/	
u8 MPU_IIC_Read_Byte(u8 ack)
{
	u8 i,temp;
	MPU_IIC_SDA_IN();
	
	for(i=0;i<8;i++)
	{
		MPU_IIC_SCL=0;
		delay_us(2);
		MPU_IIC_SCL=1;                   //在时钟为高时读出；
		temp<<=1;
		if(MPU_READ_SDA)
		{temp++;}
		delay_us(2);
	}
	
	if(ack)
		MPU_IIC_Ack();
	else
		MPU_IIC_NAck();
	return temp;
}


