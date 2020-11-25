#ifndef __RELAY_INIT_H
#define __RELAY_INIT_H

#include "sys.h"

#define  	RelayN			PBout(13)	  	//继电器负开关
#define  	RelayP			PBout(14)		//继电器正开关

#define		RelayOn()		{ RelayP=1; } 	//开启继电器
#define		RelayOff()		{ RelayP=0; } 	//关闭继电器

extern u16 RelayCounter;


void 	RelayInit(void);
void 	RunRelayCounter(void);

#endif