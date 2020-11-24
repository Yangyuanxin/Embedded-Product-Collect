#include "BL8025.h"
#include "myiic.h"
#include "delay.h"
#include "RS485.h"
#include "usart.h"
#include "w25qxx.h"
#include "Current.h"

uint8_t SetTime_Hour;
uint8_t SetTime_Minute;
uint8_t SetTime_Second;

uint8_t SetDate_Year;
uint8_t SetDate_Month;
uint8_t SetDate_Day;
uint8_t SetDate_Week;

uint8_t Current_Hour;
uint8_t Current_Minute;
uint8_t Current_Second;
uint8_t Current_Second_Test;

uint8_t Current_MinuteBak;
uint8_t Current_SecondBak;
uint8_t Current_Second_TestBak;

uint8_t Current_Year;
uint8_t Current_Month;
uint8_t Current_Day;
uint8_t Current_Week;

volatile uint16_t BL8025TimerMs;

uint8_t u8_HalfHourCntBuf[32];

uint8_t u8_TrickleCurrentTimeBuf1_8[8];
uint8_t u8_TrickleCurrentTimeBuf9_16[8];
uint8_t u8_TrickleCurrentTimeBuf17_24[8];
uint8_t u8_TrickleCurrentTimeBuf25_32[8];
uint8_t u8_JitterRunTimeBuf[32];

void BL8025_Init(void)   //
{
	IIC_Init();//IIC初始化
	
	BL8025ReadDateTime(); //读时间
	
	if((uni_SysStatus.Str.u8_BL8025TInit != 0x55)  //BL8025不等于0X55，说明没有初始化过
			|| (Current_Year==0)
			|| (Current_Month==0)
			|| (Current_Day==0)
			|| (Current_Hour>23)
			|| (Current_Minute>59))
	{
		SetDate_Year = 18;
		SetDate_Month = 1;
		SetDate_Day = 1;
		SetDate_Week = 1;
		SetTime_Hour = 0;
		SetTime_Minute = 0;
		SetTime_Second = 0;
		
		BL8025SetDate();//设置年月日
		BL8025SetTime();//设置时分秒
		
		uni_SysStatus.Str.u8_BL8025TInit = 0x55;
	}
}

uint8_t Hex2BCD(uint8_t hex) //十进制转换为二进制
{
  uint8_t i,j,k; 
  
  i = hex / 10; 
  j = hex % 10; 
  k = j + (i << 4); 
  
  return k; 	
}


uint8_t BCD2Hex(uint8_t BCD) //二进制转换为十进制
{
  uint8_t temp; 
  
  temp = BCD & 0x0f;          //用4位二进制数来表示1位十进制数中的0~9这10个数码 
  BCD >>= 4; 
  BCD &= 0x0f; 
  BCD *= 10; 
  temp += BCD; 
  
  return temp;	
}


void BL8025SetDate(void)
{
	BL8025WriteOneByte(BL8025_YEAR_ADD,Hex2BCD(SetDate_Year));
	BL8025WriteOneByte(BL8025_MONTH_ADD,Hex2BCD(SetDate_Month));
	BL8025WriteOneByte(BL8025_DAY_ADD,Hex2BCD(SetDate_Day));
		
	switch(SetDate_Week) //设置星期几
	{
		case 0x00:	SetDate_Week = 0x01;break;
		case 0x01:	SetDate_Week = 0x02;break;
		case 0x02:	SetDate_Week = 0x04;break;
		case 0x03:	SetDate_Week = 0x08;break;
		case 0x04:	SetDate_Week = 0x10;break;
		case 0x05:	SetDate_Week = 0x20;break;
		case 0x06:	SetDate_Week = 0x40;break;
		
		default:break;
	}
	
	BL8025WriteOneByte(BL8025_WEEK_ADD,Hex2BCD(SetDate_Week)); //设置星期几
}

