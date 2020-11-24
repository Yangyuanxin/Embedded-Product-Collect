#include "watchdog.h"

#define WATCHDOGDEV "/dev/watchdog"

static int fd_watchdog=-1;

/*开门狗初始化*/
void watchdog_init()
{
	fd_watchdog = open(WATCHDOGDEV, O_WRONLY);
	if(fd_watchdog == -1) {
	int err = errno;
	printf("\n!!! FAILED to open /dev/watchdog, errno: %d, %s\n", err, strerror(err));
	syslog(LOG_WARNING, "FAILED to open /dev/watchdog, errno: %d, %s", err, strerror(err));	
	exit(EXIT_FAILURE);
	}

	int timeout = 60;  
  
    ioctl(fd_watchdog, WDIOC_SETOPTIONS, WDIOS_ENABLECARD);  
    ioctl(fd_watchdog, WDIOC_SETTIMEOUT, &timeout); 
}

/*喂狗*/
void feed_the_watchdog()
{
	if(fd_watchdog > 0) 
	{
		static unsigned char food = 0;
		ssize_t eaten = write(fd_watchdog, &food, 1);
		if(eaten != 1) 
		{
			printf("FAILED feeding watchdog");
		}
		printf("feed_the_watchdog\n");
		return ;
	}
	else
	{
		printf("open '%s'-->fd:%d\n",WATCHDOGDEV,fd_watchdog);
		return ;
	}
}

/*关开门狗*/
void close_watchdog()
{
	printf("close_watchdog()\n");
	if(fd_watchdog > 0)
	{
		ssize_t wclose = write(fd_watchdog, "V", 1);
		if(wclose != 1) 
		{
			printf("FAILED write close watchdog");
		}
		close(fd_watchdog);
		fd_watchdog=-1;
	}
}

