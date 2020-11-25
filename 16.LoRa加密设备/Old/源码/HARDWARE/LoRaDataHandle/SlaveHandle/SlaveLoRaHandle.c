#include "SlaveLoRaHandle.h"



/**********************************************************************
						变量区
***********************************************************************/
	void LoRaCtrlRelayACK(u8 *data,u16 datalen);
	void LoRaRequestIDACK(u8 *data,u16 datalen);
	void LoRaRevEncryptdDataACK(u8 *data,u16 datalen);
	void SplitLoRaDataToDecryptedData(void);

	//----------------------------------------------------------
	extern LoRaStruct	LoRaPact;			//定义lora解析协议结构体
	extern u8	LoRaRxBuf[LORA_RX_SIZE];	//定义接收缓冲区
	extern u8	LoRaTxBuf[LORA_TX_SIZE];	//定义发送缓冲区
	extern _ST st;							//定义主机的状态枚举
	extern u16	LoRaTransCounter;			//传输计时器(收/发总时长)
	extern u16 	WaitIDCounter;				//等待对方ID计时器
	//------------------------------------------------
	extern u8 	SelfID_Buf[16];				//保存自身标识符
	extern u8 	OtherID_Buf[16];			//保存对方标识符
	extern u8 	SM4_KEY[16];				//SM4密钥
	extern u8 	GetBidStatus;				//得到对方标识符
	extern u8 	AgreeStatus;				//sm2密钥协商成功
	//------------------------------------------------
	extern u8 	EncryptedData[1100];		//保存加密后的数据
	extern u8 	DecryptedData[1100];		//保存解密后的数据
	extern u8 	PackSum;					//总的包数
	extern u8 	CountPackNum;				//数据包计数
	extern u16 	AfterEncryptSzie;			//加密后的数据长度
	extern u16 	PackSumSize ;				//总的数据包长度
/**********************************************************************/


/**********************************************
	* 功能描述：LoRa从机传输处理
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
***********************************************/
void LoRaSlaveMainPorcess(void)
{
	ReceiveData(LoRaPact.RxBuf, &LoRaPact.RxCnt);			//接收数据包
	HandleSlaveLoRaProtocol(LoRaPact.RxBuf,LoRaPact.RxCnt);	//解析数据包		
	//处理完毕
	if( LoRaPact.RxCnt )				
	{
		LoRaPact.RxCnt = 0;		//将接收计数器清零
		SX1276RxStateEnter();	//进入接收状态
	}
	
}


/***************************************************
	* 功能描述：	LoRa协议处理
	* 入口参数：	无
	* 返回值：		无
****************************************************/
void HandleSlaveLoRaProtocol(u8 *data,u16 datalen)
{
	u32 tempAddr = 0x00;	
	u16 tempCrc  = 0x00;

	//获取设备地址
	tempAddr = (data[2]<<24)|(data[3]<<16)|(data[4]<<8)|data[5];	

	//数据包长度为0
	if(!datalen)
		return;

	//判断设备地址是否一致
	if(tempAddr != LoRaPact.DeviceAddr)	
		return;

	//判断CRC校验是否通过
	tempCrc = CalCrc16(data,datalen-2,0xFFFF);				 
	if(tempCrc != ( (data[datalen - 2]<<8)|data[datalen-1]))
		return;

	//需要加入控制函数，控制单灯和雷达
	switch(data[0])
	{
		//LoRa通讯测试
		case 0x50:
			if(data[6] == 1)						//应答函数
				NET_LED = !NET_LED;					//改变NET灯的状态
			LoRaCtrlRelayACK(LoRaPact.TxBuf,0);		//应答
		break;
		
		//秘钥协商
		case 0x51:
			if( (data[6] == 'I')&&
				(data[7] == 'D') )
			{
				memcpy(OtherID_Buf,&LoRaPact.RxBuf[8],16);
				GetBidStatus = ENABLE;
				LoRaRequestIDACK(LoRaPact.TxBuf,0);
			}
		break;
		
		//数据传输前的1包
		case 0x5E:			
			PackSum 	= data[6];				//总的包数
			PackSumSize = (data[7]<<8)|data[8];	//总的数据包长度
		break;
		
		//接收加密后的透传数据
		case 0x5F:
			if(  PackSum <= 0 )
			{
				memcpy(EncryptedData,&data[8],data[1]-10);
				
				RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
				SplitLoRaDataToDecryptedData();
				
				PackSumSize = 0;
				LoRaRevEncryptdDataACK(LoRaPact.TxBuf,0);
			}
			else
			{
				if(CountPackNum <= PackSum)
				{
					memcpy(&EncryptedData[CountPackNum*224],&data[8],data[1]-10);
					CountPackNum ++;
				}
				//接收完成
				if(CountPackNum > PackSum)
				{
					RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
					SplitLoRaDataToDecryptedData();
					
					//拆包解密
					//存储解密日志
					PackSum = 0;
					PackSumSize = 0;
					CountPackNum = 0;
					LoRaRevEncryptdDataACK(LoRaPact.TxBuf,0);
				}
			}
		break;
		
		//其他
		default:
		break;
	}
	memset(LoRaPact.RxBuf,0,LORA_RX_SIZE);
}


