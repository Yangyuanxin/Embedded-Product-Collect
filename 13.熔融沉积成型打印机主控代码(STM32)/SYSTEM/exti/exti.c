#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//外部中断 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/01  
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved	  
////////////////////////////////////////////////////////////////////////////////// 	  

uint8_t pressedkey;
extern bool poweroff_flag;
 
//外部中断初始化函数
void EXTIX_Init(void)
{
 
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef  GPIO_InitStructure;
	
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟

//	  KEY_Init();//初始化按键对应io模式
	 
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能PC,PD端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //KEY端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //设置成上拉输入
 GPIO_Init(GPIOD, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.0

    //GPIOC.5 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource0);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

//    //GPIOA.15	  中断线以及中断初始化配置
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource8);

//  	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
//  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

//    //GPIOA.0	  中断线以及中断初始化配置
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource9);

//   	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
//  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
//		
//		//GPIOA.0	  中断线以及中断初始化配置
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource10);

//   	EXTI_InitStructure.EXTI_Line=EXTI_Line10;
//  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
//		
//		//GPIOA.0	  中断线以及中断初始化配置
//  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource11);

//   	EXTI_InitStructure.EXTI_Line=EXTI_Line11;
//  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


 
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
		
//		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//  	NVIC_Init(&NVIC_InitStructure); 
// 
// 
//   	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
//  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
//  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级1
//  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//  	NVIC_Init(&NVIC_InitStructure); 
 
}

 
void EXTI0_IRQHandler(void)
{
  delay_ms(10);    //消抖
	if(PDin(0)==0)
	{	  
		poweroff_flag = 1;
//		LED0=!LED0;
//		LED1=!LED1;	
	}
	EXTI_ClearITPendingBit(EXTI_Line0);  //清除EXTI0线路挂起位
}
// void EXTI9_5_IRQHandler(void)
//{			
//	delay_ms(20);
//	if(KEY_D == 0)
//	{
//		pressedkey = 2;
//	}
//	else if(KEY_R == 0)
//  {
//	  pressedkey = 4;
//  }	
//	else if(KEY_M == 0)
//	{
//		pressedkey = 5;
//	}
//	else
//	{
//		pressedkey =0;
//	}
//	//pressedkey = KEY_Scan(0);
////	delay_ms(10);   //消抖			 
////	if(KEY0==0)	{
////		LED0=!LED0;
////		//system_task_return = 1;
////		//mui_init();
////	}
//	//else system_task_return = 0;
// 	 EXTI_ClearITPendingBit(EXTI_Line7);    //清除LINE5上的中断标志位
//   EXTI_ClearITPendingBit(EXTI_Line8);    //清除LINE5上的中断标志位 
//   EXTI_ClearITPendingBit(EXTI_Line9);    //清除LINE5上的中断标志位   
//}


//void EXTI15_10_IRQHandler(void)
//{
//	delay_ms(20);
//	if(KEY_U==0)
//	{
//		pressedkey = 1;
//	}
//	else if(KEY_L == 0)
//	{
//		pressedkey = 3;
//	}
//	else
//	{
//		pressedkey = 0;
//	}
//		
//	
//	//pressedkey = KEY_Scan(0);
////  delay_ms(10);    //消抖			 
////  if(KEY1==0)	{
////		LED1=!LED1;
////		
////	}
//	 EXTI_ClearITPendingBit(EXTI_Line10);  //清除LINE15线路挂起位
//	 EXTI_ClearITPendingBit(EXTI_Line11);    //清除LINE5上的中断标志位 
//}
