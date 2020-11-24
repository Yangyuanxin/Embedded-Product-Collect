#include "net.h"
#include "utils.h"

#define NET_ERR_CMD_OPEN -101
#define NET_ERR_CMD_RETURN -102
#define NET_ERR_CMD_TIMEOUT -103
#define NET_ERR_NO_DHCPC -104
#define NET_ERR_KILL_DHCPC -105
#define NET_ERR_ROUTE_DEL -106
#define NET_ERR_ROUTE_ADD -107

#define ROUTE_DEL_TIMEOUT 10000
#define ROUTE_ADD_TIMEOUT 5000
#define net_dbg(format, arg...) 	do {  printf(  "%s: " format "\n",__FUNCTION__, ## arg); } while (0)

int net_del_default_gateway(char* ifname)
{
	char cmd[256];
	char string[1024] = {0};

	sprintf(cmd, "/bin/busybox route del default gw 0.0.0.0 dev %s", ifname);	
	return(pf_exec_simple_cmd(cmd, string, sizeof(string)));
}

int net_set_default_gateway(char* ifname, char* gw)
{
	int ret;
	char cmd[512] = {0};
	char string[1024] = {0};
	
	memset(cmd,0,sizeof(cmd));
	//sprintf(cmd, "/bin/busybox route del default gw 0.0.0.0 dev %s", ifname);	
	sprintf(cmd, "/bin/busybox route del default gw  0.0.0.0 dev %s", ifname);	
	ret = pf_exec_simple_cmd(cmd, string, sizeof(string));
	if(ret)
		return ret;

	sprintf(cmd, "route add default gw %s dev %s", gw, ifname);
	printf("%s \n",cmd);
	ret = pf_exec_simple_cmd(cmd, string, sizeof(string));
	if(ret)
		return NET_ERR_ROUTE_ADD;
	
	return 0;
}

int net_get_mac(char *ifname, unsigned char *mac)   
{   
	int sock, ret;   
	struct ifreq ifr;   

	sock = socket(AF_INET, SOCK_STREAM, 0);   
	if(sock < 0)
	{   
		perror("socket");   
		return -1;   
	}   

	memset(&ifr, 0, sizeof(ifr));   
	strcpy(ifr.ifr_name, ifname);   

	ret = ioctl(sock, SIOCGIFHWADDR, &ifr, sizeof(ifr));   
	if(ret)
	{
		perror("SIOCGIFHWADDR");
		goto l_err1;
	}

//	DEBUG("family = %d, AF_INET = %d\n", ifr.ifr_hwaddr.sa_family, AF_INET);
	
	memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);   

	l_err1:
	if(sock>0)
		close(sock);   
	return ret;   
}   

int net_set_mac(char *ifname, unsigned char *mac)   
{   
	int sock, ret=-1;   
	struct ifreq ifr;   

	sock = socket(AF_INET, SOCK_STREAM, 0);   
	if(sock < 0)
	{   
		perror("socket");   
		return -1;   
	}   

	memset(&ifr, 0, sizeof(ifr));   
	strcpy(ifr.ifr_name, ifname);   

	if(ioctl(sock, SIOCGIFFLAGS, &ifr))
	{
		perror("SIOCGIFFLAGS");
		goto l_err2;
	}

	strcpy(ifr.ifr_name, ifname);   
	ifr.ifr_flags &= ~IFF_UP;
	if(ioctl(sock, SIOCSIFFLAGS, &ifr))
	{
		perror("if down");
		goto l_err2;
	}

	strcpy(ifr.ifr_name, ifname);   
	ifr.ifr_hwaddr.sa_family = AF_UNIX;
	memcpy(ifr.ifr_hwaddr.sa_data, mac, 6);   

	ret = ioctl(sock, SIOCSIFHWADDR, &ifr, sizeof(ifr));   
	if(ret)
	{
		perror("SIOCSIFHWADDR");
		goto l_err2;
	}

	strcpy(ifr.ifr_name, ifname);   
	ifr.ifr_flags |= IFF_UP;
	if(ioctl(sock, SIOCSIFFLAGS, &ifr))
	{
		perror("if up");
		goto l_err2;
	}
	
	l_err2:
	if(sock>0)
		close(sock);   
	return ret;   
} 	

