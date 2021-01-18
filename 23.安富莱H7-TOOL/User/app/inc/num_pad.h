/*
*********************************************************************************************************
*
*    模块名称 : main模块
*    文件名称 : main.h
*    版    本 : V1.1
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _MAIN_H_
#define _MAIN_H

typedef enum
{
    NUMPAD_IP = 0,    /* IP地址 */
    NUMPAD_TEL = 1, /* 电话号码 */
    NUMPAD_INT = 2, /* 整数（带范围判断） */
    NUMPAD_STR = 3    /* 任意字母数字 小数点 */
} NUMPAD_MODE_E;

/* 形参类型0： IP地址 */
typedef struct
{
    uint8_t ip_buf[4];
} NUMPAD_IP_T;

/* 形参类型1：支持小数点 */
typedef struct
{
    int32_t Min;        /* 最小值 */
    int32_t Max;        /* 最大值 */
    uint8_t DotNum; /* 小数点位数 */
} NUMPAD_INT_T;

void DispInvlidInput(void);
uint8_t InputNumber(NUMPAD_MODE_E _Mode, char *_Caption, void *_pInParam, void *_pOutParam);
void ClearWinNumPad(uint16_t _usColor);
uint8_t InputInt(char *_Caption, int32_t _min, int32_t _max, int32_t *_value);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
