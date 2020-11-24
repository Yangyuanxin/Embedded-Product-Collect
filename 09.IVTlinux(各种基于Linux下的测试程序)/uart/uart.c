/*
** File: uart.c
**
** Description:
**      Provides an RS-232 interface that is very similar to the CVI provided
**      interface library
*/

#include "uart.h"
#include "utils.h"
#include "log.h"

#define DEBUG_UART 	0
#define LOG_UART 		0

static devName_t gdevName[] = {{"/dev/ttymxc0"},{"/dev/ttymxc1"},{"/dev/ttymxc2"},{"/dev/ttymxc3"},{"/dev/ttymxc4"}}; 

/*

*/
long GetBaudRate(long baudRate)
{
    long BaudR;
    switch(baudRate)
    {
	case 115200:
		BaudR=B115200;
		break;
	case 57600:
		BaudR=B57600;
		break;
	case 38400:
		BaudR=B38400;
		break;	
	case 19200:
		BaudR=B19200;
		break;
	case 9600:
		BaudR=B9600;
		break;
	case 2400:
		BaudR=B2400;
		break;
	default:
		BaudR=B0;
    }
    return BaudR;
}



/*
** Function: 以只写方式打开串串口
** Returns:
*/
int OpenCom_write(int portNo, const char deviceName[],long baudRate)
{

   // return OpenComConfig(portNo, deviceName, baudRate, 1, 8, 1, 0, 0);

	int port = portNo;
//	const char deviceName[]=deviceName;
//	long baudRate=baudRate;
	//int parity=1;
	//int dataBits=8;
	//int stopBits=1;
	//int iqSize=0;
	//int oqSize=0;
   
	struct termios newtio;

    //long BaudR;
    //tcgetattr(ports[port].handle, &mytermios_old);
    // int stopBit
    ports[port].busy = 1;
    strcpy(ports[port].name,deviceName);
	//printf("before  open(device)\n");
	//printf("&&&&\nports[portNo].busy is %d\n",ports[port].busy);
	//printf("&&&&\nports[portNo]is %d\n",port);
	 
     if ((ports[port].handle = open(deviceName, O_WRONLY , 0222)) == -1)
    {
        printf("open %s failed\n",deviceName);
        
       // assert(0);
       return -1;
    }
	 
	//printf("port is %d,uartfd is %d\n",port,ports[port].handle);
   // printf("after  open(device)\n");
    /* set the port to raw I/O */
    newtio.c_cflag = CS8 | CLOCAL | CREAD ;
//    newtio.c_cflag |=CRTSCTS;
    newtio.c_iflag = IGNPAR;
//    newtio.c_oflag = 0;
//    newtio.c_lflag = 0;
    newtio.c_oflag = ~OPOST;
    newtio.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);

    newtio.c_cc[VINTR]    = 0;
    newtio.c_cc[VQUIT]    = 0;
    newtio.c_cc[VERASE]   = 0;
    newtio.c_cc[VKILL]    = 0;
    newtio.c_cc[VEOF]     = 4;
    newtio.c_cc[VTIME]    = 0;
   // newtio.c_cc[VTIME]    = 20;
    newtio.c_cc[VMIN]     = 1;
    newtio.c_cc[VSWTC]    = 0;
    newtio.c_cc[VSTART]   = 0;
    newtio.c_cc[VSTOP]    = 0;
    newtio.c_cc[VSUSP]    = 0;
    newtio.c_cc[VEOL]     = 0;
    newtio.c_cc[VREPRINT] = 0;
    newtio.c_cc[VDISCARD] = 0;
    newtio.c_cc[VWERASE]  = 0;
    newtio.c_cc[VLNEXT]   = 0;
    newtio.c_cc[VEOL2]    = 0;
	
    
	//printf("before  cfsetispeed\n");
    cfsetispeed(&newtio, GetBaudRate(baudRate));
	
	cfsetospeed(&newtio, GetBaudRate(baudRate));
	//printf("before  tcsetattr\n");
	tcflush(ports[port].handle, TCIFLUSH); 
// tcsetattr(fd,TCSANOW,&newtio); 

    tcsetattr(ports[port].handle, TCSANOW, &newtio);
    return 0;

	////////////////////////////////////////////////////
}


