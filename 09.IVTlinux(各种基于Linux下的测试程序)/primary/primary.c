#include "gps.h"
#include "net.h"
#include "net_tcp.h"
#include "utils.h"
#include "net_local.h"
#include "primary.h"

int primaryModuleMsgSend(char *buf, uint16_t src, uint16_t dst, uint32_t tag, uint16_t op, uint32_t ch, uint32_t size)
{
	ipc_msg_t*  pMsg = NULL;
	PCMDHEADER  pCmd = NULL;

	pMsg = (ipc_msg_t *)buf;
	memset(pMsg,0,sizeof(ipc_msg_t));
	pCmd = (PCMDHEADER)(buf+sizeof(ipc_msg_t));
	
	pMsg->srcModule 		= src;
	pMsg->tagModule 		= dst;
	pCmd->cmdTag 			= tag;
	pCmd->cmdChannel		= ch;
	pCmd->cmdOperate		= op;
	pCmd->cbSize 			= size;

	//printf("gModSocket[dst].clifd = %d\n",gModSocket[src].clifd);
	return write(gModSocket[src].clifd, buf, MSG_CMD_SZ + size);	

}

/*模块注册*/
int moduleRegist(module_t module , const char *moduleName)
{
	char send_buf[LOCAL_BUFSIZE];
	ipc_msg_t Msg;
	CMDHEADER cmdInfo;

	gModSocket[module].clifd = INVALID_SOCKET;
	while(gModSocket[module].clifd <= 0)
	{
		gModSocket[module].clifd = cli_conn(SERVER_FILE, moduleName);
		if(gModSocket[module].clifd <= 0)
		{
			delayMs(1000);
		}
	}

	memset(&cmdInfo,0,sizeof(CMDHEADER));
	cmdInfo.cmdTag = MODULE_REGISTER;
	cmdInfo.cmdOperate = OPERATE_START;
	cmdInfo.cmdChannel = 0;
	cmdInfo.cbSize = 0;

	memset(&Msg, 0, sizeof(Msg));
	Msg.srcModule = module;
	Msg.tagModule = MAIN_MANAGE_MODULE;

	printf("Module[%d] : send register message!	client -> server, clifd=%d\n",module,gModSocket[module].clifd);

	memset(send_buf, 0, LOCAL_BUFSIZE);
	memcpy(send_buf,&Msg,sizeof(ipc_msg_t));
	memcpy(send_buf + sizeof(ipc_msg_t),&cmdInfo,sizeof(CMDHEADER));

	if(write(gModSocket[module].clifd, send_buf, MSG_CMD_SZ) == -1)
	{
		perror("can not send message\n");
		return FAILURE;
	}
	gModSocket[module].mod_num = module;
	
	return SUCCESS;
}

int primaryModuleInit()
{
	int i = 0;

	for (i = 0; i<MAX_MODULE_CONN_NUM; i++)
	{
		gModSocket[i].fd = INVALID_SOCKET;
		gModSocket[i].clifd= INVALID_SOCKET;
		gModSocket[i].mod_num = 0;
	}	
	
	return SUCCESS;
}

int primaryModuleMsgRecv(int fd ,char *buf)
{
	ipc_msg_t		ipcMsg;	
	PCMDHEADER 		pCmd	= NULL;
	int recvlen = 0,cbSize	= 0;

	recvlen = read(fd, buf, MSG_CMD_SZ);		
	if(recvlen > 0)
	{
		if (recvlen != MSG_CMD_SZ)
		{	/*discard received data */
			return 0;
		}
		memcpy(&ipcMsg,buf,sizeof(ipc_msg_t));
		pCmd = (PCMDHEADER)(buf+sizeof(ipc_msg_t));	
		if((pCmd->cbSize > 0)&&((pCmd->cbSize + recvlen) < LOCAL_BUFSIZE))
		{
			cbSize = read(fd, buf + recvlen, pCmd->cbSize);
			if(cbSize != pCmd->cbSize)
			{
				DEBUG("recv msg body len %d, cbSize = %d!\n",cbSize,pCmd->cbSize);	
				return 0;
			}
			recvlen += cbSize;
			buf[recvlen] = '\0';
		}
	}

	return recvlen;
}

/*功能模块的接收、回应*/
/*如果模块只发送不接收，则不需要调用*/
int funcModuleMsgRecv(module_t module , char *buf , struct timeval *timeout)
{
	fd_set 			rset;	
	int	 			fd 		= gModSocket[module].clifd;
	int 			buflen 	= 0;	 

	FD_ZERO(&rset);
	FD_SET(fd, &rset);

	if (-1 != select(fd + 1, &rset, NULL, NULL, timeout))
	{
		buflen = primaryModuleMsgRecv(gModSocket[module].clifd,buf);
		return buflen;
	}
	return 0;
}	

void procGpsData(char *buffer ,int buflen)
{
	PCMDHEADER pCmdInfo = (PCMDHEADER)(buffer+sizeof(ipc_msg_t));

	switch(pCmdInfo->cmdTag)
	{
		case GPS_DATA_UPDATA:
			printf("recv gps data\n");
			break;
		case GPS_NOFIND:
			printf("no find gps\n");
			break;
		case GPS_GET_DATA_ERROR:
			printf("no gps signal\n");
			break;	
		default:
			break;
	}

}

