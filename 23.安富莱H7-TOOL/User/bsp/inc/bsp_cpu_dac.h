/*
*********************************************************************************************************
*
*    模块名称 : DAC驱动模块 
*    文件名称 : bsp_cpu_dac.h
*    版    本 : V1.0
*
*    Copyright (C), 2015-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_CPU_DAC_H
#define __BSP_CPU_DAC_H

typedef enum
{
    DAC_WAVE_NO = 0,
    DAC_WAVE_SIN,
    DAC_WAVE_SQUARE,
    DAC_WAVE_TRI
} DAC_WAVE_TYPE_E;

/* 波形控制结构 */
typedef struct
{
    uint8_t Type;
    uint8_t VoltRange;
    int16_t VoltMax;
    int16_t VoltMin;
    uint32_t Freq;
    uint16_t Duty;
    uint32_t CycleCount;
    uint32_t CycleSetting;
    uint8_t Run;
} DAC_WAVE_T;

/* 这些函数是通用的设置，软件控制DAC数据 */
void bsp_InitDAC1(void);
void bsp_SetDAC1(uint16_t _dac);

void bsp_InitDAC2(void);
void bsp_SetDAC2(uint16_t _dac);

/* 下面的函数用于DMA波形发生器 */
void dac1_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq);
void dac2_SetSinWave(uint16_t _low, uint16_t _high, uint32_t _freq);
void dac1_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac1_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac1_StopWave(void);
void dac1_StartDacWave(void);

void dac2_InitForDMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq);
void dac2_SetSinWave(uint16_t _low, uint16_t _high, uint32_t _freq);
void dac2_SetRectWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac2_SetTriWave(uint16_t _low, uint16_t _high, uint32_t _freq, uint16_t _duty);
void dac2_StopWave(void);

int16_t dac1_DacToVolt(uint16_t _dac);
int16_t dac1_VoltToDac(int16_t _volt);
int16_t dac1_DacToCurr(uint16_t _dac);
int16_t dac1_CurrToDac(int16_t _curr);

#define WAVE_SAMPLE_SIZE 128
extern uint16_t g_Wave1[WAVE_SAMPLE_SIZE];
//extern uint16_t g_Wave2[128];

extern DAC_WAVE_T g_tDacWave;

#endif
