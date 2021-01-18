/*
*********************************************************************************************************
*
*    模块名称 : TIM捕获模块
*    文件名称 : bsp_tim_capture.c
*    版    本 : V1.0
*    说    明 : 利用TIM5 CH1-CH4通道定时捕获功能，DMA脉冲时刻到内存。
*
*    修改记录 :
*        版本号  日期         作者     说明
*        V1.0    2019-01-29   armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/* 
    PH10/TIM5_CH1    - FMC_D1
    PH11/TIM5_CH2    - FMC_D3
    PH12/TIM5_CH3    - FMC_D4 
    PI0/TIM5_CH4     - FMC_D0    
    
    TIM5自由计数，32bit。每隔 21.474181125秒归零
    计数器频率 = 200MHz
*/

/* 定义GPIO端口 */
#define TIMx_CH1_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define TIMx_CH1_GPIO           GPIOH
#define TIMx_CH1_PIN            GPIO_PIN_10

#define TIMx_CH2_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define TIMx_CH2_GPIO           GPIOH
#define TIMx_CH2_PIN            GPIO_PIN_11

#define TIMx_CH3_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define TIMx_CH3_GPIO           GPIOH
#define TIMx_CH3_PIN            GPIO_PIN_12

#define TIMx_CH4_CLK_ENABLE()   __HAL_RCC_GPIOI_CLK_ENABLE()
#define TIMx_CH4_GPIO           GPIOI
#define TIMx_CH4_PIN            GPIO_PIN_0


/* PB8/TIM4_CH3 捕获脉宽 */
#define TIMx                    TIM5
#define TIMx_CHANNEL            TIM_CHANNEL_3
#define TIMx_ACTIVE_CHANNEL     HAL_TIM_ACTIVE_CHANNEL_3
#define TIMx_IRQHandler         TIM5_IRQHandler

/* Definition for TIMx's NVIC */
#define TIMx_IRQn               TIM5_IRQn

#define TIMx_CLK_ENABLE()       __HAL_RCC_TIM5_CLK_ENABLE()
#define TIMx_GPIO_AF            GPIO_AF2_TIM5

/* DAM定义 */
#define CH1_DMA_CLK_ENABLE()            __HAL_RCC_DMA1_CLK_ENABLE()
#define CH1_DMA_Stream                  DMA1_Stream2
#define CH1_DMA_Stream_IRQn             DMA1_Stream2_IRQn
#define CH1_DMA_Stream_IRQHandle        DMA1_Stream2_IRQHandler
#define CH1_DMA_REQUEST                 DMA_REQUEST_TIM5_CH1

#define CH2_DMA_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()
#define CH2_DMA_Stream                  DMA2_Stream2
#define CH2_DMA_Stream_IRQn             DMA2_Stream2_IRQn
#define CH2_DMA_Stream_IRQHandle        DMA2_Stream2_IRQHandler
#define CH2_DMA_REQUEST                 DMA_REQUEST_TIM5_CH2

#define CH3_DMA_CLK_ENABLE()            __HAL_RCC_DMA1_CLK_ENABLE()
#define CH3_DMA_Stream                  DMA1_Stream3
#define CH3_DMA_Stream_IRQn             DMA1_Stream3_IRQn
#define CH3_DMA_Stream_IRQHandle        DMA1_Stream3_IRQHandler
#define CH3_DMA_REQUEST                 DMA_REQUEST_TIM5_CH3

#define CH4_DMA_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()
#define CH4_DMA_Stream                  DMA2_Stream3
#define CH4_DMA_Stream_IRQn             DMA2_Stream3_IRQn
#define CH4_DMA_Stream_IRQHandle        DMA2_Stream3_IRQHandler
#define CH4_DMA_REQUEST                 DMA_REQUEST_TIM5_CH4


static TIM_HandleTypeDef s_TimHandle;

#define TIM_BUFF_SIZE   128
ALIGN_32BYTES(uint32_t s_ch0[TIM_BUFF_SIZE]);
ALIGN_32BYTES(uint32_t s_ch1[TIM_BUFF_SIZE]);
ALIGN_32BYTES(uint32_t s_ch2[TIM_BUFF_SIZE]);
ALIGN_32BYTES(uint32_t s_ch3[TIM_BUFF_SIZE]);

PUSLE_T g_tPulse;

