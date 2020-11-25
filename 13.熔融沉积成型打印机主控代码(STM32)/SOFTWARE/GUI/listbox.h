#ifndef __LISTBOX_H
#define __LISTBOX_H 
#include "gui.h"  	 			  
#include "scrollbar.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-列表框 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2012/10/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************
//升级说明 
//V1.1 20140810
//1,新增对长条目的滚动显示功能
//2,修正原来存在的一些小bug
//3,修改滚动条检测机制,使用更方便
//4,去掉LBOX_ITEM_HEIGHT宏定义,改用变量方式控制(在gui_phy结构体里面设置)
//////////////////////////////////////////////////////////////////////////////////

//////listbox上的符号的默认颜色 
#define LBOX_DFT_LBKCOLOR		0XFFFF		//内部背景色
#define LBOX_DFT_LSELCOLOR		0X0000//0XFFFF		//选中list后的字体颜色
#define LBOX_DFT_LSELBKCOLOR	0XAF7D//0X001F		//选中list后的背景色
#define LBOX_DFT_LNCOLOR		0X0000		//未选中的list字体颜色
#define LBOX_DFT_RIMCOLOR		0XF800		//边框颜色

////listboxtype
#define LBOX_TYPE_RIM 			0X01		//有边框的 
//滚条宽度
#define LBOX_SCB_WIDTH 			0X14//0X0E		//滚动条宽度为15个像素

    
//list结构体.链表结构
__packed typedef struct 
{
	void * prevlist;
	void * nextlist;
	u32 id;
	u8 *name;
}_listbox_list;

//listbox结构体定义
__packed typedef struct 
{
	u16 top; 				  		//listbox顶端坐标
	u16 left;                       //listbox左端坐标
	u16 width; 				  		//宽度
	u16 height;						//高度 必须为12/16的倍数

	u8 type;						//类型标记字
									//[bit7]:1,需要画滚动条出来(条件是totalitems>itemsperpage);0,不需要画出来.(此位由软件自动控制)
									//[bit6:0]:保留					 
								
	u8 sta;							//listbox状态,[bit7]:滑动标志;[bit6]:编号有效的标志;[bit5:0]:第一次按下的编号.	 
	u8 id;							//listbox 的id
	u8 dbclick;					   	//双击,
									//[7]:0,没有双击.1,有双击.
									//[6~0]:0,保留.

	u8 font;						//文字字体 12/16
	u16 selindex;					//选中的索引
 
	u16 lbkcolor;					//内部背景颜色
 	u16 lnselcolor; 				//list name 选中后的颜色
  	u16 lnselbkcolor; 				//list name 选中后的背景颜色
	u16 lncolor; 					//list name 未选中的颜色
	u16 rimcolor;					//边框颜色
//////////////////////
	u8 *fname;						//当前选中的index的名字
	u16 namelen;					//name所占的点数.
	u16 curnamepos;					//当前的偏移
	u32 oldtime;					//上一次更新时间 

	_scrollbar_obj * scbv;			//垂直滚动条
	_listbox_list *list;			//链表	    
}_listbox_obj;


_listbox_obj * listbox_creat(u16 left,u16 top,u16 width,u16 height,u8 type,u8 font);//创建listbox
_listbox_list * list_search(_listbox_list *listx,u16 index);//查找编号为index的list,并取得详细信息
void listbox_delete(_listbox_obj *listbox_del);	 			//删除listbox
u8 listbox_check(_listbox_obj * listbox,void * in_key);		//检查listbox的按下状态
u8 listbox_addlist(_listbox_obj * listbox,u8 *name);		//增加一个list
void listbox_draw_list(_listbox_obj *listbox);				//画list
void listbox_draw_listbox(_listbox_obj *listbox);			//重画listbox
void listbox_2click_hook(_listbox_obj *listbox);			//双击钩子函数
#endif





