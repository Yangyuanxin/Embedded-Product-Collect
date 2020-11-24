#include "Relay.h"
#include "RS485.h"
#include "w25qxx.h"
#include "BL8025.h"
#include "Current.h"
#include "usart.h"

void Relay_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOD E时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//带上拉
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化 
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化 
	
	GPIO_Write(GPIOD,0x0000);		 //All off
	GPIO_Write(GPIOE,0x0000);		//All off
}

void Relay_Out(void)
{
	uint8_t i;
	uint16_t temp = 0;
	
	temp = GPIO_ReadOutputData(GPIOE);
	
	for(i=0;i<8;i++)
	{
		temp = GPIO_ReadOutputData(GPIOE);
		
		if(u8_RelayStatus1_8 & (1<<i))
		{
			GPIO_Write(GPIOE,(temp | (1<<i)));
		}
		else
		{
			GPIO_Write(GPIOE,(temp & (~(1<<i))));
		}
	}
	
	for(i=0;i<8;i++)
	{
		temp = GPIO_ReadOutputData(GPIOE);
		if(u8_RelayStatus9_16 & (1<<i))
		{
			GPIO_Write(GPIOE,(temp | (0x0001<<(i+8))));
		}
				else
		{
			GPIO_Write(GPIOE,(temp & (~(0x0001<<(i+8)))));
		}
	}
	
	temp = GPIO_ReadOutputData(GPIOD);
	
	for(i=0;i<8;i++)
	{
		temp = GPIO_ReadOutputData(GPIOD);
		if(u8_RelayStatus17_24 & (1<<i))
		{
			GPIO_Write(GPIOD,(temp | (1<<i)));
		}
		else
		{
			GPIO_Write(GPIOD,(temp & (~(1<<i))));
		}
	}
	
	for(i=0;i<8;i++)
	{
		temp = GPIO_ReadOutputData(GPIOD);
		if(u8_RelayStatus25_32 & (1<<i))
		{
			GPIO_Write(GPIOD,(temp | (0x0001<<(i+8))));
		}
		else
		{
			GPIO_Write(GPIOD,(temp & (~(1<<(i+8)))));
		}
	}	
}
void Relay_Judge(void)  
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		if(u8_RelayStatus1_8 & (1<<i)) //for循环判断每一个继电器状态
		{
			if(u8_RemoveStatus1_8 & (1<<i))//已经移除设备，关闭继电器
			{
//				printf("\r\n 判断后已经移除！！\r\n");
				u8_RelayStatus1_8 &= ~(1<<i);//继电器状态设置为0  关闭
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0;//继电器工作时间设置为0
				u8_TrickleCurrentTimeBuf1_8[i] = 0;//涓流时间设置为0
				u8_TrickleCurrentStatus1_8 &=~(1<<i);//涓流状态设置为0  正常 不涓流
				u8_RemoveStatus1_8 &= ~(1<<i);//移除状态设置为0
				u8_RelayRemoveCntBuf[i] = 0;//用于记录满足电流小于3和运行时间大于30分钟的计数，当值大于10则说明移除设备，这里清零，重新计数
				u8_JitterRunTimeBuf[i] = 0;//防抖动时间
				u16_RelayCurrentBuf[i] = 0;//继电器电流值		
			}
//			if((uint8_t)(u16_RelayCurrentBuf[i]/500) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//用电电流大于最大供电功率
			if((uint8_t)(u16_RelayCurrentBuf[i]/100) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//用电电流大于最大供电功率
			{
//				printf("\r\n 判断后大于最大供电功率！！\r\n");
				u8_RelayStatus1_8 &= ~(1<<i);//继电器状态设置为0  关闭
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0;  //继电器状态工作时间设置为0
				u8_TrickleCurrentTimeBuf1_8[i] = 0;//涓流时间设置为0
				u8_TrickleCurrentStatus1_8 &= ~(1<<i);//涓流状态设置为0
				u8_RemoveStatus1_8 &= ~(1<<i); //移除状态设置为0 
				u8_RelayRemoveCntBuf[i] = 0;  //
				u8_JitterRunTimeBuf[i] = 0;		//防抖动时间
				u8_RelayOverCurrentStatus1_8 |= (1<<i);//继电器过流状态 
				u16_RelayCurrentBuf[i] = 0;	//当前继电器电流值			
			}
			if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i] == 0x02)		//计时模式
			{
				//运行时间大于设定时间，关闭继电器
					if(uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] >= uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[i]) //工作时间大于系统所设置的时间
					{
//					printf("\r\n 计时模式判断后大于设置时间！！\r\n");
						u8_RelayStatus1_8 &= ~(1<<i);  //继电器状态设置为0 关闭
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0; //继电器工作时间为关闭
						u8_TrickleCurrentTimeBuf1_8[i] = 0;     //继电器涓流时间
						u8_TrickleCurrentStatus1_8 &= ~(1<<i);  //继电器涓流状态为0  正常 不是涓流
						u8_RemoveStatus1_8 &= ~(1<<i);   //继电器移除状态设置为0
						u8_RelayRemoveCntBuf[i] = 0;     //用于记录满足电流小于3和运行时间大于30分钟的计数，当值大于10则说明移除设备，这里清零，重新计数
						u8_JitterRunTimeBuf[i] = 0;      //防抖动时间设置为0
						u16_RelayCurrentBuf[i] = 0;      //继电器当前电流值设置为0
					}
					
			}		
			else if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i] == 0x01)		//自停模式
			{
				//运行时间大于最大运行时间或者涓流时间大于最大涓流供电时间，关闭继电器
				if((uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] >= uni_SysStatus.Str.u8_MaxPowerSupplyTime)
					|| (u8_TrickleCurrentTimeBuf1_8[i] >= uni_SysStatus.Str.u8_TricklePowerSupplyTime))
				{
//					printf("\r\n 自停模式判断后大于最大运行时间或者涓流大于最大涓流供电时间！！\r\n");
					u8_RelayStatus1_8 &= ~(1<<i); //继电器设置为0 关闭状态
					uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0;//继电器工作时间
					u8_TrickleCurrentTimeBuf1_8[i] = 0; //继电器涓流时间
					u8_TrickleCurrentStatus1_8 &= ~(1<<i);//继电器涓流状态
					u8_RemoveStatus1_8 &= ~(1<<i);//继电器移除状态 为0正常
					u8_RelayRemoveCntBuf[i] = 0;//
					u8_JitterRunTimeBuf[i] = 0;//继电器防抖动时间
					u16_RelayCurrentBuf[i] = 0;//继电器电流值
				}
			}
		}
	}
	
	for(i=0;i<8;i++)
	{
		if(u8_RelayStatus9_16 & (1<<i))
		{
			if(u8_RemoveStatus9_16 & (1<<i))		//已经移除设备，关闭继电器
			{
				u8_RelayStatus9_16 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] = 0;
				u8_TrickleCurrentTimeBuf9_16[i] = 0;
				u8_TrickleCurrentStatus9_16 &= ~(1<<i);
				u8_RemoveStatus9_16 &= ~(1<<i);	
				u8_RelayRemoveCntBuf[i+8] = 0;	
				u8_JitterRunTimeBuf[i+8] = 0;	
				u16_RelayCurrentBuf[i+8] = 0;				
			}
			if((uint8_t)(u16_RelayCurrentBuf[i+8]/100) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//用电电流大于最大供电功率
			{
				u8_RelayStatus9_16 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] = 0;
				u8_TrickleCurrentTimeBuf9_16[i] = 0;
				u8_TrickleCurrentStatus9_16 &= ~(1<<i);
				u8_RemoveStatus9_16 &= ~(1<<i);	
				u8_RelayRemoveCntBuf[i+8] = 0;	
				u8_JitterRunTimeBuf[i+8] = 0;		
				u8_RelayOverCurrentStatus9_16 |= (1<<i);	
				u16_RelayCurrentBuf[i+8] = 0;					
			}			
			if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+8] == 0x02)	//计时模式
			{
					if(uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] >= uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[i+8]) 
					{
						u8_RelayStatus9_16 &= ~(1<<i);
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] = 0;
						u8_TrickleCurrentTimeBuf9_16[i] = 0;
						u8_TrickleCurrentStatus9_16 &= ~(1<<i);
						u8_RemoveStatus9_16 &= ~(1<<i);		
						u8_RelayRemoveCntBuf[i+8] = 0;
						u16_RelayCurrentBuf[i+8] = 0;	
					}
			}		
			else if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+8] == 0x01)	//自停模式
			{
				if((uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] >= uni_SysStatus.Str.u8_MaxPowerSupplyTime)
					||(u8_TrickleCurrentTimeBuf9_16[i] >= uni_SysStatus.Str.u8_TricklePowerSupplyTime))
				{
						u8_RelayStatus9_16 &= ~(1<<i);
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] = 0;
						u8_TrickleCurrentTimeBuf9_16[i] = 0;
						u8_TrickleCurrentStatus9_16 &= ~(1<<i);
						u8_RemoveStatus9_16 &= ~(1<<i);
						u8_RelayRemoveCntBuf[i+8] = 0;
						u8_JitterRunTimeBuf[i+8] = 0;	
						u16_RelayCurrentBuf[i+8] = 0;	
				}
			}
		}
	}	
	
	for(i=0;i<8;i++)
	{
		if(u8_RelayStatus17_24 & (1<<i))
		{
		  if(u8_RemoveStatus17_24 & (1<<i))		//已经移除设备，关闭继电器
			{
				u8_RelayStatus17_24 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] = 0;
				u8_TrickleCurrentTimeBuf17_24[i] = 0;
				u8_TrickleCurrentStatus17_24 &= ~(1<<i);
				u8_RemoveStatus17_24 &= ~(1<<i);
				u8_RelayRemoveCntBuf[i+16] = 0;
				u8_JitterRunTimeBuf[i+16] = 0;	
				u16_RelayCurrentBuf[i+16] = 0;	
			}
			if((uint8_t)(u16_RelayCurrentBuf[i+16]/100) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//用电电流大于最大供电功率
			{
				u8_RelayOverCurrentStatus17_24 |= (1<<i);
				
				u8_RelayStatus17_24 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] = 0;
				u8_TrickleCurrentTimeBuf17_24[i] = 0;
				u8_TrickleCurrentStatus17_24 &= ~(1<<i);
				u8_RemoveStatus17_24 &= ~(1<<i);
				u8_RelayRemoveCntBuf[i+16] = 0;
				u8_JitterRunTimeBuf[i+16] = 0;
				u16_RelayCurrentBuf[i+16] = 0;				
			}			
			if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+16] == 0x02)
			{
					if(uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] >= uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[i+16]) 
					{
						u8_RelayStatus17_24 &= ~(1<<i);
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] = 0;
						u8_TrickleCurrentTimeBuf17_24[i] = 0;
						u8_TrickleCurrentStatus17_24 &= ~(1<<i);
						u8_RemoveStatus17_24 &= ~(1<<i);
						u8_RelayRemoveCntBuf[i+16] = 0;
						u8_JitterRunTimeBuf[i+16] = 0;	
						u16_RelayCurrentBuf[i+16] = 0;
					}
			}	
			else if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+16] == 0x01)
			{
				if((uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] >= uni_SysStatus.Str.u8_MaxPowerSupplyTime)
					||(u8_TrickleCurrentTimeBuf17_24[i] >= uni_SysStatus.Str.u8_TricklePowerSupplyTime))
				{
					u8_RelayStatus17_24 &= ~(1<<i);
					uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] = 0;
					u8_TrickleCurrentTimeBuf17_24[i] = 0;
					u8_TrickleCurrentStatus17_24 &= ~(1<<i);
					u8_RemoveStatus17_24 &= ~(1<<i);
					u8_RelayRemoveCntBuf[i+16] = 0;
					u8_JitterRunTimeBuf[i+16] = 0;	
					u16_RelayCurrentBuf[i+16] = 0;
				}
			}
	
		}
	}	

	for(i=0;i<8;i++)
	{
		if(u8_RelayStatus25_32 & (1<<i))		
		{
			if(u8_RemoveStatus25_32 & (1<<i))		//已经移除设备，关闭继电器
			{
				u8_RelayStatus25_32 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] = 0;
				u8_TrickleCurrentTimeBuf25_32[i] = 0;
				u8_TrickleCurrentStatus25_32 &= ~(1<<i);
				u8_RemoveStatus25_32 &= ~(1<<i);
				u8_RelayRemoveCntBuf[i+24] = 0;
				u8_JitterRunTimeBuf[i+24] = 0;	
				u16_RelayCurrentBuf[i+24] = 0;
			}
			if((uint8_t)(u16_RelayCurrentBuf[i+24]/100) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//用电电流大于最大供电功率
			{
				u8_RelayOverCurrentStatus25_32 |= (1<<i);
				
				u8_RelayStatus25_32 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] = 0;
				u8_TrickleCurrentTimeBuf25_32[i] = 0;
				u8_TrickleCurrentStatus25_32 &= ~(1<<i);
				u8_RemoveStatus25_32 &= ~(1<<i);
				u8_RelayRemoveCntBuf[i+24] = 0;
				u8_JitterRunTimeBuf[i+24] = 0;	
        u16_RelayCurrentBuf[i+24] = 0;				
			}				
			if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+24] == 0x02)
			{
					if(uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] >= uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[i+24]) 
					{
						u8_RelayStatus25_32 &= ~(1<<i);
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] = 0;
						u8_TrickleCurrentTimeBuf25_32[i] = 0;
						u8_TrickleCurrentStatus25_32 &= ~(1<<i);
						u8_RemoveStatus25_32 &= ~(1<<i);
						u8_RelayRemoveCntBuf[i+24] = 0;
						u8_JitterRunTimeBuf[i+24] = 0;
						u16_RelayCurrentBuf[i+24] = 0;
					}
			}		
			else if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+24] == 0x01)
			{
				if((uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] >= uni_SysStatus.Str.u8_MaxPowerSupplyTime)
					||(u8_TrickleCurrentTimeBuf25_32[i] >= uni_SysStatus.Str.u8_TricklePowerSupplyTime))
				{
					u8_RelayStatus25_32 &= ~(1<<i);
					uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] = 0;
					u8_TrickleCurrentTimeBuf25_32[i] = 0;
					u8_TrickleCurrentStatus25_32 &= ~(1<<i);
					u8_RemoveStatus25_32 &= ~(1<<i);
					u8_RelayRemoveCntBuf[i+24] = 0;
					u8_JitterRunTimeBuf[i+24] = 0;
					u16_RelayCurrentBuf[i+24] = 0;
				}
			}
		}
	}


	if(u1_NeedDectCurrentF) //需要检测电流
	{  //电流检测的时间小于系统设定的电流检测通电时长
		if(u16_CurrentForceCheckTimeS < uni_SysStatus.Str.u8_CurrentDetectionPowerTime)
		{
			if(u8_NeedDectCh < 9)  //由于之前检测电流时，发现没有开启这里先开启 继电器 方便在电流检测时进行检测
			{
				u8_RelayStatus1_8 |= 0x01<<(u8_NeedDectCh-1);  
			}
			else 	if(u8_NeedDectCh < 17)
			{
				u8_RelayStatus9_16 |= 0x01<<(u8_NeedDectCh-9);
			}
			else 	if(u8_NeedDectCh < 25)
			{
				u8_RelayStatus17_24 |= 0x01<<(u8_NeedDectCh-17);
			}
			else 	if(u8_NeedDectCh < 33)
			{
				u8_RelayStatus25_32 |= 0x01<<(u8_NeedDectCh-25);
			}
		}
		else
		{
			u1_NeedDectCurrentF = 0;
			u1_ForceDectCurrentEndF = 1;  //电流检测的时间大于系统设定的电流检测通电时长  表示检测结束
			
			if(u8_NeedDectCh < 9)
			{
				u8_RelayStatus1_8 &= ~(0x01<<(u8_NeedDectCh-1)); //关闭继电器
			}
			else 	if(u8_NeedDectCh < 17)
			{
				u8_RelayStatus9_16 &= ~(0x01<<(u8_NeedDectCh-9));
			}
			else 	if(u8_NeedDectCh < 25)
			{
				u8_RelayStatus17_24 &= ~(0x01<<(u8_NeedDectCh-17));
			}
			else 	if(u8_NeedDectCh < 33)
			{
				u8_RelayStatus25_32 &= ~(0x01<<(u8_NeedDectCh-25));
			}				
		}
	}	
}
void Relay_Process(void)
{
	Relay_Judge();  //继电器状态判断
	Relay_Out();//控制继电器开关
	
	uni_SysStatus.Str.u8_RelayStatusBuf[0] = u8_RelayStatus1_8;
	uni_SysStatus.Str.u8_RelayStatusBuf[1] = u8_RelayStatus9_16;
	uni_SysStatus.Str.u8_RelayStatusBuf[2] = u8_RelayStatus17_24;
	uni_SysStatus.Str.u8_RelayStatusBuf[3] = u8_RelayStatus25_32;
	
	u8_RelayStatus1_8Bak = u8_RelayStatus1_8;
	u8_RelayStatus9_16Bak = u8_RelayStatus9_16;
	u8_RelayStatus17_24Bak = u8_RelayStatus17_24;
	u8_RelayStatus25_32Bak = u8_RelayStatus25_32;
}