/*
** Function: 以只读取方式打开串口
** Returns:
**    -1 on failure ,fd>0 success
*/
int OpenCom_read(const char deviceName[],long baudRate)
{
   	int fd;
//	const char deviceName[]=deviceName;
//	long baudRate=baudRate;
	//int parity=1;
	//int dataBits=8;
	//int stopBits=1;
	//int iqSize=0;
	//int oqSize=0;
   
	struct termios newtio;

    //long BaudR;
    //tcgetattr(ports[port].handle, &mytermios_old);
                 // int stopBit
    //ports[port].busy = 1;
    //strcpy(ports[port].name,deviceName);
	//printf("before  open(device)\n");
	//printf("&&&&\nports[portNo].busy is %d\n",ports[port].busy);
	//printf("&&&&\nports[portNo]is %d\n",port);
	 
     if ((fd = open(deviceName, O_RDONLY , 0444)) == -1)
    {
        printf("open %s failed\n",deviceName);
       return -1;
    }
	 
	printf("port is %s,duartfd is %d\n",deviceName,fd);
   // printf("after  open(device)\n");
    /* set the port to raw I/O */
    newtio.c_cflag = CS8 | CLOCAL | CREAD ;
//    newtio.c_cflag |=CRTSCTS;
    newtio.c_iflag = IGNPAR;
//    newtio.c_oflag = 0;
//    newtio.c_lflag = 0;
    newtio.c_oflag = ~OPOST;
    newtio.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);

    newtio.c_cc[VINTR]    = 0;
    newtio.c_cc[VQUIT]    = 0;
    newtio.c_cc[VERASE]   = 0;
    newtio.c_cc[VKILL]    = 0;
    newtio.c_cc[VEOF]     = 4;
    newtio.c_cc[VTIME]    = 0;
   // newtio.c_cc[VTIME]    = 20;
    newtio.c_cc[VMIN]     = 1;
    newtio.c_cc[VSWTC]    = 0;
    newtio.c_cc[VSTART]   = 0;
    newtio.c_cc[VSTOP]    = 0;
    newtio.c_cc[VSUSP]    = 0;
    newtio.c_cc[VEOL]     = 0;
    newtio.c_cc[VREPRINT] = 0;
    newtio.c_cc[VDISCARD] = 0;
    newtio.c_cc[VWERASE]  = 0;
    newtio.c_cc[VLNEXT]   = 0;
    newtio.c_cc[VEOL2]    = 0;
	
    
	//printf("before  cfsetispeed\n");
    cfsetispeed(&newtio, GetBaudRate(baudRate));
	
	cfsetospeed(&newtio, GetBaudRate(baudRate));
	//printf("before  tcsetattr\n");
	//tcflush(ports[port].handle, TCIFLUSH); 
// tcsetattr(fd,TCSANOW,&newtio); 

    tcsetattr(fd, TCSANOW, &newtio);
    return fd;

}

/*
功能:按输入的参数打开串口，成功后把设备描述符放到数据ports[port].handle中去
** Arguments:
**    portNo - ports数据中对应的成员
**    deviceName - 设备名
**    baudRate - 波特率(57600 for example)
**    parity - 0 for no parity
**    dataBits - 7 or 8
**    stopBits - 1 or 2
**    iqSize - ignored
**    oqSize - ignored
** Returns: 失败-1,成功0
** Limitations:
**    parity, stopBits, iqSize, and oqSize are ignored
*/
int OpenComConfig(int port,
                  const char deviceName[],
                  long baudRate,
                  int parity,
                  int dataBits,
                  int stopBits,
                  int iqSize,
                  int oqSize){

   
	struct termios newtio;

    //long BaudR;
    //tcgetattr(ports[port].handle, &mytermios_old);
                 // int stopBit
    ports[port].busy = 1;
    strcpy(ports[port].name,deviceName);
	//printf("before  open(device)\n");
	//printf("&&&&\nports[portNo].busy is %d\n",ports[port].busy);
	//printf("&&&&\nports[portNo]is %d\n",port);
	 
     if ((ports[port].handle = open(deviceName, O_RDWR , 0666)) == -1)
    {
        printf("open %s failed\n",deviceName);
        
       // assert(0);
       return -1;
    }
	 
	printf("port is %d,name %s baudRate %ld uartfd is %d\n",port,ports[port].name,baudRate,ports[port].handle);
   // printf("after  open(device)\n");
    /* set the port to raw I/O */
    newtio.c_cflag = CS8 | CLOCAL | CREAD ;
//    newtio.c_cflag |=CRTSCTS;
    newtio.c_iflag = IGNPAR;
//    newtio.c_oflag = 0;
//    newtio.c_lflag = 0;
    newtio.c_oflag = ~OPOST;
    newtio.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);

    newtio.c_cc[VINTR]    = 0;
    newtio.c_cc[VQUIT]    = 0;
    newtio.c_cc[VERASE]   = 0;
    newtio.c_cc[VKILL]    = 0;
    newtio.c_cc[VEOF]     = 4;
    newtio.c_cc[VTIME]    = 0;
   // newtio.c_cc[VTIME]    = 20;
    newtio.c_cc[VMIN]     = 1;
    newtio.c_cc[VSWTC]    = 0;
    newtio.c_cc[VSTART]   = 0;
    newtio.c_cc[VSTOP]    = 0;
    newtio.c_cc[VSUSP]    = 0;
    newtio.c_cc[VEOL]     = 0;
    newtio.c_cc[VREPRINT] = 0;
    newtio.c_cc[VDISCARD] = 0;
    newtio.c_cc[VWERASE]  = 0;
    newtio.c_cc[VLNEXT]   = 0;
    newtio.c_cc[VEOL2]    = 0;
	
    
	//printf("before  cfsetispeed\n");
    cfsetispeed(&newtio, GetBaudRate(baudRate));
	
	cfsetospeed(&newtio, GetBaudRate(baudRate));
	//printf("before  tcsetattr\n");
	tcflush(ports[port].handle, TCIFLUSH); 
