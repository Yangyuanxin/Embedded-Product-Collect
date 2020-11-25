/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        command.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-01
 * Description:      
 * Others:      
 * Function List:    
    1. 创建command模块
    2. 销毁command模块
    3. command模块定时处理入口
 * History: 
    1. Date:         2019-03-01
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <gm_type.h>
#include "error_code.h"

#define CMD_MAX_LEN  1024


typedef enum
{
    COMMAND_UART = 0,
    COMMAND_SMS = 1,
    COMMAND_GPRS = 2,
}CommandReceiveFromEnum;

/**
 * Function:   配置指令入口函数
 * Description:配置指令入口函数
 * Input:	   from[in]:输入数据来源CommandReceiveFromEnum
 *             p_cmd[in]: 输入数据指针
 *             src_len[in]: 输入数据长度
               pinfo: 短信原始信息
 * Output:	   p_rsp[out]:输出数据指针
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:
 */
GM_ERRCODE command_on_receive_data(CommandReceiveFromEnum from, char* p_cmd, u16 src_len, char* p_rsp, void * pmsg);


#endif

