/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        main.c
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-02-28
 * Description:      APP主函数头
 * Others:      
 * Function List:    
    1. APP主入口
 * History: 
    1. Date:         2019-02-28
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */


#include <gm_type.h>
#include <gm_timer.h>
#include <gm_uart.h>
#include <gm_system.h>
#include <gm_stdlib.h>
#include "main.h"
#include "error_code.h"
#include "utility.h"
#include "hard_ware.h"
#include "led.h"
#include "system_state.h"
#include "config_service.h"
#include "g_sensor.h"
#include "uart.h"
#include "watch_dog.h"
#include "log_service.h"
#include "relay.h"
#include "gsm.h"
#include "sms.h"
#include "gps.h"
#include "gprs.h"
#include "auto_test.h"
#include "update_file.h"
#include "bms.h"
#include "gm_app.h"
#include "gm_fs.h"

static void timer_self_test_start(void);
static void upload_boot_log(void);


extern s32 TK_dll_set_sb(S32 val);
extern void app_main_entry(void);
extern void DLL_init_SB(void);

extern const s32 service_feature;
u32 dll_version=0;
u32 dll_edition=0;
s32 g_exportfunc[3]={0};

void Service_Entry(dll_struct* dll)
{
	TK_dll_set_sb(service_feature);

	dll_version = dll->header.version;
	dll_edition = dll->header.feature;
	dll->export_func_count = sizeof(g_exportfunc) / sizeof(g_exportfunc[0]);
	dll->export_funcs = g_exportfunc;
	
	dll->export_funcs[0] = (S32)DLL_init_SB;
	dll->export_funcs[1] = (S32)app_main_entry;
}


void DLL_init_SB(void)
{
	TK_dll_set_sb(service_feature);
}




//注意主函数里面调用其他函数的顺序有严格要求
void app_main_entry(void)
{
	//基础部分
	util_create();
	uart_create();
	system_state_create();
	config_service_read_from_local();
    config_service_save_to_local();

	gsm_create();
    //其中创建网络服务
	gprs_create();
	sms_create();
	hard_ware_create();
	led_create();
	relay_create();
	watch_dog_create();
	gps_create();
	bms_create();

	GM_StartTimer(GM_TIMER_MAIN_UPLOAD_LOG, 2*TIM_GEN_1SECOND, upload_boot_log);
	
	g_sensor_create();
	system_state_set_work_state(GM_SYSTEM_STATE_WORK);
	gps_power_on(true);
	
	
	//延迟30秒再开始自检，否则还没有获取到IMEI会误入自检
	if(GM_REBOOT_POWER_ON == system_state_get_boot_reason(false))
	{
		GM_StartTimer(GM_TIMER_SELF_CHECK_START, 30*TIM_GEN_1SECOND, timer_self_test_start);
		LOG(INFO,"It may enter self check 30 seconds later.");
	}

	
    //更精准的定时器
    GM_CreateKalTimer(0);
    GM_StartKalTimer(0, kal_timer_1s_proc, GM_TICKS_1_SEC);

    GM_StartTimer(GM_TIMER_10MS_MAIN, TIM_GEN_10MS, timer_10ms_proc);
	GM_StartTimer(GM_TIMER_1S_MAIN, TIM_GEN_1SECOND, timer_1s_proc);
}


//唤醒时用这个定时器处理
void timer_10ms_proc(void)
{
	if (GM_SYSTEM_STATE_WORK == system_state_get_work_state())
	{
		GM_StartTimer(GM_TIMER_10MS_MAIN, TIM_GEN_10MS, timer_10ms_proc);
		led_timer_proc();
		g_sensor_timer_proc();
    	gprs_timer_proc();
		hard_ware_timer_proc();
		bms_timer_proc();
	}
}

void timer_1s_proc(void)
{
	if (GM_SYSTEM_STATE_WORK == system_state_get_work_state())
	{
		GM_StartTimer(GM_TIMER_1S_MAIN, TIM_GEN_1SECOND, timer_1s_proc);
		util_timer_proc();
		watch_dog_timer_proc();
		uart_timer_proc();
		system_state_timer_proc();
		relay_timer_proc();
		auto_test_timer_proc();
	}
}

//休眠时用这个定时器处理
void kal_timer_1s_proc(void* p_arg)
{
	if (GM_SYSTEM_STATE_SLEEP == system_state_get_work_state())
	{
		//降频处理
		led_timer_proc();
		g_sensor_timer_proc();
		gprs_timer_proc();
		hard_ware_timer_proc();
		bms_timer_proc();

		//正常处理
		util_timer_proc();
		watch_dog_timer_proc();
		uart_timer_proc();
		system_state_timer_proc();
		relay_timer_proc();
		auto_test_timer_proc();
	}
}

