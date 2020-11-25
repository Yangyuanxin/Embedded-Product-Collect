/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        log_service.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建log_service模块
    2. 销毁log_service模块
    3. log_service模块定时处理入口
    4. 打印日志到串口
    5. 上传日志到日志后台
 * History: 
    1. Date:         2019-03-01
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __LOG_SERVICE_H__
#define __LOG_SERVICE_H__

#include "gm_type.h"
#include "error_code.h"
#include "json.h"
#include "socket.h"
#include "stdio.h"

#define LOG_BUFFER_NUM 100

typedef enum 
{
	DEBUG = 0,
	INFO = 1,
	WARN = 2,
	ERROR = 3,
	FATAL= 4
}LogLevel;

#define LOG log_service_print
#define LOG_HEX log_service_print_hex
#define LOG_TEST log_service_print_test


#define TRACE log_service_print(DEBUG, "%s(%d):%s()", __FILE__, __LINE__,__FUNCTION__);

/**
 * Function:   创建log_service模块
 * Description:创建log_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用，否则调用其它接口返回失败错误码
 */
GM_ERRCODE log_service_create(void);

/**
 * Function:   销毁log_service模块
 * Description:销毁log_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE log_service_destroy(void);

/**
 * Function:   log_service模块定时处理入口
 * Description:log_service模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE log_service_timer_proc(void);

/**
 * Function:   设置日志级别
 * Description:
 * Input:	   log_level：日志级别
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
void log_service_set_level(LogLevel log_level, LogLevel upload_level);

/**
 * Function:   获取日志级别
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   log_level：日志级别
 * Others:	   无
 */
void log_service_get_level(LogLevel *log_level, LogLevel *upload_level);

/**
 * Function:   设置日志开关
 * Description:
 * Input:	   enable：true——打开;false——关闭
 * Output:	   无
 * Return:	   无
 * Others:	   无
 */
void log_service_enable_print(bool enable);



/*由socket层回调*/
void log_service_connection_failed(void);
void log_service_connection_ok(void);
void log_service_close_for_reconnect(void);


/**
 * Function:   打印日志到串口
 * Description:
 * Input:	   level:日志界别；format:格式化字符串；...:可变参数
 * Output:	   无
 * Return:	   无
 * Others:	   1秒钟处理一次
 */
void log_service_print(LogLevel level,const char *format, ...);

/**
 * Function:   打印自动化测试日志到串口
 * Description:
 * Input:	   level:日志界别；format:格式化字符串；...:可变参数
 * Output:	   无
 * Return:	   无
 * Others:	   1秒钟处理一次
 */
void log_service_print_test(const char *format, ...);


/**
 * Function:   打印16进制数据到串口
 * Description:
 * Input:	   p_data:数据指针；len:数据长度
 * Output:	   无
 * Return:	   无
 * Others:	   1秒钟处理一次
 */
void log_service_print_hex(const char* p_data, const U16 len);


/**
 * Function:   上传日志到日志后台
 * Description:异步操作，先内存缓存，然后定时写文件和上传日志后台，函数内部会添加公共的头
 * Input:	   level:日志界别；p_root——指向json对象的指针
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   p_root由被调函数（本函数）释放
 */
void log_service_upload(LogLevel level,JsonObject* p_root);

#endif

