/*
*********************************************************************************************************
*
*    模块名称 : DAC波形发生器
*    文件名称 : bsp_cpu_dac.c
*    版    本 : V1.0
*    说    明 : 使用STM32内部DAC输出波形。支持DAC1和DAC2输出不同的波形。
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-02-07  armfly  正式发布
*
*    Copyright (C), 2015-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"

/*
    H7-TOOL 使用PA4用作 DAC_OUT1

    DAC1使用了TIM6作为定时触发， DMA通道: DMA1_Stream5
//    DAC2使用了TIM7作为定时触发， DMA通道: DMA2_Stream6    
    
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable 开启了DAC输出缓冲，增加驱动能力,
    开了缓冲之后，靠近0V和参考电源时，失真厉害，最低50mV
    不开缓冲波形较好，到0V目测不出明显失真。
    
    功能：
    1、输出正弦波，幅度和频率可调节
    2、输出方波，幅度偏移可调节，频率可调节，占空比可以调节
    3、输出三角波，幅度可调节，频率可调节，上升沿占比可调节
    4、基本的DAC输出直流电平的函数
    
    
    硬件用PG3控制输出电压量程 0-10V、±10V
    
    硬件用 PE3/65130_SW 控制DAC电路的电源
*/
/* 控制DAC电路的电源 PE3/65130_SW */
#define DAC_POWER_CLK_ENABLE()      __HAL_RCC_GPIOE_CLK_ENABLE()
#define DAC_POWER_GPIO              GPIOE
#define DAC_POWER_PIN               GPIO_PIN_3
#define DAC_POWER_ON()              BSP_SET_GPIO_1(DAC_POWER_GPIO, DAC_POWER_PIN)   /* DAC POWER ON */
#define DAC_POWER_OFF()             BSP_SET_GPIO_0(DAC_POWER_GPIO, DAC_POWER_PIN)   /* DAC POWER OFF */
#define DAC_POWER_IS_ON()           ((DAC_POWER_GPIO->IDR & DAC_POWER_PIN) == 0)    /* 如果已使能输出，返回ture */

/* DAC 引脚定义 */
#define DACx DAC1
#define DACx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __HAL_RCC_DMA1_CLK_ENABLE()

#define DACx_CLK_ENABLE()               __HAL_RCC_DAC12_CLK_ENABLE()
#define DACx_FORCE_RESET()              __HAL_RCC_DAC12_FORCE_RESET()
#define DACx_RELEASE_RESET()            __HAL_RCC_DAC12_RELEASE_RESET()

/* Definition for DACx Channel Pin */
#define DACx_CHANNEL_PIN                GPIO_PIN_4
#define DACx_CHANNEL_GPIO_PORT          GPIOA

/* Definition for DACx's DMA_STREAM */
#define DACx_CHANNEL                    DAC_CHANNEL_1

/* Definition for DACx's DMA_STREAM */
#define DACx_DMA_INSTANCE               DMA1_Stream5

/* Definition for DACx's NVIC */
#define DACx_DMA_IRQn                   DMA1_Stream5_IRQn
#define DACx_DMA_IRQHandler             DMA1_Stream5_IRQHandler

/* DMA波形缓冲区 */
#define WAVE_SAMPLE_SIZE 128
uint16_t g_Wave1[WAVE_SAMPLE_SIZE];
//uint16_t g_Wave2[128];

DAC_WAVE_T g_tDacWave;

DAC_HandleTypeDef DacHandle;
static DAC_ChannelConfTypeDef sConfig;

static void TIM6_Config(uint32_t _freq);

