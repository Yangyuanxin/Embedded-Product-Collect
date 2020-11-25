/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        command.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-04-19
 * Description:      
 * Others:      
 * Function List:    
    1. 创建command模块
    2. 销毁command模块
    3. command模块定时处理入口
 * History: 
    1. Date:         2019-04-19
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#include <stdarg.h>
#include <gm_stdlib.h>
#include <gm_timer.h>
#include <gm_system.h>
#include <gm_memory.h>
#include "command.h"
#include "utility.h"
#include "gsm.h"
#include "applied_math.h"
#include "hard_ware.h"
#include "config_service.h"
#include "log_service.h"
#include "system_state.h"
#include "relay.h"
#include "gprs.h"
#include "gps_service.h"
#include "main.h"
#include "agps_service.h"
#include "gm_sms.h"
#include "g_sensor.h"
#include "auto_test.h"
#include "watch_dog.h"
#include "bms.h"

#pragma diag_suppress 870 

#define CMD_NAME_MAX_LEN 11

typedef enum
{
	CMD_DEVICE_TYPE,
	CMD_APN,
	CMD_APN_INIT,
	CMD_APNC,
	CMD_AUTO_APN,
	CMD_SERVER,
	CMD_PROTOCOL,
	CMD_UPLOAD_TIM,
	CMD_GMT ,
	CMD_SYSTEM_RESET,
	CMD_FACTORY_ALL,
	CMD_FACTORY,
	CMD_COLD_START,
	CMD_RELAY,
	CMD_PRELAY,
    CMD_DYD,
    CMD_HFYD,
	CMD_GPSON,
	CMD_GPSOFF,
	CMD_PARAM,
	CMD_WRITEIMEI,
	CMD_IMEI,
	CMD_ICCID,
	CMD_IMSI,
	CMD_WHERE,
	CMD_STATUS,
	CMD_LBS_ON,
	CMD_LBS_UPLOAD_ONCE,
	CMD_LANGUAGE,
	CMD_HEARTBEAT_MIN,
	CMD_HEARTBEAT_SEC,
	CMD_HBEX,
	CMD_OVER_SPEED,
	CMD_VER,
	CMD_SENSOR_PARA,
	CMD_SHAKE_ALARM_SWITCH,
	CMD_DEFMODE,
	CMD_AUTO_DEFENCE_DELAY,
	CMD_SHAKE_ALARM_PARA,
	CMD_DEFENCE_ON,
	CMD_DEFENCE_OFF,
	CMD_BAT_ALM_SWITCH,
	CMD_POWER_OFF_ALM_PARA,
	CMD_MILEAGE,
	CMD_JT808_LOGIN,
	CMD_SLEEP_TIME,
    CMD_123_POSITION,
    CMD_LOG_LEVEL,
    CMD_SOS,
    CMD_CENTER,
    CMD_GPRS_PARA,
    CMD_STATIC_UPLOAD,
    CMD_TEST,
    CMD_EXIT,
    CMD_RECORD,
    CMD_RESULT,
    CMD_PMTK,
    CMD_HARD_REBOOT,
    CMD_BMS_BATTERY,
    
	CMD_CMD_MAX,
}CommandID;

typedef struct
{
    char cmd_name[CMD_NAME_MAX_LEN];
    CommandID cmd_id;
}CommandInfo;

static CommandInfo s_cmd_infos[CMD_CMD_MAX + 1] = 
{
	{"DEVICETYPE",CMD_DEVICE_TYPE},
   	{"APN",CMD_APN},
	{"APNINIT",CMD_APN_INIT},
	{"APNC",CMD_APNC},
	{"ASETAPN",CMD_AUTO_APN},
	{"GMNET",CMD_SERVER},
	{"PROTOCOL",CMD_PROTOCOL},
	{"TIMER",CMD_UPLOAD_TIM},
	{"GMT",CMD_GMT},
	{"RESET",CMD_SYSTEM_RESET},
	{"FACTORYALL",CMD_FACTORY_ALL},
	{"FACTORY",CMD_FACTORY},
	{"COLDSTART",CMD_COLD_START},
	{"RELAY",CMD_RELAY},
	{"PRELAY",CMD_PRELAY},
    {"DYD",CMD_DYD},
    {"HFYD",CMD_HFYD},
	{"GPSON",CMD_GPSON},
	{"GPSOFF",CMD_GPSOFF},
	{"PARAM",CMD_PARAM},
	{"GOOIMEI",CMD_WRITEIMEI},
	{"IMEI",CMD_IMEI},
	{"ICCID",CMD_ICCID},
	{"IMSI",CMD_IMSI},
	{"WHERE",CMD_WHERE},
	{"STATUS",CMD_STATUS},
	{"LBSON",CMD_LBS_ON},
	{"LBS",CMD_LBS_UPLOAD_ONCE},
	{"LANG",CMD_LANGUAGE},
	{"HBT",CMD_HEARTBEAT_MIN},
	{"HBTS",CMD_HEARTBEAT_SEC},
	{"HBEX",CMD_HBEX},
	{"SPEED",CMD_OVER_SPEED},
	{"VERSION",CMD_VER},
	{"GMVER",CMD_VER},
	{"SENSORSET",CMD_SENSOR_PARA},
	{"SENALM",CMD_SHAKE_ALARM_SWITCH},
	{"DEFMODE",CMD_DEFMODE},
	{"DEFENSE",CMD_AUTO_DEFENCE_DELAY},
	{"SENSOR",CMD_SHAKE_ALARM_PARA},
	{"000",CMD_DEFENCE_OFF},
	{"111",CMD_DEFENCE_ON},
	{"BATALM",CMD_BAT_ALM_SWITCH},
	{"POWERALM",CMD_POWER_OFF_ALM_PARA},
	{"MILEAGE",CMD_MILEAGE},
	{"LOGIN",CMD_JT808_LOGIN},
	{"SENDS",CMD_SLEEP_TIME},
    {"123",CMD_123_POSITION},
	{"LOGLEVEL",CMD_LOG_LEVEL},
    {"SOS",CMD_SOS},
    {"CENTER",CMD_CENTER},
    {"GPRSSET",CMD_GPRS_PARA},
    {"STATICREP",CMD_STATIC_UPLOAD},
    {"TEST",CMD_TEST},
    {"EXIT",CMD_EXIT},
    {"RECORD",CMD_RECORD},
    {"RESULT",CMD_RESULT},
	{"PMTK",CMD_PMTK},
	{"HARDREBOOT",CMD_HARD_REBOOT},
	{"BATTERY",CMD_BMS_BATTERY},
};

static CommandID get_cmd_id(const char* cmd_name);
static const char* set_success_rsp(CommandReceiveFromEnum from);
static const char* set_fail_rsp(CommandReceiveFromEnum from);
static const char* get_protocol_str(U16 lang,CommandReceiveFromEnum from,U8 protocol_type);
static const char* get_enable_str(U16 lang,CommandReceiveFromEnum from,bool is_on);
static char command_scan(const char* p_command, const char* p_format, ...);

static CommandID get_cmd_id(const char* cmd_name)
{
	U8 index = 0;
	for (index = 0; index < CMD_CMD_MAX + 1; ++index)
	{
		if (!GM_strcmp(cmd_name, s_cmd_infos[index].cmd_name))
		{
			return s_cmd_infos[index].cmd_id;
		}
	}
	return CMD_CMD_MAX;
}

static const char set_success_rsp_en[] = "Exec Success.";
static const char set_success_rsp_ch[] = "设置成功";
static const char set_fail_rsp_en[] = "Exec failure!";
static const char set_fail_rsp_ch[] = "指令错误";

static const char* set_success_rsp(CommandReceiveFromEnum from)
{
	U16 lang = 0;
	config_service_get(CFG_LANGUAGE, TYPE_SHORT, &lang, sizeof(lang));

	if (1 == lang && COMMAND_GPRS == from)
	{
		return set_success_rsp_ch;
	}
	else
	{
		return set_success_rsp_en;
	}
}

static const char* set_fail_rsp(CommandReceiveFromEnum from)
{
	U16 lang = 0;
	config_service_get(CFG_LANGUAGE, TYPE_SHORT, &lang, sizeof(lang));
	
	if (1 == lang && COMMAND_GPRS == from)
	{
		return set_fail_rsp_ch;
	}
	else
	{
		return set_fail_rsp_en;
	}
}

static const char* get_protocol_str(U16 lang,CommandReceiveFromEnum from,U8 protocol_type)
{
	static char protocol_str[20] = {0};
	if (1 == lang && COMMAND_GPRS == from)
	{	
		if (1 == protocol_type)
		{
			GM_strcpy(protocol_str, "谷米协议");
		}
		else if(2 == protocol_type)
		{
			GM_strcpy(protocol_str, "康凯斯协议");
		}
		else if(3 == protocol_type)
		{
			GM_strcpy(protocol_str, "部标808");
		}
		else
		{
			GM_strcpy(protocol_str, "未知协议");
		}		

	}
	else
	{
		if (1 == protocol_type)
		{
			GM_strcpy(protocol_str, "GOOME");
		}
		else if(2 == protocol_type)
		{
			GM_strcpy(protocol_str, "CONCOX");
		}
		else if(3 == protocol_type)
		{
			GM_strcpy(protocol_str, "JT808");
		}
		else
		{
			GM_strcpy(protocol_str, "unknown");
		}		
	}
	return protocol_str;
}

