/*
*********************************************************************************************************
*
*   模块名称 : TIM基本定时中断和PWM驱动模块
*   文件名称 : bsp_tim_pwm.c
*   版    本 : V1.6
*   说    明 : 利用STM32H7内部TIM输出PWM信号， 并实现基本的定时中断
*   修改记录 :
*       版本号  日期        作者     说明
*       V1.0    2013-08-16 armfly  正式发布
*       V1.1    2014-06-15 armfly  完善 bsp_SetTIMOutPWM，当占空比=0和100%时，关闭定时器，GPIO配置为输出
*       V1.2    2015-05-08 armfly  解决TIM8不能输出PWM的问题。
*       V1.3    2015-07-23 armfly  初始化定时器，必须设置 TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;       
*                                  TIM1 和 TIM8 必须设置。否则蜂鸣器的控制不正常。
*       V1.4    2015-07-30 armfly  增加反相引脚输出PWM函数 bsp_SetTIMOutPWM_N();
*       V1.5    2016-02-01 armfly  去掉 TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
*       V1.6    2016-02-27 armfly  解决TIM14无法中断的BUG, TIM8_TRG_COM_TIM14_IRQn
*
*   Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
    注意，STM32H7有TIM1 – TIM17（没有TIM9，TIM10和TIM11）共计14个定时器。
    可以输出到GPIO的TIM通道:
    TIM1_CH1,  PA8   PE9   PK1
    TIM1_CH2,  PA9   PE11
    TIM1_CH3,  PA10  PE13  PJ9
    TIM1_CH4,  PA11  PE14  PJ11

    TIM2_CH1,  PA0   PA5   PA15
    TIM2_CH2,  PA1   PB3  
    TIM2_CH3,  PA2
    TIM2_CH4,  PA3   PB11

    TIM3_CH1,  PA6   PC6  PB4
    TIM3_CH2,  PA7   PC7  PB5   
    TIM3_CH3,  PB0   PC8  
    TIM3_CH4,  PB1   PC9  

    TIM4_CH1,  PB6   PD12
    TIM4_CH2,  PB7   PD13
    TIM4_CH3,  PB8   PD14
    TIM4_CH4,  PB9   PD15

    TIM5_CH1,  PA0   PH10
    TIM5_CH2,  PA1   PH11
    TIM5_CH3,  PA2   PH12
    TIM5_CH4,  PA3   PI0

    TIM8_CH1,  PC6   PI5  PJ8
    TIM8_CH2,  PC7   PI6  PJ10
    TIM8_CH3,  PC8   PI7  PK0
    TIM8_CH4,  PC9   

    TIM12_CH1,  PB14  PH6
    TIM12_CH2,  PB15  PH9

    TIM13_CH1,  PF8

    TIM14_CH1,  PF9

    TIM15_CH1,  PE5 
    TIM15_CH2,  PE6

    TIM16_CH1,  PB8   PF6
    TIM16_CH2,  PF7

    TIM17_CH1,  PB9
*/

