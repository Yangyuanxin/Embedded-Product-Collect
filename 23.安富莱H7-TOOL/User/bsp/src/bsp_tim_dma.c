/*
*********************************************************************************************************
*
*    模块名称 : TIM定时DMA操作模块
*    文件名称 : bps_tim_dma.c
*    版    本 : V1.3
*    说    明 : TIM定时触发DMA传输，FMC输出到GPIO
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-01-20  Eric    正式发布
*
*    Copyright (C), 2019-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"

/* 方便Cache类的API操作，做32字节对齐, 用的SRAM3 */
#if defined(__ICCARM__)
#pragma location = 0x38000000
uint8_t SRC_Buffer_Toggle[16] =
    {
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
};

#elif defined(__CC_ARM)
//ALIGN_32BYTES(__attribute__((section (".RAM_D3"))) uint32_t SRC_Buffer_Toggle[16]) =
const uint32_t SRC_Buffer_Toggle[16] =
    {
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
        0x00U,
        0xFFU,
};
#endif

/*
*********************************************************************************************************
*    函 数 名: TIM12_Config
*    功能说明: 配置TIM12，用于触发DMAMUX的请求发生器
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void TIM12_Config(void)
{
  TIM_HandleTypeDef htim = {0};
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfig;

  __HAL_RCC_TIM12_CLK_ENABLE();

  /*-----------------------------------------------------------------------
        bsp.c 文件中 void SystemClock_Config(void) 函数对时钟的配置如下: 

        System Clock source       = PLL (HSE)
        SYSCLK(Hz)                = 400000000 (CPU Clock)
        HCLK(Hz)                  = 200000000 (AXI and AHBs Clock)
        AHB Prescaler             = 2
        D1 APB3 Prescaler         = 2 (APB3 Clock  100MHz)
        D2 APB1 Prescaler         = 2 (APB1 Clock  100MHz)
        D2 APB2 Prescaler         = 2 (APB2 Clock  100MHz)
        D3 APB4 Prescaler         = 2 (APB4 Clock  100MHz)

        因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = APB1 x 2 = 200MHz;
        因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = APB2 x 2 = 200MHz;
        APB4上面的TIMxCLK没有分频，所以就是100MHz;

        APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14，LPTIM1
        APB2 定时器有 TIM1, TIM8 , TIM15, TIM16，TIM17

        APB4 定时器有 LPTIM2，LPTIM3，LPTIM4，LPTIM5

    TIM12CLK = 200MHz/(Period + 1) / (Prescaler + 1) = 5MHz
    函数bsp_InitTimDMA1中DMAMUX1选择的是双边沿触发，每个时钟可以触发两次。
    ----------------------------------------------------------------------- */
  htim.Instance = TIM12;

  htim.Init.Period = 200 / 1 - 1;
  htim.Init.Prescaler = 0;
  htim.Init.ClockDivision = 0;
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim);

  sConfig.OCMode = TIM_OCMODE_PWM1;
  sConfig.OCPolarity = TIM_OCPOLARITY_LOW;

  /* 选择OC1 */
  sConfig.Pulse = (htim.Init.Period + 1) / 2 - 1;
  if (HAL_TIM_OC_ConfigChannel(&htim, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_OC_Start(&htim, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler(__FILE__, __LINE__);
  }

  /* TIM12 TRGO 触发DMAMUX1的请求发生器 HAL_DMAMUX1_REQ_GEN_TIM12_TRGO */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitTimDMA1
*    功能说明: 定时触发DMA控制FMC
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitTimDMA1(void)
{
  DMA_HandleTypeDef DMA_Handle = {0};
  HAL_DMA_MuxRequestGeneratorConfigTypeDef dmamux_ReqGenParams = {0};

  /* 使能时钟 */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /*##-1- 配置DMA ##################################################*/
  /* 使用的DMA1，而DMAMUX1的请求发生器使用的通道0，即DMA_REQUEST_GENERATOR0 */
  DMA_Handle.Instance = DMA1_Stream1;
  DMA_Handle.Init.Request = DMA_REQUEST_GENERATOR0;
  DMA_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
  DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
  DMA_Handle.Init.MemInc = DMA_MINC_ENABLE;
  DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; //DMA_PDATAALIGN_BYTE;
  DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;    //DMA_PDATAALIGN_BYTE;
  DMA_Handle.Init.Mode = DMA_CIRCULAR;
  DMA_Handle.Init.Priority = DMA_PRIORITY_VERY_HIGH; //DMA_PRIORITY_LOW;
  DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  DMA_Handle.Init.MemBurst = DMA_MBURST_SINGLE;    // DMA_MBURST_SINGLE;
  DMA_Handle.Init.PeriphBurst = DMA_MBURST_SINGLE; //DMA_PBURST_SINGLE;

  /* 初始化 */
  HAL_DMA_Init(&DMA_Handle);

  /*##-2- 配置DMAMUX1 ##################################################*/
  /* 可以选择LPTIM1和TIM12触发 */
  //dmamux_ReqGenParams.SignalID  =    HAL_DMAMUX1_REQ_GEN_LPTIM1_OUT;  /* 请求触发器选择LPTIM1_OUT */
  dmamux_ReqGenParams.SignalID = HAL_DMAMUX1_REQ_GEN_TIM12_TRGO; /* 请求触发器选择TIM12_TRGO */
  dmamux_ReqGenParams.Polarity = HAL_DMAMUX_REQ_GEN_RISING;      /* TIM输出的上升沿和下降沿均可触发  */
  dmamux_ReqGenParams.RequestNumber = 3;                         /* 触发后，传输进行1次DMA传输 */

  HAL_DMAEx_ConfigMuxRequestGenerator(&DMA_Handle, &dmamux_ReqGenParams);
  HAL_DMAEx_EnableMuxRequestGenerator(&DMA_Handle);

  /*##-3- 启动DMA #####################################################*/
  HAL_DMA_Start(&DMA_Handle, (uint32_t)SRC_Buffer_Toggle, (uint32_t)0x60000000, 8);

  /* LPTIM1和TIM12都进行了初始化，方便测试 */
  //    LPTIM1_Config();
  TIM12_Config();
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
