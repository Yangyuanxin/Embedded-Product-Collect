/*
*********************************************************************************************************
*
*    模块名称 : FDCAN驱动模块
*    文件名称 : bsp_can.c
*    版    本 : V1.0
*    说    明 : CAN驱动. 
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2018-11-14  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
    启用CAN1，需要将V7主板上的J12、J13两个跳线帽都插到1-2脚。（缺省是不插）
    启用CNA2，硬件无需跳线，以太网功能需要屏蔽（有引脚复用）
*/

/* FDCAN1 GPIO定义 */
#define FDCAN1_TX_PIN GPIO_PIN_12
#define FDCAN1_TX_GPIO_PORT GPIOA
#define FDCAN1_TX_AF GPIO_AF9_FDCAN1
#define FDCAN1_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

#define FDCAN1_RX_PIN GPIO_PIN_11
#define FDCAN1_RX_GPIO_PORT GPIOA
#define FDCAN1_RX_AF GPIO_AF9_FDCAN1
#define FDCAN1_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()

FDCAN_HandleTypeDef hfdcan1;
FDCAN_FilterTypeDef sFilterConfig1;

/* FDCAN1 GPIO定义 */
#define FDCAN2_TX_PIN GPIO_PIN_13
#define FDCAN2_TX_GPIO_PORT GPIOB
#define FDCAN2_TX_AF GPIO_AF9_FDCAN2
#define FDCAN2_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define FDCAN2_RX_PIN GPIO_PIN_12
#define FDCAN2_RX_GPIO_PORT GPIOB
#define FDCAN2_RX_AF GPIO_AF9_FDCAN2
#define FDCAN2_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

FDCAN_HandleTypeDef hfdcan2;
FDCAN_FilterTypeDef sFilterConfig2;

FDCAN_RxHeaderTypeDef g_Can1RxHeader;
uint8_t g_Can1RxData[8];

FDCAN_RxHeaderTypeDef g_Can2RxHeader;
uint8_t g_Can2RxData[8];

/*
*********************************************************************************************************
*    函 数 名: bsp_InitCan1
*    功能说明: 初始CAN1
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitCan1(void)
{
    /*                Bit time configuration:
        Bit time parameter         | Nominal      |  Data
        ---------------------------|--------------|----------------
        fdcan_ker_ck               | 20 MHz       | 20 MHz
        Time_quantum (tq)          | 50 ns        | 50 ns
        Synchronization_segment    | 1 tq         | 1 tq
        Propagation_segment        | 23 tq        | 1 tq
        Phase_segment_1            | 8 tq         | 4 tq
        Phase_segment_2            | 8 tq         | 4 tq
        Synchronization_Jump_width | 8 tq         | 4 tq
        Bit_length                 | 40 tq = 2 祍 | 10 tq = 0.5 祍
        Bit_rate                   | 0.5 MBit/s   | 2 MBit/s
    */
    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission = ENABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = ENABLE;
    hfdcan1.Init.NominalPrescaler = 0x1; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
    hfdcan1.Init.NominalSyncJumpWidth = 0x8;
    hfdcan1.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
    hfdcan1.Init.NominalTimeSeg2 = 0x8;
    hfdcan1.Init.DataPrescaler = 0x1;
    hfdcan1.Init.DataSyncJumpWidth = 0x4;
    hfdcan1.Init.DataTimeSeg1 = 0x5; /* DataTimeSeg1 = Propagation_segment + Phase_segment_1 */
    hfdcan1.Init.DataTimeSeg2 = 0x4;
    hfdcan1.Init.MessageRAMOffset = 0;
    hfdcan1.Init.StdFiltersNbr = 1;
    hfdcan1.Init.ExtFiltersNbr = 0;
    hfdcan1.Init.RxFifo0ElmtsNbr = 2;
    hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
    hfdcan1.Init.RxFifo1ElmtsNbr = 0;
    hfdcan1.Init.RxBuffersNbr = 0;
    hfdcan1.Init.TxEventsNbr = 0;
    hfdcan1.Init.TxBuffersNbr = 0;
    hfdcan1.Init.TxFifoQueueElmtsNbr = 2;
    hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
    HAL_FDCAN_Init(&hfdcan1);

    /* Configure Rx filter */
    sFilterConfig1.IdType = FDCAN_STANDARD_ID;
    sFilterConfig1.FilterIndex = 0;
    sFilterConfig1.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig1.FilterID1 = 0x111;
    sFilterConfig1.FilterID2 = 0x7FF; /* For acceptance, MessageID and FilterID1 must match exactly */
    HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig1);

    /* Configure Rx FIFO 0 watermark to 2 */
    HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 2);

    /* Activate Rx FIFO 0 watermark notification */
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_WATERMARK, 0);

    /* Start the FDCAN module */
    HAL_FDCAN_Start(&hfdcan1);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitCan2
