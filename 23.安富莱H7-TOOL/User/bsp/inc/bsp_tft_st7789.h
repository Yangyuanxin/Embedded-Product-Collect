/*
*********************************************************************************************************
*
*    模块名称 : 驱动液晶 1.3寸IPS屏
*    文件名称 : bsp_tft_st7789.h
*    版    本 : V2.0
*    说    明 : 头文件
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_TFT_ST7789_H
#define _BSP_TFT_ST7789_H

void ST7789_InitHard(void);
void ST7789_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth);
void ST7789_QuitWinMode(void);
void ST7789_DispOn(void);
void ST7789_DispOff(void);
void ST7789_ClrScr(uint16_t _usColor);
void ST7789_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t ST7789_GetPixel(uint16_t _usX, uint16_t _usY);
void ST7789_DrawLine(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor);
void ST7789_DrawHLine(uint16_t _usX, uint16_t _usY, uint16_t _usLen, uint16_t _usColor);
void ST7789_DrawVLine(uint16_t _usX, uint16_t _usY, uint16_t _usLen, uint16_t _usColor);
void ST7789_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor);
void ST7789_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void ST7789_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void ST7789_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void ST7789_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
void ST7789_DrawBMP8(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t *_ptr);
void ST7789_SetDirection(uint8_t _dir);
void ST7789_GetChipDescribe(char *_str);
void ST7789_WriteData16(uint16_t data2);

void ST7789_DrawScreen(void);

#endif
