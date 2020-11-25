#ifndef __SLAVELORAHANDLE
#define __SLAVELORAHANDLE

#include "MyApiPath.h"
#include "modules_init.h"
#include "LoRaProtocolCmd.h"
#include "update_sm4key.h"


void 	LoRaSlaveHandleSm2KeyAgreement(void);
void 	HandleLoRaCommunicateProtocol(u8 *data,u16 datalen);
void 	LoRaNodeHandleCommunicateData(void);

#endif



