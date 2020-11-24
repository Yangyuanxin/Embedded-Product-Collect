#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>  
#include <syslog.h> 
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
#include <fcntl.h>
#include <linux/watchdog.h>


void watchdog_init();
void feed_the_watchdog();
void close_watchdog();


#endif


