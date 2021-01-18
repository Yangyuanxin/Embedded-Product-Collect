/*
*********************************************************************************************************
*
*	模块名称 : 头文件汇总
*	文件名称 : includes.h
*	版    本 : V1.0
*	说    明 : 当前使用头文件汇总
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2020-06-12  Eric2013   首次发布
*
*	Copyright (C), 2020-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _INCLUDES_H_
#define _INCLUDES_H_

/*
*********************************************************************************************************
*                                         APP / BSP
*********************************************************************************************************
*/
#include "bsp.h"

/*
*********************************************************************************************************
*                                            其它
*********************************************************************************************************
*/
#include "ui_def.h"
#include "param.h"
#include "modbus_register.h"
#include "disp_lib.h"   
#include "modbus_reg_addr.h"
 
#include "lcd_menu.h"
#include "file_lib.h"
#include "lua_if.h"
#include "prog_if.h"
#include "modify_param.h"
#include "param.h"
#include "wifi_if.h"
#include "usb_if.h"
    
/* 主程序状态字定义, MS = Main Status */
enum
{
    MS_LINK_MODE = 0,       /* 联机状态 */

    MS_SYSTEM_SET,          /* 系统设置 */
    MS_HARD_INFO,           /* 关于-硬件信息 */
    MS_ESP32_TEST,          /* ESP32模块升级状态 */
    MS_USB_EMMC,            /* 虚拟串口状态。RS232 RS485 TTL-UART */
    MS_MODIFY_PARAM,        /* 参数设置 */
    MS_FILE_MANAGE,         /* 文件管理 */
    
    MS_PROG_SELECT_FILE,    /* 脱机下载器浏览文件 */
    MS_PROG_WORK,           /* 脱机下载器工作界面 */
    MS_PROG_SETTING,        /* 脱机下载器参数设置 */
    MS_PROG_MODIFY_PARAM,   /* 修改参数比如复位类型 */
    
    MS_VOLTAGE_METER,       /* 电压表 */
    MS_CURRENT_METER,       /* 高侧电流表 */
    MS_TEMP_METER,          /* 温度表 */
    MS_RESISTOR_METER,      /* 电阻表 */
    MS_TVCC_POWER,          /* 微型数控电源 */    
    MS_PULSE_METER,         /* 脉冲计 */
    
    MS_MINI_DSO,            /* 迷你示波器 */
    
    MS_EXTEND_MENU1,        /* 第1级菜单  */
    MS_EXTEND_MENU_LUA,     /* 第2级菜单-LUA程序 */      
    MS_EXTEND_MENU_REC,     /* 第2级菜单-数据记录仪 */    
    
    MS_LUA_SELECT_FILE,     /* 浏览lua文件 */
    MS_LUA_EXEC_FILE,       /* 执行lua文件 */
    
    MS_MONITOR_UART,        /* 串口监视器 */
    MS_MONITOR_CAN,         /* CAN监视器 */
    MS_MONITOR_GPIO,        /* IO监视器 */
    MS_MONITOR_ANALOG,      /* 模拟量监视器 */
    
    MS_DAPLINK,             /* 仿真器状态 */    
    MS_DAP_SETTING_MENU,    /* DAP长按S进入，功能设置 */
    MS_JUMP_APP,            /* 跳到APP状态 */
    
    MS_RTT_VIEWER,          /* rtt viewer */
		
    MS_DS18B20_METER,       /* DS18B20温度表 */
    MS_USB_UART,            /* USB虚拟串口 */
};

uint16_t NextStatus(uint16_t _NowStatus);
uint16_t LastStatus(uint16_t _NowStatus);
void DSO_StartMode2(void);
void PlayKeyTone(void);

extern uint16_t g_MainStatus;

extern uint8_t qspi_file_buf[1024];
extern uint8_t qspi_read_buf[1024];


#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
