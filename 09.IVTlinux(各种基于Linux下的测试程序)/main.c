#include <stdio.h>
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
#include <time.h>
#include "gps.h"
#include "log.h"
#include "uart.h"
#include "utils.h"
#include "timer.h"
#include "config.h"
#include "watchdog.h"
#include "gps.h"
#include "video.h"
#include "primary.h"
#include "supervisorRegister.h"

static ST_UCHAR doIt = SD_TRUE;

void hwInit()
{
	//watchdog_init();
	initUartConf();	
}

void exit_success()
{
	close_watchdog();
	closeUart();
	videoModuleExit();
	
	printf("****************************************************************\n");
	printf("exit success...\n");
	exit(EXIT_SUCCESS);
}

void createThreads()
{
	#if 0
	pthread_t thread_id;
	int res;

	res = pthread_create (&thread_id, NULL, (void *)gpsModuleThread, 0);
	if (0 != res) {
	  logerror("Create gpsComThread Thread Failed\n");
	  exit(-1);
	}
	#endif
}

void sigHandler()
{
	signalCancelHandler();
	doIt = SD_FALSE;

}

void signalExit(void)
{
	
	/*退出的信号*/
	/*KILL*/
	if(signal(SIGTERM,sigHandler) == SIG_ERR){  
        perror("signal errror");  
        exit(EXIT_FAILURE);  
    }

	/*Ctrl-C*/
	if(signal(SIGINT,sigHandler) == SIG_ERR){  
        perror("signal errror");  
        exit(EXIT_FAILURE);  
    }

	/*终端退出*/
	if(signal(SIGHUP,sigHandler)== SIG_ERR){  
        perror("signal errror");  
        exit(EXIT_FAILURE);  
    }

	/*Ctrl-\*/
	if(signal(SIGQUIT,sigHandler) == SIG_ERR){  
        perror("signal errror");  
        exit(EXIT_FAILURE);  
    }
	
}

void usage(void)
{
   printf("usage: app_name [option]\n");
   printf("\t-v            show module version information\n");
   printf("\t-h            print this help and exit\n");
}

int  main(int argc ,char **argv)
{
	int opt = 0;
	
	while ((opt = getopt(argc, argv, "vh")) != -1) {
	switch (opt) {
		case 'h':
			usage();
			return 0;
		case 'v':
			printf(VERSION_INFO);
			return 0;
		default:
			fprintf(stderr, "Illegal argument: \"%c\"\n", opt);
			return -1;
		}
	}

	printf(VERSION_INFO);
	logdebug(VERSION_INFO);//日志记录
	supervisorRegister(argc,argv);//监管注册
	signalExit();//退出信号
	initMachineConf();
	hwInit();//硬件初始化
	sysTimerInit();//定时器线程
	createThreads();
	primaryModuleRun();
	videoModuleRun();
	mainModuleRun();
	gpsModuleRun();
	
	delayMs(100);
	while(doIt)
	{
		delayMs(1000);
	}
	exit_success();//成功退出
	
	return 0;
}