/****************************************
	* 功能描述：SM2密钥协商
	* 入口参数：
	* 返回值：
	* 备注：	
*********************************************/
void SlaveHandleSm2KeyAgreement(void)
{
	//密钥协商成功则退出函数
	if(AgreeStatus != DISABLE)
		return ;
	
	if(GetBidStatus)
	{
		GetBidStatus = DISABLE;
		SetTheBID(OtherID_Buf,16);
		AgreeStatus = ENABLE;
		TimerDelay_Ms(500);
		HSC32I1_ReadLenByte(SM4_KEYADDR,SM4_KEY,16);
	}
}


//将串口数据拆解成加密后的数据
void SplitLoRaDataToDecryptedData(void)
{
	u8 	i = 0x00;
	u8 	pack_sum = 0x00;
	u8 	remain_Size = 0x00;
	u16 after_decrypt_size= 0x00;
	u32 pack_sum_size = 0x00;
	
	remain_Size = PackSumSize % LORA_PACK_SIZE;		//计算剩余数据大小
	if( PackSumSize > LORA_PACK_SIZE )
	{
		pack_sum = PackSumSize / LORA_PACK_SIZE;	//得到总的包数
		
		for(i=0; i<pack_sum; i++)
		{
			DecryptTheText(&EncryptedData[i*224],LORA_PACK_SIZE);
			TimerDelay_Ms(50);
			GetDecryptedText( &DecryptedData[i*224],LORA_PACK_SIZE );
		}
		
		DecryptTheText(&EncryptedData[pack_sum*224],remain_Size);
		TimerDelay_Ms(50);
		if( remain_Size%16 == 0 )
		{
			after_decrypt_size = remain_Size;
			GetDecryptedText( &DecryptedData[pack_sum*224],after_decrypt_size );
		}
		else
		{
			after_decrypt_size = ((remain_Size/16)+1)*16;
			GetDecryptedText( &DecryptedData[pack_sum*224],after_decrypt_size );
		}
		pack_sum_size = pack_sum*LORA_PACK_SIZE + after_decrypt_size;	//总的数据长度
	}
	
	else
	{
		DecryptTheText(EncryptedData,PackSumSize);
		TimerDelay_Ms(50);
		
		if( remain_Size%16 == 0 )
		{
			after_decrypt_size = remain_Size;
			GetDecryptedText( DecryptedData,after_decrypt_size );
		}
		else
		{
			after_decrypt_size = ((remain_Size/16)+1)*16;
			GetDecryptedText( DecryptedData,after_decrypt_size );
		}
		pack_sum_size = after_decrypt_size;
	}
	
	//存储加密后的数据(待写)
	memcpy(EncryptedData,DecryptedData,1100);
	RestoreWhichLog(EncryptFlag,EncryptLogBuf,0,0,Log_Successful);
	
	memset(EncryptedData,0,1100);
	memset(DecryptedData,0,1100);
	memset(EncryptLogBuf,0,1100);
}


void LoRaCtrlRelayACK(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x50;						//帧头
	data[1] = 0x09;						//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DeviceAddr>>24; 	//LoRa设备地址
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	
	data[6] = 1;						//指令动作 0/1
	
	crc 	= CalCrc16(data,7,0xFFFF);	//获取CRC校验值
	data[7] = crc>>8;					//低字节在前
	data[8] = crc;						//高字节在后
	
	EncryptTheText(data,0x09);
	TimerDelay_Ms(50);		
	GetEncryptedText(data,16);
	
	SX1276TxDataCRC32(data,16);			//LoRa发送数据
}


//主机请求ID应答
void LoRaRequestIDACK(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x51;						//帧头
	data[1] = 26;						//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DeviceAddr>>24; 	//LoRa设备地址
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	data[6] = 'O';
	data[7] = 'K';
	memcpy(&data[8],SelfID_Buf,24);
	
	crc 	= CalCrc16(data,24,0xFFFF);//获取CRC校验值
	data[24] = crc>>8;					//低字节在前
	data[25] = crc;						//高字节在后
	
	SX1276TxDataCRC32(data,26);			//LoRa发送数据
}


//接收完LoRa数据的应答包
void LoRaRevEncryptdDataACK(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x5F;								//帧头
	data[1] = 0x08;								//数据包长度
	
	data[2] = LoRaPact.DeviceAddr>>24; 		//LoRa设备地址
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;

	crc 	= CalCrc16(data,6,0xFFFF);			//获取CRC校验值
	data[6] = crc>>8;							//低字节在前
	data[7] = crc;								//高字节在后
	
	SX1276TxDataCRC32(data,8);					//LoRa发送数据
}




 
 

