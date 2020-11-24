#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"
#include "stm32f10x.h"
#include"udp.h"
#include"dbg.h"
#include "sys_cfg.h"


#if 1
#define ETH_MSG dbg_msg
#else
#define ETH_MSG (void)
#endif

#if 1
#define ETH_ERROR dbg_error
#else
#define ETH_ERROR (void)
#endif


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

//uIP事件处理
//处理收到的网络数据包
void uip_polling(void);

static struct timer arp_timer, link_timer;


#define Device_ID1  (*(u32*)(0x1FFFF7E8))
#define Device_ID2  (*(u32*)(0x1FFFF7EC))
#define Device_ID3  (*(u32*)(0x1FFFF7F0))


/*
 * @brief           写寄存器(功能码0x10)
 * @retval 0 连接正常   < 0 失败
 */
int link_ok = -1;
int eth_link_isok()
{
	return link_ok;
}



//uip事件处理函数
void uIPMain()
{
	//设置MAC地址
	uip_ethaddr.addr[0] = 0x88;  //必须为偶数
	uip_ethaddr.addr[1] = (u8)(Device_ID1);
	uip_ethaddr.addr[2] = (u8)(Device_ID1 >> 16);
	uip_ethaddr.addr[3] = (u8)(Device_ID2);
	uip_ethaddr.addr[4] = (u8)(Device_ID2 >> 16);
	uip_ethaddr.addr[5] = (u8)(Device_ID3);

eth_restart:

	tapdev_init();  //STM32 ETH 初始化

    //初始化uIP时钟 10ms中断
	uIP_ClockInit();

	uip_init();  //uIP初始化

#if UIP_CONF_DHCP  //自动获取IP
	dhcpc_init((const void*)&uip_ethaddr, 6);
#else
	{
		uip_ipaddr_t ipaddr;

		//设置本地设置IP地址
		uip_ipaddr(ipaddr, sys_cfg.lip1, sys_cfg.lip2, sys_cfg.lip3, sys_cfg.lip4);
		uip_sethostaddr(ipaddr);
		//设置网关IP地址
		uip_ipaddr(ipaddr, sys_cfg.drip1, sys_cfg.drip2, sys_cfg.drip3, sys_cfg.drip4);
		uip_setdraddr(ipaddr);
		//设置网络掩码
		uip_ipaddr(ipaddr, sys_cfg.netmask1, sys_cfg.netmask2, sys_cfg.netmask3, sys_cfg.netmask4);
		uip_setnetmask(ipaddr);
	}
#endif

	//uip_listen(HTONS(1200));	 //监听1200端口,用于TCP Server
	//uip_listen(HTONS(80));	 //监听80端口,用于Web Server
    //tcp_client_reconnect();	 //尝试连接到TCP Server端,用于TCP Client

	udp_Init();


	//创建1个10秒的定时器
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	timer_set(&link_timer, CLOCK_SECOND * 3);

	while(1)
	{
		uip_polling();

//		if(tcp_server_sta & (1 << 6))  //收到客户端数据
//		{
//			//tcp_server_sta &= ~(1 << 6);
//		}
//
//		if(tcp_client_sta & (1 << 6))  //收到服务端数据
//		{
//			//tcp_client_sta &= ~(1 << 6);
//		}

		//每3秒检查一次网线连接状态
		if(timer_expired(&link_timer))
		{
			static u8 link_off_cnt;

			timer_reset(&link_timer);
			if(!tapdev_link_up())  //网线断开
			{
				link_off_cnt++;
				if(link_off_cnt == 3)  //连续三次检测到
				{
					link_ok = -1;
					ETH_ERROR("eth link down restart now\n");
					goto eth_restart;
				}
			}
			else
			{
				link_ok = 0;
				link_off_cnt = 0;
			}
		}
	}
}

//uIP事件处理
//处理收到的网络数据包
void uip_polling()
{
	u8 i;

	uip_len = tapdev_read();	//从网络设备读取一个IP包,得到数据长度uip_len在uip.c中定义
	if(uip_len > 0) 			//有数据
	{
		//处理IP数据包(只有校验通过的IP包才会被接收)
		if(BUF->type == htons(UIP_ETHTYPE_IP))//是否是IP包?
		{
			uip_arp_ipin();	//去除以太网头结构，更新ARP表
			uip_input();   	//IP包处理
			//当上面的函数执行后，如果需要发送数据，则全局变量 uip_len > 0
			//需要发送的数据在uip_buf, 长度是uip_len  (这是2个全局变量)
			if(uip_len > 0)//需要回应数据
			{
				uip_arp_out();  //加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();  //发送数据到以太网
			}

			//ETH_MSG("eth rx ip\n");
		}
		else if(BUF->type == htons(UIP_ETHTYPE_ARP))  //处理arp报文,是否是ARP请求包?
		{
			uip_arp_arpin();
			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len(这是2个全局变量)
			if(uip_len > 0)
			{
				tapdev_send();  //需要发送数据,则通过tapdev_send发送
			}

			//ETH_MSG("eth rx arp\n");
		}
		else
		{
			//ETH_MSG("eth rx other\n");
		}
	}

	//每隔10秒调用1次ARP定时器函数 用于定期ARP处理,ARP表10秒更新一次，旧的条目会被抛弃
	else if(timer_expired(&arp_timer))  //10秒定时器超时
	{
		timer_reset(&arp_timer);
		uip_arp_timer();
	}
	else
	{

#if UIP_CONF_TCP
		//轮流处理每个TCP连接, UIP_CONNS缺省是40个
		for(i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);	//处理TCP通信事件
			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len>0)
			{
				uip_arp_out();  //加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();  //发送数据到以太网
			}
		}
#endif

		#if UIP_UDP	//UIP_UDP
		//轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个
		for(i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i);	//处理UDP通信事件
			//当上面的函数执行后，如果需要发送数据，则全局变量uip_len>0
			//需要发送的数据在uip_buf, 长度是uip_len (这是2个全局变量)
			if(uip_len > 0)
			{
				uip_arp_out();//加以太网头结构，在主动连接时可能要构造ARP请求
				tapdev_send();//发送数据到以太网
			}
		}
		#endif
	}
}
