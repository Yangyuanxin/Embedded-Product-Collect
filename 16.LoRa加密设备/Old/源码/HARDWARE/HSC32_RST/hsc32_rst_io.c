#include "hsc32_rst_io.h"


//初始化LORA电源使能引脚，并打开电源
void HSC32RstInit(void)
{	
	GPIO_InitTypeDef 	GPIO_InitStructure;					//定义结构体
	
	RCC_APB2PeriphClockCmd( HSC32IO_RCC, ENABLE); 			//开启GPIOC的外设时钟
	
	GPIO_InitStructure.GPIO_Pin 	= HSC32IO_PIN;			//选择要控制的GPIOB引脚
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;  	//设置引脚模式为通用推挽输出
	GPIO_InitStructure.GPIO_Speed  	= GPIO_Speed_50MHz; 	//设置引脚速率为50MHz 
	GPIO_Init(HSC32IO_PORT, &GPIO_InitStructure);			//调用库函数，初始化GPIO
	
	HSC_RST_PIN = 0;
	delay_ms(500);
	HSC_RST_PIN = 1;
}