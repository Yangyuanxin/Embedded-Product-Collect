/*
*********************************************************************************************************
*
*    模块名称 : 示波器ADC模块
*    文件名称 : bsp_adc_dso.c
*    版    本 : V1.0
*    说    明 : 使用STM32内部ADC，同步采集两路波形。占用了部分GPIO控制示波器模块的增益和耦合方式。
*                使用 ADC_EXTERNALTRIG_T3_TRGO 作为ADC触发源
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2018-10-25  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"
#include "nvic_prio_cfg.h"

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
        
/*
    H7-TOOL 示波器电路，AC/DC耦合、增益控制GPIO
*/    
/* CH1 增益控制 */
#define G1A_PIN                 0
#define G1A_0()                 HC595_WriteGPIO(G1A_PIN, 0)
#define G1A_1()                 HC595_WriteGPIO(G1A_PIN, 1)

#define G1B_PIN                 1
#define G1B_0()                 HC595_WriteGPIO(G1B_PIN, 0)
#define G1B_1()                 HC595_WriteGPIO(G1B_PIN, 1)

#define G1C_PIN                 2
#define G1C_0()                 HC595_WriteGPIO(G1C_PIN, 0)
#define G1C_1()                 HC595_WriteGPIO(G1C_PIN, 1)

/* CH2 增益控制 */
#define G2A_PIN                 4
#define G2A_0()                 HC595_WriteGPIO(G2A_PIN, 0)
#define G2A_1()                 HC595_WriteGPIO(G2A_PIN, 1)

#define G2B_PIN                 5
#define G2B_0()                 HC595_WriteGPIO(G2B_PIN, 0)
#define G2B_1()                 HC595_WriteGPIO(G2B_PIN, 1)

#define G2C_PIN                 6
#define G2C_0()                 HC595_WriteGPIO(G2C_PIN, 0)
#define G2C_1()                 HC595_WriteGPIO(G2C_PIN, 1)

/* CH1 交流/直流耦合 */
#define AC1_PIN                 3
#define AC1_0()                 HC595_WriteGPIO(AC1_PIN, 0)
#define AC1_1()                 HC595_WriteGPIO(AC1_PIN, 1)

/* CH2 交流/直流耦合 */
#define AC2_PIN                 7
#define AC2_0()                 HC595_WriteGPIO(AC2_PIN, 0)
#define AC2_1()                 HC595_WriteGPIO(AC2_PIN, 1)


/* 高端电流 增益控制 */
#define GC_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define GC_GPIO                 GPIOG
#define GC_PIN                  GPIO_PIN_2
#define GC_0()                  BSP_SET_GPIO_0(GC_GPIO, GC_PIN)
#define GC_1()                  BSP_SET_GPIO_1(GC_GPIO, GC_PIN)


/* 示波器模式，电压模式和电流模式使用不同的ADC通道 */

#define    ENABLE_DIFFERENTIAL_ENDED        0    /* 0表示配置为单端，1表示配置为差分 */

#define    H7_ADC_SAMPLETIME_1CYCLE_5       ADC_SAMPLETIME_1CYCLE_5

/********************************** 电压模式的GPIO定义 ***********************************/

/*-------------ADC CH1通道GPIO, ADC通道，DMA定义 -------------*/
#if 1
    #define ADCH1                           ADC1
    #define ADCH1_CLK_ENABLE()              __HAL_RCC_ADC12_CLK_ENABLE()
    #define ADCH1_FORCE_RESET()             __HAL_RCC_ADC12_FORCE_RESET()
    #define ADCH1_RELEASE_RESET()           __HAL_RCC_ADC12_RELEASE_RESET()
    #define ADCH1_CHANNEL                   ADC_CHANNEL_2        /* 用PF11 */
    
    /* 差分正端 PF11/ADC1_INP2 + PF13/ADC2_INP2 + PF9/ADC3_INP2 */
    #define ADCH1_P_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
    #define ADCH1_P_GPIO_PORT               GPIOF
    //#define ADCH1_P_PIN                GPIO_PIN_11 | GPIO_PIN_9 | GPIO_PIN_13
    #define ADCH1_P_PIN                     GPIO_PIN_11

    /* 差分负端 PF12/ADC1_INN2 + PF10/ADC3_INN2 + PF14/ADC2_INN2 */
    #define ADCH1_N_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
    #define ADCH1_N_GPIO_PORT               GPIOF
    #define ADCH1_N_PIN                     GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_14

    #define CH1_DMA_CLK_ENABLE()            __HAL_RCC_DMA1_CLK_ENABLE()
    #define CH1_DMA_Stream                  DMA1_Stream1
    #define CH1_DMA_Stream_IRQn             DMA1_Stream1_IRQn
    #define CH1_DMA_Stream_IRQHandle        DMA1_Stream1_IRQHandler
    #define CH1_DMA_REQUEST_ADC             DMA_REQUEST_ADC1
#else
    #define ADCH1                            ADC3
    #define ADCH1_CLK_ENABLE()               __HAL_RCC_ADC3_CLK_ENABLE()
    #define ADCH1_FORCE_RESET()              __HAL_RCC_ADC3_FORCE_RESET()
    #define ADCH1_RELEASE_RESET()            __HAL_RCC_ADC3_RELEASE_RESET()
    #define ADCH1_CHANNEL                    ADC_CHANNEL_2        /* 用PF11 */
    /* 差分正端 PF11/ADC1_INP2 + PF13/ADC2_INP2 + PF9/ADC3_INP2 */
    #define ADCH1_P_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
    #define ADCH1_P_GPIO_PORT          GPIOF
    //#define ADCH1_P_PIN                GPIO_PIN_11 | GPIO_PIN_9 | GPIO_PIN_13
    #define ADCH1_P_PIN                GPIO_PIN_9

    /* 差分负端 PF12/ADC1_INN2 + PF10/ADC3_INN2 + PF14/ADC2_INN2 */
    #define ADCH1_N_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()
    #define ADCH1_N_GPIO_PORT          GPIOF
    #define ADCH1_N_PIN                GPIO_PIN_12 | GPIO_PIN_10 | GPIO_PIN_14

    #define CH1_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()
    #define CH1_DMA_Stream                     DMA1_Stream1
    #define CH1_DMA_Stream_IRQn                 DMA1_Stream1_IRQn
    #define CH1_DMA_Stream_IRQHandle         DMA1_Stream1_IRQHandler
    #define CH1_DMA_REQUEST_ADC                 DMA_REQUEST_ADC1
#endif

/*------------- ADC CH2通道GPIO, ADC通道，DMA定义 -------------*/
#if 1
    #define ADCH2                               ADC3
    #define ADCH2_CLK_ENABLE()                  __HAL_RCC_ADC3_CLK_ENABLE()
    #define ADCH2_FORCE_RESET()                 __HAL_RCC_ADC3_FORCE_RESET()
    #define ADCH2_RELEASE_RESET()               __HAL_RCC_ADC3_RELEASE_RESET()
    #define ADCH2_CHANNEL                       ADC_CHANNEL_1
    /* 差分正端 PC3_C/ADC3_INP1 */
    #define ADCH2_P_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
    #define ADCH2_P_PIN                         GPIO_PIN_3
    #define ADCH2_P_GPIO_PORT                   GPIOC
    /* 差分负端 PC2_C/ADC3_INN1 */
    #define ADCH2_N_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOC_CLK_ENABLE()
    #define ADCH2_N_PIN                         GPIO_PIN_2
    #define ADCH2_N_GPIO_PORT                   GPIOC

    #define CH2_DMA_CLK_ENABLE()                __HAL_RCC_DMA2_CLK_ENABLE()
    #define CH2_DMA_Stream                      DMA2_Stream1
    #define CH2_DMA_Stream_IRQn                 DMA2_Stream1_IRQn
    #define CH2_DMA_Stream_IRQHandle            DMA2_Stream1_IRQHandler
    #define CH2_DMA_REQUEST_ADC                 DMA_REQUEST_ADC3
#else
    #define ADCH2                            ADC3
    #define ADCH2_CLK_ENABLE()               __HAL_RCC_ADC3_CLK_ENABLE()
    #define ADCH2_FORCE_RESET()              __HAL_RCC_ADC3_FORCE_RESET()
    #define ADCH2_RELEASE_RESET()            __HAL_RCC_ADC3_RELEASE_RESET()
    #define ADCH2_CHANNEL                    ADC_CHANNEL_2

    /* 差分正端 PF9/ADC3_INP2 */ 
    #define ADCH2_P_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOF_CLK_ENABLE()
    #define ADCH2_P_PIN                        GPIO_PIN_9
    #define ADCH2_P_GPIO_PORT                  GPIOF
    /* 差分负端 PC2_C/ADC3_INN1 */
    #define ADCH2_N_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()
    #define ADCH2_N_PIN                        GPIO_PIN_2
    #define ADCH2_N_GPIO_PORT                  GPIOC

    #define CH2_DMA_CLK_ENABLE()           __HAL_RCC_DMA2_CLK_ENABLE()
    #define CH2_DMA_Stream                     DMA2_Stream1
    #define CH2_DMA_Stream_IRQn                 DMA2_Stream1_IRQn
    #define CH2_DMA_Stream_IRQHandle         DMA2_Stream1_IRQHandler
    #define CH2_DMA_REQUEST_ADC                 DMA_REQUEST_ADC3

#endif


/********************************** 电压模式的GPIO定义（结束）********************************/


/********************************** 高端电流模式的GPIO定义 (无差分） ***********************************/
/*
    高端电流： PA6/ADC12_INP3
    负载电压： PF8/ADC3_INP7
*/
/* ADC CH1通道GPIO, ADC通道，DMA定义 */
#define CT_ADCH1                            ADC1
#define CT_ADCH1_CLK_ENABLE()               __HAL_RCC_ADC12_CLK_ENABLE()
#define CT_ADCH1_FORCE_RESET()              __HAL_RCC_ADC12_FORCE_RESET()
#define CT_ADCH1_RELEASE_RESET()            __HAL_RCC_ADC12_RELEASE_RESET()
#define CT_ADCH1_CHANNEL                    ADC_CHANNEL_3
/* 差分正端 PA6/ADC12_INP3 */
#define CT_ADCH1_P_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define CT_ADCH1_P_GPIO_PORT                GPIOA
#define CT_ADCH1_P_PIN                      GPIO_PIN_6
/* 差分负端没有 */
#define CT_ADCH1_N_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define CT_ADCH1_N_GPIO_PORT                GPIOA
#define CT_ADCH1_N_PIN                      GPIO_PIN_6

#define CT_CH1_DMA_CLK_ENABLE()             __HAL_RCC_DMA1_CLK_ENABLE()
#define CT_CH1_DMA_Stream                   DMA1_Stream1
#define CT_CH1_DMA_Stream_IRQn              DMA1_Stream1_IRQn
#define CT_CH1_DMA_Stream_IRQHandle         DMA1_Stream1_IRQHandler
#define CT_CH1_DMA_REQUEST_ADC              DMA_REQUEST_ADC1

/* ADC CH2通道GPIO, ADC通道，DMA定义 */
#define CT_ADCH2                            ADC3
#define CT_ADCH2_CLK_ENABLE()               __HAL_RCC_ADC3_CLK_ENABLE()
#define CT_ADCH2_FORCE_RESET()              __HAL_RCC_ADC3_FORCE_RESET()
#define CT_ADCH2_RELEASE_RESET()            __HAL_RCC_ADC3_RELEASE_RESET()
#define CT_ADCH2_CHANNEL                    ADC_CHANNEL_7
/* 差分正端 PF8/ADC3_INP7 */
#define CT_ADCH2_P_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOF_CLK_ENABLE()
#define CT_ADCH2_P_PIN                      GPIO_PIN_8
#define CT_ADCH2_P_GPIO_PORT                GPIOF
/* 差分负端没有，PF8/ADC3_INP7 */
#define CT_ADCH2_N_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOF_CLK_ENABLE()
#define CT_ADCH2_N_PIN                      GPIO_PIN_8
#define CT_ADCH2_N_GPIO_PORT                GPIOF