/*
*********************************************************************************************************
*    函 数 名: bsp_InitTIMCapture
*    功能说明: 配置TIM作为输入捕获
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_ConfigTIMCapture(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.Mode = GPIO_MODE_AF_PP;                         /* 设置AF模式 */
    gpio_init.Pull = GPIO_NOPULL;                                 /* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH; /* GPIO速度等级 */

    gpio_init.Pin = TIMx_CH1_PIN;
    HAL_GPIO_Init(TIMx_CH1_GPIO, &gpio_init);

    gpio_init.Pin = TIMx_CH2_PIN;
    HAL_GPIO_Init(TIMx_CH2_GPIO, &gpio_init);

    gpio_init.Pin = TIMx_CH3_PIN;
    HAL_GPIO_Init(TIMx_CH3_GPIO, &gpio_init);

    gpio_init.Pin = TIMx_CH4_PIN;
    HAL_GPIO_Init(TIMx_CH4_GPIO, &gpio_init);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_StartDetectPulse
*    功能说明: 开始检测脉冲频率、占空比、脉冲个数。 使用TIM5_CH1，
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartDetectPulse(void)
{
    {
        g_tPulse.C1 = 0;
        g_tPulse.C2 = 0;
        g_tPulse.Count = 0;
        g_tPulse.Duty = 0;
        g_tPulse.Freq = 0;
    }
    
    EIO_D1_Config(ES_GPIO_IN);
    
    /* 配置GPIO */
    {
        GPIO_InitTypeDef gpio_init;

        /* GPIO时钟使能 */
        TIMx_CH1_CLK_ENABLE();

        gpio_init.Mode = GPIO_MODE_AF_OD;                       /* 设置开漏输出 */
        gpio_init.Pull = GPIO_NOPULL;                           /* 上下拉电阻不使能 */
        gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;            /* GPIO速度等级 */
        gpio_init.Alternate = TIMx_GPIO_AF;

        gpio_init.Pin = TIMx_CH1_PIN;
        HAL_GPIO_Init(TIMx_CH1_GPIO, &gpio_init);
    }

    /* 配置TIM */
    {
        TIM_IC_InitTypeDef sICConfig;

        TIMx_CLK_ENABLE();
        
        /* Set TIMx instance */
        s_TimHandle.Instance = TIMx;

        /* 设置分频为 1680/2， 捕获计数器值的单位正好是 10us, 方便脉宽比较 
            SystemCoreClock 是主频. 常用值: 168000000, 180000000,192000000
        */
        s_TimHandle.Init.Period = 0xFFFFFFFF;   /* TIM5是32位 */
        s_TimHandle.Init.Prescaler = 0;
        s_TimHandle.Init.ClockDivision = 0;
        s_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        s_TimHandle.Init.RepetitionCounter = 0;
        if (HAL_TIM_IC_Init(&s_TimHandle) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /*##-2- Configure the Input Capture channel ################################*/
        /* Configure the Input Capture of channel 2 */
        sICConfig.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
        sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
        sICConfig.ICFilter = 0;
        if (HAL_TIM_IC_ConfigChannel(&s_TimHandle, &sICConfig, TIM_CHANNEL_1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /* Non-Blocking mode: DMA */
        //        HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
        //        HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);

        //        /*##-3- Start the Input Capture in interrupt mode ##########################*/
        //        if (HAL_TIM_IC_Start_IT(&s_TimHandle, TIMx_CHANNEL) != HAL_OK)
        //        {
        //            Error_Handler(__FILE__, __LINE__);
        //        }
        //HAL_TIM_IC_Start_DMA(&s_TimHandle, TIM_CHANNEL_1, uint32_t *pData, uint16_t Length);
        HAL_TIM_IC_Start_IT(&s_TimHandle, TIM_CHANNEL_1);
    }

    {
        
        HAL_NVIC_SetPriority(TIMx_IRQn, 0, 0);

        /* Enable the TIMx global Interrupt */
        HAL_NVIC_EnableIRQ(TIMx_IRQn);
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_StopDetectPulse
*    功能说明: 停止检测脉冲频率、占空比、脉冲个数。 使用TIM5_CH1，
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StopDetectPulse(void)
{
    HAL_NVIC_DisableIRQ(TIMx_IRQn);
    __HAL_TIM_DISABLE(&s_TimHandle);  
}

/*
*********************************************************************************************************
*    函 数 名: bsp_GetPulseParam
*    功能说明: 计算脉冲测量数据。
*    形    参: 无
*    返 回 值: 无。结果在 g_tPulse全局结构变量
*********************************************************************************************************
*/
void bsp_GetPulseParam(void)
{
    uint32_t c0, c1, c2;
    uint32_t period;
    
    DISABLE_INT(); 
    c0 = g_tPulse.C0;
    c1 = g_tPulse.C1;
    c2 = g_tPulse.C2;
    ENABLE_INT();  
    
    if (c2 == c0)
    {
        g_tPulse.Freq = 0;
        g_tPulse.Duty = 0;
    }
    else
    {
        if (c2 > c0)
        {
            period = c2 - c0;
        }
        else
        {
            period = (0xFFFFFFFF - c0) + c2 + 1;
        }
           
        g_tPulse.Freq = (float)(SystemCoreClock / 2) / period;
        g_tPulse.Duty = 100 * (float)(c1 - c0) / period;
    }
}

HAL_StatusTypeDef my_HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{  
  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = HAL_TIM_DMACaptureCplt;
     
      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = HAL_TIM_DMAError ;
      
      /* Enable t  he DMA Stream */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)&htim->Instance->CCR1, (uint32_t)pData, Length); 
      
      /* Enable the TIM Capture/Compare 1 DMA request */      
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
    }
    break;
    
    case TIM_CHANNEL_2:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = HAL_TIM_DMACaptureCplt;
     
      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferErrorCallback = HAL_TIM_DMAError ;
      
      /* Enable t  he DMA Stream */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)&htim->Instance->CCR2, (uint32_t)pData, Length);
      
      /* Enable the TIM Capture/Compare 2  DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
    }
    break;
    
    case TIM_CHANNEL_3:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = HAL_TIM_DMACaptureCplt;
     
      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferErrorCallback = HAL_TIM_DMAError ;
      
      /* Enable t  he DMA Stream */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)&htim->Instance->CCR3, (uint32_t)pData, Length);
      
      /* Enable the TIM Capture/Compare 3  DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
    }
    break;
    
    case TIM_CHANNEL_4:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferCpltCallback = HAL_TIM_DMACaptureCplt;
     
      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferErrorCallback = HAL_TIM_DMAError ;
      
      /* Enable t  he DMA Stream */
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC4], (uint32_t)&htim->Instance->CCR4, (uint32_t)pData, Length);
      
      /* Enable the TIM Capture/Compare 4  DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC4);
    }
    break;
    
    default:
    break;
  }

  /* Enable the Input Capture channel */
  TIM_CCxChannelCmd(htim->Instance, Channel, TIM_CCx_ENABLE);
   
  /* Enable the Peripheral 移动到外面 */
  // __HAL_TIM_ENABLE(htim); 
  
  /* Return function status */
  return HAL_OK;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_StartLogicCH4
