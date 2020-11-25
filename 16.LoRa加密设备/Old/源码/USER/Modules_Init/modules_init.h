#ifndef __MODULES_INIT_H
#define __MODULES_INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "relay_init.h"
#include "dip_switch.h"
#include "com_status_key_init.h"
#include "hsc32_rst_io.h"
#include "advance_timer.h"
#include "ordinary_timer.h"
#include "my_delay.h"
#include "rtc.h"
#include "w25qxx_flash.h"
#include "lora.h"
#include "my_sm2.h"
#include "hostcomprotocol.h"
#include "xLog.h"
#include "LoRaDeviceTask.h"
#include "MasterLoRaHandle.h"
#include "SlaveLoRaHandle.h"


#define 	True			1
#define		False			0


typedef enum
{
	MASTER = 0X7D,
	SLAVE  = 0X7E,
	NoneLoRaDevice
}_LoRaDeviceIs;


void 	ModulesInit(void);

#endif




