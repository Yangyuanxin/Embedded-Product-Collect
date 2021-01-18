/*
*********************************************************************************************************
*
*    模块名称 : 随机数模块
*    文件名称 : bsp_rng.c
*    版    本 : V1.0
*    说    明 : 产生随机数
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2020-07-20 armfly  正式发布
*
*    Copyright (C), 2015-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/* RNG handler declaration */
RNG_HandleTypeDef RngHandle;

/**
  * @brief RNG MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  * @param hrng: RNG handle pointer
  * @retval None
  */
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng)
{  

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  /*Select PLL output as RNG clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RNG;
  PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLL;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);


  /* RNG Peripheral clock enable */
  __HAL_RCC_RNG_CLK_ENABLE();

}

/**
  * @brief RNG MSP De-Initialization
  *        This function freeze the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrng: RNG handle pointer
  * @retval None
  */
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng)
{
  /* Enable RNG reset state */
  __HAL_RCC_RNG_FORCE_RESET();

  /* Release RNG from reset state */
  __HAL_RCC_RNG_RELEASE_RESET();
} 

/*
*********************************************************************************************************
*    函 数 名: bsp_InitRNG
*    功能说明: 初始化RND部件
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitRNG(void)
{
    /*## Configure the RNG peripheral #######################################*/
    RngHandle.Instance = RNG;

    /* DeInitialize the RNG peripheral */
    if (HAL_RNG_DeInit(&RngHandle) != HAL_OK)
    {
        /* DeInitialization Error */
        Error_Handler(__FILE__, __LINE__);
    }    

    /* Initialize the RNG peripheral */
    if (HAL_RNG_Init(&RngHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler(__FILE__, __LINE__);
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_GenRNG
*    功能说明: 产生随机数
*    形    参: _pRng : 存储结果
*              _Len : 字节数
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_GenRNG(uint8_t *_pRng, uint32_t _Len)
{
    uint32_t rng;
    uint32_t i;
    
    
    for (i = 0; i < _Len; i++)
    {
        if ((i % 4) == 0)
        {
            HAL_RNG_GenerateRandomNumber(&RngHandle, &rng);
        }
        
        _pRng[i] = rng >> ((i % 4) * 8);
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
