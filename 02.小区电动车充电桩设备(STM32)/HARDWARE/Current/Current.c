#include "Current.h"
#include "delay.h"
#include "usart.h"
#include "RS485.h"
#include "w25qxx.h" 
#include "BL8025.h"
#include <math.h>

uint8_t u8_CurrentSelCh;
uint16_t u16_ReadVlaue[10];
uint8_t u8_ReadCnt = 0;

uint16_t u16_RelayCurrentADCBuf[32];
uint16_t u16_RelayCurrentBuf[32];
uint16_t u16_ADCBuf[100];
uint8_t u8_RelayRemoveCntBuf[32];

UniBitDef uniCurrentDectect;
uint8_t u8_NeedDectCh;

__IO uint16_t u16_CurrentCheckTimeMs;
__IO uint16_t u16_CurrentForceCheckTimeMs;
__IO uint16_t u16_CurrentForceCheckTimeS;
uint16_t u16_IsOnlineCheckTimeS;

void Current_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
	
  //先初始化ADC3通道0 1 IO口
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
	
	//控制通道引脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;//PC0--9
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//带上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化  
 
	GPIO_SetBits(GPIOC,GPIO_Pin_8);		//74HC4067 芯片禁止
	GPIO_SetBits(GPIOC,GPIO_Pin_9);
	
	Adc3_Init();//初始化ADC3
}

void SetADCCh(uint8_t ch)
{
	uint16_t temp;
	
	if(ch < 17)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
		
		temp = GPIO_ReadOutputData(GPIOC);
		temp &= 0xfff0;
		switch(ch)
		{
			case 1: temp |= 0x0000;break;
			case 2: temp |= 0x0008;break;
			case 3: temp |= 0x0004;break;
			case 4: temp |= 0x000c;break;
			case 5: temp |= 0x0001;break;
			case 6: temp |= 0x0009;break;
			case 7: temp |= 0x0005;break;
			case 8: temp |= 0x000d;break;
			case 9: temp |= 0x0002;break;
			case 10: temp |= 0x000a;break;
			case 11: temp |= 0x0006;break;
			case 12: temp |= 0x000e;break;
			case 13: temp |= 0x0003;break;
			case 14: temp |= 0x000b;break;
			case 15: temp |= 0x0007;break;
			case 16: temp |= 0x000f;break;
			default:break;
		}
//		temp |= (ch-1);
		
		GPIO_Write(GPIOC,temp);
	}
	else
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
		
		temp = GPIO_ReadOutputData(GPIOC);
		temp &= 0xff0f;
		
		switch(ch)
		{
			case 17: temp |= 0x0000;break;
			case 18: temp |= 0x0080;break;
			case 19: temp |= 0x0040;break;
			case 20: temp |= 0x00c0;break;
			case 21: temp |= 0x0010;break;
			case 22: temp |= 0x0090;break;
			case 23: temp |= 0x0050;break;
			case 24: temp |= 0x00d0;break;
			case 25: temp |= 0x0020;break;
			case 26: temp |= 0x00a0;break;
			case 27: temp |= 0x0060;break;
			case 28: temp |= 0x00e0;break;
			case 29: temp |= 0x0030;break;
			case 30: temp |= 0x00b0;break;
			case 31: temp |= 0x0070;break;
			case 32: temp |= 0x00f0;break;
			default:break;
		}
		
