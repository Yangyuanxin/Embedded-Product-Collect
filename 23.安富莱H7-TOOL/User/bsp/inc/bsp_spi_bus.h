/*
*********************************************************************************************************
*
*    模块名称 : SPI总线驱动
*    文件名称 : bsp_spi_bus.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_SPI_BUS_H
#define __BSP_SPI_BUS_H

#define USE_SPI1_EN 0
#define USE_SPI2_EN 1
#define USE_SPI3_EN 1
#define USE_SPI4_EN 1
#define USE_SPI5_EN 1
#define USE_SPI6_EN 0

#define SPI1_BUFFER_SIZE (4 * 1024)
#define SPI2_BUFFER_SIZE (4 * 1024)
#define SPI3_BUFFER_SIZE (4 * 1024)
#define SPI4_BUFFER_SIZE (4 * 1024)
#define SPI5_BUFFER_SIZE (4 * 1024)
#define SPI6_BUFFER_SIZE (4 * 1024)

/* 重定义下SPI SCK时钟，方便移植 */
#define SPI_BAUDRATEPRESCALER_50M       SPI_BAUDRATEPRESCALER_2     /* 50M */
#define SPI_BAUDRATEPRESCALER_25M       SPI_BAUDRATEPRESCALER_4     /* 25M */
#define SPI_BAUDRATEPRESCALER_12_5M     SPI_BAUDRATEPRESCALER_8     /* 12.5M */
#define SPI_BAUDRATEPRESCALER_6_25M     SPI_BAUDRATEPRESCALER_16    /* 6.25M */
#define SPI_BAUDRATEPRESCALER_3_125M    SPI_BAUDRATEPRESCALER_32    /* 3.125M */
#define SPI_BAUDRATEPRESCALER_1_5625M   SPI_BAUDRATEPRESCALER_64    /* 1.5625M */
#define SPI_BAUDRATEPRESCALER_781_25K   SPI_BAUDRATEPRESCALER_128   /* 781.25K */
#define SPI_BAUDRATEPRESCALER_390_625K  SPI_BAUDRATEPRESCALER_256   /* 390.625K */

void bsp_InitSPIBus(void);
void bsp_spiTransfer(SPI_TypeDef *_spi);
void bsp_InitSPIParam(SPI_TypeDef *_spi, uint32_t _BaudRatePrescaler, uint32_t _CLKPhase, uint32_t _CLKPolarity);

void bsp_SpiBusEnter(void);
void bsp_SpiBusExit(void);
uint8_t bsp_SpiBusBusy(void);

void bsp_SpiSendBuf(SPI_TypeDef *_spi, const char *_txbuf, uint32_t _txlen);
void bsp_SpiReciveBuf(SPI_TypeDef *_spi, char *_rxbuf, uint32_t _rxlen);    
void bsp_SpiSendRecive(SPI_TypeDef *_spi, const char *_txbuf, uint32_t _txlen, char *_rxbuf, uint32_t _rxlen);

//extern uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];
//extern uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];
//extern uint32_t g_spiLen;

//extern uint8_t g_spi_busy;

extern uint32_t spi2_len;
extern uint8_t spi2_TxBuf[SPI2_BUFFER_SIZE];
extern ALIGN_32BYTES(uint8_t spi2_RxBuf[SPI2_BUFFER_SIZE]); /* 必须32字节对齐 */

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