static void timer_self_test_start(void)
{
	auto_test_create(true);
}

static void upload_boot_log(void)
{
	JsonObject* p_log_root = NULL;
	char kernel_version[50] = {0};
	GPSChipType gps_chip_type = GM_GPS_TYPE_UNKNOWN;
	char gsensor_type_str[20] = {0};
	ConfigDeviceTypeEnum dev_type_id = DEVICE_MAX;
	char check_sum_str[9] = {0};
	U8 protocol_type = 0;
	U8 protocol_ver = 0;
	U32 app_check_sum = 0;
	float voltage = 0;
		   
	p_log_root = json_create();
	json_add_string(p_log_root, "event", "boot");
	
	json_add_string(p_log_root, "reason", system_state_get_boot_reason_str(system_state_get_boot_reason(true)));
	
	system_state_set_boot_reason(GM_RREBOOT_UNKNOWN);
	
	
	json_add_string(p_log_root, "app version", VERSION_NUMBER);
	
	json_add_string(p_log_root, "app build time", SW_APP_BUILD_DATE_TIME);

    //lz modified for only save at most 2 copies. 
    // master/minor both exist , use master
    if (GM_FS_CheckFile(UPDATE_TARGET_IMAGE) >= 0)
    {
        app_check_sum = update_filemod_get_checksum(UPDATE_TARGET_IMAGE);
    }
    else  // UPDATE_MINOR_IMAGE 肯定存在, 否则不可能运行
    {
        app_check_sum = update_filemod_get_checksum(UPDATE_MINOR_IMAGE);
    }
	GM_snprintf(check_sum_str, 8, "%4X", app_check_sum);
	system_state_set_bin_checksum(app_check_sum);

	json_add_string(p_log_root, "app check sum",check_sum_str);
	
    GM_ReleaseVerno((U8*)kernel_version);
	GM_sprintf(kernel_version + GM_strlen(kernel_version), "(%s)",GM_BuildDateTime());
	json_add_string(p_log_root, "kernel version", kernel_version);

	config_service_get(CFG_DEVICETYPE, TYPE_SHORT, &dev_type_id, sizeof(U16));
	json_add_string(p_log_root, "device type", (char*)config_service_get_device_type(dev_type_id));
	
	hard_ware_get_power_voltage(&voltage);
	json_add_double(p_log_root, "power voltage", voltage);

	config_service_get(CFG_GPS_TYPE, TYPE_BYTE, &gps_chip_type, sizeof(gps_chip_type));
	json_add_int(p_log_root, "gps chip", gps_chip_type);

	g_sensor_get_typestr(system_state_get_gsensor_type(),gsensor_type_str,sizeof(gsensor_type_str));
	json_add_string(p_log_root, "g_sensor type", gsensor_type_str);
	
	json_add_string(p_log_root, "server", (char*)config_service_get_pointer(CFG_SERVERADDR));

	config_service_get(CFG_PROTOCOL, TYPE_BYTE, &protocol_type, sizeof(protocol_type));
	json_add_int(p_log_root, "protocol", protocol_type);
	config_service_get(CFG_PROTOCOL_VER, TYPE_BYTE, &protocol_ver, sizeof(protocol_ver));
	json_add_int(p_log_root, "protocol_ver", protocol_ver);

	json_add_int(p_log_root, "system state bits", system_state_get_status_bits());

	json_add_int(p_log_root, "power on", system_state_get_reboot_counts(GM_REBOOT_POWER_ON));
	json_add_int(p_log_root, "exceptions reboots", system_state_get_reboot_counts(GM_RREBOOT_UNKNOWN));
	json_add_int(p_log_root, "GPRS reboots", system_state_get_reboot_counts(GM_REBOOT_GPRS));
	json_add_int(p_log_root, "GPS reboots", system_state_get_reboot_counts(GM_REBOOT_GPS));
	json_add_int(p_log_root, "command reboots", system_state_get_reboot_counts(GM_REBOOT_CMD));
	json_add_int(p_log_root, "upgrade reboots", system_state_get_reboot_counts(GM_REBOOT_UPDATE));
	json_add_int(p_log_root, "check params reboots", system_state_get_reboot_counts(GM_REBOOT_CHECKPARA));
	
	json_add_int(p_log_root, "gprs_last_good", system_state_get_last_good_time());
	json_add_int(p_log_root, "gprs_ok_count", system_state_get_call_ok_count());
	json_add_string(p_log_root, "gpss_reboot_reason", system_state_get_gpss_reboot_reason());

	log_service_upload(INFO,p_log_root);
}



