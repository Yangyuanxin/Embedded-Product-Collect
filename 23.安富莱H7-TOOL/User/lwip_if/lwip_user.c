/*
*********************************************************************************************************
*
*    模块名称 : lwip 协议栈用户接口
*    文件名称 : lwip_user.c
*    版    本 : V1.0
*    说    明 : 提供WEB服务器功能。
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2018-12-05  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "stdio.h"
#include "bsp.h"
#include "num_pad.h"
#include "param.h"

/* for LwIP */
//#include "netconf.h"
//#include "main_lwip.h"
//#include "httpd_w.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dhcp.h"

#include "ethernetif.h"
#include "app_ethernet.h"
#include "http_cgi_ssi.h"

#include "tcp_echoserver.h"
#include "lwip_user.h"
#include "net_udp.h"

static uint8_t s_lwip_status = 0;

struct netif gnetif;

static void Netif_Config(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

#if LWIP_DHCP
    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else
    /* IP address default setting */
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif

    /* add the network interface */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

    /*  Registers the default network interface */
    netif_set_default(&gnetif);

    ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(&gnetif, ethernet_link_status_updated);
#endif
}

/*
*********************************************************************************************************
*    函 数 名: lwip_pro
*    功能说明: lwip 轮询，插入到主循环中
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void lwip_pro(void)
{
    switch (s_lwip_status)
    {       
        case 0:
            lwip_init();    /* Initialize the LwIP stack */
            s_lwip_status++;
            break;

        case 1:
            /* Configure the Network interface */
            Netif_Config();
            s_lwip_status++;
            break;

        case 2:
            /* Http webserver Init */
            http_server_init();

            /* tcp server init */
            tcp_echoserver_init();

            udp_server_init();      /* 开启UDP监听 */
        
            s_lwip_status = 10;
            break;        

        case 10:    
            /* Read a received packet from the Ethernet buffers and send it 
               to the lwIP for handling */
            ethernetif_input(&gnetif);

            /* Handle timeouts */
            sys_check_timeouts();

        #if LWIP_NETIF_LINK_CALLBACK
            Ethernet_Link_Periodic_Handle(&gnetif);
        #endif

        #if LWIP_DHCP
            DHCP_Periodic_Handle(&gnetif);
        #endif            
            break;        
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
