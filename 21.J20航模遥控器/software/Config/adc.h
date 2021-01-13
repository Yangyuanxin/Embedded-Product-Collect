#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h" //记得添加此头文件，因为config.c用到GPIO相关函数等
#include "sys.h"

#define adcNum 9		//采样通道数
#define chNum 8	//遥杆通道数
#define sampleNum 10 //每个通道采样次数

extern volatile u16 chValue[adcNum * sampleNum]; //ADC采样值
extern volatile u16 chResult[chNum];//滤波后的ADC采样值
extern volatile u16 PWMvalue[chNum];//控制PWM占空比
extern volatile float batVolt;//电池电压
extern volatile u8 batVoltSignal;//是否报警

void TIM2_Init(u16 arr, u16 psc); //TIM2定时器初始化
void DMA1_Init(void);
void GPIOA_Init(void);
void ADC_Pin_Init(void);//ADC1初始化
u16 Get_Adc(u8 ch); //获取一次ADC的值
u16 Get_Adc_Average(u8 ch, u8 times); //ADC采样值进行均值滤波
float map(float value, float fromLow, float fromHigh, float toLow, float toHigh); //映射函数
int mapChValue(int val, int lower, int middle, int upper, int reverse);//通道的重映射函数
int GetMedianNum(volatile u16 * bArray, int ch);//中值滤波
#endif
