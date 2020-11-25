/************************************************************************
	* 文件说明：两个LoRa模块之间通信协议；
				1.控制LED状态				--	0x50
				2.控制LED状态应答			--	0x51
				3.获取对方ID标识符			--	0x52
				4.获取对方ID标识符应答		--	0x53
				5.时间同步指令				--	0x54
				6.时间同步指令应答			--	0x55
				7.发送总的加密数据包信息	--	0x56
				8.发送加密数据				--	0x57
				9.接收完总的加密数据应答	--	0x58
				10.请求更新SM4密钥指令		--	0x59

	* 作者：	ZWQ
	* 时间：	2019-3-2
	* 备注：
*************************************************************************/
#include "LoRaProtocolCmd.h"


/***************************************************
					变量区
****************************************************/
	extern u8  SelfID_Buf[16];
	extern u8  DecryptedData[1100];
	extern u8  NetLedStatus;
/***************************************************/


/******************************************
	* 功能描述：控制LED状态指令
	* 入口参数：data    -- 数据地址
				datalen -- 数据长度
	* 返回值：	无
	* 备注：	协议参数需要改变
*******************************************/
void LoRaCtrlNetLedCmd(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x50;							//帧头
	data[1] = 0x09;							//数据包的长度
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 1;							//动作指令0/1	
	
	crc_value 	= CalCrc16(data,7,0xffff);	//获取CRC校验值
	data[7] = crc_value>>8;					//低字节在前
	data[8] = crc_value;					//高字节在后
	
	SX1276TxDataCRC32(data,16);				//LoRa发送数据
}


/******************************************
	* 功能描述：控制LED状态指令
	* 入口参数：data    -- 数据地址
				datalen -- 数据长度
	* 返回值：	无
	* 备注：	协议参数需要改变
*******************************************/
void LoRaCtrlNetLedCmdACK(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x51;							//帧头
	data[1] = 0x09;							//数据包的长度
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = NetLedStatus;					//动作指令0/1	
	
	crc_value = CalCrc16(data,7,0xffff);	//获取CRC校验值
	data[7] = crc_value>>8;					//低字节在前
	data[8] = crc_value;					//高字节在后
	
	SX1276TxDataCRC32(data,16);				//LoRa发送数据
}


/******************************************
	* 功能描述：获取对方ID标识符
	* 入口参数：data    -- 数据地址
				datalen -- 数据长度
	* 返回值：	无
	* 备注：	协议参数需要改变
*******************************************/
void LoRaRequestIDIDCmd(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x52;								//帧头
	data[1] = 26;								//数据包的长度
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 		//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 'I';								//标识符
	data[7] = 'D';
	
	memcpy(&data[8],SelfID_Buf,16);				//本机随机数
	
	crc_value 	 = CalCrc16(data,24,0xffff);	//获取CRC校验值
	data[24] = crc_value>>8;					//低字节在前
	data[25] = crc_value;						//高字节在后
	
	SX1276TxDataCRC32(data,26);					//LoRa发送数据
}


//主机请求ID应答
void LoRaRequestIDACK(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x53;								//帧头
	data[1] = 26;								//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DeviceAddr>>24; 			//LoRa设备地址
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	
	data[6] = 'O';
	data[7] = 'K';
	memcpy(&data[8],SelfID_Buf,24);
	
	crc_value 	 = CalCrc16(data,24,0xFFFF);	//获取CRC校验值
	data[24] = crc_value>>8;					//低字 节在前
	data[25] = crc_value;						//高字节在后
	
	SX1276TxDataCRC32(data,26);					//LoRa发送数据
}