int net_set_ipv4_addr(char *ifname,char *ip_addr,char *mask_addr)
{
    int fd, ret;
    unsigned long ip,mask,broadcast;
    struct ifreq ifr;
    struct sockaddr_in sin;
    
    if((fd=socket(AF_INET,SOCK_DGRAM,0))<0)
    {
       return -1;
    }
#if 0
	memset(&ifr, 0, sizeof(ifr));   
	strcpy(ifr.ifr_name, ifname);   

	if(ioctl(fd, SIOCGIFFLAGS, &ifr))
	{
		perror("SIOCGIFFLAGS");
		ret = -1;
		goto l_err3;
	}

	strcpy(ifr.ifr_name, ifname);   
	ifr.ifr_flags &= ~IFF_UP;
	if(ioctl(fd, SIOCSIFFLAGS, &ifr))
	{
		perror("if down");
		ret = -1;
		goto l_err3;
	}
#endif
    /*ip address*/
    strcpy(ifr.ifr_name,ifname);
    memset(&sin, 0, sizeof(struct sockaddr));        
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(ip_addr);
    ip=sin.sin_addr.s_addr;
	
    net_dbg("dev = %s ip_addr %s mask = %s\n",ifname,ip_addr,mask_addr);
	
    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
	
    if(ioctl(fd, SIOCSIFADDR, &ifr)<0)
    {
        perror("SIOCSIFADDR");
		ret = -1;
		goto l_err3;
    }

	//DEBUG("mask_addr %s \n", mask_addr);
    /*netmask*/
    strcpy(ifr.ifr_name,ifname);
    memset(&sin, 0, sizeof(struct sockaddr));        
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(mask_addr);
    mask=sin.sin_addr.s_addr;
    memcpy(&ifr.ifr_netmask,&sin,sizeof(struct sockaddr));
    if(ioctl(fd,SIOCSIFNETMASK,&ifr)<0)
    {
        perror("SIOCSIFNETMASK");
		ret = -1;
		goto l_err3;
    }

    /*broadcast*/
    broadcast=ip |(~mask);
    strcpy(ifr.ifr_name,ifname);
    memset(&sin, 0, sizeof(struct sockaddr));        
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = broadcast;        
    memcpy((char *) &ifr.ifr_broadaddr, &sin,sizeof(struct sockaddr));
    if (ioctl(fd, SIOCSIFBRDADDR, &ifr) < 0)
    {
        perror("SIOCSIFBRDADDR");
		ret = -1;
		goto l_err3;
    }
#if 0
	strcpy(ifr.ifr_name, ifname);   
	ifr.ifr_flags |= IFF_UP;
	if(ioctl(fd, SIOCSIFFLAGS, &ifr))
	{
		perror("if up");
		ret = -1;
		goto l_err3;
	}
#endif

	ret = 0;
	l_err3:
	if(fd>0)
    	close(fd);
    return ret;
}

