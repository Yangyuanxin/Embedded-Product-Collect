#ifndef __SCROLLBAR_H
#define __SCROLLBAR_H 		 
#include "gui.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-滚动条 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/10/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//滚动条上的符号的颜色
#define SCROLLBAR_SIGN_RCOLOR 	0X0000	    //符号松开颜色为黑色
#define SCROLLBAR_SIGN_PCOLOR 	0XFFFF	    //符号按下颜色为白色

//滚动条尺寸设定
#define SCROLLBAR_PART_LEN 		20			//两端按钮的长度(即宽度)
#define SCROLLBAR_MIN_THICK		10			//最小的厚度(即高度)

//方向定义
#define SCROLLBAR_DIR_HOR		0X00		//水平scrollbar
#define SCROLLBAR_DIR_VER		0X80		//垂直scrollbar
//图标编号
#define SCROLLBAR_SIGN_UP	 	0X00	    //上图标
#define SCROLLBAR_SIGN_DOWN	 	0X01	    //下图标
#define SCROLLBAR_SIGN_LEFT	 	0X02	    //左图标
#define SCROLLBAR_SIGN_RIGHT 	0X03	    //右图标
#define SCROLLBAR_SIGN_HOR	 	0X04	    //水平图标
#define SCROLLBAR_SIGN_VER 		0X05	    //垂直图标
//按钮定义
#define SCROLLBAR_BTN_LTP 		0X01	    //左端/顶部按钮被按下
#define SCROLLBAR_BTN_MID 		0X02	    //中间按下
#define SCROLLBAR_BTN_RBP 		0X04	    //右端/底部按钮被按下
//滚动条类型定义
#define SCROLLBAR_TYPE_STD	 	0X00
#define SCROLLBAR_TYPE_NBT   	0X01

//滚动条默认颜色 
#define SCLB_DFT_INBKCOLOR		0XFFFF		//填充色A颜色
#define SCLB_DFT_BTNCOLOR		0XBD19//0X2DDA //0X031F		//填充色B颜色
#define SCLB_DFT_RIMCOLOR		0XBD19//0X4A49		//边框颜色

//滚动条结构体定义
__packed typedef struct 
{
	u16 top; 				  		//按钮顶端坐标
	u16 left;                       //按钮左端坐标
	u16 width; 				  		//宽度(至少大于40)
	u16 height;						//高度(至少大于2)

	u8 type;						//类型标记字节 [bit7]:方向,0,水平;1,垂直;bit[6:2]:保留;[bit1:0]:按钮类型,0,标准滚动条;1,没有端按钮的滚动条;2,3,保留.
	u8 sta;							//按钮状态 bit0:左端/顶部按钮;bit1:中间按钮;bit2:右端/底部按钮.	 
	u8 id;							//滚动条id

 	u16 totalitems;					//总条目数
	u16 itemsperpage;				//每页显示的条目数
	u16 topitem;					//最顶/左端的条目 
	u16 scbbarlen;					//滚条长度
 
	u16 inbkcolor; 					//内部颜色
	u16 btncolor;					//符号按钮填充色
	u16 rimcolor;					//边框颜色
}_scrollbar_obj;


_scrollbar_obj * scrollbar_creat(u16 left,u16 top,u16 width,u16 height,u8 type);
void scrollbar_delete(_scrollbar_obj *scrollbar_del);	 
void scrollbar_draw_sign(u16 x,u16 y,u16 signcolor,u8 signx);
void scrollbar_draw_btn(u16 x,u16 y,u16 width,u16 height,u16 bkcolor,u16 outcolor,u16 signcolor,u8 signx);
void scrollbar_setpos(_scrollbar_obj *scbx);				//设置滑动条位置
void scrollbar_draw_scrollbar(_scrollbar_obj *scbx);		//画滑动条
u8 scrollbar_check(_scrollbar_obj * scbx,void * in_key);	//滑动条按键处理函数
void scrollbar_hook(_scrollbar_obj *scbx);					//钩子函数

void test_scb(u16 x,u16 y,u16 width,u16 height,u8 type,u16 icur,u16 incolor,u16 btncolor,u16 outcolor);
#endif





