/*                                        
*********************************************************************************************************
*                                      
*    模块名称 : 字库模块
*    文件名称 : fonts.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __FONTS_H_
#define __FONTS_H_

#define USE_SMALL_FONT /* 定义此行表示使用小字库， 这个宏只在bsp_tft+lcd.c中使用 */

/* QSPI Flash 中2MB字库的地址分配 0x91E00000 */
//#define QSPI_ADDR_HZK   (0x90000000 + 30 * 1024 * 1024)
#define QSPI_ADDR_HZK   (30 * 1024 * 1024)
#define HZK12_ADDR (QSPI_ADDR_HZK + 0x0)     /* 12点阵汉字库地址 */
#define HZK16_ADDR (QSPI_ADDR_HZK + 0x2C9D0) /* 16点阵汉字库地址 */
#define HZK24_ADDR (QSPI_ADDR_HZK + 0x68190) /* 24点阵汉字库地址 */
#define HZK32_ADDR (QSPI_ADDR_HZK + 0xEDF00) /* 32点阵汉字库地址 */

extern unsigned char const g_Ascii12[];
extern unsigned char const g_Ascii16[];
extern unsigned char const g_Ascii24[];
extern unsigned char const g_Ascii32[];

extern unsigned char const g_Ascii24_VarWidth[];
extern unsigned char const g_Ascii32_VarWidth[];

extern unsigned char const g_Ascii62x40[]; /* 62 * 40 ASCII,部分字符 */
extern unsigned char const g_Ascii96x40[]; /* 96 * 40 ASCII,部分字符 */

extern unsigned char const g_Hz12[];
extern unsigned char const g_Hz16[];
extern unsigned char const g_Hz24[];
extern unsigned char const g_Hz32[];

extern unsigned char const g_RA8875_Ascii12_width[];
extern unsigned char const g_RA8875_Ascii16_width[];
extern unsigned char const g_RA8875_Ascii24_width[];
extern unsigned char const g_RA8875_Ascii32_width[];

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