//		temp |= ((ch-17)<<4);
		
		GPIO_Write(GPIOC,temp);		
	}
}
uint16_t CalcRMS(void)   
{
	long 		Temp = 0;
	uint8_t i;
	
	for(i=0;i<100;i++)
	{
		//Temp = sqrt((pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + ... + pSrc[blockSize-1] * pSrc[blockSize-1]));
		Temp += u16_ADCBuf[i] * u16_ADCBuf[i];
	}
	//printf("Temp1 = %d \r\n",(uint16_t)Temp);
	//Temp /= 40;
	Temp = (uint16_t)(Temp/100);
	//printf("Temp2 = %d \r\n",(uint16_t)Temp);
	
	Temp = sqrt(Temp) * 22;//		//扩大100倍，看看测试数据
	//printf("Temp3 = %d \r\n",(uint16_t)Temp);
	return ((uint16_t)(Temp*1));
}
void ReadSetChCurrentValue(uint8_t ch)
{
	uint16_t  temp_val=0;
	uint8_t i;
	uint16_t max = 0;
	
	uint16_t SetCurrent = 0;
	
	SetADCCh(ch);

	if(ch < 17)  //判断通道  读AD值
	{

		for(i=0;i<100;i++)
		{
			temp_val = Get_Adc3(ADC_Channel_0);
			u16_ADCBuf[i] = temp_val;
			
			if(temp_val > max)
			{
				max = temp_val;
			}
			delay_us(200);
		}	
//		for(i=0;i<40;i++)
//		{
//			printf("%d \r\n",u16_ADCBuf[i]);
//		}
//		printf("\r\n");
	}
	else if(ch < 33)
	{
		for(i=0;i<100;i++)
		{
			temp_val = Get_Adc3(ADC_Channel_1);
			u16_ADCBuf[i] = temp_val;
			
			if(temp_val > max)
			{
				max = temp_val;
			}
			delay_us(200);
		}
		
//		u16_RelayCurrentADCBuf[ch] = max;
	}
	
//		for(i=0;i<40;i++)
//		{
//			printf("%d \r\n",u16_ADCBuf[i]);
//		}
//		printf("\r\n");
	
//	u16_RelayCurrentADCBuf[ch-1] = max;
//	u16_RelayCurrentBuf[ch-1] = (uint16_t)(((float)u16_RelayCurrentADCBuf[ch-1]*100) / 9); 	
	
	u16_RelayCurrentBuf[ch-1] = CalcRMS();  //计算电流值

		//if(u16_RelayCurrentBuf[ch-1] > 10)
//		{
//			printf("\r\n 当前检测电流：");
//			printf("%d,%d\r\n ",ch,u16_RelayCurrentBuf[ch-1]);
//		}
		//  继电器电流值小于所设置的电流值则设置为无负载，且抖动的时间大于系统设置值=>说明移除设备了
		if((u16_RelayCurrentBuf[ch-1] < MIN_CURRENT_CHECK) && (u8_JitterRunTimeBuf[ch-1] >= uni_SysStatus.Str.u8_JitterTime))		//ADC值小于90则认为是无负载，涓流0.27A为是300
		{ 
			u8_RelayRemoveCntBuf[ch-1]++;		//用于记录满足电流小于90和运行时间大于30分钟，就计数
			if(u8_RelayRemoveCntBuf[ch-1] > CHECK_READ_TIMES)	//检测到10次小于()，则认为是移除设备
			{
//				printf("\r\n 设备移除检测！！\r\n");
				
				u8_RelayRemoveCntBuf[ch-1] = 0;
				u8_JitterRunTimeBuf[ch-1] = 0;
				
				//u1_isOnlineDectEndF = 1;
				
				if(ch<9)
				{
					if(u8_RelayStatus1_8 & (1<<(ch-1)))		//当检测电流小于最低电流时
					{
						if((u8_TrickleCurrentStatus1_8 & (1<<(ch-1))) == 0)			//如果不处于涓流状态，则认为是用电设备移除
						{
							u8_RemoveStatus1_8 |= (1<<(ch-1));  //不处于涓流，说明已经移除，把移除标志置为1
							
							if(!u1_isOnlineDectEndF)
							{
								u8_RemoveStatus1_8Bak = u8_RemoveStatus1_8;
							}
//							u8_RemoveStatus1_8Bak = u8_RemoveStatus1_8;
//							printf("\r\n 设备移除！！\r\n");
						}
//						u8_RelayRemoveCntBuf[i] = 0;
					}
				}
				else if(ch<17)
				{
					if(u8_RelayStatus9_16 & (1<<(ch-9)))
					{
						if((u8_TrickleCurrentStatus9_16 & (1<<(ch-9))) == 0)
						{
							u8_RemoveStatus9_16 |= (1<<(ch-9));
//						u8_RelayRemoveCntBuf[i-8] = 0;
//							u8_RemoveStatus9_16Bak = u8_RemoveStatus9_16;
							if(!u1_isOnlineDectEndF)
							{
								u8_RemoveStatus9_16Bak = u8_RemoveStatus9_16;
							}
						}
					}
				}
				else if(ch<25)
				{
					if(u8_RelayStatus17_24 & (1<<(ch-17)))
					{
						if((u8_TrickleCurrentStatus17_24 & (1<<(ch-17))) == 0)
						{
							u8_RemoveStatus17_24 |= (1<<(ch-17));
//						u8_RelayRemoveCntBuf[i-16] = 0;
							
//							u8_RemoveStatus17_24Bak = u8_RemoveStatus17_24;
							if(!u1_isOnlineDectEndF)
							{
								u8_RemoveStatus17_24Bak = u8_RemoveStatus17_24;
							}
						}
					}
				}
				else if(ch<33)
				{
					if(u8_RelayStatus25_32 & (1<<(ch-25)))
					{
						if((u8_TrickleCurrentStatus25_32 & (1<<(ch-25))) == 0)
						{
							u8_RemoveStatus25_32 |= (1<<(ch-25));
//						u8_RelayRemoveCntBuf[i-24] = 0;
//							u8_RemoveStatus25_32Bak = u8_RemoveStatus25_32;
							if(!u1_isOnlineDectEndF)
							{
								u8_RemoveStatus25_32Bak = u8_RemoveStatus25_32;
							}
						}
					}
				}
			}
		}
		else		//未移除		
		{				
			u8_RelayRemoveCntBuf[ch-1] = 0;
			
			SetCurrent = (uint16_t)uni_SysStatus.Str.u8_TrickleDetectionThreshold*10;			//设置电流扩大了1000倍（单位是0.01A）
//			if(u16_RelayCurrentBuf[i] < uni_SysStatus.Str.u8_TrickleDetectionThreshold)		
			//检测电流已放大1000倍
			if((u16_RelayCurrentBuf[ch-1] < SetCurrent) 				//电流小于涓流阈值
				&& (uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[ch-1] > 0)//uni_SysStatus.Str.u8_JitterTime) 		//运行时间不等于0
				&& (uni_SysStatus.Str.u8_RelayWorkModeBuf[ch-1] != 0x02))					//不是计时模式
			{
				//printf("\r\n 电流小于设置阈值！！\r\n");
				if(ch < 9)
				{
					if(u8_RelayStatus1_8 & (1<<(ch-1)))
					{
						u8_TrickleCurrentStatus1_8 |= 1<<(ch-1);			//涓流状态
						
//						printf("\r\n 涓流状态！！\r\n");
					}
				}
				else if(ch<17)
				{
					if(u8_RelayStatus9_16 & (1<<(ch-9)))
					{
						u8_TrickleCurrentStatus9_16 |= 1<<(ch-9);
					}
				}
				else if(ch<25)
				{
					if(u8_RelayStatus17_24 & (1<<(ch-17)))
					{
						u8_TrickleCurrentStatus17_24 |= 1<<(ch-17);
					}
				}
				else if(ch<33)
				{
					if(u8_RelayStatus25_32 & (1<<(ch-25)))
					{
						u8_TrickleCurrentStatus25_32 |= 1<<(ch-25);
					}
				}
			}
			
			else		//20180904
			{
				if(ch < 9)
				{
					u8_TrickleCurrentStatus1_8 &= ~(1<<(ch-1));
				}
				else if(ch < 17)
				{
					u8_TrickleCurrentStatus9_16 &= ~(1<<(ch-9));
				}
				else if(ch < 25)
				{
					u8_TrickleCurrentStatus17_24 &= ~(1<<(ch-17));
				}
				else if(ch < 33)
				{
					u8_TrickleCurrentStatus25_32 &= ~(1<<(ch-25));
				}
			}
		}
}

