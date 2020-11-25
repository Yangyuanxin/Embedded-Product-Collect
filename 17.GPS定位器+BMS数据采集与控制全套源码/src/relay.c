/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        relay.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建relay模块
    2. 销毁relay模块
    3. relay模块定时处理入口
    4. 断油电
    5. 恢复油电
 * History: 
    1. Date:         2019-03-01
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#include "relay.h"
#include "hard_ware.h"
#include "gps.h"
#include "system_state.h"
#include "utility.h"
#include "log_service.h"

/**
 * Function:   创建relay模块
 * Description:创建relay模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE relay_create(void)
{
	if (system_state_get_user_relay_state())
	{
		return relay_on(false);
	}
    
    //lz add for compiler error
    return GM_SUCCESS;
}

/**
 * Function:   销毁relay模块
 * Description:销毁relay模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE relay_destroy(void)
{
	return GM_SUCCESS;
}

/**
 * Function:   relay模块定时处理入口
 * Description:不满足断油电条件的,定时检查条件,条件满足时执行
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   1秒钟调用1次
 */
GM_ERRCODE relay_timer_proc(void)
{
	bool user_relay_state = system_state_get_user_relay_state();
	bool device_relay_state = system_state_get_device_relay_state();
	if (user_relay_state != device_relay_state)
	{
		if (true == user_relay_state)
		{
			return relay_on(true);
		}
		else
		{
			return relay_off();
		}
	}
	else
	{
		return GM_SUCCESS;
	}
}

/**
 * Function:   断油电
 * Description:接常闭继电器,打开继电器=断油电,不满足执行条件时会在条件满足时执行最后一条指令
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE relay_on (bool check_speed)
{
	GM_ERRCODE ret = GM_SUCCESS;
	GPSData gps_data;

	system_state_set_user_relay_state(true);

	//如果当前是休眠状态,直接断油电
	//如果当前是静止状态,        直接断油电
	//如果当前系统刚刚启动(小于2秒）,直接断油电
	if (GM_SYSTEM_STATE_SLEEP == system_state_get_work_state() 
		|| VEHICLE_STATE_STATIC == system_state_get_vehicle_state()
		|| system_state_get_start_time() < 2)
	{
		ret = hard_ware_set_relay(true);
		if (GM_SUCCESS == ret)
		{
			JsonObject* p_json_log = json_create();
			json_add_string(p_json_log,"event","relay on");
			json_add_string(p_json_log,"state","static");
			log_service_upload(INFO, p_json_log);
			system_state_set_device_relay_state(true);
			LOG(INFO,"Succeed to open relay.");
		}
		return ret;
	}
    //否则检查速度情况
	if (check_speed && !gps_get_last_data(&gps_data))
	{
		return GM_WILL_DELAY_EXEC; 
	}
	//如果是3D定位并且速度小于20,直接执行,否则会延迟执行
	if (!check_speed || (gps_is_fixed() && gps_data.speed < 20))
	{
		ret = hard_ware_set_relay(true);
		if (GM_SUCCESS == ret)
		{
			JsonObject* p_json_log = json_create();
			json_add_string(p_json_log,"event","relay on");
			json_add_string(p_json_log,"state","low speed");
			log_service_upload(INFO, p_json_log);
			system_state_set_device_relay_state(true);
			LOG(INFO,"Succeed to open relay.");
		}
		return ret;
	}
	else
	{
		return GM_WILL_DELAY_EXEC;
	}
}

/**
 * Function:   恢复油电
 * Description:接常闭继电器,关闭继电器=恢复油电,不满足执行条件时会在条件满足时执行最后一条指令
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE relay_off (void)
{
	GM_ERRCODE ret = GM_SUCCESS;

	system_state_set_user_relay_state(false);

	//如果当前状态和用户希望的状态一致,直接返回成功
	if (false == system_state_get_device_relay_state())
	{
		return GM_SUCCESS;
	}
	
	ret = hard_ware_set_relay(false);
	if (GM_SUCCESS == ret)
	{
		JsonObject* p_json_log = json_create();
		json_add_string(p_json_log,"event","relay off");
		log_service_upload(INFO, p_json_log);
		system_state_set_device_relay_state(false);
	}
	return ret;
}

