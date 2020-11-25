#include <stdint.h>
#include <stdbool.h> 
#include "platform.h"


#if defined( USE_SX1276_RADIO )

#include "lora_spi_init.h"
#include "sx1276-Hal.h"


//sx1276引脚初始化(片选、复位)
void SX1276InitIo( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | 			//使能GPIO时钟，辅助功能时钟
							RCC_APB2Periph_GPIOB | 
							RCC_APB2Periph_AFIO,  ENABLE );

    GPIO_InitStructure.GPIO_Mode  	= GPIO_Mode_Out_PP;		//推挽输出
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;		//I/O口时速
	
    //配置模块的片选引脚 -- PA4
    GPIO_InitStructure.GPIO_Pin 	= SEL_PIN;				
    GPIO_Init( SEL_IOPORT, &GPIO_InitStructure );
	GPIO_SetBits(SEL_IOPORT,SEL_PIN);						//置高
	
	//配置模块的复位引脚 -- PB1
	GPIO_InitStructure.GPIO_Pin 	= RST_PIN;				//复位引脚			
	GPIO_Init( RST_IOPORT, &GPIO_InitStructure );
	
	//---------------------------------------------------------------------
	//Configure DIO0 -- PB0
	GPIO_InitStructure.GPIO_Mode 	= 	GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_InitStructure.GPIO_Pin 	=  	DIO0_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );
	
	// Configure DIO3 as input
    // Configure DIO4 as input
    // Configure DIO5 as input
	//---------------------------------------------------------------------
	
	//初始化SPI1接口
	LoRaSPI_Init();	
}


/************************************
	*功能描述：	设置RST引脚状态
	*入口参数：	状态值(0|1)
	*返回值：	无
*************************************/
void SX1276SetReset(u8 state)
{
    if( state == RADIO_RESET_ON )
    {
		RST_L();	//将复位引脚置低
    }
    else
    {
		RST_H();	//将复位引脚置高
    }
}


void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    u8 i;

	SEL_L();    //片选拉低(数据传输开始)
    //GPIO_WriteBit( SEL_IOPORT, SEL_PIN, Bit_RESET );

    SPIx_InOut( addr | 0x80 );		//1000 0000(写操作+addr)
    for( i = 0; i < size; i++ )
    {
        SPIx_InOut( buffer[i] );	//SPI写入一个Byte数据
    }

    //SEL = 1;
	SEL_H();	//片选拉高(数据传输结束)
    //GPIO_WriteBit( SEL_IOPORT, SEL_PIN, Bit_SET );
}


void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    u8 i;

	SEL_L();	//片选拉低(数据传输开始)
	
    SPIx_InOut( addr & 0x7F );			//0111 1111(读操作+addr)

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SPIx_InOut( 0 );	//SPI读取一个Byte数据
    }

	SEL_H();	//片选拉高(数据传输结束)
}


#if 1
/********************************************
	*功能描述：	往指定地址写入1Byte数据
	*入口参数：	addr:寄存器地址
				data:8bit的数据
	*返回值：	无
*********************************************/
void SX1276Write(u8 addr, u8 data)
{
    SX1276WriteBuffer(addr, &data, 1);
}


/********************************************
	*功能描述：	从指定地址读取1Byte数据
	*入口参数：	addr:寄存器地址
				*data:数据的地址
	*返回值：	无
*********************************************/
void SX1276Read( u8 addr, u8 *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}
#endif


void SX1276WriteFifo( u8 *buffer, u8 size )
{
    SX1276WriteBuffer( 0, buffer, size );
}


void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}


u8 SX1276ReadDio0( void )
{
	//返回DIO0引脚的状态
	return GPIO_ReadInputDataBit( DIO0_IOPORT, DIO0_PIN );
}


u8 SX1276ReadDio1( void )
{
    //返回DIO1引脚的状态
//	return GPIO_ReadInputDataBit( DIO1_IOPORT, DIO1_PIN );
}


u8 SX1276ReadDio2( void )
{
	//返回DIO2引脚的状态
//	return GPIO_ReadInputDataBit( DIO2_IOPORT, DIO2_PIN );
}


uint8_t SX1276ReadDio3( void )
{
    //return IoePinGet( RF_DIO3_PIN );
}


uint8_t SX1276ReadDio4( void )
{
    //return IoePinGet( RF_DIO4_PIN );
}


uint8_t SX1276ReadDio5( void )
{
   //return IoePinGet( RF_DIO5_PIN );
}

#endif




