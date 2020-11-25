/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        system_state.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-12
 * Description:      全局系统状态管理
 * Others:           
 * Function List:    
    1.
    
 * History: 
    1. Date:         2019-03-12
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __SYSTEM_STATE_H__
#define __SYSTEM_STATE_H__

#include "gm_type.h"
#include "error_code.h"
#include "g_sensor.h"
#include <time.h>
#include "gps.h"


typedef enum 
{
    GM_RREBOOT_UNKNOWN = -1,
    GM_REBOOT_CMD = 0,
    GM_REBOOT_UPDATE = 1,
    GM_REBOOT_POWER_ON = 2,
    GM_REBOOT_GPRS = 3,
    GM_REBOOT_GPS = 4,
    GM_REBOOT_CHECKPARA = 5,
}BootReason;


typedef enum
{
    GM_SYSTEM_STATE_WORK = 0,
    GM_SYSTEM_STATE_SLEEP = 1
}SystemWorkState;

typedef enum
{
	VEHICLE_STATE_RUN = 0,
    VEHICLE_STATE_STATIC = 1
} VehicleState;


void system_state_set_last_gps(const GPSData *p_gps);
void system_state_get_last_gps(GPSData *p_gps);


/**
 * Function:   创建system_state模块
 * Description:创建system_state模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE system_state_create(void);

/**
 * Function:   销毁system_state模块
 * Description:销毁system_state模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE system_state_destroy(void);

/**
 * Function:   重置system_state数据
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE system_state_reset(void);

/**
 * Function:   清除重启次数
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE system_state_clear_reboot_count(void);

/**
 * Function:   system_state模块定时处理入口
 * Description:system_state模块定时处理入口
 * Input:       无
 * Output:      无
 * Return:      GM_SUCCESS——成功；其它错误码——失败
 * Others:      1秒调用1次
 */
GM_ERRCODE system_state_timer_proc(void);



typedef enum
{
    SYSBIT_START_TYPE = 1L,           //启动类型:0-重启,1——上电
    SYSBIT_UPLOADED_LOCATE = 1L<<1,   //上电后是否已上传过定位
    SYSBIT_UPLOADED_STATIC = 1L<<2,   //是否已发送静止点
    SYSBIT_UPLOADED_GPS = 1L<<3,      //修改IP后是否已上传GPS数据
    SYSBIT_CHARGE_OPEN = 1L<<4,       //充电是否开启(软件控制)
    SYSBIT_ACC_CHECK_MODE = 1L<<5,    //ACC检测模式:0——震动检测；  1——ACC线检测
    SYSBIT_RELAY_STATE = 1L<<6,       //设备relay端口状态:           0——低电平（恢复油电）；  1——高电平（断油电）  
    SYSBIT_RELAY_STATE_USER = 1L<<7,  //用户设置relay端口状态:0——低电平（恢复油电）；  1——高电平（断油电）  
    
    SYSBIT_ALARM_NO_POWER = 1L<<16,   //断电报警
    SYSBIT_ALARM_BAT_LOW = 1L<<17,    //电池低电报警
    SYSBIT_ALARM_LOW_POWER = 1L<<18,  //电源电压过低报警
    SYSBIT_ALARM_HIGH_POWER = 1L<<19, //电源电压过高报警
    SYSBIT_ALARM_SHOCK = 1L<<20,      //震动报警
    SYSBIT_ALARM_SPEED = 1L<<21,      //超速报警
    SYSBIT_ALARM_FAKE_BASE = 1L<<22,  //伪基站报警
    SYSBIT_ALARM_COLLIDE = 1L<<23,    //碰撞报警
    SYSBIT_ALARM_SPEED_UP = 1L<<24,   //急加速报警
    SYSBIT_ALARM_SPEED_DOWN = 1L<<25, //急减速报警
    SYSBIT_ALARM_TURN_OVER = 1L<<26,  //翻车报警
    SYSBIT_ALARM_SHARP_TURN = 1L<<27, //急转弯报警
    SYSBIT_ALARM_PULL_DOWN = 1L<<28,  //拆动报警
    SYSBIT_ALARM_MOVE = 1L<<29,       //车辆移动报警（使用原位移报警位）
}StateBitEnum;


