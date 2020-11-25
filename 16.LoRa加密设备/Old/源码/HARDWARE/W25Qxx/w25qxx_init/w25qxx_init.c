#include "w25qxx_init.h"


//以下是SPI模块的初始化代码，配置成主机模式，访问SD Card/W25Q64/NRF24L01						  
//SPI口初始化
//这里针是对SPI2的初始化
//void W25QxxBaseInit(void)
//{
//	GPIO_InitTypeDef 	GPIO_InitStructure;
//	SPI_InitTypeDef  	SPI_InitStructure;

////	RCC_APB2PeriphClockCmd( SPIX_RCC,ENABLE);					//使能SPI始终
//	RCC_APB1PeriphClockCmd(	SPIX_RCC,  ENABLE );				//SPI2时钟使能 	
//	RCC_APB2PeriphClockCmd(	SPIX_FLASH_CS_CLK      |
//							SPIX_PIN_SCK_PORT_CLK  |
//							SPIX_PIN_MISO_PORT_CLK |
//							SPIX_PIN_MOSI_PORT_CLK , ENABLE );	//使能SPI引脚时钟

//	//SPIx -- SCK引脚 -- 
//	GPIO_InitStructure.GPIO_Pin   = SPIX_PIN_SCK;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  			//PB13/14/15复用推挽输出 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(SPIX_PIN_SCK_PORT, &GPIO_InitStructure);			//初始化GPIOB

//	//SPIx -- MISO引脚 -- 
//	GPIO_InitStructure.GPIO_Pin   =  SPIX_PIN_MISO;
//	GPIO_Init(SPIX_PIN_MISO_PORT, &GPIO_InitStructure);			//初始化GPIOB
//	
//	//SPIx -- MOSI引脚 -- 
//	GPIO_InitStructure.GPIO_Pin   =  SPIX_PIN_MOSI;
//	GPIO_Init(SPIX_PIN_MOSI_PORT, &GPIO_InitStructure);			//初始化GPIOB
//	
//	GPIO_SetBits(GPIOB,	GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);	//拉高PB13/14/15
//	/////////////////////////////////////////////////////////////////////////////
//	GPIO_InitStructure.GPIO_Pin   = SPIX_FLASH_CS_PIN;  		//选择片选引脚 
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;			//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(SPIX_FLASH_CS_PORT, &GPIO_InitStructure);			//初始化片选引脚
//	GPIO_SetBits(SPIX_FLASH_CS_PORT,SPIX_FLASH_CS_PIN);					

//	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
//	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;					//设置SPI工作模式:设置为主SPI
//	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;					//设置SPI的数据大小:SPI发送接收8位帧结构
//	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;					//串行同步时钟的空闲状态为高电平
//	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;					//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
//	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;						//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
//	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;					//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
//	SPI_InitStructure.SPI_CRCPolynomial 	= 7;								//CRC值计算的多项式
//	SPI_Init(SPIX, &SPI_InitStructure);											//根据指定的参数初始化外设SPIx寄存器

//	SPI_Cmd(SPIX, ENABLE); 		//使能SPI外设
//	SPIX_ReadWriteByte(0xff);	//启动传输	 
//}   



//----------------------------
void W25QxxSPI_Init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	SPI_InitTypeDef  	SPI_InitStructure;

	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );	//SPI2时钟使能 	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//使能SPI引脚时钟

	//SPIx -- SCK引脚 -- 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//初始化GPIOB

	//SPIx -- MISO引脚 -- 
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化GPIOB
	
	//SPIx -- MOSI引脚 -- 
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化GPIOB
	
	GPIO_SetBits(GPIOB,	GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);	//拉高PB13/14/15
	/////////////////////////////////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;  		//选择片选引脚 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//初始化片选引脚
//	GPIO_SetBits(GPIOB,GPIO_Pin_12);					//
	W25QXX_CS = 1;										//拉高片选引脚

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;  //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;					//设置为主SPI
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;					//SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;					//空闲状态为高电平
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;					//第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;						//内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;					//数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;								//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);											//根据指定的参数初始化外设SPIx寄存器

	SPI_Cmd(SPI2, ENABLE); 		//使能SPI外设
	SPIX_ReadWriteByte(0xff);	//启动传输	 
}   

//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
void SPIX_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1 &= 0XFFC7;
	SPI2->CR1 |= SPI_BaudRatePrescaler;	//设置SPI2速度 
	SPI_Cmd(SPI2,ENABLE); 
} 


//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPIX_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;	
	//检查指定的SPI标志位设置与否:发送缓存空标志位	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) 
	{
		retry++;
		if(retry>200)
			return 0;
	}			  
	SPI_I2S_SendData(SPI2, TxData);		//通过外设SPIx发送一个数据
	retry = 0;
	//检查指定的SPI标志位设置与否:接受缓存非空标志位
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) 
	{
		retry++;
		if(retry>200)
			return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据					    
}



