#ifndef _CONF_H_
#define _CONF_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#else

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#endif

#include "glabtypes.h"
#include "iniConfigFile.h"

#define CONLINELENGTH 1024 	//配置文件一行的长度最大值

/////////////////////////////////////////////////////////////////////////////////////
#define NONE                 ""
#define BOLD                 ""

#define VERSION_INFO  BOLD"\n [IVT V1.0]  "NONE  __DATE__ " (" __TIME__ ")" "\n\n"

/* the maximum number of ports we are willing to open */
#define MAX_PORTS 			5

#define DEBUG_MODE			0

/*串口最大的帧缓存*/
#define MAX_SERIAL_LEN		(64*1024)
#define MAX_BUF_SIZE 		(10240)

/*this array hold information about each port we have opened */
struct PortInfo{
	int busy;
	char name[32];
	int handle;
	char buf[MAX_BUF_SIZE];
	int buflen;
	pthread_mutex_t lock;
};

//定义设备名称
typedef struct _devName_tag
{
	char string[16];
}devName_t;

typedef struct _tag_Chinese_name {
    int index;
    char chineseName[64];
}PACKED chinese_name_t;

typedef struct _chinese_name_t{
	char name[64];
}Chinesename_t;

/* 网络参数 */
typedef struct {
    uint32_t EthernetIP;                    /* 以太网IP */
    uint32_t IPMask;                        /* 子网掩码 */
    uint32_t GatewayIP;                     /* 默认网关 */
    uint32_t DNSIP;                         /* DNS IP */
	uint32_t ServerIP;                      /* 服务器 IP */
    uint16_t ServerPort;                    /* 服务器端口 */
    uint16_t HttpPort;                      /* HTTP端口 */
    uint8_t  DHCPEnable;                    /* 启用DHCP 0 关闭; 1 打开 */
    uint8_t  UPNPEnable;                    /* 启用UPNP 0 关闭; 1 打开 */
    uint8_t  MacAddr[6];                    /* MAC地址 */
    uint8_t  Reserve[8];					/* 备用*/
} NETCONFIG, *PNETCONFIG;

/*摄像头参数*/
typedef struct _cameraPara_tag
{
	int 		width;//视频宽度
	int 		hight;//视频高度
}cameraPara_t;

/* 视频参数 */
typedef struct _videoConfig_tag{
	
	cameraPara_t cameraPara;
	
} videoConfig_t;


/*this array hold information about each port we have opened */
struct PortInfo ports[MAX_PORTS];//记录5个串口设备的结构体

//char *ReadConfigfile(char *fileName, char *item,char * mydefault,char *dest);
//void get_key_string(char *section,char * file,char* key,char* dest,char *def);
//int WriteConfigfile(char *fileName, char *item,char *value);
void initMachineConf();
void loadConfig();


#endif