// tcsetattr(fd,TCSANOW,&newtio); 

    tcsetattr(ports[port].handle, TCSANOW, &newtio);
	pthread_mutex_init(&ports[port].lock,NULL);
	
    return 0;
}


/*
** Function: CloseCom
**
** Description:
**    Closes a previously opened port
**
** Arguments:
**    The port handle to close
**
**    Returns:
**    -1 on failure
*/
int CloseCom(int portNo)
{
	struct termios mytermios_old;
	
    if (ports[portNo].busy)
    {
		// tcsetattr (ports[portNo].handle, TCSADRAIN, &mytermios_old);
		tcgetattr(ports[portNo].handle, &mytermios_old);
		close(ports[portNo].handle);
		ports[portNo].busy = 0;
		return 0;
    }
    else
    {
        return -1;
    }
}

/*
** Function: ComRd
**
** Description:
**    Reads the specified number of bytes from the port.
**    Returns when these bytes have been read, or timeout occurs.
**
** Arguments:
**    portNo - the handle
**    buf - where to store the data
**    maxCnt - the maximum number of bytes to read
**
** Returns:
**    The actual number of bytes read
*/
int ComRd(int portNo, char buf[], int maxCnt,int Timeout)
{
    int actualRead = 0;
    fd_set rfds;
    struct timeval tv;
    int retval;
	#if 0
    if (!ports[portNo].busy)
    {
        assert(0);
    }
	#endif
    /* camp on the port until data appears or 5 seconds have passed */
    FD_ZERO(&rfds);
    FD_SET(ports[portNo].handle, &rfds);
    tv.tv_sec = Timeout/1000;
    tv.tv_usec = (Timeout%1000)*1000;
    retval = select(16, &rfds, NULL, NULL, &tv);

    if (retval>0)
    {  
        printf("before read\n");
        actualRead = read(ports[portNo].handle, buf, maxCnt);
    }  
	//add++++++++++++++++++++++++++
	//tcflush(ports[portNo].handle,TCIOFLUSH) ;
	printf("afterread\n");
    return actualRead;
}

/*****************************************************************
功能:清空串口所有数据
args: fd 串口套接字；
return: 成功0;错误-1;
*****************************************************************/
int clear_uart(int fd)
{
	char tmp[1024];
	int maxfd;
	fd_set readfds;
	int ret,n=0;
	int flag;
	struct timeval tv;
	maxfd=fd+1;
	
	while(1)
	{
		tv.tv_sec = 0; 
		tv.tv_usec = 50*1000; 	
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		ret = select(maxfd, &readfds, 0, 0, &tv);
		if(ret > 0)
	 	{
	        if(FD_ISSET(fd,&readfds)) //KEYMACHINEPORT
	        {
  				ret = read(fd,tmp ,sizeof(tmp));
				if( ret > 0 )
				{
					n += ret;
					continue;
				}
     			else
     			{
      				flag = -1;
					break;
     			}
	        }
		}
		else if(ret == 0 )
		{
			flag = 0;
			break;
		}
		else
        {
			flag= -1;
 			break;
       }
	}
	printf("clear serial fd(%d),date cleaned n=%d\n",fd,n);
	tcflush(fd,TCIOFLUSH);
	return flag;
}

