#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h" //记得添加此头文件，因为config.c用到GPIO相关函数等
#include "sys.h"

extern u16 NTC_Average;
extern u16 T12_Average;

void Adc_Init(void);//ADC1初始化
u16 Get_Adc(u8 ch); //获取一次ADC的值
u16 Get_Adc_Average(u8 ch,u8 times);//ADC采样值进行均值滤波
float map(float value,float fromLow,float fromHigh,float toLow,float toHigh);//映射函数
u16 get_NTC_temp(void);
void get_T12_ADC(void);
void get_Vin(void);
u16 get_T12_temp(void);
u16 sleepCheck(void);
#endif
