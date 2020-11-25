/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        gsm.h
 * Author:           李耀轩       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建gsm模块
    2. 销毁gsm模块
    3. gsm模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       李耀轩
       Modification: 创建初始版本
       
    2. Date: 	     2019-04-08	 
	   Author:		 王志华
	   Modification: 实现基本功能

 */

#include <ctype.h>
#include <gm_network.h>
#include <gm_callback.h>
#include <gm_timer.h>
#include <gm_call.h>
#include <gm_stdlib.h>
#include <gm_system.h>
#include "utility.h"
#include "gsm.h"
#include "log_service.h"
#include "gps_service.h"
#include "system_state.h"
#include "gps.h"
#include "led.h"
#include "config_service.h"
#include "auto_test.h"


//伪基站检查周期20秒
#define GM_FAKE_CELL_CHECK_PERIOD 20

//伪基站持续时间120秒,报警
#define GM_FAKE_CELL_CONTINUE_MAX_TIME 120

//伪基站持续时间内总次数达到50次以上才报警
#define GM_FAKE_CELL_CONTINUE_MAX_COUNT 50


#define GM_CSQ_MIN 0
#define GM_CSQ_MAX 31



typedef struct
{
	bool sim_is_valid;

	U8 imei[GM_IMEI_LEN + 1];
	U8 imsi[GM_IMSI_LEN + 1];
	U8 iccid[GM_ICCID_LEN + 1];

	gm_cell_info_struct cell_info;

	//每20秒收到伪基站回调次数
	U8 fake_cell_count_per_20second;

	//每120秒收到伪基站回调次数
	U8 fake_cell_count_per_120second;

	//伪基站信号持续时间（秒）
	U8 fake_cell_continue_seconds;  
}GSM;

static GSM s_gsm;

static void check_sim_card(void);
GsmRegState gsm_get_creg_state(void);
static bool imei_is_valid(U8* p_imei);
static void get_gsm_info(void);
static void gsm_iccid_callback(void* p_msg);
static void gsm_lbs_callback(void* p_msg);
static void gsm_imei_callback(void* p_imei);
static void gsm_call_status_callback(void* p_msg);
static void gsm_incomming_call(void* evt);
static void gsm_fake_cell_callback(void* evt);
static void check_fake_cell_alarm_timer_proc(void);





GM_ERRCODE gsm_create(void)
{
	s_gsm.sim_is_valid = false;
	GM_memset(s_gsm.imei,0,GM_IMEI_LEN + 1);
	GM_memset(s_gsm.imsi,0,GM_IMSI_LEN + 1);
	GM_memset(s_gsm.iccid,0,GM_ICCID_LEN + 1);

	GM_memset(&s_gsm.cell_info,0,sizeof(s_gsm.cell_info));
	
	s_gsm.fake_cell_count_per_20second = 0;
	s_gsm.fake_cell_count_per_120second = 0;
	s_gsm.fake_cell_continue_seconds = 0;
	
	GM_RegisterCallBack(GM_CB_FAKE_CELL_RECEIVE, (U32)gsm_fake_cell_callback);
    GM_RegisterCallBack(GM_CB_CALL_RECEIVE, (U32)gsm_incomming_call);

	GM_StartTimer(GM_TIMER_GSM_CHECK_SIMCARD, TIM_GEN_1SECOND*2, check_sim_card);
	GM_StartTimer(GM_TIMER_GSM_CHECK_FAKE_CELL_ALARM, TIM_GEN_1SECOND*GM_FAKE_CELL_CHECK_PERIOD, check_fake_cell_alarm_timer_proc);	
	return GM_SUCCESS;
}

GM_ERRCODE gsm_destroy(void)
{
	return GM_SUCCESS;
}

GsmRegState gsm_get_creg_state(void)
{
	GsmRegState status = GSM_CREG_UNREGISTER;
	if(!GM_GetCregStatus(&status))
	{
		LOG(ERROR, "Failed to GM_GetCregStatus!");
	}
	return status;
}

GM_ERRCODE gsm_get_iccid(U8* p_iccid)
{
	U8 len = GM_strlen((char*)s_gsm.iccid);
	if (GM_ICCID_LEN == len)
	{
		GM_memcpy(p_iccid, s_gsm.iccid, GM_ICCID_LEN);
		p_iccid[GM_ICCID_LEN] = 0;
		return GM_SUCCESS;
	}
	else
	{
		return GM_NOT_INIT;
	}
}

