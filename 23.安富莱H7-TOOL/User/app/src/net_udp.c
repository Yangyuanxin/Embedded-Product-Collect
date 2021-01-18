/*
*********************************************************************************************************
*
*    模块名称 : UDP搜索模块
*    文件名称 : net_udp.c
*    版    本 : V1.0
*    说    明 : 使用UDP广播，搜索局域网内的设备，并配置IP等参数
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2016-11-22  armfly  正式发布
*
*    Copyright (C), 2016-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "udp.h"
#include "net_udp.h"
#include "param.h"
#include "local_reg_addr.h"
#include "tcp_modbus_slave.h"

#define MODS_ADDR 0x01 /* MODBSU 从站地址 */

#define UDP_TX_SIZE (256 + 8)

struct udp_pcb *g_udp_pcb;
struct pbuf *p_udp_tx;

static uint8_t udp_tx_buf[UDP_TX_SIZE];
static uint16_t udp_tx_len;

static void udp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p_rx, struct ip_addr *addr, u16_t port);

/*
*********************************************************************************************************
*    函 数 名: UDP_server_init
*    功能说明: 创建UDP服务器，端口号固定 17262
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void UDP_server_init(void)
{
    g_udp_pcb = udp_new(); //申请udp控制块
    //udp_bind(pcb,IP_ADDR_ANY,UDP_LOCAL_PORT);

    p_udp_tx = pbuf_alloc(PBUF_RAW, sizeof(udp_tx_buf), PBUF_RAM); // 按照指定类型分配一个pbuf结构体  // struct pbuf *p_tx;
    p_udp_tx->payload = (void *)udp_tx_buf;

    //g_udp_pcb->so_options |= SOF_BROADCAST;
    udp_bind(g_udp_pcb, IP_ADDR_ANY, 30010);        /* 绑定本地IP地址和端口号（作为udp服务器） */
    udp_recv(g_udp_pcb, udp_server_recv, NULL); /* 设置UDP段到时的回调函数 */
}

/*
*********************************************************************************************************
*    函 数 名: udp_server_recv
*    功能说明: 接收到UDP数据包的回调函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void udp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p_rx, struct ip_addr *addr, u16_t port)
{
    struct ip_addr destAddr = *addr; /* 获取远程主机 IP地址 */

    //pcb->local_ip;
    IP4_ADDR(&destAddr, 255, 255, 255, 255); //设置网络接口的ip地址
    if (p_rx != NULL)
    {
        /* 分析UDP数据包 */
        {
            const uint8_t mac_ff[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            uint16_t i;

            g_tModbus.TCP_Flag = 0;
            if (p_rx->len >= 8)
            {
                /* 6字节MAC地址， 0xFFFFFF 是广播MAC. */
                if (memcmp(p_rx->payload, mac_ff, 6) == 0 || memcmp(p_rx->payload, &g_tVar.MACaddr, 6) == 0)
                {
                    /* 带MAC前缀的udp数据包 */
                    MODBUS_Poll((uint8_t *)p_rx->payload + 6, p_rx->len - 6);

                    for (i = 0; i < g_tModbus.TxCount; i++)
                    {
                        g_tModbus.TxBuf[g_tModbus.TxCount - i - 1 + 6] = g_tModbus.TxBuf[g_tModbus.TxCount - i - 1];
                    }

                    //MODS_Analyze((uint8_t *)p_rx->payload + 6, p_rx->len - 6, &udp_tx_buf[6], &udp_tx_len);    /* 分析MODBUS数据帧 */
                    if (g_tModbus.TxCount > 0)
                    {
                        memcpy(g_tModbus.TxBuf, &g_tVar.MACaddr, 6); /* 本机MAC放到应答数据包前缀 */
                        g_tModbus.TxCount += 6;
                    }
                }
                else /* 不带MAC前缀 */
                {
                    MODBUS_Poll(p_rx->payload, p_rx->len); /* 分析MODBUS数据帧 */
                }
            }
        }

        if (g_tModbus.TxCount > 0)
        {
            /* 准备应答数据 */
            p_udp_tx->payload = (void *)g_tModbus.TxBuf;
            p_udp_tx->len = g_tModbus.TxCount;
            p_udp_tx->tot_len = g_tModbus.TxCount;

            udp_sendto(pcb, p_udp_tx, &destAddr, port); /* 将收到的数据再发送出去 */
        }

        pbuf_free(p_rx); /* 释放该UDP段 */
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
