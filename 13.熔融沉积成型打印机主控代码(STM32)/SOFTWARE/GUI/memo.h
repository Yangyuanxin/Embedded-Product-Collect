#ifndef __MEMO_H
#define __MEMO_H 			 
#include "gui.h"  
#include "scrollbar.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-文本框 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//升级说明 
//V1.1 20140814
//修改memo_draw_memo函数,添加模式控制,实现从头显示/从尾显示
//////////////////////////////////////////////////////////////////////////////////

//memo默认的文字颜色及背景色		 
#define MEMO_DFT_TBKC 	0XFFFF		//默认的文本背景色
#define MEMO_DFT_TXTC 	0X0000		//默认的文本颜色

//memo的滚动条宽度定义
#define MEMO_SCB_WIDTH 	0X0E		//滚动条宽度为15个像素


//memo结构体定义
__packed typedef struct 
{
	u16 top; 				  		//memo顶端坐标
	u16 left;                       //memo左端坐标
	u16 width; 				  		//memo宽度(包含滚动条的宽度)
	u16 height;						//memo高度

	u8 id;							//memoID
	u8 type;						//memo类型
									//[7:1]:保留					 
									//[0]:0,不允许编辑;1,允许编辑			   

	u8 sta;							//memo状态
									//[7]:0,memo没有检测到按下事件;1,有按下事件.
									//[6]:临时数据有效标志
									//[5]:滑动标志
									//[4:2]:保留
									//[1]:光标显示状态 0,未显示.1,显示.	  
									//[0]:memo选中状态 0,未选中.1,选中.	  

	u32 textlen;					//memo字符串text的长度
	u8 *text;						//memo的内容,由textlen的大小来决定
	u32 *offsettbl;					//字符串偏移量表,记录每行开始的字符串在text里面的位置,大小为:scbv->totalitems
	u8 font;						//text文字字体
											  
 	u16 lin;						//光标所在的行位置(是指所有行)
	u16 col;						//光标所在列位置(按宽度除以(font/2)计算)

	u16 typos;						//临时的y坐标
	u16 txpos;						//临时的x坐标

	u32 memo_timer_old;				//上一次时间

	_scrollbar_obj * scbv;			//垂直滚动条
	u16 textcolor; 				  	//文字颜色
	u16 textbkcolor;				//文字背景色
}_memo_obj;

_memo_obj * memo_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type,u8 font,u32 textlen);
void memo_delete(_memo_obj * memo_del);
void memo_draw(_memo_obj * memox);


u8 memo_set_fontsize(_memo_obj * memox,u8 size);
u16 memo_get_cursorxpos(_memo_obj * memox);
void memo_read_cursorbkctbl(_memo_obj * memox);
void memo_draw_cursor(_memo_obj * memox,u16 color);
void memo_recover_cursorbkctbl(_memo_obj * memox);
void memo_show_cursor(_memo_obj * memox,u8 sta);
void memo_cursor_flash(_memo_obj *memox);
void memo_draw_text(_memo_obj * memox);	  
void memo_add_text(_memo_obj * memox,u8 * str);
void memo_draw_memo(_memo_obj * memox,u8 mode);
u8 memo_check(_memo_obj * memox,void * in_key);	 
 
//
void memo_test(u16 x,u16 y,u16 width,u16 height,u8 type,u8 sta,u16 textlen);

#endif

















