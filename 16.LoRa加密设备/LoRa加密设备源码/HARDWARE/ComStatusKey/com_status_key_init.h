#ifndef __COM_STATUS_KEY_INIT_H
#define __COM_STATUS_KEY_INIT_H

#include "sys.h"
#include "delay.h"
#include "led.h"
#include "hostcomprotocol.h"

#define		COM_STATUS_KEY				PCin(4)
#define		COM_STATUS_KEY_KEEP_TIME	2000	

typedef enum
{
	KEY_AT = 0x8a,
	KEY_NONE
}_KeyStatusNow;


void 	ComStatusChangeKeyInit(void);
void 	ScanComStatusKey(void);
void 	RunComStatusChangeTimer(void);
void 	IsComChangeToAT_WorkMode(void);

#endif