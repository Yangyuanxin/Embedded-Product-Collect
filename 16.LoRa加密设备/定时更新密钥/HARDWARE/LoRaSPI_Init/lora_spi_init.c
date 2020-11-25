#include "lora_spi_init.h"


void LoRaSPI_Init(void)
{
    SPI_InitTypeDef		SPI_InitStructure;
    GPIO_InitTypeDef 	GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd( SPI_PIN_SCK_PORT_CLK  |
							SPI_PIN_MISO_PORT_CLK | 			//使能GPIOx时钟
							SPI_PIN_MOSI_PORT_CLK , ENABLE );
    RCC_APB2PeriphClockCmd( SPI_CLK, ENABLE );					//使能SPI1时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );		//使能辅助功能时钟
	
    //GPIO configuration 
	GPIO_InitStructure.GPIO_Pin 	= SPI_PIN_SCK;				//时钟CLK引脚
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  	= GPIO_Mode_AF_PP;
    GPIO_Init( SPI_PIN_SCK_PORT, &GPIO_InitStructure );			//根据指定参数初始化
	
    GPIO_InitStructure.GPIO_Pin 	= SPI_PIN_MOSI;				//MOSI引脚
    GPIO_Init( SPI_PIN_MOSI_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;  	//MISO引脚
    GPIO_InitStructure.GPIO_Pin 	= SPI_PIN_MISO;
    GPIO_Init( SPI_PIN_MISO_PORT, &GPIO_InitStructure );

    //SPI_INTERFACE Config 
    SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;	//全线全双工
    SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;					//主机模式
    SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;					//数据宽度为8bit
    SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_Low;						//时钟空闲状态为低电平
    SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_1Edge;					//第一个跳变沿开始采样
    SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;						//软件管理NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; 		//时钟时速9MHz
    SPI_InitStructure.SPI_FirstBit			= SPI_FirstBit_MSB;					//从最高位开始传输
    SPI_InitStructure.SPI_CRCPolynomial 	= 7;								//CRC多项式系数为7
    SPI_Init( SPI_INTERFACE, &SPI_InitStructure );								//初始化SPI
	
    SPI_Cmd( SPI_INTERFACE, ENABLE );	//使能SPI1
}


/***************************************************
	* 功能描述：SPI数据传输
	* 入口参数：outdata	-- 待发送的数据
	* 返回值：	SPI读取的数据
	* 备注：	SPI为一个环状结构，如要读取数据
				首先应写入1Byte的数据后读取。
****************************************************/
u8 SPIx_InOut( u8 outData )
{
    //发送数据
    SPI_I2S_SendData( SPI_INTERFACE, outData );
	
	//等待接收标志位为"1"
    while( SPI_I2S_GetFlagStatus( SPI_INTERFACE, SPI_I2S_FLAG_RXNE ) == RESET );
	
	//返回接收到的数据
    return SPI_I2S_ReceiveData( SPI_INTERFACE );
}






