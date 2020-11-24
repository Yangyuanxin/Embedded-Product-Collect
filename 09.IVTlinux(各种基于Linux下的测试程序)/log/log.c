#include "log.h"

#define LOG_DIR 			"/opt/ivt/logs/"
#define MAX_DAYS 			31
#define MAX_STR_LEN			2048

typedef struct log_struct{
	char logdir[128];
	char logname[32];
	FILE *logfp;
	unsigned long int maxlogsize;
	pthread_mutex_t logfile_mutex;
}log_t;

typedef enum {
	DEBUG_LOG = 0,
	INFO_LOG,
	WARN_LOG,
	ERR_LOG
}log_level_t;

static log_t glog[] = {
    {LOG_DIR,"debuglog",NULL,(16*1024*1024),PTHREAD_MUTEX_INITIALIZER},
    {LOG_DIR,"infolog",NULL,(8*1024*1024),PTHREAD_MUTEX_INITIALIZER},
	{LOG_DIR,"warnlog",NULL,(1*1024*1024),PTHREAD_MUTEX_INITIALIZER},
	{LOG_DIR,"errlog",NULL,(2*1024*1024),PTHREAD_MUTEX_INITIALIZER},
}; 

/*计算日志文件的字节数*/
unsigned long int getLogVolume(log_level_t logLevel)
{
    char fileName[256] ={0};
    struct stat fileStat;
    unsigned long int diskUse=0;
	unsigned char day = 1;

	for(day = 1 ; day <= MAX_DAYS; day++)
	{
		sprintf(fileName,"%s%s_day%02d.txt", glog[logLevel].logdir,glog[logLevel].logname,day);
    	if (stat(fileName, &fileStat) == 0)
    	{
        	diskUse += ((unsigned long)fileStat.st_size);
		}
	}	
    //printf("\n日志%s%s占用%lu字节\n", glog[logLevel].logdir,glog[logLevel].logname,diskUse);
    return diskUse;
}

//核对所有日志占用空间，并删除最早日期的日志文件
void checkLogVolume(unsigned char noday , log_level_t logLevel)
{
    unsigned char day = (bcdtoint(noday))%(MAX_DAYS + 1);
    char fileName[256] = {0};
	int i = 0;

    while(getLogVolume(logLevel)>(glog[logLevel].maxlogsize)) //判断日志空间的大小
    {
        day = ((bcdtoint(noday) + i)%MAX_DAYS) + 1;//删除较早的日志 
        sprintf(fileName,"%s%s_day%02d.txt", glog[logLevel].logdir,glog[logLevel].logname,day);
        if(remove(fileName) == 0)
        {
            printf("日志空间太大, 删除%s\n", fileName);
        }
        i++;
        if(i >= MAX_DAYS)
        {
            break;
        }
    }
    //printf("===== checklogvolume end  ===\n");
}

int logfile_open(log_level_t logLevel)
{
    char fileday[128] = {0};
    char filename[512] = {0};

	unit_tradetime nowtime;
	getfulltime(&nowtime);
    //printf("[log] into log_record\n");
    checkLogVolume(nowtime.date.day , logLevel);
    memset(filename,0,sizeof(filename));
    strcpy(filename,glog[logLevel].logdir);
    sprintf(fileday,"%s_day%02x.txt",glog[logLevel].logname,nowtime.date.day);
    strcat(filename,fileday);
    //printf("record log in %s\n",filename);

	if(access(glog[logLevel].logdir, F_OK) == 0)   //文件目录存在
	{
    	glog[logLevel].logfp = fopen(filename,"ab+");
		//printf("fopen record log in %s\n",filename);
    	if(glog[logLevel].logfp == NULL)
   		{
        	return 0;
   		}
	}	
	else
	{
		if(!mkdir(glog[logLevel].logdir , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))//创建
		{
			//printf("mkdir log in %s\n",filename);
			glog[logLevel].logfp = fopen(filename,"ab+");
			//printf("create log in %s\n",filename);
    		if(glog[logLevel].logfp == NULL)
   			{
        		return 0;
   			}
		}
		else
		{
			return 0;	
		}
	}

	if(glog[logLevel].logfp == NULL)
   	{
   		return 0;
   	}

    fprintf(glog[logLevel].logfp,"[%.2x-",nowtime.date.month);
    fprintf(glog[logLevel].logfp,"%.2x ",nowtime.date.day);
    fprintf(glog[logLevel].logfp,"%.2x:",nowtime.hour);
	fprintf(glog[logLevel].logfp,"%.2x:",nowtime.minute);
	fprintf(glog[logLevel].logfp,"%.2x] ",nowtime.second);
	
	return 1;
}

