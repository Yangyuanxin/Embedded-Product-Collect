#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
 
#define TBA_MCU PAin(0)  //PA0
#define TBB_MCU PAin(4)	 //PA4 
#define XWA_MCU PAin(5)	 //PA5
#define XWB_MCU PAin(6)	 //PA6
#define MCU_start PAin(7)//PA7
#define UP_MCU PAin(8)   //PA8

#define IR_UP PBin(8)    //PB8
#define IR_DOWN PBin(9)  //PB9



void KEY_Init(void);//IO≥ı ºªØ
					    
#endif
