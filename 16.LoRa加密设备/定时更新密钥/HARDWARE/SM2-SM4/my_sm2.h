#ifndef __MY_SM2_H
#define __MY_SM2_H

#include "string.h"
#include "sys.h"
#include "hsc32i1.h"
//#include "logs.h"
#include "xLog.h"
#include "dip_switch.h"
/*************************************************
				内部寄存器地址划分
			   ---------------------
	0	--	加密算法(10Bytes)
	1	--	自身随机秘钥(200Bytes)
	21	--	解析加密秘钥(200Bytes)
	41	--	加密源数据,明文(400bytes)
	81	--	加密后数据,密文(400Bytes)
	121	--	解密源数据,密文(400Bytes)
	161	--	解密后数据,明文(400Bytes)
**************************************************/

//加密协议地址划分
#define   	SM4_KEYADDR					0		//SM4密钥
#define		AID_ADDR					2		//本方标识符
#define		BID_ADDR					4		//对方标识符
#define		BEFORE_ENCRYPTED_ADDR		6		//加密前的数据
#define		AFTER_ENCRYPTED_ADDR		32		//加密后的数据
#define		BEFORE_DECRYPTED_ADDR		58		//解密前的数据
#define		AFTER_DECRYPTED_ADDR		84		//解密后的数据
#define		WHO_IS_SEND_REV_ADDR		110		//判断发送方还是接收方

typedef enum
{
	Agreement_A = 0x30,
	Agreement_B,
	Agreement_None
}_SM2_Agreement_x;


void 	HSC32i1Init(u8 *id_buf,u16 id_size);
void 	GetSM4Key(u8 *key_buf,u8 key_size);
void 	GetTheAID(u8 *AID_Buf,u16 AID_Size);
void 	SetTheBID(u8 *BID_Buf,u16 BID_Size);
void 	EncryptTheText(u8 *Buf,u16 TextSize);
void 	GetEncryptedText(u8 *Buf,u16 TextSize);
void 	DecryptTheText(u8 *Buf,u16 TextSize);
void 	GetDecryptedText(u8 *Buf,u16 TextSize);
void 	SetSM2AgreementProtocol(void);
void 	SendIdKeyInfo(u8 *buf,u8 size);

#endif




