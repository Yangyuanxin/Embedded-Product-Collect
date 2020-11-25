/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        auto_test.c
 * Author:           王志华     
 * Version:          1.0
 * Date:             2019-05-25
 * Description:      定时输出日志到串口，供自动化测试程序检测 
 * Others:           
 * Function List:    
    1. 创建auto_test模块
    2. 销毁auto_test模块
    3. auto_test模块定时处理入口
 * History: 
    1. Date:         2019-05-25
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */ 

#include <gm_system.h>
#include <gm_stdlib.h>
#include <gm_timer.h>
#include <gm_fs.h>
#include "auto_test.h"
#include "system_state.h"
#include "log_service.h"
#include "hard_ware.h"
#include "gps.h"
#include "g_sensor.h"
#include "config_service.h"
#include "gsm.h"
#include "gprs.h"
#include "system_state.h"
#include "utility.h"
#include "relay.h"
#include "agps_service.h"
#include "gps_service.h"
#include "applied_math.h"
#include "uart.h"
#include "uart.h"

#pragma diag_suppress 870 

//5分钟自检成功
#define SELF_TEST_TIME_STANDARD (5*SECONDS_PER_MIN)
#define TEST_FAIL_FILE L"Z:\\goome\\GmTestFailFile\0"
#define REALY_ACC_PERIOD 20
#define AUTO_TEST_MAGIC_NUMBER 0xFEFEFEFE
#define BATTERY_VOLTAGE_STANDARD 3.9



typedef struct
{
	//数据长度
	U16 data_len;

	//16比特CRC
    U16 crc;
	
	//特征码
	U32 magic;	
	
	//必选项，一次成功即成功的项
	bool required_items_test_pass;

	//必选项，一次失败即失败的项
	bool required_items_test_fail;

	//必选项测试结果，true通过
	bool required_items_result;

	//可选项，一次成功即成功的项
	bool optional_items_test_pass;

	//可选项，一次失败即失败的项
	bool optional_items_test_fail;

	//必选项测试结果，true通过
	bool all_items_result;
	
	//必选检查项:测试时长,一次成功即成功
	U16 test_time_seconds;

	char app_version[64];
	char kernal_version[64];

	//必选检查项:外电电压，一次失败即失败
	bool power_fail;
	float voltage_min;
	float voltage_max;
	
	//必选检查项:SIM卡，一次失败即失败
	bool sim_fail;
	char imei[20];
	char chip_rid[50];
	u8 iccid[30];

	//必选检查项:校准，一次失败即失败
	bool adjust_fail;
	U8 adjust_code[65];
	bool channels_adjust_fail[4];
	
	//必选检查项:网络，一次失败即失败
	bool network_fail;
	U32 csq_counts;
	float csq_avg;
	U8 csq_min;
	U16 network_fail_counts;
	//交给工具检查这2个次
	U16 heart_counts;
	U16 location_data_counts;
	

	//可选检查项：电池，一次成功即成功
	bool battery_ok;
	float battery_voltage;

	//可选检查项：ACC,一次失败即失败
	bool acc_fail;
	U8 acc_on_count;
    U8 acc_off_count;

	//必选检查项:重启次数，一次失败即失败
	bool reboot_counts_fail;
	//断网重启次数
	U16 reboot_for_power_counts;
	//断网重启次数
	U16 reboot_for_net_counts;
	//升级重启次数
	U16 reboot_for_upgrade_counts;
	//异常重启次数
	U16 reboot_for_exception_counts;

	//必选检查项:GPS,一次失败即失败
	bool gps_fail;
	//首次定位时间
	U16 first_fix_time;
	float snr_avg;
	U32 snr_counts;


	//必选检查项:运动传感，一次失败即失败
	bool g_sensor_fail;
	U32 g_sensor_error_counts;
	U32 g_sensor_sleep_time;
	U32 g_sensor_shake_counts;
}AutoTestResult;

typedef struct
{
	bool is_self_check;
	bool start;
	AutoTestResult test_result;
}AutoTest;


static AutoTest s_auto_test;

