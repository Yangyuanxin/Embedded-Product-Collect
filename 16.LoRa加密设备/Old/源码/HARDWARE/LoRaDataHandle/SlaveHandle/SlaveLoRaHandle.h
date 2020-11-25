#ifndef __SLAVELORAHANDLE
#define __SLAVELORAHANDLE

#include "string.h"
#include "sys.h"
#include "updata_crc.h"
#include "delay.h"
#include "usart.h"
#include "mcu_id.h"
#include "led.h"
#include "sx1276.h"
#include "lora.h"
#include "dip_switch.h"
#include "modules_init.h"

void 	SlaveHandleSm2KeyAgreement(void);
void 	HandleSlaveLoRaProtocol(u8 *data,u16 datalen);
void 	LoRaSlaveMainPorcess(void);

#endif