/**
 * Function:    获取状态位
 * Description: bit0 ——启动类型:0-重启,1——上电
			    bit1 ——上电后是否已上传过定位
			    bit2 ——是否已发送静止点
			    bit3 ——修改IP后是否已上传GPS数据
			    bit4 ——充电是否开启(软件控制)
			    bit5 ——ACC检测模式:0——震动检测；  1——ACC线检测
			    bit6 ——设备relay端口状态:           0——低电平（恢复油电）；  1——高电平（断油电）  
			    bit7 ——用户设置relay端口状态:0——低电平（恢复油电）；  1——高电平（断油电）
			    bit8 ——备用
			    bit9 ——备用
			    bit10——备用
			    bit11——备用
			    bit12——备用
			    bit13——备用
			    bit14——备用
			    bit15——备用
			    
			    bit16——断电报警,
			    bit17——电池低电报警
			    bit18——电源电压过低报警
			    bit19——电源电压过高报警
			    bit20——震动报警
			    bit21——超速报警
			    bit22——伪基站报警
			    bit23——碰撞报警
			    bit24——急加速报警
			    bit25——急减速报警
			    bit26——翻车报警
			    bit27——急转弯报警
			    bit28——拆动报警
			    bit29——车辆移动报警
			    bit30——备用
			    bit31——备用
 * Input:       无
 * Output:      无
 * Return:      状态位
 * Others:      
 */
U32 system_state_get_status_bits(void);

/**
 * Function:   获取启动时间
 * Description:
 * Input:      无
 * Output:     无
 * Return:     启动时间（秒）
 * Others:      
 */
U32 system_state_get_start_time(void);

/**
 * Function:   获取启动原因
 * Description:
 * Input:      add_counts:是否增加统计次数
 * Output:     无
 * Return:     启动原因
 * Others:      
 */
BootReason system_state_get_boot_reason(bool add_counts);

/**
 * Function:   获取启动原因字符串
 * Description:
 * Input:      reason:启动原因
 * Output:     无
 * Return:     启动原因字符串
 * Others:      
 */
const char* system_state_get_boot_reason_str(BootReason reason);


/**
 * Function:   设置启动原因
 * Description:
 * Input:      无
 * Output:     无
 * Return:     GM_SUCCESS——成功；其它错误码——失败
 * Others:      
 */
GM_ERRCODE system_state_set_boot_reason(const BootReason boot_reason);

/**
 * Function:   获取重启次数
 * Description:
 * Input:      boot_reason:重启原因
 * Output:     无
 * Return:     指定重启原因的重启次数
 * Others:      
 */
U32 system_state_get_reboot_counts(const BootReason boot_reason);

/**
 * Function:   获取工作状态
 * Description:
 * Input:      无
 * Output:     无
 * Return:     工作状态
 * Others:      
 */
SystemWorkState system_state_get_work_state(void);

/**
 * Function:   设置车辆状态
 * Description:
 * Input:      无
 * Output:     无
 * Return:     GM_SUCCESS——成功；其它错误码——失败
 * Others:      
 */
GM_ERRCODE system_state_set_work_state(const SystemWorkState work_state);

/**
 * Function:   设置工作状态
 * Description:
 * Input:      无
 * Output:     无
 * Return:     GM_SUCCESS——成功；其它错误码——失败
 * Others:      
 */
GM_ERRCODE system_state_set_vehicle_state(const VehicleState work_state);

/**
 * Function:   获取车辆状态
 * Description:
 * Input:      无
 * Output:     无
 * Return:     车辆状态
 * Others:      
 */
VehicleState system_state_get_vehicle_state(void);

bool system_state_has_reported_gps_since_boot(void);

GM_ERRCODE system_state_set_has_reported_gps_since_boot(bool state);

bool system_state_has_reported_lbs_since_boot(void);

GM_ERRCODE system_state_set_has_reported_lbs_since_boot(bool state);

