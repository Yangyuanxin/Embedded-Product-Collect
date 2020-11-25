#ifndef __CARDREADER_H
#define __CARDREADER_H

#include "sys.h"
#include "ff.h"

#define SD_FINISHED_STEPPERRELEASE true  //if sd support and the file is finished: disable steppers?
#define SD_FINISHED_RELEASECOMMAND "M84 X Y Z E" // You might want to keep the z enabled so your bed stays in place.


typedef enum {
   LS_SerialPrint = 0,
   LS_Count,
   LS_GetFilename
 }LsAction;

typedef struct {
 bool saving;
 bool sdprinting;	
 bool cardOK ;
  //int16_t n;
 unsigned long autostart_atmillis;
 unsigned long  sdpos; 
 unsigned long filesize;	
 bool autostart_stilltocheck;
 LsAction lsAction; //stored for recursion.
 u8 filename[32];
 FIL fgcode;
}CardReader;

extern CardReader card;

void card_ls(void);
void card_initsd(void);
void card_release(void);						//释放SD卡
void card_openFile(char* fname,bool read);
void card_startFileprint(void);
void card_pauseSDPrint(void);
void card_setIndex(long index);	  //设置文件位置
void card_getStatus(void);  //获取SD打印状态
void card_closefile(void);
void card_removeFile(char* fname);

uint8_t card_percentDone(void);

void card_printingHasFinished(void);
void card_checkautostart(bool force);
void card_write_command(char *buf);

bool card_eof(void);
int16_t card_get(void);

int16_t file_read(void);


#endif //__CARDREADER_H

