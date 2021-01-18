/*
*********************************************************************************************************
*
*    模块名称 : SPI总线驱动（快速模式）
*    文件名称 : bsp_spi_fast.h
*    版    本 : V1.2
*    说    明 : SPI总线底层驱动。不用HAL库，直接寄存器操作提高效率。仅仅用于SWD接口
*    修改记录 :
*        版本号  日期        作者    说明
*       v1.0    2019-08-23  armfly  首版。

*    Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/************************* SPI2 *********************/

#define SPI2_SCK_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define SPI2_SCK_GPIO GPIOD
#define SPI2_SCK_PIN GPIO_PIN_3
#define SPI2_SCK_AF GPIO_AF5_SPI2

//#define SPI2_MISO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
//#define SPI2_MISO_GPIO                GPIOB
//#define SPI2_MISO_PIN                 GPIO_PIN_4
//#define SPI2_MISO_AF                GPIO_AF5_SPI2

#define SPI2_MOSI_CLK_ENABLE() __HAL_RCC_GPIOI_CLK_ENABLE()
#define SPI2_MOSI_GPIO GPIOI
#define SPI2_MOSI_PIN GPIO_PIN_3
#define SPI2_MOSI_AF GPIO_AF5_SPI2

#define SET_SPI_TX_MODE(datasize)                         \
    SPI2->CR2 = 1;                                          \
    SPI2->CR1 = SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_CSTART; \
    while ((SPI2->SR & SPI_FLAG_TXE) == 0)                  \
        ;                                                     \
    SPI2->TXDR = 0X125A;                                    \
    while ((SPI2->SR & SPI_SR_TXC) == 0)                    \
        ;                                                     \
    SPI2->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;            \
    SPI2->CR1 &= ~(SPI_CR1_SPE);

#define SET_SPI_RX_MODE(datasize)                                      \
    SPI2->CR1 &= ~(SPI_CR1_SPE);                                         \
    SPI2->CFG1 = SPI_BAUDRATEPRESCALER_2 | SPI_DATASIZE_8BIT;            \
    SPI2->CR1 &= ~(SPI_CR1_HDDIR); /* 0 表示接收， 1表示发送 */ \
    SPI2->CR1 |= SPI_CR1_SPE;             /* 使能SPI */

extern SPI_HandleTypeDef hspi2;
void SWD_SendBitsOK(uint8_t _bits, uint32_t _data)
{
#if 1
    //static uint8_t s_first = 1;

    //    if (s_first == 0)
    //    {
    //        while ((SPI2->SR & SPI_SR_TXC) == 0);                //while ((SPI2->SR & SPI_FLAG_EOT) == 0); SPI_SR_TXC
    //    }
    //    else
    //    {
    //        s_first = 0;
    //    }
    //    SPI2->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;
    //    SPI2->CR1 &= ~(SPI_CR1_SPE);

    //SPI2->CR1 |= SPI_CR1_HDDIR;    /* 0 表示接收， 1表示发送 */
    _bits--;

    SPI2->CFG1 = SPI_BAUDRATEPRESCALER_8 | _bits;
    SPI2->CR1 = SPI_CR1_SSI | SPI_CR1_HDDIR;
    SPI2->CR2 = 1;
    SPI2->CR1 = SPI_CR1_SPE | SPI_CR1_HDDIR | SPI_CR1_SSI;

    SPI2->CR1 = SPI_CR1_SPE | SPI_CR1_HDDIR | SPI_CR1_SSI | SPI_CR1_CSTART;

    while ((SPI2->SR & SPI_FLAG_TXE) == 0)
        ;

    if (_bits > 15)
    {
        *((__IO uint32_t *)&SPI2->TXDR) = _data;
    }
    else if (_bits > 7)
    {
        *((__IO uint16_t *)&SPI2->TXDR) = _data;
    }
    else
    {
        *((__IO uint8_t *)&SPI2->TXDR) = _data;
    }

    while ((SPI2->SR & SPI_SR_TXC) == 0)
        ;
    SPI2->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

    SPI2->CR1 &= ~(SPI_CR1_SPE);
#else
    HAL_SPI_Transmit(&hspi2, "123", 1, 1000);
#endif
}

uint32_t SWD_ReadBitsOK(uint8_t _bits)
{

    _bits--;

    SPI2->CFG1 = SPI_BAUDRATEPRESCALER_8 | _bits;
    SPI2->CR1 = SPI_CR1_SSI;
    SPI2->CR2 = 1;
    SPI2->CR1 = SPI_CR1_SPE | SPI_CR1_SSI;

    SPI2->CR1 = SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_CSTART;

    while ((SPI2->SR & SPI_FLAG_TXE) == 0)
        ;

    if (_bits > 15)
    {
        *((__IO uint32_t *)&SPI2->TXDR) = 0;
    }
    else if (_bits > 7)
    {
        *((__IO uint16_t *)&SPI2->TXDR) = 0;
    }
    else
    {
        *((__IO uint8_t *)&SPI2->TXDR) = 0;
    }

    while ((SPI2->SR & SPI_SR_TXC) == 0)
        ;
    SPI2->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

    SPI2->CR1 &= ~(SPI_CR1_SPE);

    return SPI2->RXDR;
}