*    功能说明: 启动4通道逻辑信号采样。TIM5具有200M时钟，采用捕获DMA传送信号边沿到内存.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartLogicCH4(void)
{    
    EIO_D0_Config(ES_GPIO_IN);
    EIO_D1_Config(ES_GPIO_IN);
    EIO_D3_Config(ES_GPIO_IN);
    EIO_D4_Config(ES_GPIO_IN);
    
    /* 配置GPIO */
    {
        GPIO_InitTypeDef gpio_init;

        /* GPIO时钟使能 */
        TIMx_CH1_CLK_ENABLE();
        TIMx_CH2_CLK_ENABLE();
        TIMx_CH3_CLK_ENABLE();
        TIMx_CH4_CLK_ENABLE();

        gpio_init.Mode = GPIO_MODE_AF_OD;                       /* 设置开漏输出 */
        gpio_init.Pull = GPIO_NOPULL;                           /* 上下拉电阻不使能 */
        gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;            /* GPIO速度等级 */
        gpio_init.Alternate = TIMx_GPIO_AF;

        gpio_init.Pin = TIMx_CH1_PIN;
        HAL_GPIO_Init(TIMx_CH1_GPIO, &gpio_init);

        gpio_init.Pin = TIMx_CH2_PIN;
        HAL_GPIO_Init(TIMx_CH2_GPIO, &gpio_init);

        gpio_init.Pin = TIMx_CH3_PIN;
        HAL_GPIO_Init(TIMx_CH3_GPIO, &gpio_init);

        gpio_init.Pin = TIMx_CH4_PIN;
        HAL_GPIO_Init(TIMx_CH4_GPIO, &gpio_init);
    }
    
    /* 配置DMA, 4个 */
    {
        static DMA_HandleTypeDef  DmaHandle1 = {0};
        static DMA_HandleTypeDef  DmaHandle2 = {0};
        static DMA_HandleTypeDef  DmaHandle3 = {0};
        static DMA_HandleTypeDef  DmaHandle4 = {0};
        
        /* Enable DMA clock */
        CH1_DMA_CLK_ENABLE();
        CH2_DMA_CLK_ENABLE();
        CH3_DMA_CLK_ENABLE();
        CH4_DMA_CLK_ENABLE();
       
        DmaHandle1.Instance                 = CH1_DMA_Stream;
        DmaHandle1.Init.Request             = CH1_DMA_REQUEST;
        DmaHandle1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        DmaHandle1.Init.PeriphInc           = DMA_PINC_DISABLE;
        DmaHandle1.Init.MemInc              = DMA_MINC_ENABLE;
        DmaHandle1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        DmaHandle1.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
        DmaHandle1.Init.Mode                = DMA_NORMAL;    // DMA_CIRCULAR; DMA_NORMAL
        DmaHandle1.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
        
        DmaHandle2.Instance                 = CH2_DMA_Stream;
        DmaHandle2.Init.Request             = CH2_DMA_REQUEST;
        DmaHandle2.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        DmaHandle2.Init.PeriphInc           = DMA_PINC_DISABLE;
        DmaHandle2.Init.MemInc              = DMA_MINC_ENABLE;
        DmaHandle2.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        DmaHandle2.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
        DmaHandle2.Init.Mode                = DMA_NORMAL;    // DMA_CIRCULAR; DMA_NORMAL
        DmaHandle2.Init.Priority            = DMA_PRIORITY_VERY_HIGH;

        DmaHandle3.Instance                 = CH3_DMA_Stream;
        DmaHandle3.Init.Request             = CH3_DMA_REQUEST;
        DmaHandle3.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        DmaHandle3.Init.PeriphInc           = DMA_PINC_DISABLE;
        DmaHandle3.Init.MemInc              = DMA_MINC_ENABLE;
        DmaHandle3.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        DmaHandle3.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
        DmaHandle3.Init.Mode                = DMA_NORMAL;    // DMA_CIRCULAR; DMA_NORMAL
        DmaHandle3.Init.Priority            = DMA_PRIORITY_MEDIUM;
        
        DmaHandle4.Instance                 = CH4_DMA_Stream;
        DmaHandle4.Init.Request             = CH4_DMA_REQUEST;
        DmaHandle4.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        DmaHandle4.Init.PeriphInc           = DMA_PINC_DISABLE;
        DmaHandle4.Init.MemInc              = DMA_MINC_ENABLE;
        DmaHandle4.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        DmaHandle4.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
        DmaHandle4.Init.Mode                = DMA_NORMAL;    // DMA_CIRCULAR; DMA_NORMAL
        DmaHandle4.Init.Priority            = DMA_PRIORITY_MEDIUM;        
        
        /* Deinitialize  & Initialize the DMA for new transfer */
        HAL_DMA_DeInit(&DmaHandle1);
        HAL_DMA_DeInit(&DmaHandle2);
        HAL_DMA_DeInit(&DmaHandle3);
        HAL_DMA_DeInit(&DmaHandle4);

        HAL_DMA_Init(&DmaHandle1);
        HAL_DMA_Init(&DmaHandle2);        
        HAL_DMA_Init(&DmaHandle3);
        HAL_DMA_Init(&DmaHandle4);

        /* Associate the DMA handle */
        __HAL_LINKDMA(&s_TimHandle, hdma[TIM_DMA_ID_CC1], DmaHandle1);     
        __HAL_LINKDMA(&s_TimHandle, hdma[TIM_DMA_ID_CC2], DmaHandle2);
        __HAL_LINKDMA(&s_TimHandle, hdma[TIM_DMA_ID_CC3], DmaHandle3);
        __HAL_LINKDMA(&s_TimHandle, hdma[TIM_DMA_ID_CC4], DmaHandle4);
                          
        /* NVIC configuration for DMA Input data interrupt */
        HAL_NVIC_SetPriority(CH1_DMA_Stream_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(CH1_DMA_Stream_IRQn);          
    }

    /* 配置TIM */
    {
        TIM_IC_InitTypeDef sICConfig;

        TIMx_CLK_ENABLE();
        
        /* Set TIMx instance */
        s_TimHandle.Instance = TIMx;

        /* 设置分频为 1680/2， 捕获计数器值的单位正好是 10us, 方便脉宽比较 
            SystemCoreClock 是主频. 常用值: 168000000, 180000000,192000000
        */
        s_TimHandle.Init.Period = 0xFFFFFFFF;   /* TIM5是32位 */
        s_TimHandle.Init.Prescaler = 0;
        s_TimHandle.Init.ClockDivision = 0;
        s_TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
        s_TimHandle.Init.RepetitionCounter = 0;
        if (HAL_TIM_IC_Init(&s_TimHandle) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        /*##-2- Configure the Input Capture channel ################################*/
        sICConfig.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
        sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
        sICConfig.ICFilter = 0;
        if (HAL_TIM_IC_ConfigChannel(&s_TimHandle, &sICConfig, TIM_CHANNEL_1) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sICConfig.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
        sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
        sICConfig.ICFilter = 0;
        if (HAL_TIM_IC_ConfigChannel(&s_TimHandle, &sICConfig, TIM_CHANNEL_2) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sICConfig.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
        sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
        sICConfig.ICFilter = 0;
        if (HAL_TIM_IC_ConfigChannel(&s_TimHandle, &sICConfig, TIM_CHANNEL_3) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }

        sICConfig.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
        sICConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
        sICConfig.ICPrescaler = TIM_ICPSC_DIV1;
        sICConfig.ICFilter = 0;
        if (HAL_TIM_IC_ConfigChannel(&s_TimHandle, &sICConfig, TIM_CHANNEL_4) != HAL_OK)
        {
            Error_Handler(__FILE__, __LINE__);
        }        

        __HAL_TIM_DISABLE(&s_TimHandle); 
        
        my_HAL_TIM_IC_Start_DMA(&s_TimHandle, TIM_CHANNEL_1, s_ch0, TIM_BUFF_SIZE);        
        my_HAL_TIM_IC_Start_DMA(&s_TimHandle, TIM_CHANNEL_2, s_ch1, TIM_BUFF_SIZE);
        my_HAL_TIM_IC_Start_DMA(&s_TimHandle, TIM_CHANNEL_3, s_ch2, TIM_BUFF_SIZE);
        my_HAL_TIM_IC_Start_DMA(&s_TimHandle, TIM_CHANNEL_4, s_ch3, TIM_BUFF_SIZE);  

        __HAL_TIM_ENABLE(&s_TimHandle);       
    }

//    {
//        
//        HAL_NVIC_SetPriority(TIMx_IRQn, 0, 0);

//        /* Enable the TIMx global Interrupt */
//        HAL_NVIC_EnableIRQ(TIMx_IRQn);
//    }
}



/*
*********************************************************************************************************
*    函 数 名: bsp_StopLogicCH4
*    功能说明: 停止4通道逻辑信号采样。TIM5具有200M时钟，采用捕获DMA传送信号边沿到内存.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_StopLogicCH4(void)
{
    SCB_InvalidateDCache_by_Addr((uint32_t *)s_ch0,  100);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        SCB_InvalidateDCache_by_Addr((uint32_t *)s_ch0,  TIM_BUFF_SIZE);
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        SCB_InvalidateDCache_by_Addr((uint32_t *)s_ch1,  TIM_BUFF_SIZE);
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
    {
        SCB_InvalidateDCache_by_Addr((uint32_t *)s_ch2,  TIM_BUFF_SIZE);
    }
    else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
    {
        SCB_InvalidateDCache_by_Addr((uint32_t *)s_ch3,  TIM_BUFF_SIZE);
    }    
}
  
/*
*********************************************************************************************************
*    函 数 名: TIMx_IRQHandler
*    功能说明: TIM中断服务程序
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void TIMx_IRQHandler(void)
{
    static uint32_t s_c0 = 0;
    static uint32_t s_c1 = 0;
    
    if (TIMx->SR & TIM_IT_CC1)
    {
        TIMx->SR = (uint16_t)~TIM_IT_CC1;
        
        if (TIMx_CH1_GPIO->IDR & TIMx_CH1_PIN)  /* 高电平时刻 */
        {            
            g_tPulse.Count++;          

            g_tPulse.C0 = s_c0;
            g_tPulse.C1 = s_c1;
            g_tPulse.C2 = TIMx->CCR1;           

            s_c0 = g_tPulse.C2;
        }
        else    /* 低电平时刻 */
        {
            s_c1 = TIMx->CCR1;
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
