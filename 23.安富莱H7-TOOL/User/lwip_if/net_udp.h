/*
*********************************************************************************************************
*
*    模块名称 : UDP搜索模块
*    文件名称 : net_udp.h
*
*********************************************************************************************************
*/
#include "bsp.h"

#ifndef _NET_UDP_H
#define _NET_UDP_H

#define UDP_TX_SIZE (1500 + 8)

extern uint8_t udp_tx_buf[];
extern uint16_t udp_tx_len;

void udp_server_init(void);

void lua_udp_SendBuf(uint8_t *_buf, uint16_t _len, uint16_t _port);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
