/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        led.c
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-04-08
 * Description:      封装LED接口
 * Others:           提供GSM、GPS指示灯常亮、灭、闪烁等功能
 * Function List:    
    1. 创建led模块
    2. 销毁led模块
    3. 定时处理入口
    4. 
    
 * History: 
    1. Date:         2019-04-08
       Author:       王志华
       Modification: 创建初始版本
    2. Date:          
       Author:         
       Modification: 

 */

#include <gm_timer.h>
#include "led.h"
#include "hard_ware.h"
#include "log_service.h"
#include "system_state.h"

//处理LED定时器周期（毫秒）
#define GM_LED_PROC_PERIOD_MS 10

//GSM指示灯开始时快闪的时间（秒）
#define GM_GSM_LED_QUICK_FLASH_SECONDS 7

//指示灯快闪周期（毫秒）
#define GM_LED_QUICK_FLASH_PERIOD_MS 100

//指示灯慢闪亮灯时长（毫秒）
#define GM_LED_SLOW_FLASH_ON_MS 100

//指示灯慢闪灭灯时长（毫秒）
#define GM_LED_SLOW_FLASH_OFF_MS 1900




typedef struct
{
	U32 start_time_ms;

	bool gsm_led_is_on;
	U16 gsm_led_state_time;
	LedState gsm_state;
	
	bool gps_led_is_on;
	U16 gps_led_state_time;
	LedState gps_state;
	

}Led;

static Led s_led;

static void gsm_led_proc(void);

static void gps_led_proc(void);


GM_ERRCODE led_create(void)
{
	s_led.start_time_ms = 0;
	
	s_led.gsm_led_is_on = false;
	s_led.gsm_led_state_time = 0;
	s_led.gsm_state = GM_LED_FLASH;
	
	s_led.gps_led_is_on = false;
	s_led.gps_led_state_time = 0;
	s_led.gps_state = GM_LED_FLASH;
	
	return GM_SUCCESS;
}


GM_ERRCODE led_destroy(void)
{
	return GM_SUCCESS;
}

void led_timer_proc(void)
{
	s_led.start_time_ms += GM_LED_PROC_PERIOD_MS;

	gsm_led_proc();
	gps_led_proc();  
}

static void gsm_led_proc(void)
{
	s_led.gsm_led_state_time += GM_LED_PROC_PERIOD_MS;
	
	//7秒以内快闪
	if (s_led.start_time_ms < GM_GSM_LED_QUICK_FLASH_SECONDS * TIM_GEN_1SECOND)
	{
		if (s_led.gsm_led_state_time >= GM_LED_QUICK_FLASH_PERIOD_MS)
        {
			s_led.gsm_led_is_on = !s_led.gsm_led_is_on;
            hard_ware_set_gsm_led(s_led.gsm_led_is_on);
			s_led.gsm_led_state_time = 0;
        } 
	}
	else
	{
		//常亮或者长灭
		if (s_led.gsm_state != GM_LED_FLASH)
		{
			 s_led.gsm_led_is_on = (bool)s_led.gsm_state;
			 hard_ware_set_gsm_led(s_led.gsm_led_is_on);
			 s_led.gsm_led_state_time = 0;
		}
		//慢闪
		else
		{
			if (s_led.gsm_led_is_on && s_led.gsm_led_state_time >= GM_LED_SLOW_FLASH_ON_MS)
			{
				s_led.gsm_led_is_on = false;
				hard_ware_set_gsm_led(s_led.gsm_led_is_on);
				s_led.gsm_led_state_time = 0;
				
			}
			else if(false == s_led.gsm_led_is_on && s_led.gsm_led_state_time >= GM_LED_SLOW_FLASH_OFF_MS)
			{
				s_led.gsm_led_is_on = true;
				hard_ware_set_gsm_led(s_led.gsm_led_is_on);
				s_led.gsm_led_state_time = 0;
			}
			else
			{
			}
		}
    }
}

static void gps_led_proc(void)
{
	s_led.gps_led_state_time += GM_LED_PROC_PERIOD_MS;
	
	//常亮或者长灭
	if (s_led.gps_state != GM_LED_FLASH)
	{
		 s_led.gps_led_is_on = (bool)s_led.gps_state;
		 hard_ware_set_gps_led(s_led.gps_led_is_on);
		 s_led.gps_led_state_time = 0;
	}
	//快闪
	else
	{
		if (s_led.gps_led_state_time >= GM_LED_QUICK_FLASH_PERIOD_MS)
        {
			s_led.gps_led_is_on = !s_led.gps_led_is_on;
            hard_ware_set_gps_led(s_led.gps_led_is_on);
			s_led.gps_led_state_time = 0;
        } 
	}
}


void led_set_gps_state(LedState state)
{
	s_led.gps_state = state;
}

void led_set_gsm_state(LedState state)
{
	s_led.gsm_state = state;
}

