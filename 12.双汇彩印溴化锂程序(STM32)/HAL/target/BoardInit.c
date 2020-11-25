//*
//*********************************************************************************************************
//*
//*                                                 LOGO
//*
//*
//* 文 件 名 : BoardInit.c
//*
//* 文件描述 : STM32系统目标板初始化模块
//*
//* 作    者 : 
//* 版    本 : 
//* 编 译 器 : IAR EWARM 5.20
//*********************************************************************************************************
//*

#include "ucos_ii.h"
#include "stm32f10x_lib.h"
#include "head.h"

//*
//*********************************************************************************************************
//* Function : 
//* Describe : 
//*--------------------------------------------------------------------------------------------------------
//* Author   : 
//* Date     : 
//*--------------------------------------------------------------------------------------------------------
//* Mender   :
//* Date     :
//* Describe :
//*********************************************************************************************************
//*
void Target_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    //--------------------------------------------------------------------
    //下面是给各模块开启时钟    
    //启动GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE,\
                           ENABLE);
    //启动AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
	//启动TIM2时钟、启动TIM3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_SPI2,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    //--------------------------------------------------------------------
    //初始化串口的相应IO管脚等
    UART_Configuration();
    
    Init_Ki();
    Init_Relay();
    InitKey();
    
    //初始化各中断
    //--------------------------------------------------------------------
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);


    //设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


    //串口1接收中断打开    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	//串口2接收中断打开    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	/*Enable the USART3 Interrupt*/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /*Enable the USART4 Interrupt*/
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /*Enable the USART5 Interrupt*/
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	/* Enable the TIM2 Interrupt*/ 
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	/* Enable the TIM3 Interrupt*/ 
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//*
//********************************************************************************************************
//* 函 数 名 : OS_CPU_SysTickInit()
//**------------------------------------------------------------------------------------------------------
//* 描    述 : 初始化系统Tick
//**------------------------------------------------------------------------------------------------------
//* 输入参数 : None.
//**------------------------------------------------------------------------------------------------------
//* 输出参数 : None.
//**------------------------------------------------------------------------------------------------------
//* 返   回  : None.
//********************************************************************************************************
//*
void  OS_CPU_SysTickInit (void)
{
    RCC_ClocksTypeDef  rcc_clocks;

    // 获取系统频率
    RCC_GetClocksFreq(&rcc_clocks);

    // 配置HCLK作为SysTick时钟
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    
    // 配置自动加载数值
    SysTick_SetReload((rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC) - 1);
    
    // 使能SysTick
    SysTick_CounterCmd(SysTick_Counter_Enable);
    
    // 使能SysTick中断
    SysTick_ITConfig(ENABLE);
}

//
//*********************************************************************************************************
//**                            End Of File
//*********************************************************************************************************
//
