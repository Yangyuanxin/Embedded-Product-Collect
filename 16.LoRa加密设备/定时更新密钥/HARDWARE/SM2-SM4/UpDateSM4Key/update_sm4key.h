#ifndef __UPDATE_SM4KEY_H
#define	__UPDATE_SM4KEY_H


#include "sys.h"
#include "string.h"
#include "modules_init.h"

#define		UPDATE_SM4_TIME_VALUE		30000	//更新时间1个小时


extern u8 UpdateSM4KeyFlag; 


void	RunCalcOneHourCounter(void);
void 	TheTimeUptoUpdateSM4KeyHandle(void);
void 	DisableDMA1Channelx(u8 _which_uart);
void 	EnableDMA1Channelx(u8 _which_uart);
void 	HostComSoftwareReset(u8 _which_uart);
void 	LoRaSoftWareParaReset(void);
void 	ResendGetSlaveIdCmdProcess(void);

#endif