GM_ERRCODE gsm_get_imsi(U8* p_imsi)
{
	U8 len = GM_strlen((char*)s_gsm.imsi);
	if (GM_IMSI_LEN == len)
	{
		GM_memcpy(p_imsi, s_gsm.imsi, GM_IMSI_LEN);
		p_imsi[GM_IMSI_LEN] = 0;
		return GM_SUCCESS;
	}
	else
	{
		return GM_NOT_INIT;
	}
}

GM_ERRCODE gsm_get_imei(U8* p_imei)
{
	U8 len = GM_strlen((char*)s_gsm.imei);
	if (GM_IMEI_LEN == len)
	{
		GM_memcpy(p_imei, s_gsm.imei, GM_IMEI_LEN);
		p_imei[GM_IMEI_LEN] = 0;
		return GM_SUCCESS;
	}
	else
	{
		GM_memcpy(p_imei, GOOME_IMEI_DEFAULT, GM_IMEI_LEN);
		return GM_NOT_INIT;
	}
}

GM_ERRCODE gsm_set_imei(U8* p_imei)
{
	U8 index_string = 0;
	U8 index_hex = 0;
	U8 imei_hex_write[GM_IMEI_HEX_LEN] = {0};
	U8 imei_hex_read[GM_IMEI_HEX_LEN] = {0};
	U16 lib_index = 0;
	S32 result = 0;
	
	if (!imei_is_valid(p_imei))
	{
		LOG(ERROR,"Invalid IMEI:%s",p_imei);
		return GM_PARAM_ERROR;
	}

	//写入IMEI号，停止自检启动定时器
	GM_StopTimer(GM_TIMER_SELF_CHECK_START);
	auto_test_destroy();
	
    for (index_string = 0; index_string < GM_IMEI_LEN - 1; index_string += 2)
    {
        imei_hex_write[index_hex++] = (util_chr(p_imei[index_string + 1]) << 4) + util_chr(p_imei[index_string]);
    }

    imei_hex_write[index_hex++] = 0xF0 + util_chr(p_imei[GM_IMEI_LEN - 1]);
    imei_hex_write[index_hex++] = 0xFF;
    imei_hex_write[index_hex++] = 0xFF;


    lib_index = GM_ReadNvramLid(NVRAM_EF_IMEI_IMEISV_LID);
    GM_ReadWriteNvram(false, lib_index, 1, imei_hex_write, GM_IMEI_HEX_LEN, &result);    

	//写入长度正确,再读出来校验一次
    if (result > 0)
    {
        GM_ReadWriteNvram(true, lib_index, 1, imei_hex_read, GM_IMEI_HEX_LEN, &result);
        if (0 == GM_memcmp(imei_hex_write, imei_hex_read, GM_IMEI_HEX_LEN))
        {
            GM_memcpy(s_gsm.imei, p_imei, GM_IMEI_LEN);
			LOG(INFO,"Succeed to write IMEI:%s",p_imei);
            return GM_SUCCESS;
        }
        else
        {
            LOG(ERROR,"IMEI check error,read:%s,write:%s",imei_hex_read,p_imei);
            return GM_SYSTEM_ERROR;
        }
    }
    else
    {
    	LOG(ERROR,"Failed to write IMEI!");
        return GM_SYSTEM_ERROR;
    }
}

static bool imei_is_valid(U8* p_imei)
{
	U8 index = 0;
	if (GM_IMEI_LEN != GM_strlen((char*)p_imei))
    {
    	return false;
    }

    for (index = 0;  index < GM_IMEI_LEN;  index++)
    {
        if (!util_isdigit(p_imei[index]))
        {
            return false;
        }
    }
	return true;
}


U8 gsm_get_csq(void)
{
	S32 csq = GM_GetSignalValue();
	csq = csq < GM_CSQ_MIN ? GM_CSQ_MIN : csq;
	csq = csq > GM_CSQ_MAX ? GM_CSQ_MAX : csq;
	return (U8)csq;
}

GM_ERRCODE gsm_get_cell_info(gm_cell_info_struct* p_cell_info)
{
	if (NULL == p_cell_info)
	{
		return GM_PARAM_ERROR;
	}

	if (!s_gsm.sim_is_valid)
	{
		LOG(ERROR,"SIM is invalid");
		return GM_NOT_INIT;
	}
    if((s_gsm.cell_info.serv_info.lac > 0)&&(s_gsm.cell_info.serv_info.ci > 0))
    {
        *p_cell_info = s_gsm.cell_info;
    	LOG(DEBUG,"Succeed to Get LBS");
    	return GM_SUCCESS;
    }

    return GM_NOT_INIT;
}

