 #include "led.h"
 #include "stm32f10x.h"
 
 void Led_init(void)
 {
    GPIO_InitTypeDef GPIO_Initstruct;
 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
    GPIO_Initstruct.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Initstruct.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
    GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_Initstruct);
	 
    GPIO_SetBits(GPIOB,GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13);     
}
