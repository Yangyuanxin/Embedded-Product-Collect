#ifndef __MENUCONTROL_H
#define __MENUCONTROL_H

#include "sys.h"
#include "touch.h"
#include "gui_conf.h"
#include "fontupd.h"
#include "fontshow.h"
#include "picdecode.h"
#include "button.h"
#include "filelistbox.h"
#include "flash.h"

#define U8_TYPE 0
#define U16_TYPE 1
#define U32_TYPE 2
#define INT_TYPE 3
#define FLOAT_TYPE 4
#define DOUBLE_TYPE 5


#define FONT_SIZE 16


#define USER_DATA_ADDR   (u32)5242880 //5*1024*1024 

//数值输入结构体.
__packed typedef struct 
{
	void *address;
	u8 type;
	double up_limit;
	double low_limit;
	bool judge;
	u32 store_address;
}_data_input;

#define ICOS_SD_CARD      	0			
#define ICOS_FLASH_DISK		1				
#define ICOS_LOCATION		0

typedef void (*menuFunc_t)();
extern menuFunc_t currentMenu;
extern bool redraw_screen;

void interface_update(void);
void main_interface(void);
void filebrowse_interface(void);
void picview_interface(void);
void print_interface(void);

#endif //__MENUCONTROL_H

