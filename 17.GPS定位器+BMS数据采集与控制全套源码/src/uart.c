/**
 * Copyright @ 深圳市谷米万物科技有限公司. 2009-2019. All rights reserved.
 * File name:        uart.c
 * Author:           王志华       
 * Version:          1.0
 * Date:             2019-03-06
 * Description:      串口配置管理、数据读写
 * Others:           接收到数据回调接收数据的模块,使用顺序:创建->注册回调函数->打开串口
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


#include <time.h>
#include <gm_stdlib.h>
#include <gm_memory.h>
#include <gm_gpio.h>
#include <gm_callback.h>
#include "uart.h"
#include "fifo.h"
#include "log_service.h"
#include "utility.h"
#include "nmea_protocol.h"
#include "gps.h"
#include "command.h"
#include "auto_test.h"
#include "bms.h"

//每次串口接收数据内存大小
#define GM_UART_RCV_BUFF_LEN 1024

//串口缓冲区内存大小
#define GM_UART_RCV_FIFO_LEN 2048

typedef struct
{
	U32 baud_rate;
	U16 no_data_to_reopen_time;
	time_t last_rcv_time;
	bool is_open;
	char rcv_buff[GM_UART_RCV_BUFF_LEN];
	FifoType rcv_fifo;
}UARTPara;

typedef struct
{
	bool inited;
	UARTPara UARTParas[GM_UART_MAX];
}UART,*PUART;


static UART g_uart;

static void debug_port_on_receive(void* msg);

static void gps_port_on_receive(void* msg);

static void bms_port_on_receive(void* msg);



/**
 * Function:   创建uart模块
 * Description:创建uart模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   使用前必须调用,否则调用其它接口返回失败错误码
 */
GM_ERRCODE uart_create(void)
{
	U8 index = 0;
	g_uart.inited = true;
	
	for (index = GM_UART_DEBUG; index < GM_UART_MAX; ++index)
	{	
		g_uart.UARTParas[index].baud_rate = BAUD_RATE_HIGH;
		g_uart.UARTParas[index].no_data_to_reopen_time = 0;
		g_uart.UARTParas[index].last_rcv_time = 0;
		g_uart.UARTParas[index].is_open = false;
		fifo_init(&g_uart.UARTParas[index].rcv_fifo,GM_UART_RCV_FIFO_LEN);
	}
	
	
	GM_RegisterCallBack(GM_CB_UART1_RECEIVE, (U32)debug_port_on_receive);
	GM_RegisterCallBack(GM_CB_UART2_RECEIVE, (U32)gps_port_on_receive);
	GM_RegisterCallBack(GM_CB_UART3_RECEIVE, (U32)bms_port_on_receive);
	
	g_uart.inited = true;

	uart_open_port(GM_UART_DEBUG,BAUD_RATE_HIGH,0);

	return GM_SUCCESS;
}