void BL8025SetTime(void)
{
	BL8025WriteOneByte(BL8025_HOUR_ADD,Hex2BCD(SetTime_Hour));
	BL8025WriteOneByte(BL8025_MINUTE_ADD,Hex2BCD(SetTime_Minute));
	BL8025WriteOneByte(BL8025_SECOND_ADD,Hex2BCD(SetTime_Second));
}
void BL8025WriteOneByte(uint8_t add,uint8_t dat) 
{
  IIC_Start();  	
	IIC_Send_Byte(BL8025_ADD);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(add);
	IIC_Wait_Ack();	
	IIC_Send_Byte(dat);
	IIC_Wait_Ack();	
	IIC_Stop();//产生一个停止条件
	delay_ms(10);			
}

uint8_t BL8025ReadOneByte(uint8_t add)
{
	uint8_t temp=0;	
	
  IIC_Start();  
	IIC_Send_Byte(BL8025_ADD);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(add);
	IIC_Wait_Ack();	
	IIC_Start();
	IIC_Send_Byte(BL8025_ADD | 0x01);
	IIC_Wait_Ack();	
	
	temp=IIC_Read_Byte(0);	
	
	IIC_Stop();//产生一个停止条件
	delay_ms(10);		
    
	return temp;	
}
void BL8025ReadDateTime(void)   //读当前时间
{
	Current_Hour = BCD2Hex(BL8025ReadOneByte(BL8025_HOUR_ADD));
	Current_Minute = BCD2Hex(BL8025ReadOneByte(BL8025_MINUTE_ADD));
	Current_Second = BCD2Hex(BL8025ReadOneByte(BL8025_SECOND_ADD));

	Current_Year = BCD2Hex(BL8025ReadOneByte(BL8025_YEAR_ADD));
	Current_Month = BCD2Hex(BL8025ReadOneByte(BL8025_MONTH_ADD));
	Current_Day = BCD2Hex(BL8025ReadOneByte(BL8025_DAY_ADD));
	
	Current_Week = BL8025ReadOneByte(BL8025_WEEK_ADD);	
	
	switch(Current_Week)
	{
		case 0x01:	Current_Week = 0;break;
		case 0x02:	Current_Week = 1;break;
		case 0x04:	Current_Week = 2;break;
		case 0x08:	Current_Week = 3;break;
		case 0x10:	Current_Week = 4;break;
		case 0x20:	Current_Week = 5;break;
		case 0x40:	Current_Week = 6;break;
		
		default:break;
	}
}
void Print_Time(void)  //打印时间出来
{
	USART_Send_Char(2+0x30);
	USART_Send_Char(0+0x30);
	USART_Send_Char(Current_Year/10+0x30);
	USART_Send_Char(Current_Year%10+0x30);
	USART_Send_Char('-');
	USART_Send_Char(Current_Month/10+0x30);
	USART_Send_Char(Current_Month%10+0x30);
	USART_Send_Char('-');
	USART_Send_Char(Current_Day/10+0x30);
	USART_Send_Char(Current_Day%10+0x30);
	USART_Send_Char(' ');
	USART_Send_Char(Current_Hour/10+0x30);
	USART_Send_Char(Current_Hour%10+0x30);
	USART_Send_Char(':');
	USART_Send_Char(Current_Minute/10+0x30);
	USART_Send_Char(Current_Minute%10+0x30);
	USART_Send_Char(':');
	USART_Send_Char(Current_Second/10+0x30);
	USART_Send_Char(Current_Second%10+0x30);
	USART_Send_Char(' ');
	USART_Send_Char(' ');
	USART_Send_Char(' ');
	USART_Send_Char(Current_Week%10+0x30);
	USART_Send_String((char*)"\r\n ");
}
void BL8025_Process(void)
{
	int8_t i;
	
	if(BL8025TimerMs > 1000)
	{
		BL8025TimerMs = 0;
		
		BL8025ReadDateTime();//读取年月日时分秒星期
		
		Current_Second_Test = Current_Second;
		
//		Print_Time();		 //打印数据，调试用
		if(Current_SecondBak != Current_Second)		//计时单位为秒  
		{
			Current_SecondBak = Current_Second;  
			
			for(i=0;i<8;i++)			//1-8号继电器防抖动时间计时
			{
				if(u8_RelayStatus1_8 & (1<<i))
				{
					if((u16_RelayCurrentBuf[i] < MIN_CURRENT_CHECK)) //电流小于90，说明没有在充电，也有可能是插座抖动导致。这里做累加计时判断，当电流小于90的时候大于系统设置，则判断为没有在充电
					{
						u8_JitterRunTimeBuf[i]++;
					}
					else
					{
						u8_JitterRunTimeBuf[i] = 0;
						
					}
				}
			}
			for(i=0;i<8;i++)	//9-16号继电器防抖动时间计时
			{
				if(u8_RelayStatus9_16 & (1<<i))
				{
					if((u16_RelayCurrentBuf[i+8] < MIN_CURRENT_CHECK))
					{
						u8_JitterRunTimeBuf[i+8]++;
					}
					else
					{
						u8_JitterRunTimeBuf[i+8] = 0;
					}
				}
			}
			for(i=0;i<8;i++)	//17-24号继电器防抖动时间计时
			{
				if(u8_RelayStatus17_24 & (1<<i))
				{
					if((u16_RelayCurrentBuf[i+16] < MIN_CURRENT_CHECK))
					{
						u8_JitterRunTimeBuf[i+16]++;
					}
					else
					{
						u8_JitterRunTimeBuf[i+16] = 0;
					}
				}
			}
			for(i=0;i<8;i++)			//25-32号继电器防抖动时间计时
			{
				if(u8_RelayStatus25_32 & (1<<i))
				{
					if((u16_RelayCurrentBuf[i+24] < MIN_CURRENT_CHECK))
					{
						u8_JitterRunTimeBuf[i+24]++;
					}
					else
					{
						u8_JitterRunTimeBuf[i+24] = 0;
					}
				}
			}
		}
		if(Current_MinuteBak != Current_Minute)			//正常运行按分钟计时，用这三行
		{
			Current_MinuteBak = Current_Minute;
		
//		if(Current_Second_TestBak != Current_Second_Test)		//测试按秒计时，用这三行
//		{
//			Current_Second_TestBak = Current_Second_Test;
			
			for(i=0;i<8;i++)			//1-8号继电器运行时间计时
			{
				if(u8_RelayStatus1_8 & (1<<i))
				{
					u8_HalfHourCntBuf[i]++;
					if(u8_HalfHourCntBuf[i] >= 30)			//时间单位是30分钟，即0.5小时
					{
						u8_HalfHourCntBuf[i] = 0;
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i]++;			//每0.5小时加1
						
						if(u8_TrickleCurrentStatus1_8 & (1<<i))			//涓流时间计时
						{
							u8_TrickleCurrentTimeBuf1_8[i]++;			//每0.5小时加1
						}
					}
				}
			}
			//以下为9-16,17-24和25-32号继电器计时，计时如上所示
			for(i=0;i<8;i++)
			{
				if(u8_RelayStatus9_16 & (1<<i))
				{
					u8_HalfHourCntBuf[i+8]++;
					if(u8_HalfHourCntBuf[i+8] >= 30)
					{
						u8_HalfHourCntBuf[i+8] = 0;
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8]++;
						
						if(u8_TrickleCurrentStatus9_16 & (1<<i))
						{
							u8_TrickleCurrentTimeBuf9_16[i]++;
						}
					}
					
				}
			}
			
			for(i=0;i<8;i++)
			{
				if(u8_RelayStatus17_24 & (1<<i))
				{
					u8_HalfHourCntBuf[i+16]++;
					if(u8_HalfHourCntBuf[i+16] >= 30)
					{
						u8_HalfHourCntBuf[i+16] = 0;
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16]++;
						
						if(u8_TrickleCurrentStatus17_24 & (1<<i))
						{
							u8_TrickleCurrentTimeBuf17_24[i]++;
						}
					}
					
				}
			}	

			for(i=0;i<8;i++)
			{
				if(u8_RelayStatus25_32 & (1<<i))
				{
					u8_HalfHourCntBuf[i+24]++;
					if(u8_HalfHourCntBuf[i+24] >= 30)
					{
						u8_HalfHourCntBuf[i+24] = 0;
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24]++;
						
						if(u8_TrickleCurrentStatus25_32 & (1<<i))
						{
							u8_TrickleCurrentTimeBuf25_32[i]++;
						}
					}
					
				}
			}			
		}
	}
}
void BL8025_TimerMS(void)
{
	BL8025TimerMs++;
}

