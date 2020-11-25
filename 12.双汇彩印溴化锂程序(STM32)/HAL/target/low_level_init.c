//*
//*********************************************************************************************************
//*
//*                                                 LOGO
//*
//*
//* 文 件 名 : low_level_init.c
//*
//* 文件描述 : 本函数在系统复位后,数据段初始化之前被底层调用
//*
//*            主要用途是初始化一些硬件外设(如FSMC),系统时钟,看门狗,中断等
//*
//*            注: 由于段没有被初始化,所以
//*                使用全局变量,静态变量就会出错, 修改他们也会被初始化代码重新覆盖
//*                使用0初始化的变量也会出现错误, 因为还没有运行初始化代码
//*                此函数仅仅用于底层初始化
//*                
//*                系统复位后使用MSP堆栈, 调用函数注意MSP尺寸
//*
//* 作    者 : SuperMario 
//* 版    本 : 2008.11.1a
//* 编 译 器 : IAR EWARM 5.20
//*********************************************************************************************************
//*
#ifdef __cplusplus
extern "C" {
#endif
  
#include "stm32f10x_lib.h"
  
#pragma language=extended

__interwork int __low_level_init(void);

__interwork int __low_level_init(void)
{
    // 关闭中断
    asm("CPSID  I");
    
    //=====================================================================================================
    // 复位目标系统的NVIC,恢复为上电默认值
    //=====================================================================================================
    NVIC_DeInit();
    NVIC_SCBDeInit();
    // 如果在RAM中调试, 修改中断向量表首地址 NVIC_VectTab_RAM = 0x20000000
    // NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
    // 否则修改中断向量表在Flash中, NVIC_VectTab_FLASH = 0x08000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    //=====================================================================================================
    // 系统时钟初始化
    //=====================================================================================================
    // RCC system reset(for debug purpose)
    // 将RCC寄存器恢复为上电默认值
    RCC_DeInit();
    // Enable HSE
    // 打开high-speed external clock signal
    RCC_HSEConfig(RCC_HSE_ON);
    // 等待外部外部晶振ready
    RCC_WaitForHSEStartUp();
    // Enable Prefetch Buffer
    // 启用预取缓冲区
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    // FLASH_Latency_0 = 000: 0等待状态, 当 0 < SYSCLK≤24MHz
    // FLASH_Latency_1 = 001: 1等待状态, 当 24MHz < SYSCLK≤ 48MHz
    // FLASH_Latency_2 = 010: 2等待状态, 当 48MHz < SYSCLK≤ 72MHz
    // SYSCLK = 72MHz
    // Flash 2 wait state
    FLASH_SetLatency(FLASH_Latency_2);
    // HCLK   = SYSCLK
    // HCLK   = 处理器时钟
    // SYSCLK = PLL_CLK or HSE or HSI
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    // PCLK2 = HCLK
    // PCLK2 = 72MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);
    // PCLK1 最大不可超过36MHz
    // PCLK1 = 72MHz / 2 = 36MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);
    // ADCCLK = PCLK2/2
    // ADCCLK = 12MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div2);
    // PLLCLK = 8MHz * 9 = 72 MHz
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    // Enable PLL
    RCC_PLLCmd(ENABLE);
    // 等待PLL Ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    // 连接SYSCLK到PLL_CLK, SYSCLK = PLL_CLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    // 等待 SYSCLK 稳定
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
    
    
    
    /*
    // FSMC 接口初始化
    // Enable FSMC clock, 控制位在RCC_AHBENR寄存器中
    // AHB Peripheral Clock enable register (RCC_AHBENR:0x40021014) Reset value: 0x0000 0014
    // 控制位,描述如下:
    // |--------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |Bit31 - Bit11 |Bit10|Bit9 |Bit8 |Bit7 |Bit6 |Bit5 |Bit4 |Bit3 |Bit2 |Bit1 |Bit0 |
    // |--------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // | Reserved     |SDIO |res. |FSMC |res. |CRCEN|res. |FLITF|res. |SRAM |DMA2 |DMA1 |
    // |--------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |     0        |  0  |  0  |  1  |  0  |  0  |  0  |  1  |  0  |  1  |  0  |  0  |
    // |--------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    *(vu32 *)0x40021014 = 0x00000114;
    
    // A0 - A18 ---- GPIOD, GPIOE, GPIOF and GPIOG
    // D0 - D15 ---- GPIOD, GPIOE
    // CE       ---- PG10 ( NE3 )
    // OE       ---- PD4
    // WE       ---- PD5
    // LB       ---- PE0
    // UB       ---- PE1
    // 使能 GPIOD, GPIOE, GPIOF and GPIOG 时钟
    // APB2 Peripheral reset register (RCC_APB2RSTR:0x40021018) Reset value: 0x00000 0000
    // APB2 高16位Reserved, 低14位有外设控制位,描述如下:
    // |------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |Bit15 |Bit14 |Bit13|Bit12|Bit11|Bit10|Bit9 |Bit8 |Bit7 |Bit6 |Bit5 |Bit4 |Bit3 |Bit2 |Bit1 |Bit0 |
    // |------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |ADC3  |USART1|TIM8 |SPI1 |TIM1 |ADC2 |ADC1 |IOPG |IOPF |IOPE |IOPD |IOPC |IOPB |IOPA |res. |AFIO |
    // |------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |  0   |   0  |  0  |  0  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |
    // |------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    *(vu32 *)0x40021018 = 0x000001E0;
  
    //---------------  SRAM Data lines, NOE and NWE configuration ---------------*
    //----------------  SRAM Address lines configuration -------------------------*
    //----------------  NOE and NWE configuration --------------------------------*  
    //----------------  NE3 configuration ----------------------------------------*
    //----------------  NBL0, NBL1 configuration ---------------------------------*
  
    *(vu32 *)0x40011400 = 0x44BB44BB;
    *(vu32 *)0x40011404 = 0xBBBBBBBB;
  
    *(vu32 *)0x40011800 = 0xB44444BB;
    *(vu32 *)0x40011804 = 0xBBBBBBBB;
   
    *(vu32 *)0x40011C00 = 0x44BBBBBB;
    *(vu32 *)0x40011C04 = 0xBBBB4444;  

    *(vu32 *)0x40012000 = 0x44BBBBBB;
    *(vu32 *)0x40012004 = 0x44444B44;
  
    //----------------  FSMC Configuration ---------------------------------------*  
    //----------------  Enable FSMC Bank1_SRAM Bank ------------------------------*
  
    *(vu32 *)0xA0000010 = 0x00001011;
    *(vu32 *)0xA0000014 = 0x00000200;
    */

    //=====================================================================================================
    // 修改返回值可以选择不初始化或初始化数据段
    // Return: 0 不执行段初始化
    //         1 执行段初始化
    //=====================================================================================================
    return 1;
}

#pragma language=default

#ifdef __cplusplus
}
#endif
