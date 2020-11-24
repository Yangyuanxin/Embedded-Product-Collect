#include "video.h"
#include "primary.h"
#include "sharemem.h"

#define MAX_VIDEO_CHANNELS		4

static devName_t gVideoName[] = {{"/dev/video1"},{"/dev/video2"},{"/dev/video3"},{"/dev/video4"},{"/dev/video5"}}; 
static cameraObject_t gCameraSelf[MAX_VIDEO_CHANNELS];

void initVideoConfig(int index,int width,int hight)
{
	gCameraSelf[index].index 			= index;
	gCameraSelf[index].cameraPara.width = width;
	gCameraSelf[index].cameraPara.hight = hight;
	memcpy(gCameraSelf[index].devName.string,gVideoName[index].string,sizeof(devName_t));

	return;
}

int videoModuleInit(int index,int width,int hight)
{
	int fd = -1;

	initVideoConfig(index,width,hight);
	fd = open_camera(gCameraSelf[index].devName.string);
	printf("videoModuleInit fd = %d\n",fd);
	if(fd > 0)
	{
		gCameraSelf[index].fdCam = fd;
		init_camera(&gCameraSelf[index]);
		start_capture(&gCameraSelf[index]);
		frameShareMemInit((width*hight*2),MAX_VIDEO_CHANNELS);
		return index;
	}	

	return -1;
}

void videoModuleThread(void* arg)
{
	int fd = -1 , index = -1;
	fd_set readfds;
	struct timeval tv;

	index = (int)arg;
	fd = gCameraSelf[index].fdCam;
	if(fd <= 0)
	{
		return;
	}
	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		
		tv.tv_sec = 0;
		tv.tv_usec = 35000;

		if(select(fd + 1,&readfds,NULL,NULL,&tv) > 0)
		{
			read_frame(&gCameraSelf[index]);
		}		
	}
}

int videoModuleRun()
{
	int index 			= 0;
	pthread_t pthreadID	= -1;
	
	index = videoModuleInit(0,640,480);
	if(index < 0)
	{
		return FAILURE;
	}
	moduleRegist(VIDEO_MODULE,CLI_VIDEO);
	if(pthread_create(&pthreadID, NULL, (void *)videoModuleThread, (void *)index))
	{
		printf("Failed to create init GpsModule thread!\n");	
		return FAILURE;
	}
	return SUCCESS;	
}

void videoModuleExit()
{
	int i = 0;

	for(i = 0 ; i < MAX_VIDEO_CHANNELS;i++)
	{
		if(gCameraSelf[i].fdCam > 0)
		{
			stop_capture(&gCameraSelf[i]);
			close_camera_device(&gCameraSelf[i]);
		}
	}
}

