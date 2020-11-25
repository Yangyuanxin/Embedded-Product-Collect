/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        auto_test.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-05-25
 * Description:      
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

#ifndef __AUTO_TEST_H__
#define __AUTO_TEST_H__
 
#include "gm_type.h"
#include "error_code.h"
#include "system_state.h"

 
 /**
  * Function:	创建auto_test模块
  * Description:创建auto_test模块
  * Input:		is_self_check:true——自检；false——自动化测试
  * Output: 	无
  * Return: 	GM_SUCCESS——成功；其它错误码——失败
  * Others: 	使用前必须调用,否则调用其它接口返回失败错误码
  */
GM_ERRCODE auto_test_create(bool is_self_check);
 
 /**
  * Function:	销毁auto_test模块
  * Description:销毁auto_test模块
  * Input:		无
  * Output: 	无
  * Return: 	GM_SUCCESS——成功；其它错误码——失败
  * Others: 	  
  */
GM_ERRCODE auto_test_destroy(void);

/**
 * Function:   自检结果恢复默认值
 * Description:
 * Input: 	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   恢复出厂设置时调用
 */
GM_ERRCODE auto_test_reset(void);

 
 /**
  * Function:	auto_test模块定时处理入口
  * Description:auto_test模块定时处理入口
  * Input:		 无
  * Output: 	 无
  * Return: 	 GM_SUCCESS——成功；其它错误码——失败
  * Others: 	 1秒调用1次
  */
GM_ERRCODE auto_test_timer_proc(void);

/**
  * Function:	获取自检结果
  * Description:
  * Input:		result:自检结果json字符串
  * Output: 	无
  * Return: 	无
  * Others: 	无
  */
void auto_test_get_result(bool is_history,char* json_str);

/**
  * Function:	acc开计数器
  * Description:
  * Input:		无
  * Output: 	无
  * Return: 	无
  * Others: 	无
  */
void auto_test_acc_on(void);

/**
  * Function:	acc关闭计数器
  * Description:
  * Input:		无
  * Output: 	无
  * Return: 	无
  * Others: 	无
  */
void auto_test_acc_off(void);

/**
  * Function:	修复GSM校准值
  * Description:03设备校准值大于540修改为540,05设备校准值大于520修改为520
  * Input:		无
  * Output: 	无
  * Return: 	无
  * Others: 	无
  */
void auto_test_repair_ramp(void);
#endif

