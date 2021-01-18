#include "beeper.h"
#include "delay.h"
#include "main.h"
//蜂鸣器IO初始化
void BEEPER_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
	//初始化 蜂鸣器引脚PB9	  推挽输出
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);//输出低电平
}
//蜂鸣器短响一次
void beeperOnce(void)
{
	if (setData.beeperFlag) {
		for (u8 i=0; i<255; i++) {
			Beeper = 1;
			delay_us(125);
			Beeper = 0;
			delay_us(125);
		}
	}
}
