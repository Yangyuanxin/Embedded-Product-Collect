#ifndef __BUTTON_H
#define __BUTTON_H 			 
#include "gui.h"  
#include "button.h"
#include "picdecode.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-按钮 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved	
//********************************************************************************
//升级说明 
//V1.1 20140722
//1,修改btn_draw_picbtn函数,支持图片按钮尺寸大于图片尺寸,图片居中显示
//2,修改btn_check函数,增加对无效按键的判断处理.
//////////////////////////////////////////////////////////////////////////////////

//标准按钮边框颜色定义
#define STD_BTN_LTOC 	0XFFFF	    //左外线颜色
#define STD_BTN_LTIC 	0XDEFC		//左内线颜色
#define STD_BTN_RBOC 	0X8431		//右外线颜色
#define STD_BTN_RBIC 	0XA535		//右内线颜色
#define STD_BTN_INSC	0XEF5D	    //按钮内部填充颜色
#define STD_BTN_DFLC	0XA535 		//未使能按钮caption左边颜色
#define STD_BTN_DFRC	0XFFFF		//未使能按钮caption右边颜色
//圆角按钮颜色定义
#define ARC_BTN_RIMC	0X0000		//圆角按钮边框颜色
#define ARC_BTN_TP1C	0XD6BB		//第一行的颜色
#define ARC_BTN_UPHC	0X8452		//上半部分颜色
#define ARC_BTN_DNHC	0X8452//0X52CD		//下半部分颜色
//按钮颜色定义
#define BTN_DFU_BCFUC	0X0000		//默认松开的颜色
#define BTN_DFU_BCFDC	0X0000		//默认按下的颜色

//按钮状态定义
#define BTN_RELEASE 	0X00
#define BTN_PRESS   	0X01
#define BTN_INACTIVE   	0X02
//按钮类型定义
#define BTN_TYPE_STD 	0X00
#define BTN_TYPE_PIC   	0X01
#define BTN_TYPE_ANG 	0X02
#define BTN_TYPE_TEXTA 	0X03
#define BTN_TYPE_TEXTB 	0X04


//按钮结构体定义
__packed typedef struct 
{
	u16 top; 				  		//按钮顶端坐标
	u16 left;                       //按钮左端坐标
	u16 width; 				  		//宽度
	u16 height;						//高度

	u8 id;							//按钮ID
	u8 type;						//按钮类型
									//[7]:0,模式A,按下是一种状态,松开是一种状态.
									//	  1,模式B,每按下一次,状态改变一次.按一下按下,再按一下弹起.
									//[6:4]:保留
									//[3:0]:0,标准按钮;1,图片按钮;2,边角按钮;3,文字按钮(背景透明),4,文字按钮(背景单一)
	u8 sta;							//按钮状态
									//[7]:坐标状态 0,松开.1,按下.(并不是实际的TP状态)
									//[6]:0,此次按键无效;1,此次按键有效.(根据实际的TP状态决定)
									//[5:2]:保留
									//[1:0]:0,激活的(松开);1,按下;2,未被激活的

	u8 *caption;					//按钮名字
	u8 font;						//caption文字字体
	u8 arcbtnr;						//圆角按钮时圆角的半径										
	u16 bcfucolor; 				  	//button caption font up color
	u16 bcfdcolor; 				  	//button caption font down color

	u16 *bkctbl;					//对于文字按钮:
									//背景色表(按钮为文字按钮的时候使用)
									//a,当为文字按钮(背景透明时),用于存储背景色
									//b,当为文字按钮(背景单一是),bkctbl[0]:存放松开时的背景色;bkctbl[1]:存放按下时的背景色.
									//对于边角按钮:
									//bkctbl[0]:圆角按钮边框的颜色
									//bkctbl[1]:圆角按钮第一行的颜色
									//bkctbl[2]:圆角按钮上半部分的颜色
									//bkctbl[3]:圆角按钮下半部分的颜色	

	u8 *picbtnpathu;				//图片按钮松开时的图片路径
	u8 *picbtnpathd;		 		//图片按钮按下时的图片路径
}_btn_obj;

u8 screen_key_chk(_btn_obj **keydev, _in_obj * key,u8 num);
void btn_read_backcolor(_btn_obj * btnx);
void btn_recover_backcolor(_btn_obj * btnx);
_btn_obj * btn_creat(u16 left,u16 top,u16 width,u16 height,u8 id,u8 type);
void btn_delete(_btn_obj * btn_del);
void set_color(u16 up,u16 down);
u8 btn_check(_btn_obj * btnx,void * in_key);	//按钮是否被按下检测
void btn_draw(_btn_obj * btnx);					//画按钮
void btn_on_keyup(_btn_obj * btnx);				//按钮松开操作
void btn_on_keydown(_btn_obj * btnx);			//按钮按下操作
void btn_draw_stdbtn(_btn_obj * btnx);			//画标准按钮
void btn_press_hook(_btn_obj *btnx);
void btn_release_hook(_btn_obj *btnx);
void btn_draw_arcrectangle(u16 x,u16 y,u16 width,u16 height,u8 r,u8 mode,u16 upcolor,u16 downcolor);
void btn_draw_arcbtn(_btn_obj * btnx);
void btn_draw_picbtn(_btn_obj * btnx);
void btn_draw_textabtn(_btn_obj * btnx);
void btn_draw_textbbtn(_btn_obj * btnx);

void btn_test(u16 x,u16 y,u16 width,u16 height,u8 type,u8 sta,u8 r,u8 *caption,u8 *uppath,u8 *downpath);

#endif
