/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        sms.h
 * Author:           李耀轩     
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建sms模块
    2. 销毁sms模块
    3. sms模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       李耀轩
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __SMS_H__
#define __SMS_H__

#include "gm_type.h"
#include "error_code.h"
#include "gm_sms.h"

/**
 * Function:   创建sms模块
 * Description:创建sms模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE sms_create(void);

/**
 * Function:   销毁sms模块
 * Description:销毁sms模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE sms_destroy(void);

/**
 * Function:   发送短信
 * Description:
 * Input:	   pdata:发送短信内容
 *             dist_number:发送短信号码
 *             contentDcs:发送短信格式
 * Output:	   无
 * Return:	   GM_SUCCESS——成功接收,存放发送短信缓存；其它错误码——失败
 * Others:
 */
GM_ERRCODE sms_send(const char* p_data, u16 data_len, char* number, ENUM_SMSAL_DCS doc_type);

#endif



