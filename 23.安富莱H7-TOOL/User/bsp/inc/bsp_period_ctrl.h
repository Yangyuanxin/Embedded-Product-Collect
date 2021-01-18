/*
*********************************************************************************************************
*
*    模块名称 : 周期性控制某个IO. 比如闪烁指示灯
*    文件名称 : bsp_period_ctrl.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_PERIOD_CTRL_H
#define __BSP_PERIOD_CTRL_H

typedef struct
{
    /* 下面是一个函数指针 */
    void (*OnFunc)(void);    /* 回调函数，比如点亮LED的函数 */
    void (*OffFunc)(void); /* 回调函数，比如熄灭LED的函数 */

    uint8_t ucEnalbe;
    uint8_t ucState;
    uint16_t usOnTime;
    uint16_t usOffTime;
    uint16_t usCycle;
    uint16_t usCount;
    uint16_t usCycleCount;
} PERIOD_CTRL_T;

/* 供外部调用的函数声明 */
void PERIOD_InitVar(void);
void PERIOD_Scan(void);
void PERIOD_Start(PERIOD_CTRL_T *_ptPer, uint16_t _usOnTime, uint16_t _usOffTime, uint16_t _usCycle);
void PERIOD_Stop(PERIOD_CTRL_T *_ptPer);

extern PERIOD_CTRL_T g_tWiFiLed; /* WiFi 指示灯 */
extern PERIOD_CTRL_T g_tRunLed;    /* 运行 指示灯 */

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
