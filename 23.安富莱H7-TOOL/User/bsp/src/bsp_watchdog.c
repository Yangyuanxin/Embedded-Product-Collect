/*
*********************************************************************************************************
*                                      
*    模块名称 : STM32内部看门狗
*    文件名称 : bsp_watchdog.c
*    版    本 : V1.0
*    说    明 : STM32内部独立看门狗IWDG和窗口看门狗WWDG驱动程序
*    修改记录 :
*        版本号   日期        作者     说明
*        V1.0    2015-10-06  armfly   正式发布
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

static void bsp_InitIwdg(uint32_t _ulIWDGTime);

/*
*********************************************************************************************************
*    函 数 名: bsp_StartDog
*    功能说明: 启动看门狗， 独立看门狗设置为 1000ms, 窗口看门狗不使能。
*    形    参: 无
*    返 回 值: 无                
*********************************************************************************************************
*/
void bsp_StartDog(void)
{
    /* 启动独立看门狗, 超时1000ms */
    bsp_InitIwdg(1000);

    /*
        窗口看门狗初始化
        窗口刷新时间段: ~780 * (0x7f-0x50) = 36.6ms < refresh window < ~780 * 64 = 49.9ms
        
        首先设置计数器值是 0x7f，窗口值是 0x50，我们不能够在计数器递减到 0x50 之前做刷新        
    */
    //bsp_InitWwdg(0x7f, 0x50, WWDG_Prescaler_8);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_FeedDog
*    功能说明: 喂狗函，针对独立看门狗
*    形    参: 无
*    返 回 值: 无                
*********************************************************************************************************
*/
void bsp_FeedDog(void)
{
    IWDG_ReloadCounter();
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitIwdg
*    功能说明: 独立看门狗时间配置函数
*    形    参: IWDGTime: 0 ---- 0x0FFF
*              独立看门狗时间设置,单位为ms,IWDGTime = 1000 大约就是一秒的时间
*              LSI = 34000左右
*    返 回 值: 无                
*********************************************************************************************************
*/
static void bsp_InitIwdg(uint32_t _ulIWDGTime)
{

    /* 检测系统是否从独立看门狗复位中恢复 */
    if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
    {
        /* 清除复位标志 */
        RCC_ClearFlag();
    }
    else
    {
        /* 标志没有设置 */
    }

    /* 使能LSI */
    RCC_LSICmd(ENABLE);

    /* 等待直到LSI就绪 */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }

    /* 写入0x5555表示允许访问IWDG_PR 和IWDG_RLR寄存器 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /*  LSI/32 分频， 之后周期差不多1ms */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /*
        特别注意，由于这里_ulIWDGTime的最小单位是ms, 所以这里重装计数的计数时 需要除以1000
         Counter Reload Value = (_ulIWDGTime / 1000) /(1 / IWDG counter clock period)
                          = (_ulIWDGTime / 1000) / (32/LSI)
                          = (_ulIWDGTime / 1000) / (32/LsiFreq)
                          = LsiFreq * _ulIWDGTime / 32000
         实际测试LsiFreq = 34000，所以这里取1的时候 大概就是1ms 
    */
    IWDG_SetReload(_ulIWDGTime);

    /* 重载IWDG计数 */
    IWDG_ReloadCounter();

    /* 使能 IWDG (LSI oscillator 由硬件使能) */
    IWDG_Enable();
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitWwdg
*    功能说明: 窗口看门狗配置 
*    形    参：
*             _ucTreg       : T[6:0],计数器值     范围0x40 到 0x7F                                               
*             _ucWreg       : W[6:0],窗口值     必须小于 0x80
*            WWDG_Prescaler : 窗口看门狗分频    PCLK1 = 42MHz
*                             WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
*                              WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
*                              WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
*                              WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8 
*    返 回 值: 无                
*********************************************************************************************************
*/
void bsp_InitWwdg(uint8_t _ucTreg, uint8_t _ucWreg, uint32_t WWDG_Prescaler)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 检测系统是否从窗口看门狗复位中恢复 */
    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
    {
        /* 清除复位标志 */
        RCC_ClearFlag();
    }
    else
    {
        /* WWDGRST 标志没有设置 */
    }

    /* 使能WWDG时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    /* 
       窗口看门狗分频设置：
       比如选择WWDG_Prescaler_8
       (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)  
    */
    WWDG_SetPrescaler(WWDG_Prescaler);

    /* 
     设置窗口值是_ucWreg，用户必须在小于_ucWreg且大于0x40时刷新计数
     器，要不会造成系统复位。
    */
    WWDG_SetWindowValue(_ucWreg);

    /* 
     使能WWDG，设置计数器
     比如设置_ucTreg=127 8分频时，那么溢出时间就是= ~780 us * 64 = 49.92 ms 
     窗口看门狗的刷新时间段是: ~780 * (127-80) = 36.6ms < 刷新窗口看门狗 < ~780 * 64 = 49.9ms
    */
    WWDG_Enable(_ucTreg);

    /* 清除EWI中断标志 */
    WWDG_ClearFlag();

    /* 使能EW中断 */
    WWDG_EnableIT();

    /* 设置 WWDG 的NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
*********************************************************************************************************
*    函 数 名: WWDG_IRQHandler
*    功能说明: 窗口看门狗调用中断服务程序。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void WWDG_IRQHandler(void)
{
    /*特别注意，下面两句的执行顺序不能反了，反了就出问题了*/
    /* 清除EWI标志 */
    if (WWDG_GetFlagStatus != RESET)
    {
        /* 清除复位标志 */
        WWDG_ClearFlag();

        /* 更新窗口看门狗计数器 */
        WWDG_SetCounter(127);
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