static const char* get_enable_str(U16 lang,CommandReceiveFromEnum from,bool is_on)
{
	static char is_on_str[20] = {0};

	if (1 == lang && COMMAND_GPRS == from)
	{	
		if(is_on)
		{
			GM_strcpy(is_on_str, "打开");
		}
		else
		{
			GM_strcpy(is_on_str, "关闭");
		}
	}
	else
	{
		if(is_on)
		{
			GM_strcpy(is_on_str, "enable");
		}
		else
		{
			GM_strcpy(is_on_str, "disable");
		}
	}
	return is_on_str;
}

//0:关闭,1:未定位,2:已定位
static const char* get_fixstate_str(U16 lang,CommandReceiveFromEnum from,GPSState state)
{
	static char is_on_str[20] = {0};

	if (1 == lang && COMMAND_GPRS == from)
	{	
		if(GM_GPS_OFF == state)
		{
			GM_strcpy(is_on_str, "关闭");
		}
		else if(state >= GM_GPS_FIX_3D)
		{
			GM_strcpy(is_on_str, "已定位");
		}
		else
		{
			GM_strcpy(is_on_str, "未定位");
		}
	}
	else
	{
		if(GM_GPS_OFF == state)
		{
			GM_strcpy(is_on_str, "close");
		}
		else if(state >= GM_GPS_FIX_3D)
		{
			GM_strcpy(is_on_str, "fixed");
		}
		else
		{
			GM_strcpy(is_on_str, "unfixed");
		}
	}
	return is_on_str;
}

//0:运动,1:静止
static const char* get_vehiclestate_str(U16 lang,CommandReceiveFromEnum from,VehicleState state)
{
	static char is_on_str[20] = {0};

	if (1 == lang && COMMAND_GPRS == from)
	{	
		if(VEHICLE_STATE_RUN == state)
		{
			GM_strcpy(is_on_str, "运动");
		}
		else
		{
			GM_strcpy(is_on_str, "静止");
		}
	}
	else
	{
		if(VEHICLE_STATE_RUN == state)
		{
			GM_strcpy(is_on_str, "run");
		}
		else
		{
			GM_strcpy(is_on_str, "static");
		}
	}
	return is_on_str;
}