void bsp_InitSPIParamFast(void)
{
    SPI_HandleTypeDef hspi2;

    hspi2.Instance = SPI2;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 7;
    hspi2.Init.CRCLength = SPI_CRC_LENGTH_8BIT;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE; /* Recommanded setting to avoid glitches */
    hspi2.Init.Mode = SPI_MODE_MASTER;

    HAL_SPI_Init(&hspi2);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitSPI2_Fast
*    功能说明: 配置SPI总线。 
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPI2_Fast(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 配置GPIO时钟 */
    SPI2_SCK_CLK_ENABLE();

    SPI2_MOSI_CLK_ENABLE();

    /* 使能SPI时钟 */
    __HAL_RCC_SPI2_CLK_ENABLE();

    /* 配置GPIO为SPI功能 */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP; /* 上拉 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = SPI2_SCK_AF;

    GPIO_InitStruct.Pin = SPI2_SCK_PIN;
    HAL_GPIO_Init(SPI2_SCK_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI2_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPI2_MOSI_AF;
    HAL_GPIO_Init(SPI2_MOSI_GPIO, &GPIO_InitStruct);

    bsp_InitSPIParam(SPI2, SPI_BAUDRATEPRESCALER_256, SPI_PHASE_2EDGE, SPI_POLARITY_HIGH);

    SPI2->CR2 = 1;
    //bsp_InitSPIParamFast();

    //    SPI2->CR1 &= ~(SPI_CR1_SPE);
    //    SPI2->CFG1 = SPI_BAUDRATEPRESCALER_64 | SPI_DATASIZE_8BIT;
    //    SPI2->CFG2 = SPI_DIRECTION_2LINES | SPI_MODE_MASTER | SPI_FIRSTBIT_LSB | SPI_PHASE_1EDGE | SPI_POLARITY_HIGH | SPI_MASTER_KEEP_IO_STATE_ENABLE;
    //    //SPI2->CR1 &= ~(SPI_CR1_HDDIR);        /* 0 表示接收， 1表示发送 */
    //    SPI2->CR1 |= SPI_CR1_HDDIR;            /* 0 表示接收， 1表示发送 */
    //    SPI2->CR1 |= SPI_CR1_SPE;        /* 使能SPI */
    //                SPI2->CR1 &= ~(SPI_CR1_SPE);
    //        SPI2->CFG1 = SPI_BAUDRATEPRESCALER_64 | (datasize - 1);

    //    SPI2->CR2 = 2;
    //    SPI2->CR1 |= SPI_CR1_SPE ;    /* 使能SPI */
    //    while ((SPI2->SR & SPI_FLAG_TXE) == 0);
    //    SPI2->CR1 = SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_CSTART;
    //    while(1)
    //    {
    //        static uint32_t rx = 0;
    ////
    //        SWD_SendBits(4, 5);
    //        SWD_SendBits(16, 5);
    //        SWD_SendBits(16, 5);
    //        SWD_SendBits(16, 5);

    ////        rx = SWD_ReadBits(8);
    ////        rx = SWD_ReadBits(12);
    ////        rx = SWD_ReadBits(32);
    ////        HAL_SPI_Transmit(&hspi2, "U", 1, 1000);
    ////        HAL_SPI_Transmit(&hspi2, "U", 1, 1000);
    ////        HAL_SPI_Transmit(&hspi2, "U", 1, 1000);
    //
    //        bsp_DelayUS(2000);
    //    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitSPIParam
*    功能说明: 配置SPI总线参数，波特率、
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPI2_TxMode(void)
{
    /* Disable the selected SPI peripheral */
    //__HAL_SPI_DISABLE(hspi);
    SPI2->CR1 &= ~(SPI_CR1_SPE);

    /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
    /* Configure : SPI Mode, Communication Mode, Clock polarity and phase, NSS management,
    Communication speed, First bit, CRC calculation state, CRC Length */

    /* SPIx CFG1 Configuration */
    //  WRITE_REG(hspi->Instance->CFG1, (hspi->Init.BaudRatePrescaler | hspi->Init.CRCCalculation | crc_length |
    //                                   hspi->Init.FifoThreshold     | hspi->Init.DataSize));
    SPI2->CFG1 = SPI_BAUDRATEPRESCALER_2 | SPI_DATASIZE_8BIT;

    /* SPIx CFG2 Configuration */
    //    WRITE_REG(hspi->Instance->CFG2, (hspi->Init.NSSPMode   | hspi->Init.TIMode           | hspi->Init.NSSPolarity             |
    //                                   hspi->Init.NSS          | hspi->Init.CLKPolarity      | hspi->Init.CLKPhase                |
    //                                   hspi->Init.FirstBit     | hspi->Init.Mode             | hspi->Init.MasterInterDataIdleness |
    //                                   hspi->Init.Direction    | hspi->Init.MasterSSIdleness | hspi->Init.IOSwap));
    SPI2->CFG2 = SPI_DIRECTION_1LINE | SPI_MODE_MASTER | SPI_PHASE_1EDGE | SPI_POLARITY_HIGH | SPI_MASTER_KEEP_IO_STATE_ENABLE;

    //SPI2->CR1 &= ~(SPI_CR1_HDDIR);        /* 0 表示接收， 1表示发送 */
    SPI2->CR1 |= SPI_CR1_HDDIR; /* 0 表示接收， 1表示发送 */

    SPI2->CR1 |= SPI_CR1_SPE; /* 使能SPI */
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