static void init_test_result(AutoTestResult* p_test_result);
static void auto_test_self_test(void);
static void struct_to_jsonstr(const AutoTestResult result,char* json_str);
static GM_ERRCODE read_result_from_file(const U16* FileName,AutoTestResult* p_result);
static void auto_test_save_to_file(const U16* FileName);


static void auto_test_check_power(void);
static void auto_test_check_sim(void);
static void auto_test_check_adjust(void);
static void auto_test_check_network(void);
static void auto_test_check_battery_status(void);
static void auto_test_check_acc(void);
static void auto_test_check_reboot(void);
static void auto_test_check_gps_location(void);
static void auto_test_check_gsensor(void);

static U32 auto_test_calc_hashnr(const u8 *key, u32 len);
static void auto_test_check_chip_rid(void);
static bool is_device_05(ConfigDeviceTypeEnum dev_type);

GM_ERRCODE auto_test_create(bool is_self_check)
{

	//防止重复多次进入自检
	if (s_auto_test.start && is_self_check)
	{
		LOG(INFO,"Already start self check!");
		return GM_ERROR_STATUS;
	}

	//如果不是缺省IMEI，不自检
   	if (!config_service_is_default_imei() && is_self_check)
   	{
		LOG(INFO,"It is not default imei!");
   		return GM_ERROR_STATUS;
   	}
	
	LOG(INFO,"start test,is_self_check=%d",is_self_check);
	s_auto_test.start = true;
	s_auto_test.is_self_check = is_self_check;
	
	init_test_result(&s_auto_test.test_result);
	
	log_service_enable_print(false);
	config_service_set_test_mode(true);

	auto_test_check_chip_rid();
	
    GM_ReleaseVerno((U8*)s_auto_test.test_result.kernal_version);
	
	return GM_SUCCESS;
}
 
GM_ERRCODE auto_test_destroy(void)
{
	LOG_TEST("Exit test mode.");
	s_auto_test.start = false;
	
	log_service_enable_print(true);
	config_service_set_test_mode(false);
	
	return GM_SUCCESS;
}

GM_ERRCODE auto_test_reset(void)
{
	init_test_result(&s_auto_test.test_result);
    GM_FS_Delete(TEST_FAIL_FILE);
	return GM_SUCCESS;
}

GM_ERRCODE auto_test_timer_proc(void)
{
	if(!s_auto_test.start)
	{
		init_test_result(&s_auto_test.test_result);
		return GM_SUCCESS;
	}
	
	s_auto_test.test_result.test_time_seconds++;
	if (s_auto_test.test_result.test_time_seconds % (REALY_ACC_PERIOD/2) == 0)
	{
		if(system_state_get_device_relay_state())
		{
			relay_off();
		}
		else
		{
			relay_on(false);
		}
	}

	
	auto_test_self_test();
	
	return GM_SUCCESS;
}

static void init_test_result(AutoTestResult* p_test_result)
{
	if (NULL == p_test_result)
	{
		return;
	}
	
	GM_memset(p_test_result,0,sizeof(AutoTestResult));
	
	p_test_result->data_len = sizeof(AutoTestResult) - 2*sizeof(U16) - sizeof(U32);
	p_test_result->magic = AUTO_TEST_MAGIC_NUMBER;
	p_test_result->voltage_min = 200;
	p_test_result->csq_min = 31;
}

