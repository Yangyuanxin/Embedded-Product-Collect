/*
*********************************************************************************************************
*
*    模块名称 : 外部SDRAM驱动模块
*    文件名称 : bsp_fmc_sdram.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2014-05-04 armfly  ST固件库版本 V1.3.0
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_FMC_SDRAM_H
#define _BSP_FMC_SDRAM_H

#define EXT_SDRAM_ADDR ((uint32_t)0xC0000000)
#define EXT_SDRAM_SIZE (32 * 1024 * 1024)

/* LCD显存,第1页, 分配2M字节 */
#define SDRAM_LCD_BUF1 EXT_SDRAM_ADDR

/* LCD显存,第2页, 分配2M字节 */
#define SDRAM_LCD_BUF2 (EXT_SDRAM_ADDR + SDRAM_LCD_SIZE)

#define SDRAM_LCD_SIZE (2 * 1024 * 1024) /* 每层2M */
#define SDRAM_LCD_LAYER 2                /* 2层 */

/* 剩下的12M字节，提供给应用程序使用 */
#define SDRAM_APP_BUF (EXT_SDRAM_ADDR + SDRAM_LCD_SIZE * SDRAM_LCD_LAYER)
#define SDRAM_APP_SIZE (EXT_SDRAM_SIZE - SDRAM_LCD_SIZE * SDRAM_LCD_LAYER)

void bsp_InitExtSDRAM(void);
uint32_t bsp_TestExtSDRAM1(void);
uint32_t bsp_TestExtSDRAM2(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
