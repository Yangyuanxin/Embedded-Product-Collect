#ifndef __HOSTCOMDATAHANDLE_H
#define	__HOSTCOMDATAHANDLE_H

#include <string.h>
#include <stdio.h>
#include "sys.h"
#include "hostcomprotocol.h"


#define		TEMP_COM_PACK_SIZE			10
#define		TEMP_COM_BUFFER_SIZE		1300

typedef struct ComDataQueue
{
	u8 	TempComDataBuffer[TEMP_COM_BUFFER_SIZE];//临时串口数据缓冲区
	u8 	QueueNowPoint;							//队列现在指向
	u8 	QueueCount;								//操作队列计数
	u8  SharePara;								//共享参数
	u16	EveryPackSize[TEMP_COM_PACK_SIZE];		//记录每一包串口数据的大小
	u16 EnterOffsetAddr;						//进队列偏移地址
	u16 OutOffsetAddr;							//出队列偏移地址
}_ComDataQueue;

extern _ComDataQueue com_data_queue;

void 	EnterComDataQueue(u8 *SourceBuf,_ComDataQueue *_com_data_queue,u16 DataLen);
void	OutComDataQueue(_ComDataQueue *_com_data_queue,u8 *DestBuf);

#endif