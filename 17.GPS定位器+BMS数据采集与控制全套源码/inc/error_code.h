/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        error_code.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-02-28
 * Description:      各模块公共错误码定义
 * Others:           成功为0,未知错误码为-1,其它已知错误码均为负整数
 * Function List:    
 * History: 
    1. Date:         2019-02-28
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

typedef enum 
{
	GM_SUCCESS = 0,             //成功
	GM_UNKNOWN = -1,            //未知错误
	GM_NOT_INIT = -2,           //未初始化
	GM_MEM_NOT_ENOUGH = -3,     //内存不足
	GM_PARAM_ERROR = -4,        //参数错误
	GM_EMPTY_BUF = -5,          //没有数据
    GM_ERROR_STATUS = -6,       //状态错误
    GM_HARD_WARE_ERROR = -7,    //硬件错误
    GM_SYSTEM_ERROR = -8,       //系统错误
    GM_WILL_DELAY_EXEC = -9,    //延迟执行
    GM_NET_ERROR = -10,         //网络错误 
}GM_ERRCODE;

#endif

