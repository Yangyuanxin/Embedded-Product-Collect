#ifndef _SUPERVISTOR_REGISTER_H_
#define _SUPERVISTOR_REGISTER_H_
#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <string.h>
#include <sys/msg.h>
#include <errno.h>

int supervisorRegister(int argc, char **argv);
void signalCancelHandler();


#endif
