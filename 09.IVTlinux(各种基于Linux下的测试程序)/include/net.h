#ifndef _NET_H
#define _NET_H

#include <netinet/in.h>
#include <netinet/tcp.h> /* TCP_NODELAY */
#include <unistd.h>		/* for convenience */
#include <limits.h>
#include <time.h>
#include <pwd.h>
#include <signal.h>		/* for SIG_ERR */
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>   /* abs() */
#include <string.h>	/* for convenience */
#include <stddef.h>		/* for offsetof */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>	/* for domain socket*/
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/termios.h>
#include <sys/ioctl.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <net/if.h>

#include <assert.h>
#include <signal.h>
#include <netdb.h>
#include <net/if_arp.h> 
#include <asm/types.h>
#include <netinet/ether.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include "glabtypes.h"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

//ifname一般为"eth0", ip_addr, gw等ip地址为"xxx.xxx.xxx.xxx", mac为6字节的数组
int net_del_default_gateway(char* ifname);
int net_set_default_gateway(char* ifname, char* gw);
int net_get_mac(char *ifname, unsigned char *mac);
int net_set_mac(char *ifname, unsigned char *mac);
int net_set_ipv4_addr(char *ifname,char *ip_addr,char *mask_addr);
int net_get_ipv4_addr(char *ifname,char *ip_addr,char *mask_addr, char* broad_addr);

#endif

