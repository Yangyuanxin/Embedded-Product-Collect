#include "iic.h"
#include "delay.h"


//初始化IIC
void IIC_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	IIC_SCL_RCC, ENABLE );			//使能GPIOB时钟

	GPIO_InitStructure.GPIO_Pin   = IIC_SCL_PIN ;			//GPIOB.6/7
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = IIC_SDA_PIN ;			//GPIOB.6/7
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, IIC_SCL_PIN|IIC_SDA_PIN); 			//PB10,PB11 输出高
}


//产生IIC起始信号(下降沿)
void IIC_Start(void)
{
    SDA_OUT();		//sda线输出
    IIC_SDA = 1;
    IIC_SCL = 1;
    delay_us(5);
    IIC_SDA = 0;	//START:when CLK is high,DATA change form high to low
    delay_us(5);
    IIC_SCL = 0;	//钳住I2C总线，准备发送或接收数据
}


//产生IIC停止信号(上升沿)
void IIC_Stop(void)
{
    SDA_OUT();		//sda线输出
    IIC_SCL = 0;
    IIC_SDA = 0; 	//STOP:when CLK is high DATA change form low to high
    delay_us(5);
    IIC_SCL = 1;
    IIC_SDA = 1; 	//发送I2C总线结束信号
    delay_us(5);
}


//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
	
    SDA_IN();      			//SDA设置为输入
    IIC_SDA = 1;		
    delay_us(5);
    IIC_SCL = 1 ;
    delay_us(5);			//等待ACK(SDA为0)
    while( READ_SDA() )
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
	IIC_SCL = 0;

    return 0;
}


//产生ACK应答
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;	//SDA数据线拉低
    delay_us(5);	//2us
    IIC_SCL = 1;
    delay_us(5);	//2us
    IIC_SCL = 0;
}


// 不产生ACK应答
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;	//SDA数据线拉高
    delay_us(5);
    IIC_SCL = 1;
    delay_us(5);
    IIC_SCL = 0;
}


//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
    u8 t;
	
    SDA_OUT();		//SDA为输出
    IIC_SCL = 0;	//拉低时钟开始数据传输
    
	for(t = 0; t < 8; t++)
    {
        if( (txd & 0x80) >> 7 )
            IIC_SDA = 1;
        else
            IIC_SDA = 0;
		
		txd <<= 1;			//左移1bit
		delay_us(5);		
		IIC_SCL = 1;
		delay_us(5);
		IIC_SCL = 0;
		delay_us(5);
    }
	
}


//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    u8 i, receive = 0;
	
    SDA_IN();				//SDA设置为输入
    for(i = 0; i < 8; i++ )
    {
        IIC_SCL = 0;
        delay_us(5);
        IIC_SCL = 1;
        receive <<= 1;
        if( READ_SDA() )
            receive++;
        delay_us(5);
    }
	 
    if (!ack)
        IIC_NAck();		//发送nACK
    else
        IIC_Ack(); 		//发送ACK
    return receive;
}






