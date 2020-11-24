#include "stm32f10x.h"		 		   
#include "uip.h"	    
#include "tcp.h"


//TCP应用接口函数(UIP_APPCALL)
//完成TCP服务(包括server和client)
void tcp_appcall(void)
{
//	switch(uip_conn->lport)//本地监听端口80和1200
//	{
//		case HTONS(80):
//			//httpd_appcall();
//			break;
//		case HTONS(1200):
//		    tcp_server_appcall();
//			break;
//		default:
//		    break;
//	}
//	switch(uip_conn->rport)	//远程连接1400端口
//	{
//	    case HTONS(1400):
//			tcp_client_appcall();
//	       break;
//	    default:
//	       break;
//	}
}

