static void check_sim_card(void)
{
	if (GM_CheckSimValid())
    {
        if (!s_gsm.sim_is_valid)
        {
           LOG(DEBUG,"sim ready!");
        }
        s_gsm.sim_is_valid = true;
    }
    else
    {
       s_gsm.sim_is_valid = false;
    }
	get_gsm_info();
}

static void get_gsm_info(void)
{
	GM_GetIccid(gsm_iccid_callback);
	GM_GetLbsInfo(gsm_lbs_callback);
	GM_memset(s_gsm.imsi, 0, sizeof(s_gsm.imsi));
    GM_GetImsi((char*)s_gsm.imsi);
	GM_GetImei(gsm_imei_callback);
}

//20秒内出现50次以上伪基站信号报警，20秒内没有伪基站信号报警恢复，报警最先间隔2分钟
static void gsm_iccid_callback(void* p_msg)
{
	if (NULL == p_msg || GM_strlen((char*)p_msg) != GM_ICCID_LEN)
    {
		return;
    }
    GM_memcpy(s_gsm.iccid, p_msg, GM_ICCID_LEN);
}

static void gsm_lbs_callback(void* p_msg)
{
	U32 delay = 0;
    if (NULL == p_msg)
    {
	    GM_StartTimer(GM_TIMER_GSM_CHECK_SIMCARD, TIM_GEN_1SECOND, check_sim_card);
        return;
    }
    
    GM_memcpy((void *)&s_gsm.cell_info, (void *)(p_msg), sizeof(gm_cell_info_struct));
	if (s_gsm.cell_info.nbr_cell_num == 0)
	{
		delay = TIM_GEN_1SECOND;
	}
	else
	{
		delay = TIM_GEN_1SECOND * 10;
	}
	GM_StartTimer(GM_TIMER_GSM_CHECK_SIMCARD, delay, check_sim_card);
}

static void gsm_imei_callback(void* p_imei)
{
	if (NULL == p_imei || !imei_is_valid(p_imei))
	{
		return;
    }
	if (GM_IMEI_LEN != GM_strlen((char*)s_gsm.imei))
	{
		GM_memcpy(s_gsm.imei, p_imei, GM_IMEI_LEN);
	}
}

static void gsm_call_status_callback(void* p_msg)
{
    int status = *(int*) p_msg;
  
    switch(status)
    {
        case 0:
            LOG(INFO,"phone calling."); 
            break;
        case 1:
            LOG(INFO,"call OK."); 
            break;
        case 2:
            LOG(INFO,"call hang."); 
            break;
    }
}

static void gsm_incomming_call(void* p_phone_number)
{
	if (NULL == p_phone_number)
	{
		return;
	}
    LOG(INFO,"incoming:%s", (char*)p_phone_number);
    GM_CallAnswer(gsm_call_status_callback);
}

static void gsm_fake_cell_callback(void* evt)
{
    u16* arfcn =(u16 *)evt;
	LOG(DEBUG,"gsm_fake_cell_callback: arfcn=%d,%d,%d,%d",*arfcn,
		s_gsm.fake_cell_count_per_20second,
		s_gsm.fake_cell_count_per_120second,
		s_gsm.fake_cell_continue_seconds);
    
	s_gsm.fake_cell_count_per_20second++;
	s_gsm.fake_cell_count_per_120second++;
}

//每20秒内有伪基站回调,连续6个20秒都有回调,且总次数大于50次报警
static void check_fake_cell_alarm_timer_proc(void)
{
	if (s_gsm.fake_cell_count_per_20second > 0)
	{
		s_gsm.fake_cell_continue_seconds += GM_FAKE_CELL_CHECK_PERIOD;
	}
	else
	{
		s_gsm.fake_cell_count_per_120second = 0;
		s_gsm.fake_cell_continue_seconds = 0;
		system_state_set_fakecell_alarm(false);
	}

	//20秒计数器清零
	s_gsm.fake_cell_count_per_20second = 0;

	if (s_gsm.fake_cell_continue_seconds >= GM_FAKE_CELL_CONTINUE_MAX_TIME)
	{
		//1、连续超过120秒
		//2、休眠状态
		//3、未定位
		if (s_gsm.fake_cell_count_per_120second > GM_FAKE_CELL_CONTINUE_MAX_COUNT 
			&& system_state_get_work_state() == GM_SYSTEM_STATE_SLEEP
			&& !gps_is_fixed())
		{
			AlarmInfo alarm_info;
			alarm_info.type = ALARM_FAKE_CELL;
			gps_service_push_alarm(&alarm_info);
			system_state_set_fakecell_alarm(true);
		}
		s_gsm.fake_cell_count_per_120second = 0;
		s_gsm.fake_cell_continue_seconds = 0;
	}
}