GM_ERRCODE command_on_receive_data(CommandReceiveFromEnum from, char* p_cmd, u16 cmd_len, char* p_rsp, void * pmsg)
{	
	char cmd_name[CMD_NAME_MAX_LEN] = {0};
	char* p_cmd_content = NULL;
	char para_num = 0;
	CommandID cmd_id = CMD_CMD_MAX;
    gm_sms_new_msg_struct* p_sms = NULL;
	U16 lang = 0;

	if (NULL == p_cmd || 0 == cmd_len || cmd_len > (CMD_MAX_LEN - 1) || NULL == p_rsp)
	{
		return GM_PARAM_ERROR;
	}

	p_cmd_content = (char*)GM_MemoryAlloc(cmd_len + 1);
	GM_memcpy(p_cmd_content, p_cmd, cmd_len);
	p_cmd_content[cmd_len] = 0;
	LOG(INFO,"From %d received cmd:%s len:%d",from,p_cmd_content, cmd_len);

	util_remove_char((U8*)p_cmd_content,cmd_len,' ');
	util_remove_char((U8*)p_cmd_content,cmd_len,'\r');
	util_remove_char((U8*)p_cmd_content,cmd_len,'\n');
	cmd_len = GM_strlen(p_cmd_content);
	
	
	if (p_cmd_content[cmd_len -1] != '#')
	{
		GM_snprintf(p_rsp,CMD_MAX_LEN,"invalid cmd from %d,len=%d:%s",from,cmd_len,p_cmd_content);
        GM_MemoryFree(p_cmd_content);
        p_cmd_content = NULL;
		return GM_PARAM_ERROR;
	}
		
	config_service_get(CFG_LANGUAGE, TYPE_SHORT, &lang, sizeof(lang));

	command_scan((char*)p_cmd_content, "s", cmd_name);
	util_string_upper((U8*)cmd_name,GM_strlen(cmd_name));
	cmd_id = get_cmd_id(cmd_name);
	switch (cmd_id)
	{
		case CMD_DEVICE_TYPE:
		{
			char dev_type_str[16] = {0};
			ConfigDeviceTypeEnum dev_type_id = DEVICE_MAX;
			para_num = command_scan((char*)p_cmd_content, "s;s", cmd_name,&dev_type_str);
			if (para_num == 1)
			{
				config_service_get(CFG_DEVICETYPE, TYPE_SHORT, &dev_type_id, sizeof(U16));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "设备型号:%s", (char*)config_service_get_device_type(dev_type_id));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Device type:%s", (char*)config_service_get_device_type(dev_type_id));
				}
			}
			else if(para_num == 2)
			{
				//设置设备类型
				dev_type_id = config_service_get_device_type_id(dev_type_str);
				if (dev_type_id == DEVICE_NONE || dev_type_id >= DEVICE_MAX)
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					config_service_set_device((ConfigDeviceTypeEnum)dev_type_id);
	                config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_APN:
		{	
			char apn_name[CMD_NAME_MAX_LEN] = {0};
			char user_name[CMD_NAME_MAX_LEN] = {0};
			char pass_word[CMD_NAME_MAX_LEN] = {0};
			
            para_num = command_scan((char*)p_cmd_content, "s;sss", cmd_name,apn_name,user_name,pass_word);

			if (para_num == 1)
			{
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "APN名称:%s,用户名:%s,密码:%s", (char*)config_service_get_pointer(CFG_APN_NAME),(char*)config_service_get_pointer(CFG_APN_USER),(char*)config_service_get_pointer(CFG_APN_PWD));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "APN name:%s,user:%s,passpord:%s", (char*)config_service_get_pointer(CFG_APN_NAME),(char*)config_service_get_pointer(CFG_APN_USER),(char*)config_service_get_pointer(CFG_APN_PWD));
				}
			}
			else if(para_num == 2)
			{
				//设置APN
				if(0 == GM_strlen(apn_name))
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					config_service_set(CFG_APN_NAME, TYPE_STRING, apn_name, GM_strlen(apn_name));
					config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                    system_state_set_gpss_reboot_reason("cmd apn param2");
					gprs_destroy();
				}
			
			}
			else if(para_num == 3)
			{
				//设置APN
				config_service_set(CFG_APN_NAME, TYPE_STRING, apn_name, GM_strlen(apn_name));
				config_service_set(CFG_APN_USER, TYPE_STRING, user_name, GM_strlen(user_name));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                    system_state_set_gpss_reboot_reason("cmd apn param3");
					gprs_destroy();
				}
			}
			else if(para_num == 4)
			{
				//设置APN
				config_service_set(CFG_APN_NAME, TYPE_STRING, apn_name, GM_strlen(apn_name));
				config_service_set(CFG_APN_USER, TYPE_STRING, user_name, GM_strlen(user_name));
				config_service_set(CFG_APN_PWD, TYPE_STRING, pass_word, GM_strlen(pass_word));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                    system_state_set_gpss_reboot_reason("cmd apn param4");
					gprs_destroy();
				}	
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_APN_INIT:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
				//APN初始化
				config_service_set(CFG_APN_NAME, TYPE_STRING, APN_DEFAULT, GM_strlen((const char *)APN_DEFAULT));
    			config_service_set(CFG_APN_USER, TYPE_STRING, APN_USER_DEFAULT, GM_strlen((const char *)APN_USER_DEFAULT));
    			config_service_set(CFG_APN_PWD, TYPE_STRING, APN_USER_DEFAULT, GM_strlen((const char *)APN_USER_DEFAULT));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                    system_state_set_gpss_reboot_reason("cmd apn init");
					gprs_destroy();
				}	
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);	
			}
		}
		break;
		
		case CMD_APNC:
		{
			u16 mcc = 0;
    		u16 mnc = 0;
			para_num = command_scan((char*)p_cmd_content, "s;ii", &mcc,&mnc);
			if(para_num == 3)
			{
				//设置移动国家代码和移动网络代码
				goome_auto_apn_struct auto_apn;
				auto_apn.mcc = mcc;
				auto_apn.mnc = mnc;
			    GM_AutoApnCheck(&auto_apn);
			}
			
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_AUTO_APN:
		{
			bool is_on = 0;
			para_num = command_scan((char*)p_cmd_content, "s;w", cmd_name,&is_on);
			if (para_num == 1)
			{
				config_service_get(CFG_APN_CHECK, TYPE_BOOL, &is_on, sizeof(is_on));
				GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Auto APN:%s",get_enable_str(lang,from,is_on));
			}
			else if(para_num == 2)
			{

				config_service_set(CFG_APN_CHECK, TYPE_BOOL, &is_on, sizeof(is_on));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                    system_state_set_gpss_reboot_reason("cmd auto apn");
					gprs_destroy();
				}
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_SERVER:
		{
			char addr[GOOME_DNS_MAX_LENTH] = {0};
			U32 port = 0;
			bool is_udp = false;
			para_num = command_scan((char*)p_cmd_content, "s;siw", cmd_name,addr,&port,&is_udp);

			if (para_num == 1)
			{
				if (1 == lang && COMMAND_GPRS == from)
				{	
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "平台:%s", (char*)config_service_get_pointer(CFG_SERVERADDR));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Server:%s", (char*)config_service_get_pointer(CFG_SERVERADDR));
				}
			}
			else if(para_num == 3 || para_num == 4)
			{
				//设置
				bool is_lock_ip = true;
				char server_addr[64] = {0};
				config_service_get(CFG_SERVERLOCK, TYPE_BYTE, &is_lock_ip, sizeof(is_lock_ip));
				if (is_lock_ip)
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
					break;
				}
				GM_snprintf(server_addr, 31, "%s:%d", addr,port);
				LOG(INFO,"Set server addr:%s",server_addr);
				config_service_set(CFG_SERVERADDR, TYPE_STRING, server_addr, GM_strlen(server_addr));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
                gps_service_change_config();
				gps_power_on(true);
				g_sensor_reset_noshake_time();
				system_state_set_reported_gps_since_modify_ip(false);
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_PROTOCOL:
		{
			U8 protocol_type = 0;
			U8 protocol_ver = 0;
			para_num = command_scan((char*)p_cmd_content, "s;ii", cmd_name,&protocol_type,&protocol_ver);
			if (para_num == 1)
			{
				config_service_get(CFG_PROTOCOL, TYPE_BYTE, &protocol_type, sizeof(protocol_type));
				config_service_get(CFG_PROTOCOL_VER, TYPE_BYTE, &protocol_ver, sizeof(protocol_ver));
				if (1 == lang && COMMAND_GPRS == from)
				{	
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "协议:%s,协议版本:%d", get_protocol_str(lang,from,protocol_type),protocol_ver);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Protocol:%s,protocol version:%d", get_protocol_str(lang,from,protocol_type),protocol_ver);
				}
			}
			else if(para_num == 2)
			{	
				if(protocol_type >= PROTOCOL_MAX)
				{		
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					config_service_set(CFG_PROTOCOL, TYPE_BYTE, &protocol_type, sizeof(protocol_type));
					config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 3)
			{
				if(protocol_type >= PROTOCOL_MAX || protocol_ver >= PROTOCOL_APP_MAX)
				{		
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					config_service_set(CFG_PROTOCOL, TYPE_BYTE, &protocol_type, sizeof(protocol_type));
					config_service_set(CFG_PROTOCOL_VER, TYPE_BYTE, &protocol_ver, sizeof(protocol_ver));
					config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_UPLOAD_TIM:
		{
			U16 upload_time = 0;
			para_num = command_scan((char*)p_cmd_content, "s;i", cmd_name,&upload_time);
			if (para_num == 1)
			{
				config_service_get(CFG_UPLOADTIME, TYPE_SHORT, &upload_time, sizeof(upload_time));
				if (1 == lang && COMMAND_GPRS == from)
				{	
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "上传间隔:%d秒", upload_time);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Upload interval:%d seconds", upload_time);
				}
			}
			else if(para_num == 2)
			{
				//设置
				if(upload_time < CONFIG_UPLOAD_TIME_MIN || upload_time > CONFIG_UPLOAD_TIME_MAX)
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					config_service_set(CFG_UPLOADTIME, TYPE_SHORT, &upload_time, sizeof(U16));
					config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_GMT:
		{
			char east_or_west = 'E';
			S8 time_zone = 0;
			para_num = command_scan((char*)p_cmd_content, "s;ci", cmd_name,&east_or_west,&time_zone);

			if (para_num == 1)
			{
				time_zone = config_service_get_zone();
				if (1 == lang && COMMAND_GPRS == from)
				{	
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "时区:%d", time_zone);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Timezone:%d", time_zone);
				}
			}
			else if(para_num == 3 || para_num == 4)
			{
				east_or_west = util_to_upper(east_or_west);
				//设置
				if (east_or_west == 'E')
				{
					
				}
				else if (east_or_west == 'W')
				{
					time_zone *= -1;
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
					break;
				}
				config_service_set(CFG_TIME_ZONE, TYPE_BYTE, &time_zone, sizeof(time_zone));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_SYSTEM_RESET:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
				//延时重启
				hard_ware_reboot(GM_REBOOT_CMD,5);
				if (1 == lang && COMMAND_GPRS == from)
				{	
					GM_memcpy(p_rsp, "系统正在重启", CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, "System is restaring", CMD_MAX_LEN);
				}
			}
			else
			{
				//错误
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_FACTORY_ALL:
		case CMD_FACTORY:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
				//恢复出厂设置
				config_service_restore_factory_config(cmd_id == CMD_FACTORY_ALL);
				agps_service_delele_file();
                gps_service_his_file_delete();
				system_state_reset();
				if (cmd_id == CMD_FACTORY_ALL)
				{
					auto_test_reset();
				}
				
				//延时重启
				hard_ware_reboot(GM_REBOOT_CMD,5);
				if (1 == lang && COMMAND_GPRS == from)
				{	
					GM_memcpy(p_rsp, "系统正在恢复出厂设置并重启", CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, "System is recovering and restarting.", CMD_MAX_LEN);
				}
				
			}
			else
			{
				//错误
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_COLD_START:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
				agps_service_delele_file();
                gps_service_his_file_delete();
				system_state_set_cold_boot(true);
						
				//延时重启
				hard_ware_reboot(GM_REBOOT_CMD,5);
				if (1 == lang && COMMAND_GPRS == from)
				{	
					GM_memcpy(p_rsp, "设置下次启动为冷启动，重启中。。。", CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, "Set full cold start and restarting.", CMD_MAX_LEN);
				}
				
			}
			else
			{
				//错误
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_RELAY:
		case CMD_PRELAY:
		{
			U16 lang = 0;
			bool is_on = true;
			config_service_get(CFG_LANGUAGE, TYPE_SHORT, &lang, sizeof(lang));
			para_num = command_scan((char*)p_cmd_content, "s;w", cmd_name,&is_on);
			if (para_num == 1)
			{
				char is_on_str[20] = {0};
				
 				config_service_get(CFG_IS_BMS_MOS_CUT_OFF, TYPE_BOOL, &is_on, sizeof(is_on));
				if (1 == lang && COMMAND_GPRS == from)
				{	
					if(is_on)
					{
						GM_strcpy(is_on_str, "已断开MOS放电");
					}
					else
					{
						GM_strcpy(is_on_str, "已打开MOS放电");
					}
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "MOS状态:%s", is_on_str);
				}
				else
				{
					if(is_on)
					{
						GM_strcpy(is_on_str, "OFF");
					}
					else
					{
						GM_strcpy(is_on_str, "ON");
					}
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, " MOS:%s", is_on_str);
				}
				
			}
			else if (para_num == 2)
			{
				if (1 == lang && COMMAND_GPRS == from)
				{
					if (is_on)
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "断开MOS放电成功");
					}
					else
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "打开MOS放电成功");
					}
				}
				else
				{
					if (is_on)
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "Succeed to cut off battery mos current output.");
					}
					else
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "Succeed to restore battery mos current output.");
					}
				}
				
    			config_service_set(CFG_IS_BMS_MOS_CUT_OFF, TYPE_BOOL, &is_on, sizeof(is_on));
				bms_battery_mos_output_ctrl(is_on);
				config_service_save_to_local();
			}
			/*
			if (para_num == 1)
			{
				char is_on_str[10] = {0};
 				is_on = system_state_get_device_relay_state();
				if (1 == lang && COMMAND_GPRS == from)
				{	
					if(is_on)
					{
						GM_strcpy(is_on_str, "已断开");
					}
					else
					{
						GM_strcpy(is_on_str, "未断开");
					}
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "油电状态:%s", is_on_str);
				}
				else
				{
					if(is_on)
					{
						GM_strcpy(is_on_str, "ON");
					}
					else
					{
						GM_strcpy(is_on_str, "OFF");
					}
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, " Relay:%s", is_on_str);
				}
				
			}
			else if(para_num == 2)
			{
 				GM_ERRCODE ret = GM_SUCCESS;
				if (is_on)
				{
					ret = relay_on(cmd_id == CMD_RELAY);
					if (GM_SUCCESS == ret)
					{
						if (1 == lang && COMMAND_GPRS == from)
						{
							GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "断油电成功");
						}
						else
						{
							GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "Succeed to cut off the oil and circut.");
						}
					}
					else if(GM_WILL_DELAY_EXEC == ret)
					{
						if (1 == lang && COMMAND_GPRS == from)
						{
							GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "车辆正在高速行驶或者速度未知,断油电可能会引发事故,系统会在速度较低时自动执行断油电");
						}
						else
						{
							GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "System will cut off the oil and circut when speed is safe.");
						} 
					}
					else
					{
						char fail_english[CMD_MAX_LEN] = "Exec failure!";
						char fail_chinese[CMD_MAX_LEN] = "断油电失败(设备故障)";
						if (0 == lang || COMMAND_SMS == from)
						{
							GM_memcpy(p_rsp, fail_english, GM_strlen(fail_english));
						}
						else
						{
							GM_memcpy(p_rsp, fail_chinese, GM_strlen(fail_chinese));
						}
					}
				}
				else
				{
					ret = relay_off();
					if (GM_SUCCESS == ret)
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, set_success_rsp(from),is_on);
					}
					else
					{ 
						char fail_chinese[CMD_MAX_LEN] = "恢复油电失败(设备故障)";
						if (1 == lang || COMMAND_GPRS == from)
						{
							GM_memcpy(p_rsp, fail_chinese, GM_strlen(fail_chinese));
						}
						else
						{
							GM_memcpy(p_rsp, set_fail_rsp_en, GM_strlen(set_fail_rsp_en));
						}
					}
				}	
			}*/
			else
			{ 
				char wrong_command_rsp_chinese[CMD_MAX_LEN] = "指令格式错误,请修改后重试";
				if (1 == lang || COMMAND_GPRS == from)
				{
					GM_memcpy(p_rsp, wrong_command_rsp_chinese, GM_strlen(wrong_command_rsp_chinese));
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp_en, GM_strlen(set_fail_rsp_en));
				}
			}
		}
		break;

		case CMD_HFYD:
		{
			U16 lang = 0;
			bool is_on = true;
			config_service_get(CFG_LANGUAGE, TYPE_SHORT, &lang, sizeof(lang));
			para_num = command_scan((char*)p_cmd_content, "s;w", cmd_name,&is_on);
			if (para_num == 1)
			{
				char is_on_str[10] = {0};
 				is_on = system_state_get_device_relay_state();
				if (1 == lang && COMMAND_GPRS == from)
				{	
					if(is_on)
					{
						GM_strcpy(is_on_str, "已断开");
					}
					else
					{
						GM_strcpy(is_on_str, "未断开");
					}
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "油电状态:%s", is_on_str);
				}
				else
				{
					if(is_on)
					{
						GM_strcpy(is_on_str, "ON");
					}
					else
					{
						GM_strcpy(is_on_str, "OFF");
					}
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, " Relay:%s", is_on_str);
				}
				
			}
			else if(para_num == 2)
			{
 				GM_ERRCODE ret = GM_SUCCESS;
				if (! is_on)
				{
					ret = relay_off();
				}
				if (GM_SUCCESS == ret)
				{
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, set_success_rsp(from),is_on);
				}
				else
				{ 
					char fail_chinese[CMD_MAX_LEN] = "恢复油电失败(设备故障)";
					if (1 == lang || COMMAND_GPRS == from)
					{
						GM_memcpy(p_rsp, fail_chinese, GM_strlen(fail_chinese));
					}
					else
					{
						GM_memcpy(p_rsp, set_fail_rsp_en, GM_strlen(set_fail_rsp_en));
					}
				}
			}
			else
			{ 
				char wrong_command_rsp_chinese[CMD_MAX_LEN] = "指令格式错误,请修改后重试";
				if (1 == lang || COMMAND_GPRS == from)
				{
					GM_memcpy(p_rsp, wrong_command_rsp_chinese, GM_strlen(wrong_command_rsp_chinese));
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp_en, GM_strlen(set_fail_rsp_en));
				}
			}
		}
		break;

		case CMD_DYD:
		{
			U16 lang = 0;
			bool is_on = true;
			config_service_get(CFG_LANGUAGE, TYPE_SHORT, &lang, sizeof(lang));
			para_num = command_scan((char*)p_cmd_content, "s;w", cmd_name,&is_on);
			if (para_num == 1)
			{
				char is_on_str[10] = {0};
 				is_on = system_state_get_device_relay_state();
				if (1 == lang && COMMAND_GPRS == from)
				{	
					if(is_on)
					{
						GM_strcpy(is_on_str, "已断开");
					}
					else
					{
						GM_strcpy(is_on_str, "未断开");
					}
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "油电状态:%s", is_on_str);
				}
				else
				{
					if(is_on)
					{
						GM_strcpy(is_on_str, "ON");
					}
					else
					{
						GM_strcpy(is_on_str, "OFF");
					}
					GM_snprintf((char*)p_rsp,CMD_MAX_LEN, " Relay:%s", is_on_str);
				}
				
			}
			else if(para_num == 2)
			{
 				GM_ERRCODE ret = GM_SUCCESS;
				if (is_on)
				{
					ret = relay_on(cmd_id == CMD_RELAY);
				}
				if (GM_SUCCESS == ret)
				{
					if (1 == lang && COMMAND_GPRS == from)
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "断油电成功");
					}
					else
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "Succeed to cut off the oil and circut.");
					}
				}
				else if(GM_WILL_DELAY_EXEC == ret)
				{
					if (1 == lang && COMMAND_GPRS == from)
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "车辆正在高速行驶或者速度未知,断油电可能会引发事故,系统会在速度较低时自动执行断油电");
					}
					else
					{
						GM_snprintf((char*)p_rsp,CMD_MAX_LEN, "System will cut off the oil and circut when speed is safe.");
					} 
				}
				else
				{
					char fail_english[CMD_MAX_LEN] = "Exec failure!";
					char fail_chinese[CMD_MAX_LEN] = "断油电失败(设备故障)";
					if (0 == lang || COMMAND_SMS == from)
					{
						GM_memcpy(p_rsp, fail_english, GM_strlen(fail_english));
					}
					else
					{
						GM_memcpy(p_rsp, fail_chinese, GM_strlen(fail_chinese));
					}
				}
			}
			else
			{ 
				char wrong_command_rsp_chinese[CMD_MAX_LEN] = "指令格式错误,请修改后重试";
				if (1 == lang || COMMAND_GPRS == from)
				{
					GM_memcpy(p_rsp, wrong_command_rsp_chinese, GM_strlen(wrong_command_rsp_chinese));
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp_en, GM_strlen(set_fail_rsp_en));
				}
			}
		}
		break;
		
		case CMD_GPSON:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{	
				gps_power_on(true);
				g_sensor_reset_noshake_time();
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}		
		}
		break;
		
		case CMD_GPSOFF:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{	
				bool gps_close = true;
				config_service_set(CFG_GPS_CLOSE,TYPE_BOOL,&gps_close,sizeof(gps_close));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}		
		}
		break;
	
		case CMD_PARAM:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{	
				char server[32] = {0};
				ConfigProtocolEnum protocol = config_service_get_app_protocol();
				U8 protocol_ver = 0;
				U16 upload_time = 0;
				U16 heartbeat_time = 0;
				
				config_service_get(CFG_SERVERADDR, TYPE_STRING, server, sizeof(server));
				config_service_get(CFG_PROTOCOL_VER, TYPE_BYTE, &protocol_ver, sizeof(protocol_ver));
				config_service_get(CFG_UPLOADTIME, TYPE_SHORT, &upload_time, sizeof(upload_time));
				config_service_get(CFG_HEART_INTERVAL, TYPE_SHORT, &heartbeat_time, sizeof(heartbeat_time));
							
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "服务器:%s,协议:%s,协议版本:%d,上传间隔:%d秒,心跳间隔:%d分钟,APN:%s", server,get_protocol_str(lang,from,protocol),protocol_ver,upload_time,heartbeat_time/SECONDS_PER_MIN,(char*)config_service_get_pointer(CFG_APN_NAME));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Server:%s,protocol:%s,protocol version:%d,upload interval:%d seconds,heartbeat interval:%d minutes,APN:%s", server,get_protocol_str(lang,from,protocol),protocol_ver,upload_time,heartbeat_time/SECONDS_PER_MIN,(char*)config_service_get_pointer(CFG_APN_NAME));
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_WRITEIMEI:
		{	
			char imei[32] = {0};
			para_num = command_scan((char*)p_cmd_content, "s;s", cmd_name,imei);
			if(para_num == 2)
			{
				if (GM_SUCCESS == gsm_set_imei((U8*)imei))
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_IMEI:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{	
				char imei[32] = {0};
				gsm_get_imei((U8*)imei);
				GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "IMEI:%s", imei);
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_ICCID:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
				char iccid[32] = {0};
				gsm_get_iccid((U8*)iccid);
				GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "ICCID:%s", iccid);
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_IMSI:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
				char imsi[32] = {0};
				gsm_get_imsi((U8*)imsi);
				GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "IMSI:%s", imsi);
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_WHERE:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
				GPSData gps_data;
				bool ret = true;
				ret = gps_get_last_data(&gps_data);
				if (true == ret)
				{
					if (1 == lang && COMMAND_GPRS == from)
					{
						GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "纬度:%f,经度:%f,速度:%d公里/小时,方向:%d度", gps_data.lat,gps_data.lng,(U16)gps_data.speed,(U16)gps_data.course);
					}
					else
					{
						GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Latitude:%f,longitude:%f,speed:%d km/h,course:%d", gps_data.lat,gps_data.lng,(U16)gps_data.speed,(U16)gps_data.course);
					}
				}
				else
				{
					U16 lang = 0;
					char fail_english[CMD_MAX_LEN] = "GPS Unfixed,please retry later";
					char fail_chinese[CMD_MAX_LEN] = "GPS未定位,请稍后重试";
					config_service_get(CFG_LANGUAGE, TYPE_SHORT, &lang, sizeof(lang));	
					if (1 == lang && COMMAND_GPRS == from)
					{
						GM_memcpy(p_rsp, fail_chinese, GM_strlen(fail_chinese));
					}
					else
					{
						GM_memcpy(p_rsp, fail_english, GM_strlen(fail_english));
					}
				}
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_STATUS:
		{
			
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
				float power_voltage = 0;
				float battery_voltage = 0;
				bool is_defence_by_hand = false;
				bool acc_state = false;
				
				hard_ware_get_power_voltage(&power_voltage);
				hard_ware_get_internal_battery_voltage(&battery_voltage);
				hard_ware_get_acc_level(&acc_state);
				config_service_get(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &is_defence_by_hand, sizeof(is_defence_by_hand));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "GSM信号强度:%d,GPRS状态:%d(0:掉线,1:上线),GPS:%s,电源电压:%.1fV,电池电压%.1fV,ACC:%d,运动传感器:%s,油电状态:%d(0:未断开,1:已断开),设防模式:%d(0:自动,1:手动),设防状态:%d(0:撤防,1:设防)",
						gsm_get_csq(),
						gprs_is_ok(),
						get_fixstate_str(lang, from, gps_get_state()),
						power_voltage,
						battery_voltage,
						acc_state,
						get_vehiclestate_str(lang, from, system_state_get_vehicle_state()),
						system_state_get_device_relay_state(),
						is_defence_by_hand,
						system_state_get_defence());
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "GSM:%d,GPRS:%d,GPS:%s,voltage:%.1fV,battery:%.1fV,ACC:%d,g_sensor:%s,relay:%d,defence mode:%d,defence state:%d",
						gsm_get_csq(),
						gprs_is_ok(),
						get_fixstate_str(lang, from, gps_get_state()),
						power_voltage,
						battery_voltage,
						acc_state,
						get_vehiclestate_str(lang, from, system_state_get_vehicle_state()),
						system_state_get_device_relay_state(),
						is_defence_by_hand,
						system_state_get_defence());
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
			
		case CMD_LBS_ON:
		{
			bool is_on = false;
			u16 lbs_interval = 0;
			u16 wait_gps_time = 0;
			para_num = command_scan((char*)p_cmd_content, "s;wii", cmd_name,&is_on,&lbs_interval,&wait_gps_time);
			if (para_num == 1)
			{
				config_service_get(CFG_IS_LBS_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
				config_service_get(CFG_LBS_INTERVAL, TYPE_SHORT, &lbs_interval, sizeof(lbs_interval));
				config_service_get(CFG_WAIT_GPS_TIME, TYPE_SHORT, &wait_gps_time, sizeof(wait_gps_time));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "上传基站:%s,基站上传间隔:%d秒,等待GPS定位%d秒",get_enable_str(lang,from,is_on),lbs_interval,wait_gps_time);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "LBS:%s,upload interval:%d seconds,wait for GPS %d seconds",get_enable_str(lang,from,is_on),lbs_interval,wait_gps_time);
				}
			}
			else if(para_num == 2)
			{
				config_service_set(CFG_IS_LBS_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 3)
			{
				config_service_set(CFG_IS_LBS_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
				config_service_set(CFG_LBS_INTERVAL, TYPE_SHORT, &lbs_interval, sizeof(lbs_interval));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 4)
			{
				config_service_set(CFG_IS_LBS_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
				config_service_set(CFG_LBS_INTERVAL, TYPE_SHORT, &lbs_interval, sizeof(lbs_interval));
				config_service_set(CFG_WAIT_GPS_TIME, TYPE_SHORT, &wait_gps_time, sizeof(wait_gps_time));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_LBS_UPLOAD_ONCE:
		{
			gps_service_push_lbs();
			GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
		}
		break;
		
		case CMD_LANGUAGE:
		{
			U16 language = 0;
			para_num = command_scan((char*)p_cmd_content, "s;i", cmd_name,&language);
			if (para_num == 1)
			{
				config_service_get(CFG_LANGUAGE, TYPE_SHORT, &language, sizeof(language));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "语言:%d(0:英语,1:中文)",language);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Language:%d(0:English,1:Chinese)",language);
				}
			}
			else if(para_num == 2)
			{
				if (language <= 1)
				{
					config_service_set(CFG_LANGUAGE, TYPE_SHORT, &language, sizeof(language));
					if(GM_SUCCESS != config_service_save_to_local())
					{	
						GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
					}
					else
					{
						GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
					}
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_HEARTBEAT_MIN:
		case CMD_HEARTBEAT_SEC:
		{	
			U16 heart_interval = 0;
			para_num = command_scan((char*)p_cmd_content, "s;i", cmd_name,&heart_interval);
			if (para_num == 1)
			{
				config_service_get(CFG_HEART_INTERVAL, TYPE_SHORT, &heart_interval, sizeof(heart_interval));
				if (CMD_HEARTBEAT_MIN == cmd_id)
				{
					heart_interval /= SECONDS_PER_MIN;
					if (1 == lang && COMMAND_GPRS == from)
					{
						GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "心跳间隔:%d分钟",heart_interval);
					}
					else
					{
						GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Heart beat interval:%d minutes",heart_interval);
					}
				}
				else
				{
					if (1 == lang && COMMAND_GPRS == from)
					{
						GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "心跳间隔:%d秒",heart_interval);
					}
					else
					{
						GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Heart beat interval:%d seconds",heart_interval);
					}
				}
			}
			else if(para_num == 2)
			{
				if (CMD_HEARTBEAT_MIN == cmd_id)
				{
					heart_interval *= SECONDS_PER_MIN;
				}
				//限制范围在1秒到10分钟
				if(heart_interval < GOOME_HEARTBEAT_MIN || heart_interval > GOOME_HEARTBEAT_MAX)
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					config_service_set(CFG_HEART_INTERVAL, TYPE_SHORT, &heart_interval, sizeof(heart_interval));
					config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);			
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_HBEX:
		{
			U8 heart_protocol = 0;
			para_num = command_scan((char*)p_cmd_content, "s;i", cmd_name,&heart_protocol);
			if (para_num == 1)
			{
				config_service_get(CFG_HBPROTOCOL, TYPE_BYTE, &heart_protocol, sizeof(heart_protocol));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "使用扩展心跳:%d(1:普通心跳,2:扩展心跳)",heart_protocol);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Extend protocol of heart beat:%d",heart_protocol);
				}
			}
			else if(para_num == 2)
			{
				if (heart_protocol <= 1)
				{
					config_service_set(CFG_HBPROTOCOL, TYPE_BYTE, &heart_protocol, sizeof(heart_protocol));
					if(GM_SUCCESS != config_service_save_to_local())
					{	
						GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
					}
					else
					{
						GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
					}
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_OVER_SPEED:
		{
			bool is_on = true;
			U8 check_time = 0;
			U8 speed_threshold = 0;
			para_num = command_scan((char*)p_cmd_content, "s;wii", cmd_name,&is_on,&check_time,&speed_threshold);
			if (para_num == 1)
			{
				config_service_get(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
				config_service_get(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &check_time, sizeof(check_time));
				config_service_get(CFG_SPEEDTHR, TYPE_BYTE, &speed_threshold, sizeof(speed_threshold));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "超速报警开关:%s,检测时间:%d秒,超速阈值:%d公里/小时",get_enable_str(lang, from, is_on),check_time,speed_threshold);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Speed alarm:%s,check time:%d seconds,speed threshold:%d km/h",get_enable_str(lang, from, is_on),check_time,speed_threshold);
				}
			}
			else if(para_num == 2)
			{
				config_service_set(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
				config_service_save_to_local();
				GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
			}
			else if(para_num == 3)
			{
				if (check_time >=3 && check_time <= 100)
				{
					config_service_set(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
					config_service_set(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &check_time, sizeof(check_time));
					config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 4)
			{
				if (check_time >=3 && check_time <= 100 && speed_threshold >=CONFIG_SPEEDTHR_MIN && speed_threshold <= CONFIG_SPEEDTHR_MAX)
				{
					config_service_set(CFG_SPEED_ALARM_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
					config_service_set(CFG_SPEED_CHECK_TIME, TYPE_BYTE, &check_time, sizeof(check_time));
					config_service_set(CFG_SPEEDTHR, TYPE_BYTE, &speed_threshold, sizeof(speed_threshold));
					config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_VER:
		{
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{	
				char kernel_version[80];
				ConfigDeviceTypeEnum dev_type_id = DEVICE_MAX;
				
			    GM_memset(kernel_version, 0, sizeof(kernel_version));
			    GM_ReleaseVerno((U8*)kernel_version);
				GM_sprintf(kernel_version + GM_strlen(kernel_version), "(%s)",GM_BuildDateTime());
				config_service_get(CFG_DEVICETYPE, TYPE_SHORT, &dev_type_id, sizeof(U16));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "版本:%s(%s),内核:%s,校验码:%4X,设备型号:%s",VERSION_NUMBER,SW_APP_BUILD_DATE_TIME,kernel_version,system_state_get_bin_checksum(),(char*)config_service_get_device_type(dev_type_id));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Version:%s(%s),kernel:%s,check sum:%4X,device type:%s",VERSION_NUMBER,SW_APP_BUILD_DATE_TIME,kernel_version,system_state_get_bin_checksum(),(char*)config_service_get_device_type(dev_type_id));
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		//SENSORSET,10,2,2,1#（唤醒时间，唤醒检测次数，震动报警检测次数，震动报警检测时间）
		case CMD_SENSOR_PARA:
		{
			U8 awake_check_time = 0;
			U8 awake_check_count = 0;
			U8 shake_check_time = 0;
			U8 shake_check_count = 0;
			para_num = command_scan((char*)p_cmd_content, "s;iiii", cmd_name,&awake_check_time,&awake_check_count,&shake_check_count,&shake_check_time);
			if (para_num == 1)
			{
				config_service_get(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &awake_check_time, sizeof(awake_check_time));
	            config_service_get(CFG_AWAKE_COUNT, TYPE_BYTE, &awake_check_count, sizeof(awake_check_count));
				config_service_get(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &shake_check_time, sizeof(shake_check_time));
				config_service_get(CFG_SHAKE_COUNT, TYPE_BYTE, &shake_check_count, sizeof(shake_check_count));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "唤醒检测时间:%d秒,唤醒检测次数:%d,震动报警检测时间:%d秒,震动报警检测次数:%d",awake_check_time,awake_check_count,shake_check_time,shake_check_count);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Awake check time:%d seconds,awake check count:%d,shake alarm check time:%d seconds,shake alarm check count:%d",awake_check_time,awake_check_count,shake_check_time,shake_check_count);
				}
			}
			else if(para_num == 2)
			{
				config_service_set(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &awake_check_time, sizeof(awake_check_time));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 3)
			{
				config_service_set(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &awake_check_time, sizeof(awake_check_time));
	            config_service_set(CFG_AWAKE_COUNT, TYPE_BYTE, &awake_check_count, sizeof(awake_check_count));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 4)
			{
				config_service_set(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &awake_check_time, sizeof(awake_check_time));
	            config_service_set(CFG_AWAKE_COUNT, TYPE_BYTE, &awake_check_count, sizeof(awake_check_count));
				config_service_set(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &shake_check_time, sizeof(shake_check_time));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 5)
			{
				config_service_set(CFG_AWAKE_CHECK_TIME, TYPE_BYTE, &awake_check_time, sizeof(awake_check_time));
	            config_service_set(CFG_AWAKE_COUNT, TYPE_BYTE, &awake_check_count, sizeof(awake_check_count));
				config_service_set(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &shake_check_time, sizeof(shake_check_time));
				config_service_set(CFG_SHAKE_COUNT, TYPE_BYTE, &shake_check_count, sizeof(shake_check_count));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_SHAKE_ALARM_SWITCH:
		{
			bool is_on = false;
			para_num = command_scan((char*)p_cmd_content, "s;w", cmd_name,&is_on);
			if (para_num == 1)
			{
				config_service_get(CFG_IS_SHAKE_ALARM_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "震动报警开关:%s",get_enable_str(lang, from, is_on));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Shake alarm:%s",get_enable_str(lang, from, is_on));
				}
			}
			else if(para_num == 2)
			{

				config_service_set(CFG_IS_SHAKE_ALARM_ENABLE, TYPE_BOOL, &is_on, sizeof(is_on));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_DEFMODE:
		{
			bool is_manual_defence = false;
			para_num = command_scan((char*)p_cmd_content, "s;w", cmd_name,&is_manual_defence);
			if (para_num == 1)
			{
				config_service_get(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &is_manual_defence, sizeof(is_manual_defence));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "设防模式:%d(0:自动设防,1:手动设防)",is_manual_defence);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Defence mode:%d(0:auto defence,1:manual defence)",is_manual_defence);
				}
			}
			else if(para_num == 2)
			{

				config_service_set(CFG_IS_MANUAL_DEFENCE, TYPE_BOOL, &is_manual_defence, sizeof(is_manual_defence));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_AUTO_DEFENCE_DELAY:
		{
			U16 auto_defence_delay = 0;
			para_num = command_scan((char*)p_cmd_content, "s;i", cmd_name,&auto_defence_delay);
			if (para_num == 1)
			{
				config_service_get(CFG_AUTO_DEFENCE_DELAY, TYPE_SHORT, &auto_defence_delay, sizeof(auto_defence_delay));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "自动设防延时:%秒",auto_defence_delay);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Auto defence delay:%d seconds",auto_defence_delay);
				}
			}
			else if(para_num == 2)
			{
				auto_defence_delay *= SECONDS_PER_MIN;
				config_service_set(CFG_AUTO_DEFENCE_DELAY, TYPE_SHORT, &auto_defence_delay, sizeof(auto_defence_delay));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		//SENSOR,10,180,30#（震动报警检测时间10S,自动设防模式下震动报警延时180秒,震动报警间隔1800秒(30分钟)）
		case CMD_SHAKE_ALARM_PARA:
		{
			//CMD_SENSOR_PARA也设置了这个参数
			U8 shake_check_time = 0;
			U16 shake_alarm_delay = 0;
			U16 shake_alarm_interval = 0;
			para_num = command_scan((char*)p_cmd_content, "s;iii", cmd_name,&shake_check_time,&shake_alarm_delay,&shake_alarm_interval);
			if (para_num == 1)
			{
				config_service_get(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &shake_check_time, sizeof(shake_check_time));
				config_service_get(CFG_SHAKE_ALARM_DELAY, TYPE_SHORT, &shake_alarm_delay, sizeof(shake_alarm_delay));
				config_service_get(CFG_SHAKE_ALARM_INTERVAL, TYPE_SHORT, &shake_alarm_interval, sizeof(shake_alarm_interval));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "震动报警检测时间%d秒,自动设防模式下震动报警延时%d秒,震动报警间隔%d分钟",shake_check_time,shake_alarm_delay,shake_alarm_interval/SECONDS_PER_MIN);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "shake alarm check time %d seconds,shake alarm delay %d seconds,report interval %d minutes",shake_check_time,shake_alarm_delay,shake_alarm_interval/SECONDS_PER_MIN);
				}
			}
			else if(para_num == 2)
			{
				config_service_set(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &shake_check_time, sizeof(shake_check_time));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 3)
			{
				config_service_set(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &shake_check_time, sizeof(shake_check_time));
				config_service_set(CFG_SHAKE_ALARM_DELAY, TYPE_SHORT, &shake_alarm_delay, sizeof(shake_alarm_delay));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 4)
			{
				shake_alarm_interval *= SECONDS_PER_MIN;
				config_service_set(CFG_SHAKE_CHECK_TIME, TYPE_BYTE, &shake_check_time, sizeof(shake_check_time));
				config_service_set(CFG_SHAKE_ALARM_DELAY, TYPE_SHORT, &shake_alarm_delay, sizeof(shake_alarm_delay));
				config_service_set(CFG_SHAKE_ALARM_INTERVAL, TYPE_SHORT, &shake_alarm_interval, sizeof(shake_alarm_interval));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_DEFENCE_ON:
		{
			bool is_defence_by_hand = false;
			config_service_get(CFG_IS_MANUAL_DEFENCE,TYPE_BOOL, &is_defence_by_hand, sizeof(is_defence_by_hand));
			if (is_defence_by_hand)
			{
				system_state_set_defence(true);
				GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
			}
			else
			{
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "当前为自动设防模式，不能手动设防");
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Could not set defence on manually because of auto defence mode");
				}
			}	
		}
		break;
		
		case CMD_DEFENCE_OFF:
		{
			bool is_defence_by_hand = false;
			config_service_get(CFG_IS_MANUAL_DEFENCE,TYPE_BOOL, &is_defence_by_hand, sizeof(is_defence_by_hand));
			if (is_defence_by_hand)
			{
				system_state_set_defence(false);
				GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
			}
			else
			{
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "当前为自动设防模式，不能手动撤防");
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Could not set defence off manually because of auto defence mode.");
				}
			}	
		}
		break;

		case CMD_BAT_ALM_SWITCH:
		{
			u8 is_on = 0;
			para_num = command_scan((char*)p_cmd_content, "s;w", cmd_name,&is_on);
			if (para_num == 1)
			{
				config_service_get(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &is_on, sizeof(is_on));
                is_on = is_on ? 0: 1;  //  0,打开,1:关闭
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "内置电池电压过低报警开关:%s",get_enable_str(lang, from, is_on));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Low voltage alarm of battery:%s",get_enable_str(lang, from, is_on));
				}
			}
			else if(para_num == 2)
			{
                is_on = is_on ? 0: 1;  //  0,打开,1:关闭
				config_service_set(CFG_LOWBATTALM_DISABLE, TYPE_BYTE, &is_on, sizeof(is_on));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		//POWERALM,ON,10,100#（断电检测10秒,接电后100S后才能产生断电报警）   POWERALM,OFF#关闭断电报警
		case CMD_POWER_OFF_ALM_PARA:
		{
			u8 is_on = 0;   //0,打开,1:关闭
			U16 power_alarm_check_time = 0;
			U16 power_off_alarm_delay = 0;
			para_num = command_scan((char*)p_cmd_content, "s;wii", cmd_name,&is_on,&power_alarm_check_time,&power_off_alarm_delay);
			if (para_num == 1)
			{
			
				config_service_get(CFG_CUTOFFALM_DISABLE, TYPE_BYTE, &is_on, sizeof(is_on));
                is_on = is_on? 0: 1; //0,打开,1:关闭
				config_service_get(CFG_POWER_CHECK_TIME, TYPE_SHORT, &power_alarm_check_time, sizeof(power_alarm_check_time));
				config_service_get(CFG_POWER_CHARGE_MIN_TIME, TYPE_SHORT, &power_off_alarm_delay, sizeof(power_off_alarm_delay));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "断电报警:%s,断电检测%s秒,接电后%d秒后才会产生断电报警",get_enable_str(lang, from, is_on),power_alarm_check_time,power_off_alarm_delay);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Power off alarm:%s, check time:%d seconds,report power off alarm %d seconds later than power on",get_enable_str(lang, from, is_on),power_alarm_check_time,power_off_alarm_delay);
				}
			}
			else if(para_num == 2)
			{
                is_on = is_on? 0: 1; //0,打开,1:关闭
				config_service_set(CFG_CUTOFFALM_DISABLE, TYPE_BYTE, &is_on, sizeof(is_on));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				
			}
			else if(para_num == 3)
			{
                is_on = is_on? 0: 1; //0,打开,1:关闭
				config_service_set(CFG_CUTOFFALM_DISABLE, TYPE_BYTE, &is_on, sizeof(is_on));
				config_service_set(CFG_POWER_CHECK_TIME, TYPE_SHORT, &power_alarm_check_time, sizeof(power_alarm_check_time));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				
			}
			else if(para_num == 4)
			{
                is_on = is_on? 0: 1; //0,打开,1:关闭
				config_service_set(CFG_CUTOFFALM_DISABLE, TYPE_BYTE, &is_on, sizeof(is_on));
				config_service_set(CFG_POWER_CHECK_TIME, TYPE_SHORT, &power_alarm_check_time, sizeof(power_alarm_check_time));
				config_service_set(CFG_POWER_CHARGE_MIN_TIME, TYPE_SHORT, &power_off_alarm_delay, sizeof(power_off_alarm_delay));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_MILEAGE:
		{
			S32 mileage = 0;
            S64 mileage_in_metre;
			para_num = command_scan((char*)p_cmd_content, "s;i", cmd_name,&mileage);
			if (para_num == 1)
			{		
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "里程:%lld公里",system_state_get_mileage()/1000);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Mileage:%lld km",system_state_get_mileage()/1000);
				}
			}
			else if(para_num == 2)
			{
                mileage_in_metre = mileage * 1000;
				system_state_set_mileage(mileage_in_metre);
				GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		//LOGIN,44,33,11111,0,0000099,1,粤B88888#
		case CMD_JT808_LOGIN:
		{
			U16 province_id = 0;             // 省域ID
    		U16 city_id = 0;                 // 市域ID
    		char oem_id[32] = {0};           // OEM商ID 
    		U8 protocol_type = 0;            // 部标协议类型(标准 补充)
    		char device_id[32] = {0};        // 终端ID
    		U8 license_color = 0;            // 车牌颜色
    		char license_number[32] = {0};   // 机动车号牌

			para_num = command_scan((char*)p_cmd_content, "s;iisisis", cmd_name,&province_id,&city_id,oem_id,&protocol_type,device_id,&license_color,license_number);
			if (para_num == 1)
			{
				config_service_get(CFG_JT_PROVINCE, TYPE_SHORT, &province_id, sizeof(province_id));
				config_service_get(CFG_JT_CITY, TYPE_SHORT, &city_id, sizeof(city_id));
				config_service_get(CFG_JT_OEM_ID, TYPE_STRING, oem_id, sizeof(oem_id));
				config_service_get(CFG_JT_AT_TYPE, TYPE_BYTE, &protocol_type, sizeof(protocol_type));
				config_service_get(CFG_JT_DEVICE_ID, TYPE_STRING, device_id, sizeof(device_id));
				config_service_get(CFG_JT_VEHICLE_COLOR, TYPE_BYTE, &license_color, sizeof(license_color));
				config_service_get(CFG_JT_VEHICLE_NUMBER, TYPE_STRING, license_number, sizeof(license_number));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "省域ID:%d,市域ID:%d,OEM商ID:%s,部标协议类型(标准 补充):%d,终端ID:%s,车牌颜色:%d,机动车号牌:%s",province_id,city_id,oem_id,protocol_type,device_id,license_color,license_number);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Province ID:%d,city ID:%d,OEM ID:%s,protocol type:%d,device ID:%s,license colour:%d,license number:%s",province_id,city_id,oem_id,protocol_type,device_id,license_color,license_number);
				}
			}
			else if(para_num == 8)
			{
				
				if(0 == device_id[0] || 0 == oem_id[0] || 0 == license_number[0] || protocol_type > GB_ATTYPE_CUSTOM)
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					config_service_set(CFG_JT_PROVINCE, TYPE_SHORT, &province_id, sizeof(province_id));
					config_service_set(CFG_JT_CITY, TYPE_SHORT, &city_id, sizeof(city_id));
					config_service_set(CFG_JT_OEM_ID, TYPE_STRING, oem_id, sizeof(oem_id));
					config_service_set(CFG_JT_AT_TYPE, TYPE_BYTE, &protocol_type, sizeof(protocol_type));
					config_service_set(CFG_JT_DEVICE_ID, TYPE_STRING, device_id, sizeof(device_id));
					config_service_set(CFG_JT_VEHICLE_COLOR, TYPE_BYTE, &license_color, sizeof(license_color));
					config_service_set(CFG_JT_VEHICLE_NUMBER, TYPE_STRING, license_number, sizeof(license_number));
					config_service_save_to_local();
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}	
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
    			
		}
		break;

		case CMD_SLEEP_TIME:
		{
			U16 sleep_time = 0;
			para_num = command_scan((char*)p_cmd_content, "s;i", cmd_name,&sleep_time);
			if (para_num == 1)
			{
				config_service_get(CFG_SLEEP_TIME, TYPE_SHORT, &sleep_time, sizeof(sleep_time));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "休眠时间:%d分钟",sleep_time);
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Sleep time:%d minutes",sleep_time);
				}
			}
			else if(para_num == 2)
			{
				if (sleep_time >= MINS_PER_HOUR)
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
					break;
				}
				config_service_set(CFG_SLEEP_TIME, TYPE_SHORT, &sleep_time, sizeof(sleep_time));
				config_service_save_to_local();
				GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		case CMD_123_POSITION:
		{
            if(! pmsg) break;
            p_sms =(gm_sms_new_msg_struct*)pmsg;
            
			para_num = command_scan((char*)p_cmd_content, "s", cmd_name);
			if (para_num == 1)
			{
                gps_service_push_position_request((u8*)&p_sms->asciiNum[0], GM_strlen(&p_sms->asciiNum[0]), (u8*)cmd_name, GM_strlen(cmd_name));
				GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}		
		}
		break;

		case CMD_LOG_LEVEL:
		{
            LogLevel log_level = WARN;
            LogLevel upload_level = FATAL;
			para_num = command_scan((char*)p_cmd_content, "s;ii", cmd_name,&log_level,&upload_level);
			if(para_num == 1)
			{
                log_service_get_level(&log_level, &upload_level);
				GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Log level:%d , Upload level:%d",log_level, upload_level);
			}
			else if(para_num == 2)
			{
                upload_level = FATAL;
				if (log_level <= FATAL)
				{
					log_service_set_level(log_level, upload_level);
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
			}
			else if(para_num == 3)
			{
				if (log_level <= FATAL && upload_level > DEBUG && upload_level <= FATAL)
				{
					log_service_set_level(log_level, upload_level);
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
        
		case CMD_SOS:
		{
            //设置SOS号码[特权号] SOS,A,号码1,号码2,号码3#
            //  SOS,A,13800138000#  设置第1个SOS电话
            //SOS,D,1,2,3#
            char add_or_del = 0;
            char sos_num[3][16] = {"","",""};   //长度定义要与g_para一致
            
			para_num = command_scan((char*)p_cmd_content, "s;csss", cmd_name,&add_or_del,sos_num[0],sos_num[1],sos_num[2]);
			if(para_num < 2)
			{
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "SOS号码:%s,%s,%s",
                        config_service_get_pointer(CFG_USER1_NUMBER),config_service_get_pointer(CFG_USER2_NUMBER),
                        config_service_get_pointer(CFG_USER3_NUMBER));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "SOSNumber:%s,%s,%s",
                        config_service_get_pointer(CFG_USER1_NUMBER),config_service_get_pointer(CFG_USER2_NUMBER),
                        config_service_get_pointer(CFG_USER3_NUMBER));
				}
			}
			else if(para_num >= 3)
			{
                if(add_or_del == 'A' || add_or_del == 'a')
                {
                    if(sos_num[0][0] != 0 && GM_strlen(sos_num[0])< sizeof(sos_num[0]))
                    {
                        config_service_set(CFG_USER1_NUMBER, TYPE_STRING, sos_num[0], GM_strlen((char *)sos_num[0]));
                    }
                    if(sos_num[1][0] != 0 && GM_strlen(sos_num[1])< sizeof(sos_num[1]))
                    {
                        config_service_set(CFG_USER2_NUMBER, TYPE_STRING, sos_num[1], GM_strlen((char *)sos_num[1]));
                    }
                    if(sos_num[2][0] != 0 && GM_strlen(sos_num[2])< sizeof(sos_num[2]))
                    {
                        config_service_set(CFG_USER3_NUMBER, TYPE_STRING, sos_num[2], GM_strlen((char *)sos_num[2]));
                    }
                    GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                    config_service_save_to_local();
                }
                else if(add_or_del == 'D' || add_or_del == 'd')
                {
                	U8 index = 0;
					for (index = 0; index < 3; ++index)
					{
						S32 del_index = GM_atoi(sos_num[0]);
						switch (del_index)
						{
							case 1:
							{
								config_service_set(CFG_USER1_NUMBER, TYPE_STRING, "", 0);
								break;
							}
							case 2:
							{
								config_service_set(CFG_USER2_NUMBER, TYPE_STRING, "", 0);
								break;
							}
							case 3:
							{
								config_service_set(CFG_USER3_NUMBER, TYPE_STRING, "", 0);
								break;
							}
								
						}
					}
                    config_service_save_to_local();
                    GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                }
                else
                {
                    GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
                }       
			}
            else
            {
                GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
            }       
		}
		break;

		case CMD_CENTER:
		{
            /*
            CENTER,A,中心号码# 
            CENTER,D# 
            CENTER,A,13800138000#  SET CENTER OK
            CENTER,D#      DEL CENTER OK
            */
            char add_or_del = '\0';
            char cent_num[20] = {0};
            
			para_num = command_scan((char*)p_cmd_content, "s;cs", cmd_name,&add_or_del,cent_num);
			if(para_num < 2)
			{
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "中心号码:%s",config_service_get_pointer(CFG_CENTER_NUMBER));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "CenterNumber:%s",config_service_get_pointer(CFG_CENTER_NUMBER));
				}
			}
			else if(para_num == 2)
			{
                if(add_or_del == 'D' || add_or_del == 'd')
                {
                    config_service_set(CFG_CENTER_NUMBER, TYPE_STRING, cent_num, 0);
                    config_service_save_to_local();
                    GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                }
                else
                {
                    GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
                }       
			}
			else if(para_num == 3)
			{
                if((add_or_del == 'A' || add_or_del == 'a') && cent_num[0] != 0)
                {
                    config_service_set(CFG_CENTER_NUMBER, TYPE_STRING, cent_num, GM_strlen((char *)cent_num));
	                config_service_save_to_local();
                    GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
                }
                else
                {
                    GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
                }       
			}
            else
            {
                GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
            }       
		}
		break;

		case CMD_GPRS_PARA:
		{
			bool is_on = true;
			config_service_get(CFG_APN_CHECK, TYPE_BOOL, &is_on, sizeof(is_on));
			if (1 == lang && COMMAND_GPRS == from)
			{
				GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "自适应APN:%s,APN名称:%s,用户名:%s,密码:%s,服务器:%s",get_enable_str(lang,from,is_on),(char*)config_service_get_pointer(CFG_APN_NAME),(char*)config_service_get_pointer(CFG_APN_USER),(char*)config_service_get_pointer(CFG_APN_PWD),(char*)config_service_get_pointer(CFG_SERVERADDR));
			}
			else
			{
				GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Auto APN:%s,APN name:%s,user:%s,passpord:%s,server:%s",get_enable_str(lang,from,is_on),(char*)config_service_get_pointer(CFG_APN_NAME),(char*)config_service_get_pointer(CFG_APN_USER),(char*)config_service_get_pointer(CFG_APN_PWD),(char*)config_service_get_pointer(CFG_SERVERADDR));
			}
		}
		break;

		case CMD_STATIC_UPLOAD:
		{
			bool is_on = false;
			para_num = command_scan((char*)p_cmd_content, "s;w", cmd_name,&is_on);
			if (para_num == 1)
			{
				config_service_get(CFG_IS_STATIC_UPLOAD, TYPE_BOOL, &is_on, sizeof(is_on));
				if (1 == lang && COMMAND_GPRS == from)
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "静止上传:%s",get_enable_str(lang, from, is_on));
				}
				else
				{
					GM_snprintf((char*)p_rsp, CMD_MAX_LEN, "Upload when static:%s",get_enable_str(lang, from, is_on));
				}
			}
			else if(para_num == 2)
			{

				config_service_set(CFG_IS_STATIC_UPLOAD, TYPE_BOOL, &is_on, sizeof(is_on));
				if(GM_SUCCESS != config_service_save_to_local())
				{	
					GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
				}
				else
				{
					GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
				}
				
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_TEST:
		{	
			if (COMMAND_SMS == from)
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
			else
			{
				auto_test_create(false);
				GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_EXIT:
		{
			auto_test_destroy();
			GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
		}
		break;

		case CMD_RECORD:
		{
			auto_test_get_result(true,p_rsp);
		}
		break;

		case CMD_RESULT:
		{
			auto_test_get_result(false,p_rsp);
		}
		break;

		case CMD_PMTK:
		{
			char pmtk_cmd[20] = {0};
			char pmtk_para1[20] = {0};
			char pmtk_para2[20] = {0};
			char pmtk_para3[20] = {0};
			char pmtk_para4[20] = {0};
			char pmtk_para5[20] = {0};
			para_num = command_scan((char*)p_cmd_content, "s;ssssss", cmd_name,pmtk_cmd,pmtk_para1,pmtk_para2,pmtk_para3,pmtk_para4,pmtk_para5);
			
			if(para_num == 7)
			{
				char pmtk_cmd_full[100] = {0};
				GM_snprintf(pmtk_cmd_full, 50, "%s,%s,%s,%s,%s,%s", pmtk_cmd,pmtk_para1,pmtk_para2,pmtk_para3,pmtk_para4,pmtk_para5);
				gps_write_mtk_cmd(pmtk_cmd_full);
			}
			else if(para_num == 6)
			{
				char pmtk_cmd_full[100] = {0};
				GM_snprintf(pmtk_cmd_full, 50, "%s,%s,%s,%s,%s", pmtk_cmd,pmtk_para1,pmtk_para2,pmtk_para3,pmtk_para4);
				gps_write_mtk_cmd(pmtk_cmd_full);
			}
			else if(para_num == 5)
			{
				char pmtk_cmd_full[100] = {0};
				GM_snprintf(pmtk_cmd_full, 50, "%s,%s,%s,%s", pmtk_cmd,pmtk_para1,pmtk_para2,pmtk_para3);
				gps_write_mtk_cmd(pmtk_cmd_full);
			}
			else if(para_num == 4)
			{
				char pmtk_cmd_full[50] = {0};
				GM_snprintf(pmtk_cmd_full, 50, "%s,%s,%s", pmtk_cmd,pmtk_para1,pmtk_para2);
				gps_write_mtk_cmd(pmtk_cmd_full);
			}
			else if(para_num == 3)
			{
				char pmtk_cmd_full[50] = {0};
				GM_snprintf(pmtk_cmd_full, 50, "%s,%s", pmtk_cmd,pmtk_para1);
				gps_write_mtk_cmd(pmtk_cmd_full);
			}
			else if(para_num == 2)
			{
				gps_write_mtk_cmd(pmtk_cmd);
			}
			else
			{
				GM_memcpy(p_rsp, set_fail_rsp(from), CMD_MAX_LEN);
			}
		}
		break;

		case CMD_HARD_REBOOT:
		{
			watch_dog_hard_reboot();
			GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
		}
		break;

		case CMD_BMS_BATTERY:
		{
			if (GM_EMPTY_BUF == bms_battery_request_response(p_rsp))
			{
				LOG(ERROR, "bms_battery_request_response len(%d), %s", GM_strlen(p_rsp), p_rsp);
				GM_memcpy(p_rsp, set_success_rsp(from), CMD_MAX_LEN);
			}
		}
		break;
		
		default:
		{
			U16 lang = 0;
			char wrong_command_rsp_english[CMD_MAX_LEN] = "Do not support the command!";
			char wrong_command_rsp_chinese[CMD_MAX_LEN] = "不支持此指令";
	        config_service_get(CFG_LANGUAGE, TYPE_SHORT, &lang, sizeof(lang));
			if (1 == lang && COMMAND_GPRS == from)
			{
				GM_memcpy(p_rsp, wrong_command_rsp_chinese, GM_strlen(wrong_command_rsp_chinese));
			}
			else
			{
				GM_memcpy(p_rsp, wrong_command_rsp_english, GM_strlen(wrong_command_rsp_english));
			}
		}
	}
	
	do
	{
		JsonObject* p_log = json_create();
		json_add_string(p_log, "event", "command");
		json_add_int(p_log, "from", from);
		json_add_string(p_log, "request", p_cmd_content);
		json_add_string(p_log, "response", p_rsp);
		log_service_upload(INFO, p_log);
	}while (0);
	
	GM_MemoryFree(p_cmd_content);
	p_cmd_content = NULL;
	return GM_SUCCESS;
}

//返回参数个数,-1——失败；0——无参数
static char command_scan(const char* p_command, const char* p_format, ...)
{
    bool para_num = false;
    bool optional = false;
	const char* p_field = p_command;
	char type = 0;
	S32 value_32 = 0;
	char* p_buf = NULL;
    char switch_text[8] = {0};
	
	U8 index = 0;
	
	
    va_list ap;
    va_start(ap, p_format);

    while (*p_format) 
	{
        type = *p_format++;

        if (type == ';') 
		{
            // 后面所有的域都是可选的
            optional = true;
            continue;
        }

        if (!p_field && !optional) 
		{
            goto parse_error;
        }

        switch (type) 
		{
			case 'c': 
			{ 
				char value = 0;
                if (p_field && util_isprint((U8)*p_field) && *p_field != ',' && *p_field != '#')
                {
                    value = *p_field;
					para_num++;
                }
                *va_arg(ap, char*) = value;
            } 
			break;
			
            case 'w': 
			{ 
                index = 0;
                if (p_field) 
				{
                    while (util_isprint((U8)*p_field) && *p_field != ',' && *p_field != '#')
                    {
                        switch_text[index++] = *p_field++;
                    }
                }
				switch_text[index] = 0;

				util_string_upper((U8*)switch_text,GM_strlen(switch_text));

				if (!GM_strcmp(switch_text, "ON") || !GM_strcmp(switch_text, "1"))
				{
					*va_arg(ap, char*) = true;
					para_num++;
				}
				else if (!GM_strcmp(switch_text, "OFF") || !GM_strcmp(switch_text, "0"))
				{
					*va_arg(ap, char*) = false;
					para_num++;
				}
				else
				{
				}
				
            } 
			break;
			
			// Integer value, default 0 (S32).
            case 'i': 
			{ 
                value_32 = 0;

                if (p_field && util_isdigit(*p_field)) 
				{	
                    char *endptr;
                    value_32 = util_strtol(p_field, &endptr);
                    if (util_isprint((U8)*endptr) && *endptr != ',' && *endptr != '#')
                    {
                    	
                        goto parse_error;
                    }
					para_num++;
                }
                *va_arg(ap, S32*) = value_32;
				
            } 
			break;

			// String value (char *).
            case 's': 
			{ 
                p_buf = va_arg(ap, char*);

                if (p_field && util_isprint((U8)*p_field) && *p_field != ',' && *p_field != '#') 
				{
                    while (util_isprint((U8)*p_field) && *p_field != ',' && *p_field != '#')
                    {
                        *p_buf++ = *p_field++;
                    }
					para_num++;
                }
                *p_buf = '\0';
            } 
			break;

            default:
			{ 
                goto parse_error;
            }
        }

        /* Progress to the next p_field. */
    	while (util_isprint((U8)*p_command) && *p_command != ',' && *p_command != '#')
    	{
        	p_command++;
    	}
    	/* Make sure there is a p_field there. */ 
    	if (*p_command == ',') 
		{ 
        	p_command++;
        	p_field = p_command;
    	} 
		else 
		{
        	p_field = NULL;
    	}
    }

parse_error:
    va_end(ap);
    return para_num;
}

