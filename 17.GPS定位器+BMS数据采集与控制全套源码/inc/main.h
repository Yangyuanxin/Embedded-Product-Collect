/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        main.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-02-28
 * Description:      APP主函数头
 * Others:      
 * Function List:    
    1. APP主入口
 * History: 
    1. Date:         2019-02-28
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __GOOME_MAIN_H__
#define __GOOME_MAIN_H__


/**
 * Function:   主函数入口
 * Description:唯一的主函数入口
 * Input:      无
 * Output:     无
 * Return:     无
 * Others:     无
 */
extern void app_main_entry(void);

/**
 * Function:   普通10ms定时器
 * Description:工作状态时有效,由gsensor唤醒时开启
 * Input:      无
 * Output:     无
 * Return:     无
 * Others:     无
 */
void timer_10ms_proc(void);

/**
 * Function:   普通1秒定时器
 * Description:工作状态时有效,由gsensor唤醒时开启
 * Input:      无
 * Output:     无
 * Return:     无
 * Others:     无
 */
void timer_1s_proc(void);

/**
 * Function:   内核1秒定时器
 * Description:休眠时有效
 * Input:      无
 * Output:     无
 * Return:     无
 * Others:     无
 */
void kal_timer_1s_proc(void* p_arg);


#endif

