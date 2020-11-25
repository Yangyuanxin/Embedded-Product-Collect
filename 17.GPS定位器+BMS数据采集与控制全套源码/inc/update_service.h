/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        update_service.h
 * Author:           梁震       
 * Version:          1.0
 * Date:             2019-02-28
 * Description:      模块升级服务
 * Others:      
 * Function List:    
    1. 创建
    2. 销毁 
    3. 
 * History: 
    1. Date:         2019-03-01         
       Author:       梁震
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __UPDATE_SERVICE_H__
#define __UPDATE_SERVICE_H__

#include "gm_type.h"
#include "error_code.h"
#include "socket.h"

/**
 * Function:   创建update_service模块
 * Description:创建update_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE update_service_create(bool first_create);

/**
 * Function:   销毁update_service模块
 * Description:销毁update_service模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE update_service_destroy(void);

/**
 * Function:   update_service模块定时处理入口
 * Description:update_service模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE update_service_timer_proc(void);


/**
 * Function:   获取update_service模块状态
 * Description:
 * Input:	   无
 * Output:	   无
 * Return:	   SocketStatus
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
SocketStatus update_service_get_status(void);


void update_service_connection_failed(void);
void update_service_connection_ok(void);
void update_service_finish(u32 wait);
void update_service_result_to_server(void);

void update_service_after_response(bool newsocket, u32 total_len);
void update_service_after_blocks_finish(void);

/**
 * Function:   确定升级模块是否处于等待重启， 需要配合update_service_get_status使用
 * Description: 用于无线模块发现升级了，不进入休眠，立即重启，以得到升级后的执行效果（否则要等下次唤醒，可能是一周甚至一月以后）
 * Input:	   无
 * Output:	   无
 * Return:	   true:升级完成 false:不需要升级，或升级失败
 * Others:	   只有update_service_get_status 得到SOCKET_STATUS_DATA_FINISH时，才可以用此函数判断
 */
bool update_service_is_waiting_reboot(void);

#endif

