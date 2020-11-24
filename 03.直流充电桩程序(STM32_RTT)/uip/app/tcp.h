#ifndef __TCP_H__
#define __TCP_H__
/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"
#include "psock.h"
#include "stm32f10x.h"	

//通信程序状态字(用户可以自己定义)  
enum
{
	STATE_CMD		= 0,	//命令接收状态 
	STATE_TX_TEST	= 1,	//连续发送数据包状态(速度测试)  
	STATE_RX_TEST	= 2		//连续接收数据包状态(速度测试)  
};	 
//定义 uip_tcp_appstate_t 数据类型，用户可以添加应用程序需要用到
//成员变量。不要更改结构体类型的名字，因为这个类型名会被uip引用。
//uip.h 中定义的 	struct uip_conn  结构体中引用了 uip_tcp_appstate_t		  
struct tcp_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 
typedef struct tcp_appstate uip_tcp_appstate_t;

void tcp_appcall(void);
void tcp_client_appcall(void);
void tcp_server_appcall(void);

//定义应用程序回调函数 
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_appcall //定义回调函数为 tcp_ppcall
#endif
/////////////////////////////////////TCP SERVER/////////////////////////////////////
extern u8 tcp_server_databuf[];   		//发送数据缓存	 
extern u8 tcp_server_sta;				//服务端状态   
//tcp server 函数
void tcp_server_aborted(void);
void tcp_server_timedout(void);
void tcp_server_closed(void);
void tcp_server_connected(void);
void tcp_server_newdata(void);
void tcp_server_acked(void);
void tcp_server_senddata(void);
/////////////////////////////////////TCP CLIENT/////////////////////////////////////
extern u8 tcp_client_databuf[];   		//发送数据缓存	 
extern u8 tcp_client_sta;				//客户端状态   
void tcp_client_reconnect(void);
void tcp_client_connected(void);
void tcp_client_aborted(void);
void tcp_client_timedout(void);
void tcp_client_closed(void);
void tcp_client_acked(void);
void tcp_client_senddata(void);
////////////////////////////////////////////////////////////////////////////////////


#endif
























