/*
*********************************************************************************************************
*
*    模块名称 : 示波器模块ADC底层的驱动
*    文件名称 : bsp_cpu_adc.h
*    说    明 : 头文件
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_CPU_ADC_H
#define __BSP_CPU_ADC_H

#define FREQ_NUM 17
extern const int32_t TabelFreq[FREQ_NUM];

#define BUFF_SIZE_NUM 10 /* 采样深度 */
extern const int TabelBufSize[BUFF_SIZE_NUM];

/* ADC控制结构 */
typedef struct
{
    uint16_t DC1;
    uint16_t DC2;
    uint16_t Gain1;
    uint16_t Gain2;
    int16_t Offset1;
    int16_t Offset2;

    uint16_t FreqID;
    uint16_t SampleSizeID;
    uint16_t TrigLevel;
    uint16_t TrigPos;
    uint16_t TrigMode;
    uint16_t TrigChan;
    uint16_t TrigEdge;
    uint16_t ChanEnable;
    uint16_t Run;

    uint16_t MeasuteMode; /* 1表示电流检测模式 */
    uint16_t CurrGain;        /* 电流增益(量程) */

    uint32_t DmaPos;

    uint8_t TrigFlag; /* 触发标志 */

    uint8_t DmaTransCplt; /* DMA传输完成一轮, 触发时，至少DMA采集完一轮才有效 */
} DSO_T;

/* 触发模式 */
enum
{
    TRIG_MODE_AUTO = 0,        /* 自动触发 */
    TRIG_MODE_NORMAL = 1, /* 普通触发 */
    TRIG_MODE_SINGLE = 2, /* 单次触发 */
};

/* 触发模式 */
enum
{
    TRIG_EDGE_FALLING = 0, /* 下降沿触发 */
    TRIG_EDGE_RISING = 1,    /* 上升沿触发 */
};

/* ADC通道定义。 用于低速多通道扫描模式 */
/*
        PF11/ADC1_INP2    ---- CH1电压
        PA6/ADC1_INP3    ---- 高端电流
    
        PC3_C/ADC3_INP1    ---- CH2电压
        PF5/ADC3_INP4    ---- TVCC输出电流检测
        PF3/ADC3_INP5    ---- NTC热敏电阻阻值检测
        PF8/ADC3_INP7    ---- 负载电压
        PH2/ADC3_INP13    ---- TVCC电压检测        
        PH3/ADC3_INP14    ---- 12V供电电压检测
        PH5/ADC3_INP16    ---- USB供电电压检测            
    */
typedef enum
{
    AN_CH1 = 0,                 /* CH1电压 */
    AN_CH2 = 1,                 /* CH2电压 */
    AN_HIGH_SIDE_VOLT, /* 高端负载电压 */
    AN_HIGH_SIDE_CURR, /* 高端负载电流 */

    AN_TVCC_VOLT, /* TVCC电压检测    */
    AN_TVCC_CURR, /* TVCC输出电流    */
    AN_NTC_RES,        /* NTC热敏电阻阻值检测 */
    AN_12V_VOLT,    /* 12V供电电压检测 */
    AN_USB_VOLT,    /* USB供电电压检测 */
} ADC_CHAN_E;


#define DSO_PACKAGE_SIZE 1024

/* Definition of ADCH1 conversions data table size */
#define ADC_BUFFER_SIZE ((uint32_t)16 * 1024) /* Size of array aADCH1ConvertedData[], Aligned on cache line size */

/* Variable containing ADC conversions data */
#if 1
    extern uint16_t *aADCH1ConvertedData;
    extern uint16_t *aADCH2ConvertedData;

    extern float *g_Ch1WaveBuf; /* 校准以后的值 */
    extern float *g_Ch2WaveBuf;
#else
    extern ALIGN_32BYTES(uint16_t aADCH1ConvertedData[ADC_BUFFER_SIZE]);
    extern ALIGN_32BYTES(uint16_t aADCH2ConvertedData[ADC_BUFFER_SIZE]);

    extern float g_Ch1WaveBuf[ADC_BUFFER_SIZE]; /* 校准以后的值 */
    extern float g_Ch2WaveBuf[ADC_BUFFER_SIZE];
#endif

extern DSO_T g_tDSO;

void DSO_InitHard(void);
void DSO_ConfigCtrlGPIO(void);
void DSO_SetDC(uint8_t _ch, uint8_t _mode);
void DSO_SetGain(uint8_t _ch, uint8_t _gain);
void DSO_SetOffset(uint8_t _ch, int16_t _OffsetVolt);
void DSO_SetCurrGain(uint8_t _gain);
void DSO_SetTriger(void);

void DSO_StartADC(uint32_t _uiFreq);
void DSO_PauseADC(void);
void DSO_StopADC(void);

void DSO_SetSampRate(uint32_t _ulFreq);
void DSO_LockWave(void);

float bsp_GetAdcAvg(uint8_t _ch);
void bsp_AdcTask10ms(void);

float bsp_AdcToHighSideCurr(float _adc);
float bsp_AdcToHighSideVolt(float _adc);
float bsp_AdcToCH1Volt(float _adc);
float bsp_AdcToCH2Volt(float _adc);

#endif
