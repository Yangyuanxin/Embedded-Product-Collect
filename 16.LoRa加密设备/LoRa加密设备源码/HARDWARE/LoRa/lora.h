#ifndef __LORA_H
#define	__LORA_H

#include "sys.h"
#include "timer.h"
#include "lora_pwr.h"
#include "MasterLoRaHandle.h"
/***************************/
//ZM470SX1276模块相关头文件
#include "radio.h"
#include "platform.h"
#include "sx1276.h"
#include "sx1276-Hal.h"
#include "sx1276-LoRa.h"
#include "sx1276-LoRaMisc.h"
/****************************/
#include "my_sm2.h"
#include "MyApiPath.h"
#include "hostcomprotocol.h"
#include "com_status_key_init.h"


//定义枚举类型(LoRa状态)
typedef enum ST
{
	Tx_ST,
	Rx_ST,
	Other
}_ST;

extern u16  LoRaTransCounter;


void 	MyLoRaInit(void);
void 	SetLoRaStandBy(void);
void 	SetLoRaSleep(void);
void 	ReceiveData(u8 *LoRaBuf,u8 *RevCnt);
void 	RunLoRaTransCounter(void);
void 	RunWaitIDCounter(void);

#endif





