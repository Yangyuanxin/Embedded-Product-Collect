/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        uart.h
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-06
 * Description:      串口配置管理、数据读写
 * Others:           1、使用顺序:创建->注册回调函数->打开串口 
 					 2、GM_UART_DEBUG端口接收到的数据发给commandgps模块
 					 3、GM_UART_GPS端口接受到的数据发送给gps模块
 * Function List:    
    1. 创建uart模块
    2. 销毁uart模块
    3. 定时处理入口
    4. 注册回调函数
    5. 打开串口
    6. 关闭串口
    7. 向串口写数据
    
 * History: 
    1. Date:         2019-03-06
       Author:       王志华
       Modification: 创建初始版本
    2. Date: 		 
	   Author:		 
	   Modification: 

 */

#ifndef __UART_H__
#define __UART_H__

#include "gm_type.h"
#include "error_code.h"
#include "gm_uart.h"

typedef enum 
{
	GM_UART_DEBUG = UART_PORT1,
	GM_UART_GPS = UART_PORT2,
	GM_UART_BMS = UART_PORT3,
	GM_UART_MAX = 3
}UARTPort;

#define BAUD_RATE_HIGH 115200
#define BAUD_RATE_LOW 9600

/**
 * Function:   创建uart模块
 * Description:创建uart模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE uart_create(void);

/**
 * Function:   销毁uart模块
 * Description:销毁uart模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_destroy(void);

/**
 * Function:   uart模块定时处理入口
 * Description:uart模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_timer_proc(void);


/**
 * Function:   打开串口
 * Description:波特率只有特定的几个值（9600、115200等）
 * Input:	   port:哪个串口；baud_rate:波特率;no_data_to_reopen_time:超过时间收不到数据重新打开串口,如果为0则检查
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_open_port(const UARTPort port, const U32 baud_rate, const U16 no_data_to_reopen_time);

/**
 * Function:   关闭串口
 * Description:波特率只有特定的几个值（9600、115200等）
 * Input:	   port:哪个串口
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_close_port(const UARTPort port);


/**
 * Function:   向串口写数据
 * Description:同步操作
 * Input:	   port:哪个串口；p_data:数据指针；len——数据长度
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_write(const UARTPort port, const U8* p_data, const U16 len);

#endif



