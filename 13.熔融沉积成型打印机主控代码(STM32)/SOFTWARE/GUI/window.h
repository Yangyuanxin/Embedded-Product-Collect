#ifndef __WINDOW_H
#define __WINDOW_H 			 
#include "gui.h"  
#include "button.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-窗体 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//升级说明 
//V1.1 20140815
//1,修正window_creat函数和window_delete函数的小bug
//2,修改window_msg_box函数,新增时间参数
//////////////////////////////////////////////////////////////////////////////////

//默认的窗体caption背景色caption的颜色.
//以及窗体背景色.
#define WIN_CAPTION_UPC	   	  	0X5CF4//0X7D7C	//窗口CAPTION上部分填充色
#define WIN_CAPTION_DOWNC				0X5CF4//0X5458	//窗口CAPTION下部分填充色
#define WIN_CAPTION_COLOR				0XFFFF	//窗口名颜色为白色
#define WIN_BODY_BKC					  0XD699	//窗口底色,0XEF5D

#define WIN_CAPTION_HEIGHT				0X20	//默认的高度

//窗体上的关闭按钮相关信息定义
#define WIN_BTN_SIZE					0X18	//正方形尺 寸边长(必须小于WIN_CAPTION_HEIGHT)
#define WIN_BTN_OFFSIDE					0X06	//按钮/标题偏离边框的像素

#define WIN_BTN_FUPC					0XFFFF	//按钮松开时文字的颜色
#define WIN_BTN_FDOWNC				0XDCD0	//按钮按下时文字的颜色
#define WIN_BTN_RIMC					0XFE18	//边框颜色
#define WIN_BTN_TP1C					0XF36D	//第一行的颜色
#define WIN_BTN_UPHC					0XF36D	//上半部分颜色,0X6D3C
#define WIN_BTN_DNHC					0XF36D//0XC800	//下半部分颜色,0XAE5C



//标准窗体边框颜色定义
#define STD_WIN_RIM_OUTC 	0X3317	    //外线颜色
#define STD_WIN_RIM_MIDC 	0X859B		//中线颜色
#define STD_WIN_RIM_INC 	0X2212		//内线颜色	    		   
//圆边窗体边框颜色定义
#define ARC_WIN_RIM_COLOR 	0XB595	    //边线颜色  


//window结构体定义
__packed typedef struct 
{
	u16 top; 				  		//window顶端坐标
	u16 left;                       //window左端坐标
	u16 width; 				  		//window宽度(包含滚动条的宽度)
	u16 height;						//window高度

	u8 id;							//window
	u8 type;						//window类型
									//[7]:0,没有关闭按钮.1,有关闭按钮			   
									//[6]:0,不读取背景色.1,读取背景色.					 
									//[5]:0,标题靠左.1,标题居中.					 
									//[4:2]:保留					 
									//[1:0]:0,标准的窗口(仿XP);1,圆边窗口(仿Andriod)			   

	u8 sta;							//window状态
 									//[7:0]:保留

	u8 *caption;					//window名字
	u8 captionheight;				//caption栏的高度
	u8 font;						//window文字字体
	u8 arcwinr;						//圆角窗口的圆角的半径	
										
	u16 captionbkcu; 				//caption的上半部分背景色
	u16 captionbkcd; 				//caption的下半部分背景色
 	u16 captioncolor; 				//caption的颜色
	u16 windowbkc; 					//window的背景色

	u16 *bkctbl;					//背景色表(需要读取窗体背景色的时候用到)
	_btn_obj* closebtn;				//串口关闭按钮
}_window_obj;

_window_obj * window_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font);
void window_delete(_window_obj * window_del);
void window_draw(_window_obj * windowx);
u8 window_msg_box(u16 x,u16 y,u16 width,u16 height,u8 *str,u8 *caption,u8 font,u16 color,u8 mode,u16 time);




void win_test(u16 x,u16 y,u16 width,u16 height,u8 type,u16 cup,u16 cdown,u8 *caption);
#endif

