int net_get_ipv4_addr(char *ifname,char *ip_addr,char *mask_addr, char* broad_addr)
{
	struct ifreq ifr;
	//char address[32] = {0};
	int sockfd;
	//struct sockaddr_in* addr; 
	struct sockaddr_in addr;
	int ret;
	
	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	strncpy(ifr.ifr_name,ifname,IFNAMSIZ-1);

	if(ioctl(sockfd,SIOCGIFADDR,&ifr) == -1)
	{
		perror("ioctl error");
		ret = -1;
		goto net_get_ipv4_addr_err;
	}

	//addr = (struct sockaddr_in *)&(ifr.ifr_addr);
	memcpy(&addr,&(ifr.ifr_addr),sizeof(struct sockaddr_in));	
	//DEBUG("inet addr: %s ",address);
	//strcpy(ip_addr, inet_ntoa(addr->sin_addr));
	strcpy(ip_addr, inet_ntoa(addr.sin_addr));

	if(ioctl(sockfd,SIOCGIFBRDADDR,&ifr) == -1)
	{
		perror("ioctl error");
		ret = -1;
		goto net_get_ipv4_addr_err;;
	}

	//addr = (struct sockaddr_in *)&ifr.ifr_broadaddr;
	//address = inet_ntoa(addr->sin_addr);
	//DEBUG("broad addr: %s ",address);
	memcpy(&addr,&(ifr.ifr_addr),sizeof(struct sockaddr_in));	
	//strcpy(broad_addr, inet_ntoa(addr->sin_addr));
	strcpy(broad_addr, inet_ntoa(addr.sin_addr));

	if(ioctl(sockfd,SIOCGIFNETMASK,&ifr) == -1)
	{
		perror("ioctl error"); 
		ret = -1;
		goto net_get_ipv4_addr_err;;
	}

	//addr = (struct sockaddr_in *)&ifr.ifr_addr;
	//address = inet_ntoa(addr->sin_addr);
	//lvr_printf("inet mask: %s ",address);
	memcpy(&addr,&(ifr.ifr_addr),sizeof(struct sockaddr_in));
	//strcpy(mask_addr, inet_ntoa(addr->sin_addr));
	strcpy(broad_addr, inet_ntoa(addr.sin_addr));

	ret = 0;
net_get_ipv4_addr_err:
	if(sockfd>0)
		close(sockfd);
	return ret;
}

#if 0
//如果未通过dhcp配置过IP，则输入0，否则输入以前成功配置的IP
int net_start_udhcpc(char* ip)
{
	int pid;
	
	pid = vfork();
	if(pid < 0)
		return pid;
	
	if(pid == 0)
	{
		if(!ip)
			execl("/bin/busybox", "busybox", "udhcpc", "-b", "-p", "/etc/udhcpc_pid", (char * )0);
		else
			execl("/bin/busybox", "busybox", "udhcpc", "-b", "-p", "/etc/udhcpc_pid", "-r", ip, (char * )0);
		_exit(0);
	}
		
	return 0;
}

