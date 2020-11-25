/**
  ******************************************************************************
  * @file    lcd12864.h
  * @author  xiaoyuan
  * @version V1.0
  * @date    2016-04-12
  * @brief   
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD12864_H
#define __LCD12864_H	

#include "stm32f10x.h"

#define PIN_CS  GPIO_Pin_12     //RS(CS)并行的指令/数据选择信号。串行的片选信号。
#define PIN_SID GPIO_Pin_13    //R/W（SID）并行的读写选择信号。串行的数据口
#define PIN_CLK GPIO_Pin_14    //E（CLK）并行的使能信号。串行的同步时钟。
#define PIN_PSB GPIO_Pin_15   // 并/串行接口选择：H并行，L串行
#define PIN_RST GPIO_Pin_4

#define SET_CS      GPIO_SetBits(GPIOE,PIN_CS)
#define RESET_CS    GPIO_ResetBits(GPIOE,PIN_CS)
#define SET_SID     GPIO_SetBits(GPIOE,PIN_SID)
#define RESET_SID   GPIO_ResetBits(GPIOE,PIN_SID)
#define SET_SCLK     GPIO_SetBits(GPIOE,PIN_CLK)
#define RESET_SCLK   GPIO_ResetBits(GPIOE,PIN_CLK)
#define SET_PSB     GPIO_SetBits(GPIOE,PIN_PSB)
#define RESET_PSB   GPIO_ResetBits(GPIOE,PIN_PSB)
#define SET_RST     GPIO_SetBits(GPIOE,PIN_RST)
#define RESET_RST   GPIO_ResetBits(GPIOE,PIN_RST)

#define LCD12864_MODE_CMD  (uint32_t)0xf8 //串行 写入命令要先写入0xf8
#define LCD12864_MODE_DTAE (uint32_t)0xfa //串行 写入数据要先写入0xfa


void LCD12864_Init(void);
void LCD12864_WriteByte(unsigned char byte);
void LCD12864_WriteData(uint8_t data) ; 
void LCD12864_WriteCmd(uint8_t cmd) ; 
void LCD12864_Clear(void);
void LCD12864_ClearGraphMemory(void);
void LCD12864_HightlightShow (unsigned char CX, unsigned char CY, unsigned char width,unsigned char f);
void LCD12864_ShowString(uint8_t row,uint8_t col,uint8_t *data1);
void LCD12864_ShowNum(uint8_t row,uint8_t col,uint16_t num);
void LCD12864_ShowFloat(uint8_t row,uint8_t col,float fnum);
#endif

