/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        sms.h
 * Author:           李耀轩     
 * Version:          1.0
 * Date:             2019-03-07
 * Description:      
 * Others:      
 * Function List:    
    1. 创建call模块
    2. 销毁call模块
    3. call模块定时处理入口
 * History: 
    1. Date:         2019-03-07
       Author:       李耀轩
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __CALL_H__
#define __CALL_H__

#include "gm_type.h"
#include "error_code.h"
#include "gm_call.h"

/**
 * Function:   创建call模块
 * Description:创建call模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE call_create(void);

/**
 * Function:   销毁call模块
 * Description:销毁call模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE call_destroy(void);

/**
 * Function:   call模块定时处理入口
 * Description:call模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   1秒钟调用1次
 */
GM_ERRCODE call_timer_proc(void);



/**
 * Function:   发送短信
 * Description:
 * Input:	 
 *         dist_number[in]:拨打号码参数指针
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:
 */
GM_ERRCODE gm_make_phone_call(const u8 *dist_number);

#endif
