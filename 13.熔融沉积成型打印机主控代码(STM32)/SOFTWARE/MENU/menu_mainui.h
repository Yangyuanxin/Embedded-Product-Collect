#ifndef __MENU_MAINUI_H
#define __MENU_MAINUI_H

//#include "dma.h"
#include "lcd.h"
#include "delay.h"
#include "malloc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//主界面控制 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/2/22
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   
  
#define MUI_BACK_COLOR		LIGHTBLUE		//MUI选择图标底色
#define MUI_FONT_COLOR 		BLUE 			//MUI字体颜色

#define MUI_EX_BACKCOLOR	0X0000			//窗体外部背景色
#define MUI_IN_BACKCOLOR	0X8C51			//窗体内部背景色

////////////////////////////////////////////////////////////////////////////////////////////
//各图标/图片路径
extern  u8 *const mui_icos_path_tbl[6]; //图标路径表
////////////////////////////////////////////////////////////////////////////////////////////
//每个图标的参数信息  //图标的属性，可以理解为c语言中面向对象的思想。
__packed typedef struct _m_mui_icos
{										    
	u16 x;			//图标坐标及尺寸
	u16 y;
	u8 width;
	u8 height; 
	u8 * path;		//图标路径
	u8 * name;		//图标名字
}m_mui_icos;

//主界面 控制器
typedef struct _m_mui_dev
{					
	u16 tpx;			//触摸屏最近一次的X坐标
	u16 tpy;			//触摸屏最近一次的Y坐标
	u8 status;			//当前选中状态.
						//bit7:0,没有有效触摸;1,有有效触摸.
						//bit6~5:保留
						//bit4~0:0~8,被选中的图标编号;0XF,没有选中任何图标  
	m_mui_icos icos[6];	//1页,共6个图标
}m_mui_dev;
extern m_mui_dev muidev;

void mui_init(uint8_t mode);
void mui_load_icos(void);
void mui_language_set(u8 language);
void mui_set_sel(u8 sel); 
u8 mui_touch_chk(void);

#endif






















 




