#ifndef _UART_H_
#define _UART_H_

#include <stdio.h>
#include <assert.h> 
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include "config.h"

typedef enum _port_tag
{
	PORT_ONE,
	PORT_TWO,
	PORT_THREE,
	PORT_FOUR,
	PORT_FIVE,
}port_t;

#if 0
#define PORT_FOUR 	4
#define DEV_FOUR  	"/dev/ttymxc4" 

#define PORT_THREE 	3
#define DEV_THREE  	"/dev/ttymxc3" 

#define PORT_TWO 	2
#define DEV_TWO  	"/dev/ttymxc2" 

#define PORT_ONE 	1
#define DEV_ONE  	"/dev/ttymxc1" 

#define PORT_ZERO 	0
#define DEV_ZERO  	"/dev/ttymxc0" 
#endif

#define GPS_PROT	PORT_FOUR

typedef struct _com_buf
{
	char buf[MAX_BUF_SIZE];
	int len;
	pthread_mutex_t 	lock;/*»¥³âËø*/
}comBuf_t;

int OpenCom(int portNo,const char deviceName[],long baudRate);
int CloseCom(int portNo);
int ComRd(int portNo,char buf[],int maxCnt,int Timeout);
int ComWrt(int portNo,const char * buf,int maxCnt);
int clear_uart(int fd);
int OpenCom_read (const char deviceName[],long baudRate);
int OpenCom_write(int portNo, const char deviceName[],long baudRate);
void initUartConf();
void closeUart();
int readUartStringData(int portNo,int fd);


//char read_cb(char *chr);
//void snd_bity(char key);


//long GetBaudRate(long baudRate);
//int OpenComConfig(int port,
//                  const char deviceName[],
//                  long baudRate,
//                  int parity,
//                  int dataBits,
//                  int stopBits,
//                  int iqSize,
//                  int oqSize);


#endif

