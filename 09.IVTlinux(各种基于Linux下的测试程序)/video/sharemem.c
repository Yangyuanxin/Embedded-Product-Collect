#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "sharemem.h"

#define FRAME_SHM_KEY		0x1234

typedef struct _frameShareMem_tag{
	int key;				//共享内存key
	int shmID;				//共享内存id
	void *shm_addr;			//共享内存地址
	int memSize;			//分块内存大小
	int memCount;			//块数
}frameShareMem_t;
static frameShareMem_t gFrameShareMem;

/*共享内存初始化*/
int shareMemInit(int key, int memSize, int memCount)
{
	int shmID = -1;

	if((!memSize)||(!memCount))
	{
		return -1;
	}
	shmID = shmget(key,(memSize*memCount),IPC_CREAT | 0666);
	if(shmID < 0 ){
		shmID = shmget(key, 0, 0);
	}
	
	if(shmID < 0 )
		return -1;
	return shmID;
}

/*从共享内存中指定的偏移量处写数据*/
void shareMemWrite(int mid,int offset,void *buf,int length)
{
	char *pDst = shmat(mid,0,0);
	if(pDst == NULL)
		return;
	memcpy(pDst+offset,buf,length);
	shmdt(pDst);
}

/*从共享内存中指定的偏移量处读取数据*/
void ShareMemRead(int mid,int offset,void *buf,int length)
{
	char *pSrc = shmat(mid,0,0);
	if(pSrc == NULL)
		return;
	memcpy(buf,pSrc+offset,length);
	shmdt(pSrc);
}

int frameShareMemInit(size_t size,size_t count)
{
	int shmID = -1;
	
	shmID = shareMemInit(FRAME_SHM_KEY,size,count);
	if(shmID > 0)
	{
		gFrameShareMem.key 		= FRAME_SHM_KEY;
		gFrameShareMem.shmID 	= shmID;
		gFrameShareMem.memSize 	= size;
		gFrameShareMem.memCount = count;
		return shmID;
	}
	else
	{
		printf("frameShareMemInit fail\n");
		return -1;
	}
}

void frameShareMemWrite(int index, uint8_t *yuv_frame, size_t yuv_length)
{
	if(index < gFrameShareMem.memCount)
	{
		shareMemWrite(gFrameShareMem.shmID,index*gFrameShareMem.memSize,yuv_frame,yuv_length);
	}
}

