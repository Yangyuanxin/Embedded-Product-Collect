/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        watch_dog.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-06
 * Description:      实现软件看门狗功能
 * Others:      
 * Function List:    
    1. 创建
    2. 销毁 
    3. 定时处理入口
    4. 开启/关闭软件看门狗
 * History:  
    1. Date:         2019-03-06    
       Author:       王志华 
       Modification  创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#include "watch_dog.h"
#include "gm_wdt.h"
#include "gm_memory.h"
#include "hard_ware.h"

//第1个软件看门狗清零时间（10秒）
#define GM_FIRST_SW_WDT_CLEAR_TIME 10

//第2个软件看门狗重启时间（1800秒=30分钟）
#define GM_SECCOND_SW_WDT_RESTART_TIME 1800

//第2个软件看门狗清零时间（300秒=5分钟）
#define GM_SECCOND_SW_WDT_CLEAR_TIME 300

typedef struct
{
	bool software_watchdog_enable;
	bool last_gpio_pin_level;
	bool stop_feed_hard_watch_dog;
}WatchDog, *PWatchDog;
	
static WatchDog s_watch_dog;

GM_ERRCODE watch_dog_create(void)
{
	
	//第1个软件看门狗,内部固定30秒重启
	GM_SoftwareWdtCounterEnable(true);

	//第2软件看门狗,自定义重启时长
	//使用两个软件看门狗是为了确保可靠性
	GM_SetSwLogicCounterEnable(true);
	GM_SetSwLogicCounterMax(GM_SECCOND_SW_WDT_RESTART_TIME);

	s_watch_dog.stop_feed_hard_watch_dog = false;
	s_watch_dog.software_watchdog_enable = true;
	s_watch_dog.last_gpio_pin_level = false;
	
	return GM_SUCCESS;
}

GM_ERRCODE watch_dog_destroy(void)
{
	GM_SoftwareWdtCounterEnable(false);
	GM_SetSwLogicCounterEnable(false);

	return GM_SUCCESS;
}

GM_ERRCODE watch_dog_timer_proc(void)
{
	
	if (!s_watch_dog.stop_feed_hard_watch_dog)
	{
		//处理硬件看门狗
		s_watch_dog.last_gpio_pin_level = !(s_watch_dog.last_gpio_pin_level);
		hard_ware_set_watchdog(s_watch_dog.last_gpio_pin_level);
	}

	//处理软件看门狗
	if (s_watch_dog.software_watchdog_enable)
	{
	    if (GM_GetSoftwareWdtCounter() >= GM_FIRST_SW_WDT_CLEAR_TIME)
        {
            GM_SetSoftwareWdtCounter(0);
        }

        if (GM_GetSwLogicCounter() >= GM_SECCOND_SW_WDT_CLEAR_TIME)
        {
            GM_SetSwLogicCounter(0);
        }
	}
	
	return GM_SUCCESS;
}

GM_ERRCODE watch_dog_enable(bool enable)
{
	s_watch_dog.software_watchdog_enable = enable;
	
	GM_SoftwareWdtCounterEnable(enable);
	GM_SetSwLogicCounterEnable(enable);
	
	return GM_SUCCESS;
}

GM_ERRCODE watch_dog_hard_reboot(void)
{
	s_watch_dog.stop_feed_hard_watch_dog = true;
	return GM_SUCCESS;
}

