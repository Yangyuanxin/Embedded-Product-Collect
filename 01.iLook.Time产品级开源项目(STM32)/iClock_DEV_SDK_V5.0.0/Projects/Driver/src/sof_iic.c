/*----------------------------------------------------------------------------------------*/
/* bmp280.c                                                                               */
/* 项目描述:                                                                              */
/*                                                                                        */
/*----------------------------------------------------------------------------------------*/
/* 2014/12/30         	   manwjh 建立 V0.1                                               */
/*                                                                                        */
/*                                                                                        */
/*----------------------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "platform_config.h"
#include "sof_iic.h"

//define
#define SDA_H   GPIO_SetBits(I2C_BUS,I2C_SDA_PIN)
#define SDA_L   GPIO_ResetBits(I2C_BUS,I2C_SDA_PIN)

#define SCK_H   GPIO_SetBits(I2C_BUS,I2C_SCK_PIN)
#define SCK_L   GPIO_ResetBits(I2C_BUS,I2C_SCK_PIN)


#define SDA_State()   GPIO_ReadInputDataBit(I2C_BUS,I2C_SDA_PIN)
#define SCK_State()   GPIO_ReadInputDataBit(I2C_BUS,I2C_SCK_PIN)

/*----------------------------------------------------------------------------------------*/


__inline void I2C_delay()
{
  int i=3;
  
  while(i--);
}

void I2C_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  int i;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_I2C_BUS, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin =  I2C_BUS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//   GPIO_Mode_Out_PP;//GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(I2C_BUS, &GPIO_InitStructure);

  GPIO_SetBits(I2C_BUS,I2C_SCK_PIN);
  GPIO_SetBits(I2C_BUS,I2C_SDA_PIN);

  
  for(i=0;i<9;i++){
    SCK_H;
    I2C_delay();
    SCK_L;
    I2C_delay();
  } 
  SCK_H;
  SDA_H;
}

I2C_STATE I2C_CheckBusy(void)
{
  if( SDA_State()==0 )
    return I2C_BUSY;
  else
    return I2C_IDLE;
}


I2C_STATE I2C_Start(void)
{
	SDA_H;
	SCK_H;
	I2C_delay();
	if(!SDA_State())  
	  return I2C_BUSY;	/* SDA线为低电平则总线忙,退出 */
	SDA_L;
	I2C_delay();
	if(SDA_State())
	  return I2C_ERROR;	/* SDA线为高电平则总线出错,退出 */
	SDA_L;
	I2C_delay();
	
	return I2C_OK;
}

void I2C_Stop(void)
{
	SCK_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCK_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

void I2C_Ack(void)
{	
	SCK_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCK_H;
	I2C_delay();
	SCK_L;
	I2C_delay();
}

void I2C_NoAck(void)
{	
	SCK_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCK_H;
	I2C_delay();
	SCK_L;
	I2C_delay();
}

I2C_STATE I2C_WaitAck(void) 	
{
	SCK_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCK_H;
	I2C_delay();
	if(SDA_State())
	{
      SCK_L;
      return I2C_NOACK;
	}
	SCK_L;
	return I2C_ACK;
}

void I2C_SendByte(uint8_t SendByte) 
{
    uint8_t i=8;
    while(i--)
    {
        SCK_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
		SCK_H;
        I2C_delay();
    }
    SCK_L;
}


uint8_t I2C_ReceiveByte(void)  
{ 
    uint8_t i=8;
    uint8_t ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCK_L;
      I2C_delay();
	    SCK_H;
      I2C_delay();
      if(SDA_State())
      {
        ReceiveByte|=0x01;
      }
    }
    SCK_L;
    return ReceiveByte;
}


