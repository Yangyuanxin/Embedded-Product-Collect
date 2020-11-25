#ifndef __PROGRESSBAR_H
#define __PROGRESSBAR_H 	 
#include "gui.h" 	
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//GUI-进度条 代码	   
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
//修改progressbar_check函数加入模糊检测 
//////////////////////////////////////////////////////////////////////////////////

////进度条上的符号的默认颜色 
#define PRGB_DFT_BKACOLOR		0XEF7D		//背景色A颜色
#define PRGB_DFT_BKBCOLOR		0XE73C		//背景色B颜色
#define PRGB_DFT_FILLACOLOR		0X0FBB		//填充色A颜色
#define PRGB_DFT_FILLBCOLOR		0X071F		//填充色B颜色
#define PRGB_DFT_RIMCOLOR		0X4A49		//边框颜色
#define PRGB_DFT_BTNCOLOR		0X0000		//浮动条/文字颜色,黑色

//进度条type
#define PRGB_TYPE_HOR 			0X00		//水平的
#define PRGB_TYPE_VER			0X80		//垂直的
#define PRGB_TYPE_FEN			0X40  		//浮标是否显示(百分进度条的时候,表示百分数是否显示)
#define PRGB_TYPE_PEN			0X20  		//进度柱是否显示
#define PRGB_TYPE_STD			0X00  		//标准进度条
#define PRGB_TYPE_TEXT			0X01  		//百分数进度条

#define PRGB_BTN_DOWN			0X01  		//中间按钮被按下了


//进度条结构体定义
__packed typedef struct 
{
	u16 top; 				  		//进度条顶端坐标
	u16 left;                       //进度条左端坐标
	u16 width; 				  		//宽度(至少大于40)
	u16 height;						//高度(至少大于2)

	u8 type;						//类型标记字 
									//[bit7]:方向,0,水平;1,垂直;
									//[bit6]:0,不显示浮标;1,显示浮标;
									//[bit5]:0,不显示进度柱,1,显示进度柱(其实就是颜色是否填充,是否跟着浮标填充颜色.)
									//[bit4:2]:保留;
									//[bit1:0]:0,标准进度条;1,百分数进度条;2,3,保留.
	u8 sta;							//进度条状态 bit0:进度条被按下.	 
	u8 id;							//进度条id

 	u32 totallen;					//总长度		    
	u32 curpos;						//当前位置
	u16 prgbarlen;					//滚条长度 由用户设定.仅在标准滑动条时有效
 
	u16 inbkcolora; 				//内部背景颜色
	u16 inbkcolorb; 				//内部背景颜色
	u16 infcolora; 					//内部填充颜色a
	u16 infcolorb; 					//内部填充颜色b
 	u16 btncolor;					//浮标填充色(标准prg时为内部滑动点的颜色,百分数prg时,为字体颜色,扩展时,无效)
	u16 rimcolor;					//边框颜色
}_progressbar_obj;


_progressbar_obj * progressbar_creat(u16 left,u16 top,u16 width,u16 height,u8 type);
u8 progressbar_check(_progressbar_obj * prgbx,void * in_key);
void progressbar_delete(_progressbar_obj *progressbar_del);	 
void progressbar_hook(_progressbar_obj *prgbx);
void progressbar_draw_progressbar(_progressbar_obj *prgbx);
void progressbar_setpos(_progressbar_obj *prgbx);
void progressbar_draw_sign(_progressbar_obj *prgbx,u16 x,u16 y);
void progressbar_show_percnt(u16 x,u16 y,u8 dir,u8 num,u16 color);

#endif