uint8_t Current_Get_Val(void)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<ADC_READ_TIMES;t++)
	{
		temp_val+=Get_Adc3(ADC_Channel_0);	//读取ADC值,通道5
		delay_ms(5);
	}
	temp_val/=ADC_READ_TIMES;//得到平均值 
	if(temp_val>4000)temp_val=4000;
	return (u8)(100-(temp_val/40));
}
void Current_Process(void)  //电流检测
{
	uint8_t i;

	if(u16_CurrentCheckTimeMs > 1000)			//每1秒检测一次
	{
		u16_CurrentCheckTimeMs = 0;
	
		if(u1_isOnlineDectF)
		{
			if(u16_IsOnlineCheckTimeS < 20)///时间累加用来判断是否接入充电接口
			{
				u16_IsOnlineCheckTimeS++;
			}
		}

		for(i=0;i<8;i++)
		{
			if(u8_RelayStatus1_8 & (1<<i))  //判断继电器是否开启
			{
				ReadSetChCurrentValue(i+1); //读电流值
			}
		}
			for(i=0;i<8;i++)
		{
			if(u8_RelayStatus9_16 & (1<<i))
			{
				ReadSetChCurrentValue(i+9);
			}
		}
			for(i=0;i<8;i++)
		{
			if(u8_RelayStatus17_24 & (1<<i))
			{
				ReadSetChCurrentValue(i+17);
			}
		}
			for(i=0;i<8;i++)
		{
			if(u8_RelayStatus25_32 & (1<<i))
			{
				ReadSetChCurrentValue(i+25);
			}
		}
	//	ReadSetChCurrentValue(1);
	}
}
void Current_Timer_Ms(void)  //电流检测通电时长计算
{
	u16_CurrentCheckTimeMs++;
	
	if(u1_NeedDectCurrentF)
	{
		u16_CurrentForceCheckTimeMs++;
		if(u16_CurrentForceCheckTimeMs > 500)//500MS
		{
			u16_CurrentForceCheckTimeMs = 0;
			u16_CurrentForceCheckTimeS++;
		}
	}
	else
	{
		u16_CurrentForceCheckTimeMs = 0;
		u16_CurrentForceCheckTimeS = 0;
	}
}











