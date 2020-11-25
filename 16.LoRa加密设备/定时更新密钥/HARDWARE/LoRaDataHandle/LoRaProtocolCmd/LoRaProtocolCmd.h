#ifndef __LORAPROTOCOLCMD_H
#define __LORAPROTOCOLCMD_H

#include "modules_init.h"


void 	LoRaCtrlNetLedCmd(u8 *data,u16 datalen);			//控制LED应答
void 	LoRaCtrlNetLedCmdACK(u8 *data,u16 datalen);			//控制LED应答
void 	LoRaRequestIDIDCmd(u8 *data,u16 datalen);			//获取ID指令
void 	LoRaRequestIDACK(u8 *data,u16 datalen);				//成功接收ID应答
void 	LoRaTimeSynchronizationCmd(u8 *data,u16 datalen);	//时间同步指令
void 	LoRaBeforeSendEncryptedData(u8 *data,u16 datalen);	//发送数据包信息指令
void 	LoRaSendEncryptedData(u8 *data,u16 datalen);		//发送加密数据指令
void 	LoRaRevEncryptdDataACK(u8 *data,u16 datalen);		//接收完成加密数据应答
void 	SendTheDecryptedData(u8 *buf,u16 datalen);			//串口打印解密数据
void 	LoRaRequestUpdateSM4KeyCmd(u8 *data,u16 datalen);	//更新SM4密钥指令
void 	LoRaRequestUpdateSM4KeyACK(u8 *data,u16 datalen);	//更新密钥应答

#endif