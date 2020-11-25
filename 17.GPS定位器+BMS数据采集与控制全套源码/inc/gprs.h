/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        gprs.h
 * Author:           李耀轩       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建gprs模块
    2. 销毁gprs模块
    3. gprs模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       李耀轩
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __GPRS_H__
#define __GPRS_H__

#include "gm_type.h"
#include "error_code.h"

/**
 * Function:   创建gprs模块
 * Description:创建gprs模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE gprs_create(void);

/**
 * Function:   销毁gprs模块
 * Description:销毁gprs模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE gprs_destroy(void);

/**
 * Function:   gprs模块定时处理入口
 * Description:gprs模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   1秒钟调用1次
 */
GM_ERRCODE gprs_timer_proc(void);

/**
 * Function:   判断GPRS是否正常
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   true——正常；false——不正常
 * Others:	   
 */
bool gprs_is_ok(void);


/**
 * Function:   判断是否需要重启
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   true——重启；false——不重启
 * Others:	   
 */
bool gprs_check_need_reboot(u32 check_time);

u32 gprs_get_last_good_time(void);
u32 gprs_get_call_ok_count(void);

#endif