int net_stop_udhcpc()
{
	int pid = 0;
	FILE* fp = fopen("/etc/udhcpc_pid", "rb");

	if(fp <= 0)
		return NET_ERR_NO_DHCPC;

	if(fscanf(fp, "%d", &pid) == 1)
	{
		if(kill(pid, SIGTERM) == -1)
			return NET_ERR_KILL_DHCPC;
		return 0;
	}

	return NET_ERR_NO_DHCPC; //no dhcpc process
}
#endif
#if 0
int net_detect_link(char *ifname)
{
    struct ifreq ifr;
    struct ethtool_value edata;
	int fd;
	
    /* Open a socket. */
    if (( fd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
    {
        perror("detect_ethtool socket error");
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    edata.cmd = ETHTOOL_GLINK;

    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);
    ifr.ifr_data = (char *) &edata;

    if (ioctl(fd, SIOCETHTOOL, &ifr) == -1)
    {
        perror("SIOCETHTOOL");
		if(fd>0)
			close(fd);
        return 2;
    }
	if(fd>0)
		close(fd);
    return (edata.data ? 1 : 0);
}
#else

int net_detect_link(char *ifname)
{
	int ret = 0;
	struct ifreq ifr;
	int skfd = -1;
	
	memset(&ifr, 0, sizeof(ifr));
	
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
	
	if (( skfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0)
	{
		perror("----1 ");
		ret = -1;
		goto	error_0;
		//return -1;
	}
	
	if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) 
	{
		perror("SIOCGIFFLAGS");
		ret = -1;
		goto	error_0;
		//return -1;
	} 
	
	ret = (ifr.ifr_flags&IFF_RUNNING) ? 1 : 0;
error_0:	
	if(skfd>0)
		close(skfd);
	
	
	return ret;
}
#endif
#define BUFSIZE 8192
 
struct route_info{
 u_int dstAddr;
 u_int srcAddr;
 u_int gateWay;
 char ifName[IF_NAMESIZE];
};
int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
  struct nlmsghdr *nlHdr;
  int readLen = 0, msgLen = 0;
  do{
    //收到内核的应答
    if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
    {
      perror("SOCK READ: ");
      return -1;
    }
   
    nlHdr = (struct nlmsghdr *)bufPtr;
    //检查header是否有效
    if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
    {
      perror("Error in recieved packet");
      return -1;
    }
   
    /* Check if the its the last message */
    if(nlHdr->nlmsg_type == NLMSG_DONE) 
    {
      break;
    }
    else
    {
      /* Else move the pointer to buffer appropriately */
      bufPtr += readLen;
      msgLen += readLen;
    }
   
    /* Check if its a multi part message */
    if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0) 
    {
      /* return if its not */
     break;
    }
  } while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
  return msgLen;
}
//分析返回的路由信息
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway)
{
  struct rtmsg *rtMsg;
  struct rtattr *rtAttr;
  int rtLen;
  char *tempBuf = NULL;
 //2007-12-10
  struct in_addr dst;
  struct in_addr gate;
  
  tempBuf = (char *)malloc(100);
  //DEBUG("parseRoutes malloc:%x %d\n", tempBuf, 100);
  
  rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);
  // If the route is not for AF_INET or does not belong to main routing table
  //then return. 
  if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
  return;
  /* get the rtattr field */
  rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
  rtLen = RTM_PAYLOAD(nlHdr);
  for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
   switch(rtAttr->rta_type) {
   case RTA_OIF:
    if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
    break;
   case RTA_GATEWAY:
    rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
    break;
   case RTA_PREFSRC:
    rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
    break;
   case RTA_DST:
    rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
    break;
   }
  }
  //2007-12-10
  dst.s_addr = rtInfo->dstAddr;
  if (strstr((char *)inet_ntoa(dst), "0.0.0.0"))
  {
    gate.s_addr = rtInfo->gateWay;
    sprintf(gateway, "%s",(char *)inet_ntoa(gate));
  }
  //DEBUG("parseRoutes free:%x\n", tempBuf);
  free(tempBuf);
  return;
}
/********************************************************************
* 函数名： net_get_gateway
* 参数名： gateway(out)   网关
* 返回值： 0            成功
*          -1             失败
* 功  能：获取本地机的网关
********************************************************************/
int net_get_gateway(char *gateway)
{
 struct nlmsghdr *nlMsg;
 struct rtmsg *rtMsg;
 struct route_info *rtInfo;
 char msgBuf[BUFSIZE];
 int ret;
 
 int sock, len, msgSeq = 0;
 //创建 Socket 
 if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
 {
  perror("Socket Creation: ");
  return -1;
 }
 
 /* Initialize the buffer */
 memset(msgBuf, 0, BUFSIZE);
 
 /* point the header and the msg structure pointers into the buffer */
 nlMsg = (struct nlmsghdr *)msgBuf;
 rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);
 
 /* Fill in the nlmsg header*/
 nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
 nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .
 
 nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
 nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
 nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.
 
 /* Send the request */
 if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
  //DEBUG("Write To Socket Failed...\n");
  ret = -1;
  goto net_get_gateway_err;
 }
 
 /* Read the response */
 if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
  //DEBUG("Read From Socket Failed...\n");
  ret = -1;
  goto net_get_gateway_err;
 }
 /* Parse and print the response */
 rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
 //DEBUG("net_get_gateway malloc:%x %d\n", rtInfo, sizeof(struct route_info));
 for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
  memset(rtInfo, 0, sizeof(struct route_info));
  parseRoutes(nlMsg, rtInfo,gateway);
 }
 //DEBUG("net_get_gateway free:%x\n", rtInfo);
 free(rtInfo);
 ret = 0; 
net_get_gateway_err: 
 if(sock>0)
	 close(sock);
 return ret;
}



