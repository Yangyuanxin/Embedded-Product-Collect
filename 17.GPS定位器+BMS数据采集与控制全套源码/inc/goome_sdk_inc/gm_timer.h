/*****************************************************************************
*  Copyright Statement:
*  --------------------
* www.goome.net
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_timer.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *  Timer related APIs
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
 

#ifndef __GM_TIMER_H__
#define __GM_TIMER_H__

#include "gm_type.h"


typedef enum
{
    GM_TIMER_ID_START = 254,
		
	GM_TIMER_10MS_MAIN,

	GM_TIMER_1S_MAIN,
 
    GM_TIMER_DNS_IP_TIM,

	//限定时间收不到数据以不同波特率打开GPS串口定时器ID
    GM_TIMER_GPS_CHECK_RECEIVED,
    
	//限定时间收不到版本号发送查询MTK版本号
    GM_TIMER_GPS_QUERY_MTK_VERSION,
    
    //限定时间收不到版本号发送查询TD和AT版本号
    GM_TIMER_GPS_QUERY_TD_AT_VERSION,
    
	//限定时间收不到版本号发送查询TD版本号
    GM_TIMER_GPS_QUERY_TD_VERSION,
    
	//限定时间收不到版本号发送查询AT版本号
    GM_TIMER_GPS_QUERY_AT_VERSION,
    
    //限定时间收不到确认发送打开VTG语句
    GM_TIMER_GPS_OPEN_TD_VTG,

	//检查是否定位成功(1分钟不定位，上传LBS)
	GM_TIMER_GPS_CHECK_STATE,

	//GSensor主任务定时器(100毫秒)
	GM_TIMER_GSENSOR_MAIN_TASK,

	//加速度大于急刹车加速度后检查速度变化
	GM_TIMER_GSENSOR_CHECK_SPEED_AFTER_EMERGENCY_BRAKE,

	//加速度大于急加速加速度后检查速度变化
	GM_TIMER_GSENSOR_CHECK_SPEED_AFTER_RAPID_ACCERATION,

	//加速度大于急转弯加速度后检查角度变化
	GM_TIMER_GSENSOR_CHECK_ANGLE_AFTER_SUDDEN_TURN,

	//加速度大于急转弯加速度后检查角度变化
	GM_TIMER_GSENSOR_CHECK_ACC_AFTER_SHAKE,

	//检查伪基站报警定时器，每20秒一次
	GM_TIMER_GSM_CHECK_FAKE_CELL_ALARM,
	
    //每秒钟检查一次SIM卡状态
	GM_TIMER_GSM_CHECK_SIMCARD,

	//发现有SIM卡以后等一会去获取SIM卡信息
	GM_TIMER_GSM_GET_SIM_INFO,

	GM_TIMER_SMS_SEND,

	GM_TIMER_CMD_REBOOT,

    GM_TIMER_UPDATE_REBOOT,

	GM_TIMER_HARDWARE_AUTODEFENCE,

	GM_TIMER_HARDWARE_REBOOT,

	GM_TIMER_SELF_CHECK_START,

	GM_TIMER_MAIN_UPLOAD_LOG,

	GM_TIMER_BMS_TRANSPRENT,
    
    TIMER_ID_END=GM_TIMER_ID_START + 90
}GM_TIMER_ID;



#define GM_TICKS_10_MSEC           (2)         /* 10 msec */
#define GM_TICKS_50_MSEC           (10)        /* 50 msec */
#define GM_TICKS_100_MSEC          (21)        /* 100 msec */
#define GM_TICKS_500_MSEC          (108)       /* 500 msec */
#define GM_TICKS_1024_MSEC         (221)       /* 1024 msec */

#define GM_TICKS_1_SEC             (217)       /* 1 sec */
#define GM_TICKS_2_SEC_2           (433)       /* 2 sec */
#define GM_TICKS_3_SEC             (650)       /* 3 sec */
#define GM_TICKS_5_SEC             (1083)      /* 5 sec */
#define GM_TICKS_10_SEC            (2167)      /* 10 sec */
#define GM_TICKS_30_SEC            (6500)      /* 30 sec */
#define GM_TICKS_1_MIN             (13000)     /* 1 min */




#define TIM_GEN_1MS               2
#define TIM_GEN_10MS              10 // 3  // 10
#define TIM_GEN_100MS             100
#define TIM_GEN_1SECOND           1000
#define TIM_GEN_1_MIN             60000



typedef struct
{
    kal_uint8 unused;
} *kal_timerid;


typedef void (*kal_timer_func_ptr)(void *param_ptr);

/*
*******************************************************
系统定时器
要比较小心它的优先级很高，在回调函数运行
时，其他事件是得不到处理的
建议只用来做计数用，不做其它用处。

系统定器时 216个 tick 为1秒钟

封装的函数
*********************************************************
*/

extern s32 GM_StopKalTimer(U8 timer_id);
extern s32 GM_StartTimer(U16 timerid, U32 delay, FuncPtr funcPtr);
extern s32 GM_StopTimer(U16 timerid);
extern s32 GM_CreateKalTimer(U8 timer_id);
extern s32 GM_StartKalTimer(U8 timer_id, PsFuncPtr handler_func_ptr, U32 ticks);

#endif  // End-of __GM_TIMER_H__

