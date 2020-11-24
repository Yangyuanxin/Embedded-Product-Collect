#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <termios.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <pthread.h>
#include "gps.h"
#include "uart.h"
#include "timer.h"
#include "utils.h"
#include "primary.h"
#include "gps_analyse.h"

//统计信息
typedef struct _statistics_tag{
	uint32_t errorTimes;	//读取超时次数
	uint32_t nosignTimes;	//读取时无信号次数
	float totalKM;			//总里程
}statistics_t;

/*所有gps数据相关信息*/
typedef struct _gpsObject_tag{
	int timerIndex;				//定时器索引
	GPRMC gpsData;				//上一次获取到的gps数据
	statistics_t statistInfo;	//统计信息
}gpsObject_t;
static gpsObject_t gGpsSelf;

int initGpsUart()
{
	int ret;
	ret = OpenCom(GPS_PROT, ports[GPS_PROT].name, 9600);
	if(ret == -1)
	{
		printf("OpenCom fail...\n");
		return -1;
	}
	
  	return 0;
}

int gpsModuleMsgSend(char*buf,uint32_t cmdtag,uint32_t size)
{	
	return(primaryModuleMsgSend(buf,GPS_MODULE,MAIN_MANAGE_MODULE,cmdtag,0,0,size));
}

int sendGpsMsg(uint32_t cmdtag,GPRMC *gps_data)
{
	char buf[LOCAL_BUFSIZE] = {0};
	
	switch(cmdtag)
	{
		case GPS_DATA_UPDATA:
			memcpy(&buf[MSG_CMD_SZ],gps_data,sizeof(GPRMC));
			return gpsModuleMsgSend(buf,GPS_DATA_UPDATA,sizeof(GPRMC));
		case GPS_NOFIND:
		case GPS_GET_DATA_ERROR:
			return gpsModuleMsgSend(buf,cmdtag,0);
		default:
			return 0;
	}

}

BOOL getGpsSysTimer(void * lpParam,int nParamLen)
{
	GPRMC gps_data;
	int ret = 0;

	memset(&gps_data,0,sizeof(gps_data));
	pthread_mutex_lock(&ports[GPS_PROT].lock);
	if(strlen(ports[GPS_PROT].buf)>0)
	{
		ret = gps_analyse(ports[GPS_PROT].buf,&gps_data);
		//printf("%s\n",ports[GPS_PROT].buf);
		if(ret > 0)
		{
			//printf("%s\n",ports[GPS_PROT].buf);
			memset(ports[GPS_PROT].buf,0,sizeof(ports[GPS_PROT].buf));
			ports[GPS_PROT].buflen = 0;
			print_gps(&gps_data);
			sendGpsMsg(GPS_DATA_UPDATA,&gps_data);
		}
		else
		{
			sendGpsMsg(GPS_GET_DATA_ERROR,&gps_data);
			//no signal
			if(ports[GPS_PROT].buflen>=(MAX_BUF_SIZE-1))
			{
				memset(ports[GPS_PROT].buf,0,sizeof(ports[GPS_PROT].buf));
				ports[GPS_PROT].buflen = 0;
			}
			
		}
	}
	else
	{
		//no gps
		sendGpsMsg(GPS_NOFIND,&gps_data);
	}	
	pthread_mutex_unlock(&ports[GPS_PROT].lock);
	return true;
}

void setGpsSystimer(void)
{
	gGpsSelf.timerIndex = trSysTimerAlloc((SYS_TIMER_HANDLER)getGpsSysTimer,0,0,500);
	trSysTimerStart(gGpsSelf.timerIndex, 1, 0);
}

void gpsModuleThread()
{
	int fd = -1, maxFd = -1,nready = -1;
	fd_set readfds;
	struct timeval tv;	
	
	fd = ports[GPS_PROT].handle;
	if(fd <= 0)
	{
		return ;
	}
	maxFd = fd;
	printf("rcv_com_thread create success!\n");
	
	tcflush(ports[GPS_PROT].handle,TCIOFLUSH); 
	printf("ports[%d].handle=%d\n",GPS_PROT,ports[GPS_PROT].handle);
	delayMs(100);
	setGpsSystimer();
	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		
		tv.tv_sec = 0;
		tv.tv_usec = 500000;
		nready = select(maxFd+1, &readfds, 0, 0, NULL);
		if(nready < 0)
		{
			printf("[UART] select error...\n\n");
			continue;
		}
		else if(nready == 0)
		{
			printf("[UART] select time out...\n\n");
		}
		else
		{
			if(FD_ISSET(fd, &readfds))
			{
				readUartStringData(GPS_PROT,fd);
				--nready ;
			}
		}
	}
}

int gpsModuleRun()
{
	pthread_t 		pthreadID		= -1;

	initGpsUart();
	moduleRegist(GPS_MODULE,CLI_GPS);
	if(pthread_create(&pthreadID, NULL, (void *)gpsModuleThread, NULL))
	{
		printf("Failed to create init GpsModule thread!\n");	
		return FAILURE;
	}
	return SUCCESS;
}

