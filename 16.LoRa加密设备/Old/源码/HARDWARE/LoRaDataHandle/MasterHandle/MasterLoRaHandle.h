#ifndef __MASTERLORAHANDLE_H
#define __MASTERLORAHANDLE_H

#include "string.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "relay_init.h"
#include "sx1276.h"
#include "my_sm2.h"
#include "xLog.h"
#include "modules_init.h"

#define			LORA_RX_SIZE		255
#define			LORA_TX_SIZE		255
#define 		INFO_SAVE_ADDR		0x0800F000 		//Flash存储信息地址
#define			LORA_PACK_SIZE		224


//定义单灯控制器结构体
typedef struct
{
	u32 DeviceAddr;		//LoRa设备地址
	u32 DstDeviceAddr;	//目标主机地址
	u8 	*RxBuf;			//接收缓冲区地址
	u8 	RxCnt;			//接收计数器
	u8 	*TxBuf;			//发送缓冲区地址
	u16 TxCnt;			//发送计数器
} LoRaStruct;	
	
extern LoRaStruct	LoRaPact;	//声明结构体
extern u8 	EncryptedData[1100];//保存加密后的数据
extern u8 	PackSum;			//总的包数
extern u16 	AfterEncryptSzie;	//加密后的数据长度
extern u16 	PackSumSize;		//总的数据包长度
extern u8 	CountPackNum;		//数据包计数


void 	SetLoRaStruPara(void);
void 	HandleLoRaProto(u8 *data,u16 datalen);
void 	MasterHandleSm2KeyAgreement(void);
void 	LoRaMasterMainProcess(void);

#endif