/**
 * Function:   销毁uart模块
 * Description:销毁uart模块
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_destroy(void)
{
	U8 index = 0;
	
	g_uart.inited = false;
		
	for (index = GM_UART_DEBUG; index < GM_UART_MAX; ++index)
	{
		g_uart.UARTParas[index].baud_rate = 115200;
		g_uart.UARTParas[index].last_rcv_time = 0;
		g_uart.UARTParas[index].is_open = false;
		fifo_delete(&g_uart.UARTParas[index].rcv_fifo);
		uart_close_port((UARTPort)index);
	}
	return GM_SUCCESS;
}

/**
 * Function:   uart模块定时处理入口
 * Description:uart模块定时处理入口
 * Input:	   无
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_timer_proc(void)
{
	GM_ERRCODE ret = GM_SUCCESS;
	U8 index = 0;
	for (index = GM_UART_DEBUG; index < GM_UART_MAX; index++)
	{
		//检查GPS端口是否长时间收不到数据
		if (false == g_uart.UARTParas[index].is_open || 0 == g_uart.UARTParas[index].no_data_to_reopen_time)
		{
			//LOG(DEBUG,"port:%d,reopentime:%d",index,g_uart.UARTParas[index].no_data_to_reopen_time);
			continue;
		}
		
		//在规定的时间内未收到数据重新打开串口
		if(util_clock() - g_uart.UARTParas[index].last_rcv_time >= g_uart.UARTParas[index].no_data_to_reopen_time)
		{
			LOG(WARN,"Reopen UART:%d",index);
		    uart_close_port((UARTPort)index);
			ret = uart_open_port((UARTPort)index, g_uart.UARTParas[index].baud_rate,g_uart.UARTParas[index].no_data_to_reopen_time);
			if (GM_SUCCESS != ret)
			{	
				LOG(ERROR,"Failed to open UART %d",index);
				continue;
			}
		}
		else
		{
			//LOG(DEBUG,"UART %d,receive time=%d",index,g_uart.UARTParas[index].last_rcv_time);
		}
	}
	return GM_SUCCESS;
}


/**
 * Function:   打开串口
 * Description:波特率只有特定的几个值（9600、115200等）
 * Input:	   port:哪个串口；baud_rate:波特率
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_open_port(const UARTPort port, const U32 baud_rate, const U16 no_data_to_reopen_time)
{
	S32 ret = 0;
	if (false == g_uart.inited)
	{
		return GM_NOT_INIT;
	}
	
	if (g_uart.UARTParas[port].is_open)
	{
		return GM_SUCCESS;
	}
	
	g_uart.UARTParas[port].baud_rate = baud_rate;
	ret = GM_UartOpen((Enum_SerialPort)port, baud_rate);

	if (1 == ret)
	{
	    g_uart.UARTParas[port].baud_rate = baud_rate;
		g_uart.UARTParas[port].no_data_to_reopen_time = no_data_to_reopen_time;
		g_uart.UARTParas[port].last_rcv_time = util_clock();
		g_uart.UARTParas[port].is_open = true;
		return GM_SUCCESS;
	}
	else
	{
		return GM_HARD_WARE_ERROR;
	}	
}

/**
 * Function:   关闭串口
 * Description:波特率只有特定的几个值（9600、115200等）
 * Input:	   port:哪个串口
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_close_port(const UARTPort port)
{
	if (false == g_uart.inited)
	{
		return GM_NOT_INIT;
	}
	
	if(false == g_uart.UARTParas[port].is_open)
	{
		return GM_NOT_INIT;
	}

	//关闭串口前要清理缓存
    GM_UartClrRxBuffer((Enum_SerialPort)port);
	GM_UartClrTxBuffer((Enum_SerialPort)port);
	
	GM_UartClose((Enum_SerialPort)port);
	
	g_uart.UARTParas[port].last_rcv_time = 0;
	g_uart.UARTParas[port].is_open = false;

	fifo_reset(&g_uart.UARTParas[port].rcv_fifo);

	if (GM_UART_DEBUG == port)
	{
		GM_GpioInit(GM_GPIO10, PINDIRECTION_OUT, PINLEVEL_LOW, PINPULLSEL_DISABLE);
		GM_GpioInit(GM_GPIO11, PINDIRECTION_OUT, PINLEVEL_LOW, PINPULLSEL_DISABLE);
	}
	if (GM_UART_GPS == port)
	{
		GM_GpioInit(GM_GPIO12, PINDIRECTION_IN, PINLEVEL_LOW, PINPULLSEL_DISABLE);
		GM_GpioInit(GM_GPIO17, PINDIRECTION_IN, PINLEVEL_LOW, PINPULLSEL_DISABLE);
	}
	if (GM_UART_BMS == port)
	{
		GM_GpioInit(GM_GPIO0, PINDIRECTION_IN, PINLEVEL_LOW, PINPULLSEL_DISABLE);
		GM_GpioInit(GM_GPIO1, PINDIRECTION_IN, PINLEVEL_LOW, PINPULLSEL_DISABLE);
	}
	return GM_SUCCESS;
}


/**
 * Function:   向串口写数据
 * Description:同步操作
 * Input:	   port:哪个串口；p_data:数据指针；len——数据长度
 * Output:	   无
 * Return:	   GM_SUCCESS——成功；其它错误码——失败
 * Others:	   
 */
