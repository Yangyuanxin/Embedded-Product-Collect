#ifndef __MENU_COMMON_H
#define __MENU_COMMON_H 	
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   	 
#include "key.h"	 	  	 
//#include "wdg.h" 	 
//#include "timer.h"	 	 	 
#include "lcd.h"
//#include "usmart.h"	
//#include "rtc.h"
//#include "wkup.h"	
//#include "adc.h"	
//#include "dac.h"	
//#include "dma.h"	
#include "24cxx.h"	
#include "flash.h"	
#include "touch.h"	
//#include "24l01.h"	
//#include "remote.h"	 
//#include "ds18b20.h"
//#include "ps2.h"	
//#include "mouse.h"	 
//#include "stmflash.h"	 
#include "malloc.h"
#include "string.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "exfuns.h"  
#include "fontupd.h"
#include "fontshow.h"		
#include "picdecode.h"
//#include "t9input.h" 
//#include "includes.h"
#include "ff.h"
#include "picdecode.h"
#include "gui_conf.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP通用 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/2/16
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//V1.1 20140216
//新增对各种分辨率LCD的支持.
////////////////////////////////////////////////////////////////////////////////// 	  


//硬件平台软硬件版本定义	   	
#define HARDWARE_VERSION	   		10		//硬件版本,放大10倍,如1.0表示为10
#define SOFTWARE_VERSION	    	100		//软件版本,放大100倍,如1.00,表示为100
//硬件V3.0
//1，采用STM32F103RCT6作为主控
//2，采用CH340G作为USB转串口芯片
//3，修改JTAG SWD的两个IO口被占用,从而影响仿真的问题.
//4，采用W25Q64作为外部FLASH存储器
//5，更改部分外设的连接方式.
//6，去掉JF24C的预留位置
 




//系统数据保存基址			  
#define SYSTEM_PARA_SAVE_BASE 		100		//系统信息保存首地址.从100开始.
			    
extern volatile u8 system_task_return;

////////////////////////////////////////////////////////////////////////////////////////////
//各图标/图片路径
extern u8*const  APP_OK_PIC;			//确认图标
extern u8*const  APP_CANCEL_PIC;		//取消图标
extern u8*const  APP_UNSELECT_PIC;		//未选中图标
extern u8*const  APP_SELECT_PIC;		//选中图标
extern u8*const  APP_ASCII_60;			//大数字字体路径
extern u8*const  APP_ASCII_28;			//大数字字体路径
extern u8*const  APP_VOL_PIC;			//音量图片路径

extern u8*const APP_ASCII_S6030;	//数码管大字体路径
extern u8*const APP_ASCII_5427;		//普通大字体路径
extern u8*const APP_ASCII_3618;		//普通大字体路径
extern u8*const APP_ASCII_2814;		//普通大字体路径

extern u8* asc2_s6030;				//数码管字体60*30大字体点阵集
extern u8* asc2_5427;				//普通字体54*27大字体点阵集
extern u8* asc2_3618;				//普通字体36*18大字体点阵集
extern u8* asc2_2814;				//普通字体28*14大字体点阵集

extern const u8 APP_ALIENTEK_ICO1824[];	//启动界面图标,存放在flash
extern const u8 APP_ALIENTEK_ICO2432[];	//启动界面图标,存放在flash
extern const u8 APP_ALIENTEK_ICO3648[];	//启动界面图标,存放在flash
////////////////////////////////////////////////////////////////////////////////////////////
//APP的总功能数目
#define APP_FUNS_NUM	9

//app主要功能界面标题
extern u8*const  APP_MFUNS_CAPTION_TBL[APP_FUNS_NUM][GUI_LANGUAGE_NUM];
extern u8*const  APP_MODESEL_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const  APP_REMIND_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const  APP_SAVE_CAPTION_TBL[GUI_LANGUAGE_NUM];
extern u8*const  APP_CREAT_ERR_MSG_TBL[GUI_LANGUAGE_NUM];
//平滑线的起止颜色定义
#define WIN_SMOOTH_LINE_SEC	0XB1FFC4	//起止颜色
#define WIN_SMOOTH_LINE_MC	0X1600B1	//中间颜色

