#include "led.h"
#include "delay.h"
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
	GPIO_SetBits(GPIOE,GPIO_Pin_6);
}

void LED_Blink(uint8_t frequency)
{
	uint8_t i;
	for(i=0;i<frequency;i++)
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_5);
		delay_ms(100);
		GPIO_SetBits(GPIOE,GPIO_Pin_5);
		delay_ms(100);
	}
}