bool system_state_has_reported_static_gps(void);

GM_ERRCODE system_state_set_has_reported_static_gps(bool state);

bool system_state_has_reported_gps_since_modify_ip(void);

GM_ERRCODE system_state_set_reported_gps_since_modify_ip(bool state);

bool system_state_get_defence(void);

GM_ERRCODE system_state_set_defence(bool state);

bool system_state_get_high_voltage_alarm(void);

GM_ERRCODE system_state_set_high_voltage_alarm(bool state);
/*bit16——断电报警-------------------
  bit17——电池低电报警-------------------
  bit18——电源电压过低报警
  bit19——震动报警
  bit20——超速报警
  bit21——伪基站报警
  bit22——电源电压过高报警
  bit23——碰撞报警
  bit24——急加速报警
  bit25——急减速报警
  bit26——翻车报警
  bit27——急转弯报警
  bit28——拆动报警
  bit29——车辆移动报警
*/
bool system_state_get_power_off_alarm(void);

GM_ERRCODE system_state_set_power_off_alarm(bool state);

bool system_state_get_battery_low_voltage_alarm(void);

GM_ERRCODE system_state_set_battery_low_voltage_alarm(bool state);

bool system_state_get_shake_alarm(void);

GM_ERRCODE system_state_set_shake_alarm(bool state);

bool system_state_get_overspeed_alarm(void);

GM_ERRCODE system_state_set_overspeed_alarm(bool state);

bool system_state_get_fakecell_alarm(void);

GM_ERRCODE system_state_set_fakecell_alarm(bool state);

bool system_state_get_highvoltage_alarm(void);

GM_ERRCODE system_state_set_highvoltage_alarm(bool state);

bool system_state_get_collision_alarm(void);

GM_ERRCODE system_state_set_collision_alarm(bool state);

bool system_state_get_speed_up_alarm(void);

GM_ERRCODE system_state_set_speed_up_alarm(bool state);

bool system_state_get_speed_down_alarm(void);

GM_ERRCODE system_state_set_speed_down_alarm(bool state);

bool system_state_get_turn_over_alarm(void);

GM_ERRCODE system_state_set_turn_over_alarm(bool state);

bool system_state_get_sharp_turn_alarm(void);

GM_ERRCODE system_state_set_sharp_turn_alarm(bool state);

bool system_state_get_remove_alarm(void);

GM_ERRCODE system_state_set_remove_alarm(bool state);

bool system_state_get_move_alarm(void);

GM_ERRCODE system_state_set_move_alarm(bool state);

bool system_state_get_move_alarm(void);

GM_ERRCODE system_state_set_move_alarm(bool state);

bool system_state_get_has_started_charge(void);

GM_ERRCODE system_state_set_has_started_charge(bool state);

bool system_state_get_acc_is_line_mode(void);

GM_ERRCODE system_state_set_acc_is_line_mode(bool state);

bool system_state_get_device_relay_state(void);

GM_ERRCODE system_state_set_device_relay_state(bool state);

bool system_state_get_user_relay_state(void);

bool system_state_is_cold_boot(void);

GM_ERRCODE system_state_set_cold_boot(bool state);

GM_ERRCODE system_state_set_user_relay_state(bool state);

U8 system_state_get_extern_battery_voltage_grade(void);

GM_ERRCODE system_state_set_extern_battery_voltage_grade(U8 voltage_grade);

void system_state_set_mileage(U64 mileage);

U64 system_state_get_mileage(void);

void system_state_set_bin_checksum(U32 check_sum);

U32 system_state_get_bin_checksum(void);

void system_state_set_gsensor_type(GSensorType gsensor_type);

GSensorType system_state_get_gsensor_type(void);

u32 system_state_get_last_good_time(void);
u32 system_state_get_call_ok_count(void);

void system_state_set_gpss_reboot_reason(const char *reason);
const char *system_state_get_gpss_reboot_reason(void);

void system_state_set_ip_cache(U8 index,const U8* ip);
void system_state_get_ip_cache(U8 index,U8* ip);


#endif

