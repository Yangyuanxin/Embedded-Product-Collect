#ifndef __UDP_H_
#define __UDP_H_

#include"uip-conf.h"

#if UIP_CONF_UDP

void udp_appcall(void);
void udp_Init(void);

#define UIP_UDP_APPCALL  udp_appcall //UDP的用户接口函数

#endif

#endif