*    功能说明: 初始CAN2
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitCan2(void)
{
    /*                Bit time configuration:
        Bit time parameter         | Nominal      |  Data
        ---------------------------|--------------|----------------
        fdcan_ker_ck               | 20 MHz       | 20 MHz
        Time_quantum (tq)          | 50 ns        | 50 ns
        Synchronization_segment    | 1 tq         | 1 tq
        Propagation_segment        | 23 tq        | 1 tq
        Phase_segment_1            | 8 tq         | 4 tq
        Phase_segment_2            | 8 tq         | 4 tq
        Synchronization_Jump_width | 8 tq         | 4 tq
        Bit_length                 | 40 tq = 2 祍 | 10 tq = 0.5 祍
        Bit_rate                   | 0.5 MBit/s   | 2 MBit/s
    */
    hfdcan2.Instance = FDCAN2;
    hfdcan2.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
    hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan2.Init.AutoRetransmission = ENABLE;
    hfdcan2.Init.TransmitPause = DISABLE;
    hfdcan2.Init.ProtocolException = ENABLE;
    hfdcan2.Init.NominalPrescaler = 0x1; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
    hfdcan2.Init.NominalSyncJumpWidth = 0x8;
    hfdcan2.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
    hfdcan2.Init.NominalTimeSeg2 = 0x8;
    hfdcan2.Init.DataPrescaler = 0x1;
    hfdcan2.Init.DataSyncJumpWidth = 0x4;
    hfdcan2.Init.DataTimeSeg1 = 0x5; /* DataTimeSeg1 = Propagation_segment + Phase_segment_1 */
    hfdcan2.Init.DataTimeSeg2 = 0x4;
    hfdcan2.Init.MessageRAMOffset = 0;
    hfdcan2.Init.StdFiltersNbr = 1;
    hfdcan2.Init.ExtFiltersNbr = 0;
    hfdcan2.Init.RxFifo0ElmtsNbr = 2;
    hfdcan2.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
    hfdcan2.Init.RxFifo1ElmtsNbr = 0;
    hfdcan2.Init.RxBuffersNbr = 0;
    hfdcan2.Init.TxEventsNbr = 0;
    hfdcan2.Init.TxBuffersNbr = 0;
    hfdcan2.Init.TxFifoQueueElmtsNbr = 2;
    hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    hfdcan2.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
    HAL_FDCAN_Init(&hfdcan2);

    /* Configure Rx filter */
    sFilterConfig2.IdType = FDCAN_STANDARD_ID;
    sFilterConfig2.FilterIndex = 0;
    sFilterConfig2.FilterType = FDCAN_FILTER_MASK;
    sFilterConfig2.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig2.FilterID1 = 0x222;
    sFilterConfig2.FilterID2 = 0x7FF; /* For acceptance, MessageID and FilterID1 must match exactly */
    HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig2);

    /* Configure Rx FIFO 0 watermark to 2 */
    HAL_FDCAN_ConfigFifoWatermark(&hfdcan2, FDCAN_CFG_RX_FIFO0, 2);

    /* Activate Rx FIFO 0 watermark notification */
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_WATERMARK, 0);

    /* Start the FDCAN module */
    HAL_FDCAN_Start(&hfdcan2);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_DeInitCan1