void primaryModuleHandle(int fd,char *buffer ,int buflen)
{
	PCMDHEADER pCmdInfo = (PCMDHEADER)(buffer+sizeof(ipc_msg_t));
	ipc_msg_t ipcMsg;
	
	memcpy(&ipcMsg,buffer,sizeof(ipc_msg_t));
	if(MODULE_REGISTER == pCmdInfo->cmdTag)
	{
		gModSocket[ipcMsg.srcModule].fd = fd;
		DEBUG("MAIN_MANAGE_MODULE receive Moudule[%d] register ack! server fd = %d\n",ipcMsg.srcModule,fd);
		return ;		
	}
	switch(GET_MODULE(pCmdInfo->cmdTag))
	{
		case MAIN_MANAGE_MODULE:
			break;
		case COMMUNI_MANAGE_MODULE:
			break;
		case GPS_MODULE:
			procGpsData(buffer,buflen);
			break;
		case NET_TRANS_MODULE:
			break;
		case VOICE_MODULE:
			break;
		default:
			break;
	}
}

void primaryMsgRecv(int confd, fd_set *allset)
{
	int recvlen =0;
	char buf[LOCAL_BUFSIZE] = {0};
	
	recvlen = primaryModuleMsgRecv(confd,buf);
	if(recvlen==0)
	{
		printf("0000000000000000000000000000000000000000\n");
		FD_CLR(confd, allset);
		if(confd > 0)
		{
			close(confd);
		}	
	}
	else if(recvlen > 0)
	{
		primaryModuleHandle(confd,buf, recvlen);
	}

	return ;
}

void primaryModuleReisterHandle()
{
	//创建本地侦听
	int		n, maxfd, listenfd, fd = -1;
	uid_t	uid;
	fd_set	rset, listenSet;
	struct timeval tv;
	
	DEBUG("ivt primaryModuleReisterHandle: =%d\n",getpid());

	/* obtain fd to listen for client requests on */
	if ((listenfd = serv_listen(SERVER_FILE)) < 0)
	{
		DEBUG("serv_listen error");
		return;
	}

	while(1)//处理模块注册
	{
		FD_ZERO(&listenSet);
		FD_ZERO(&rset);
		FD_SET(listenfd, &listenSet);
		FD_SET(listenfd, &rset);

		maxfd = listenfd;
		tv.tv_sec	 = 0;
		tv.tv_usec	 = 500000;
		
		if ((n = select(maxfd + 1, &rset, NULL, NULL, &tv)) < 0)
		{
			perror("primaryModuleReister select error");
			continue;
		}

		if (FD_ISSET(listenfd, &listenSet)) {
		DEBUG("listenfd(%d) serv_accept debug\n",listenfd);
		/* accept new client request */
		if ((fd = serv_accept(listenfd, &uid)) < 0)
		{
			DEBUG("serv_accept error: %d", fd);
			perror("******** PrimaryModule serv_accept\n");
			fflush(0);
			continue;
		}
		if (fd > maxfd)
			maxfd = fd;

		DEBUG("Primary Module :  socket = %d\n", fd);
		if(fd > 0)
		{
			FD_SET(fd, &rset);
			primaryMsgRecv(fd, &rset);

			/*
			FD_SET(fd, &rset);
			tv.tv_sec	 = 0;
			tv.tv_usec	 = 500000;
			
			if ((n = select(maxfd + 1, &rset, NULL, NULL, &tv)) < 0)
			{
				perror("primary select error");
				continue;
			}
			
			if (FD_ISSET(fd, &rset)) 
			{
				primaryMsgRecv(fd, &rset);
			}
			*/
		}	
		continue;
		}	
	}

}

void primaryModuleHandleThread()
{
	int i = 0 ,maxfd = -1,n;
	fd_set	rset;
	struct timeval tv;

	delayMs(300);
	while(1)
	{
		FD_ZERO(&rset);	
		for (i = 0; i<MAX_MODULE_CONN_NUM; i++)
		{
			if (gModSocket[i].fd > 0)
			{
				if (gModSocket[i].fd  > maxfd)
				{
					maxfd = gModSocket[i].fd ;
				}	
				FD_SET(gModSocket[i].fd, &rset);
			}
		}

		tv.tv_sec = 0;
		tv.tv_usec = 300000;
		if ((n = select(maxfd + 1, &rset, NULL, NULL, &tv)) < 0)
		{
			perror("primaryModuleHandleThread select error");
			continue;
		}
		else if(n > 0)
		{
			for (i = 0; i < MAX_MODULE_CONN_NUM; i++) 
			{
				if ((gModSocket[i].fd) <= 0)
					continue;
				if (FD_ISSET(gModSocket[i].fd, &rset)) 
				{
					primaryMsgRecv(gModSocket[i].fd, &rset);
				}
			}
		}	
	}
}

int primaryModuleRun()
{
	pthread_t 		pthreadID		= -1;

	primaryModuleInit();

	//处理注册
	if(pthread_create(&pthreadID, NULL, (void *)primaryModuleReisterHandle, NULL))
	{
		printf("Failed to create primary Module thread!\n");	
		return FAILURE;
	}

	if(pthread_create(&pthreadID, NULL, (void *)primaryModuleHandleThread, NULL))
	{
		printf("Failed to create primary Module thread!\n");	
		return FAILURE;
	}
	return SUCCESS;
}

void mainModuleThread()
{
	char buf[LOCAL_BUFSIZE] = {0};
	int buflen = 0;
	struct timeval	timeout;
	
	while(1)
	{
		timeout.tv_sec 	= 0;
		timeout.tv_usec = 300000;
		buflen = 0;
		memset(buf ,0 ,LOCAL_BUFSIZE);
		buflen = funcModuleMsgRecv(MAIN_MANAGE_MODULE,buf,&timeout);
	}
}

int mainModuleRun()
{
	pthread_t 		pthreadID		= -1;
	
	moduleRegist(MAIN_MANAGE_MODULE , CLI_MAIN);
	if(pthread_create(&pthreadID, NULL, (void *)mainModuleThread, NULL))
	{
		printf("Failed to create primary Module thread!\n");	
		return FAILURE;
	}
	return SUCCESS;
}

