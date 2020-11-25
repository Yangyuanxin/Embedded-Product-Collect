/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        led.h
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

#ifndef __LED_H__
#define __LED_H__

#include "gm_type.h"
#include "error_code.h"


typedef enum 
{
    GM_LED_OFF = 0,           //灭
    GM_LED_ON = 1,            //亮
    GM_LED_FLASH = 2,         //闪烁
}LedState;


/**
 * Function:   创建hard_ware模块
 * Description:创建hard_ware模块
 * Input:      无
 * Output:     无
 * Return:     GM_SUCCESS——成功；其它错误码——失败
 * Others:     使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE led_create(void);

/**
 * Function:   销毁hard_ware模块
 * Description:销毁hard_ware模块
 * Input:      无
 * Output:     无
 * Return:     GM_SUCCESS——成功；其它错误码——失败
 * Others:       
 */
GM_ERRCODE led_destroy(void);

/**
 * Function:   hard_ware模块定时处理入口
 * Description:hard_ware模块定时处理入口
 * Input:       无
 * Output:      无
 * Return:      无
 * Others:      100毫秒调用1次
 */
void led_timer_proc(void);

/**
 * Function:   设置GPS LED状态
 * Description:
 * Input:      state:状态
 * Output:     无
 * Return:     无
 * Others:       
 */
void led_set_gps_state(LedState state);

/**
 * Function:   设置GSM LED状态
 * Description:
 * Input:      state:状态
 * Output:     无
 * Return:     无
 * Others:       
 */
void led_set_gsm_state(LedState state);

#endif



