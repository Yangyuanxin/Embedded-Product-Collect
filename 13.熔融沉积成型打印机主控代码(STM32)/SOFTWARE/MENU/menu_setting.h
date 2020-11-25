#ifndef __MENU_SETTING_H
#define __MENU_SETTING_H
#include "sys.h"
//#include "includes.h" 	   	 
#include "menu_common.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//APP-设置 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/20
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
//list结构体.链表结构
__packed typedef struct 
{
	u8 syslanguage;			//默认系统语言
 	u8 lcdbklight;			//LED背光亮度 10~110.10代表最暗;110代表最亮
	u8 picmode;				//图片浏览模式:0,顺序循环播放;1,随机播放.
	u8 audiomode;			//音频播放模式:0,顺序循环播放;1,随机播放;2,单曲循环播放.
	u8 videomode;			//视频播放模式:0,顺序循环播放;1,随机播放;2,单曲循环播放.
	u8 saveflag;			//保存标志,0X0A,保存过了;其他,还从未保存	   
}_system_setings;

extern _system_setings systemset;	//在settings.c里面设置 

u8 sysset_time_set(u16 x,u16 y,u8 *hour,u8 *min,u8*caption);
u8 sysset_date_set(u16 x,u16 y,u16 *year,u8 *month,u8 *date,u8*caption);
//void sysset_wm8978_vol3d_show(u16 x,u16 y,u8 mask,u8 val);
//void sysset_wm8978_eq_show(u16 x,u16 y,u8 mask,u8 eqx,u8 val);
//u8 sysset_wm8978_vol3d_set(u16 x,u16 y,_wm8978_obj *wmset,u8* caption);
//u8 sysset_wm8978_eq_set(u16 x,u16 y,_wm8978_obj *wmset,u8 eqx,u8* caption);
u8 sysset_bklight_set(u16 x,u16 y,u8* caption,u16 *bkval);
u8 sysset_system_update_cpymsg(u8*pname,u8 pct,u8 mode);
void sysset_system_update(u8*caption,u16 sx,u16 sy);
void sysset_system_info(u16 x,u16 y,u8*caption);
void sysset_system_status(u16 x,u16 y,u8* caption);
void sysset_system_about(u16 x,u16 y,u8*caption);
u8* set_search_caption(const u8 *mcaption);	 
void sysset_read_para(_system_setings * sysset);
void sysset_save_para(_system_setings * sysset);

u8 sysset_play(void);
										   
#endif












