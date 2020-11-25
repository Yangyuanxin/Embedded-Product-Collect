#include "beep.h"


	   

//初始化PE8为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void BEEP_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能GPIOE端口时钟
 
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //BEEP-->PE.4 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);	 //根据参数初始化GPIOE.4
 
 GPIO_ResetBits(GPIOE,GPIO_Pin_2);//输出0，关闭蜂鸣器输出


}
void BEEP_Ring(uint16_t time) //蜂鸣器短鸣
{	
	GPIO_SetBits(GPIOE,GPIO_Pin_2);  
	delay_ms(time);
	GPIO_ResetBits(GPIOE,GPIO_Pin_2);	
}