//弹出窗口选择条目的设置信息
#define APP_ITEM_BTN1_WIDTH		60	  		//有2个按键时的宽度
#define APP_ITEM_BTN2_WIDTH		100			//只有1个按键时的宽度
#define APP_ITEM_BTN_HEIGHT		30			//按键高度
#define APP_ITEM_ICO_SIZE		32			//ICO图标的尺寸

#define APP_ITEM_SEL_BKCOLOR	0XAF7D//0X0EC3		//选择时的背景色
#define APP_WIN_BACK_COLOR	 	0XFFFF//0XC618		//窗体内部背景色


#define APP_FB_TOPBAR_HEIGHT	20			//文件浏览界面,顶部横条的高度
#define APP_FB_BTMBAR_HEIGHT	20			//文件浏览界面/测试界面,底部横条的高度
#define APP_TEST_TOPBAR_HEIGHT	20			//测试界面,顶部横条高度

//π值定义
#define	app_pi	3.1415926535897932384626433832795  
/////////////////////////////////////////////////////////////////////////
																			    
u32  app_get_rand(u32 max);
void app_srand(u32 seed);
void app_set_lcdsize(u8 mode);
void app_read_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_recover_bkcolor(u16 x,u16 y,u16 width,u16 height,u16 *ctbl);
void app_gui_tcbar(u16 x,u16 y,u16 width,u16 height,u8 mode);
u8 app_get_numlen(long long num,u8 dir);
void app_show_float(u16 x,u16 y,long long num,u8 flen,u8 clen,u8 font,u16 color,u16 bkcolor);
void app_filebrower(u8 *topname,u8 mode);		 
void app_showbigchar(u8 *fontbase,u16 x,u16 y,u8 chr,u8 size,u16 color,u16 bkcolor);
void app_showbigstring(u8 *fontbase,u16 x,u16 y,const u8 *p,u8 size,u16 color,u16 bkcolor);
void app_showbignum(u8 *fontbase,u16 x,u16 y,u32 num,u8 len,u8 size,u16 color,u16 bkcolor);
void app_showbig2num(u8 *fontbase,u16 x,u16 y,u8 num,u8 size,u16 color,u16 bkcolor);
void app_show_nummid(u16 x,u16 y,u16 width,u16 height,u32 num,u8 len,u8 size,u16 ptcolor,u16 bkcolor);
void app_draw_smooth_line(u16 x,u16 y,u16 width,u16 height,u32 sergb,u32 mrgb);
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor);

u8 app_tp_is_in_area(_m_tp_dev *tp,u16 x,u16 y,u16 width,u16 height);
void app_show_items(u16 x,u16 y,u16 itemwidth,u16 itemheight,u8*name,u8*icopath,u16 color,u16 bkcolor);
u8 * app_get_icopath(u8 mode,u8 *selpath,u8 *unselpath,u8 selx,u8 index);
u8 app_items_sel(u16 x,u16 y,u16 width,u16 height,u8 *items[],u8 itemsize,u8 *selx,u8 mode,u8*caption);
u8 app_listbox_select(u8 *sel,u8 *top,u8 * caption,u8 *items[],u8 itemsize);
void app_show_mono_icos(u16 x,u16 y,u8 width,u8 height,u8 *icosbase,u16 color,u16 bkcolor);

u8 app_system_file_check(u8* diskx);//系统文件检测
u8 app_boot_cpdmsg(u8*pname,u8 pct,u8 mode);
void app_boot_cpdmsg_set(u16 x,u16 y,u8 fsize);
u8 app_system_update(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode));
void app_getstm32_sn(u32 *sn0,u32 *sn1,u32 *sn2);
void app_get_version(u8*buf,u32 ver,u8 len);//得到版本号

void app_usmart_getsn(void);//USMART专用.
u8 app_system_parameter_init(void);//系统信息初始化
#endif




























































