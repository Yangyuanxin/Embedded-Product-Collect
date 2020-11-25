#include "lora_pwr.h"


//初始化LORA电源使能引脚，并打开电源
void LoRaPWR_Init(void)
{	
	GPIO_InitTypeDef 	GPIO_InitStructure;					//定义结构体
	
	RCC_APB2PeriphClockCmd( LORA_PWR_RCC, ENABLE); 			//开启GPIOC的外设时钟
															   
	GPIO_InitStructure.GPIO_Pin 	= LORA_PWR_PIN;			//选择要控制的GPIOB引脚
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;  	//设置引脚模式为通用推挽输出
	GPIO_InitStructure.GPIO_Speed  	= GPIO_Speed_50MHz; 	//设置引脚速率为50MHz 
	GPIO_Init(LORA_PWR_PORT, &GPIO_InitStructure);			//调用库函数，初始化GPIO

	GPIO_SetBits(LORA_PWR_PORT,LORA_PWR_PIN);
}