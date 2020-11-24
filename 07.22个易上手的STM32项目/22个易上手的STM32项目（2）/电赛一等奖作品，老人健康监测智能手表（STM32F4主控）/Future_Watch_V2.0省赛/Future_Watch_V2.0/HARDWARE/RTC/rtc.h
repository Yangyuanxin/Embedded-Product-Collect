#ifndef __RTC_H
#define __RTC_H	 
#include "sys.h" 
 //////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//RTC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/5
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//修改说明
//V1.1 20140726
//新增:RTC_Get_Week函数,用于根据年月日信息,得到星期信息.
////////////////////////////////////////////////////////////////////////////////// 
typedef struct{
	u8 hour;
	u8 min;
	u8 sec;
	u8 month;
	u8 date;
	u8 week;
	u8 year;
}Clock;
	
u8 RTC_Init(void);						//RTC初始化
u8 RTC_Wait_Synchro(void);				//等待同步
u8 RTC_Init_Mode(void);					//进入初始化模式
void RTC_Write_BKR(u32 BKRx,u32 data);	//写后备区域SRAM
u32 RTC_Read_BKR(u32 BKRx);				//读后备区域SRAM
u8 RTC_DEC2BCD(u8 val);					//十进制转换为BCD码
u8 RTC_BCD2DEC(u8 val);					//BCD码转换为十进制数据
u8 RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm);			//RTC时间设置
u8 RTC_Set_Date(u8 year,u8 month,u8 date,u8 week); 		//RTC日期设置
void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm);	//获取RTC时间
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week);//获取RTC日期
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec);		//设置闹钟时间(按星期闹铃,24小时制)
void RTC_Set_WakeUp(u8 wksel,u16 cnt);					//周期性唤醒定时器设置
u8 RTC_Get_Week(u16 year,u8 month,u8 day);				//根据输入的年月日,计算当日所属星期几
#endif

















