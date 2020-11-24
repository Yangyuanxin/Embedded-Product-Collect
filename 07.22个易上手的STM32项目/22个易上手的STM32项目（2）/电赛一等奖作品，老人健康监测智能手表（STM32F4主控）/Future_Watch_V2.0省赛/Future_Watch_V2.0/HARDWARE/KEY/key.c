#include "key.h"

//按键初始化
void Key_Init(void)
{
	GPIO_Clock_Set(IOPBEN);		//使能PB时钟
	GPIO_Init(GPIOB,P13,IO_MODE_OUT,IO_SPEED_50M,IO_OTYPER_PP,IO_KEEP,IO_L);	//PB13 推挽输出低
	GPIO_Init(GPIOB,P12,IO_MODE_IN,IO_SPEED_50M,IO_OTYPER_PP,IO_DOWN,IO_L);		//PB12 下拉输入低
}

