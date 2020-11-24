#include "WatchDog.h"


void WatchDogGPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//带上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  	
}	
void WatchDogGPIO_Enable(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}
void WatchDogGPIO_Disable(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}
void WatchDog_Feed(void)
{
	GPIO_ToggleBits(GPIOA,GPIO_Pin_3);
}


