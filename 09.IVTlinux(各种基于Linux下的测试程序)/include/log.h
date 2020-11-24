#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/io.h>
#include <unistd.h>
#include "utils.h"
#include "stime.h"


void logdebug(const char * strInfo, ...);
void loginfo(const char * strInfo, ...);
void logwarn(const char * strInfo, ...);
void logerror(const char * strInfo, ...);

#endif // LOG_H