/*
*********************************************************************************************************
*    函 数 名: bsp_InitDAC1
*    功能说明: 配置PA4/DAC1。 不启用DMA。调用 bsp_SetDAC1()函数修改输出DAC值
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitDAC1(void)
{
    /* 电压输出量程控制GPIO配置 */
    {
        GPIO_InitTypeDef gpio_init;

        /* DAC power 控制 */
        DAC_POWER_CLK_ENABLE();
        DAC_POWER_OFF();
        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        /* 设置推挽输出 */
        gpio_init.Pull = GPIO_NOPULL;                        /* 上下拉电阻不使能 */
        gpio_init.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度等级 */
        gpio_init.Pin = DAC_POWER_PIN;
        HAL_GPIO_Init(DAC_POWER_GPIO, &gpio_init);
    }

    /* 配置DAC, 无触发，不用DMA */
    {
        DacHandle.Instance = DACx;

        HAL_DAC_DeInit(&DacHandle);

        /*##-1- Initialize the DAC peripheral ######################################*/
        if (HAL_DAC_Init(&DacHandle) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /*##-1- DAC channel1 Configuration #########################################*/
        sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
        sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
        //sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

        if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DAC_CHANNEL_1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /*##-2- Enable DAC selected channel and associated DMA #############################*/
        if (HAL_DAC_Start(&DacHandle, DAC_CHANNEL_1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }

    g_tDacWave.Type = DAC_WAVE_NONE;
    g_tDacWave.CycleSetting = 0;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SetDAC1
*    功能说明: 设置DAC1输出数据寄存器，改变输出电压
*    形    参: _dac : DAC数据，0-4095
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetDAC1(uint16_t _dac)
{
    DAC1->DHR12R1 = _dac;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SetDAC2
*    功能说明: 设置DAC2输出数据寄存器，改变输出电压
*    形    参: _dac : DAC数据，0-4095
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetDAC2(uint16_t _dac)
{
    DAC1->DHR12R2 = _dac;
}

/*
*********************************************************************************************************
*    函 数 名: HAL_DAC_MspInit
*    功能说明: 配置DAC用到的时钟，引脚和DMA通道
*    形    参: hdac  DAC_HandleTypeDef类型结构体指针变量
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /*##-1- 使能时钟 #################################*/
    /* 使能GPIO时钟 */
    DACx_CHANNEL_GPIO_CLK_ENABLE();
    /* 使能DAC外设时钟 */
    DACx_CLK_ENABLE();

    /*##-2- 配置GPIO ##########################################*/
    /* DAC Channel1 GPIO 配置 */
    GPIO_InitStruct.Pin = DACx_CHANNEL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DACx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
}

/*
*********************************************************************************************************
*    函 数 名: HAL_DAC_MspDeInit
*    功能说明: 复位DAC
*    形    参: hdac  DAC_HandleTypeDef类型结构体指针变量
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac)
{
    /*##-1- 复位DAC外设 ##################################################*/
    DACx_FORCE_RESET();
    DACx_RELEASE_RESET();

    /*##-2- 复位DAC对应GPIO ################################*/
    HAL_GPIO_DeInit(DACx_CHANNEL_GPIO_PORT, DACx_CHANNEL_PIN);

    /*##-3- 关闭DAC用的DMA Stream ############################################*/
    HAL_DMA_DeInit(hdac->DMA_Handle1);

    /*##-4- 关闭DMA中断 ###########################################*/
    HAL_NVIC_DisableIRQ(DACx_DMA_IRQn);
}

/*
*********************************************************************************************************
*    函 数 名: HAL_TIM_Base_MspInit
*    功能说明: 初始化定时器时钟
*    形    参: htim  TIM_HandleTypeDef类型结构体指针变量
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    /* TIM6 时钟使能 */
    __HAL_RCC_TIM6_CLK_ENABLE();
}

/*
*********************************************************************************************************
*    函 数 名: HAL_TIM_Base_MspDeInit
*    功能说明: 复位定时器时钟
*    形    参: htim  TIM_HandleTypeDef类型结构体指针变量
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
    /*##-1- 复位外设 ##################################################*/
    __HAL_RCC_TIM6_FORCE_RESET();
    __HAL_RCC_TIM6_RELEASE_RESET();
}

/*
*********************************************************************************************************
*    函 数 名: TIM6_Config
*    功能说明: 配置TIM6作为DAC触发源
*    形    参: _freq : 采样频率，单位Hz
*    返 回 值: 无
*********************************************************************************************************
*/
static void TIM6_Config(uint32_t _freq)
{
    static TIM_HandleTypeDef htim;
    TIM_MasterConfigTypeDef sMasterConfig;

    /*##-1- Configure the TIM peripheral #######################################*/
    /* Time base configuration */
    htim.Instance = TIM6;

    if (_freq < 400)
    {
        htim.Init.Prescaler = 100 - 1;
        htim.Init.Period = ((SystemCoreClock / 2) / (htim.Init.Prescaler + 1)) / _freq - 1;
        htim.Init.ClockDivision = 0;
    }    
    else if (_freq < 4000)
    {
        htim.Init.Prescaler = 10 - 1;
        htim.Init.Period = ((SystemCoreClock / 2) / (htim.Init.Prescaler + 1)) / _freq - 1;
        htim.Init.ClockDivision = 0;
    }
    else
    {
        htim.Init.Period = (SystemCoreClock / 2) / _freq - 1;
        htim.Init.Prescaler = 0;
        htim.Init.ClockDivision = 0;
    }

    htim.Init.Period = (SystemCoreClock / 2) / _freq - 1;
    htim.Init.Prescaler = 0;
    htim.Init.ClockDivision = 0;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim);

    /* TIM6 TRGO selection */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

    HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);

    /*##-2- Enable TIM peripheral counter ######################################*/
    HAL_TIM_Base_Start(&htim);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_StartDAC1_DMA
