/*
*********************************************************************************************************
*
*    模块名称 : 串行EEPROM 24xx02驱动模块
*    文件名称 : bsp_eeprom_24xx.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H

/* H7-TOOL安装的是24C16 (2K字节） */

//#define AT24C02
//#define AT24C04
#define AT24C16
//#define AT24C128

#ifdef AT24C02
#define EE_MODEL_NAME "AT24C02"
#define EE_DEV_ADDR 0xA0 /* 设备地址 */
#define EE_PAGE_SIZE 8     /* 页面大小(字节) */
#define EE_SIZE 256             /* 总容量(字节) */
#define EE_ADDR_BYTES 1    /* 地址字节个数 */
#endif

#ifdef AT24C04
#define EE_MODEL_NAME "AT24C04"
#define EE_DEV_ADDR 0xA0 /* 设备地址 */
#define EE_PAGE_SIZE 16    /* 页面大小(字节) */
#define EE_SIZE 512             /* 总容量(字节) */
#define EE_ADDR_BYTES 1    /* 地址字节个数 */
#endif

#ifdef AT24C16
#define EE_MODEL_NAME "AT24C16"
#define EE_DEV_ADDR 0xA0 /* 设备地址 */
#define EE_PAGE_SIZE 16    /* 页面大小(字节) */
#define EE_SIZE 2048         /* 总容量(字节) */
#define EE_ADDR_BYTES 1    /* 地址字节个数 */
#endif

#ifdef AT24C128
#define EE_MODEL_NAME "AT24C128"
#define EE_DEV_ADDR 0xA0        /* 设备地址 */
#define EE_PAGE_SIZE 64            /* 页面大小(字节) */
#define EE_SIZE (16 * 1024) /* 总容量(字节) */
#define EE_ADDR_BYTES 2            /* 地址字节个数 */
#endif

uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
