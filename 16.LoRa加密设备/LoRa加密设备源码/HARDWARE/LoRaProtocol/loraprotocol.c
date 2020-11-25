#include "loraprotocol.h"


/**********************************************************************
							变量区
***********************************************************************/
	extern u16  LoRaTransCounter;
	extern u8 	SelfID_Buf[16];		//保存自身标识符
	extern u8 	OtherID_Buf[16];	//保存对方标识符
	LoRaStruct	LoRaPact;			//定义lora解析协议结构体
	u8	LoRaRxBuf[LORA_RX_SIZE];	//定义接收缓冲区
	u8	LoRaTxBuf[LORA_TX_SIZE];	//定义发送缓冲区
	static u8 RelayStatus = ENABLE;	//继电器的状态值
	//---------------------------------------------------------
	u8 	EncryptedData[1100] = {0,};	//保存加密后的数据
	
	u8 	PackSum = 0x00;				//总的包数
	u16 AfterEncryptSzie = 0x00;	//加密后的数据长度
	u16 PackSumSize = 0x00;			//总的数据包长度
	u8 	CountPackNum = 0x00;		//数据包计数
/**********************************************************************/


/***************************************************
	* 功能描述：	设置LoRa结构体参数
	* 入口参数：	无
	* 返回值：		无
****************************************************/
void SetLoRaStruPara(void)
{
	LoRaPact.RxCnt 	    	= 0x00;
	LoRaPact.TxCnt 	    	= 0x00;
	LoRaPact.RxBuf 	    	= LoRaRxBuf;	
	LoRaPact.TxBuf 	    	= LoRaTxBuf;
	LoRaPact.DeviceAddr 	= ReadDeviceID();	//读取本机设备号
	LoRaPact.DstDeviceAddr 	= 0x09;
	//****3保存日志(设置设备地址)
	RestoreOperateLog(OperateLogBuf,SET_DEVICE_ADDR_CODE,SEC_CODE,Log_Successful);
}


/******************************************
	* 功能描述：控制继电器指令
	* 入口参数：data    -- 数据地址
				datalen -- 数据长度
	* 返回值：	无
	* 备注：	协议参数需要改变
*******************************************/
void LoRaGetIDCmd(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x51;						//帧头
	data[1] = 26;						//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 'I';
	data[7] = 'D';
	
	memcpy(&data[8],SelfID_Buf,16);
	crc 	= CalCrc16(data,24,0xffff);	//获取CRC校验值
	data[24] = crc>>8;					//低字节在前
	data[25] = crc;						//高字节在后
	
	SX1276TxDataCRC32(data,26);			//LoRa发送数据
}


/******************************************
	* 功能描述：控制继电器指令
	* 入口参数：data    -- 数据地址
				datalen -- 数据长度
	* 返回值：	无
	* 备注：	协议参数需要改变
*******************************************/
void LoRaCtrlRelayCmd(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x50;						//帧头
	data[1] = 0x09;						//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	//指令动作 0/1
	if( RelayCounter > 200 )
	{
		RelayCounter = 0;
		data[6] = RelayStatus;			//动作指令0/1	
		RelayStatus  = !RelayStatus;	//改变控制指令的值
	}
	
	crc 	= CalCrc16(data,7,0xffff);	//获取CRC校验值
	data[7] = crc>>8;					//低字节在前
	data[8] = crc;						//高字节在后
	EncryptTheText(data,0x09);
	TimerDelay_Ms(100);
	GetEncryptedText(data,16);
	SX1276TxDataCRC32(data,16);			//LoRa发送数据
}


//在发送加密数据之前
void LoRaBeforeSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x5E;							//帧头
	data[1] = 0x0b;							//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa目标设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;						//总的包数量
	data[7] = PackSumSize>>8;				//数据包长度
	data[8] = PackSumSize;
	
	crc 	 = CalCrc16(data,9,0xffff);		//获取CRC校验值
	data[9]  = crc>>8;						//低字节在前
	data[10] = crc;							//高字节在后
	
	SX1276TxDataCRC32(data,11);				//LoRa发送数据
}


void LoRaSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x5F;								//帧头
	data[1] = datalen+10;						//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 		//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;		//总的包数量
	data[7] = CountPackNum;	//此刻的包号
	
	memcpy(&data[8],&EncryptedData[CountPackNum*224],datalen);
	
	crc 	= CalCrc16(data,datalen+8,0xffff);	//获取CRC校验值
	data[datalen+8] = crc>>8;					//高字节在前
	data[datalen+9] = crc;						//低字节在后
	
	SX1276TxDataCRC32(data,datalen+10);			//LoRa发送数据
}


//将串口数据拆解成加密后的数据
void SplitComDataToEncryptedData(void)
{
	u8 	i = 0x00;
	u8 	RemainSize = 0x00;
	
	RemainSize = LoRaPact.TxCnt % LORA_PACK_SIZE;		//计算剩余数据大小
	if( LoRaPact.TxCnt>LORA_PACK_SIZE )
	{
		PackSum = LoRaPact.TxCnt / LORA_PACK_SIZE;		//得到总的包数
		
		for(i=0; i<PackSum; i++)
		{
			EncryptTheText(&hostcom_struct.Comx_RxBuf[i*224],LORA_PACK_SIZE);
			TimerDelay_Ms(50);
			GetEncryptedText( &EncryptedData[i*224],LORA_PACK_SIZE );
		}
		
		EncryptTheText(&hostcom_struct.Comx_RxBuf[PackSum*224],RemainSize);
		TimerDelay_Ms(50);
		if( RemainSize%16 == 0 )
		{
			AfterEncryptSzie = RemainSize;
			GetEncryptedText( &EncryptedData[PackSum*224],AfterEncryptSzie );
		}
		else
		{
			AfterEncryptSzie = ((RemainSize/16)+1)*16;
			GetEncryptedText( &EncryptedData[PackSum*224],AfterEncryptSzie );
		}
		PackSumSize = PackSum*LORA_PACK_SIZE + AfterEncryptSzie;	//总的数据长度
	}
	
	else
	{
		EncryptTheText(hostcom_struct.Comx_RxBuf,LoRaPact.TxCnt);
		TimerDelay_Ms(50);
		
		if( RemainSize%16 == 0 )
		{
			AfterEncryptSzie = RemainSize;
			GetEncryptedText( EncryptedData,RemainSize );
		}
		else
		{
			AfterEncryptSzie = ((RemainSize/16)+1)*16;
			GetEncryptedText( EncryptedData,((RemainSize/16)+1)*16 );
		}
		PackSumSize = AfterEncryptSzie;
	}
	//存储加密后的数据(待写)
	RestoreEncryptLog(EncryptLogBuf,Log_Successful);
}


void LoRaHandleEncryptedData(void)
{
	u8 size = 0x00;
	
	SplitComDataToEncryptedData();					//获取加密数据
	LoRaBeforeSendEncryptedData(LoRaPact.TxBuf,0);	//在发送加密数据前的数据包
	TimerDelay_Ms(5);
	
	//不足1包
	if(PackSum <= 0)
	{
		LoRaSendEncryptedData(LoRaPact.TxBuf,PackSumSize);			//LoRa发送加密数据
		RestoreCommunicateLog(CommunicateLogBuf,Log_Successful);	//存储LoRa发送的内容
	}
	
	//至少为1包
	else
	{
		while( CountPackNum<PackSum+1 )
		{
			if(CountPackNum >= PackSum )
			{	
				size = PackSumSize - PackSum*LORA_PACK_SIZE;				//发送最后一包
				LoRaSendEncryptedData(LoRaPact.TxBuf,size);					//LoRa发送最后1包加密数据
				RestoreCommunicateLog(CommunicateLogBuf,Log_Successful);	//存储LoRa发送的数据
				break;
			}
			else
			{
				//正常发送
				LoRaSendEncryptedData(LoRaPact.TxBuf,LORA_PACK_SIZE);
				CountPackNum ++;
				TimerDelay_Ms(20);
			}
		}
	}
	
}



