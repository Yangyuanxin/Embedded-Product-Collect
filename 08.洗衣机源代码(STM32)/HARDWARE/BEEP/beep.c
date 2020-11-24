#include "beep.h"
#include "stm32f10x.h"


void FMQ_Init(void)
{
	GPIO_InitTypeDef GPIO_Initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Pin=GPIO_Pin_8;
	GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	
}