GM_ERRCODE uart_write(const UARTPort port, const U8* p_data, const U16 len)
{
	s32 ret = -1;
	if (false == g_uart.inited)
	{
		return GM_NOT_INIT;
	}

	if (!g_uart.UARTParas[port].is_open)
	{
		GM_UartWrite((Enum_SerialPort)port, (U8*)"error len", GM_strlen("error len"));
		return GM_NOT_INIT;
	}

	if (len > 10*CMD_MAX_LEN)
	{
		return GM_PARAM_ERROR;
	}
	
	ret = GM_UartWrite((Enum_SerialPort)port, (U8*)p_data, len);
	if (ret <= 0)
	{
		uart_close_port(port);
		uart_open_port(port, g_uart.UARTParas[port].baud_rate, g_uart.UARTParas[port].no_data_to_reopen_time);
		return GM_HARD_WARE_ERROR;
	}
	else
	{
		return GM_SUCCESS;
	}
}

static void debug_port_on_receive(void* msg)
{
	S32 len = 0;
	char cmd_rsp[CMD_MAX_LEN] = {0};
	
	if (false == g_uart.UARTParas[GM_UART_DEBUG].is_open)
	{
		return;
	}
	
	g_uart.UARTParas[GM_UART_DEBUG].last_rcv_time = util_clock();	

	do
	{
		GM_memset(g_uart.UARTParas[GM_UART_DEBUG].rcv_buff, 0, GM_UART_RCV_BUFF_LEN);
		len = GM_UartRead((Enum_SerialPort)GM_UART_DEBUG, (U8*)g_uart.UARTParas[GM_UART_DEBUG].rcv_buff, GM_UART_RCV_BUFF_LEN);
		if (len > 0)
		{     
			command_on_receive_data(COMMAND_UART,g_uart.UARTParas[GM_UART_DEBUG].rcv_buff,len,cmd_rsp,NULL);
			if (GM_SUCCESS != bms_uart_receive(g_uart.UARTParas[GM_UART_DEBUG].rcv_buff,len))
			{
				//加结束符
				GM_snprintf(cmd_rsp + GM_strlen(cmd_rsp), (CMD_MAX_LEN - GM_strlen(cmd_rsp)), "\r\n");
				uart_write(GM_UART_DEBUG, (U8*)cmd_rsp, GM_strlen(cmd_rsp));
			}
		}
		else
		{
			break;
		}
	}while (len >= GM_UART_RCV_BUFF_LEN);
		 
	return;
}


