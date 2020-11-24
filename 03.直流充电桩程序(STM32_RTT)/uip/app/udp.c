#include "uip.h"
#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "server.h"
#include "sys_cfg.h"
#include "string.h"

#if 1
#define UDP_MSG dbg_msg
#define UDP_ERROR dbg_error
#else
#define UDP_MSG (void)
#define UDP_ERROR (void)
#endif

#define UDP_SEND_BUF_LEN 100
u8 udp_send_buf[UDP_SEND_BUF_LEN];
static u16 udp_send_len;
static u8 udp_ready;




void udp_Init()
{
	struct uip_udp_conn* c;
	uip_ipaddr_t ipaddr = {0, 0};

	uip_ipaddr(ipaddr, sys_cfg.rip1, sys_cfg.rip2, sys_cfg.rip3, sys_cfg.rip4);
	c = uip_udp_new(&ipaddr, HTONS(sys_cfg.rport));


	uip_udp_bind(c, HTONS(1024));
	if(c != 0)
	{
		udp_ready = 1;
	}
}

void udp_appcall()
{
#if UIP_CONF_DHCP
	dhcpc_appcall();
#endif

	switch(uip_udp_conn->lport)
	{
#if UIP_CONF_DHCP
	case HTONS(68):  //dhcp
		//dhcpc_appcall();
			break;
#endif
	}

	if(uip_newdata())
	{
		//uip_flags &= ~2;
		if((uip_udp_conn->rport == HTONS(sys_cfg.rport)) && (uip_udp_conn->lport == HTONS(1024)))
		{
			if(server_handler.recv_callback != NULL)
			{
				server_handler.recv_callback(uip_appdata, uip_len);
			}
		}
	}


	if(uip_poll())  //当定时轮询到达
	{
		if(udp_send_len)  //需要发送数据
		{
			memcpy(uip_appdata, udp_send_buf, udp_send_len);  //拷贝
			uip_udp_send(udp_send_len);  //发送
			udp_send_len = 0;
		}
	}
}



/*
 * @brief  UDP发送数据
 * @param  buf 数据
 * @param  len 数据字节数
 * @retval 0成功  1失败
 */
int udp_send(u8 *buf, u16 len)
{
	if(!udp_ready)
	{
		return 1;
	}

	if(len <= UDP_SEND_BUF_LEN)
	{
		memcpy(udp_send_buf, buf, len);
		udp_send_len = len;
		return 0;
	}
	else
	{
		return 1;
	}
}


//#if 1
//
////两种方法计算结果是一致的  STM32用硬件算法   软件算法用于其它平台校验使用
//
///*
// * @brief  STM32内部硬件CRC32计算   注意打开CRC时钟！
// * @param  buf 数据
// * @param  len 数据字节数
// * @retval 校验结果
// */
//u32 crc32(u8 *buf, u16 len)
//{
//	u16 index;
//	CRC_ResetDR();        //复位CRC
//	for(index = 0; index < len; index++)
//	{
//		CRC->DR = (u32)buf[index];
//	}
//	return (CRC->DR);
//}
//#else
///*
// * @brief  软件CRC32计算
// * @param  buf 数据
// * @param  len 数据字节数
// * @retval 校验结果
// */
//u32 crc32(u8 *buf, u16 len)
//{
//
//#define POLY  0x04c11db7
//
//    int i,j;
//    u32 crc = 0xFFFFFFFF;
//    for (j = 0; j < len; j++)
//    {
//        crc = crc ^ (*buf++);
//        for (i = 0; i < 32; i++)
//        {
//            if (crc & 0x80000000)
//                crc = (crc << 1) ^ POLY;
//            else
//                crc <<= 1;
//        }
//    }
//    return(crc);
//}
//#endif
























