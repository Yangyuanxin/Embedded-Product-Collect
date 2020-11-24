#include "ACPowerDetection.h" 
#include "delay.h" 
#include "RS485.h"
#include "CRC.h"

uint8_t u8_ACPowerStatus;
__IO uint16_t u16_ACPowerDetectionTimerMs;
//IO初始化
void ACPowerDetection_GPIO_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

  //GPIOA4初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	u8_ACPowerStatus = 1;
}
void ACPowerDetection_Process(void)
{
	if(u16_ACPowerDetectionTimerMs > 10000)		//每10秒检测一次
	{
		u16_ACPowerDetectionTimerMs = 0;
		
		u8_AlarmStatus &= ~0x80;
		  
		if(ACPowerIN() == 0)			//  交流不存在  光耦不导通，所以拉低
		{
			delay_ms(10);
			if(ACPowerIN() == 0)
			{
//				u8_ACPowerStatus = 1;
				u8_AlarmStatus |= 0x80;  //给予警告
			}
		}
	}
}
void ACPowerDetection_Timer_Ms(void)
{
	u16_ACPowerDetectionTimerMs++;
}