//时间同步指令
void LoRaTimeSynchronizationCmd(u8 *data,u16 datalen)
{
	u16 crc_value = 0x00;
	
	data[0]  = 0x54;						//帧头
	data[1]  = 0x0F;						//数据包长度
	
	data[2]  = LoRaPact.DstDeviceAddr>>24; 	//LoRa设备地址
	data[3]  = LoRaPact.DstDeviceAddr>>16;
	data[4]  = LoRaPact.DstDeviceAddr>>8;
	data[5]  = LoRaPact.DstDeviceAddr>>0;
	
	data[6]  = SysYear>>8;			//年
	data[7]  = SysYear;
	data[8]  = SysMonth;			//月
	data[9]  = SysDate;				//日
	data[10] = SysHour;				//小时
	data[11] = SysMin;				//分钟
	data[12] = SysSec;				//秒
	
	crc_value = CalCrc16(data,13,0xFFFF);
	data[13] = crc_value>>8;				//高字节
	data[14] = crc_value;
	
	SX1276TxDataCRC32(data,15);				//LoRa发送数据
}


//发送总的加密数据包信息
void LoRaBeforeSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x56;								//帧头
	data[1] = 0x0B;								//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 		//LoRa目标设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;							//总的包数量
	data[7] = PackSumSize>>8;					//数据包长度
	data[8] = PackSumSize;
	
	crc_value 	 = CalCrc16(data,9,0xffff);		//获取CRC校验值
	data[9]  = crc_value>>8;					//低字节在前
	data[10] = crc_value;						//高字节在后
	
	SX1276TxDataCRC32(data,11);					//LoRa发送数据
}


//发送加密数据
void LoRaSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x57;										//帧头
	data[1] = datalen+10;								//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 				//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;									//总的包数量
	data[7] = CountPackNum;								//此刻的包号
	
	memcpy(&data[8],&EncryptedData[CountPackNum*224],datalen);
	
	crc_value 	= CalCrc16(data,datalen+8,0xffff);		//获取CRC校验值
	data[datalen+8] = crc_value>>8;						//高字节在前
	data[datalen+9] = crc_value;						//低字节在后
	
	SX1276TxDataCRC32(data,datalen+10);					//LoRa发送数据
}


//接收完加密数据的应答包
void LoRaRevEncryptdDataACK(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x58;								//帧头
	data[1] = 0x08;								//数据包长度
	
	data[2] = LoRaPact.DeviceAddr>>24; 			//LoRa设备地址
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	
	data[6] = 'E';
	data[7] = 'D';
	
	crc_value 	= CalCrc16(data,8,0xFFFF);		//获取CRC校验值
	data[8] = crc_value>>8;						//低字节在前
	data[9] = crc_value;						//高字节在后
	
	SX1276TxDataCRC32(data,10);					//LoRa发送数据
}


//请求更新SM4密钥指令
void LoRaRequestUpdateSM4KeyCmd(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x59;								//帧头
	data[1] = 0x08;								//数据包长度
	
	data[2] = LoRaPact.DeviceAddr>>24; 			//LoRa设备地址
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	
	data[6] = 'U';
	data[7] = 'P';
	data[8] = 'K';
	
	crc_value 	 = CalCrc16(data,9,0xFFFF);		//获取CRC校验值
	data[9]  = crc_value>>8;					//低字节在前
	data[10] = crc_value;						//高字节在后
	
	SX1276TxDataCRC32(data,11);					//LoRa发送数据
}


//发送解密后的数据
void SendTheDecryptedData(u8 *buf,u16 datalen)
{
	memcpy(buf,DecryptedData,datalen);
	
	if(hostcom_struct.WhichUart == UART1)
	{
//		//串口1DMA1发送
//		DMA_SetCurrDataCounter(DMA1_Channel4,datalen);
//		DMA_Cmd(DMA1_Channel4, ENABLE);
		RS485Uart1SendData(COM1,buf,datalen);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
//		//串口3DMA1发送
//		DMA_SetCurrDataCounter(DMA1_Channel2,datalen);
//		DMA_Cmd(DMA1_Channel2, ENABLE);
		RS485Uart1SendData(COM3,buf,datalen);
	}
}




