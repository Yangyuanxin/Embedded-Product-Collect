/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        watch_dog.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-02-28
 * Description:      实现软件看门狗功能
 * Others:      
 * Function List:    
    1. 创建
    2. 销毁 
    3. 定时处理入口
    4. 开启/关闭软件看门狗
 * History:  
    1. Date:         2019-02-28    
       Author:       王志华 
       Modification  创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __WATCH_DOG_H__
#define __WATCH_DOG_H__

#include "gm_type.h"
#include "error_code.h"


/**
 * Function:   创建watch_dog模块
 * Description:创建watch_dog模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE watch_dog_create(void);

/**
 * Function:   销毁watch_dog模块
 * Description:销毁watch_dog模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE watch_dog_destroy(void);

/**
 * Function:   watch_dog模块定时处理入口
 * Description:watch_dog模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE watch_dog_timer_proc(void);

/**
 * Function:   开启/关闭软件看门狗
 * Description:默认开启
 * Input:	   enable:true——开启;false——关闭
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE watch_dog_enable(bool enable);

/**
 * Function:   停止喂硬件看门狗，达到硬重启的目的
 * Description:通过指令下发
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE watch_dog_hard_reboot(void);

#endif


