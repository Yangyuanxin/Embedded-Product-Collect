#include "motor.h"


//马达初始化
void Motor_Init(void)
{
	GPIO_Clock_Set(IOPBEN);		//使能PB时钟
	GPIO_Init(GPIOB,P1,IO_MODE_OUT,IO_SPEED_50M,IO_OTYPER_PP,IO_KEEP,IO_L);	//PB1 推挽输出低
}
