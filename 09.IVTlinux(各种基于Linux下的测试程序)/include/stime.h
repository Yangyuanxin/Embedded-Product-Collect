#ifndef STIME_INCLUDED
#define STIME_INCLUDED

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
#include "utils.h"
#include "glabtypes.h"

typedef struct tag_unit_datetime
{
  unsigned char yearh;
  unsigned char yearl;
  unsigned char month;
  unsigned char day;
}PACKED unit_datetime;

typedef struct tag_unit_tradetime{
  unit_datetime date; // 4bytes
  unsigned char hour;
  unsigned char minute;
  unsigned char second;
}PACKED unit_tradetime;

ST_ULONG getTickCount();
int getfulltime(unit_tradetime * fulltime);


#endif
