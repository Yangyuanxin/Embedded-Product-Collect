#ifndef __UPDATE_SM4KEY_H
#define	__UPDATE_SM4KEY_H


#include "sys.h"
#include "modules_init.h"

#define		UPDATE_SM4_TIME_VALUE		40	//更新时间1个小时


extern u8 UpdateSM4KeyFlag; 


void	RunCalcOneHourCounter(void);
void 	DisableDMA1Channelx(u8 _which_uart);
void 	EnableDMA1Channelx(u8 _which_uart);
void 	TheTimeUptoUpdateSM4KeyHandle(void);
void 	HostComSoftwareReset(void);

#endif