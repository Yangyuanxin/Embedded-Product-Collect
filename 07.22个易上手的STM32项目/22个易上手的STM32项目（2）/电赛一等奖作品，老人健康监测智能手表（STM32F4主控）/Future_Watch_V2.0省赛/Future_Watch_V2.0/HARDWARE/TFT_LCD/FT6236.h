/*
**************************************************************
*
* 文件名称：FT6236.h;
*
* 版本说明：V1.0；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：无；
*           
* 完成日期：2016/06/26;
*
* 作者姓名：郑子烁;
*
* 其他备注：触屏芯片FT6236驱动程序；
*           来自厂家例程，参考正点原子FT5206；
*          
* 修改说明：无；
*
**************************************************************
*/


#ifndef __FT6236_H__
#define __FT6236_H__

#include "FT6236_iic.h"
#include "GUI.h"
#include "led.h"

//I2C读写命令	
#define FT_CMD_WR 				0X70    	//写命令
#define FT_CMD_RD 				0X71		//读命令

//FT6236 部分寄存器定义 
#define FT_DEVIDE_MODE 			0x00   		//FT6236模式控制寄存器
#define FT_REG_NUM_FINGER       0x02		//触摸状态寄存器

#define FT_TP1_REG 				0X03	  	//第一个触摸点数据地址
#define FT_TP2_REG 				0X09		//第二个触摸点数据地址
#define FT_TP3_REG 				0X0F		//第三个触摸点数据地址
#define FT_TP4_REG 				0X15		//第四个触摸点数据地址
#define FT_TP5_REG 				0X1B		//第五个触摸点数据地址  
 

#define	FT_ID_G_LIB_VERSION		0xA1        //版本		
#define FT_ID_G_MODE 			0xA4   		//FT6236中断模式控制寄存器
#define FT_ID_G_THGROUP			0x80   		//触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE	0x88   		//激活状态周期设置寄存器  
#define Chip_Vendor_ID          0xA3        //芯片ID(0x36)
#define ID_G_FT6236ID			0xA8		//0x11

#define TP_PRES_DOWN            0x80        //触屏被按下	
#define TP_COORD_UD             0x40        //触摸坐标更新标记


//触摸点相关数据结构体定义
//触摸情况，b7:按下1/松开0; b6:0没有按键按下/1有按键按下;
//bit5:保留；bit4-bit0触摸点按下有效标志，有效为1，分别对应触摸点5-1；
//支持5点触摸，需要使用5组坐标存储触摸点数据
typedef struct			
{
	u8 TouchSta;	
	u16 x[2];		
	u16 y[2];
	u16 Point_X[30];
	u16 Point_Y[30];
	u16 LastX_Point;
	u16 LastY_Point;
	u8 Point_Num;
	u8 Gesture_Sta;
}TouchPointRefTypeDef;

extern TouchPointRefTypeDef TPR_Structure;


/*
* 函数名称：FT6236_Init；
* 功能说明：FT6236初始化；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/		
void FT6236_Init(void);


/*
* 函数名称：FT6236_Write_Reg；
* 功能说明：FT6236连续写寄存器；
* 入口参数：reg-->寄存器地址，buf-->发送缓存空间，len-->连续长度；
* 返回值  ：0-->成功，1-->失败；
* 备注    ：无；
*/
u8 FT6236_Write_Reg(u16 reg,u8 *buf,u8 len);


/*
* 函数名称：FT6236_Read_Reg；
* 功能说明：FT6236连续读寄存器；
* 入口参数：reg-->寄存器地址，buf-->接收缓存空间，len-->连续长度；
* 返回值  ：0-->成功，1-->失败；
* 备注    ：无；
*/		  
u8 FT6236_Read_Reg(u16 reg,u8 *buf,u8 len); 



/*
* 函数名称：FT6236_Scan；
* 功能说明：触屏扫描；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/	
void FT6236_Scan(void);
int Gesture_Scan(void);

#endif
