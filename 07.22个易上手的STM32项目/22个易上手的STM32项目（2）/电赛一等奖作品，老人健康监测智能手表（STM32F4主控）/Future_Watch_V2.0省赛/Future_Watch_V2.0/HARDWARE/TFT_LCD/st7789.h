/*
**************************************************************
*
* 文件名称：st7789.h;
*
* 版本说明：V1.1；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：无；
*           
* 完成日期：2016/06/25;
*
* 作者姓名：郑子烁;
*
* 其他备注：1.54寸液晶屏驱动程序； 80并口，驱动芯片st7789；
*          
* 修改说明：2016/6/25 增加读点函数；
*
**************************************************************
*/


#ifndef __ST7789_H__
#define __ST7789_H__


#include "sys.h"
#include "delay.h"
#include "gpio.h"


//#define CS1  PCout(6)  
//#define RS   PCout(7)  
//#define WRD  PCout(8)  
//#define RDB  PCout(9)  

#define CS1  PBout(9)  
#define RS   PBout(8)  
#define WRD  PBout(7)  
#define RDB  PCout(3) 


//#define DATA_OUT(x) GPIOF->ODR =x
//#define DATA_READ() (GPIOF->IDR)//&0x000000ff
#define DATA_OUT(x) GPIOA->ODR =x
#define DATA_READ() (GPIOA->IDR)//&0x000000ff

///*******直接操作GPIO清除寄存器*******/
//#define LCD_CS_SET GPIOC->BSRR |=1<<6                              //片选端；
//#define LCD_RS_SET GPIOC->BSRR |=1<<7                              //命令\数据； (0/1)
//#define LCD_WR_SET GPIOC->BSRR |=1<<8                              //写入；      (0->1有效动作/1无效)
//#define LCD_RD_SET GPIOC->BSRR |=1<<9                              //读出；      (0->1有效动作/1无效)
//////#define LCD_RESET_SET                                           //液晶复位，如果有接入；
///*******直接操作GPIO清除寄存器*******/
#define LCD_CS_SET GPIOB->BSRRL |=1<<9                              //片选端；
#define LCD_RS_SET GPIOB->BSRRL |=1<<8                              //命令\数据； (0/1)
#define LCD_WR_SET GPIOB->BSRRL |=1<<7                              //写入；      (0->1有效动作/1无效)
#define LCD_RD_SET GPIOC->BSRRL |=1<<3                              //读出；      (0->1有效动作/1无效)


//#define LCD_CS_CLR GPIOC->BSRR  |=1<<22                              //片选端；
//#define LCD_RS_CLR GPIOC->BSRR  |=1<<23                              //命令\数据；
//#define LCD_WR_CLR GPIOC->BSRR  |=1<<24                              //写入；
//#define LCD_RD_CLR GPIOC->BSRR  |=1<<25                              //读出；
#define LCD_CS_CLR GPIOB->BSRRH |=1<<9                              //片选端；
#define LCD_RS_CLR GPIOB->BSRRH |=1<<8                             //命令\数据；
#define LCD_WR_CLR GPIOB->BSRRH |=1<<7                             //写入；
#define LCD_RD_CLR GPIOC->BSRRH |=1<<3                             //读出；
#define LCD_RESET_CLR                                           //液晶复位，如果有接入；


/****LCD参数管理器****/
typedef struct
{
	u8 with;         //液晶屏宽度；
	u8 height;       //液晶屏长度；
	u8 wramcmd;      //开始写gram命令；
	u8 setxcmd;      //设置x坐标；
	u8 setycmd;      //设置y坐标；
	u16 color;       //画笔颜色；
}_lcd_dev;

extern _lcd_dev lcd;


/**********函数声明***********/

/*
* 函数名称：Init_ST7789；
* 功能说明：st7789初始化；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/
void Init_ST7789(void);


/*
* 函数名称：Write_ST7789_Reg；
* 功能说明：写命令；
* 入口参数：hibyte->命令；
* 返回值  ：无；
* 备注    ：无；
*/
void Write_ST7789_Reg(u8 hibyte);
 

/*
* 函数名称：Write_ST7789_Data；
* 功能说明：写数据；
* 入口参数：hibyte->数据；
* 返回值  ：无；
* 备注    ：无；
*/
void Write_ST7789_Data(u8 hibyte);


/*
* 函数名称：Read_ST7789_Data；
* 功能说明：读数据；
* 入口参数：无；
* 返回值  ：8位数据；
* 备注    ：无；
*/
u8 Read_ST7789_Data(void);


/*
* 函数名称：Address_Set；
* 功能说明：坐标设置；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/
void  Address_Set(u16 x,u16 y);


/*
* 函数名称：Write_ST7789_16Bit；
* 功能说明：写16位颜色数据；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/
void Write_ST7789_16Bit(u8 hibyte,u8 lowbyte);


/*
* 函数名称：Write_ST7789_Point；
* 功能说明：写点的颜色数据；
* 入口参数：目标坐标；
* 返回值  ：16位颜色值；
* 备注    ：无；
*/
void Write_ST7789_Point(u16 x,u16 y,u16 color);


/*
* 函数名称：Read_ST7789_Point；
* 功能说明：读点的颜色数据；
* 入口参数：目标坐标；
* 返回值  ：16位颜色值；
* 备注    ：无；
*/
u16 Read_ST7789_Point(u16 x,u16 y);

void Draw_Background(u8 x_s,u8 y_s,u8 length,u8 wide,u16 color);
u8 Read_LCD_Reg(u8 reg);

#endif
