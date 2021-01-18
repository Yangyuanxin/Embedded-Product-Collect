/*
*********************************************************************************************************
*
*    模块名称 : SPI总线驱动
*    文件名称 : bsp_spi_bus1.h
*    版    本 : V1.2
*    说    明 : SPI总线底层驱动。支持多个SPI独立使用。
*    修改记录 :
*        版本号  日期        作者    说明
*       v1.0    2014-10-24 armfly  首版。将串行FLASH、TSC2046、VS1053、AD7705、ADS1256等SPI设备的配置
*                                    和收发数据的函数进行汇总分类。并解决不同速度的设备间的共享问题。
*        V1.1    2015-02-25 armfly  硬件SPI时，没有开启GPIOB时钟，已解决。
*        V1.2    2015-07-23 armfly  修改 bsp_SPI_Init() 函数，增加开关SPI时钟的语句。规范硬件SPI和软件SPI的宏定义.
*
*    Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
    口线分配
    LCD :
        PC10/SPI3_SCK    
        PC12/SPI3_MOSI
        
    输出端口（SPI控制器）
        PE2/SPI4_SCK
        PE5/SPI4_MISO
        PE6/SPI4_MOSI
    
    SWD烧写口
        PD3/SPI2_SCK
        PI3/SPI2_MOSI
        PH6/SPI5_SCK

        - SWDIO发送时 : PD3/SPI2_SCK, PI3/SPI2_MOSI   (SPI2做主机）
        - SWDIO接收时 : PD3/SPI2_SCK，, PI3/SPI2_MOSI (SPI2做从机，SPI5只用SCK产生时钟）
*/

/* 
    armfly ： 2018-10-03  SPI 遗留问题待查 (估计HAL库存在缺陷）
    1. DMA模式，发送缓冲区必须是 const 定位在flash才能正常传输。定位在SRAM，则DMA传输异常。
    2. INT模式，32字节一下连续发送正常。48字节以上异常，只能发送一次。第2次会中途终止。
    3. 目前采用普通轮询模式，未发现问题。
    
    测试代码如下：
    bsp_InitSPIBus();
    bsp_InitSPIParam(SPI_BAUDRATEPRESCALER_8, SPI_PHASE_1EDGE, SPI_POLARITY_LOW);
    while (1)
    {
        g_spiLen = 32;        // 数据长度
        bsp_spiTransfer();
        
        bsp_DelayUS(1000);
    }
*/
//#define USE_SPI_DMA
//#define USE_SPI_INT
#define USE_SPI_POLL

/************************* SPI1 *********************/
#if USE_SPI1_EN == 1
    //#define SPI1_CLK_ENABLE()            __HAL_RCC_SPI1_CLK_ENABLE()

    //#define SPI1_FORCE_RESET()            __HAL_RCC_SPI1_FORCE_RESET()
    //#define SPI1_RELEASE_RESET()        __HAL_RCC_SPI1_RELEASE_RESET()

    //#define SPI1_SCK_CLK_ENABLE()        __HAL_RCC_GPIOD_CLK_ENABLE()
    //#define SPI1_SCK_GPIO                GPIOD
    //#define SPI1_SCK_PIN                GPIO_PIN_3
    //#define SPI1_SCK_AF                    GPIO_AF5_SPI1

    //#define SPI1_MISO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
    //#define SPI1_MISO_GPIO                GPIOB
    //#define SPI1_MISO_PIN                 GPIO_PIN_4
    //#define SPI1_MISO_AF                GPIO_AF5_SPI1

    //#define SPI1_MOSI_CLK_ENABLE()        __HAL_RCC_GPIOI_CLK_ENABLE()
    //#define SPI1_MOSI_GPIO                GPIOI
    //#define SPI1_MOSI_PIN                 GPIO_PIN_3
    //#define SPI1_MOSI_AF                GPIO_AF5_SPI1
#endif

/************************* SPI2 *********************/
#if USE_SPI2_EN == 1
    #define SPI2_SCK_CLK_ENABLE()        __HAL_RCC_GPIOD_CLK_ENABLE()
    #define SPI2_SCK_GPIO                GPIOD
    #define SPI2_SCK_PIN                GPIO_PIN_3
    #define SPI2_SCK_AF                    GPIO_AF5_SPI2

    //#define SPI2_MISO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
    //#define SPI2_MISO_GPIO                GPIOB
    //#define SPI2_MISO_PIN                 GPIO_PIN_4
    //#define SPI2_MISO_AF                GPIO_AF5_SPI2

    #define SPI2_MOSI_CLK_ENABLE()        __HAL_RCC_GPIOI_CLK_ENABLE()
    #define SPI2_MOSI_GPIO                GPIOI
    #define SPI2_MOSI_PIN                 GPIO_PIN_3
    #define SPI2_MOSI_AF                GPIO_AF5_SPI2
#endif

/************************* SPI3 *********************/
#if USE_SPI3_EN == 1
    #define SPI3_SCK_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
    #define SPI3_SCK_GPIO                GPIOC
    #define SPI3_SCK_PIN                GPIO_PIN_10
    #define SPI3_SCK_AF                    GPIO_AF5_SPI3

    //#define SPI3_MISO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
    //#define SPI3_MISO_GPIO                GPIOB
    //#define SPI3_MISO_PIN                 GPIO_PIN_4
    //#define SPI3_MISO_AF                GPIO_AF5_SPI3

    #define SPI3_MOSI_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
    #define SPI3_MOSI_GPIO                GPIOC
    #define SPI3_MOSI_PIN                 GPIO_PIN_12
    #define SPI3_MOSI_AF                GPIO_AF5_SPI3
#endif

/************************* SPI4 *********************/
#if USE_SPI4_EN == 1
    #define SPI4_SCK_CLK_ENABLE()        __HAL_RCC_GPIOE_CLK_ENABLE()
    #define SPI4_SCK_GPIO                GPIOE
    #define SPI4_SCK_PIN                GPIO_PIN_2
    #define SPI4_SCK_AF                    GPIO_AF5_SPI4

    #define SPI4_MISO_CLK_ENABLE()        __HAL_RCC_GPIOE_CLK_ENABLE()
    #define SPI4_MISO_GPIO                GPIOE
    #define SPI4_MISO_PIN                 GPIO_PIN_5
    #define SPI4_MISO_AF                GPIO_AF5_SPI4

    #define SPI4_MOSI_CLK_ENABLE()        __HAL_RCC_GPIOE_CLK_ENABLE()
    #define SPI4_MOSI_GPIO                GPIOE
    #define SPI4_MOSI_PIN                 GPIO_PIN_6
    #define SPI4_MOSI_AF                GPIO_AF5_SPI4
