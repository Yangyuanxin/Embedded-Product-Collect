/*
*********************************************************************************************************
*
*    模块名称 : 输出端口的电平控制
*    文件名称 : bsp_power_tvcc.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_POWER_TVCC_H
#define __BSP_POWER_TVCC_H

/* 常用电压档位 5.0 - 1.8V */
#define TVCC_50 32
#define TVCC_33 48
#define TVCC_30 53
#define TVCC_28 56
#define TVCC_26 61
#define TVCC_25 63
#define TVCC_18 88

/* 供外部调用的函数声明 */
void bsp_InitTVCC(void);
void bsp_SetTVCC(uint16_t _volt);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
