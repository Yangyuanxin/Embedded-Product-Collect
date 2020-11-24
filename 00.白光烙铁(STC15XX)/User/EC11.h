/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     除注明出处外，以下所有内容版权均属王志浩所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留作者的版权声明。
 *
 * @file			EC11.h
 * @brief			
 * @author		王志浩
 * @version		V2.2
 * @date			2017-8-30
 */
#ifndef _EC11_H_
#define	_EC11_H_


//宏定义
#define KEYDOWN_TIME	10u
#define	KEYHOLD_TIME	250u

/*****************************************************/
/******************** 参数结构定义 *******************/
/****************************************************/
extern unsigned char str_buff[20];//缓存
extern unsigned char BeepCounterFlag;

typedef struct
{
	unsigned char	Coder;				//编码器旋转标志位，0为无动作，1为顺时针，2为逆时针
	unsigned char	Key;					//按键短按标志位,0为没有按键按下，1为短按，2为长按
}EC11_State_s;

//工作模式下的菜单结构体
	/* 菜单信息 */
typedef struct
{
	char 	OPTINO_NAME[3][20];//页名称
	char  ADS_str[3][4][20]; //角度选项
}MENU_MSG_s;
typedef struct
{

	/* 温度参数结构 */
	struct 
	{
		unsigned int  	Max_Temp;  	
		unsigned int 	Slp_Temp; 
		unsigned int   Default_Temp;
		unsigned char  Default_CH;
		unsigned int   Last_Temp;
		unsigned char  Last_CH;		
	}Temp_s;
	/* 时间参数结构 */
	struct 
	{
		unsigned int 	Slp_Time;
		unsigned int 	ScrSaver_Time;
		unsigned int 	Shutdown_Time;
	}Time_s;
	/* 其他参数结构*/
	struct 
	{
		unsigned char 	Work_Mode;  
		unsigned char 	Heater_Tip;
		unsigned char 	Shock_Sensor; 
		unsigned char 	Beep_Volume;  
	}Other_s;
} MENU_DATA_s;
//工厂模式下的菜单结构体
typedef struct
{
	/* 工厂模式菜单信息 */
	struct
	{
		char 	OPTINO_NAME[3][20];//页名称
		char  ADS_str[3][4][15]; //角度选项
	}FAC_MSG_s;

	/* 调整参数结构 */
	struct 
	{
		unsigned char 	Iron;
		unsigned int 	Voltage; 
		unsigned char 	Encoder;
	}Adjust_s;
	struct 
	{
		unsigned int 	Ch1;
		unsigned int 	Ch2; 
		unsigned int 	Ch3;
		unsigned int 	Ch4;
	}Channel_s;
	struct 
	{
		unsigned char 	Temp_Step;
		unsigned char 	Lim_Voltage;
		unsigned char	Recovery;
	}Other_s;
	unsigned int Heater_Tip[8][14];
} FAC_DATA_s;
/********************************************************/
extern MENU_DATA_s 				MENU_DATA_Table;
extern FAC_DATA_s					FAC_DATA_Table;
extern EC11_State_s				ec11_state;
extern unsigned char ShutdownCounterFlag;
/* 函数声明 */

/* 编码器捕捉 */
void EC11_Capture(void);

/* 修改值 */
unsigned char set_var(void);

/* 选项检测 */
unsigned char option_check_data(void);

/* 页面查询 */
extern unsigned char Menu_check_data(void); 

/* 参数设定 */
unsigned int ParaSet(void);
unsigned int ParaSet_Fac(void);

/* OLED */
void Display_cursor(void);
void OLED_printf(unsigned char x,unsigned char y,char *str,float var_num);
void DisPlay(unsigned char num);



#endif