*    功能说明: 配置PA4 为DAC_OUT1, 启用DMA, 开始持续输出波形
*    形    参: _BufAddr : DMA数据缓冲区地址。 必须定位在0x24000000 RAM，或flash常量
*              _Count  : 缓冲区样本个数
*             _DacFreq : DAC样本更新频率, Hz,芯片规格书最高1MHz。 实测可以到10MHz.
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartDAC1_DMA(uint32_t _BufAddr, uint32_t _Count, uint32_t _DacFreq)
{
    DAC_POWER_ON(); /* 打开DAC供电电路 */

    TIM6_Config(_DacFreq); /* DAC转换频率最高1M */

    /* 配置DAC, TIM6触发，DMA启用 */
    {
        DacHandle.Instance = DACx;

        HAL_DAC_DeInit(&DacHandle);

        /*##-1- Initialize the DAC peripheral ######################################*/
        if (HAL_DAC_Init(&DacHandle) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /* 配置DMA */
        {
            static DMA_HandleTypeDef hdma_dac1;

            /* 使能DMA1时钟 */
            DMAx_CLK_ENABLE();

            /* 配置 DACx_DMA_STREAM  */
            hdma_dac1.Instance = DACx_DMA_INSTANCE;

            hdma_dac1.Init.Request = DMA_REQUEST_DAC1;

            hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
            hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
            hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
            hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdma_dac1.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
            hdma_dac1.Init.Mode = DMA_CIRCULAR;
            hdma_dac1.Init.Priority = DMA_PRIORITY_HIGH;

            HAL_DMA_Init(&hdma_dac1);

            /* 关联DMA句柄到DAC句柄下 */
            __HAL_LINKDMA(&DacHandle, DMA_Handle1, hdma_dac1);
        }

        /*##-1- DAC channel1 Configuration #########################################*/
        sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
        //        sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;    //GND附近非线性失真厉害
        sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

        if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DAC_CHANNEL_1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /*##-2- Enable DAC selected channel and associated DMA #############################*/
        if (HAL_DAC_Start_DMA(&DacHandle, DAC_CHANNEL_1, (uint32_t *)_BufAddr, _Count, DAC_ALIGN_12B_R) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: dac1_SetSinWave
*    功能说明: DAC1输出正弦波
*    形    参: _vpp : 幅度 0-2047;
*              _freq : 频率
*    返 回 值: 无
*********************************************************************************************************
*/
void dac1_SetSinWave(uint16_t _bottom, uint16_t _top, uint32_t _freq)
{
    uint16_t i;
    uint16_t mid; /* 中值 */
    uint16_t att; /* 幅度 */

    mid = (_bottom + _top) / 2; /* 0位的值 */
    att = (_top - _bottom) / 2; /* 正弦波幅度，峰峰值除以2 */

    for (i = 0; i < WAVE_SAMPLE_SIZE; i++)
    {
        g_Wave1[i] = mid + (int32_t)(att * sin((i * 2 * 3.14159) / WAVE_SAMPLE_SIZE));
    }

    bsp_StartDAC1_DMA((uint32_t)&g_Wave1, WAVE_SAMPLE_SIZE, _freq * WAVE_SAMPLE_SIZE);
}

/*
*********************************************************************************************************
*    函 数 名: dac1_SetRectWave
*    功能说明: DAC1输出方波
*    形    参: _bottom : 低电平时DAC, 
*              _top : 高电平时DAC
*              _freq : 频率 Hz
*              _duty : 占空比 1% - 99%, 调节步数 1%
*    返 回 值: 无
*********************************************************************************************************
*/
void dac1_SetRectWave(uint16_t _bottom, uint16_t _top, uint32_t _freq, uint16_t _duty)
{
    uint16_t i;

    for (i = 0; i < (_duty * WAVE_SAMPLE_SIZE) / 100; i++)
    {
        g_Wave1[i] = _top;
    }
    for (; i < WAVE_SAMPLE_SIZE; i++)
    {
        g_Wave1[i] = _bottom;
    }

    bsp_StartDAC1_DMA((uint32_t)&g_Wave1, WAVE_SAMPLE_SIZE, _freq * WAVE_SAMPLE_SIZE);
}

/*
*********************************************************************************************************
*    函 数 名: dac1_SetTriWave
*    功能说明: DAC1输出三角波
*    形    参: _bottom : 低电平时DAC, 
*              _top : 高电平时DAC
*              _freq : 频率 Hz
*              _duty : 占空比
*    返 回 值: 无
*********************************************************************************************************
*/
void dac1_SetTriWave(uint16_t _bottom, uint16_t _top, uint32_t _freq, uint16_t _duty)
{
    uint32_t i;
    uint16_t dac;
    uint16_t m;

    /* 构造三角波数组，128个样本，从 _bottom 到 _top */
    m = (_duty * WAVE_SAMPLE_SIZE) / 100;

    if (m == 0)
    {
        m = 1;
    }

    if (m > WAVE_SAMPLE_SIZE - 1)
    {
        m = WAVE_SAMPLE_SIZE - 1;
    }
    for (i = 0; i < m; i++)
    {
        dac = _bottom + ((_top - _bottom) * i) / m;
        g_Wave1[i] = dac;
    }
    for (; i < WAVE_SAMPLE_SIZE; i++)
    {
        dac = _top - ((_top - _bottom) * (i - m)) / (WAVE_SAMPLE_SIZE - m);
        g_Wave1[i] = dac;
    }

    bsp_StartDAC1_DMA((uint32_t)&g_Wave1, WAVE_SAMPLE_SIZE, _freq * WAVE_SAMPLE_SIZE);
}

/*
*********************************************************************************************************
*    函 数 名: dac1_StopWave
*    功能说明: 停止DAC1输出
*    形    参: 无
*              _freq : 频率 0-5Hz
*    返 回 值: 无
*********************************************************************************************************
*/
void dac1_StopWave(void)
{
    //    __HAL_RCC_DAC12_FORCE_RESET();
    //    __HAL_RCC_DAC12_RELEASE_RESET();
    //
    //    HAL_DMA_DeInit(DacHandle.DMA_Handle1);

    HAL_DAC_Stop_DMA(&DacHandle, DAC_CHANNEL_1); /* 函数内部会关闭DAC */

    bsp_InitDAC1(); /* 重新启用DAC */

    bsp_SetDAC1(32767);

    DAC_POWER_OFF();
}

/*
*********************************************************************************************************
*    函 数 名: dac1_DacToVolt
*    功能说明: DAC值转换为电压 mV   (暂未用到，没有实现4点校准）
*    形    参: _dac ： DAC值，0-4095
*    返 回 值: 无
*********************************************************************************************************
*/
int16_t dac1_DacToVolt(uint16_t _dac)
{
    int32_t volt;

    /* 正负10V量程 */
    {
        volt = CaculTwoPoint(g_tCalib.Dac10V.x1, g_tCalib.Dac10V.y1,
                                                 g_tCalib.Dac10V.x2, g_tCalib.Dac10V.y2, _dac);
    }
    return volt;
}

/*
*********************************************************************************************************
*    函 数 名: dac1_VoltToDac
*    功能说明: 电压 mV转化为DAC值
*    形    参: _volt ： mv电压值
*    返 回 值: 无
*********************************************************************************************************
*/
int16_t dac1_VoltToDac(int16_t _volt)
{
    int32_t dac;
    int32_t x1, y1, x2, y2;

    if (_volt <= g_tCalib.Dac10V.y2)
    {
        x1 = g_tCalib.Dac10V.y1;
        y1 = g_tCalib.Dac10V.x1;
        x2 = g_tCalib.Dac10V.y2;
        y2 = g_tCalib.Dac10V.x2;
    }
    else if (_volt <= g_tCalib.Dac10V.y3)
    {
        x1 = g_tCalib.Dac10V.y2;
        y1 = g_tCalib.Dac10V.x2;
        x2 = g_tCalib.Dac10V.y3;
        y2 = g_tCalib.Dac10V.x3;
    }
    else
    {
        x1 = g_tCalib.Dac10V.y3;
        y1 = g_tCalib.Dac10V.x3;
        x2 = g_tCalib.Dac10V.y4;
        y2 = g_tCalib.Dac10V.x4;
    }

    /* 正负10V量程 */
    dac = CaculTwoPoint(x1, y1, x2, y2, _volt);
    if (dac < 0)
    {
        dac = 0;
    }
    else if (dac > 4095)
    {
        dac = 4095;
    }
    return dac;
}

/*
*********************************************************************************************************
*    函 数 名: dac1_DacToCurr
*    功能说明: DAC值转换为电流 uA   (暂未用到，没有实现4点校准）
*    形    参: _dac ： DAC值，0-4095
*    返 回 值: 无
*********************************************************************************************************
*/
int16_t dac1_DacToCurr(uint16_t _dac)
{
    int32_t curr;

    curr = CaculTwoPoint(g_tCalib.Dac20mA.x1, g_tCalib.Dac20mA.y1,
                                             g_tCalib.Dac20mA.x2, g_tCalib.Dac20mA.y2, _dac);
    return curr;
}

/*
*********************************************************************************************************
*    函 数 名: dac1_CurrToDac
*    功能说明: 电流uA转化为DAC值
*    形    参: _curr ： uA 电流值
*    返 回 值: 无
*********************************************************************************************************
*/
int16_t dac1_CurrToDac(int16_t _curr)
{
    int32_t dac;
    int32_t x1, y1, x2, y2;

    if (_curr <= g_tCalib.Dac20mA.y2)
    {
        x1 = g_tCalib.Dac20mA.y1;
        y1 = g_tCalib.Dac20mA.x1;
        x2 = g_tCalib.Dac20mA.y2;
        y2 = g_tCalib.Dac20mA.x2;
    }
    else if (_curr <= g_tCalib.Dac20mA.y3)
    {
        x1 = g_tCalib.Dac20mA.y2;
        y1 = g_tCalib.Dac20mA.x2;
        x2 = g_tCalib.Dac20mA.y3;
        y2 = g_tCalib.Dac20mA.x3;
    }
    else
    {
        x1 = g_tCalib.Dac20mA.y3;
        y1 = g_tCalib.Dac20mA.x3;
        x2 = g_tCalib.Dac20mA.y4;
        y2 = g_tCalib.Dac20mA.x4;
    }

    /* 正负10V量程 */
    dac = CaculTwoPoint(x1, y1, x2, y2, _curr);
    if (dac < 0)
    {
        dac = 0;
    }
    else if (dac > 4095)
    {
        dac = 4095;
    }
    return dac;
}

/*
*********************************************************************************************************
*    函 数 名: dac1_StartDacWave
*    功能说明: 启动波形
*    形    参: 无。  g_tDacWave 全局结构中存放有波形参数
*    返 回 值: 无
*********************************************************************************************************
*/
void dac1_StartDacWave(void)
{
    uint16_t bottom;
    uint16_t top;

    g_tDacWave.CycleCount = 0;

    switch (g_tDacWave.Type)
    {
        case DAC_WAVE_NO:
            dac1_StopWave();
            DAC_POWER_ON(); /* 打开DAC供电电路 */
            break;

        case DAC_WAVE_SIN:
            bottom = dac1_VoltToDac(g_tDacWave.VoltMin);
            top = dac1_VoltToDac(g_tDacWave.VoltMax);
            dac1_SetSinWave(bottom, top, g_tDacWave.Freq);
            break;

        case DAC_WAVE_SQUARE:
            bottom = dac1_VoltToDac(g_tDacWave.VoltMin);
            top = dac1_VoltToDac(g_tDacWave.VoltMax);
            dac1_SetRectWave(bottom, top, g_tDacWave.Freq, g_tDacWave.Duty);
            break;

        case DAC_WAVE_TRI:
            bottom = dac1_VoltToDac(g_tDacWave.VoltMin);
            top = dac1_VoltToDac(g_tDacWave.VoltMax);
            dac1_SetTriWave(bottom, top, g_tDacWave.Freq, g_tDacWave.Duty);
            break;
    }
}

/* DMA传输完成时回调 */
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac)
{
    if (hdac == &DacHandle)
    {
        if (g_tDacWave.CycleSetting == 0) /* 一直循环 */
        {
            ;
        }
        else
        {
            ++g_tDacWave.CycleCount;
            if (g_tDacWave.CycleCount >= g_tDacWave.CycleSetting)
            {
                HAL_DAC_Stop_DMA(&DacHandle, DAC_CHANNEL_1);
            }
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