#endif

/************************* SPI5 *********************/
#if USE_SPI5_EN == 1
    #define SPI5_SCK_CLK_ENABLE()        __HAL_RCC_GPIOH_CLK_ENABLE()
    #define SPI5_SCK_GPIO                GPIOH
    #define SPI5_SCK_PIN                GPIO_PIN_6
    #define SPI5_SCK_AF                    GPIO_AF5_SPI5

    //#define SPI5_MISO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
    //#define SPI5_MISO_GPIO                GPIOB
    //#define SPI5_MISO_PIN                 GPIO_PIN_4
    //#define SPI5_MISO_AF                GPIO_AF5_SPI5

    //#define SPI5_MOSI_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
    //#define SPI5_MOSI_GPIO                GPIOB
    //#define SPI5_MOSI_PIN                 GPIO_PIN_5
    //#define SPI5_MOSI_AF                GPIO_AF5_SPI5
#endif

/************************* SPI6 *********************/
#if USE_SPI6_EN == 1
    #define SPI6_SCK_CLK_ENABLE()        __HAL_RCC_GPIOH_CLK_ENABLE()
    #define SPI6_SCK_GPIO                GPIOH
    #define SPI6_SCK_PIN                GPIO_PIN_6
    #define SPI6_SCK_AF                    GPIO_AF5_SPI6

    //#define SPI6_MISO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
    //#define SPI6_MISO_GPIO                GPIOB
    //#define SPI6_MISO_PIN                 GPIO_PIN_4
    //#define SPI6_MISO_AF                GPIO_AF5_SPI6

    //#define SPI6_MOSI_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
    //#define SPI6_MOSI_GPIO                GPIOB
    //#define SPI6_MOSI_PIN                 GPIO_PIN_5
    //#define SPI6_MOSI_AF                GPIO_AF5_SPI6
#endif

/************************* end *********************/

#ifdef USE_SPI_DMA
/* Definition for SPIx's DMA */
#define SPIx_TX_DMA_STREAM               DMA2_Stream3
#define SPIx_RX_DMA_STREAM               DMA2_Stream2

#define SPIx_TX_DMA_REQUEST              DMA_REQUEST_SPI1_TX
#define SPIx_RX_DMA_REQUEST              DMA_REQUEST_SPI1_RX

/* Definition for SPIx's NVIC */
#define SPIx_DMA_TX_IRQn                 DMA2_Stream3_IRQn
#define SPIx_DMA_RX_IRQn                 DMA2_Stream2_IRQn

#define SPIx_DMA_TX_IRQHandler           DMA2_Stream3_IRQHandler
#define SPIx_DMA_RX_IRQHandler           DMA2_Stream2_IRQHandler
#endif

#ifdef USE_SPI_INT
#define SPIx_IRQn                        SPI1_IRQn
#define SPIx_IRQHandler                  SPI1_IRQHandler
#endif

enum {
    TRANSFER_WAIT,
    TRANSFER_COMPLETE,
    TRANSFER_ERROR
};

#if USE_SPI1_EN == 1
    static SPI_HandleTypeDef hspi1;
    uint8_t spi1_TxBuf[SPI1_BUFFER_SIZE];
    ALIGN_32BYTES(uint8_t spi1_RxBuf[SPI1_BUFFER_SIZE]);    /* 必须32字节对齐 */
    uint32_t spi1_len;        /* 收发的数据长度 */
    __IO uint32_t spi1_wTransferState = TRANSFER_WAIT;        /* transfer state */
    /* 备份SPI几个关键传输参数，波特率，相位，极性. 如果不同外设切换，需要重新Init SPI参数 */
    static uint32_t spi1_BaudRatePrescaler;
    static uint32_t spi1_CLKPhase;
    static uint32_t spi1_CLKPolarity;
#endif

#if USE_SPI2_EN == 1
    SPI_HandleTypeDef hspi2;
    uint8_t spi2_TxBuf[SPI2_BUFFER_SIZE];
    ALIGN_32BYTES(uint8_t spi2_RxBuf[SPI2_BUFFER_SIZE]);    /* 必须32字节对齐 */
    uint32_t spi2_len;        /* 收发的数据长度 */    
    __IO uint32_t spi2_wTransferState = TRANSFER_WAIT;        /* transfer state */
    /* 备份SPI几个关键传输参数，波特率，相位，极性. 如果不同外设切换，需要重新Init SPI参数 */
    static uint32_t spi2_BaudRatePrescaler;
    static uint32_t spi2_CLKPhase;
    static uint32_t spi2_CLKPolarity;
#endif

#if USE_SPI3_EN == 1
    static SPI_HandleTypeDef hspi3;
    uint8_t spi3_TxBuf[SPI3_BUFFER_SIZE];
    ALIGN_32BYTES(uint8_t spi3_RxBuf[SPI3_BUFFER_SIZE]);    /* 必须32字节对齐 */
    uint32_t spi3_len;        /* 收发的数据长度 */
    __IO uint32_t spi3_wTransferState = TRANSFER_WAIT;        /* transfer state */
    /* 备份SPI几个关键传输参数，波特率，相位，极性. 如果不同外设切换，需要重新Init SPI参数 */
    static uint32_t spi3_BaudRatePrescaler;
    static uint32_t spi3_CLKPhase;
    static uint32_t spi3_CLKPolarity;    
#endif

#if USE_SPI4_EN == 1
    static SPI_HandleTypeDef hspi4;
    uint8_t spi4_TxBuf[SPI4_BUFFER_SIZE];
    ALIGN_32BYTES(uint8_t spi4_RxBuf[SPI4_BUFFER_SIZE]);    /* 必须32字节对齐 */
    uint32_t spi4_len;        /* 收发的数据长度 */    
    __IO uint32_t spi4_wTransferState = TRANSFER_WAIT;        /* transfer state */
    /* 备份SPI几个关键传输参数，波特率，相位，极性. 如果不同外设切换，需要重新Init SPI参数 */
    static uint32_t spi4_BaudRatePrescaler;
    static uint32_t spi4_CLKPhase;
    static uint32_t spi4_CLKPolarity;    
