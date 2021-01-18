/*
*********************************************************************************************************
*
*    模块名称 : 中断优先级配置文件
*    文件名称 : nvic_prio_cfg.h
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _NVIC_PRIO_CFG_H
#define _NVIC_PRIO_CFG_H

/*
    HAL_NVIC_SetPriority(SysTick_IRQn, SysTick_IRQ_PRIO, 0U); 
        
    HAL_NVIC_SetPriority(OTG_HS_IRQn, OTG_HS_IRQ_PRIG, 0);
    HAL_NVIC_SetPriority(TIMx_IRQn, CDC_TIMx_IRQ_PRIO, 0);
    
    HAL_NVIC_SetPriority(USART1_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_SetPriority(TIM_HARD_IRQn, TIM_HARD_IRQ_PRIO, 0);
    
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, TFT_DMA2_STREAM3_IRQ_PRIO, 0);
    
    HAL_NVIC_SetPriority(SPI5_IRQn, TFT_SPI5_IRQ_PRIO, 0);
    
    HAL_NVIC_SetPriority(QUADSPI_IRQn, QSPI_IRQ_PRIO, 0);
    HAL_NVIC_SetPriority(MDMA_IRQn, QSPI_MDMA_IRQ_PRIO, 0);

    HAL_NVIC_SetPriority(SDMMC1_IRQn, SDMMC1_IRQ_PRIO, 0);
    
    HAL_NVIC_SetPriority(ADC_IRQn, CH1_ADC_IRQ_PRIO, 0);
    HAL_NVIC_SetPriority(ADC3_IRQn, CH2_ADC3_IRQ_PRIO, 0);
    
    HAL_NVIC_SetPriority(CH1_DMA_Stream_IRQn, CH1_DMA_Stream_IRQ_PRIO, 0);    
    HAL_NVIC_SetPriority(CH2_DMA_Stream_IRQn, CH2_DMA_Stream_IRQ_PRIO, 0);
    
    HAL_NVIC_SetPriority(CT_CH1_DMA_Stream_IRQn, CT_CH1_DMA_Stream_IRQ_PRIO, 0);
    HAL_NVIC_SetPriority(CT_CH2_DMA_Stream_IRQn, CT_CH2_DMA_Stream_IRQ_PRIO, 0);
    
*/

/*
    USB中断中，会访问SDMMC
    USB中断中，执行lua，lua则可能访问其他任意设备，因此USB中断优先级设置较低
*/

#if 1
#define SysTick_IRQ_PRIO            0
#define OTG_HS_IRQ_PRIG             15

#define CDC_TIMx_IRQ_PRIO           5

#define UART_IRQ_PRIO               5
#define TIM_HARD_IRQ_PRIO           4

#define TFT_DMA2_STREAM3_IRQ_PRIO   5
#define TFT_SPI5_IRQ_PRIO           5

#define QSPI_IRQ_PRIO               5
#define QSPI_MDMA_IRQ_PRIO          5

#define SDMMC1_IRQ_PRIO             5

#define CH1_ADC_IRQ_PRIO            5
#define CH2_ADC3_IRQ_PRIO           5

#define CH1_DMA_Stream_IRQ_PRIO     5
#define CH2_DMA_Stream_IRQ_PRIO     5

#define CT_CH1_DMA_Stream_IRQ_PRIO  5
#define CT_CH2_DMA_Stream_IRQ_PRIO  5

#else   /* V1.43 */

#define SysTick_IRQ_PRIO            15
#define OTG_HS_IRQ_PRIG             0

#define CDC_TIMx_IRQ_PRIO           6

#define UART_IRQ_PRIO               0
#define TIM_HARD_IRQ_PRIO           0

#define TFT_DMA2_STREAM3_IRQ_PRIO   1
#define TFT_SPI5_IRQ_PRIO           1

#define QSPI_IRQ_PRIO               15
#define QSPI_MDMA_IRQ_PRIO          2

#define SDMMC1_IRQ_PRIO             0

#define CH1_ADC_IRQ_PRIO            0
#define CH2_ADC3_IRQ_PRIO           0

#define CH1_DMA_Stream_IRQ_PRIO     1
#define CH2_DMA_Stream_IRQ_PRIO     1

#define CT_CH1_DMA_Stream_IRQ_PRIO  1
#define CT_CH2_DMA_Stream_IRQ_PRIO  1

/* V1.43

    HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 1, 0);
    HAL_NVIC_SetPriority(TIMx_IRQn, 6, 0);
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 3);
    HAL_NVIC_SetPriority(UART4_IRQn, 0, 4);
    HAL_NVIC_SetPriority(UART5_IRQn, 0, 5);
    HAL_NVIC_SetPriority(USART6_IRQn, 0, 6);
    HAL_NVIC_SetPriority(UART7_IRQn, 0, 6);
    HAL_NVIC_SetPriority(UART8_IRQn, 0, 6);
    HAL_NVIC_SetPriority(TIM_HARD_IRQn, 0, 2);
    HAL_NVIC_SetPriority((IRQn_Type)irq, _PreemptionPriority, _SubPriority);
    HAL_NVIC_SetPriority(TIMx_IRQn, 0, 0);
    HAL_NVIC_SetPriority(CH1_DMA_Stream_IRQn, 1, 0);

    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 1, 1);
    HAL_NVIC_SetPriority(SPI5_IRQn, 1, 0);

    HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
    HAL_NVIC_SetPriority(MDMA_IRQn, 0x02, 0);
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_SetPriority(ADC3_IRQn, 0, 0);

    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_SetPriority(ADC3_IRQn, 0, 0);
    HAL_NVIC_SetPriority(CH1_DMA_Stream_IRQn, 1, 0);
    HAL_NVIC_SetPriority(CH2_DMA_Stream_IRQn, 1, 0);
    HAL_NVIC_SetPriority(CT_CH1_DMA_Stream_IRQn, 1, 0);
    HAL_NVIC_SetPriority(CT_CH2_DMA_Stream_IRQn, 1, 0);
    HAL_NVIC_SetPriority(CT_CH1_DMA_Stream_IRQn, 1, 0);
    HAL_NVIC_SetPriority(CT_CH2_DMA_Stream_IRQn, 1, 0);
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 1);
    HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 0, 1);
    HAL_NVIC_SetPriority(FDCAN_CAL_IRQn, 0, 0);
    HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 0, 1);
    HAL_NVIC_SetPriority(FDCAN2_IT1_IRQn, 0, 1);
    HAL_NVIC_SetPriority(FDCAN_CAL_IRQn, 0, 0);
*/

#endif

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