#define CT_CH2_DMA_CLK_ENABLE()             __HAL_RCC_DMA2_CLK_ENABLE()
#define CT_CH2_DMA_Stream                   DMA2_Stream1
#define CT_CH2_DMA_Stream_IRQn              DMA2_Stream1_IRQn
#define CT_CH2_DMA_Stream_IRQHandle         DMA2_Stream1_IRQHandler
#define CT_CH2_DMA_REQUEST_ADC              DMA_REQUEST_ADC3

/********************************** 高端电流模式的GPIO定义（结束） *******************************/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* ADC handle declaration */
ADC_HandleTypeDef             AdcHandle1 = {0};
ADC_HandleTypeDef             AdcHandle2 = {0};

/* ADC channel configuration structure declaration */
ADC_ChannelConfTypeDef        sConfig1 = {0};
ADC_ChannelConfTypeDef        sConfig2 = {0};

#if 1   /* 2020-07-17 修改为指针访问，腾出内存给heap */
    /* 0x38000000  64KB */
    uint16_t *aADCH1ConvertedData = (uint16_t *)0x38000000;
    uint16_t *aADCH2ConvertedData = (uint16_t *)(0x38000000 + 32 * 1024);

    /* 0x20000000  128KB */
    float *g_Ch1WaveBuf = (float *)0x20000000;
    float *g_Ch2WaveBuf = (float *)(0x20000000 + 64 * 1024);
#else
    ALIGN_32BYTES (uint16_t   aADCH1ConvertedData[ADC_BUFFER_SIZE]);
    ALIGN_32BYTES (uint16_t   aADCH2ConvertedData[ADC_BUFFER_SIZE]);

    float g_Ch1WaveBuf[ADC_BUFFER_SIZE];
    float g_Ch2WaveBuf[ADC_BUFFER_SIZE];
#endif

#define SCAN_MODE_ADC1_NUM            2        /* 低速扫描模式, ADC1通道个数 */
#define SCAN_MODE_ADC3_NUM            7        /* 低速扫描模式, ADC3通道个数 */
#define SCAN_MODE_SAMPLE_SIZE        16        /* 低速扫描模式, 每通道样本个数. 用于软件平均 */
#define SCAN_MODE_SAMPLETIME_ADC1        ADC_SAMPLETIME_64CYCLES_5    //ADC_SAMPLETIME_810CYCLES_5
#define SCAN_MODE_SAMPLETIME_ADC3        ADC_SAMPLETIME_64CYCLES_5

const int32_t TabelFreq[FREQ_NUM] =
{
    100,
    200,
    500,
    1 * 1000,
    2 * 1000,
    5 * 1000,
    10 * 1000,
    20 * 1000,
    50 * 1000,
    100 * 1000,
    200 * 1000,
    500 * 1000,
    1 * 1000000,
    2 * 1000000,
    5 * 1000000,
    10 * 1000000,
    20 * 1000000,
};

/* 采样深度 */
const int TabelBufSize[BUFF_SIZE_NUM] =
{
    1*1024,
    2*1024,
    4*1024,
    8*1024,
    16*1024,
    32*1024,
    64*1024,
    128*1024,
    256*1024,
    512*1024,
};

DSO_T g_tDSO;

static void TIM3_Config(uint32_t _freq);
static float AdcSumAvg(uint8_t _AdcNo, uint8_t _Offset);

void bsp_StopAdcCH1(void);
void bsp_StopAdcCH2(void);

void __DSO_EnableDog(void);
void __DSO_DisableDog(void);

void DSO_SetTrigerToggle(uint8_t _TrigEdge);

/*
*********************************************************************************************************
*    函 数 名: DSO_TrigFinished
*    功能说明: 触发采集结束. 在中断中执行的.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_TrigFinished(void)
{
    //DSO_LockWave();  -- 不能在这个地方保存数据
    DSO_PauseADC();
    g_tDSO.TrigFlag = 1;
    
}

/*
*********************************************************************************************************
*    函 数 名: ADC_IRQHandler
*    功能说明: ADC模拟看门狗中断.  ; ADC1, ADC2 
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void IRQ_WatchDog(void)
{
    uint32_t TransBuffSize;    /* 通信传输的采样深度- PC软件设置的 */
    int32_t DmaPos;
    int32_t BeginPos;
    uint32_t tmp_isr;
    uint32_t tmp_ier;
    uint8_t triged = 0;
    uint16_t *pAdcBuf;
    ADC_HandleTypeDef *hadc;
    
    /* CH1 触发 */
    tmp_isr = AdcHandle1.Instance->ISR;
    tmp_ier = AdcHandle1.Instance->IER;
    if (((tmp_isr & ADC_FLAG_AWD1) == ADC_FLAG_AWD1) && ((tmp_ier & ADC_IT_AWD1) == ADC_IT_AWD1))
    {
        /* Clear ADC Analog watchdog flag */
        AdcHandle1.Instance->ISR = ADC_FLAG_AWD1;
        
        pAdcBuf = aADCH1ConvertedData;
        hadc = &AdcHandle1;
        DmaPos = ADC_BUFFER_SIZE - CT_CH1_DMA_Stream->NDTR;        /* DMA传输位置 */        
        triged = 1;
    }
    
    /* CH2 触发*/
    tmp_isr = AdcHandle2.Instance->ISR;
    tmp_ier = AdcHandle2.Instance->IER;
    if (((tmp_isr & ADC_FLAG_AWD1) == ADC_FLAG_AWD1) && ((tmp_ier & ADC_IT_AWD1) == ADC_IT_AWD1))
    {
        /* Clear ADC Analog watchdog flag */
        AdcHandle2.Instance->ISR = ADC_FLAG_AWD1;
        
        pAdcBuf = aADCH2ConvertedData;
        hadc = &AdcHandle2;        
        DmaPos = ADC_BUFFER_SIZE - CT_CH2_DMA_Stream->NDTR;        /* DMA传输剩余字节数 */    
        triged = 2;
    }    

    if (triged == 1 || triged == 2)
    {
        if (DmaPos >= ADC_BUFFER_SIZE)
        {
            DmaPos = 0;
        }

        /* 软件滤波，防止触发电平附近抖动 */
        /* 在启动ADC DMA后，必须采集至少10个样本，才开始做触发有效判断 */
        if (pAdcBuf[20] != 0 || pAdcBuf[21] != 0)
        //if (g_tDSO.DmaTransCplt > 2)
        {
            uint32_t adc_sum = 0;
            uint32_t avg_last, avg_now;
            uint8_t i;
            int32_t pos;
            int32_t now;
            uint32_t rem_time;
            
            now = DmaPos;            
            if (--now < 0)
            {
                now = ADC_BUFFER_SIZE - 1;
            }
            pos = now;
            
            adc_sum = 0;
            for (i = 0; i < 5; i++)
            {
                if (--pos < 0)
                {
                    pos = ADC_BUFFER_SIZE - 1;
                }                    
                adc_sum += pAdcBuf[pos];
            }
            avg_now = adc_sum / 5;
            
            adc_sum = 0;
            for (i = 0; i < 10; i++)
            {
                if (--pos < 0)
                {
                    pos = ADC_BUFFER_SIZE - 1;
                }                    
                adc_sum += pAdcBuf[pos];
            }
            avg_last = adc_sum / 10;
            
            /* 下降沿或上升沿 */
            if ((avg_last > g_tDSO.TrigLevel && avg_now <= g_tDSO.TrigLevel && g_tDSO.TrigEdge == TRIG_EDGE_FALLING) ||
                (avg_last < g_tDSO.TrigLevel && avg_now >= g_tDSO.TrigLevel && g_tDSO.TrigEdge == TRIG_EDGE_RISING))                
            {
                /* Disable the ADC Analog watchdog interrupt */
                __HAL_ADC_DISABLE_IT(hadc, ADC_IT_AWD1);  
                
                /* 采样深度 */
                if (g_tDSO.SampleSizeID < BUFF_SIZE_NUM)
                {
                    TransBuffSize = TabelBufSize[g_tDSO.SampleSizeID];
                }
                else
                {
                    TransBuffSize = 1 * 1024;
                }
                
                /* 计算剩余采集时间 us单位 */
                rem_time = ((int64_t)(TransBuffSize * (100 - g_tDSO.TrigPos) / 100) * 1000000) / TabelFreq[g_tDSO.FreqID];
                if (rem_time == 0)
                {
                    DSO_TrigFinished();                
                }
                else
                {
                    bsp_StartHardTimer(1, rem_time, DSO_TrigFinished);    
                }
                /* 记录采样位置 */
                BeginPos = DmaPos - TransBuffSize * g_tDSO.TrigPos / 100;
                if (BeginPos < 0)
                {
                    BeginPos = ADC_BUFFER_SIZE + BeginPos;
                }
        
                g_tDSO.DmaPos = BeginPos;    /* 保存触发前的位置 */             
            }
            else
            {                
                if (g_tDSO.TrigEdge == TRIG_EDGE_FALLING)
                {
                    if (avg_last < g_tDSO.TrigLevel)
                    {
                        DSO_SetTrigerToggle(TRIG_EDGE_RISING);
                    }
                    else
                    {
                        DSO_SetTrigerToggle(TRIG_EDGE_FALLING);
                    }
                }
                else    /* TRIG_EDGE_RISING */
                {
                    if (avg_last < g_tDSO.TrigLevel)
                    {
                        DSO_SetTrigerToggle(TRIG_EDGE_RISING);
                    }
                    else
                    {
                        DSO_SetTrigerToggle(TRIG_EDGE_FALLING);
                    }
                }
                
                /* Clear ADC Analog watchdog flag */
                AdcHandle1.Instance->ISR = ADC_FLAG_AWD1;  
                AdcHandle2.Instance->ISR = ADC_FLAG_AWD1;                
            }            
        }

    }        
}

void ADC_IRQHandler(void)
{
    IRQ_WatchDog();
}

void ADC3_IRQHandler(void)
{
    IRQ_WatchDog();
}

/*
*********************************************************************************************************
*    函 数 名: DSO_InitHard
*    功能说明: 配置控制用通道耦合和增益的GPIO, 配置ADC
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_InitHard(void)
{
    DSO_ConfigCtrlGPIO();
    
    g_tDSO.DC1 = 0;
    g_tDSO.DC2 = 0;
    g_tDSO.Gain1 = 0;
    g_tDSO.Gain2 = 0;
    g_tDSO.FreqID = 0;
    g_tDSO.SampleSizeID = 0;
    g_tDSO.TrigChan = 0;
    g_tDSO.TrigLevel = 32767;
    g_tDSO.TrigPos = 50;
    g_tDSO.TrigMode = 0;
    g_tDSO.Run = 0;
    
    g_tDSO.MeasuteMode = 0;
    
    g_tDSO.DmaPos = 0;
    
    DSO_SetGain(1, g_tDSO.Gain1);
    DSO_SetGain(2, g_tDSO.Gain2);
    
    /* V1.30 */
    {
        uint32_t i;
        
        for (i = 0; i < ADC_BUFFER_SIZE; i++)
        {
            g_Ch1WaveBuf[i] = 0;
            g_Ch2WaveBuf[i] = 0;
        }
    }
    
}

/*
*********************************************************************************************************
*    函 数 名: DSO_ConfigCtrlGPIO
*    功能说明: 配置控制用通道耦合和增益的GPIO
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_ConfigCtrlGPIO(void)
{
    /* 配置控制增益和耦合的GPIO */
    GPIO_InitTypeDef gpio_init;

    /* 打开GPIO时钟 */
    GC_CLK_ENABLE();
    
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        /* 设置推挽输出 */
    gpio_init.Pull = GPIO_NOPULL;                /* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;      /* GPIO速度等级 */    
    

    gpio_init.Pin = GC_PIN;    
    HAL_GPIO_Init(GC_GPIO, &gpio_init);    

    DSO_SetDC(1, 0);    /* CH1选择AC耦合 */
    DSO_SetDC(2, 0);    /* CH1选择AC耦合 */
    DSO_SetGain(1, 0);    /* CH1选择小增益 衰减1/5, 第2个参数1表示不衰减1;1 */
    DSO_SetGain(2, 0);    /* CH2选择小增益 衰减1/5, 第2个参数1表示不衰减1;1 */

    DSO_SetCurrGain(1);    /* 电流检测量程设置为 2A */
}

