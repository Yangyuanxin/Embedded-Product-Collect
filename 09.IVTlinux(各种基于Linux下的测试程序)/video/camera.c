#include "camera.h"
#include "video.h"
#include "sharemem.h"

//char yuv_file_name[128] = "ivt.yuv\0";
//FILE *yuv_fp = NULL;

/*set video capture ways(mmap)*/
int init_mmap(cameraObject_t* cameraObject)
{
	int n_buffer = 0;
	/*to request frame cache, contain requested counts*/
	struct v4l2_requestbuffers reqbufs;

	if(!cameraObject)
	{
		exit(EXIT_FAILURE);
	}
	memset(&reqbufs, 0, sizeof(reqbufs));
	reqbufs.count = BUFFER_COUNT; 	 							/*the number of buffer*/
	reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;    
	reqbufs.memory = V4L2_MEMORY_MMAP;				

	if(-1 == ioctl(cameraObject->fdCam,VIDIOC_REQBUFS,&reqbufs))
	{
		perror("Fail to ioctl 'VIDIOC_REQBUFS'");
		exit(EXIT_FAILURE);
	}

	memcpy(&(cameraObject->reqbuf),&reqbufs,sizeof(reqbufs));
	//usr_buf = calloc(reqbufs.count, sizeof(usr_buf));
	cameraObject->framebuf = calloc(reqbufs.count, sizeof(frameBuf_t));
	if(cameraObject->framebuf == NULL)
	{
		printf("Out of memory\n");
		exit(-1);
	}

	/*map kernel cache to user process*/
	for(n_buffer = 0; n_buffer < reqbufs.count; ++n_buffer)
	{
		//stand for a frame
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffer;
		
		/*check the information of the kernel cache requested*/
		if(-1 == ioctl(cameraObject->fdCam,VIDIOC_QUERYBUF,&buf))
		{
			perror("Fail to ioctl : VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}

		cameraObject->framebuf[n_buffer].length = buf.length;
		cameraObject->framebuf[n_buffer].start = (char *)mmap(NULL,buf.length,PROT_READ | PROT_WRITE,MAP_SHARED, cameraObject->fdCam,buf.m.offset);

		if(MAP_FAILED == cameraObject->framebuf[n_buffer].start)
		{
			perror("Fail to mmap");
			exit(EXIT_FAILURE);
		}

	}
	//yuv_fp = fopen(yuv_file_name, "wa+");
	return (EXIT_SUCCESS);
}

int open_camera(char *videoDev)
{
	int fd;
	struct v4l2_input inp;

	fd = open(videoDev, O_RDWR | O_NONBLOCK,0);
	if(fd < 0)
	{	
		fprintf(stderr, "%s open err \n", videoDev);
		exit(EXIT_FAILURE);
	}

	inp.index = 0;
	if (-1 == ioctl (fd, VIDIOC_S_INPUT, &inp))
	{
		fprintf(stderr, "VIDIOC_S_INPUT \n");
	}

	return fd;
}

int init_camera(cameraObject_t* cameraObject)
{
	struct v4l2_capability 	cap;		/* decive fuction, such as video input */
	struct v4l2_format 		tv_fmt;		/* frame format */  
	struct v4l2_fmtdesc 	fmtdesc;  	/* detail control value */
	//struct v4l2_control 	ctrl;
	int ret = -1;
	
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.index 	= 0 ;                	/* the number to check */
	fmtdesc.type 	= V4L2_BUF_TYPE_VIDEO_CAPTURE;

	/* check video decive driver capability */
	if((ret=ioctl(cameraObject->fdCam, VIDIOC_QUERYCAP, &cap))<0)
	{
		fprintf(stderr, "fail to ioctl VIDEO_QUERYCAP \n");
		exit(EXIT_FAILURE);
	}
	
	/*judge wherher or not to be a video-get device*/
	if(!(cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE))
	{
		fprintf(stderr, "The Current device is not a video capture device \n");
		exit(EXIT_FAILURE);
	}

	/*judge whether or not to supply the form of video stream*/
	if(!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		printf("The Current device does not support streaming i/o\n");
		exit(EXIT_FAILURE);
	}
	
	printf("\ncamera driver name is : %s\n",cap.driver);
	printf("camera device name is : %s\n",cap.card);
	printf("camera bus information: %s\n",cap.bus_info);
#if 0
	/*show all the support format*/
	printf("\n");
	while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
	{	
		printf("support device %d.%s\n",fmtdesc.index+1,fmtdesc.description);
		fmtdesc.index++;
	}
	printf("\n");
#endif
	/*set the form of camera capture data*/
	tv_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;      /*v4l2_buf_typea,camera must use V4L2_BUF_TYPE_VIDEO_CAPTURE*/
	tv_fmt.fmt.pix.width = cameraObject->cameraPara.width;
	tv_fmt.fmt.pix.height = cameraObject->cameraPara.hight;
	//tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	//tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420;	/*V4L2_PIX_FMT_YYUV*/
	tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;	/*V4L2_PIX_FMT_YYUV*/
	//tv_fmt.fmt.pix.field = V4L2_FIELD_NONE;   		/*V4L2_FIELD_NONE*/
	tv_fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;   	/*V4L2_FIELD_NONE*/
	if (ioctl(cameraObject->fdCam, VIDIOC_S_FMT, &tv_fmt)< 0) 
	{
		fprintf(stderr,"VIDIOC_S_FMT set err\n");
		exit(-1);
		close(cameraObject->fdCam);
		exit(EXIT_FAILURE);
	}

	return(init_mmap(cameraObject));
}

int start_capture(cameraObject_t* cameraObject)
{
	unsigned int i;
	enum v4l2_buf_type type;
	
	/*place the kernel cache to a queue*/
	for(i = 0; i < cameraObject->reqbuf.count; i++)
	{
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if(-1 == ioctl(cameraObject->fdCam, VIDIOC_QBUF, &buf))
		{
			perror("Fail to ioctl 'VIDIOC_QBUF'");
			exit(EXIT_FAILURE);
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(cameraObject->fdCam, VIDIOC_STREAMON, &type))
	{
		//printf("i=%d.\n", i);
		perror("VIDIOC_STREAMON");
		close(cameraObject->fdCam);
		exit(EXIT_FAILURE);
	}

	return 0;
}	

void stop_capture(cameraObject_t* cameraObject)
{
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(-1 == ioctl(cameraObject->fdCam,VIDIOC_STREAMOFF,&type))
	{
		perror("Fail to ioctl 'VIDIOC_STREAMOFF'");
		exit(EXIT_FAILURE);
	}
}

int close_camera_device(cameraObject_t* cameraObject)
{
	unsigned int i;

	for(i = 0;i < cameraObject->reqbuf.count; i++)
	{
		if(-1 == munmap(cameraObject->framebuf[i].start,cameraObject->framebuf[i].length))
		{
			exit(-1);
		}
	}

	free(cameraObject->framebuf);
	if(-1 == close(cameraObject->fdCam))
	{
		perror("Fail to close fd");
		exit(EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int save_frame(uint8_t *yuv_frame, size_t yuv_length , FILE *yuv_fp) 
{
	if(yuv_fp && yuv_frame && yuv_length)
	{
		return (fwrite(yuv_frame, yuv_length, 1, yuv_fp));
		printf("save_frame yuv_length = %d\n",yuv_length);	
		return yuv_length;
	}
	return 0;
}

void updata_frame(int index,uint8_t *yuv_frame, size_t yuv_length)
{
	if(yuv_frame&&yuv_length)
	{
		frameShareMemWrite(index,yuv_frame,yuv_length);
	}
	return ;
}

int read_frame(cameraObject_t* cameraObject)
{
	struct v4l2_buffer buf;
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	//put cache from queue
	if(-1 == ioctl(cameraObject->fdCam, VIDIOC_DQBUF,&buf))
	{
		//perror("Fail to ioctl 'VIDIOC_DQBUF'");
		exit(EXIT_FAILURE);
	}
	assert(buf.index < cameraObject->reqbuf.count);
	updata_frame(cameraObject->index,cameraObject->framebuf[buf.index].start, buf.length);
	if(-1 == ioctl(cameraObject->fdCam, VIDIOC_QBUF,&buf))
	{
		perror("Fail to ioctl 'VIDIOC_QBUF'");
		exit(EXIT_FAILURE);
	}
	return 1;
}

