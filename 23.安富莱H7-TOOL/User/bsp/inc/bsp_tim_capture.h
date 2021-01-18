/*
*********************************************************************************************************
*
*    模块名称 : TIM捕获模块
*    文件名称 : bsp_tim_capture.h
*
*********************************************************************************************************
*/

#ifndef _BSP_TIM_CAPTURE_H
#define _BSP_TIM_CAPTURE_H

typedef struct
{
    uint32_t C0;
    uint32_t C1;
    uint32_t C2;
    uint32_t Count;
    float Freq;
     float Duty;
}PUSLE_T;

extern PUSLE_T g_tPulse;

void bsp_GetPulseParam(void);
void bsp_StartDetectPulse(void);
void bsp_StopDetectPulse(void);

void bsp_StartLogicCH4(void);
void bsp_StopLogicCH4(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
