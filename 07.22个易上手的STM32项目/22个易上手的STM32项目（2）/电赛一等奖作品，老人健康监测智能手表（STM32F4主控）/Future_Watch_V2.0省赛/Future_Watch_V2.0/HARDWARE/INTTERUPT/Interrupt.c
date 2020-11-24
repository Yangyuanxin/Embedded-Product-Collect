#include  "Interrupt.h"

void Interrupt_Set(void)
{
	MY_NVIC_Init(2,1,TIM1_CC_IRQn,2);               //捕获中断优先级设置；
	MY_NVIC_Init(3,2,TIM4_IRQn,3);					//定时器4
	MY_NVIC_Init(3,3,USART1_IRQn,2);				//串口 
}