/*
*********************************************************************************************************
*    函 数 名: DSO_SetDC
*    功能说明: 控制AC DC耦合模式
*    形    参: _ch : 通道1或2
*              _mode : 0或1.  1表示DC耦合 0表示AC耦合
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetDC(uint8_t _ch, uint8_t _mode)
{    
    if (_ch == 1)
    {
        if (_mode == 1)
        {
            AC1_1();
        }
        else
        {
            AC1_0();
        }
    }
    else
    {
        if (_mode == 1)
        {
            AC2_1();
        }
        else
        {
            AC2_0();
        }
    }    
    HC595_LockData();
}

/*
*********************************************************************************************************
*    函 数 名: DSO_SetGain
*    功能说明: 控制增益模式
*    形    参: _ch : 通道1或2
*              _gain : 0-7.  7最大
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetGain(uint8_t _ch, uint8_t _gain)
{    
    if (_ch == 1)
    {
        if (_gain == 0)      {G1C_0(); G1B_0(); G1A_0();}
        else if (_gain == 1) {G1C_0(); G1B_0(); G1A_1();}
        else if (_gain == 2) {G1C_0(); G1B_1(); G1A_0();}
        else if (_gain == 3) {G1C_0(); G1B_1(); G1A_1();}
        else if (_gain == 4) {G1C_1(); G1B_0(); G1A_0();}
        else if (_gain == 5) {G1C_1(); G1B_0(); G1A_1();}
        else if (_gain == 6) {G1C_1(); G1B_1(); G1A_0();}
        else if (_gain == 7) {G1C_1(); G1B_1(); G1A_1();}
    }
    else if (_ch == 2)
    {
        if (_gain == 0)      {G2C_0(); G2B_0(); G2A_0();}
        else if (_gain == 1) {G2C_0(); G2B_0(); G2A_1();}
        else if (_gain == 2) {G2C_0(); G2B_1(); G2A_0();}
        else if (_gain == 3) {G2C_0(); G2B_1(); G2A_1();}
        else if (_gain == 4) {G2C_1(); G2B_0(); G2A_0();}
        else if (_gain == 5) {G2C_1(); G2B_0(); G2A_1();}
        else if (_gain == 6) {G2C_1(); G2B_1(); G2A_0();}
        else if (_gain == 7) {G2C_1(); G2B_1(); G2A_1();}    
    }
    HC595_LockData();
}


/*
*********************************************************************************************************
*    函 数 名: DSO_SetOffset
*    功能说明: 设置直流偏值
*    形    参: _ch : 通道1或2
*              _OffsetVolt : 直流偏值电压 -1250 , +1250
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetOffset(uint8_t _ch, int16_t _OffsetVolt)
{    
#if    1
    ;    /* 硬件不支持直流偏值设置 */
#else    
    if (_ch == 1)    /* 通道1 */
    {
        //MCP4725_SetVolt(0, _OffsetVolt + 1250);
        STM8_WritePWM(1, _OffsetVolt + 1250);
    }
    else    /* 通道2 */
    {
        //MCP4725_SetVolt(1, _OffsetVolt + 1250);
        STM8_WritePWM(2, _OffsetVolt + 1250);
    }    
#endif    
}

/*
*********************************************************************************************************
*    函 数 名: DSO_SetCurrGain
*    功能说明: 控制电流增益
*    形    参: _gain : 0表示200mA, 1表示2A
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetCurrGain(uint8_t _gain)
{    
    if (_gain == 0)
    {
        GC_1();    
    }
    else
    {
        GC_0();
    }
}

/*
*********************************************************************************************************
*    函 数 名: DSO_SetTriger
*    功能说明: 设置硬件触发。利用ADC看门狗实现. 
*    形    参: g_tDSO.TrigMode   触发模式
*              g_tDSO.TrigLevel  触发电平（0-65535）
*              g_tDSO.TrigPos    触发位置（0-100）
*              g_tDSO.TrigChan   触发通道
*    返 回 值: 无
*********************************************************************************************************
*/

void __DSO_EnableDog(void)
{
    HAL_NVIC_EnableIRQ(ADC_IRQn);    
    HAL_NVIC_EnableIRQ(ADC3_IRQn); 
}

void __DSO_DisableDog(void)
{
    HAL_NVIC_DisableIRQ(ADC_IRQn);    
    HAL_NVIC_DisableIRQ(ADC3_IRQn); 
}