*    功能说明: 释放CAN1
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_DeInitCan1(void)
{
    HAL_FDCAN_MspDeInit(&hfdcan1);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_DeInitCan2
*    功能说明: 释放CAN2
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_DeInitCan2(void)
{
    HAL_FDCAN_MspDeInit(&hfdcan2);
}

/*
*********************************************************************************************************
*    函 数 名: HAL_FDCAN_MspInit
*    功能说明: 配置CAN gpio
*    形    参: hfdcan
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

    if (hfdcan == &hfdcan1)
    {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        FDCAN1_TX_GPIO_CLK_ENABLE();
        FDCAN1_RX_GPIO_CLK_ENABLE();

        /* Select PLL1Q as source of FDCANx clock */
        RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
        RCC_PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
        HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

        /* Enable FDCANx clock */
        __HAL_RCC_FDCAN_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* FDCANx TX GPIO pin configuration  */
        GPIO_InitStruct.Pin = FDCAN1_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = FDCAN1_TX_AF;
        HAL_GPIO_Init(FDCAN1_TX_GPIO_PORT, &GPIO_InitStruct);

        /* FDCANx RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = FDCAN1_RX_PIN;
        GPIO_InitStruct.Alternate = FDCAN1_RX_AF;
        HAL_GPIO_Init(FDCAN1_RX_GPIO_PORT, &GPIO_InitStruct);

        /*##-3- Configure the NVIC #################################################*/
        /* NVIC for FDCANx */
        HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 1);
        HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 0, 1);
        HAL_NVIC_SetPriority(FDCAN_CAL_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
        HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
        HAL_NVIC_EnableIRQ(FDCAN_CAL_IRQn);
    }

    if (hfdcan == &hfdcan2)
    {
        /*##-1- Enable peripherals and GPIO Clocks #################################*/
        /* Enable GPIO TX/RX clock */
        FDCAN2_TX_GPIO_CLK_ENABLE();
        FDCAN2_RX_GPIO_CLK_ENABLE();

        /* Select PLL1Q as source of FDCANx clock */
        RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
        RCC_PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
        HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

        /* Enable FDCANx clock */
        __HAL_RCC_FDCAN_CLK_ENABLE();

        /*##-2- Configure peripheral GPIO ##########################################*/
        /* FDCANx TX GPIO pin configuration  */
        GPIO_InitStruct.Pin = FDCAN2_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = FDCAN2_TX_AF;
        HAL_GPIO_Init(FDCAN2_TX_GPIO_PORT, &GPIO_InitStruct);

        /* FDCANx RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = FDCAN2_RX_PIN;
        GPIO_InitStruct.Alternate = FDCAN2_RX_AF;
        HAL_GPIO_Init(FDCAN2_RX_GPIO_PORT, &GPIO_InitStruct);

        /*##-3- Configure the NVIC #################################################*/
        /* NVIC for FDCANx */
        HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 0, 1);
        HAL_NVIC_SetPriority(FDCAN2_IT1_IRQn, 0, 1);
        HAL_NVIC_SetPriority(FDCAN_CAL_IRQn, 0, 0);

        HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
        HAL_NVIC_EnableIRQ(FDCAN2_IT1_IRQn);
        HAL_NVIC_EnableIRQ(FDCAN_CAL_IRQn);
    }
}

/*
*********************************************************************************************************
*    函 数 名: HAL_FDCAN_MspInit
*    功能说明: 配置CAN gpio, 恢复为普通GPIO，取消中断
*    形    参: hfdcan
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *hfdcan)
{
    if (hfdcan == &hfdcan1)
    {
        /*##-1- Reset peripherals ##################################################*/
        __HAL_RCC_FDCAN_FORCE_RESET();
        __HAL_RCC_FDCAN_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks ################################*/
        /* Configure FDCANx Tx as alternate function  */
        HAL_GPIO_DeInit(FDCAN1_TX_GPIO_PORT, FDCAN1_TX_PIN);

        /* Configure FDCANx Rx as alternate function  */
        HAL_GPIO_DeInit(FDCAN1_RX_GPIO_PORT, FDCAN1_RX_PIN);

        /*##-3- Disable the NVIC for FDCANx ########################################*/
        HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN_CAL_IRQn);
    }

    if (hfdcan == &hfdcan2)
    {
        /*##-1- Reset peripherals ##################################################*/
        __HAL_RCC_FDCAN_FORCE_RESET();
        __HAL_RCC_FDCAN_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks ################################*/
        /* Configure FDCANx Tx as alternate function  */
        HAL_GPIO_DeInit(FDCAN2_TX_GPIO_PORT, FDCAN2_TX_PIN);

        /* Configure FDCANx Rx as alternate function  */
        HAL_GPIO_DeInit(FDCAN2_RX_GPIO_PORT, FDCAN2_RX_PIN);

        /*##-3- Disable the NVIC for FDCANx ########################################*/
        HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN2_IT1_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN_CAL_IRQn);
    }
}