/*
*********************************************************************************************************
*    函 数 名: bsp_RCC_GPIO_Enable
*    功能说明: 使能GPIO时钟
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_RCC_GPIO_Enable(GPIO_TypeDef* GPIOx)
{
    if (GPIOx == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (GPIOx == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (GPIOx == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (GPIOx == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (GPIOx == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (GPIOx == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
    else if (GPIOx == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
    else if (GPIOx == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
    else if (GPIOx == GPIOI) __HAL_RCC_GPIOI_CLK_ENABLE();
    else if (GPIOx == GPIOJ) __HAL_RCC_GPIOJ_CLK_ENABLE();
    else if (GPIOx == GPIOK) __HAL_RCC_GPIOK_CLK_ENABLE();
}

/*
*********************************************************************************************************
*    函 数 名: bsp_RCC_TIM_Enable
*    功能说明: 使能TIM RCC 时钟
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_RCC_TIM_Enable(TIM_TypeDef* TIMx)
{
    if (TIMx == TIM1) __HAL_RCC_TIM1_CLK_ENABLE();
    else if (TIMx == TIM2) __HAL_RCC_TIM2_CLK_ENABLE();
    else if (TIMx == TIM3) __HAL_RCC_TIM3_CLK_ENABLE();
    else if (TIMx == TIM4) __HAL_RCC_TIM4_CLK_ENABLE();
    else if (TIMx == TIM5) __HAL_RCC_TIM5_CLK_ENABLE();
    else if (TIMx == TIM6) __HAL_RCC_TIM6_CLK_ENABLE();
    else if (TIMx == TIM7) __HAL_RCC_TIM7_CLK_ENABLE();
    else if (TIMx == TIM8) __HAL_RCC_TIM8_CLK_ENABLE();
//  else if (TIMx == TIM9) __HAL_RCC_TIM9_CLK_ENABLE();
//  else if (TIMx == TIM10) __HAL_RCC_TIM10_CLK_ENABLE();
//  else if (TIMx == TIM11) __HAL_RCC_TIM11_CLK_ENABLE();
    else if (TIMx == TIM12) __HAL_RCC_TIM12_CLK_ENABLE();
    else if (TIMx == TIM13) __HAL_RCC_TIM13_CLK_ENABLE();
    else if (TIMx == TIM14) __HAL_RCC_TIM14_CLK_ENABLE();
    else if (TIMx == TIM15) __HAL_RCC_TIM15_CLK_ENABLE();
    else if (TIMx == TIM16) __HAL_RCC_TIM16_CLK_ENABLE();
    else if (TIMx == TIM17) __HAL_RCC_TIM17_CLK_ENABLE();   
    else
    {
        Error_Handler(__FILE__, __LINE__);
    }   
}

/*
*********************************************************************************************************
*   函 数 名: bsp_RCC_TIM_Disable
*   功能说明: 关闭TIM RCC 时钟
*   形    参: TIMx TIM1 - TIM17
*   返 回 值: TIM外设时钟名
*********************************************************************************************************
*/
void bsp_RCC_TIM_Disable(TIM_TypeDef* TIMx)
{
    /*
        APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
        APB2 定时器有 TIM1, TIM8 , TIM15, TIM16，TIM17
    */
    if (TIMx == TIM1) __HAL_RCC_TIM3_CLK_DISABLE();
    else if (TIMx == TIM2) __HAL_RCC_TIM2_CLK_DISABLE();
    else if (TIMx == TIM3) __HAL_RCC_TIM3_CLK_DISABLE();
    else if (TIMx == TIM4) __HAL_RCC_TIM4_CLK_DISABLE();
    else if (TIMx == TIM5) __HAL_RCC_TIM5_CLK_DISABLE();
    else if (TIMx == TIM6) __HAL_RCC_TIM6_CLK_DISABLE();
    else if (TIMx == TIM7) __HAL_RCC_TIM7_CLK_DISABLE();
    else if (TIMx == TIM8) __HAL_RCC_TIM8_CLK_DISABLE();
//  else if (TIMx == TIM9) __HAL_RCC_TIM9_CLK_DISABLE();
//  else if (TIMx == TIM10) __HAL_RCC_TIM10_CLK_DISABLE();
//  else if (TIMx == TIM11) __HAL_RCC_TIM11_CLK_DISABLE();
    else if (TIMx == TIM12) __HAL_RCC_TIM12_CLK_DISABLE();
    else if (TIMx == TIM13) __HAL_RCC_TIM13_CLK_DISABLE();
    else if (TIMx == TIM14) __HAL_RCC_TIM14_CLK_DISABLE();
    else if (TIMx == TIM15) __HAL_RCC_TIM15_CLK_DISABLE();
    else if (TIMx == TIM16) __HAL_RCC_TIM16_CLK_DISABLE();
    else if (TIMx == TIM17) __HAL_RCC_TIM17_CLK_DISABLE();
    else
    {
        Error_Handler(__FILE__, __LINE__);
    }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_GetAFofTIM
*   功能说明: 根据TIM 得到AF寄存器配置
*   形    参: TIMx TIM1 - TIM17
*   返 回 值: AF寄存器配置
*********************************************************************************************************
*/
uint8_t bsp_GetAFofTIM(TIM_TypeDef* TIMx)
{
    uint8_t ret = 0;

    if (TIMx == TIM1) ret = GPIO_AF1_TIM1;
    else if (TIMx == TIM2) ret = GPIO_AF1_TIM2;
    else if (TIMx == TIM3) ret = GPIO_AF2_TIM3;
    else if (TIMx == TIM4) ret = GPIO_AF2_TIM4;
    else if (TIMx == TIM5) ret = GPIO_AF2_TIM5;
    else if (TIMx == TIM8) ret = GPIO_AF3_TIM8;
    else if (TIMx == TIM12) ret = GPIO_AF2_TIM12;
    else if (TIMx == TIM13) ret = GPIO_AF9_TIM13;
    else if (TIMx == TIM14) ret = GPIO_AF9_TIM14;
    else if (TIMx == TIM15) ret = GPIO_AF4_TIM15;
    else if (TIMx == TIM16) ret = GPIO_AF1_TIM16;
    else if (TIMx == TIM17) ret = GPIO_AF1_TIM17;
    else
    {
        Error_Handler(__FILE__, __LINE__);
    }
    
    return ret;
}

/*
*********************************************************************************************************
*   函 数 名: bsp_ConfigTimGpio
*   功能说明: 配置GPIO和TIM时钟， GPIO连接到TIM输出通道
*   形    参: GPIOx : GPIOA - GPIOK
*             GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*             TIMx : TIM1 - TIM17
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_ConfigTimGpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinX, TIM_TypeDef* TIMx)
{
    GPIO_InitTypeDef   GPIO_InitStruct;

    /* 使能GPIO时钟 */
    bsp_RCC_GPIO_Enable(GPIOx);

    /* 使能TIM时钟 */
    bsp_RCC_TIM_Enable(TIMx);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = bsp_GetAFofTIM(TIMx);
    GPIO_InitStruct.Pin = GPIO_PinX;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*
*********************************************************************************************************
*   函 数 名: bsp_ConfigGpioOut
*   功能说明: 配置GPIO为推挽输出。主要用于PWM输出，占空比为0和100的情况。
*   形    参: GPIOx : GPIOA - GPIOK
*             GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_ConfigGpioOut(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinX)
{
    GPIO_InitTypeDef   GPIO_InitStruct;

    bsp_RCC_GPIO_Enable(GPIOx);     /* 使能GPIO时钟 */

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pin = GPIO_PinX;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*
*********************************************************************************************************
*   函 数 名: bsp_SetTIMOutPWM
*   功能说明: 设置引脚输出的PWM信号的频率和占空比.  当频率为0，并且占空为0时，关闭定时器，GPIO输出0；
*             当频率为0，占空比为100%时，GPIO输出1.
*   形    参: GPIOx : GPIOA - GPIOK
*             GPIO_Pin : GPIO_PIN_0 - GPIO__PIN_15
*             TIMx : TIM1 - TIM17
*             _ucChannel：使用的定时器通道，范围1 - 4
*             _ulFreq : PWM信号频率，单位Hz (实际测试，可以输出100MHz），0 表示禁止输出
*             _ulDutyCycle : PWM信号占空比，单位: 万分之一。如5000，表示50.00%的占空比
*   返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetTIMOutPWM(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
     uint32_t _ulFreq, uint32_t _ulDutyCycle)
{
    TIM_HandleTypeDef  TimHandle = {0};
    TIM_OC_InitTypeDef sConfig = {0};   
    uint16_t usPeriod;
    uint16_t usPrescaler;
    uint32_t pulse;
    uint32_t uiTIMxCLK;
    const uint16_t TimChannel[6+1] = {0, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4, TIM_CHANNEL_5, TIM_CHANNEL_6};

    if (_ucChannel > 6)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    
    if (_ulDutyCycle == 0)
    {       
        //bsp_RCC_TIM_Disable(TIMx);        /* 关闭TIM时钟, 可能影响其他通道 */     
        bsp_ConfigGpioOut(GPIOx, GPIO_Pin); /* 配置GPIO为推挽输出 */           
        BSP_SET_GPIO_0(GPIOx, GPIO_Pin);        /* PWM = 0 */       
        return;
    }
    else if (_ulDutyCycle == 10000)
    {
        //bsp_RCC_TIM_Disable(TIMx);        /* 关闭TIM时钟, 可能影响其他通道 */
        bsp_ConfigGpioOut(GPIOx, GPIO_Pin); /* 配置GPIO为推挽输出 */       
        BSP_SET_GPIO_1(GPIOx, GPIO_Pin);        /* PWM = 1*/    
        return;
    }
    
    /* 下面是PWM输出 */
    
    bsp_ConfigTimGpio(GPIOx, GPIO_Pin, TIMx);   /* 使能GPIO和TIM时钟，并连接TIM通道到GPIO */
    
    /*-----------------------------------------------------------------------
        bsp.c 文件中 void SystemClock_Config(void) 函数对时钟的配置如下: 

        System Clock source       = PLL (HSE)
        SYSCLK(Hz)                = 400000000 (CPU Clock)
        HCLK(Hz)                  = 200000000 (AXI and AHBs Clock)
        AHB Prescaler             = 2
        D1 APB3 Prescaler         = 2 (APB3 Clock  100MHz)
        D2 APB1 Prescaler         = 2 (APB1 Clock  100MHz)
        D2 APB2 Prescaler         = 2 (APB2 Clock  100MHz)
        D3 APB4 Prescaler         = 2 (APB4 Clock  100MHz)

        因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = APB1 x 2 = 200MHz;
        因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = APB2 x 2 = 200MHz;
        APB4上面的TIMxCLK没有分频，所以就是100MHz;

        APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14，LPTIM1
        APB2 定时器有 TIM1, TIM8 , TIM15, TIM16，TIM17

        APB4 定时器有 LPTIM2，LPTIM3，LPTIM4，LPTIM5

    ----------------------------------------------------------------------- */
    if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM15) || (TIMx == TIM16) || (TIMx == TIM17))
    {
        /* APB2 定时器时钟 = 200M */
        uiTIMxCLK = SystemCoreClock / 2;
    }
    else    
    {
        /* APB1 定时器 = 200M */
        uiTIMxCLK = SystemCoreClock / 2;
    }

    if (_ulFreq < 100)
    {
        usPrescaler = 10000 - 1;                    /* 分频比 = 10000 */
        usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;     /* 自动重装的值 */
    }
    else if (_ulFreq < 3000)
    {
        usPrescaler = 100 - 1;                  /* 分频比 = 100 */
        usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;       /* 自动重装的值 */
    }
    else    /* 大于4K的频率，无需分频 */
    {
        usPrescaler = 0;                    /* 分频比 = 1 */
        usPeriod = uiTIMxCLK / _ulFreq - 1; /* 自动重装的值 */
    }
    pulse = (_ulDutyCycle * usPeriod) / 10000;

    
    HAL_TIM_PWM_DeInit(&TimHandle);
    
    /*  PWM频率 = TIMxCLK / usPrescaler + 1）/usPeriod + 1）*/
    TimHandle.Instance = TIMx;
    TimHandle.Init.Prescaler         = usPrescaler;
    TimHandle.Init.Period            = usPeriod;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = 0;
    if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 配置定时器PWM输出通道 */
    sConfig.OCMode       = TIM_OCMODE_PWM1;
    sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

    /* 占空比 */
    sConfig.Pulse = pulse;
    if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TimChannel[_ucChannel]) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    
    /* 启动PWM输出 */
    if (HAL_TIM_PWM_Start(&TimHandle, TimChannel[_ucChannel]) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
}

/*
*********************************************************************************************************
*   函 数 名: bsp_SetTIMforInt
*   功能说明: 配置TIM和NVIC，用于简单的定时中断，开启定时中断。另外注意中断服务程序需要由用户应用程序实现。
*   形    参: TIMx : 定时器
*             _ulFreq : 定时频率 （Hz）。 0 表示关闭。
*             _PreemptionPriority : 抢占优先级
*             _SubPriority : 子优先级
*   返 回 值: 无
*********************************************************************************************************
*/
/*  
TIM定时中断服务程序范例，必须清中断标志
void TIM6_DAC_IRQHandler(void)
{
    if((TIM6->SR & TIM_FLAG_UPDATE) != RESET)
    {
        TIM6->SR = ~ TIM_FLAG_UPDATE;
        //添加用户代码
    }
}
*/
void bsp_SetTIMforInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
    TIM_HandleTypeDef   TimHandle = {0};
    uint16_t usPeriod;
    uint16_t usPrescaler;
    uint32_t uiTIMxCLK;
    
    if (_ulFreq == 0)
    {
        bsp_RCC_TIM_Disable(TIMx);
        
        
        __HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);

        /* 配置TIM定时更新中断 (Update) */
        {
            uint8_t irq = 0;    /* 中断号, 定义在 stm32h7xx.h */

            if (TIMx == TIM1) irq = TIM1_UP_IRQn;
            else if (TIMx == TIM2) irq = TIM2_IRQn;
            else if (TIMx == TIM3) irq = TIM3_IRQn;
            else if (TIMx == TIM4) irq = TIM4_IRQn;
            else if (TIMx == TIM5) irq = TIM5_IRQn;
            else if (TIMx == TIM6) irq = TIM6_DAC_IRQn;
            else if (TIMx == TIM7) irq = TIM7_IRQn;
            else if (TIMx == TIM8) irq = TIM8_UP_TIM13_IRQn;
            else if (TIMx == TIM12) irq = TIM8_BRK_TIM12_IRQn;
            else if (TIMx == TIM13) irq = TIM8_UP_TIM13_IRQn;
            else if (TIMx == TIM14) irq = TIM8_TRG_COM_TIM14_IRQn;
            else if (TIMx == TIM15) irq = TIM15_IRQn;
            else if (TIMx == TIM16) irq = TIM16_IRQn;
            else if (TIMx == TIM17) irq = TIM17_IRQn;
            else
            {
                Error_Handler(__FILE__, __LINE__);
            }   
            HAL_NVIC_DisableIRQ((IRQn_Type)irq);     
        }        
        return;
    }
    
    /* 使能TIM时钟 */
    bsp_RCC_TIM_Enable(TIMx);
    
    /*-----------------------------------------------------------------------
        bsp.c 文件中 void SystemClock_Config(void) 函数对时钟的配置如下: 

        System Clock source       = PLL (HSE)
        SYSCLK(Hz)                = 400000000 (CPU Clock)
        HCLK(Hz)                  = 200000000 (AXI and AHBs Clock)
        AHB Prescaler             = 2
        D1 APB3 Prescaler         = 2 (APB3 Clock  100MHz)
        D2 APB1 Prescaler         = 2 (APB1 Clock  100MHz)
        D2 APB2 Prescaler         = 2 (APB2 Clock  100MHz)
        D3 APB4 Prescaler         = 2 (APB4 Clock  100MHz)

        因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = APB1 x 2 = 200MHz;
        因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = APB2 x 2 = 200MHz;
        APB4上面的TIMxCLK没有分频，所以就是100MHz;

        APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14，LPTIM1
        APB2 定时器有 TIM1, TIM8 , TIM15, TIM16，TIM17

        APB4 定时器有 LPTIM2，LPTIM3，LPTIM4，LPTIM5
    ----------------------------------------------------------------------- */
    if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM15) || (TIMx == TIM16) || (TIMx == TIM17))
    {
        /* APB2 定时器时钟 = 200M */
        uiTIMxCLK = SystemCoreClock / 2;
    }
    else    
    {
        /* APB1 定时器 = 200M */
        uiTIMxCLK = SystemCoreClock / 2;
    }

    if (_ulFreq < 100)
    {
        usPrescaler = 10000 - 1;                    /* 分频比 = 10000 */
        usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;     /* 自动重装的值 */
    }
    else if (_ulFreq < 3000)
    {
        usPrescaler = 100 - 1;                  /* 分频比 = 100 */
        usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;       /* 自动重装的值 */
    }
    else    /* 大于4K的频率，无需分频 */
    {
        usPrescaler = 0;                    /* 分频比 = 1 */
        usPeriod = uiTIMxCLK / _ulFreq - 1; /* 自动重装的值 */
    }

    /* 
       定时器中断更新周期 = TIMxCLK / usPrescaler + 1）/usPeriod + 1）
    */
    TimHandle.Instance = TIMx;
    TimHandle.Init.Prescaler         = usPrescaler;
    TimHandle.Init.Period            = usPeriod;    
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 使能定时器中断  */
    __HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
    

    /* 配置TIM定时更新中断 (Update) */
    {
        uint8_t irq = 0;    /* 中断号, 定义在 stm32h7xx.h */

        if (TIMx == TIM1) irq = TIM1_UP_IRQn;
        else if (TIMx == TIM2) irq = TIM2_IRQn;
        else if (TIMx == TIM3) irq = TIM3_IRQn;
        else if (TIMx == TIM4) irq = TIM4_IRQn;
        else if (TIMx == TIM5) irq = TIM5_IRQn;
        else if (TIMx == TIM6) irq = TIM6_DAC_IRQn;
        else if (TIMx == TIM7) irq = TIM7_IRQn;
        else if (TIMx == TIM8) irq = TIM8_UP_TIM13_IRQn;
        else if (TIMx == TIM12) irq = TIM8_BRK_TIM12_IRQn;
        else if (TIMx == TIM13) irq = TIM8_UP_TIM13_IRQn;
        else if (TIMx == TIM14) irq = TIM8_TRG_COM_TIM14_IRQn;
        else if (TIMx == TIM15) irq = TIM15_IRQn;
        else if (TIMx == TIM16) irq = TIM16_IRQn;
        else if (TIMx == TIM17) irq = TIM17_IRQn;
        else
        {
            Error_Handler(__FILE__, __LINE__);
        }   
        HAL_NVIC_SetPriority((IRQn_Type)irq, _PreemptionPriority, _SubPriority);
        HAL_NVIC_EnableIRQ((IRQn_Type)irq);     
    }
    
    HAL_TIM_Base_Start(&TimHandle);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
