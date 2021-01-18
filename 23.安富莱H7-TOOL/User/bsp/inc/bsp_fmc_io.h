/*
*********************************************************************************************************
*
*    模块名称 : H7-TOOL 扩展IO驱动程序
*    文件名称 : bsp_fmc_io.h
*    说    明 :
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#ifndef __BSP_FMC_IO_H
#define __BSP_FMC_IO_H

#define EIO_READ_FMC() *(uint16_t *)0x60000000

/* 供外部调用的函数声明 */
enum
{
    EIO_D0 = 0,
    EIO_D1,
    EIO_D2,
    EIO_D3,
    EIO_D4,
    EIO_D5,
    EIO_D6,
    EIO_D7,

    EIO_D8,
    EIO_D9,

    EIO_D10, /* TTL_TX */
    EIO_D11, /* TTL_RX */

    EIO_D12, /* CAM_TX */
    EIO_D13, /* CAM_RX */

    EIO_485_TXEN = 100, /* RS485方向控制 */
};

/* IO功能选择 */
typedef enum
{
    ES_GPIO_IN = 0,     /* GPIO 输入， FMC输入 */
    ES_GPIO_OUT = 1,    /* GPIO 输出， FMC输入 */
    ES_FMC_OUT = 2,     /* GPIO 输入， FMC输出 */

    ES_FMC_NOE = 3,     /* D8专用，FMC_D8和FMC_NOE并联 */
    ES_FMC_NWE = 4,     /* D9专用，FMC_D9和FMC_NWE并联 */

    ES_GPIO_UART = 5,   /* 作为GPIO串口功能 */
    ES_GPIO_CAN = 6,    /* CAN功能 */
    
    ES_GPIO_I2C = 7,    /* I2C功能 */
    
    ES_GPIO_SWD_OUT = 8,    /* 1拖4 SWD接口用 */
    
    ES_GPIO_SPI = 9,        /* 硬件SPI功能 */
    ES_PROG_SPI_FLASH = 10, /* 脱机烧录SPI FLASH */
} EIO_SELECT_E;

void bsp_InitExtIO(void);
void EIO_ConfigPort(uint8_t _eio, EIO_SELECT_E _mode);
void EIO_SetOutLevel(uint8_t _eio, uint8_t _level);
uint8_t EIO_GetInputLevel(uint8_t _eio);
uint8_t EIO_GetOutLevel(uint8_t _eio);

void EIO_D0_Config(EIO_SELECT_E _mode);
void EIO_D1_Config(EIO_SELECT_E _mode);
void EIO_D2_Config(EIO_SELECT_E _mode);
void EIO_D3_Config(EIO_SELECT_E _mode);
void EIO_D4_Config(EIO_SELECT_E _mode);
void EIO_D5_Config(EIO_SELECT_E _mode);
void EIO_D6_Config(EIO_SELECT_E _mode);
void EIO_D7_Config(EIO_SELECT_E _mode);
void EIO_D8_Config(EIO_SELECT_E _mode);
void EIO_D9_Config(EIO_SELECT_E _mode);
void EIO_D10_Config(EIO_SELECT_E _mode);
void EIO_D11_Config(EIO_SELECT_E _mode);
void EIO_D12_Config(EIO_SELECT_E _mode);
void EIO_D13_Config(EIO_SELECT_E _mode);

uint16_t EIO_ReadFMC(void);

void BSP_CFG_GPIO_OUT(GPIO_TypeDef* GPIOx, uint16_t pin);
void BSP_CFG_GPIO_IN(GPIO_TypeDef* GPIOx, uint16_t pin);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
