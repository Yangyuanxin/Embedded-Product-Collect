/*
*********************************************************************************************************
*
*    模块名称 : 蜂鸣器驱动模块
*    文件名称 : bsp_beep.c
*    版    本 : V1.1
*    说    明 : 驱动蜂鸣器.
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2014-10-20 armfly  正式发布
*        V1.1    2015-10-06 armfly  增加静音函数。用于临时屏蔽蜂鸣器发声。
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define BEEP_HAVE_POWER     /* 定义此行表示有源蜂鸣器，直接通过GPIO驱动, 无需PWM */

#ifdef BEEP_HAVE_POWER      /* 有源蜂鸣器 */

/* PG1 */
#define BEEP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOG_CLK_ENABLE()
#define GPIO_PORT_BEEP              GPIOG
#define GPIO_PIN_BEEP               GPIO_PIN_1

#define BEEP_ENABLE()               BSP_SET_GPIO_1(GPIO_PORT_BEEP, GPIO_PIN_BEEP)   /* 使能蜂鸣器鸣叫 */
#define BEEP_DISABLE()              BSP_SET_GPIO_0(GPIO_PORT_BEEP, GPIO_PIN_BEEP)   /* 禁止蜂鸣器鸣叫 */
#else                                                                                                                 /* 无源蜂鸣器 */
/* PA0 ---> TIM5_CH1 */

/* 1500表示频率1.5KHz，5000表示50.00%的占空比 */
#define BEEP_ENABLE() bsp_SetTIMOutPWM(GPIOA, GPIO_PIN_0, TIM5, 1, 1500, 5000);

/* 禁止蜂鸣器鸣叫 */
#define BEEP_DISABLE() bsp_SetTIMOutPWM(GPIOA, GPIO_PIN_0, TIM5, 1, 1500, 0);
#endif

BEEP_T g_tBeep; /* 定义蜂鸣器全局结构体变量 */

/*
*********************************************************************************************************
*    函 数 名: BEEP_InitHard
*    功能说明: 初始化蜂鸣器硬件
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void BEEP_InitHard(void)
{
#ifdef BEEP_HAVE_POWER /* 有源蜂鸣器 */
    GPIO_InitTypeDef gpio_init;

    /* 第1步：打开GPIO时钟 */
    BEEP_GPIO_CLK_ENABLE();

    BEEP_DISABLE();

    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        /* 设置开漏输出 */
    gpio_init.Pull = GPIO_NOPULL;                        /* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度等级 */

    gpio_init.Pin = GPIO_PIN_BEEP;
    HAL_GPIO_Init(GPIO_PORT_BEEP, &gpio_init);
#endif

    g_tBeep.ucMute = 0; /* 关闭静音 */
}

/*
*********************************************************************************************************
*    函 数 名: BEEP_Start
*    功能说明: 启动蜂鸣音。
*    形    参: _usBeepTime : 蜂鸣时间，单位10ms; 0 表示不鸣叫
*              _usStopTime : 停止时间，单位10ms; 0 表示持续鸣叫
*              _usCycle : 鸣叫次数， 0 表示持续鸣叫
*    返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Start(uint16_t _usBeepTime, uint16_t _usStopTime, uint16_t _usCycle)
{
    if (_usBeepTime == 0 || g_tBeep.ucMute == 1)
    {
        return;
    }

    g_tBeep.usBeepTime = _usBeepTime;
    g_tBeep.usStopTime = _usStopTime;
    g_tBeep.usCycle = _usCycle;
    g_tBeep.usCount = 0;
    g_tBeep.usCycleCount = 0;
    g_tBeep.ucState = 0;
    g_tBeep.ucEnalbe = 1; /* 设置完全局参数后再使能发声标志 */

    BEEP_ENABLE(); /* 开始发声 */
}

/*
*********************************************************************************************************
*    函 数 名: BEEP_Stop
*    功能说明: 停止蜂鸣音。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Stop(void)
{
    g_tBeep.ucEnalbe = 0;

    //    if ((g_tBeep.usStopTime == 0) || (g_tBeep.usCycle == 0))
    //    {
    //        BEEP_DISABLE();    /* 必须在清控制标志后再停止发声，避免停止后在中断中又开启 */
    //    }
    g_tBeep.usStopTime = 0;
    g_tBeep.usCycle = 0;
    BEEP_DISABLE();
}

/*
*********************************************************************************************************
*    函 数 名: BEEP_Pause
*    功能说明: 由于TIM冲突等原因，临时屏蔽蜂鸣音。通过 BEEP_Resume 恢复
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Pause(void)
{
    BEEP_Stop();

    g_tBeep.ucMute = 1; /* 静音 */
}

/*
*********************************************************************************************************
*    函 数 名: BEEP_Resume
*    功能说明: 恢复蜂鸣器正常功能
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Resume(void)
{
    BEEP_Stop();

    g_tBeep.ucMute = 0; /* 静音 */
}

/*
*********************************************************************************************************
*    函 数 名: BEEP_KeyTone
*    功能说明: 发送按键音
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void BEEP_KeyTone(void)
{
    BEEP_Start(5, 1, 1); /* 鸣叫50ms，停10ms， 1次 */
}

/*
*********************************************************************************************************
*    函 数 名: BEEP_Pro
*    功能说明: 每隔10ms调用1次该函数，用于控制蜂鸣器发声。该函数在 bsp_timer.c 中被调用。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void BEEP_Pro(void)
{
    if ((g_tBeep.ucEnalbe == 0) || (g_tBeep.usStopTime == 0) || (g_tBeep.ucMute == 1))
    {
        return;
    }

    if (g_tBeep.ucState == 0)
    {
        if (g_tBeep.usStopTime > 0) /* 间断发声 */
        {
            if (++g_tBeep.usCount >= g_tBeep.usBeepTime)
            {
                BEEP_DISABLE(); /* 停止发声 */
                g_tBeep.usCount = 0;
                g_tBeep.ucState = 1;
            }
        }
        else
        {
            ; /* 不做任何处理，连续发声 */
        }
    }
    else if (g_tBeep.ucState == 1)
    {
        if (++g_tBeep.usCount >= g_tBeep.usStopTime)
        {
            /* 连续发声时，直到调用stop停止为止 */
            if (g_tBeep.usCycle > 0)
            {
                if (++g_tBeep.usCycleCount >= g_tBeep.usCycle)
                {
                    /* 循环次数到，停止发声 */
                    g_tBeep.ucEnalbe = 0;
                }

                if (g_tBeep.ucEnalbe == 0)
                {
                    g_tBeep.usStopTime = 0;
                    return;
                }
            }

            g_tBeep.usCount = 0;
            g_tBeep.ucState = 0;

            BEEP_ENABLE(); /* 开始发声 */
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