/*
*********************************************************************************************************
*    函 数 名: HAL_FDCAN_RxFifo0Callback
*    功能说明: CAN中断服务程序-回调函数
*    形    参: hfdcan
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if (hfdcan == &hfdcan1)
    {
        if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_WATERMARK) != RESET)
        {
            /* Retreive Rx messages from RX FIFO0 */
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &g_Can1RxHeader, g_Can1RxData);

            /* Activate Rx FIFO 0 watermark notification */
            HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_WATERMARK, 0);

            if (g_Can1RxHeader.Identifier == 0x111 && g_Can1RxHeader.IdType == FDCAN_STANDARD_ID)
            {
                bsp_PutMsg(MSG_CAN1_RX, 0); /* 发消息收到数据包，结果在g_Can1RxHeader， g_Can1RxData */
            }
        }
    }

    if (hfdcan == &hfdcan2)
    {
        if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_WATERMARK) != RESET)
        {
            /* Retreive Rx messages from RX FIFO0 */
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &g_Can2RxHeader, g_Can2RxData);

            /* Activate Rx FIFO 0 watermark notification */
            HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_WATERMARK, 0);

            if (g_Can2RxHeader.Identifier == 0x222 && g_Can2RxHeader.IdType == FDCAN_STANDARD_ID)
            {
                bsp_PutMsg(MSG_CAN2_RX, 0); /* 发消息收到数据包，结果在g_Can1RxHeader， g_Can1RxData */
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: can1_SendPacket
*    功能说明: 发送一包数据
*    形    参：_DataBuf 数据缓冲区
*              _Len 数据长度, 0-8字节
*    返 回 值: 无
*********************************************************************************************************
*/
void can1_SendPacket(uint8_t *_DataBuf, uint8_t _Len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    if (_Len > 8)
    {
        return;
    }

    /* Prepare Tx Header */
    TxHeader.Identifier = 0x222;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = (uint32_t)_Len << 16;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_ON;
    TxHeader.FDFormat = FDCAN_FD_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    /* Add messages to TX FIFO */
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, _DataBuf);
}

/*
*********************************************************************************************************
*    函 数 名: can2_SendPacket
*    功能说明: 发送一包数据
*    形    参：_DataBuf 数据缓冲区
*              _Len 数据长度, 0-8字节
*    返 回 值: 无
*********************************************************************************************************
*/
void can2_SendPacket(uint8_t *_DataBuf, uint8_t _Len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    if (_Len > 8)
    {
        return;
    }

    /* Prepare Tx Header */
    TxHeader.Identifier = 0x111;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = (uint32_t)_Len << 16;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_ON;
    TxHeader.FDFormat = FDCAN_FD_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    /* Add messages to TX FIFO */
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, _DataBuf);
}

/*
*********************************************************************************************************
*    函 数 名: FDCAN1_IT0_IRQHandler
*    功能说明: CAN中断服务程序
*    形    参: hfdcan
*    返 回 值: 无
*********************************************************************************************************
*/
void FDCAN1_IT0_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&hfdcan1);
}

void FDCAN2_IT0_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&hfdcan2);
}

void FDCAN1_IT1_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&hfdcan1);
}

void FDCAN2_IT1_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&hfdcan2);
}

void FDCAN_CAL_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&hfdcan1);

    HAL_FDCAN_IRQHandler(&hfdcan2); // ???
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
