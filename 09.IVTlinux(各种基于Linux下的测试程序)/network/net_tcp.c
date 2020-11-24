/* If this machine supports IPv6 the symbol HAVE_IPv6 should */
/* be defined in either config_unix.h or config_win32.h. The */
/* appropriate system header files should also be included   */
/* by those files.                                           */

//#include "config_win32.h"
#include "net_tcp.h"

#define IPv4	4
#define IPv6	6

struct _socket_tcp {
	int	 	 		 mode;	/* IPv4 or IPv6 */
	char	         *addr;
	uint16_t	     rx_port;
	uint16_t	     tx_port;
	int			 	 ttl;
	int	 	 	 	 fd;
	struct in_addr	 addr4;
#ifdef HAVE_IPv6
	struct in6_addr	 addr6;
#endif /* HAVE_IPv6 */
};


/*****************************************************************************/
/* IPv4 specific functions...                                                */
/*****************************************************************************/

/**
 * tcp_addr_valid:
 * @addr: string representation of IPv4 or IPv6 network address.
 *
 * Returns TRUE if @addr is valid, FALSE otherwise.
 **/
BOOL tcp_addr_valid(const char *dst)
{
    struct in_addr addr4;
	struct hostent *h;

	if (inet_pton(AF_INET, dst, &addr4)) {
		return TRUE;
	} 

	h = gethostbyname(dst);
	if (h != NULL) {
		return TRUE;
	}

    return FALSE;
}

/**
 * tcp_server:
 * @rx_port: receive port.
 * @listenque: how many client can keep connecting in the same time.
 *
 * Creates a session for sending and receiving TCP datagrams over IP
 * networks. 
 *
 * Returns: a pointer to a valid socket_tcp structure on success, NULL otherwise.
 **/
int tcp_server(uint16_t rx_port, const int listenque)
{
	struct sockaddr_in  s_in;
	const int yes = 1;	
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		return -1;
	}
	
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *) &yes, sizeof(yes)) < 0)
	{
		goto failure_tcp_server_init;
	}

	memset(&s_in, 0, sizeof(s_in));
	s_in.sin_family = AF_INET;
	s_in.sin_port = htons(rx_port);
	s_in.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(fd, (struct sockaddr *)&s_in, sizeof(s_in)) < 0)
	{
		goto failure_tcp_server_init;
	}

	if (listen(fd, listenque) )
	{
		goto failure_tcp_server_init;
	}

	return fd;

failure_tcp_server_init:
	if(fd>0)
		close(fd);
	return -1;
}

/**
 * tcp_exit:
 * @s: TCP session to be terminated.
 *
 * Closes TCP session.
 * 
 **/
void tcp_exit(int fd)
{
	if(fd>0)
		close(fd);
}

/**
 * tcp_send:
 * @s: TCP session.
 * @buffer: buffer to write data into.
 * @buflen: length of @buffer.
 * 
 * Writes to datagram queue associated with TCP session.
 *
 * Return value: number of bytes send, returns 0 if no data is available.
 **/
int tcp_send(int fd, char *buffer, int buflen)
{
	int		len;

	assert(buffer != NULL);
	assert(buflen > 0);

	len = send(fd, buffer, buflen, 0);
	if (len <= 0)
	{
		perror("tcp send");
	}

	return len;
}

/**
 * tcp_sendn:
 * @s: TCP session.
 * @buffer: pointer to buffer to be transmitted.
 * @buflen: length of @buffer.
 * 
 * Transmits a TCP datagram containing data from @buffer.
 * 
 * Return value: >0 on success, -1 on failure.
 **/
int tcp_sendn(int fd, void *vptr, int n)
{	
	int		nleft;
	int		nwritten;
	const char	*ptr;
	
	assert(vptr != NULL);
	assert(n > 0);

	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ( (nwritten = send(fd, ptr, nleft, 0)) <= 0)
		{
			if (nwritten < 0 && errno == EINTR)
			{
				nwritten = 0;		/* and call write() again */
			}
			else
			{
				perror ("send");
				return(-1);			/* error */
			}
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}

	return(n - nleft);
}

/**
 * tcp_recv:
 * @s: TCP session.
 * @buffer: buffer to read data into.
 * @buflen: length of @buffer.
 * 
 * Reads from datagram queue associated with TCP session.
 *
 * Return value: number of bytes read, returns 0 if no data is available.
 **/
int tcp_recv(int fd, char *buffer, int buflen)
{
	/* Reads data into the buffer, returning the number of bytes read.   */
	/* If no data is available, this returns the value zero immediately. */
	/* Note: since we don't care about the source address of the packet  */
	/* we receive, this function becomes protocol independent.           */
	int		len;

	assert(buffer != NULL);
	assert(buflen > 0);

	len = recv(fd, buffer, buflen, 0);
	if (len < 0)
	{
		perror("recv");
	}

	return len;
}


int tcp_recvn(int fd, void *vptr, int n)
{
	int	nleft;
	int	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = recv(fd, ptr, nleft, 0)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}


/**
 * tcp_client:
 * @hostip: remote host ip address.
 * @hostport: remote host receive port.
 * 
 * Connect to remote host.
 *
 * Return value: 
 **/
