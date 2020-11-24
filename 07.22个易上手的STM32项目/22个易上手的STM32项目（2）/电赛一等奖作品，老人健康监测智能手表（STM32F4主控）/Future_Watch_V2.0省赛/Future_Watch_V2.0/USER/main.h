#ifndef _MAIN_H_
#define _MAIN_H_

#include "sys.h"
#include "delay.h"
#include "includes.h"
#include "GUI.h"
#include "FT6236.h"
#include "st7789.h"
#include "led.h"
#include "DMA.h"
#include "battery.h"
#include "usr_wifi.h"
#include "rtc.h"
#include "usart_2.h"
#include "stdlib.h"
#include "math.h"
#include "watch.h"
#include "mlx90615.h"
#include "backlit.h"
#include "timer_4.h"
#include "timer_2.h"
#include "mpu6050.h"
#include "motor.h"
#include "key.h"
#include "wdg.h"
#include "dispwifi.h"
#include "stmflash.h"
#include "Interrupt.h"
#include "dispdata.h"
#include "heart.h"
#include "inv_mpu.h"                         //DMP；
#include "inv_mpu_dmp_motion_driver.h"  
#include "dispwifi.h"
#include "watch.h"
#include "dispdata.h"
#include "help.h"
#include "medicine.h"
#include "arm_math.h"
#include "get_data.h"
//系统状态标志位
typedef struct{
	u8 Mcu_Power;		//开关机标志位
	volatile u8 Get_Time;		//时间更新间隔设置
	volatile u8 Check_Key;		//扫描按键间隔设置
	volatile u16 Get_Heart;		//获取心率间隔设置
	volatile u16 Get_Temp;		//获取温度间隔设置
	volatile u8 Get_Tumble;		//摔倒判断间隔设置
	volatile u8 Get_backlit;		//背光判断时间设置
	volatile u16 Check_Health;		//报警检测时间
	volatile u8 Tumble_Flag;		//摔倒标志位
	
	volatile u16 Disp_Sta;		//界面显示标志位
							//第7位：显示副界面标志位，程序中，副页面只刷新一次，就不再刷新
							//第6位：WIFI界面已经显示标志位，用来给显示主界面时删除WIFI界面的句柄
							//第4位：数据界面已显示标志位
							//第5位：锁屏标志位，为1表示当前为锁屏状态
							//第3位：当前显示的是数据页面
							//第2位：当前显示的是WIFI页面
							//第1位：当前显示的是主界面
							//第0位：当前显示的是报警界面
	
	volatile u8 unlock;		//翻腕动作，亮屏幕，0->动作没有发生，1->动作发生
	
	volatile u8 Measure;		//血压正在测量标志位
							//第7位:开始测量
							//第6位:表示测量成功
	
	volatile u8 Warning;		//报警提醒功能标志位
							//第7位:摔倒
							//第6位:心率
							//第5位:体温
							//第4位:其他
							//第3位:确定发送求救信息
									
	volatile u8 Remind;			//吃药提醒界面
	volatile u8 Remind1;		//吃药提醒界面
	volatile u8 Remind2;		//吃药提醒界面
	volatile u8 Send_Start;	//收到服务器的GET，开始发送数据
	
	volatile u8 battery;		//电池电量标志位
							//第7位:满电
							//第6位:中间
							//第5位:没电
							
	volatile u8 Wifi_Connection;		//Wifi链接相关标志位
									//第7位:开始启动smartlink
									//第6位:链接失败
									//第5位:链接成功
									//第4位:已经显示"Wifi正在链接标志位"
									//第3位:开始显示,链接时的相关中文提示
	volatile u8 Motor;		//震动马达标志位
							//第7位:开启一次震动标志位
							//第6位:当前为震动标志位
							//其余位用来计数
	u8 App_Sta;			//APP是否链接标志位
	u16 Help_Time;		//报警界面，等待时间
}SYSTEM_STA;


//系统数据
typedef struct{
	u8 heart;			//心率
	float temperature;		//体温
	u8 heart_average;		//心率均值
	float temp_average;		//体温均值
	u8 boolpressure_up;	//血压收缩压
	u8 boolpressure_down;//血压舒张压
}SYSTEM_DATA;


//系统设置(此部分需要掉点保存)
typedef struct{
	u8 warning_heart;	//心率报警开关 0:关 1:开
	u8 warning_fall;	//摔倒报警开关
	u8 warning_temp;	//体温报警开关
	u8 warning_eat;		//吃药提醒开关
							//第7位 第一个时间点设置开
							//第6位 第二个时间点设置开
							//第5位 第三个时间点设置开
	u8 eat_time1;		//第一个时间点的吃药的时
	u8 eat_min1;		//第一个时间点的吃药的分
	u8 eat_time2;		//第一个时间点的吃药的时
	u8 eat_min2;		//第一个时间点的吃药的分
	u8 eat_time3;		//第一个时间点的吃药的时
	u8 eat_min3;		//第一个时间点的吃药的分
}SYSTEM_SET;


//从FLASH读数据
int Read_Data(void);

//保存数据到FLASH
int Save_Data(void);
#endif