static void auto_test_self_test(void)
{
	//IMEI
	gsm_get_imei((U8*)s_auto_test.test_result.imei);

	//APP版本
	GM_snprintf(s_auto_test.test_result.app_version,sizeof(s_auto_test.test_result.app_version),"%s(%X)", VERSION_NUMBER, system_state_get_bin_checksum());
	
	auto_test_check_chip_rid();
	auto_test_check_power();  
	auto_test_check_sim();
	auto_test_check_adjust();
	auto_test_check_network();
	auto_test_check_battery_status();
	auto_test_check_acc();
	auto_test_check_reboot();
	auto_test_check_gps_location();
	auto_test_check_gsensor();
	
   	if (s_auto_test.is_self_check)
	{
		
		s_auto_test.test_result.required_items_test_pass = ((s_auto_test.test_result.heart_counts > s_auto_test.test_result.test_time_seconds/GOOME_HEARTBEAT_DEFAULT*0.8) &&
				                                            (s_auto_test.test_result.location_data_counts > s_auto_test.test_result.test_time_seconds/CONFIG_UPLOAD_TIME_DEFAULT*0.8) &&
	   	                                                    (s_auto_test.test_result.test_time_seconds > SELF_TEST_TIME_STANDARD) &&
			                                                (s_auto_test.test_result.g_sensor_shake_counts >= G_SHAKE_BUF_LEN));
	}
	else
	{
		
		s_auto_test.test_result.required_items_test_pass = ((s_auto_test.test_result.heart_counts >= 1) &&
														    (s_auto_test.test_result.location_data_counts >= 1) &&
														    (s_auto_test.test_result.test_time_seconds > 30) &&
														    (s_auto_test.test_result.g_sensor_shake_counts > 2));
	}
	
	s_auto_test.test_result.required_items_result = (!s_auto_test.test_result.required_items_test_fail && s_auto_test.test_result.required_items_test_pass);
	
	s_auto_test.test_result.optional_items_test_pass = s_auto_test.test_result.battery_ok;
	s_auto_test.test_result.optional_items_test_fail = s_auto_test.test_result.acc_fail;
	
	s_auto_test.test_result.all_items_result = (!s_auto_test.test_result.required_items_test_fail && 
												s_auto_test.test_result.required_items_test_pass && 
												s_auto_test.test_result.optional_items_test_pass &&
												!s_auto_test.test_result.optional_items_test_fail);

	if(s_auto_test.test_result.test_time_seconds % SECONDS_PER_MIN == 0)
	{
		auto_test_save_to_file(TEST_FAIL_FILE);
	}
	
	if(!s_auto_test.is_self_check)
	{
		char test_result[1024] = {0};
		auto_test_get_result(false,test_result);	
		GM_snprintf(test_result + GM_strlen(test_result), (1024 - GM_strlen(test_result)), "\r\n");
		uart_write(GM_UART_DEBUG, (U8*)test_result, GM_strlen(test_result));
	}
		                                          
}

void auto_test_acc_on(void)
{
    s_auto_test.test_result.acc_on_count++;  
}

void auto_test_acc_off(void)
{
   s_auto_test.test_result.acc_off_count++;
}

