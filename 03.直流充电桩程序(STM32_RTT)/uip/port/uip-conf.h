#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned short uip_stats_t;
typedef int uip_udp_appstate_t;

//最大TCP连接数
#define UIP_CONF_MAX_CONNECTIONS 5

//最大TCP端口监听数
#define UIP_CONF_MAX_LISTENPORTS 5

//uIP缓存大小
#define UIP_CONF_BUFFER_SIZE     1520

//CPU大小端模式
//STM32是小端模式的
#define UIP_CONF_BYTE_ORDER  UIP_LITTLE_ENDIAN

//日志开关
#define UIP_CONF_LOGGING         0

//UDP支持开关
#define UIP_CONF_UDP             1

#define UIP_CONF_DHCP            0

//UDP校验和开关
#define UIP_CONF_UDP_CHECKSUMS   1	    


//uIP统计开关 	 
#define UIP_CONF_STATISTICS      0

											 
#include"tcp.h"

#if UIP_CONF_UDP
	#include"udp.h"

	#if UIP_CONF_DHCP
	#include"dhcpc.h"
	#endif
#endif

#endif 