/*
** Function: ComWrt
**
** Description:
**    Writes out the specified bytes to the port
**
** Arguments:
**    portNo - the handle of the port
**    buf - the bytes to write
**    maxCnt - how many to write
**
** Returns:
**    The actual number of bytes written
*/
int ComWrt(int portNo, const char *buf, int maxCnt)
{
    int written;
	//int ret;
	//int i;
	char desc[2048] = {0};
	//tcflush(ports[portNo].handle,TCIOFLUSH);   //串口发送数据前先清空输入输出缓存
	//printf("&&&&\nports[portNo].busy is %d\n",ports[portNo].busy);
	//printf("&&&&\nports[portNo]is %d\n",portNo);
	tcflush(ports[portNo].handle,TCOFLUSH);
	if (!ports[portNo].busy)
    {
        assert(0);
    }
	usleep(500);//添加延时

	HEX_to_ASCII_Convertor ((ST_UCHAR*)buf, maxCnt, (ST_UCHAR*)desc);
	#if DEBUG_UART
	printf("Send to %s (len:%d)>>>:%s\n",ports[portNo].name,maxCnt,desc);
	#endif
	
	#if LOG_UART
	logdebug("Send to %s (len:%d)>>>:%s\n",ports[portNo].name,maxCnt,desc);
	#endif
	
	//printf("comSendLen=%d\r\n",maxCnt);
    written = write(ports[portNo].handle, buf, maxCnt);
	fflush(stdout);
	usleep(10000);//添加延时
    return written;
	
}

/*
** Function: OpenCom
** Description:
**    Opens a serial port with default parameters
** Arguments:
**    portNo - handle used for further access
**    deviceName - the name of the device to open
** Returns:
**    -1 on failure ,0 success
*/
int OpenCom(int portNo, const char deviceName[],long baudRate)
{
    return OpenComConfig(portNo, deviceName, baudRate, 1, 8, 1, 0, 0);
}

void initUartConf()
{
	int i = 0;
	memset(&ports,0,sizeof(ports));
	for(i = 0; i < MAX_PORTS;i++) 
	{
		strcpy(ports[i].name,gdevName[i].string);
	}
	#if 0
	strcpy(ports[PORT_ZERO].name,DEV_ZERO);
	strcpy(ports[PORT_ONE].name,DEV_ONE);
	strcpy(ports[PORT_TWO].name,DEV_TWO);
	strcpy(ports[PORT_THREE].name,DEV_THREE);
	strcpy(ports[PORT_FOUR].name,DEV_FOUR);	
	#endif
}

void closeUart()
{
	int i = 0;

	for(i = 0; i < MAX_PORTS;i++) 
	{
		if(ports[i].handle > 0)
		{
			close(ports[i].handle);
		}
	}
}

int addComStringData(int port , char* data , int datalen)
{
	int len;
	pthread_mutex_lock(&ports[port].lock);
	len = strlen(ports[port].buf);
	if(((ports[port].buflen+datalen)<=(MAX_BUF_SIZE-1))&&(datalen))
	{
		memcpy(&ports[port].buf[len],data,datalen);
		ports[port].buflen=strlen(ports[port].buf);
	}
	len = ports[port].buflen;
	pthread_mutex_unlock(&ports[port].lock);
	return len;
}

int readUartStringData(int portNo,int fd)
{	
	char rcv_buf[MAX_BUF_SIZE] = {0};
	int rcvLen = 0;
	
	//防止数据过多时，一次性接收不全
	rcvLen = read(fd, rcv_buf, MAX_BUF_SIZE);
	if(rcvLen <= 0)
	{
		printf("[UART] receive no datas...\n");
		//tcflush(fd,TCIFLUSH); 
		//delayMs(200);
		return 1;
	}
	else
	{
		//sendUartData(portNo,rcv_buf,rcvLen);
		addComStringData(portNo,rcv_buf,rcvLen);
		//sendUartData(portNo);
	}
	
	return 0;
}


