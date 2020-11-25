#include "marlin_main.h"
#include "cardreader.h"	 
#include "sdio_sdcard.h"
#include "usart.h"
#include "exfuns.h"
#include "malloc.h"		  
#include "string.h"

CardReader card;
//unsigned long starttime=0;
//unsigned long stoptime=0;
bool autostart_stilltocheck = true; //the sd start is delayed, because otherwise the serial cannot answer fast enought to make contact with the hostsoftware.
bool autostart_atmillis = 0;

#ifdef SDSUPPORT

void gcode_ls(u8 * path)
{
	u8 res;	  
  u8 *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = mymalloc(fileinfo.lfsize);
#endif		  
  res = f_opendir(&dir,(const TCHAR*)path);
  if (res == FR_OK) 
	{	
		while(1)
		{
	      res = f_readdir(&dir, &fileinfo);               
	      if (res != FR_OK || fileinfo.fname[0] == 0) break;        
        #if _USE_LFN
        fn =(u8*)( *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname);
        #else							   
        fn =(u8*)( fileinfo.fname);
        #endif	                                         
	 	    res=f_typetell(fn);	
			  if((res&0XF0)==0X70)
		  	{ 
					printf("%s\n",  fn);
			  }
		} 
  }	  
	myfree(fileinfo.lfname);	  
}

void card_ls(void) //SD卡目录下的文件列表（ls是list的缩写）
{ 
  if(card.lsAction==LS_Count)
  card.lsAction=LS_SerialPrint;
  gcode_ls("0:\\GCODE");
}

void card_initsd(void)  //初始化sd卡
{ 
	card.cardOK = false;
	autostart_atmillis=millis()+5000;
  SERIAL_ECHO_START;
  if(SD_Init())	//SD_Init()返回值为0表示初始化成功
	{ 
		printf("SD card init fail");	
	}
	else 
	{ 	
		card.cardOK = true;
		printf("SD card ok");
	}
}
void card_release(void)  //释放SD卡
{ 
	card.sdprinting = false;
  card.cardOK = false;
}
void card_openFile(char* fname,bool read)  //打开文件 
{
  u8 res;
  u8 *pname;		
  pname=mymalloc(_MAX_LFN*2+1);
  if(!card.cardOK)
    return;
 // file.close();
// printf("--");
  card.sdprinting = false;

  if(read)
  {	
		strcpy((char*)pname,"0:/GCODE/");
		strcat((char*)pname,(const char*)fname); 
		res=f_open(&card.fgcode,(const TCHAR*)pname,FA_READ);
			if (res==FR_OK)
			{
				printf("File opened: ");
				printf(fname);
				printf(" Size: ");
				printf("%ld",f_size(&card.fgcode));
				printf("\n");
	//      card.sdpos = 0;    
				printf("File selected");
				//lcd_setstatus(fname);
				//LCD12864_ShowString(1,0,"成功");  //用于测试
			}
			else
			{
				//LCD12864_ShowString(1,0,"失败");  //用于测试
				printf("open failed, File: ");
				printf(fname);
				printf(".");
			}
		printf("\n");
  }
  else 
  { 
		res=f_open(&card.fgcode,(const TCHAR*)fname,FA_WRITE|FA_OPEN_ALWAYS);
    if(res!=FR_OK)
    {
      printf("open failed, File: ");
      printf(fname);
      printf(".");
    }
    else
    {
      card.saving = true;
      printf("error writing to file");
      printf(fname);
     // lcd_setstatus(fname);
    }
	 printf("\n");
  }
   myfree(pname);		
  //myfree(SRAMIN,fgcode);
}

void card_removeFile(char* fname)
{  
	u8 res;
  res = f_unlink(fname);
  if (res==FR_OK) 
    {
      printf("File deleted:");
      printf(fname);
//      card.sdpos = 0;
    }
    else
    {
      printf("Deletion failed, File: ");
      printf(fname);
      printf(".\n");
    }
}

void card_startFileprint(void)
{
  if(card.cardOK)
  {
    card.sdprinting = true; 
  }

}

void card_pauseSDPrint(void)
{
  if(card.sdprinting)
  {
    card.sdprinting = false;
  }
}

void card_setIndex(long index)
{
	//card.sdpos = index;
 f_lseek(&card.fgcode,index);
}

void card_getStatus(void)
{  
	if(card.cardOK)
  {
    printf("SD printing byte ");
    printf("%d",f_tell(&card.fgcode));
    printf("/");
    printf("%d",f_size(&card.fgcode));
  }
  else{
    printf("Not SD printing");
  }
  printf("\n");
}

uint8_t card_percentDone()
{
//	if(!isFileOpen()) 
//	  return 0; 
	card.sdpos = f_tell(&card.fgcode);
	card.filesize = f_size(&card.fgcode); 
	if(card.filesize)
	  return card.sdpos/((card.filesize+99)/100); 
	else 
	  return 0;
}

void card_closefile(void)
{ 
	f_close(&card.fgcode);
  card.saving = false; 
}

void card_write_command(char *buf)
{ 
}

void card_checkautostart(bool force)
{
	if(!force)
  {
    if(!autostart_stilltocheck)
      return;
    if(autostart_atmillis<millis())
      return;
  }
  autostart_stilltocheck=false;
  if((!card.cardOK)&&(!SD_CD))
  {
    card_initsd();
    if(!card.cardOK) //fail
    return;
  }
  
}
void card_printingHasFinished(void)
{
    st_synchronize();
    quickStop();
  card_closefile();
	starttime=0;
	card.sdprinting = false;
    if(SD_FINISHED_STEPPERRELEASE)
    {
      //  finishAndDisableSteppers();
        enquecommand(PSTR(SD_FINISHED_RELEASECOMMAND));
    }
    autotempShutdown();
//	 printf("5.\r\n");
}

bool card_eof(void)
{ 
	return f_eof(&card.fgcode);
}

int16_t card_get(void) 
{ //card.sdpos = f_tell(&card.fgcode);
  return  file_read();
}

int16_t file_read(void)
{ 
	u8 buffer[128]; 
	u8 res;
	UINT br;
	res = f_read(&card.fgcode, buffer, 1, &br);
	if (res == FR_OK )
	{// printf("%s",buffer);
	  return *buffer;//
	}else
	{ 
		return -1;// printf("f_read() fail .. \r\n");  
	}
}
#endif //SDSUPPORT