static void gps_port_on_receive(void* msg)
{
	U16 read_len = 0;
	U16 sentence_len = 0;
	GM_ERRCODE ret = GM_SUCCESS;
	U16 head = 0;
	
	g_uart.UARTParas[GM_UART_GPS].last_rcv_time = util_clock();	

	do
	{
		read_len = 0;
		GM_memset(g_uart.UARTParas[GM_UART_GPS].rcv_buff, 0, GM_UART_RCV_BUFF_LEN);
		
		read_len = GM_UartRead((Enum_SerialPort)GM_UART_GPS,(U8*)g_uart.UARTParas[GM_UART_GPS].rcv_buff,GM_UART_RCV_BUFF_LEN - 1);
		if (read_len > 0 && read_len < GM_UART_RCV_BUFF_LEN)
		{
			//先插入到缓存中,因为可能不是一个完整的语句
			fifo_insert(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo, (U8*)g_uart.UARTParas[GM_UART_GPS].rcv_buff, read_len);
			//可能有多个语句
			do
			{
				//先看看前6个字节是什么？如果是TD_AGPS_SENTENCE_HEAD或者AT_APGS_SENTENCE_HEAD,则是二进制包
				GM_memset(g_uart.UARTParas[GM_UART_GPS].rcv_buff, 0, GM_UART_RCV_BUFF_LEN);
				ret = fifo_peek(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo, (U8*)g_uart.UARTParas[GM_UART_GPS].rcv_buff, 6);
				
				if (GM_SUCCESS != ret)
				{
					break;
				}
				else
				{
					U16 head_buff[3] = {0};
					GM_memcpy(head_buff, g_uart.UARTParas[GM_UART_GPS].rcv_buff, 6);
					head = head_buff[0];
					if (head == TD_SENTENCE_HEAD)
					{
						//第0个U16是包头,第1个U16是命令字,第2个是长度,包头+校验 共8个字节
						sentence_len = head_buff[2] + 8; 
						GM_memset(g_uart.UARTParas[GM_UART_GPS].rcv_buff, 0, GM_UART_RCV_BUFF_LEN);
						ret = fifo_peek(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo, (U8*)g_uart.UARTParas[GM_UART_GPS].rcv_buff, sentence_len);
						if (GM_SUCCESS != ret)
						{
							fifo_reset(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo);
							LOG(WARN, "Receive invalid data,reset fifo!,ret=%d", ret);
							break;
						}
						else
						{
							fifo_pop_len(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo, sentence_len);
							LOG(DEBUG,"Receive TD BIN packet:");
							LOG_HEX(g_uart.UARTParas[GM_UART_GPS].rcv_buff,sentence_len);
						}
					}
					else if (head == AT_SENTENCE_HEAD)
					{
						//第0个U16是起始字节,第1个U16是长度,第2个是ClassID和MessageID,,包头+校验(4个字节)共10个字节
						sentence_len = head_buff[1] + 10;
						GM_memset(g_uart.UARTParas[GM_UART_GPS].rcv_buff, 0, GM_UART_RCV_BUFF_LEN);
						ret = fifo_peek(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo, (U8*)g_uart.UARTParas[GM_UART_GPS].rcv_buff, sentence_len);
						if (GM_SUCCESS != ret)
						{
							fifo_reset(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo);
							LOG(WARN, "Receive invalid data,reset fifo!,ret=%d", ret);
							break;
						}
						else
						{
							fifo_pop_len(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo, sentence_len);	
							LOG(DEBUG,"Receive AT BIN packet:");
							LOG_HEX(g_uart.UARTParas[GM_UART_GPS].rcv_buff,sentence_len);
						}
					}
					//不是二进制包,以'\n'为分隔取一行（一个语句）
					else
					{	
						sentence_len = GM_UART_RCV_BUFF_LEN;
						GM_memset(g_uart.UARTParas[GM_UART_GPS].rcv_buff, 0, GM_UART_RCV_BUFF_LEN);
						ret = fifo_peek_until(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo, (U8*)g_uart.UARTParas[GM_UART_GPS].rcv_buff, &sentence_len, '\n');
						if (GM_SUCCESS != ret)
						{
							LOG(DEBUG,"Failed to peek_until data from FIFO.");
							//fifo_reset(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo);
							LOG_HEX(g_uart.UARTParas[GM_UART_GPS].rcv_buff,sentence_len);
							break;
						}
						else
						{
							LOG(DEBUG,g_uart.UARTParas[GM_UART_GPS].rcv_buff);
							fifo_pop_len(&g_uart.UARTParas[GM_UART_GPS].rcv_fifo, sentence_len);
						}
					}
					
					gps_on_rcv_uart_data((char*)g_uart.UARTParas[GM_UART_GPS].rcv_buff, sentence_len);
				}
			}while (true);	
		}
		else
		{
			break;
		}
	}while (read_len >= (GM_UART_RCV_BUFF_LEN - 1));
	return;
}




static void bms_port_on_receive(void* msg)
{
	S32 len = 0;	
	
	if (false == g_uart.UARTParas[GM_UART_BMS].is_open)
	{
		return;
	}
	
	g_uart.UARTParas[GM_UART_BMS].last_rcv_time = util_clock();	

	do
	{
		GM_memset(g_uart.UARTParas[GM_UART_BMS].rcv_buff, 0, GM_UART_RCV_BUFF_LEN);
		len = GM_UartRead((Enum_SerialPort)GM_UART_BMS, (U8*)g_uart.UARTParas[GM_UART_BMS].rcv_buff, GM_UART_RCV_BUFF_LEN);
		if (len > 0)
		{
			LOG(DEBUG, "clock(%d), bms_port_on_receive len(%d).", util_clock(), len);
			bms_uart_receive(g_uart.UARTParas[GM_UART_BMS].rcv_buff,len);
		}
		else
		{
			break;
		}
	}while (len >= GM_UART_RCV_BUFF_LEN);
		 
	return;
}




