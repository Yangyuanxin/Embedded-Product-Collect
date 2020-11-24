#ifndef _NET_LOCAL_H_
#define _NET_LOCAL_H_

#include <stdio.h>		/* for convenience */
#include <stdlib.h>		/* for convenience */
#include <stddef.h>		/* for offsetof */
#include <string.h>		/* for convenience */
#include <unistd.h>		/* for convenience */
#include <signal.h>		/* for SIG_ERR */
#include <sys/types.h>		/* some systems still require this */
#include <sys/stat.h>
#include <sys/termios.h>	/* for winsize */
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <time.h>

#define CLI_PERM S_IRWXU			/* rwx for user only */

#define STALE	30	/* client's name can't be older than this (sec) */
#define QLEN	20

int serv_listen(const char *);		/* {Progs servlisten_streams servlisten_sockets} */
int serv_accept(int, uid_t *);		/* {Progs servaccept_streams servaccept_sockets} */
int cli_conn(const char *server, const char *client);		/* {Progs cliconn_streams cliconn_sockets} */

/* Read "n" bytes from a descriptor. */
ssize_t recvn(int fd, void *vptr, size_t n);

/* Write "n" bytes to a descriptor. */
ssize_t sendn(int fd, const void *vptr, size_t n);


#endif
