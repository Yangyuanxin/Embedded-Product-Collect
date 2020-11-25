/*************************************************************************
	* 文件描述：此文件是对数据的加解密处理，操作对象为加密芯片；
	* 作者：	ZWQ
	* 时间：	2019-1-4
	* 备注：	API还需修改
**************************************************************************/
#include "my_sm2.h"

static u8 Send[2] = {0x30,};
static u8 Rev[2]  = {0x31,};

/*************************************************
	* 功能描述：HSC32基本初始化以及
				sm2密钥协商初始化
	* 入口参数：id_buf  -- 标识符地址
				id_size -- 读取数据包的长度
	* 返回值：	无
	* 备注：	标识符暂定为64B
**************************************************/
void HSC32i1Init(u8 *id_buf,u16 id_size)
{
	HSC32I1BaseInit();
	delay_ms(10);
	SetSM2AgreementProtocol();
	delay_ms(10);
	GetTheAID(id_buf,id_size);
	
	//****1加密芯片自检
//	RestoreSystemLog(SystemLogBuf,HSC32_CHECK_CODE,Log_Successful);
	RestoreWhichLog(SystemLogFlag,SystemLogBuf,HSC32_CHECK_CODE,0,Log_Successful);
}


void GetSM4Key( u8 *key_buf,u8 key_size)
{
	//获取协商后的sm4密钥(暂定为64Bytes)
	HSC32I1_ReadLenByte(SM4_KEYADDR,key_buf,key_size);
}


void GetTheAID(u8 *AID_Buf,u16 AID_Size)
{
	//获取发送方的标识符(暂定为64Bytes)
	HSC32I1_ReadLenByte(AID_ADDR,AID_Buf,AID_Size);
}

 
void SetTheBID(u8 *BID_Buf,u16 BID_Size)
{
	//请求加密数据
	HSC32I1_WriteLenByte(BID_ADDR,BID_Buf,BID_Size);
}
 
void EncryptTheText(u8 *Buf,u16 TextSize)
{
	//请求加密数据
	HSC32I1_WriteLenByte(BEFORE_ENCRYPTED_ADDR,Buf,TextSize);
}


void GetEncryptedText(u8 *Buf,u16 TextSize)
{
	//获取加密后的数据
	HSC32I1_ReadLenByte(AFTER_ENCRYPTED_ADDR,Buf,TextSize);
}


void DecryptTheText(u8 *Buf,u16 TextSize)
{
	//请求加密数据
	HSC32I1_WriteLenByte(BEFORE_DECRYPTED_ADDR,Buf,TextSize);
}


void GetDecryptedText(u8 *Buf,u16 TextSize)
{
	//获取加密后的数据
	HSC32I1_ReadLenByte(AFTER_DECRYPTED_ADDR,Buf,TextSize);
}


/*******************************************
	* 功能描述：设置主从机的密钥协商协议
	* 入口参数：TheDeviceIs -- 设备属性
	* 返回值：	无
	* 备注：
********************************************/
void SetAgreementX(u8 TheDeviceIs)
{
	switch( TheDeviceIs )
	{
		//主机秘钥协商协议
		case MASTER:
			HSC32I1_WriteLenByte(WHO_IS_SEND_REV_ADDR,Send,1);
		break;
		
		//从机秘钥协商协议
		case SLAVE:
			HSC32I1_WriteLenByte(WHO_IS_SEND_REV_ADDR,Rev,1);
		break;
		
		default:
		break;
	}
	
}


/**********************************************
	* 功能描述：设置SM2协商函数接口类型
	* 入口参数：无
	* 返回值：	无
	* 备注：
***********************************************/
void SetSM2AgreementProtocol(void)
{
	switch( LoRaDeviceIs )
	{
		case MASTER:
			SetAgreementX(MASTER);
		break;
		
		case SLAVE:
			SetAgreementX(SLAVE);
		break;
		
		default:
		break;
	}
}