static GM_ERRCODE read_result_from_file(const U16* FileName,AutoTestResult* p_result)
{
	U32 file_len = 0;
    S32 file_handle = -1;
	S32 ret = -1;
    U16 crc = 0;
    
    file_handle = GM_FS_Open(FileName, GM_FS_READ_ONLY | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (file_handle < 0)
    {
        LOG(INFO,"Failed to open self check result file!");
        return GM_SYSTEM_ERROR;
    }
    
    ret = GM_FS_Read(file_handle, (void *)p_result, sizeof(AutoTestResult), &file_len);
    if (ret < 0)
    {
        LOG(ERROR,"Failed to read system state file[%d]", ret);
        GM_FS_Close(file_handle);
        return GM_SYSTEM_ERROR;
    }
    
    GM_FS_Close(file_handle);
    
    
    crc = applied_math_calc_common_crc16((U8*)(p_result->required_items_test_pass), p_result->data_len);
    
    
    if (p_result->magic != AUTO_TEST_MAGIC_NUMBER)
    {
        LOG(ERROR,"magic error:%x, ", s_auto_test.test_result.magic);
        return GM_SYSTEM_ERROR;
    }
    
    if (p_result->crc != crc)
    {
        LOG(ERROR,"crc error:%x, %x", p_result->crc, crc);
        return GM_SYSTEM_ERROR;
    }
	return GM_SUCCESS;
}

void auto_test_save_to_file(const U16* FileName)
{
	U32 file_len = 0;
    S32 file_handle = -1;
	S32 ret = -1;
	
	if (!config_service_is_default_imei())
	{
		return;
	}
	
    file_handle = GM_FS_Open(FileName, GM_FS_READ_WRITE | GM_FS_ATTR_ARCHIVE | GM_FS_CREATE);
    if (file_handle < 0)
    {
        LOG_TEST("Failed to open test file[%d]", file_handle);
    }
	
    s_auto_test.test_result.crc = applied_math_calc_common_crc16((u8*)&(s_auto_test.test_result.required_items_test_pass), s_auto_test.test_result.data_len);
    
    ret = GM_FS_Write(file_handle, (void *)&s_auto_test.test_result, sizeof(s_auto_test.test_result), &file_len);
    if (ret < 0)
    {
        LOG_TEST("Failed to write test file[%d]", ret);
        GM_FS_Close(file_handle);
    }
	else
	{
		LOG_TEST("Succeed to write test file", ret);
	}
	
	
	GM_FS_Commit(file_handle);
    GM_FS_Close(file_handle);

}

void auto_test_get_result(bool is_history,char* json_str)
{
	if (is_history)
	{
		AutoTestResult test_result;
		init_test_result(&test_result);
		read_result_from_file(TEST_FAIL_FILE,&test_result); 
		struct_to_jsonstr(test_result,json_str);
	}
	else
	{
		struct_to_jsonstr(s_auto_test.test_result,json_str);
	}
}	

static void struct_to_jsonstr(const AutoTestResult result,char* json_str)
{
	JsonObject* p_result_root = json_create();
	
	json_add_string(p_result_root, "event", "selftest");
	json_add_string(p_result_root, "chip_rid", result.chip_rid);
	json_add_string(p_result_root, "imei", (char*)result.imei);
	json_add_string(p_result_root, "iccid", (char*)result.iccid);
	
	json_add_int(p_result_root, "required_items_result", result.required_items_result);
	json_add_int(p_result_root, "all_items_result", result.all_items_result);
	
	json_add_int(p_result_root, "test_time_seconds", result.test_time_seconds);
	
	json_add_string(p_result_root, "app_version", result.app_version);
	json_add_string(p_result_root, "kernal_version", result.kernal_version);
	
	json_add_double(p_result_root, "voltage_min", result.voltage_min);
	json_add_double(p_result_root, "voltage_max", result.voltage_max);
	
	json_add_int(p_result_root, "sim_fail", result.sim_fail);
	json_add_int(p_result_root, "network_fail", result.network_fail);
	json_add_int(p_result_root, "csq_counts", result.csq_counts);
	json_add_double(p_result_root, "csq_avg", result.csq_avg);
	json_add_int(p_result_root, "csq_min", result.csq_min);
	
	json_add_int(p_result_root, "network_fail_counts", result.network_fail_counts);
	json_add_int(p_result_root, "heart_counts", result.heart_counts);
	json_add_int(p_result_root, "location_data_counts", result.location_data_counts);
	
	json_add_double(p_result_root, "battery_voltage", result.battery_voltage);
	
	json_add_int(p_result_root, "acc_on_count", result.acc_on_count);
	json_add_int(p_result_root, "acc_off_count", result.acc_off_count);
	
	json_add_int(p_result_root, "reboot_for_power_counts", result.reboot_for_power_counts);
	json_add_int(p_result_root, "reboot_for_net_counts", result.reboot_for_net_counts);
	json_add_int(p_result_root, "reboot_for_upgrade_counts", result.reboot_for_upgrade_counts);
	json_add_int(p_result_root, "reboot_for_exception_counts", result.reboot_for_exception_counts);
	json_add_int(p_result_root, "reboot_for_hardware_counts", result.reboot_for_power_counts + result.reboot_for_exception_counts);
	
	json_add_int(p_result_root, "gps_fail", result.gps_fail);
	json_add_int(p_result_root, "first_fix_time", result.first_fix_time);
	json_add_double(p_result_root, "snr_avg", result.snr_avg);
	json_add_int(p_result_root, "snr_counts", result.snr_counts);
	json_add_int(p_result_root, "g_sensor_fail", result.g_sensor_fail);
	
	
	json_add_int(p_result_root, "g_sensor_error_counts", result.g_sensor_error_counts);
	json_add_int(p_result_root, "g_sensor_sleep_time", result.g_sensor_sleep_time);
	json_add_int(p_result_root, "g_sensor_shake_counts", result.g_sensor_shake_counts);

	json_print_to_buffer(p_result_root, json_str, sizeof(json_str) - 1);
	json_destroy(p_result_root);

}



//必选检查项:外电电压，一次失败即失败
static void auto_test_check_power(void)
{
	float voltage = 0;
	static U32 power_fail_counts = 0;
	if(util_clock() <= 20)
	{
		return;
	}
	hard_ware_get_power_voltage(&voltage);
	if(voltage > s_auto_test.test_result.voltage_max)
	{
		s_auto_test.test_result.voltage_max = voltage;
	}

	if(voltage > 7 && voltage < s_auto_test.test_result.voltage_min)
	{
		s_auto_test.test_result.voltage_min = voltage;
	}

	
	if (voltage > 7 && voltage < 28)
	{
		power_fail_counts++;
	}
	else
	{
		power_fail_counts = 0;
	}
	if (power_fail_counts > 10)
	{
		s_auto_test.test_result.power_fail = true;
		s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_power!");
	}
}

//必选检查项:SIM卡，一次失败即失败
static void auto_test_check_sim(void)
{
	if(util_clock() <= 20)
	{
		return;
	}
    if (GM_CheckSimValid())
    {
    	if(GM_SUCCESS != gsm_get_iccid(s_auto_test.test_result.iccid))
    	{
    		s_auto_test.test_result.sim_fail = true;
			s_auto_test.test_result.required_items_test_fail = true;
			LOG_TEST("Failed to auto_test_check_sim!");
    	}
    }
    else
    {
    	s_auto_test.test_result.sim_fail = true;
    	s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_sim!");
    }
}

//必选检查项:校准，一次失败即失败
static void auto_test_check_adjust(void)
{
    U8 index = 0;
	u16 nvLib = 0;
    s32 result = 0;
    l1cal_rampTable_T ramp;
    index = GM_GetBarcode(s_auto_test.test_result.adjust_code);
    if (index == 64)
    {
        if (GM_strlen((char*)s_auto_test.test_result.adjust_code) < 30)
        {
            s_auto_test.test_result.adjust_fail = true;
        }
    }
    else
    {
        s_auto_test.test_result.adjust_fail = true;
    }

	for(index = NVRAM_EF_L1_RAMPTABLE_GSM850_LID;index <= NVRAM_EF_L1_RAMPTABLE_PCS1900_LID;index++)
	{
	    nvLib = GM_ReadNvramLid(index);
	    GM_ReadWriteNvram(1, nvLib, 1, &ramp, sizeof(ramp), &result);
	    if ((ramp.rampData.power[0] == ramp.rampData.power[1]) && (ramp.rampData.power[1] == ramp.rampData.power[2]) &&
	        (ramp.rampData.power[2] == ramp.rampData.power[3]) && (ramp.rampData.power[3] == ramp.rampData.power[4]))
	    {
	        s_auto_test.test_result.channels_adjust_fail[index - NVRAM_EF_L1_RAMPTABLE_GSM850_LID] = true;
			s_auto_test.test_result.adjust_fail = true;
	    }
	    else if ((ramp.rampData.power[14] > 850) || (ramp.rampData.power[15] > 850))
	    {
	        s_auto_test.test_result.channels_adjust_fail[index - NVRAM_EF_L1_RAMPTABLE_GSM850_LID] = true;
		    s_auto_test.test_result.adjust_fail = true;
		    s_auto_test.test_result.required_items_test_fail = true;
			LOG_TEST("Failed to auto_test_check_adjust!");
	    }
	    else if (ramp.rampData.power[15] < 400)
	    {
	        s_auto_test.test_result.channels_adjust_fail[index - NVRAM_EF_L1_RAMPTABLE_GSM850_LID] = true;
			s_auto_test.test_result.adjust_fail = true;
			s_auto_test.test_result.required_items_test_fail = true;
			LOG_TEST("Failed to auto_test_check_adjust!");
	    }
	    else
	    {
	        //do nothing
	    }
	}
}

//必选检查项:网络，一次失败即失败
static void auto_test_check_network(void)
{
	U8 csq = gsm_get_csq();
	
	
	if(util_clock() <= 20)
	{
		return;
	}
	if (csq < s_auto_test.test_result.csq_min)
	{
		s_auto_test.test_result.csq_min = csq;
	}
	s_auto_test.test_result.csq_avg  = (s_auto_test.test_result.csq_avg*s_auto_test.test_result.csq_counts + csq)/(s_auto_test.test_result.csq_counts + 1);
	s_auto_test.test_result.csq_counts++;
	if (s_auto_test.test_result.csq_avg < 10)
	{
		s_auto_test.test_result.network_fail = true;
		s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_network1!");
	}

	s_auto_test.test_result.network_fail_counts = gprs_get_call_ok_count() > 0 ? (gprs_get_call_ok_count() - 1) : 0;
	s_auto_test.test_result.heart_counts = gps_service_get_heart_counts();
	s_auto_test.test_result.location_data_counts = gps_service_get_location_counts();
}

//可选检查项：电池电压，一次成功即成功
static void auto_test_check_battery_status(void)
{
    static bool battery_ok = false;
	static u16 check_count = 0;
	float power_voltage = 0;
	
	hard_ware_get_power_voltage(&power_voltage);

    if (power_voltage < 7)
    {
    	hard_ware_get_internal_battery_voltage(&s_auto_test.test_result.battery_voltage);
    	if (s_auto_test.test_result.battery_voltage >= BATTERY_VOLTAGE_STANDARD)
		{
			check_count++;
			if (check_count >= 30)
			{
				battery_ok = true;
				auto_test_save_to_file(TEST_FAIL_FILE);
				check_count = 0;
			}
		}
		else
		{
			check_count = 0;
			battery_ok = false;
		}
    }
	
    if (battery_ok)
    {
         s_auto_test.test_result.battery_ok = true;
    }
}


//可选检查项：ACC,一次失败即失败
static void auto_test_check_acc(void)
{
    if ((s_auto_test.test_result.acc_on_count < (s_auto_test.test_result.test_time_seconds/REALY_ACC_PERIOD - 1)) || (s_auto_test.test_result.acc_off_count < (s_auto_test.test_result.test_time_seconds/REALY_ACC_PERIOD - 1)))
    {
       s_auto_test.test_result.acc_fail = true;
	   s_auto_test.test_result.optional_items_test_fail = true;
    }
}

//必选检查项:重启次数，一次失败即失败
static void auto_test_check_reboot(void)
{
	s_auto_test.test_result.reboot_for_power_counts = system_state_get_reboot_counts(GM_REBOOT_POWER_ON);
	s_auto_test.test_result.reboot_for_net_counts = system_state_get_reboot_counts(GM_REBOOT_GPRS);
	s_auto_test.test_result.reboot_for_upgrade_counts = system_state_get_reboot_counts(GM_REBOOT_UPDATE); 
	s_auto_test.test_result.reboot_for_exception_counts = system_state_get_reboot_counts(GM_RREBOOT_UNKNOWN);

	//首次重启不知道原因，归结为意外重
	if(s_auto_test.test_result.reboot_for_exception_counts > 1)
	{
		s_auto_test.test_result.reboot_counts_fail = true;
		s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_exception_reboot!,counts=%d",s_auto_test.test_result.reboot_for_exception_counts);
	}

	if(s_auto_test.test_result.reboot_for_power_counts > 20)
	{
		s_auto_test.test_result.reboot_counts_fail = true;
		s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_power_reboot!,counts=%d",s_auto_test.test_result.reboot_for_power_counts);
	}

	
	if(s_auto_test.test_result.reboot_for_net_counts > 20)
	{
		s_auto_test.test_result.reboot_counts_fail = true;
		s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_net_reboot!,counts=%d",s_auto_test.test_result.reboot_for_net_counts);
	}

	
	if(s_auto_test.test_result.reboot_for_upgrade_counts > 10)
	{
		s_auto_test.test_result.reboot_counts_fail = true;
		s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_net_upgrade_reboot!,counts=%d",s_auto_test.test_result.reboot_for_upgrade_counts);
	}
}

//必选检查项:GPS,一次失败即失败
static void auto_test_check_gps_location(void)
{
	float snr_avg = 0;
    if (gps_is_fixed() && s_auto_test.test_result.test_time_seconds > SECONDS_PER_MIN)
    {	
    	float snr[5] = {0};
		U8 index = 0;

    	const SNRInfo* snr_info = gps_get_snr_array();
		for(index = 0;index < 5;index++)
		{
			snr[index] = snr_info[index].snr;
			//LOG_TEST("SNR:%f",snr[index]);
		}
		snr_avg  = applied_math_avage(snr,5);
		//LOG_TEST("snr_avg:%f",snr[index]);
		s_auto_test.test_result.snr_avg = (s_auto_test.test_result.snr_avg * s_auto_test.test_result.snr_counts + snr_avg)/(s_auto_test.test_result.snr_counts + 1);
		//LOG_TEST("test_result.snr_avg:%f",snr[index]);
		s_auto_test.test_result.snr_counts++;
		if (s_auto_test.test_result.snr_avg < 25)
		{
			s_auto_test.test_result.gps_fail = true;
			s_auto_test.test_result.required_items_test_fail = true;
			LOG_TEST("Failed to auto_test_check_gps_location1!");
		}
		
		s_auto_test.test_result.first_fix_time = gps_get_fix_time();
		if (s_auto_test.test_result.first_fix_time > 2*SECONDS_PER_MIN)
		{
			s_auto_test.test_result.gps_fail = true;
			s_auto_test.test_result.required_items_test_fail = true;
			LOG_TEST("Failed to auto_test_check_gps_location2!");
		}
    }
}

static void auto_test_check_gsensor(void)
{
	s_auto_test.test_result.g_sensor_error_counts = g_sensor_get_error_count();
	s_auto_test.test_result.g_sensor_shake_counts = g_sensor_get_shake_event_count_when_read();
    if (s_auto_test.test_result.g_sensor_error_counts > 10)
    {
        s_auto_test.test_result.g_sensor_fail = true;
		s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_gsensor because of read error!");
    }
    
    if (GM_SYSTEM_STATE_SLEEP == system_state_get_work_state())
    {
        s_auto_test.test_result.g_sensor_sleep_time++;
		
        s_auto_test.test_result.g_sensor_fail = true;
		s_auto_test.test_result.required_items_test_fail = true;
		LOG_TEST("Failed to auto_test_check_gsensor because of sleep!");
    }
}

static U32 auto_test_calc_hashnr(const u8 *key, u32 len)
{
    const u8 *end = key + len;
    u32 hash = 0;;  
    
    for (hash = 0; key < end; key++)
    {
        hash *= 16777619;
        hash ^= (u32) *(u8*) key;
    }  
    return (hash);
}

static void auto_test_check_chip_rid(void)
{
    u32 hashnr_rid[2] = {0};
    u8 chip_rid[20] = {0};
    GM_memset(chip_rid, 0x00, sizeof(chip_rid));
    GM_memset(hashnr_rid, 0x00, sizeof(hashnr_rid));
    if (0 == GM_GetChipRID(chip_rid))
    {
        hashnr_rid[0] = auto_test_calc_hashnr(&chip_rid[0], 8);
        hashnr_rid[1] = auto_test_calc_hashnr(&chip_rid[8], 8);
        GM_snprintf(s_auto_test.test_result.chip_rid,sizeof(s_auto_test.test_result.chip_rid),"%08X%08X,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", 
        hashnr_rid[0], hashnr_rid[1], chip_rid[0], chip_rid[1], chip_rid[2], chip_rid[3], chip_rid[4],chip_rid[5], chip_rid[6], chip_rid[7], chip_rid[8], 
        chip_rid[9], chip_rid[10], chip_rid[11], chip_rid[12], chip_rid[13], chip_rid[14], chip_rid[15]);
    }
}

static bool is_device_05(ConfigDeviceTypeEnum dev_type)
{
	switch(dev_type)
 	{
 		case DEVICE_GS05A:
		case DEVICE_GS05B:
		case DEVICE_GS05F:
		case DEVICE_GS05I:
		case DEVICE_GS05H:
			return true;

		default:
			return false;
 	}
}

void auto_test_repair_ramp(void)
{
	l1cal_rampTable_T  ramp;
	u16 nvLib;
	s32 result;
	u8 index = 0;
	ConfigDeviceTypeEnum dev_type = DEVICE_NONE;
	JsonObject* p_json_log = json_create();
	bool need_repair = false;

	config_service_get(CFG_DEVICETYPE, TYPE_SHORT, &dev_type, sizeof(U16));
	json_add_string(p_json_log, "event", "repair ramp");

	//850
	nvLib = GM_ReadNvramLid(NVRAM_EF_L1_RAMPTABLE_GSM850_LID);
    GM_ReadWriteNvram(1, nvLib, 1, &ramp, sizeof(ramp), &result);

	for (index=0; index<15; index++)
	{
		if (is_device_05(dev_type) && ramp.rampData.power[index] > 520)
		{
			json_add_int(p_json_log, "GSM850", index);
			json_add_int(p_json_log, "power", ramp.rampData.power[index]);
			ramp.rampData.power[index] = 520;
			need_repair = true;
		}
		else if (!is_device_05(dev_type) && ramp.rampData.power[index] > 540)
		{
			json_add_int(p_json_log, "GSM850", index);
			json_add_int(p_json_log, "power", ramp.rampData.power[index]);
			ramp.rampData.power[index] = 540;
			need_repair = true;
		}
		else
		{
			LOG(INFO,"GSM850,power[%d]:%d",index,ramp.rampData.power[index]);
		}
	}
	
	GM_ReadWriteNvram(0, nvLib, 1, &ramp, sizeof(ramp), &result);
	
	//900
    nvLib = GM_ReadNvramLid(NVRAM_EF_L1_RAMPTABLE_GSM900_LID);
    GM_ReadWriteNvram(1, nvLib, 1, &ramp, sizeof(ramp), &result);

	for (index=0; index<15; index++)
	{
		if (is_device_05(dev_type) && ramp.rampData.power[index] > 520)
		{
			json_add_int(p_json_log, "GSM900", index);
			json_add_int(p_json_log, "power", ramp.rampData.power[index]);
			ramp.rampData.power[index] = 520;
			need_repair = true;
		}
		else if (!is_device_05(dev_type) && ramp.rampData.power[index] > 540)
		{
			json_add_int(p_json_log, "GSM900", index);
			json_add_int(p_json_log, "power", ramp.rampData.power[index]);
			ramp.rampData.power[index] = 540;
			need_repair = true;
		}
		else
		{
			LOG(INFO,"GSM900,power[%d]:%d",index,ramp.rampData.power[index]);
		}
	}
	
	GM_ReadWriteNvram(0, nvLib, 1, &ramp, sizeof(ramp), &result);

	//1800
	nvLib = GM_ReadNvramLid(NVRAM_EF_L1_RAMPTABLE_DCS1800_LID);
    GM_ReadWriteNvram(1, nvLib, 1, &ramp, sizeof(ramp), &result);

	for (index=0; index<15; index++)
	{
		if (ramp.rampData.power[index] > 620)
		{
			json_add_int(p_json_log, "DCS1800", index);
			json_add_int(p_json_log, "power", ramp.rampData.power[index]);
			ramp.rampData.power[index] = 620;
			need_repair = true;
		}
		else
		{
			LOG(INFO,"DCS1800,power[%d]:%d",index,ramp.rampData.power[index]);
		}
	}

	GM_ReadWriteNvram(0, nvLib, 1, &ramp, sizeof(ramp), &result);

	//1900
	nvLib = GM_ReadNvramLid(NVRAM_EF_L1_RAMPTABLE_PCS1900_LID);
    GM_ReadWriteNvram(1, nvLib, 1, &ramp, sizeof(ramp), &result);

	for (index=0; index<15; index++)
	{
		if (ramp.rampData.power[index] > 620)
		{
			json_add_int(p_json_log, "PCS1900", index);
			json_add_int(p_json_log, "power", ramp.rampData.power[index]);
			ramp.rampData.power[index] = 620;
			need_repair = true;
		}
		else
		{
			LOG(INFO,"PCS1900,power[%d]:%d",index,ramp.rampData.power[index]);
		}
	}

	if (need_repair)
	{
		GM_ReadWriteNvram(0, nvLib, 1, &ramp, sizeof(ramp), &result);
		log_service_upload(INFO, p_json_log);
		hard_ware_reboot(GM_REBOOT_CHECKPARA,5);
	}
	else
	{
		json_destroy(p_json_log);
		p_json_log = NULL;
	}
}