int tcp_client(const char * hostip, const int hostport)
{
	struct sockaddr_in servaddr;
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0)
	{
		return -1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(hostport);

	if(!inet_pton(AF_INET, hostip, &servaddr.sin_addr))
	{
		goto failure_tcp_connect;
	}

	if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		goto failure_tcp_connect;
	}

	return fd;

failure_tcp_connect:
	if(fd>0)
		close(fd);
	return -1;
}

int tcp_socket(int family, int type, int protocol)
{
	int n;

	n = socket(family, type, protocol);

	if (INVALID_SOCKET == n)
	{
		ez_err_sys("socket error");
	}

	return n;
}

int tcp_bind(int fd, struct sockaddr *sa, socklen_t salen)
{
	int n;

	n = bind(fd, sa, salen);

	if (SOCKET_ERROR == n)
	{
		ez_err_sys("bind error");
	}

	return n;
}

int tcp_listen(int fd, int backlog)
{
	int n;

	n = listen(fd, backlog);

	if (SOCKET_ERROR == n)
	{
		ez_err_sys("listen error");
	}

	return n;
}


int tcp_accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

	n = accept(fd, sa, salenptr);

	if (SOCKET_ERROR == n)
	{
		ez_err_sys("accept error");
	}

	return n;
}

int tcp_connect(int fd, struct sockaddr *sa, socklen_t salen)
{
	int n;

	n = connect(fd, sa, salen);
	if (SOCKET_ERROR == n)
	{
		ez_err_sys("connect error");
	}

	return n;
}

int tcp_getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

	n = getpeername(fd, sa, salenptr);

	if (SOCKET_ERROR == n)
	{
		ez_err_sys("getpeername error");
	}

	return n;
}


int tcp_getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

	n = getsockname(fd, sa, salenptr);

	if (SOCKET_ERROR == n)
	{
		ez_err_sys("getsockname error");
	}

	return n;
}


int tcp_getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr)
{
	int n;

	n = getsockopt(fd, level, optname, optval, optlenptr);

	if (SOCKET_ERROR == n)
	{
		ez_err_sys("getsockopt error");
	}

	return n;
}


int tcp_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	int n;

	n = setsockopt(fd, level, optname, optval, optlen);

	if (SOCKET_ERROR == n)
	{
		ez_err_sys("setsockopt error");
	}

	return n;
}


int tcp_shutdown(int fd, int how)
{
	int n;

	n = shutdown(fd, how);

	if (SOCKET_ERROR == n)
	{
		ez_err_sys("shutdown error");
	}

	return n;
}

/**
 * tcp_select:
 * @timeout: maximum period to wait for data to arrive.
 * 
 * Waits for data to arrive for TCP sessions.
 * 
 * Return value: number of TCP sessions ready for reading.
 **/
int tcp_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
       struct timeval *timeout)
{
	int		n;

	n = select(nfds, readfds, writefds, exceptfds, timeout);

	if (SOCKET_ERROR == n)
	{
		//ez_err_sys("tcp_select error\n");
		printf("++++++tcp_select error\n");
	}

	return n;
}

/**
 * tcp_set_nodelay:
 * 
 * set TCP socket connect nodelay
 * 
 **/
BOOL tcp_set_nodelay(int fd)
{
	const int optval = 1;

	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,	(const void *)&optval, sizeof(optval)) < 0)
	{
		ez_err_sys("error setsockopt nodelay");
		return FALSE;
	}

	return TRUE;
}

/**
 * tcp_set_nonblock:
 * @bNb: use nonblock or not. bNb=1, use nonblock;
 * @fd: socket fd.
 * 
 * set TCP socket connect nonblock
 * 
 **/
 BOOL tcp_set_nonblock(BOOL bNb, int fd)
{
	if (bNb)
	{
		if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		{
			ez_err_sys("fcntl(F_SETFL, O_NONBLOCK)");
			return FALSE;
		}
	}
	else
	{
		if (fcntl(fd, F_SETFL, 0) == -1)
		{
			ez_err_sys("fcntl(F_SETFL, 0)");
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * tcp_fd_zero:
 * 
 * Clears file descriptor from set associated with TCP sessions (see select(2)).
 * 
 **/
void tcp_fd_zero(fd_set *fds)
{
	FD_ZERO(fds);
}

/**
 * tcp_fd_set:
 * @s: TCP session.
 * 
 * Adds file descriptor associated of @s to set associated with TCP sessions.
 **/
void tcp_fd_set(int fd, fd_set *fds)
{
	FD_SET(fd, fds);
}

/**
 * tcp_fd_isset:
 * @s: TCP session.
 * @fd: read or write fd 
 * 
 * Checks if file descriptor associated with TCP session is ready for
 * reading.  This function should be called after tcp_select().
 *
 * Returns: non-zero if set, zero otherwise.
 **/
int tcp_fd_isset(int fd, fd_set *fds)
{
	return FD_ISSET(fd, fds);
}

int tcp_fd_clr(int fd, fd_set *fds)
{
	return FD_CLR(fd, fds);
}


