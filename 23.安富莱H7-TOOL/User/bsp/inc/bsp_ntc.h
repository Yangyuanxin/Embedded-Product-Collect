/*
*********************************************************************************************************
*
*    模块名称 : NTC驱动模块
*    文件名称 : bsp_ntc.c
*    版    本 : V1.0
*    说    明 : 
*
*    修改记录 :
*        版本号    日期           作者          说明
*       V1.0    2017-09-20        armfly          首发
*
*    Copyright (C), 2016-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_NTC_H_
#define _BSP_NTC_H_

float CalculNtcTemperFloat(float _adc);

int16_t CalculNtcTemperInt(float _adc);
float CalculNtcRes(float _adc);
float CalculRefRes(float _adc, float _res);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
