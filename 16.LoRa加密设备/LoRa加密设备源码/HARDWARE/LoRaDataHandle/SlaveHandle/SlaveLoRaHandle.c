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
	extern u8  NetLedStatus;
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
	u8 	TimeSynchronizeFlag = DISABLE;		//时间同步标志
	u32	AckCounter = 0x00;					//应答计数器
/**********************************************************************/


/**********************************************
	* 功能描述：LoRa节点处理模块通信数据
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
***********************************************/
void LoRaNodeHandleCommunicateData(void)
{
	ReceiveData(LoRaPact.RxBuf, &LoRaPact.RxCnt);					//接收数据包
	HandleLoRaCommunicateProtocol(LoRaPact.RxBuf,LoRaPact.RxCnt);	//解析数据包		
}


/***************************************************
	* 功能描述：	LoRa通信协议处理
	* 入口参数：	无
	* 返回值：		无
	* 备注：	
****************************************************/
void HandleLoRaCommunicateProtocol(u8 *data,u16 datalen)
{
	u8  led_status = 0x00;
	u16 tempCrc  = 0x00;
	u32 tempAddr = 0x00;
	
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
		//控制LED状态
		case 0x50:
			if(data[6] == 1)		//应答函数
				NET_LED = !NET_LED;	//改变NET灯的状态
			
			if(NET_LED==1)
				NetLedStatus = LED_OFF;
			else
				NetLedStatus = LED_ON;
			
			//应答包
			LoRaCtrlNetLedCmdACK(LoRaPact.TxBuf,0);
		break;
		
		//控制LED应答包
		case 0x51:	
			led_status = data[6];	//获取对方的LED状态
		break;
		
		//秘钥协商
		case 0x52:
			if( (data[6] == 'I')&&
				(data[7] == 'D') )
			{
				memcpy(OtherID_Buf,&LoRaPact.RxBuf[8],16);
				GetBidStatus = ENABLE;
				LoRaRequestIDACK(LoRaPact.TxBuf,0);
			}
		break;
		
		//获取对方的ID应答
		case 0x53:
			//... ...
		break;
		
		//时间同步
		case 0x54:
			SysYear  = data[6]<<8 | data[7];
			SysMonth = data[8];
			SysDate  = data[9];
			SysHour	 = data[10];
			SysMin	 = data[11];
			SysSec	 = data[12];
			RTC_Set(SysYear,SysMonth,SysDate,SysHour,SysMin,SysSec);
			
			//置位时间同步标志
			TimeSynchronizeFlag = ENABLE;
		break;
		
		//时间同步应答
		case 0x55:
			//... ...
		break;
		
		//数据传输前的1包
		case 0x56:			
			PackSum 	= data[6];				//总的包数
			PackSumSize = (data[7]<<8)|data[8];	//总的数据包长度
		break;
		
		//接收加密后的透传数据
		case 0x57:
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
					//存储通信日志
					RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
					
					//拆包解密
					SplitLoRaDataToDecryptedData();
					
					//变量处理
					PackSum = 0;
					PackSumSize = 0;
					CountPackNum = 0;
					LoRaRevEncryptdDataACK(LoRaPact.TxBuf,0);
				}
			}
		break;
		
		//LoRa传输完成时
		case 0x58:
			//接收状态下
			if( st==Rx_ST )
			{
				if( (data[6] == 'E')&&
					(data[7] == 'D') )
				{
					AckCounter ++;
//					hostcom_struct.Comx_TxBuf[0] = AckCounter>>24;
//					hostcom_struct.Comx_TxBuf[1] = AckCounter>>16;
//					hostcom_struct.Comx_TxBuf[2] = AckCounter>>8;
//					hostcom_struct.Comx_TxBuf[3] = AckCounter;
//					RS485Uart1SendData(hostcom_struct.WhichUart,hostcom_struct.Comx_TxBuf,4);
				}
			}
		break;
			
			
		//请求更新SM4密钥
		case 0x59:
			if( (data[6] == 'U')&&
				(data[7] == 'P')&&
				(data[8] == 'K' ) )
			{
				AgreeStatus = DISABLE;							//重置协商标志
				
				HostComSoftwareReset();							//
				RestHSC32Module();								//复位加密模块
				SetSM2AgreementProtocol();						//设置密钥协商方
				TimerDelay_Ms(50);
				GetTheAID(SelfID_Buf,16);						//获取本方随机数				
			}
		break;
			
		//其他
		default:
		break;
	}
	
	memset(LoRaPact.RxBuf,0,LORA_RX_SIZE);	//将接收缓冲区清零
	LoRaPact.RxCnt = 0;						//将接收计数器清零
	SX1276RxStateEnter();					//LoRa切换为接收状态
}


/****************************************
	* 功能描述：SM2密钥协商
	* 入口参数：
	* 返回值：
	* 备注：	
*********************************************/
void LoRaSlaveHandleSm2KeyAgreement(void)
{
	//密钥协商成功则退出函数
	if(AgreeStatus != DISABLE)
		return ;
	
	if( GetBidStatus )
	{
		GetBidStatus = DISABLE;
		SetTheBID(OtherID_Buf,16);
		AgreeStatus = ENABLE;
		TimerDelay_Ms(500);	//++
		HSC32I1_ReadLenByte(SM4_KEYADDR,SM4_KEY,16);
		
//		//进入接收状态
//		SX1276RxStateEnter();							
		//打开DMA1通道
		EnableDMA1Channelx(hostcom_struct.WhichUart);
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
	
	SendTheDecryptedData(hostcom_struct.Comx_TxBuf,PackSumSize);	//打印解密后的数据
	memcpy(EncryptedData,DecryptedData,1100);						//存储加密后的数据(待写)
	RestoreWhichLog(EncryptFlag,EncryptLogBuf,0,0,Log_Successful);
	
	memset(EncryptedData,0,1100);
	memset(DecryptedData,0,1100);
	memset(EncryptLogBuf,0,1100);
}





