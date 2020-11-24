#include"shui.h"

void Shui_init(void)
{
    GPIO_InitTypeDef GPIO_Initstruct;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
 
    //--------------GPIO时钟初始化-=-------------------//
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
    //--------------KEY0初始化-------------------------//
    GPIO_Initstruct.GPIO_Mode=GPIO_Mode_IPD;
		GPIO_Initstruct.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_Initstruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_Initstruct);		
}