#endif

#if USE_SPI5_EN == 1
    static SPI_HandleTypeDef hspi5;
    uint8_t spi5_TxBuf[SPI5_BUFFER_SIZE];
    ALIGN_32BYTES(uint8_t spi5_RxBuf[SPI5_BUFFER_SIZE]);    /* 必须32字节对齐 */
    uint32_t spi5_len;        /* 收发的数据长度 */    
    __IO uint32_t spi5_wTransferState = TRANSFER_WAIT;        /* transfer state */
    /* 备份SPI几个关键传输参数，波特率，相位，极性. 如果不同外设切换，需要重新Init SPI参数 */
    static uint32_t spi5_BaudRatePrescaler;
    static uint32_t spi5_CLKPhase;
    static uint32_t spi5_CLKPolarity;    
#endif

#if USE_SPI6_EN == 1
    static SPI_HandleTypeDef hspi6;
    uint8_t spi6_TxBuf[SPI6_BUFFER_SIZE];
    ALIGN_32BYTES(uint8_t spi6_RxBuf[SPI6_BUFFER_SIZE]);    /* 必须32字节对齐 */
    uint32_t spi6_len;        /* 收发的数据长度 */    
    __IO uint32_t spi6_wTransferState = TRANSFER_WAIT;        /* transfer state */
    /* 备份SPI几个关键传输参数，波特率，相位，极性. 如果不同外设切换，需要重新Init SPI参数 */
    static uint32_t spi6_BaudRatePrescaler;
    static uint32_t spi6_CLKPhase;
    static uint32_t spi6_CLKPolarity;    
#endif

/*************************************/

#ifdef USE_SPI_DMA
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;
#endif

//uint8_t g_spi_busy;

/*
*********************************************************************************************************
*    函 数 名: bsp_InitSPIBus1
*    功能说明: 配置SPI总线。 只包括 SCK、 MOSI、 MISO口线的配置。不包括片选CS，也不包括外设芯片特有的INT、BUSY等
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPIBus(void)
{        
    /* SPI3 用于LCD */
    bsp_InitSPIParam(SPI3, SPI_BAUDRATEPRESCALER_64, SPI_PHASE_1EDGE, SPI_POLARITY_LOW);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitSPIParam
