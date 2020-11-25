/**
  ******************************************************************************
  * @file    key.c
  * @author  xiaoyuan
  * @version V1.0
  * @date    2016-04-12
  * @brief   This file provides all the LED functions.
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "delay.h"
//#include "sys.h"
//初始化PC0和PD12为输出口.并使能这两个口的时钟		    
//KEY IO初始化
void KEY_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC,PD端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;				 //KEY端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //设置成上拉输入
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.0
	
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//PA15
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA15
//	
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;//PC5
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
// 	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC5
// 
//	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
//	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //KEY端口配置
 //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //设置成上拉输入
	//GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOD.12
 
}
//按键扫描函数（采用延时消抖）
//mode：0，不支持长按 1，支持长按
//返回按键值 返回值：0，无按键按下。1，KEY0按下。2，KEY1按下
u8 KEY_Scan(uint8_t mode)
{
	static uint8_t key_up = 1;
	if(mode) key_up = 1;
	if(key_up&&(KEY_U==0||KEY_D==0||KEY_L==0||KEY_R==0||KEY_M==0))
	{
//		delay_ms(10); 放在外部中断中执行，不需要延时
		key_up = 0;
		if(KEY_U == 0) return 1;
		else if(KEY_D == 0) return 2;
		else if(KEY_L == 0) return 3;
		else if(KEY_R == 0) return 4;
		else if(KEY_M == 0) return 5;
	}
	else if(KEY_U == 1&&KEY_D ==1&&KEY_L ==1&&KEY_R ==1&&KEY_M ==1) 
		key_up =1;
	return 0;
}
