#include"MOTOR.H"

void Motor_Init(void)
{
		GPIO_InitTypeDef GPIO_Initstruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
    GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Initstruct.GPIO_Pin=GPIO_Pin_14;
    GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_Initstruct);
    GPIO_SetBits(GPIOB,GPIO_Pin_14); 
	
		GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Initstruct.GPIO_Pin=GPIO_Pin_15;
    GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_Initstruct);
    GPIO_SetBits(GPIOB,GPIO_Pin_15); 
}

void Jinshui(void)
{
	MOTOR_A=1;
	MOTOR_B=0;
}

void Chushui(void)
{
	MOTOR_A=0;
	MOTOR_B=1;
}

void Tingshui(void)
{
	MOTOR_A=0;
	MOTOR_B=0;
}