void writelogfile(log_level_t logLevel,const char * strInfo)
{
	if(!strInfo)
        return;
	pthread_mutex_lock(&(glog[logLevel].logfile_mutex));
	if(logfile_open(logLevel))
	{
		fprintf(glog[logLevel].logfp,"%s",strInfo);
	}
	if(glog[logLevel].logfp)
	{
		fclose(glog[logLevel].logfp);
	}
	pthread_mutex_unlock(&(glog[logLevel].logfile_mutex));
}

void logdebug(const char * strInfo, ...)
{
    if(!strInfo)
        return;
    char pTemp[MAX_STR_LEN] = {0};
    
    //获取可变形参
    va_list arg_ptr;
    va_start(arg_ptr, strInfo);
    vsprintf(pTemp + strlen(pTemp), strInfo, arg_ptr);
    va_end(arg_ptr);
    //写日志文件
    //printf("logdebug = %s\n" , pTemp);
    writelogfile(DEBUG_LOG ,pTemp);
}

void logerror(const char * strInfo, ...)
{
    if(!strInfo)
        return;
    char pTemp[MAX_STR_LEN] = {0};
    
    //获取可变形参
    va_list arg_ptr;
    va_start(arg_ptr, strInfo);
    vsprintf(pTemp + strlen(pTemp), strInfo, arg_ptr);
    va_end(arg_ptr);
    //写日志文件
    //printf("logerror = %s\n" , pTemp);
    writelogfile(ERR_LOG ,pTemp);
}

void logwarn(const char * strInfo, ...)
{
    if(!strInfo)
        return;
    char pTemp[MAX_STR_LEN] = {0};
    
    //获取可变形参
    va_list arg_ptr;
    va_start(arg_ptr, strInfo);
    vsprintf(pTemp + strlen(pTemp), strInfo, arg_ptr);
    va_end(arg_ptr);
    //写日志文件
    //printf("logwarn = %s\n" , pTemp);
    writelogfile(WARN_LOG ,pTemp);
}

void loginfo(const char * strInfo, ...)
{
    if(!strInfo)
        return;
    char pTemp[MAX_STR_LEN] = {0};
    
    //获取可变形参
    va_list arg_ptr;
    va_start(arg_ptr, strInfo);
    vsprintf(pTemp + strlen(pTemp), strInfo, arg_ptr);
    va_end(arg_ptr);
    //写日志文件
    //printf("loginfo = %s\n" , pTemp);
    writelogfile(INFO_LOG ,pTemp);
}


#if 0
int main()
{
	unit_tradetime nowtime;
	getfulltime(&nowtime);

	printf("abcd");
	logdebug("abcd\n");
	printf("%s = %d %s = %d\n" , "abcd" , 123456,"444444444444" , 1111111);
	logdebug("%s = %d %s = %d\n","abcd" , 123456,"444444444444" , 1111111);

	printf("close %s fail %s return %d\n" , glog[ERR_LOG].logdir,glog[ERR_LOG].logname  , -2);
	logerror("close %s fail %s return %d\n" , glog[ERR_LOG].logdir,glog[ERR_LOG].logname , -2);

	printf("%s_day%d.txt",glog[DEBUG_LOG].logname,nowtime.date.day);
	logdebug("%s_day%d.txt\n",glog[DEBUG_LOG].logname,nowtime.date.day);
	logerror("%s_day%d.txt\n",glog[DEBUG_LOG].logname,nowtime.date.day);
	
	return 0;
}
#endif