void DSO_SetTriger(void)
{    
    ADC_AnalogWDGConfTypeDef WdgCfg;    

    /* ADC 通道1模拟看门狗配置 */
    WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    WdgCfg.Channel = ADCH1_CHANNEL;            
    WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;    // ADC_ANALOGWATCHDOG_ALL_REG;
    if (g_tDSO.TrigChan == 0)
    {
        WdgCfg.ITMode = ENABLE;        
    }
    else
    {
        WdgCfg.ITMode = DISABLE;    
    }
    
    if (g_tDSO.TrigMode == TRIG_MODE_NORMAL || g_tDSO.TrigMode == TRIG_MODE_SINGLE)
    {
        if (g_tDSO.TrigEdge == TRIG_EDGE_RISING)
        {
            WdgCfg.HighThreshold = g_tDSO.TrigLevel;
            WdgCfg.LowThreshold = 0;
        }
        else if (g_tDSO.TrigEdge == TRIG_EDGE_FALLING)
        {
            WdgCfg.HighThreshold = 65535;
            WdgCfg.LowThreshold = g_tDSO.TrigLevel;
        }
    }
    else    /* 自动触发，关闭看门狗 */
    {
        WdgCfg.HighThreshold = 65535;
        WdgCfg.LowThreshold = 0;
    }

    HAL_ADC_AnalogWDGConfig(&AdcHandle1, &WdgCfg);    

    /* ADC 通道2模拟看门狗配置 */
    WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    WdgCfg.Channel = ADCH2_CHANNEL;            
    WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;    // ADC_ANALOGWATCHDOG_ALL_REG;
    if (g_tDSO.TrigChan == 1)
    {
        WdgCfg.ITMode = ENABLE;        
    }
    else
    {
        WdgCfg.ITMode = DISABLE;    
    }
    if (g_tDSO.TrigMode == TRIG_MODE_NORMAL || g_tDSO.TrigMode == TRIG_MODE_SINGLE)
    {
        if (g_tDSO.TrigEdge == TRIG_EDGE_RISING)
        {
            WdgCfg.HighThreshold = g_tDSO.TrigLevel;
            WdgCfg.LowThreshold = 0;
        }
        else if (g_tDSO.TrigEdge == TRIG_EDGE_FALLING)
        {
            WdgCfg.HighThreshold = 65535;
            WdgCfg.LowThreshold = g_tDSO.TrigLevel;
        }
    }
    else    /* 自动触发，关闭看门狗 */
    {
        WdgCfg.HighThreshold = 65535;
        WdgCfg.LowThreshold = 0;
    }    
    HAL_ADC_AnalogWDGConfig(&AdcHandle2, &WdgCfg);    

    /* NVIC configuration for ADC interrupt */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(ADC_IRQn, CH1_ADC_IRQ_PRIO, 0);
    HAL_NVIC_DisableIRQ(ADC_IRQn);    
    
    HAL_NVIC_SetPriority(ADC3_IRQn, CH2_ADC3_IRQ_PRIO, 0);
    HAL_NVIC_DisableIRQ(ADC3_IRQn); 

    {
        uint32_t TransBuffSize;    /* 通信传输的采样深度- PC软件设置的 */
        int32_t pre_time;
        
        if (g_tDSO.TrigMode == TRIG_MODE_NORMAL || g_tDSO.TrigMode == TRIG_MODE_SINGLE)
        {
            /* 采样深度 */
            if (g_tDSO.SampleSizeID < BUFF_SIZE_NUM)
            {
                TransBuffSize = TabelBufSize[g_tDSO.SampleSizeID];
            }
            else
            {
                TransBuffSize = 1 * 1024;
            }
            
            /* 计算预采集时间 us单位 */
            pre_time = ((int64_t)(TransBuffSize * (g_tDSO.TrigPos) / 100) * 1000000) / TabelFreq[g_tDSO.FreqID];
            if (pre_time == 0)
            {
                __DSO_EnableDog();                
            }
            else
            {
                bsp_StartHardTimer(2, pre_time, __DSO_EnableDog);    
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: DSO_SetTrigerToggle
*    功能说明: 切换触发方向
*    形    参: _TrigEdge : 触发边沿模式
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetTrigerToggle(uint8_t _TrigEdge)
{    
    ADC_AnalogWDGConfTypeDef WdgCfg = {0};  

   
    /* ADC 通道1模拟看门狗配置 */
    WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    WdgCfg.Channel = ADCH1_CHANNEL;            
    WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;    // ADC_ANALOGWATCHDOG_ALL_REG;
    if (g_tDSO.TrigChan == 0)
    {
        WdgCfg.ITMode = ENABLE;        
    }
    else
    {
        WdgCfg.ITMode = DISABLE;    
    }
    
    if (g_tDSO.TrigMode == TRIG_MODE_NORMAL || g_tDSO.TrigMode == TRIG_MODE_SINGLE)
    {
        if (_TrigEdge == TRIG_EDGE_RISING)
        {
            WdgCfg.HighThreshold = g_tDSO.TrigLevel;
            WdgCfg.LowThreshold = 0;
        }
        else if (_TrigEdge == TRIG_EDGE_FALLING)
        {
            WdgCfg.HighThreshold = 65535;
            WdgCfg.LowThreshold = g_tDSO.TrigLevel;
        }
    }
    else    /* 自动触发，关闭看门狗 */
    {
        WdgCfg.HighThreshold = 65535;
        WdgCfg.LowThreshold = 0;
    }

    HAL_ADC_AnalogWDGConfig(&AdcHandle1, &WdgCfg);    

    /* ADC 通道2模拟看门狗配置 */
    WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    WdgCfg.Channel = ADCH2_CHANNEL;            
    WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;    // ADC_ANALOGWATCHDOG_ALL_REG;
    if (g_tDSO.TrigChan == 1)
    {
        WdgCfg.ITMode = ENABLE;        
    }
    else
    {
        WdgCfg.ITMode = DISABLE;    
    }
    if (g_tDSO.TrigMode == TRIG_MODE_NORMAL || g_tDSO.TrigMode == TRIG_MODE_SINGLE)
    {
        if (_TrigEdge == TRIG_EDGE_RISING)
        {
            WdgCfg.HighThreshold = g_tDSO.TrigLevel;
            WdgCfg.LowThreshold = 0;
        }
        else if (_TrigEdge == TRIG_EDGE_FALLING)
        {
            WdgCfg.HighThreshold = 65535;
            WdgCfg.LowThreshold = g_tDSO.TrigLevel;
        }
    }
    else    /* 自动触发，关闭看门狗 */
    {
        WdgCfg.HighThreshold = 65535;
        WdgCfg.LowThreshold = 0;
    }    
    HAL_ADC_AnalogWDGConfig(&AdcHandle2, &WdgCfg);    

//    /* NVIC configuration for ADC interrupt */
//    /* Priority: high-priority */
//    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
//    HAL_NVIC_DisableIRQ(ADC_IRQn);    
//    
//    HAL_NVIC_SetPriority(ADC3_IRQn, 0, 0);
//    HAL_NVIC_DisableIRQ(ADC3_IRQn); 

//    {
//        uint32_t TransBuffSize;    /* 通信传输的采样深度- PC软件设置的 */
//        int32_t pre_time;
//        
//        if (g_tDSO.TrigMode == TRIG_MODE_NORMAL || g_tDSO.TrigMode == TRIG_MODE_SINGLE)
//        {
//            /* 采样深度 */
//            if (g_tDSO.SampleSizeID < BUFF_SIZE_NUM)
//            {
//                TransBuffSize = TabelBufSize[g_tDSO.SampleSizeID];
//            }
//            else
//            {
//                TransBuffSize = 1 * 1024;
//            }
//            
//            /* 计算预采集时间 us单位 */
//            pre_time = ((int64_t)(TransBuffSize * (g_tDSO.TrigPos) / 100) * 1000000) / TabelFreq[g_tDSO.FreqID];
//            if (pre_time == 0)
//            {
//                __DSO_EnableDog();                
//            }
//            else
//            {
//                bsp_StartHardTimer(2, pre_time, __DSO_EnableDog);    
//            }
//        }
//    }
}

/*
*********************************************************************************************************
*    函 数 名: DSO_CloseTriger
*    功能说明: 关闭硬件触发。关闭ADC看门狗实现. 
*    形    参: g_tDSO.TrigMode   触发模式
*              g_tDSO.TrigLevel  触发电平（0-65535）
*              g_tDSO.TrigPos    触发位置（0-100）
*              g_tDSO.TrigChan   触发通道
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_CloseTriger(void)
{    
    ADC_AnalogWDGConfTypeDef WdgCfg = {0};    

    /* ADC 通道1模拟看门狗配置 */
    WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    WdgCfg.Channel = ADCH1_CHANNEL;            
    WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;    // ADC_ANALOGWATCHDOG_ALL_REG;
    WdgCfg.ITMode = DISABLE;    
    WdgCfg.HighThreshold = 65535;
    WdgCfg.LowThreshold = 0;
    HAL_ADC_AnalogWDGConfig(&AdcHandle1, &WdgCfg);    

    /* ADC 通道2模拟看门狗配置 */
    WdgCfg.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    WdgCfg.Channel = ADCH2_CHANNEL;            
    WdgCfg.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;    // ADC_ANALOGWATCHDOG_ALL_REG;
    WdgCfg.ITMode = DISABLE;
    WdgCfg.HighThreshold = 65535;
    WdgCfg.LowThreshold = 0;
    HAL_ADC_AnalogWDGConfig(&AdcHandle2, &WdgCfg);    

    /* NVIC configuration for ADC interrupt */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(ADC_IRQn, CH1_ADC_IRQ_PRIO, 0);
    HAL_NVIC_DisableIRQ(ADC_IRQn);    
    
    HAL_NVIC_SetPriority(ADC3_IRQn, CH2_ADC3_IRQ_PRIO, 0);
    HAL_NVIC_DisableIRQ(ADC3_IRQn);    
}


/*
*********************************************************************************************************
*    函 数 名: bsp_StartAdcCH1
*    功能说明: 配置CH1通道的GPIO, ADC, DMA
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartAdcCH1(void)
{
    if (g_tDSO.MeasuteMode == 0)
    {
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle1.Instance          = ADCH1;
        if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }

        AdcHandle1.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
        AdcHandle1.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
        AdcHandle1.Init.ScanConvMode             = DISABLE;                         /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
        AdcHandle1.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
        AdcHandle1.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
        AdcHandle1.Init.ContinuousConvMode       = DISABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
        AdcHandle1.Init.NbrOfConversion          = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.ExternalTrigConv         = ADC_EXTERNALTRIG_T3_TRGO;              /* Software start to trig the 1st conversion manually, without external event */
        AdcHandle1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
        AdcHandle1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
        AdcHandle1.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
        AdcHandle1.Init.OversamplingMode         = DISABLE;                         /* No oversampling */
        AdcHandle1.Init.LeftBitShift             = ADC_LEFTBITSHIFT_NONE;         /* Left shift of final results */
        /* Initialize ADC peripheral according to the passed parameters */
        if (HAL_ADC_Init(&AdcHandle1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        ADC_ConfigureBoostMode(&AdcHandle1);     /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */

        /* ### - 2 - Start calibration ############################################ */
    #if ENABLE_DIFFERENTIAL_ENDED == 0    /* 单端模式 */    
        if (HAL_ADCEx_Calibration_Start(&AdcHandle1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    #else    /* 差分模式 */
        HAL_ADC_Stop(&AdcHandle1);
        if (HAL_ADCEx_Calibration_Start(&AdcHandle1, ADC_CALIB_OFFSET, ADC_DIFFERENTIAL_ENDED) != HAL_OK)        
    #endif        
        {    
            Error_Handler(__FILE__, __LINE__);
        }


        /* ### - 3 - Channel configuration ######################################## */
        sConfig1.Channel      = ADCH1_CHANNEL;                /* Sampled channel number */
        sConfig1.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
        sConfig1.SamplingTime = H7_ADC_SAMPLETIME_1CYCLE_5;   /* Sampling time (number of clock cycles unit) */
    #if ENABLE_DIFFERENTIAL_ENDED == 0    /* 单端模式 */            
        sConfig1.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
    #else    /* 差分模式 */
        sConfig1.SingleDiff   = ADC_DIFFERENTIAL_ENDED;
    #endif
        sConfig1.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
        sConfig1.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
        sConfig1.OffsetRightShift       = DISABLE;                    /* No Right Offset Shift */
        sConfig1.OffsetSignedSaturation = DISABLE;                    /* No Signed Saturation */

        if (HAL_ADC_ConfigChannel(&AdcHandle1, &sConfig1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
        
        
        DSO_SetTriger();    /* 设置触发，打开看门狗 */

        /* ### - 4 - Start conversion in DMA mode ################################# */
        if (HAL_ADC_Start_DMA(&AdcHandle1,
                            (uint32_t *)aADCH1ConvertedData,
                            ADC_BUFFER_SIZE
                           ) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }
    else if (g_tDSO.MeasuteMode == 1)    /* 电流检测模式 */
    {
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle1.Instance          = CT_ADCH1;
        if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }

        AdcHandle1.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
        AdcHandle1.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
        AdcHandle1.Init.ScanConvMode             = DISABLE;                         /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
        AdcHandle1.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
        AdcHandle1.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
        AdcHandle1.Init.ContinuousConvMode       = DISABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
        AdcHandle1.Init.NbrOfConversion          = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.ExternalTrigConv         = ADC_EXTERNALTRIG_T3_TRGO;              /* Software start to trig the 1st conversion manually, without external event */
        AdcHandle1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
        AdcHandle1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
        AdcHandle1.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
        AdcHandle1.Init.OversamplingMode         = DISABLE;                         /* No oversampling */
        /* Initialize ADC peripheral according to the passed parameters */
        if (HAL_ADC_Init(&AdcHandle1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        ADC_ConfigureBoostMode(&AdcHandle1);     /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */
        
        /* ### - 2 - Start calibration ############################################ */
        if (HAL_ADCEx_Calibration_Start(&AdcHandle1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /* ### - 3 - Channel configuration ######################################## */
        sConfig1.Channel      = CT_ADCH1_CHANNEL;                /* Sampled channel number */
        sConfig1.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
        sConfig1.SamplingTime = H7_ADC_SAMPLETIME_1CYCLE_5;   /* Sampling time (number of clock cycles unit) */
        sConfig1.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
        sConfig1.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
        sConfig1.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
        if (HAL_ADC_ConfigChannel(&AdcHandle1, &sConfig1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
        
        DSO_SetTriger();    /* 设置触发，打开看门狗 */

        /* ### - 4 - Start conversion in DMA mode ################################# */
        if (HAL_ADC_Start_DMA(&AdcHandle1,
                            (uint32_t *)aADCH1ConvertedData,
                            ADC_BUFFER_SIZE
                           ) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }
    else if (g_tDSO.MeasuteMode == 2)    /* 低速多通道扫描模式 */
    {
        bsp_StopAdcCH1();
        DSO_CloseTriger();    /* 关闭ADC看门狗中断 */        
        
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle1.Instance          = ADC1;
        if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }

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
        AdcHandle1.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
        AdcHandle1.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
        AdcHandle1.Init.ScanConvMode             = ADC_SCAN_ENABLE;                          /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
        AdcHandle1.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
        AdcHandle1.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
        AdcHandle1.Init.ContinuousConvMode       = ENABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
        AdcHandle1.Init.NbrOfConversion          = 2;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle1.Init.ExternalTrigConv         = ADC_SOFTWARE_START;              /* Software start to trig the 1st conversion manually, without external event */
        AdcHandle1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
        AdcHandle1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
        AdcHandle1.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
        AdcHandle1.Init.OversamplingMode         = ENABLE;                         
        AdcHandle1.Init.Oversampling.Ratio                 = 1023;    /* 1024-oversampling */       
        AdcHandle1.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_10;         /* 6-bit right shift of the oversampled summation */    
        AdcHandle1.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;        /* A single trigger for all channel oversampled conversions */
        AdcHandle1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;  /* Oversampling buffer maintained during injection sequence */ 

        /* Initialize ADC peripheral according to the passed parameters */
        if (HAL_ADC_Init(&AdcHandle1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        ADC_ConfigureBoostMode(&AdcHandle1);     /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */        

        /* ### - 2 - Start calibration ############################################ */
        if (HAL_ADCEx_Calibration_Start(&AdcHandle1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /* ### - 3 - Channel configuration ######################################## */
        sConfig1.Channel      = ADC_CHANNEL_2;                /* Sampled channel number */
        sConfig1.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
        sConfig1.SamplingTime = SCAN_MODE_SAMPLETIME_ADC1;   /* Sampling time (number of clock cycles unit) */
        sConfig1.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
        sConfig1.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
        sConfig1.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
        sConfig1.OffsetRightShift       = DISABLE;           /* 禁止右移 */
        sConfig1.OffsetSignedSaturation = DISABLE;           /* 禁止有符号饱和 */        
        if (HAL_ADC_ConfigChannel(&AdcHandle1, &sConfig1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sConfig1.Channel      = ADC_CHANNEL_3;              /* Sampled channel number */
        sConfig1.Rank         = ADC_REGULAR_RANK_2;          /* Rank of sampled channel number ADCH1_CHANNEL */
        if (HAL_ADC_ConfigChannel(&AdcHandle1, &sConfig1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
        
        /* ### - 4 - Start conversion in DMA mode ################################# */
        if (HAL_ADC_Start_DMA(&AdcHandle1,
                            (uint32_t *)aADCH1ConvertedData,
                            SCAN_MODE_SAMPLE_SIZE * SCAN_MODE_ADC1_NUM
                           ) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }    
}

/*
*********************************************************************************************************
*    函 数 名: bsp_StartAdcCH2
*    功能说明: 配置CH2通道的GPIO, ADC, DMA
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartAdcCH2(void)
{
    if (g_tDSO.MeasuteMode    == 0)
    {    
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle2.Instance          = ADCH2;
        if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }

        AdcHandle2.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
        AdcHandle2.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
        AdcHandle2.Init.ScanConvMode             = DISABLE;                         /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
        AdcHandle2.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
        AdcHandle2.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
        AdcHandle2.Init.ContinuousConvMode       = DISABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
        AdcHandle2.Init.NbrOfConversion          = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.ExternalTrigConv         = ADC_EXTERNALTRIG_T3_TRGO;              /* Software start to trig the 1st conversion manually, without external event */
        AdcHandle2.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
        AdcHandle2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
        AdcHandle2.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
        AdcHandle2.Init.OversamplingMode         = DISABLE;                         /* No oversampling */
        AdcHandle1.Init.LeftBitShift             = ADC_LEFTBITSHIFT_NONE;         /* Left shift of final results */
        /* Initialize ADC peripheral according to the passed parameters */
        if (HAL_ADC_Init(&AdcHandle2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        ADC_ConfigureBoostMode(&AdcHandle2);     /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */
        
        /* ### - 2 - Start calibration ############################################ */
    #if ENABLE_DIFFERENTIAL_ENDED == 0    /* 单端模式 */            
        if (HAL_ADCEx_Calibration_Start(&AdcHandle2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
    #else    /* 差分模式 */
        HAL_ADC_Stop(&AdcHandle2);
        if (HAL_ADCEx_Calibration_Start(&AdcHandle2, ADC_CALIB_OFFSET, ADC_DIFFERENTIAL_ENDED) != HAL_OK)
    #endif
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /* ### - 3 - Channel configuration ######################################## */
        sConfig2.Channel      = ADCH2_CHANNEL;                /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
        sConfig2.SamplingTime = H7_ADC_SAMPLETIME_1CYCLE_5  ;   /* Sampling time (number of clock cycles unit) */
    #if ENABLE_DIFFERENTIAL_ENDED == 0    /* 单端模式 */            
        sConfig2.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
    #else    /* 差分模式 */
        sConfig2.SingleDiff   = ADC_DIFFERENTIAL_ENDED;
    #endif        
        sConfig2.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
        sConfig2.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
        sConfig2.OffsetRightShift       = DISABLE;                    /* No Right Offset Shift */
        sConfig2.OffsetSignedSaturation = DISABLE;                    /* No Signed Saturation */        
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
        
        DSO_SetTriger();    /* 设置触发，打开看门狗 */

        /* ### - 4 - Start conversion in DMA mode ################################# */
        if (HAL_ADC_Start_DMA(&AdcHandle2,
                            (uint32_t *)aADCH2ConvertedData,
                            ADC_BUFFER_SIZE
                           ) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }
    else if (g_tDSO.MeasuteMode == 1)    /* 电流检测模式 */
    {
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle2.Instance          = CT_ADCH2;
        if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }

        AdcHandle2.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
        AdcHandle2.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
        AdcHandle2.Init.ScanConvMode             = DISABLE;                         /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
        AdcHandle2.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
        AdcHandle2.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
        AdcHandle2.Init.ContinuousConvMode       = DISABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
        AdcHandle2.Init.NbrOfConversion          = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.ExternalTrigConv         = ADC_EXTERNALTRIG_T3_TRGO;              /* Software start to trig the 1st conversion manually, without external event */
        AdcHandle2.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
        AdcHandle2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
        AdcHandle2.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
        AdcHandle2.Init.OversamplingMode         = DISABLE;                         /* No oversampling */
        /* Initialize ADC peripheral according to the passed parameters */
        if (HAL_ADC_Init(&AdcHandle2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        ADC_ConfigureBoostMode(&AdcHandle2);     /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */
        
        /* ### - 2 - Start calibration ############################################ */
        if (HAL_ADCEx_Calibration_Start(&AdcHandle2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /* ### - 3 - Channel configuration ######################################## */
        sConfig2.Channel      = CT_ADCH2_CHANNEL;                /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
        sConfig2.SamplingTime = H7_ADC_SAMPLETIME_1CYCLE_5  ;   /* Sampling time (number of clock cycles unit) */
        sConfig2.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
        sConfig2.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
        sConfig2.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
        
        DSO_SetTriger();    /* 设置触发，打开看门狗 */

        /* ### - 4 - Start conversion in DMA mode ################################# */
        if (HAL_ADC_Start_DMA(&AdcHandle2,
                            (uint32_t *)aADCH2ConvertedData,
                            ADC_BUFFER_SIZE
                           ) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }
    else if (g_tDSO.MeasuteMode == 2)    /* 多通道扫描模式 */
    {
        bsp_StopAdcCH2();
        DSO_CloseTriger();    /* 关闭ADC看门狗中断 */
        
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle2.Instance          = ADC3;
        if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }

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
        AdcHandle2.Init.ClockPrescaler           = ADC_CLOCK_SYNC_PCLK_DIV4;        /* Synchronous clock mode, input ADC clock divided by 4*/
        AdcHandle2.Init.Resolution               = ADC_RESOLUTION_16B;              /* 16-bit resolution for converted data */
        AdcHandle2.Init.ScanConvMode             = ENABLE;                          /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
        AdcHandle2.Init.EOCSelection             = ADC_EOC_SINGLE_CONV;             /* EOC flag picked-up to indicate conversion end */
        AdcHandle2.Init.LowPowerAutoWait         = DISABLE;                         /* Auto-delayed conversion feature disabled */
        AdcHandle2.Init.ContinuousConvMode       = ENABLE;                          /* Continuous mode enabled (automatic conversion restart after each conversion) */
        AdcHandle2.Init.NbrOfConversion          = 7;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.DiscontinuousConvMode    = DISABLE;                         /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.NbrOfDiscConversion      = 1;                               /* Parameter discarded because sequencer is disabled */
        AdcHandle2.Init.ExternalTrigConv         = ADC_SOFTWARE_START;              /* Software start to trig the 1st conversion manually, without external event */
        AdcHandle2.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIGCONVEDGE_RISING;   /* Parameter discarded because software trigger chosen */
        AdcHandle2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR; /* ADC DMA circular requested */
        AdcHandle2.Init.Overrun                  = ADC_OVR_DATA_OVERWRITTEN;        /* DR register is overwritten with the last conversion result in case of overrun */
        AdcHandle2.Init.OversamplingMode         = ENABLE;                         
        AdcHandle2.Init.Oversampling.Ratio                 = 1023;    /* 1024-oversampling */       
        AdcHandle2.Init.Oversampling.RightBitShift         = ADC_RIGHTBITSHIFT_10;         /* 6-bit right shift of the oversampled summation */    
        AdcHandle2.Init.Oversampling.TriggeredMode         = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;        /* A single trigger for all channel oversampled conversions */
        AdcHandle2.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;  /* Oversampling buffer maintained during injection sequence */ 

        /* Initialize ADC peripheral according to the passed parameters */
        if (HAL_ADC_Init(&AdcHandle2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        ADC_ConfigureBoostMode(&AdcHandle2);     /* Enable Boost mode as ADC clock frequency is bigger than 20 MHz */
        
        /* ### - 2 - Start calibration ############################################ */
        if (HAL_ADCEx_Calibration_Start(&AdcHandle2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /* ### - 3 - Channel configuration ######################################## */
        sConfig2.Channel      = ADC_CHANNEL_1;                /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_1;          /* Rank of sampled channel number ADCH1_CHANNEL */
        sConfig2.SamplingTime = SCAN_MODE_SAMPLETIME_ADC3;   /* Sampling time (number of clock cycles unit) */
        sConfig2.SingleDiff   = ADC_SINGLE_ENDED;            /* Single-ended input channel */
        sConfig2.OffsetNumber = ADC_OFFSET_NONE;             /* No offset subtraction */ 
        sConfig2.Offset = 0;                                 /* Parameter discarded because offset correction is disabled */
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sConfig2.Channel      = ADC_CHANNEL_4;              /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_2;          /* Rank of sampled channel number ADCH1_CHANNEL */
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sConfig2.Channel      = ADC_CHANNEL_5;              /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_3;          /* Rank of sampled channel number ADCH1_CHANNEL */
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }    

        sConfig2.Channel      = ADC_CHANNEL_7;                /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_4;          /* Rank of sampled channel number ADCH1_CHANNEL */
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sConfig2.Channel      = ADC_CHANNEL_13;                /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_5;          /* Rank of sampled channel number ADCH1_CHANNEL */
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sConfig2.Channel      = ADC_CHANNEL_14;                /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_6;          /* Rank of sampled channel number ADCH1_CHANNEL */
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sConfig2.Channel      = ADC_CHANNEL_16;                /* Sampled channel number */
        sConfig2.Rank         = ADC_REGULAR_RANK_7;          /* Rank of sampled channel number ADCH1_CHANNEL */
        if (HAL_ADC_ConfigChannel(&AdcHandle2, &sConfig2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }        
        
        /* ### - 4 - Start conversion in DMA mode ################################# */
        if (HAL_ADC_Start_DMA(&AdcHandle2,
                            (uint32_t *)aADCH2ConvertedData,
                            SCAN_MODE_SAMPLE_SIZE * SCAN_MODE_ADC3_NUM
                           ) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }    
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_StopAdcCH1
*    功能说明: 复位CH1通道配置，停止ADC采集.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StopAdcCH1(void)
{
    if (g_tDSO.MeasuteMode    == 0)
    {        
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle1.Instance          = ADCH1;
        if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }
    }
    else if (g_tDSO.MeasuteMode == 1)
    {
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle1.Instance          = CT_ADCH1;
        if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }        
    }
    else if (g_tDSO.MeasuteMode == 2)
    {
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle1.Instance          = ADC1;
        if (HAL_ADC_DeInit(&AdcHandle1) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }        
    }    
}

/*
*********************************************************************************************************
*    函 数 名: bsp_StopAdcCH2
*    功能说明: 复位CH2通道配置，停止ADC采集.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StopAdcCH2(void)
{
    if (g_tDSO.MeasuteMode    == 0)
    {    
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle2.Instance          = ADCH2;
        if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }
    }
    else if (g_tDSO.MeasuteMode == 1)
    {
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle2.Instance          = CT_ADCH2;
        if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }
    }
    else if (g_tDSO.MeasuteMode == 2)
    {
        /* ### - 1 - Initialize ADC peripheral #################################### */
        AdcHandle2.Instance          = ADC3;
        if (HAL_ADC_DeInit(&AdcHandle2) != HAL_OK)
        {
            /* ADC de-initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: TIM3_Config
*    功能说明: 配置TIM3作为ADC触发源
*    形    参: _freq : 采样频率，单位Hz
*    返 回 值: 无
*********************************************************************************************************
*/
static void TIM3_Config(uint32_t _freq)
{
    static TIM_HandleTypeDef  htim = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim.Instance = TIM3;
    if (_freq == 0)
    {
//        __HAL_RCC_TIM3_CLK_DISABLE();
//        HAL_TIM_Base_Stop(&htim);
        __HAL_TIM_DISABLE(&htim);
    }
    else
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        
        /*##-1- Configure the TIM peripheral #######################################*/
        /* Time base configuration */

        htim.Init.Period            = (SystemCoreClock / 2) / _freq - 1;
        htim.Init.Prescaler         = 0;
        htim.Init.ClockDivision     = 0;
        htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
        htim.Init.RepetitionCounter = 0;
        HAL_TIM_Base_Init(&htim);

        /* TIM6 TRGO selection */
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

        HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);

        /*##-2- Enable TIM peripheral counter ######################################*/
        HAL_TIM_Base_Start(&htim);        
    }
}

/*
*********************************************************************************************************
*    函 数 名: DSO_StartADC
*    功能说明: 主要完成模拟量GPIO的配置、ADC的配置、定时器的配置以及DMA的配置。
*            - 两个ADC工作在独立模式
*            - 具有相同的外部触发， ADC_EXTERNALTRIG_T4_CC4
*            - TIM1的CC1频率的决定了采样频率
*
*    形    参: _uiFreq : 采样频率， Hz
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_StartADC( uint32_t _uiFreq)
{                
    TIM3_Config(0);
    DSO_StopADC();
    
    #if 1
    {
        uint32_t i;
        
        for (i = 0; i < ADC_BUFFER_SIZE; i++)
        {
            aADCH1ConvertedData[i] = 0;
            aADCH2ConvertedData[i] = 0;
        }
    }
    #endif
    
    g_tDSO.TrigFlag = 0;
    
    g_tDSO.DmaTransCplt = 0;
    
    bsp_StartAdcCH1();
    bsp_StartAdcCH2();
    
    /* 配置采样触发定时器，使用TIM1 CC1 */
    DSO_SetSampRate(_uiFreq);    /* 修改采样频率，并启动TIM */
}

/*
*********************************************************************************************************
*    函 数 名: DSO_StopADC
*    功能说明: 关闭ADC采样所有的外设。ADC, DMA, TIM
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_StopADC(void)
{    
    DSO_PauseADC();
    bsp_StopAdcCH1();
    bsp_StopAdcCH2();
}

/*
*********************************************************************************************************
*    函 数 名: PauseADC
*    功能说明: 暂停ADC采样。准备处理数据。保证下次DMA正常启动。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_PauseADC(void)
{
    TIM3_Config(0);
}

/**
* @brief  ADC MSP Init
* @param  hadc : ADC handle
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if (g_tDSO.MeasuteMode    == 0)
    {        
        if (hadc->Instance == ADCH1)
        {    
            GPIO_InitTypeDef          GPIO_InitStruct;
            static DMA_HandleTypeDef  DmaHandle1 = {0};
        
            /*##-1- Enable peripherals and GPIO Clocks #################################*/
            /* Enable GPIO clock ****************************************/
            ADCH1_P_GPIO_CLK_ENABLE();
            ADCH1_N_GPIO_CLK_ENABLE();
            /* ADC Periph clock enable */
            ADCH1_CLK_ENABLE();
            /* ADC Periph interface clock configuration */
            __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
            /* Enable DMA clock */
            CH1_DMA_CLK_ENABLE();

            /*##- 2- Configure peripheral GPIO #########################################*/
            /* ADC Channel GPIO pin configuration */
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Pin = ADCH1_P_PIN;      
            HAL_GPIO_Init(ADCH1_P_GPIO_PORT, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = ADCH1_N_PIN;      
            HAL_GPIO_Init(ADCH1_N_GPIO_PORT, &GPIO_InitStruct);      
            /*##- 3- Configure DMA #####################################################*/ 

            /*********************** Configure DMA parameters ***************************/
            DmaHandle1.Instance                 = CH1_DMA_Stream;
            DmaHandle1.Init.Request             = CH1_DMA_REQUEST_ADC;
            DmaHandle1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
            DmaHandle1.Init.PeriphInc           = DMA_PINC_DISABLE;
            DmaHandle1.Init.MemInc              = DMA_MINC_ENABLE;
            DmaHandle1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            DmaHandle1.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
//            if (g_tDSO.TrigMode == 0)
//            {
                DmaHandle1.Init.Mode                = DMA_CIRCULAR;    // DMA_CIRCULAR; DMA_NORMAL
//            }
//            else
//            {
//                DmaHandle1.Init.Mode                = DMA_NORMAL;    // DMA_CIRCULAR; DMA_NORMAL
//            }
            DmaHandle1.Init.Priority            = DMA_PRIORITY_MEDIUM;
            /* Deinitialize  & Initialize the DMA for new transfer */
            HAL_DMA_DeInit(&DmaHandle1);
            HAL_DMA_Init(&DmaHandle1);

            /* Associate the DMA handle */
            __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle1);

            /* NVIC configuration for DMA Input data interrupt */
            HAL_NVIC_SetPriority(CH1_DMA_Stream_IRQn, CH1_DMA_Stream_IRQ_PRIO, 0);
            HAL_NVIC_EnableIRQ(CH1_DMA_Stream_IRQn);  
        }
        else if (hadc->Instance == ADCH2)
        {
            GPIO_InitTypeDef          GPIO_InitStruct;
            static DMA_HandleTypeDef  DmaHandle2 = {0};

            /*##-1- Enable peripherals and GPIO Clocks #################################*/
            /* Enable GPIO clock ****************************************/
            ADCH2_P_GPIO_CLK_ENABLE();
            ADCH2_N_GPIO_CLK_ENABLE();
            /* ADC Periph clock enable */
            ADCH2_CLK_ENABLE();
            /* ADC Periph interface clock configuration */
            __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
            /* Enable DMA clock */
            CH2_DMA_CLK_ENABLE();

            /*##- 2- Configure peripheral GPIO #########################################*/
            /* ADC Channel GPIO pin configuration */

            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Pin = ADCH2_P_PIN;      
            HAL_GPIO_Init(ADCH2_P_GPIO_PORT, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = ADCH2_N_PIN;      
            HAL_GPIO_Init(ADCH2_N_GPIO_PORT, &GPIO_InitStruct);      
            /*##- 3- Configure DMA #####################################################*/ 

            /*********************** Configure DMA parameters ***************************/
            DmaHandle2.Instance                 = CH2_DMA_Stream;
            DmaHandle2.Init.Request             = CH2_DMA_REQUEST_ADC;
            DmaHandle2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
            DmaHandle2.Init.PeriphInc           = DMA_PINC_DISABLE;
            DmaHandle2.Init.MemInc              = DMA_MINC_ENABLE;
            DmaHandle2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            DmaHandle2.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
//            if (g_tDSO.TrigMode == 0)
//            {
                DmaHandle2.Init.Mode                = DMA_CIRCULAR;    // DMA_CIRCULAR; DMA_NORMAL
//            }
//            else
//            {
//                DmaHandle2.Init.Mode                = DMA_NORMAL;    // DMA_CIRCULAR; DMA_NORMAL
//            }

            DmaHandle2.Init.Priority            = DMA_PRIORITY_MEDIUM;
            /* Deinitialize  & Initialize the DMA for new transfer */
            HAL_DMA_DeInit(&DmaHandle2);
            HAL_DMA_Init(&DmaHandle2);

            /* Associate the DMA handle */
            __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle2);

            /* NVIC configuration for DMA Input data interrupt */
            HAL_NVIC_SetPriority(CH2_DMA_Stream_IRQn, CH2_DMA_Stream_IRQ_PRIO, 0);
            HAL_NVIC_EnableIRQ(CH2_DMA_Stream_IRQn);       
        }    
    }
    else if (g_tDSO.MeasuteMode == 1)    /* 高端电流测试模式 */
    {
        if (hadc->Instance == CT_ADCH1)
        {    
            GPIO_InitTypeDef          GPIO_InitStruct;
            static DMA_HandleTypeDef  DmaHandle1 = {0};
        
            /*##-1- Enable peripherals and GPIO Clocks #################################*/
            /* Enable GPIO clock ****************************************/
            CT_ADCH1_P_GPIO_CLK_ENABLE();
            CT_ADCH1_N_GPIO_CLK_ENABLE();
            /* ADC Periph clock enable */
            CT_ADCH1_CLK_ENABLE();
            /* ADC Periph interface clock configuration */
            __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
            /* Enable DMA clock */
            CT_CH1_DMA_CLK_ENABLE();

            /*##- 2- Configure peripheral GPIO #########################################*/
            /* ADC Channel GPIO pin configuration */
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Pin = CT_ADCH1_P_PIN;      
            HAL_GPIO_Init(CT_ADCH1_P_GPIO_PORT, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = CT_ADCH1_N_PIN;      
            HAL_GPIO_Init(CT_ADCH1_N_GPIO_PORT, &GPIO_InitStruct);      
            /*##- 3- Configure DMA #####################################################*/ 

            /*********************** Configure DMA parameters ***************************/
            DmaHandle1.Instance                 = CT_CH1_DMA_Stream;
            DmaHandle1.Init.Request             = CT_CH1_DMA_REQUEST_ADC;
            DmaHandle1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
            DmaHandle1.Init.PeriphInc           = DMA_PINC_DISABLE;
            DmaHandle1.Init.MemInc              = DMA_MINC_ENABLE;
            DmaHandle1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            DmaHandle1.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
//            if (g_tDSO.TrigMode == 0)
//            {
                DmaHandle1.Init.Mode                = DMA_CIRCULAR;    // DMA_CIRCULAR; DMA_NORMAL
//            }
//            else
//            {
//                DmaHandle1.Init.Mode                = DMA_NORMAL;    // DMA_CIRCULAR; DMA_NORMAL
//            }
            DmaHandle1.Init.Priority            = DMA_PRIORITY_MEDIUM;
            /* Deinitialize  & Initialize the DMA for new transfer */
            HAL_DMA_DeInit(&DmaHandle1);
            HAL_DMA_Init(&DmaHandle1);

            /* Associate the DMA handle */
            __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle1);

            /* NVIC configuration for DMA Input data interrupt */
            HAL_NVIC_SetPriority(CT_CH1_DMA_Stream_IRQn, CT_CH1_DMA_Stream_IRQ_PRIO, 0);
            HAL_NVIC_EnableIRQ(CT_CH1_DMA_Stream_IRQn);  
        }
        else if (hadc->Instance == CT_ADCH2)
        {
            GPIO_InitTypeDef          GPIO_InitStruct;
            static DMA_HandleTypeDef  DmaHandle2 = {0};

            /*##-1- Enable peripherals and GPIO Clocks #################################*/
            /* Enable GPIO clock ****************************************/
            CT_ADCH2_P_GPIO_CLK_ENABLE();
            CT_ADCH2_N_GPIO_CLK_ENABLE();
            /* ADC Periph clock enable */
            CT_ADCH2_CLK_ENABLE();
            /* ADC Periph interface clock configuration */
            __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
            /* Enable DMA clock */
            CT_CH2_DMA_CLK_ENABLE();

            /*##- 2- Configure peripheral GPIO #########################################*/
            /* ADC Channel GPIO pin configuration */

            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Pin = CT_ADCH2_P_PIN;      
            HAL_GPIO_Init(CT_ADCH2_P_GPIO_PORT, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = CT_ADCH2_N_PIN;      
            HAL_GPIO_Init(CT_ADCH2_N_GPIO_PORT, &GPIO_InitStruct);      
            /*##- 3- Configure DMA #####################################################*/ 

            /*********************** Configure DMA parameters ***************************/
            DmaHandle2.Instance                 = CT_CH2_DMA_Stream;
            DmaHandle2.Init.Request             = CT_CH2_DMA_REQUEST_ADC;
            DmaHandle2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
            DmaHandle2.Init.PeriphInc           = DMA_PINC_DISABLE;
            DmaHandle2.Init.MemInc              = DMA_MINC_ENABLE;
            DmaHandle2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            DmaHandle2.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
//            if (g_tDSO.TrigMode == 0)
//            {
                DmaHandle2.Init.Mode                = DMA_CIRCULAR;    // DMA_CIRCULAR; DMA_NORMAL
//            }
//            else
//            {
//                DmaHandle2.Init.Mode                = DMA_NORMAL;    // DMA_CIRCULAR; DMA_NORMAL
//            }

            DmaHandle2.Init.Priority            = DMA_PRIORITY_MEDIUM;
            /* Deinitialize  & Initialize the DMA for new transfer */
            HAL_DMA_DeInit(&DmaHandle2);
            HAL_DMA_Init(&DmaHandle2);

            /* Associate the DMA handle */
            __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle2);

            /* NVIC configuration for DMA Input data interrupt */
//            HAL_NVIC_SetPriority(CT_CH2_DMA_Stream_IRQn, CT_CH2_DMA_Stream_IRQ_PRIO, 0);
//            HAL_NVIC_EnableIRQ(CT_CH2_DMA_Stream_IRQn);       
        }    
    }
    else if (g_tDSO.MeasuteMode == 2)    /* 低速扫描多通道模式 - 1024过采样 */    
    {
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
        if (hadc->Instance == ADC1)
        {
            GPIO_InitTypeDef          GPIO_InitStruct;
            static DMA_HandleTypeDef  DmaHandle1 = {0};
        
            /*##-1- Enable peripherals and GPIO Clocks #################################*/
            /* Enable GPIO clock ****************************************/
            /* Enable GPIO clock ****************************************/
            __HAL_RCC_GPIOA_CLK_ENABLE();
            __HAL_RCC_GPIOC_CLK_ENABLE();
            __HAL_RCC_GPIOF_CLK_ENABLE();
            __HAL_RCC_GPIOH_CLK_ENABLE();
            /* ADC Periph clock enable */
            __HAL_RCC_ADC12_CLK_ENABLE();
            /* ADC Periph interface clock configuration */
            __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
            /* Enable DMA clock */
            CT_CH1_DMA_CLK_ENABLE();

            /*##- 2- Configure peripheral GPIO #########################################*/
            /* ADC Channel GPIO pin configuration */
            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            
            GPIO_InitStruct.Pin = GPIO_PIN_11;      
            HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = GPIO_PIN_6;      
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);      
            
            /*##- 3- Configure DMA #####################################################*/ 

            /*********************** Configure DMA parameters ***************************/
            DmaHandle1.Instance                 = CT_CH1_DMA_Stream;
            DmaHandle1.Init.Request             = CT_CH1_DMA_REQUEST_ADC;
            DmaHandle1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
            DmaHandle1.Init.PeriphInc           = DMA_PINC_DISABLE;
            DmaHandle1.Init.MemInc              = DMA_MINC_ENABLE;
            DmaHandle1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            DmaHandle1.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
            DmaHandle1.Init.Mode                = DMA_CIRCULAR;    // DMA_CIRCULAR; DMA_NORMAL
            DmaHandle1.Init.Priority            = DMA_PRIORITY_MEDIUM;
            /* Deinitialize  & Initialize the DMA for new transfer */
            HAL_DMA_DeInit(&DmaHandle1);
            HAL_DMA_Init(&DmaHandle1);

            /* Associate the DMA handle */
            __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle1);

            /* NVIC configuration for DMA Input data interrupt */
//            HAL_NVIC_SetPriority(CT_CH1_DMA_Stream_IRQn, CT_CH1_DMA_Stream_IRQ_PRIO, 0);
//            HAL_NVIC_EnableIRQ(CT_CH1_DMA_Stream_IRQn);  
        }
        else if (hadc->Instance == ADC3)
        {
            GPIO_InitTypeDef          GPIO_InitStruct;
            static DMA_HandleTypeDef  DmaHandle2 = {0};

            /*##-1- Enable peripherals and GPIO Clocks #################################*/
            /* Enable GPIO clock ****************************************/
            __HAL_RCC_GPIOC_CLK_ENABLE();
            __HAL_RCC_GPIOF_CLK_ENABLE();
            __HAL_RCC_GPIOH_CLK_ENABLE();
            /* ADC Periph clock enable */
            __HAL_RCC_ADC3_CLK_ENABLE();
            
            /* ADC Periph interface clock configuration */
            __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP);
            /* Enable DMA clock */
            CT_CH2_DMA_CLK_ENABLE();

            /*##- 2- Configure peripheral GPIO #########################################*/
            /* ADC Channel GPIO pin configuration */

            GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
            GPIO_InitStruct.Pull = GPIO_NOPULL;

            GPIO_InitStruct.Pin = GPIO_PIN_3;      
            HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_3 | GPIO_PIN_8;      
            HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);       

            GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5;      
            HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);       
               
            /*##- 3- Configure DMA #####################################################*/ 

            /*********************** Configure DMA parameters ***************************/
            DmaHandle2.Instance                 = CT_CH2_DMA_Stream;
            DmaHandle2.Init.Request             = CT_CH2_DMA_REQUEST_ADC;
            DmaHandle2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
            DmaHandle2.Init.PeriphInc           = DMA_PINC_DISABLE;
            DmaHandle2.Init.MemInc              = DMA_MINC_ENABLE;
            DmaHandle2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            DmaHandle2.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
            DmaHandle2.Init.Mode                = DMA_CIRCULAR;    // DMA_CIRCULAR; DMA_NORMAL
            DmaHandle2.Init.Priority            = DMA_PRIORITY_MEDIUM;
            /* Deinitialize  & Initialize the DMA for new transfer */
            HAL_DMA_DeInit(&DmaHandle2);
            HAL_DMA_Init(&DmaHandle2);

            /* Associate the DMA handle */
            __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle2);

            /* NVIC configuration for DMA Input data interrupt */
//            HAL_NVIC_SetPriority(CT_CH2_DMA_Stream_IRQn, CT_CH2_DMA_Stream_IRQ_PRIO, 0);
//            HAL_NVIC_EnableIRQ(CT_CH2_DMA_Stream_IRQn);  
        }        
    }
}

/**
  * @brief ADC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
    if (g_tDSO.MeasuteMode    == 0)
    {
        if (hadc->Instance == ADCH1)
        {
            /*##-1- Reset peripherals ##################################################*/
            ADCH1_FORCE_RESET();
            ADCH1_RELEASE_RESET();
            /* ADC Periph clock disable
            (automatically reset all ADC instances of the ADC common group) */
            __HAL_RCC_ADC12_CLK_DISABLE();

            /*##-2- Disable peripherals and GPIO Clocks ################################*/
            /* De-initialize the ADC Channel GPIO pin */
            HAL_GPIO_DeInit(ADCH1_P_GPIO_PORT, ADCH1_P_PIN);
            HAL_GPIO_DeInit(ADCH1_N_GPIO_PORT, ADCH1_N_PIN);
        }
        else if (hadc->Instance == ADCH2)
        {
          /*##-1- Reset peripherals ##################################################*/
    //      ADCH2_FORCE_RESET();
    //      ADCH2_RELEASE_RESET();
          /* ADC Periph clock disable
           (automatically reset all ADC instances of the ADC common group) */
    //      __HAL_RCC_ADC12_CLK_DISABLE();

          /*##-2- Disable peripherals and GPIO Clocks ################################*/
          /* De-initialize the ADC Channel GPIO pin */
            HAL_GPIO_DeInit(ADCH2_P_GPIO_PORT, ADCH2_P_PIN);
            HAL_GPIO_DeInit(ADCH2_N_GPIO_PORT, ADCH2_N_PIN);
        }
    }
    else if (g_tDSO.MeasuteMode == 1)    /* 电流测量模式 */
    {
        if (hadc->Instance == CT_ADCH1)
        {
            /*##-1- Reset peripherals ##################################################*/
            CT_ADCH1_FORCE_RESET();
            CT_ADCH1_RELEASE_RESET();
            /* ADC Periph clock disable
            (automatically reset all ADC instances of the ADC common group) */
            __HAL_RCC_ADC12_CLK_DISABLE();

            /*##-2- Disable peripherals and GPIO Clocks ################################*/
            /* De-initialize the ADC Channel GPIO pin */
            HAL_GPIO_DeInit(CT_ADCH1_P_GPIO_PORT, CT_ADCH1_P_PIN);
            HAL_GPIO_DeInit(CT_ADCH1_N_GPIO_PORT, CT_ADCH1_N_PIN);
        }
        else if (hadc->Instance == CT_ADCH2)
        {
          /*##-1- Reset peripherals ##################################################*/
    //      ADCH2_FORCE_RESET();
    //      ADCH2_RELEASE_RESET();
          /* ADC Periph clock disable
           (automatically reset all ADC instances of the ADC common group) */
    //      __HAL_RCC_ADC12_CLK_DISABLE();

          /*##-2- Disable peripherals and GPIO Clocks ################################*/
          /* De-initialize the ADC Channel GPIO pin */
            HAL_GPIO_DeInit(CT_ADCH2_P_GPIO_PORT, CT_ADCH2_P_PIN);
            HAL_GPIO_DeInit(CT_ADCH2_N_GPIO_PORT, CT_ADCH2_N_PIN);
        }        
    }
    else if (g_tDSO.MeasuteMode == 2)    /* 多通道扫描模式 */
    {
        if (hadc->Instance == ADC1)
        {
            /*##-1- Reset peripherals ##################################################*/
            __HAL_RCC_ADC12_FORCE_RESET();
            __HAL_RCC_ADC12_RELEASE_RESET();
            
            /* ADC Periph clock disable
            (automatically reset all ADC instances of the ADC common group) */
            __HAL_RCC_ADC12_CLK_DISABLE();
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
            /*##-2- Disable peripherals and GPIO Clocks ################################*/
            /* De-initialize the ADC Channel GPIO pin */
            HAL_GPIO_DeInit(GPIOF, GPIO_PIN_11);
            HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
        }
        else if (hadc->Instance == ADC3)
        {
          /*##-1- Reset peripherals ##################################################*/
            __HAL_RCC_ADC3_FORCE_RESET();
            __HAL_RCC_ADC3_RELEASE_RESET();
            
            /* ADC Periph clock disable
            (automatically reset all ADC instances of the ADC common group) */
            __HAL_RCC_ADC3_CLK_DISABLE();

          /*##-2- Disable peripherals and GPIO Clocks ################################*/
          /* De-initialize the ADC Channel GPIO pin */
            HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);
            HAL_GPIO_DeInit(GPIOF, GPIO_PIN_5);
            HAL_GPIO_DeInit(GPIOF, GPIO_PIN_3);
            HAL_GPIO_DeInit(GPIOF, GPIO_PIN_8);
            HAL_GPIO_DeInit(GPIOH, GPIO_PIN_2);
            HAL_GPIO_DeInit(GPIOH, GPIO_PIN_3);
            HAL_GPIO_DeInit(GPIOH, GPIO_PIN_5);
        }        
    }
}

/*
*********************************************************************************************************
*    函 数 名: DSO_SetSampRate
*    功能说明: 修改采样频率. 使用TIM4_CC4触发
*    形    参: freq : 采样频率 单位Hz
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_SetSampRate(uint32_t _ulFreq)
{
    TIM3_Config(_ulFreq);
}

/*
*********************************************************************************************************
*    函 数 名: CH1_DMA_STREAM_IRQHANDLER
*    功能说明: CH1 DAM中断服务程序
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void CH1_DMA_Stream_IRQHandle(void)
{
    HAL_DMA_IRQHandler(AdcHandle1.DMA_Handle);
}

/*
*********************************************************************************************************
*    函 数 名: CH1_DMA_STREAM_IRQHANDLER
*    功能说明: CH2 DAM中断服务程序,
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void CH2_DMA_Stream_IRQHandle(void)
{
    HAL_DMA_IRQHandler(AdcHandle2.DMA_Handle);
}

/*
*********************************************************************************************************
*    函 数 名: HAL_ADC_ConvHalfCpltCallback
*    功能说明: DAM中断服务程序回调函数，用于cache数据刷新到内存
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADCH1 || hadc->Instance == CT_ADCH1)
    {
        /* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
        SCB_InvalidateDCache_by_Addr((uint32_t *)aADCH1ConvertedData,  ADC_BUFFER_SIZE);
    }
    else if (hadc->Instance == ADCH2 || hadc->Instance == CT_ADCH2)
    {
        /* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
        SCB_InvalidateDCache_by_Addr((uint32_t *)aADCH2ConvertedData,  ADC_BUFFER_SIZE);
    }    
}

/*
*********************************************************************************************************
*    函 数 名: HAL_ADC_ConvCpltCallback
*    功能说明: DAM中断服务程序回调函数，用于cache数据刷新到内存
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADCH1 || hadc->Instance == CT_ADCH1)
    {
        /* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
        SCB_InvalidateDCache_by_Addr((uint32_t *)(aADCH1ConvertedData + ADC_BUFFER_SIZE),  ADC_BUFFER_SIZE);
    }
    else if (hadc->Instance == ADCH2 || hadc->Instance == CT_ADCH2)
    {
        /* Invalidate Data Cache to get the updated content of the SRAM on the first half of the ADC converted data buffer: 32 bytes */ 
        SCB_InvalidateDCache_by_Addr((uint32_t *)(aADCH2ConvertedData + ADC_BUFFER_SIZE),  ADC_BUFFER_SIZE);
    }    
    
    g_tDSO.DmaTransCplt++;
}

/*
*********************************************************************************************************
*    函 数 名: DSO_LockWave
*    功能说明: 锁存波形，等到PC读取
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_LockWave(void)
{
    uint32_t i;
    uint32_t pos;
    uint32_t TransSize;

//    ADC缓冲区已禁止CASHE      
//    SCB_InvalidateDCache_by_Addr((uint32_t *)aADCH1ConvertedData,  2 * ADC_BUFFER_SIZE);
//    SCB_InvalidateDCache_by_Addr((uint32_t *)aADCH2ConvertedData,  2 * ADC_BUFFER_SIZE);
    
    pos = g_tDSO.DmaPos;
    TransSize = TabelBufSize[g_tDSO.SampleSizeID];
    
    if (g_tDSO.MeasuteMode == 0)    /* CH1 CH2示波器电压通道 */
    {
        for (i = 0; i < TransSize; i++)
        {
            #if 0    /* 传送ADC原始数据 */
                g_Ch1WaveBuf[i] = aADCH1ConvertedData[pos];
                g_Ch2WaveBuf[i] = aADCH2ConvertedData[pos];        
            #else    /* 传送校准后的数据. V单位 */
                g_Ch1WaveBuf[i] = bsp_AdcToCH1Volt(aADCH1ConvertedData[pos]);
                g_Ch2WaveBuf[i] = bsp_AdcToCH2Volt(aADCH2ConvertedData[pos]);
            #endif
            if (++pos >= ADC_BUFFER_SIZE)
            {
                pos = 0;
            }    
        }
    }
    else if (g_tDSO.MeasuteMode == 1)    /*高侧电流检测模式 */
    {
        for (i = 0; i < TransSize; i++)
        {
            #if 0    /* 传送ADC原始数据 */
                g_Ch1WaveBuf[i] = aADCH1ConvertedData[pos];
                g_Ch2WaveBuf[i] = aADCH2ConvertedData[pos];        
            #else    /* 传送校准后的数据. mA单位 V单位  */
                g_Ch1WaveBuf[i] = bsp_AdcToHighSideCurr(aADCH1ConvertedData[pos]);
                g_Ch2WaveBuf[i] = bsp_AdcToHighSideVolt(aADCH2ConvertedData[pos]);        
            #endif
            if (++pos >= ADC_BUFFER_SIZE)
            {
                pos = 0;
            }    
        }
    }
    
//    SCB_InvalidateDCache_by_Addr((uint32_t *)g_Ch1WaveBuf,  128 * 1024);
}


/**
 * 选择排序算法
 */
static void SelectionSort(float *a, int len)
{
    int min;
    float tmp;
    
    for (int i = 0; i < len; i++)
    {
        min = i;
        
        /* 这个for循环是为了找出最小的值 */
        for (int j = i + 1; j < len; j++) 
        {
            if (a[min] > a[j])
            {
                min = j;
            }
        }
        
        /* 如果第一个取出的元素不是最小值，就进行交换 */
        if (min != i)
        {
            // 进行交换
            tmp = a[i];
            a[i] = a[min];
            a[min] = tmp;
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: AdcSumAvg
*    功能说明: 软件求和平均
*    形    参: _AdcNo : adc通道号，0 =ADC1  1=ADC3
*              _Offset : 偏移（rank）
*    返 回 值: 浮点型
*********************************************************************************************************
*/
static float AdcSumAvg(uint8_t _AdcNo, uint8_t _Offset)
{
    /* 2019-10-20 增加滤波算法 */
#if 1
    float sum;
    uint16_t i;
    float buf[SCAN_MODE_SAMPLE_SIZE];


    if (_AdcNo == 1)
    {
        for (i = 0; i < SCAN_MODE_SAMPLE_SIZE; i++)
        {
            buf[i] = aADCH1ConvertedData[i * SCAN_MODE_ADC1_NUM + _Offset];
        }
    }
    else if (_AdcNo == 2)
    {
        for (i = 0; i < SCAN_MODE_SAMPLE_SIZE; i++)
        {
            buf[i] = aADCH2ConvertedData[i * SCAN_MODE_ADC3_NUM + _Offset];
        }
    }
    
    /* 排序 */
    SelectionSort(buf, SCAN_MODE_SAMPLE_SIZE);
    
    /* 求和只要其中的1/3，两头的丢掉 */
    sum = 0;
    for (i = 0; i < SCAN_MODE_SAMPLE_SIZE / 3; i++)
    {
        sum += buf[i + SCAN_MODE_SAMPLE_SIZE / 3];
    }    
    sum = sum / (SCAN_MODE_SAMPLE_SIZE / 3);    /* 计算平均值 */
    
    return sum;
#else    /* 常规处理 */
    float sum;
    uint16_t i;
    
    sum = 0;
    if (_AdcNo == 1)
    {
        for (i = 0; i < SCAN_MODE_SAMPLE_SIZE; i++)
        {
            sum += aADCH1ConvertedData[i * SCAN_MODE_ADC1_NUM + _Offset];
        }
    }
    else if (_AdcNo == 2)
    {
        for (i = 0; i < SCAN_MODE_SAMPLE_SIZE; i++)
        {
            sum += aADCH2ConvertedData[i * SCAN_MODE_ADC3_NUM + _Offset];
        }
    }
    
    sum = sum / SCAN_MODE_SAMPLE_SIZE;    /* 计算平均值 */
    
    return sum;
#endif
}

/*
*********************************************************************************************************
*    函 数 名: bsp_GetAdcAvg
*    功能说明: 读取通道的ADC均值
*    形    参: 模拟通道号
*    返 回 值: 浮点型
*********************************************************************************************************
*/
float bsp_GetAdcAvg(uint8_t _ch)
{
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
    float ff;
    
    /* ADC1 扫描2个通道 */
    if (_ch == AN_CH1)            /* CH1电压 */
    {
        ff = AdcSumAvg(1, 0);
    }
    else if (_ch == AN_HIGH_SIDE_CURR)    /* 高端负载电流 */
    {
        ff = AdcSumAvg(1, 1);
    }    
    
    /* ADC3 扫描7个通道 */
    else if (_ch == AN_CH2)        /* CH2电压 */
    {
        ff = AdcSumAvg(2, 0);
    }    
    else if (_ch == AN_TVCC_CURR)    /* TVCC输出电流检测 */
    {
        ff = AdcSumAvg(2, 1);
    }
    else if (_ch == AN_NTC_RES)        /* NTC热敏电阻阻值检测 */
    {
        ff = AdcSumAvg(2, 2);
    }    
    else if (_ch == AN_HIGH_SIDE_VOLT)    /* 高端负载电压 */
    {
        ff = AdcSumAvg(2, 3);
    }    
    else if (_ch == AN_TVCC_VOLT)    /* TVCC电压检测    */
    {
        ff = AdcSumAvg(2, 4);
    }
    else if (_ch == AN_12V_VOLT)    /* 12V供电电压检测 */
    {
        ff = AdcSumAvg(2, 5);
    }
    else if (_ch == AN_USB_VOLT)    /* USB供电电压检测 */
    {
        ff = AdcSumAvg(2, 6);
    }    
    else
    {
        ff = 0;
    }
    
    return ff;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_AdcToHighSideCurr
*    功能说明: 根据ADC、校准参数转换为高侧电流值，
*    形    参: _adc : ADC值
*    返 回 值: 校准后的电流值，单位mA
*********************************************************************************************************
*/
float bsp_AdcToHighSideCurr(float _adc)
{
    float x1,y1,x2,y2;
    float curr;

    if (_adc <= g_tCalib.LoadCurr[g_tDSO.CurrGain].x2)
    {
        x1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x1;
        y1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y1;
        x2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x2;
        y2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y2;
    }
    else if (_adc <= g_tCalib.LoadCurr[g_tDSO.CurrGain].x3)
    {
        x1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x2;
        y1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y2;
        x2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x3;
        y2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y3;
    }        
    else 
    {
        x1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x3;
        y1 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y3;
        x2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].x4;
        y2 = g_tCalib.LoadCurr[g_tDSO.CurrGain].y4;            
    }
    curr = CaculTwoPointFloat(x1, y1, x2, y2, _adc);    
    if (curr < (float)0.5)        /* 0.5mA以下认为0 */
    {
        curr = 0;
    }    
    return curr;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_AdcToHighSideVolt
*    功能说明: 根据ADC、校准参数转换为高侧电压值
*    形    参: _adc : ADC值
*    返 回 值: 校准后的电压值，单位V
*********************************************************************************************************
*/
float bsp_AdcToHighSideVolt(float _adc)
{
    float volt;

    volt = CaculTwoPointFloat(
            g_tCalib.LoadVolt.x1, g_tCalib.LoadVolt.y1, 
            g_tCalib.LoadVolt.x2, g_tCalib.LoadVolt.y2, _adc);    
    
    if (volt < (float)0.3)        /* 0.3V以下认为0 */
    {
        volt = 0;
    }    
    return volt;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_AdcToCH1Volt
*    功能说明: 根据校准参数将CH1的ADC值转换为电压值
*    形    参: _adc : ADC值
*    返 回 值: 校准后的电压值，单位V
*********************************************************************************************************
*/
float bsp_AdcToCH1Volt(float _adc)
{
    float volt;

    volt =  CaculTwoPointFloat(
            g_tCalib.CH1[g_tDSO.Gain1].x1, g_tCalib.CH1[g_tDSO.Gain1].y1, 
            g_tCalib.CH1[g_tDSO.Gain1].x2, g_tCalib.CH1[g_tDSO.Gain1].y2, 
            _adc);        
    return volt;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_AdcToCH2Volt
*    功能说明: 根据校准参数将CH2的ADC值转换为电压值
*    形    参: _adc : ADC值
*    返 回 值: 校准后的电压值，单位V
*********************************************************************************************************
*/
float bsp_AdcToCH2Volt(float _adc)
{
    float volt;

    volt =  CaculTwoPointFloat(
            g_tCalib.CH2[g_tDSO.Gain2].x1, g_tCalib.CH2[g_tDSO.Gain2].y1, 
            g_tCalib.CH2[g_tDSO.Gain2].x2, g_tCalib.CH2[g_tDSO.Gain2].y2, 
            _adc);       
    return volt;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_AdcTask
*    功能说明: 扫描ADC任务.  插入10ms中断执行. 每次更新1个通道. 90ms刷新一次全局变量
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_AdcTask10ms(void)
{
    static uint8_t s_idx = 0;
    
    if (g_tDSO.MeasuteMode != 2)
    {
        g_tVar.CH1Volt = 0;
        g_tVar.CH2Volt = 0;
        g_tVar.HighSideVolt = 0;
        g_tVar.HighSideCurr = 0;
        g_tVar.USBPowerVolt = 0;
        g_tVar.TVCCVolt = 
        g_tVar.TVCCCurr = 0;
        g_tVar.NTCRes = 0;        
        g_tVar.NTCTemp = 0;
        return;
    }
    
    if (s_idx == 0)        /* CH1 电压 */
    {
        g_tVar.ADC_CH1Volt = bsp_GetAdcAvg(AN_CH1);
        g_tVar.CH1Volt = bsp_AdcToCH1Volt(g_tVar.ADC_CH1Volt);
    }
    else if (s_idx == 1)    /* CH2 电压 */
    {
        g_tVar.ADC_CH2Volt = bsp_GetAdcAvg(AN_CH2);
        g_tVar.CH2Volt = bsp_AdcToCH2Volt(g_tVar.ADC_CH2Volt);        
    }    
    else if (s_idx == 2)    /* 高侧电压 */
    {
        g_tVar.ADC_HighSideVolt = bsp_GetAdcAvg(AN_HIGH_SIDE_VOLT);
        g_tVar.HighSideVolt = bsp_AdcToHighSideVolt(g_tVar.ADC_HighSideVolt);
    }
    else if (s_idx == 3)    /* 高侧电流 */
    {
        g_tVar.ADC_HighSideCurr = bsp_GetAdcAvg(AN_HIGH_SIDE_CURR);;        
        g_tVar.HighSideCurr = bsp_AdcToHighSideCurr(g_tVar.ADC_HighSideCurr);
    }
    else if (s_idx == 4)
    {
        g_tVar.ADC_USBPowerVolt = bsp_GetAdcAvg(AN_USB_VOLT);
        g_tVar.USBPowerVolt = CaculTwoPointFloat(
            g_tCalib.USBVolt.x1, g_tCalib.USBVolt.y1, 
            g_tCalib.USBVolt.x2, g_tCalib.USBVolt.y2, 
            g_tVar.ADC_USBPowerVolt);            
    }
    else if (s_idx == 5)
    {
        g_tVar.ADC_ExtPowerVolt = bsp_GetAdcAvg(AN_12V_VOLT);
        g_tVar.ExtPowerVolt = CaculTwoPointFloat(
            g_tCalib.ExtPowerVolt.x1, g_tCalib.ExtPowerVolt.y1, 
            g_tCalib.ExtPowerVolt.x2, g_tCalib.ExtPowerVolt.y2, 
            g_tVar.ADC_ExtPowerVolt);            
    }
    else if (s_idx == 6)
    {
        g_tVar.ADC_TVCCVolt = bsp_GetAdcAvg(AN_TVCC_VOLT);
        g_tVar.TVCCVolt = CaculTwoPointFloat(
            g_tCalib.TVCCVolt.x1, g_tCalib.TVCCVolt.y1, 
            g_tCalib.TVCCVolt.x2, g_tCalib.TVCCVolt.y2, 
            g_tVar.ADC_TVCCVolt);        
    }    
    else if (s_idx == 7)    /* TVCC 电流 */
    {
        float adc;
        float x1,y1,x2,y2;
        
        adc = bsp_GetAdcAvg(AN_TVCC_CURR);
        g_tVar.ADC_TVCCCurr = adc;
        
        if (adc <= g_tCalib.TVCCCurr.x2)
        {
            x1 = g_tCalib.TVCCCurr.x1;
            y1 = g_tCalib.TVCCCurr.y1;
            x2 = g_tCalib.TVCCCurr.x2;
            y2 = g_tCalib.TVCCCurr.y2;
        }
        else if (adc <= g_tCalib.TVCCCurr.x3)
        {
            x1 = g_tCalib.TVCCCurr.x2;
            y1 = g_tCalib.TVCCCurr.y2;
            x2 = g_tCalib.TVCCCurr.x3;
            y2 = g_tCalib.TVCCCurr.y3;
        }        
        else 
        {
            x1 = g_tCalib.TVCCCurr.x3;
            y1 = g_tCalib.TVCCCurr.y3;
            x2 = g_tCalib.TVCCCurr.x4;
            y2 = g_tCalib.TVCCCurr.y4;            
        }
        g_tVar.TVCCCurr = CaculTwoPointFloat(x1, y1, x2, y2, adc);    
        if (g_tVar.TVCCCurr < (float)0.05)    /* 0.05mA以下认为0 */
        {
            g_tVar.TVCCCurr = 0;
        }        
    }
    else if (s_idx == 8)
    {
        g_tVar.ADC_NTCRes = bsp_GetAdcAvg(AN_NTC_RES);        
        g_tVar.NTCRes = CalculNtcRes(g_tVar.ADC_NTCRes);    /* 根据ADC计算电阻 */        
        g_tVar.NTCTemp = CalculNtcTemperFloat(g_tVar.NTCRes);    /* 根据电阻查表得到温度 */
    }    
    
    if (++s_idx > 8)
    {
        s_idx = 0;
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
