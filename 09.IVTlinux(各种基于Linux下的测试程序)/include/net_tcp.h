#ifndef _NET_TCP_H
#define _NET_TCP_H

#include "net.h"
#include "glabtypes.h"

typedef struct _socket_tcp socket_tcp; 

#if defined(__cplusplus)
extern "C" {
#endif

#define ez_err_sys(format, a...) do{ printf(format, ## a); return -1; } while(0)

BOOL tcp_addr_valid(const char *addr);
int tcp_server(uint16_t rx_port, const int listenque);
int tcp_client(const char * hostip, const int hostport);
void tcp_exit(int fd);

int	tcp_send(int fd, char *buffer, int buflen);
int tcp_recv(int fd, char *buffer, int buflen);
int tcp_sendn(int fd, void *vptr, int n);
int	tcp_recvn(int fd, void *vptr, int n);	

int tcp_socket(int family, int type, int protocol);
int tcp_bind(int fd, struct sockaddr *sa, socklen_t salen);
int tcp_listen(int fd, int backlog);
int tcp_accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int tcp_connect(int fd, struct sockaddr *sa, socklen_t salen);
int tcp_getpeername(int fd, struct sockaddr *sa, socklen_t *salenptr);
int tcp_getsockname(int fd, struct sockaddr *sa, socklen_t *salenptr);
int tcp_getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlenptr);
int tcp_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
int tcp_shutdown(int fd, int how);
int tcp_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
BOOL tcp_set_nodelay(int fd);
BOOL tcp_set_nonblock(BOOL bNb, int fd);

void 	tcp_fd_zero(fd_set *fds);
void 	tcp_fd_set(int fd, fd_set *fds);
int 	tcp_fd_isset(int fd, fd_set *fds);

#if defined(__cplusplus)
}
#endif

#endif
