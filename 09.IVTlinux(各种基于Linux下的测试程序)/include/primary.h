
#ifndef _PRIMARY_H_
#define _PRIMARY_H_

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
#include "config.h"
#include "common.h"

int moduleRegist(module_t module, const char *moduleName);
int primaryModuleMsgSend(char *buf, uint16_t src, uint16_t dst, uint32_t tag, uint16_t op, uint32_t ch, uint32_t size);
int funcModuleMsgRecv(module_t module , char *buf , struct timeval *timeout);
int primaryModuleRun();
int mainModuleRun();

#endif