*    功能说明: 配置SPI总线参数，波特率、
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPIParam(SPI_TypeDef *_spi, uint32_t _BaudRatePrescaler, uint32_t _CLKPhase, uint32_t _CLKPolarity)
{
    #if USE_SPI1_EN == 1
    if (_spi == SPI1)
    {
        if (spi1_BaudRatePrescaler == _BaudRatePrescaler && spi1_CLKPhase == _CLKPhase && spi1_CLKPolarity == _CLKPolarity)
        {        
            return;
        }
        
        spi1_BaudRatePrescaler = _BaudRatePrescaler;    
        spi1_CLKPhase = _CLKPhase;
        spi1_CLKPolarity = _CLKPolarity;
        
        /*##-1- Configure the SPI peripheral #######################################*/
        /* Set the SPI parameters */
        hspi1.Instance               = SPI1;
        hspi1.Init.BaudRatePrescaler = _BaudRatePrescaler;
        hspi1.Init.Direction         = SPI_DIRECTION_2LINES;
        hspi1.Init.CLKPhase          = _CLKPhase;
        hspi1.Init.CLKPolarity       = _CLKPolarity;
        hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
        hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
        hspi1.Init.TIMode            = SPI_TIMODE_DISABLE;
        hspi1.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
        hspi1.Init.CRCPolynomial     = 7;
        hspi1.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
        hspi1.Init.NSS               = SPI_NSS_SOFT;
        hspi1.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
        hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommanded setting to avoid glitches */
        hspi1.Init.Mode              = SPI_MODE_MASTER;

        if (HAL_SPI_Init(&hspi1) != HAL_OK)
        {
            /* Initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }            
    }
    #endif

    #if USE_SPI2_EN == 1
    if (_spi == SPI2)
    {
        if (spi2_BaudRatePrescaler == _BaudRatePrescaler && spi2_CLKPhase == _CLKPhase && spi2_CLKPolarity == _CLKPolarity)
        {        
            return;
        }
        
        spi2_BaudRatePrescaler = _BaudRatePrescaler;    
        spi2_CLKPhase = _CLKPhase;
        spi2_CLKPolarity = _CLKPolarity;
        
        /*##-1- Configure the SPI peripheral #######################################*/
        

        /* Set the SPI parameters */
        hspi2.Instance               = SPI2;

        hspi2.Init.Mode              = SPI_MODE_MASTER;
        hspi2.Init.Direction         =     SPI_DIRECTION_1LINE;                    // SPI_DIRECTION_2LINES;
        hspi2.Init.DataSize          = SPI_DATASIZE_8BIT;
        hspi2.Init.CLKPolarity       = _CLKPolarity;
        hspi2.Init.CLKPhase          = _CLKPhase;
        hspi2.Init.NSS               = SPI_NSS_HARD_OUTPUT;    // SPI_NSS_SOFT;
        hspi2.Init.BaudRatePrescaler = _BaudRatePrescaler;
        hspi2.Init.FirstBit          =     SPI_FIRSTBIT_LSB;                        // SPI_FIRSTBIT_MSB;
        hspi2.Init.TIMode            = SPI_TIMODE_DISABLE;
        hspi2.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
        hspi2.Init.CRCPolynomial     = 0;
        hspi2.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
        hspi2.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
        hspi2.Init.NSSPolarity            = SPI_NSS_POLARITY_LOW;
        hspi2.Init.FifoThreshold          = SPI_FIFO_THRESHOLD_01DATA;

        hspi2.Init.TxCRCInitializationPattern   = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
        hspi2.Init.RxCRCInitializationPattern   = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
        hspi2.Init.MasterSSIdleness             = SPI_MASTER_SS_IDLENESS_00CYCLE;
        hspi2.Init.MasterInterDataIdleness      = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
        hspi2.Init.MasterReceiverAutoSusp       = SPI_MASTER_RX_AUTOSUSP_DISABLE;
        hspi2.Init.MasterKeepIOState       = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommanded setting to avoid glitches */
        hspi2.Init.IOSwap                       = SPI_IO_SWAP_DISABLE;

        if (HAL_SPI_Init(&hspi2) != HAL_OK)
        {
            /* Initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }            
    }
    #endif

    #if USE_SPI3_EN == 1
    if (_spi == SPI3)
    {
        if (spi3_BaudRatePrescaler == _BaudRatePrescaler && spi3_CLKPhase == _CLKPhase && spi3_CLKPolarity == _CLKPolarity)
        {        
            return;
        }
        
        spi3_BaudRatePrescaler = _BaudRatePrescaler;    
        spi3_CLKPhase = _CLKPhase;
        spi3_CLKPolarity = _CLKPolarity;
        
        /*##-1- Configure the SPI peripheral #######################################*/
        /* Set the SPI parameters */
        hspi3.Instance               = SPI3;
        hspi3.Init.BaudRatePrescaler = _BaudRatePrescaler;
        hspi3.Init.Direction         = SPI_DIRECTION_2LINES;
        hspi3.Init.CLKPhase          = _CLKPhase;
        hspi3.Init.CLKPolarity       = _CLKPolarity;
        hspi3.Init.DataSize          = SPI_DATASIZE_8BIT;
        hspi3.Init.FirstBit          = SPI_FIRSTBIT_MSB;
        hspi3.Init.TIMode            = SPI_TIMODE_DISABLE;
        hspi3.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
        hspi3.Init.CRCPolynomial     = 7;
        hspi3.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
        hspi3.Init.NSS               = SPI_NSS_SOFT;
        hspi3.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
        hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommanded setting to avoid glitches */
        hspi3.Init.Mode              = SPI_MODE_MASTER;

        if (HAL_SPI_Init(&hspi3) != HAL_OK)
        {
            /* Initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }            
    }
    #endif
    
    #if USE_SPI4_EN == 1
    if (_spi == SPI4)
    {
        if (spi4_BaudRatePrescaler == _BaudRatePrescaler && spi4_CLKPhase == _CLKPhase && spi4_CLKPolarity == _CLKPolarity)
        {        
            return;
        }
        
        spi4_BaudRatePrescaler = _BaudRatePrescaler;    
        spi4_CLKPhase = _CLKPhase;
        spi4_CLKPolarity = _CLKPolarity;
        
        /*##-1- Configure the SPI peripheral #######################################*/
        /* Set the SPI parameters */
        hspi4.Instance               = SPI4;
        hspi4.Init.BaudRatePrescaler = _BaudRatePrescaler;
        hspi4.Init.Direction         = SPI_DIRECTION_2LINES;
        hspi4.Init.CLKPhase          = _CLKPhase;
        hspi4.Init.CLKPolarity       = _CLKPolarity;
        hspi4.Init.DataSize          = SPI_DATASIZE_8BIT;
        hspi4.Init.FirstBit          = SPI_FIRSTBIT_MSB;
        hspi4.Init.TIMode            = SPI_TIMODE_DISABLE;
        hspi4.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
        hspi4.Init.CRCPolynomial     = 7;
        hspi4.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
        hspi4.Init.NSS               = SPI_NSS_SOFT;
        hspi4.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
        hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommanded setting to avoid glitches */
        hspi4.Init.Mode              = SPI_MODE_MASTER;

        if (HAL_SPI_Init(&hspi4) != HAL_OK)
        {
            /* Initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }            
    }
    #endif

    #if USE_SPI5_EN == 1
    if (_spi == SPI5)
    {
        if (spi5_BaudRatePrescaler == _BaudRatePrescaler && spi5_CLKPhase == _CLKPhase && spi5_CLKPolarity == _CLKPolarity)
        {        
            return;
        }
        
        spi5_BaudRatePrescaler = _BaudRatePrescaler;    
        spi5_CLKPhase = _CLKPhase;
        spi5_CLKPolarity = _CLKPolarity;
        
        /*##-5- Configure the SPI peripheral #######################################*/
        /* Set the SPI parameters */
        hspi5.Instance               = SPI5;
        hspi5.Init.BaudRatePrescaler = _BaudRatePrescaler;
        hspi5.Init.Direction         = SPI_DIRECTION_2LINES;
        hspi5.Init.CLKPhase          = _CLKPhase;
        hspi5.Init.CLKPolarity       = _CLKPolarity;
        hspi5.Init.DataSize          = SPI_DATASIZE_8BIT;
        hspi5.Init.FirstBit          = SPI_FIRSTBIT_MSB;
        hspi5.Init.TIMode            = SPI_TIMODE_DISABLE;
        hspi5.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
        hspi5.Init.CRCPolynomial     = 7;
        hspi5.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
        hspi5.Init.NSS               = SPI_NSS_SOFT;
        hspi5.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
        hspi5.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommanded setting to avoid glitches */
        hspi5.Init.Mode              = SPI_MODE_MASTER;

        if (HAL_SPI_Init(&hspi5) != HAL_OK)
        {
            /* Initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }            
    }
    #endif

    #if USE_SPI6_EN == 1
    if (_spi == SPI6)
    {
        if (spi6_BaudRatePrescaler == _BaudRatePrescaler && spi6_CLKPhase == _CLKPhase && spi6_CLKPolarity == _CLKPolarity)
        {        
            return;
        }
        
        spi6_BaudRatePrescaler = _BaudRatePrescaler;    
        spi6_CLKPhase = _CLKPhase;
        spi6_CLKPolarity = _CLKPolarity;
        
        /*##-6- Configure the SPI peripheral #######################################*/
        /* Set the SPI parameters */
        hspi6.Instance               = SPI6;
        hspi6.Init.BaudRatePrescaler = _BaudRatePrescaler;
        hspi6.Init.Direction         = SPI_DIRECTION_2LINES;
        hspi6.Init.CLKPhase          = _CLKPhase;
        hspi6.Init.CLKPolarity       = _CLKPolarity;
        hspi6.Init.DataSize          = SPI_DATASIZE_8BIT;
        hspi6.Init.FirstBit          = SPI_FIRSTBIT_MSB;
        hspi6.Init.TIMode            = SPI_TIMODE_DISABLE;
        hspi6.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
        hspi6.Init.CRCPolynomial     = 7;
        hspi6.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
        hspi6.Init.NSS               = SPI_NSS_SOFT;
        hspi6.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
        hspi6.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommanded setting to avoid glitches */
        hspi6.Init.Mode              = SPI_MODE_MASTER;

        if (HAL_SPI_Init(&hspi6) != HAL_OK)
        {
            /* Initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }            
    }
    #endif
}

/*
*********************************************************************************************************
*    函 数 名: HAL_SPI_MspInit
*    功能说明: HAL库初始化SPI的回调函数。主要是配置GPIO、SPI时钟。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *_hspi)
{
#if USE_SPI1_EN
    if (_hspi == &hspi1)
    {
        GPIO_InitTypeDef  GPIO_InitStruct;
            
        /* 配置GPIO时钟 */
        #ifdef SPI1_SCK_GPIO
        SPI1_SCK_CLK_ENABLE();
        #endif
        
        #ifdef SPI1_MISO_GPIO
        SPI1_MISO_CLK_ENABLE();
        #endif
        
        #ifdef SPI1_MOSI_GPIO
        SPIx_MOSI_CLK_ENABLE();
        #endif

        /* 使能SPI时钟 */
        __HAL_RCC_SPI1_CLK_ENABLE();

        /* 配置GPIO为SPI功能 */
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLDOWN;        /* 下拉 */
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = SPI1_SCK_AF;
        
        #ifdef SPI1_SCK_GPIO
        GPIO_InitStruct.Pin       = SPI1_SCK_PIN;
        HAL_GPIO_Init(SPI1_SCK_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI1_MISO_GPIO
        GPIO_InitStruct.Pin = SPI1_MISO_PIN;
        GPIO_InitStruct.Alternate = SPI1_MISO_AF;
        HAL_GPIO_Init(SPI1_MISO_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI1_MOSI_GPIO
        GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
        GPIO_InitStruct.Alternate = SPI1_MOSI_AF;
        HAL_GPIO_Init(SPI1_MOSI_GPIO, &GPIO_InitStruct);
        #endif
    }
#endif

#if USE_SPI2_EN
    if (_hspi == &hspi2)
    {
        GPIO_InitTypeDef  GPIO_InitStruct;
            
        /* 配置GPIO时钟 */
        #ifdef SPI2_SCK_GPIO
        SPI2_SCK_CLK_ENABLE();
        #endif
        
        #ifdef SPI2_MISO_GPIO
        SPI2_MISO_CLK_ENABLE();
        #endif
        
        #ifdef SPI2_MOSI_GPIO
        SPI2_MOSI_CLK_ENABLE();
        #endif

        /* 使能SPI时钟 */
        __HAL_RCC_SPI2_CLK_ENABLE();

        /* 配置GPIO为SPI功能 */
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLDOWN;        /* 下拉 */
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = SPI2_SCK_AF;
        
        #ifdef SPI2_SCK_GPIO
        GPIO_InitStruct.Pin       = SPI2_SCK_PIN;
        HAL_GPIO_Init(SPI2_SCK_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI2_MISO_GPIO
        GPIO_InitStruct.Pin = SPI2_MISO_PIN;
        GPIO_InitStruct.Alternate = SPI2_MISO_AF;
        HAL_GPIO_Init(SPI2_MISO_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI2_MOSI_GPIO
        GPIO_InitStruct.Pin = SPI2_MOSI_PIN;
        GPIO_InitStruct.Alternate = SPI2_MOSI_AF;
        HAL_GPIO_Init(SPI2_MOSI_GPIO, &GPIO_InitStruct);
        #endif
    }
#endif

#if USE_SPI3_EN
    if (_hspi == &hspi3)
    {
        GPIO_InitTypeDef  GPIO_InitStruct;
            
        /* 配置GPIO时钟 */
        #ifdef SPI3_SCK_GPIO
        SPI3_SCK_CLK_ENABLE();
        #endif
        
        #ifdef SPI3_MISO_GPIO
        SPI3_MISO_CLK_ENABLE();
        #endif
        
        #ifdef SPI3_MOSI_GPIO
        SPI3_MOSI_CLK_ENABLE();
        #endif

        /* 使能SPI时钟 */
        __HAL_RCC_SPI3_CLK_ENABLE();

        /* 配置GPIO为SPI功能 */
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLDOWN;        /* 下拉 */
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = SPI3_SCK_AF;
        
        #ifdef SPI3_SCK_GPIO
        GPIO_InitStruct.Pin       = SPI3_SCK_PIN;
        HAL_GPIO_Init(SPI3_SCK_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI3_MISO_GPIO
        GPIO_InitStruct.Pin = SPI3_MISO_PIN;
        GPIO_InitStruct.Alternate = SPI3_MISO_AF;
        HAL_GPIO_Init(SPI3_MISO_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI3_MOSI_GPIO
        GPIO_InitStruct.Pin = SPI3_MOSI_PIN;
        GPIO_InitStruct.Alternate = SPI3_MOSI_AF;
        HAL_GPIO_Init(SPI3_MOSI_GPIO, &GPIO_InitStruct);
        #endif
    }
#endif
    
#if USE_SPI4_EN
    if (_hspi == &hspi4)
    {
        GPIO_InitTypeDef  GPIO_InitStruct;

        /* SPI4是输出SPI口，需要单独配置
        	-- D5 : SPI_CLK
            -- D2 : SPI_MOSI
            -- D3 : SPI_MISO
        */
        EIO_D5_Config(ES_GPIO_SPI);
        EIO_D2_Config(ES_GPIO_SPI);
        EIO_D3_Config(ES_GPIO_SPI);
        
        /* 配置GPIO时钟 */
        #ifdef SPI4_SCK_GPIO
        SPI4_SCK_CLK_ENABLE();
        #endif
        
        #ifdef SPI4_MISO_GPIO
        SPI4_MISO_CLK_ENABLE();
        #endif
        
        #ifdef SPI4_MOSI_GPIO
        SPI4_MOSI_CLK_ENABLE();
        #endif

        /* 使能SPI时钟 */
        __HAL_RCC_SPI4_CLK_ENABLE();

        /* 配置GPIO为SPI功能 */
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLDOWN;        /* 下拉 */
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = SPI4_SCK_AF;
        
        #ifdef SPI4_SCK_GPIO
        GPIO_InitStruct.Pin       = SPI4_SCK_PIN;
        HAL_GPIO_Init(SPI4_SCK_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI4_MISO_GPIO
        GPIO_InitStruct.Pin = SPI4_MISO_PIN;
        GPIO_InitStruct.Alternate = SPI4_MISO_AF;
        HAL_GPIO_Init(SPI4_MISO_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI4_MOSI_GPIO
        GPIO_InitStruct.Pin = SPI4_MOSI_PIN;
        GPIO_InitStruct.Alternate = SPI4_MOSI_AF;
        HAL_GPIO_Init(SPI4_MOSI_GPIO, &GPIO_InitStruct);
        #endif
    }
#endif
    
#if USE_SPI5_EN
    if (_hspi == &hspi5)
    {
        GPIO_InitTypeDef  GPIO_InitStruct;
            
        /* 配置GPIO时钟 */
        #ifdef SPI5_SCK_GPIO
        SPI5_SCK_CLK_ENABLE();
        #endif
        
        #ifdef SPI5_MISO_GPIO
        SPI5_MISO_CLK_ENABLE();
        #endif
        
        #ifdef SPI5_MOSI_GPIO
        SPI5_MOSI_CLK_ENABLE();
        #endif

        /* 使能SPI时钟 */
        __HAL_RCC_SPI5_CLK_ENABLE();

        /* 配置GPIO为SPI功能 */
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLDOWN;        /* 下拉 */
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = SPI5_SCK_AF;
        
        #ifdef SPI5_SCK_GPIO
        GPIO_InitStruct.Pin       = SPI5_SCK_PIN;
        HAL_GPIO_Init(SPI5_SCK_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI5_MISO_GPIO
        GPIO_InitStruct.Pin = SPI5_MISO_PIN;
        GPIO_InitStruct.Alternate = SPI5_MISO_AF;
        HAL_GPIO_Init(SPI5_MISO_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI5_MOSI_GPIO
        GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
        GPIO_InitStruct.Alternate = SPI5_MOSI_AF;
        HAL_GPIO_Init(SPI5_MOSI_GPIO, &GPIO_InitStruct);
        #endif
    }
#endif

#if USE_SPI6_EN
    if (_hspi == &hspi6)
    {
        GPIO_InitTypeDef  GPIO_InitStruct;
            
        /* 配置GPIO时钟 */
        #ifdef SPI6_SCK_GPIO
        SPI6_SCK_CLK_ENABLE();
        #endif
        
        #ifdef SPI6_MISO_GPIO
        SPI6_MISO_CLK_ENABLE();
        #endif
        
        #ifdef SPI6_MOSI_GPIO
        SPI6_MOSI_CLK_ENABLE();
        #endif

        /* 使能SPI时钟 */
        __HAL_RCC_SPI6_CLK_ENABLE();

        /* 配置GPIO为SPI功能 */
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLDOWN;        /* 下拉 */
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = SPI6_SCK_AF;
        
        #ifdef SPI6_SCK_GPIO
        GPIO_InitStruct.Pin       = SPI6_SCK_PIN;
        HAL_GPIO_Init(SPI6_SCK_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI6_MISO_GPIO
        GPIO_InitStruct.Pin = SPI6_MISO_PIN;
        GPIO_InitStruct.Alternate = SPI6_MISO_AF;
        HAL_GPIO_Init(SPI6_MISO_GPIO, &GPIO_InitStruct);
        #endif
        
        #ifdef SPI6_MOSI_GPIO
        GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
        GPIO_InitStruct.Alternate = SPI6_MOSI_AF;
        HAL_GPIO_Init(SPI6_MOSI_GPIO, &GPIO_InitStruct);
        #endif
    }
#endif


/* 配置DMA和NVIC -- 暂时未启用DMA 和中断模式 */
    #ifdef USE_SPI_DMA
    {
        /* Enable DMA clock */
        DMAx_CLK_ENABLE();
        
        /*##-3- Configure the DMA ##################################################*/
        /* Configure the DMA handler for Transmission process */
        hdma_tx.Instance                 = SPIx_TX_DMA_STREAM;
        hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
        hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
        hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;    // DMA_MBURST_INC4; DMA_MBURST_SINGLE
        hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;    // DMA_PBURST_INC4; DMA_PBURST_SINGLE
        hdma_tx.Init.Request             = SPIx_TX_DMA_REQUEST;
        hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_tx.Init.Mode                = DMA_NORMAL;
        hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
        HAL_DMA_Init(&hdma_tx);
        __HAL_LINKDMA(_hspi, hdmatx, hdma_tx);     /* Associate the initialized DMA handle to the the SPI handle */

        /* Configure the DMA handler for Transmission process */
        hdma_rx.Instance                 = SPIx_RX_DMA_STREAM;
        hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
        hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
        hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;    // DMA_MBURST_INC4;  DMA_MBURST_SINGLE
        hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;    // DMA_PBURST_INC4;  DMA_PBURST_SINGLE
        hdma_rx.Init.Request             = SPIx_RX_DMA_REQUEST;
        hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_rx.Init.Mode                = DMA_NORMAL;
        hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
        HAL_DMA_Init(&hdma_rx);    
       __HAL_LINKDMA(_hspi, hdmarx, hdma_rx);    /* Associate the initialized DMA handle to the the SPI handle */

        /* NVIC configuration for DMA transfer complete interrupt (SPI1_TX) */
        HAL_NVIC_SetPriority(SPIx_DMA_TX_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);
        
        /* NVIC configuration for DMA transfer complete interrupt (SPI1_RX) */
        HAL_NVIC_SetPriority(SPIx_DMA_RX_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(SPIx_DMA_RX_IRQn);
        
        /* NVIC configuration for SPI transfer complete interrupt (SPI1) */
        HAL_NVIC_SetPriority(SPIx_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(SPIx_IRQn);
    }
    #endif

    #ifdef USE_SPI_INT
        /* NVIC configuration for SPI transfer complete interrupt (SPI1) */
        HAL_NVIC_SetPriority(SPIx_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(SPIx_IRQn);
    #endif
}
    
/*
*********************************************************************************************************
*    函 数 名: bsp_spiTransfer
*    功能说明: 向SPI总线发送1个或多个字节
*    形    参:  g_spi_TxBuf : 发送的数据
*              g_spi_RxBuf  ：收发的数据长度
*              _TxRxLen ：接收的数据缓冲区
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_spiTransfer(SPI_TypeDef *_spi)
{
#ifdef USE_SPI_POLL    
    #if USE_SPI1_EN == 1
    if (_spi == SPI1) HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)spi1_TxBuf, (uint8_t *)spi1_RxBuf, spi1_len, 100);
    #endif

    #if USE_SPI2_EN == 1
    if (_spi == SPI2) HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)spi2_TxBuf, (uint8_t *)spi2_RxBuf, spi2_len, 100);
    #endif    
    
    #if USE_SPI3_EN == 1
    if (_spi == SPI3) HAL_SPI_TransmitReceive(&hspi3, (uint8_t*)spi3_TxBuf, (uint8_t *)spi3_RxBuf, spi3_len, 100);
    #endif    

    #if USE_SPI4_EN == 1
    if (_spi == SPI4) HAL_SPI_TransmitReceive(&hspi4, (uint8_t*)spi4_TxBuf, (uint8_t *)spi4_RxBuf, spi4_len, 100);
    #endif    

    #if USE_SPI5_EN == 1
    if (_spi == SPI5) HAL_SPI_TransmitReceive(&hspi5, (uint8_t*)spi5_TxBuf, (uint8_t *)spi5_RxBuf, spi5_len, 100);
    #endif    
    
    #if USE_SPI6_EN == 1
    if (_spi == SPI6) HAL_SPI_TransmitReceive(&hspi6, (uint8_t*)spi6_TxBuf, (uint8_t *)spi6_RxBuf, spi6_len, 100);
    #endif        
#endif    


#ifdef USE_SPI_DMA
    wTransferState == TRANSFER_WAIT;
    
    while (hspi.State != HAL_SPI_STATE_READY);

    if(HAL_SPI_TransmitReceive_DMA(&hspi, (uint8_t*)g_spiTxBuf, (uint8_t *)g_spiRxBuf, g_spiLen) != HAL_OK)    
    {
        Error_Handler(__FILE__, __LINE__);
    }
    
    while (wTransferState == TRANSFER_WAIT)
    {
        ;
    }
#endif
    
#ifdef USE_SPI_INT
    wTransferState == TRANSFER_WAIT;
    
    while (hspi.State != HAL_SPI_STATE_READY);

    if(HAL_SPI_TransmitReceive_IT(&hspi, (uint8_t*)g_spiTxBuf, (uint8_t *)g_spiRxBuf, g_spiLen) != HAL_OK)    
    {
        Error_Handler(__FILE__, __LINE__);
    }
    
    while (wTransferState == TRANSFER_WAIT)
    {
        ;
    }
#endif
    
//    /* Invalidate cache prior to access by CPU */
//    SCB_InvalidateDCache_by_Addr ((uint32_t *)g_spiRxBuf, SPI_BUFFER_SIZE);
}

#ifdef USE_SPI_DMA
/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    wTransferState = TRANSFER_ERROR;
}
#endif

/*
*********************************************************************************************************
*    函 数 名: bsp_spiTransfer
*    功能说明: 向SPI总线发送一组数据
*    形    参: _txbuf :  发送的数据
*              _txlen  : 数据长度
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_SpiSendBuf(SPI_TypeDef *_spi, const char *_txbuf, uint32_t _txlen)
{
    #if USE_SPI1_EN == 1
    if (_spi == SPI1 && _txlen < SPI1_BUFFER_SIZE)
    {
        memcpy(spi1_TxBuf, _txbuf, _txlen);
        spi1_len = _txlen;
    }
    #endif

    #if USE_SPI2_EN == 1
    if (_spi == SPI2 && _txlen < SPI2_BUFFER_SIZE)
    {
        memcpy(spi2_TxBuf, _txbuf, _txlen);
        spi2_len = _txlen;
    }
    #endif    
    
    #if USE_SPI3_EN == 1
    if (_spi == SPI3 && _txlen < SPI3_BUFFER_SIZE)
    {
        memcpy(spi3_TxBuf, _txbuf, _txlen);
        spi3_len = _txlen;
    }
    #endif    

    #if USE_SPI4_EN == 1
    if (_spi == SPI4 && _txlen < SPI4_BUFFER_SIZE)
    {
        memcpy(spi4_TxBuf, _txbuf, _txlen);
        spi4_len = _txlen;
    }
    #endif    

    #if USE_SPI5_EN == 1
    if (_spi == SPI5 && _txlen < SPI5_BUFFER_SIZE)
    {
        memcpy(spi5_TxBuf, _txbuf, _txlen);
        spi5_len = _txlen;
    }
    #endif    
    
    #if USE_SPI6_EN == 1
    if (_spi == SPI6 && _txlen < SPI6_BUFFER_SIZE)
    {
        memcpy(spi6_TxBuf, _txbuf, _txlen);
        spi6_len = _txlen;
    }
    #endif     

    bsp_spiTransfer(_spi);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SpiReciveBuf
*    功能说明: 向SPI总线读取一组数据
*    形    参: _rxbuf :  接受的数据
*              _rxlen  : 数据长度
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_SpiReciveBuf(SPI_TypeDef *_spi, char *_rxbuf, uint32_t _rxlen)
{
    #if USE_SPI1_EN == 1
    if (_spi == SPI1 && _txlen < SPI1_BUFFER_SIZE)
    {
        memset(spi1_TxBuf, 0, _rxlen);
        spi1_len = _rxlen;
        bsp_spiTransfer(_spi);
        memcpy(_rxbuf, spi1_RxBuf, _rxlen);
    }
    #endif

    #if USE_SPI2_EN == 1
    if (_spi == SPI2 && _rxlen < SPI2_BUFFER_SIZE)
    {
        memset(spi2_TxBuf, 0, _rxlen);
        spi2_len = _rxlen;
        bsp_spiTransfer(_spi);
        memcpy(_rxbuf, spi2_RxBuf, _rxlen);
    }
    #endif    
    
    #if USE_SPI3_EN == 1
    if (_spi == SPI3 && _rxlen < SPI3_BUFFER_SIZE)
    {
        memset(spi3_TxBuf, 0, _rxlen);
        spi3_len = _rxlen;
        bsp_spiTransfer(_spi);
        memcpy(_rxbuf, spi3_RxBuf, _rxlen);
    }
    #endif    

    #if USE_SPI4_EN == 1
    if (_spi == SPI4 && _rxlen < SPI4_BUFFER_SIZE)
    {
        memset(spi4_TxBuf, 0, _rxlen);
        spi4_len = _rxlen;
        bsp_spiTransfer(_spi);
        memcpy(_rxbuf, spi4_RxBuf, _rxlen);
    }
    #endif    

    #if USE_SPI5_EN == 1
    if (_spi == SPI5 && _rxlen < SPI5_BUFFER_SIZE)
    {
        memset(spi5_TxBuf, 0, _rxlen);
        spi5_len = _rxlen;
        bsp_spiTransfer(_spi);
        memcpy(_rxbuf, spi5_RxBuf, _rxlen);
    }
    #endif    
    
    #if USE_SPI6_EN == 1
    if (_spi == SPI6 && _rxlen < SPI6_BUFFER_SIZE)
    {
        memset(spi6_TxBuf, 0, _rxlen);
        spi6_len = _rxlen;
        bsp_spiTransfer(_spi);
        memcpy(_rxbuf, spi6_RxBuf, _rxlen);
    }
    #endif
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SpiSendRecive
*    功能说明: 向SPI总线读取一组数据
*    形    参: 
*               _txbuf : 发送的数据
*               _txlen : 发送的数据长度
*               _rxbuf : 接受的数据
*               _rxlen  : 接收的数据长度
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_SpiSendRecive(SPI_TypeDef *_spi, const char *_txbuf, uint32_t _txlen, char *_rxbuf, uint32_t _rxlen)
{
    #if USE_SPI1_EN == 1
    if (_spi == SPI1 && _txlen + _rxlen < SPI1_BUFFER_SIZE)
    {
        memset(spi1_TxBuf, 0, _txlen + _rxlen);
        memset(spi1_TxBuf, _txbuf, _txlen);    
        spi1_len = _txlen + _rxlen;
        bsp_spiTransfer(_spi);
    
        memcpy(_rxbuf, &spi1_RxBuf[_txlen], _rxlen);
    }
    #endif

    #if USE_SPI2_EN == 1
    if (_spi == SPI2 && _txlen + _rxlen < SPI2_BUFFER_SIZE)
    {
        memset(spi2_TxBuf, 0, _txlen + _rxlen);
        memcpy(spi2_TxBuf, _txbuf, _txlen);    
        spi2_len = _txlen + _rxlen;
        bsp_spiTransfer(_spi);
    
        memcpy(_rxbuf, &spi2_RxBuf[_txlen], _rxlen);
    }
    #endif    
    
    #if USE_SPI3_EN == 1
    if (_spi == SPI3 && _txlen + _rxlen < SPI3_BUFFER_SIZE)
    {
        memset(spi3_TxBuf, 0, _txlen + _rxlen);
        memcpy(spi3_TxBuf, _txbuf, _txlen);    
        spi3_len = _txlen + _rxlen;
        bsp_spiTransfer(_spi);
    
        memcpy(_rxbuf, &spi3_RxBuf[_txlen], _rxlen);
    }
    #endif    

    #if USE_SPI4_EN == 1
    if (_spi == SPI4 && _txlen + _rxlen < SPI4_BUFFER_SIZE)
    {
        memset(spi4_TxBuf, 0, _txlen + _rxlen);
        memcpy(spi4_TxBuf, _txbuf, _txlen);    
        spi4_len = _txlen + _rxlen;
        bsp_spiTransfer(_spi);
    
        memcpy(_rxbuf, &spi4_RxBuf[_txlen], _rxlen);
    }
    #endif    

    #if USE_SPI5_EN == 1
    if (_spi == SPI5 && _txlen + _rxlen < SPI5_BUFFER_SIZE)
    {
        memset(spi5_TxBuf, 0, _txlen + _rxlen);
        memcpy(spi5_TxBuf, _txbuf, _txlen);    
        spi5_len = _txlen + _rxlen;
        bsp_spiTransfer(_spi);
    
        memcpy(_rxbuf, &spi5_RxBuf[_txlen], _rxlen);
    }
    #endif    
    
    #if USE_SPI6_EN == 1
    if (_spi == SPI6 && _txlen + _rxlen < SPI6_BUFFER_SIZE)
    {
        memset(spi6_TxBuf, 0, _txlen + _rxlen);
        memcpy(spi6_TxBuf, _txbuf, _txlen);    
        spi6_len = _txlen + _rxlen;
        bsp_spiTransfer(_spi);
    
        memcpy(_rxbuf, &spi6_RxBuf[_txlen], _rxlen);
    }
    #endif
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SpiBusEnter
*    功能说明: 占用SPI总线
*    形    参: 无
*    返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_SpiBusEnter(void)
{
//    g_spi_busy = 1;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SpiBusExit
*    功能说明: 释放占用的SPI总线
*    形    参: 无
*    返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_SpiBusExit(void)
{
//    g_spi_busy = 0;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SpiBusBusy
*    功能说明: 判断SPI总线忙。方法是检测其他SPI芯片的片选信号是否为1
*    形    参: 无
*    返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
uint8_t bsp_SpiBusBusy(void)
{
//    return g_spi_busy;
    return 0;
}


#ifdef USE_SPI_INT
    /**
      * @brief  This function handles SPIx interrupt request.
      * @param  None
      * @retval None
      */
    void SPIx_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&hspi);
    }    
#endif

#ifdef USE_SPI_DMA
    /**
      * @brief  This function handles DMA Rx interrupt request.
      * @param  None
      * @retval None
      */
    void SPIx_DMA_RX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(hspi.hdmarx);
    }

    /**
      * @brief  This function handles DMA Tx interrupt request.
      * @param  None
      * @retval None
      */
    void SPIx_DMA_TX_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(hspi.hdmatx);
    }
    
    /**
      * @brief  This function handles SPIx interrupt request.
      * @param  None
      * @retval None
      */
    void SPIx_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&hspi);
    }    
#endif
    
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
