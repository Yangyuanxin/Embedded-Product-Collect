/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        relay.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建relay模块
    2. 销毁relay模块
    3. relay模块定时处理入口
    4. 断油电
    5. 恢复油电
 * History: 
    1. Date:         2019-03-01
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __RELAY_H__
#define __RELAY_H__

#include "gm_type.h"
#include "error_code.h"

/**
 * Function:   创建relay模块
 * Description:创建relay模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE relay_create(void);

/**
 * Function:   销毁relay模块
 * Description:销毁relay模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE relay_destroy(void);

/**
 * Function:   relay模块定时处理入口
 * Description:不满足断油电条件的,定时检查条件,条件满足时执行
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   1秒钟调用1次
 */
GM_ERRCODE relay_timer_proc(void);

/**
 * Function:   断油电
 * Description:接常闭继电器,打开继电器=断油电,不满足执行条件时会在条件满足时执行最后一条指令
 * Input:	   check_speed:是否检查速度
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE relay_on (bool check_speed);

/**
 * Function:   恢复油电
 * Description:接常闭继电器,关闭继电器=恢复油电,不满足执行条件时会在条件满足时执行最后一条指令
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE relay_off (void);

#endif

