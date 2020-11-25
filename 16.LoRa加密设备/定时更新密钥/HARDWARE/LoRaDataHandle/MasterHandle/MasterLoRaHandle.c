#include "MasterLoRaHandle.h"


/**********************************************************************
							变量区
***********************************************************************/
	
	
	//----------------------------------------------------------
	extern u16	LoRaTransCounter;	//传输计时器(收/发总时长)
	extern u16 	WaitIDCounter;		//等待对方ID计时器
	extern u8 	UpdateSM4KeyAckFlag;
	LoRaStruct	LoRaPact;			//定义LoRa解析协议结构体
	u8	LoRaRxBuf[LORA_RX_SIZE];	//定义接收缓冲区
	u8	LoRaTxBuf[LORA_TX_SIZE];	//定义发送缓冲区
	_ST st = Tx_ST;					//定义主机的状态枚举
	u8 	NetLedStatus = LED_ON;		//网络灯的状态				
	//------------------------------------------------
	u8 	SelfID_Buf[16]  = {0,};		//保存自身标识符
	u8 	OtherID_Buf[16] = {0,};		//保存对方标识符
	u8 	SM4_KEY[16] = {0,};			//SM4密钥
	u8 	GetBidStatus = DISABLE;		//得到对方标识符
	u8 	AgreeStatus  = DISABLE;		//sm2密钥协商成功
	//------------------------------------------------
	
	//---------------------------------------------------
	u8 	EncryptedData[1100] = {0,};	//保存加密后的数据
	u8  DecryptedData[1100] = {0,};	//保存解密后的数据
	u8 	PackSum = 0x00;				//总的包数
	u8 	CountPackNum = 0x00;		//数据包计数
	u16 AfterEncryptSzie = 0x00;	//加密后的数据长度
	u16 PackSumSize = 0x00;			//总的数据包长度
	//---------------------------------------------------
/**********************************************************************/

	
/*************************************************
	* 功能描述：LoRa节点处理串口透传数据
	* 入口参数：无
	* 返回值：	无
	* 备注：	主从机设备共用API接口
**************************************************/
void LoRaNodeHandleHostComData(void)
{
	//秘钥还未匹配成功
	if(AgreeStatus !=ENABLE)
		return ;
	
	//未切换到透传模式
	if(com_work_mode != ComMode_Trans)
		return;
	
	//判断上位机串口状态是否已经切换
	IsComChangeToAT_WorkMode();
	
	//串口接收到数据且工作在传透模式
	if( (hostcom_struct.Comx_RevCompleted == ENABLE) && (com_work_mode == ComMode_Trans) )
	{
		hostcom_struct.Comx_RevCompleted = DISABLE;		//使能下一次数据处理
		LoRaPact.TxCnt = hostcom_struct.Comx_RxCnt;		//得到接收到的数据长度
		
		LoRaHandleEncryptedData();	//发送加密数据
		SX1276RxStateEnter();		//进入接收状态

		LoRaPact.TxCnt = 0;
		PackSum 	 = 0;
		PackSumSize  = 0;
		CountPackNum = 0;
		AfterEncryptSzie = 0;
		
		//接收完数据后，待处理DMA1相关... ...
		hostcom_struct.Comx_RxCnt = 0;						//将接收计数器清零
		memset(hostcom_struct.Comx_RxBuf,0,HOST_RX_SIZE);	//将接收缓冲区清空
		if(hostcom_struct.WhichUart == UART1)
		{
			//设置数据接收长度，重新装填，并让数据接收地址偏移地址从0开始
			DMA_SetCurrDataCounter(DMA1_Channel5,HOST_RX_SIZE);		
			//打开通道5接收			
			DMA_Cmd(DMA1_Channel5,ENABLE);  					
		}
		else if(hostcom_struct.WhichUart == UART3)
		{
			//设置数据接收长度，重新装填，并让数据接收地址偏移地址从0开始
			DMA_SetCurrDataCounter(DMA1_Channel3,HOST_RX_SIZE);			
			//打开通道3接收			
			DMA_Cmd(DMA1_Channel3,ENABLE);  					
		}
	}
	
}


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
	
	//****保存系统日志(设置设备地址)
	RestoreWhichLog(SystemLogFlag,SystemLogBuf,SET_DEVICE_ADDR_CODE,0,Log_Successful);
}


/****************************************
	* 功能描述：SM2密钥协商
	* 入口参数：
	* 返回值：
	* 备注：	
*********************************************/
void LoRaMasterHandleSm2KeyAgreement(void)
{
	//密钥协商成功则退出函数
	if(AgreeStatus != DISABLE)
		return ;
	
	//得到了从机的ID
	if( GetBidStatus )			
	{
		GetBidStatus        = DISABLE;	//重置获取对方标识符标志
		UpdateSM4KeyFlag    = DISABLE;	//重置更新密钥标志
		UpdateSM4KeyAckFlag = DISABLE;	//重置更新密钥应答标志
		PackSumSize = 0;				//重置接收数据包的长度
		AgreeStatus = ENABLE;			//置位密钥协商标志
		
		SetTheBID(OtherID_Buf,16);						//开始生成SM4密钥
		TimerDelay_Ms(150);								//测试极限时间？
		HSC32I1_ReadLenByte(SM4_KEYADDR,SM4_KEY,16);	//读回SM4密钥
	
		EnableDMA1Channelx(hostcom_struct.WhichUart);	//打开DMA1通道
		SX1276RxStateEnter();
	#if 1
		//发送密钥
		RS485Uart1SendData(hostcom_struct.WhichUart,SM4_KEY,16);
	#endif
	}
	
}


/********************************************************
	* 功能描述：获取从机标识符
	* 入口参数：SlaveID_Buf -- 存储从机标识符缓冲区
	* 返回值：	无
	* 备注：	无
*********************************************************/
void GetSlaveID(void)
{
	if( AgreeStatus != DISABLE )
		return ;
	
	LoRaRequestIDIDCmd(LoRaPact.TxBuf,0);
	SX1276RxStateEnter();
}


/*********************************************
	* 功能描述：LoRa处理加密数据
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
**********************************************/
void LoRaHandleEncryptedData(void)
{
	u8 size = 0x00;
	
	SplitComDataToEncryptedData();					//获取加密数据
	LoRaBeforeSendEncryptedData(LoRaPact.TxBuf,0);	//在发送加密数据前的数据包
	TimerDelay_Ms(5);
	
	//不足1包
	if(PackSum <= 0)
	{
		//LoRa发送加密数据
		LoRaSendEncryptedData(LoRaPact.TxBuf,PackSumSize);			
		
		//存储LoRa发送的内容
		RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
	}
	
	//至少为1包
	else
	{
		while( CountPackNum<PackSum+1 )
		{
			if(CountPackNum >= PackSum )
			{	
				size = PackSumSize - PackSum*LORA_PACK_SIZE;	//发送最后一包
				LoRaSendEncryptedData(LoRaPact.TxBuf,size);		//LoRa发送最后1包加密数据
				
				//存储LoRa发送的内容
				RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
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
	RestoreWhichLog(EncryptFlag,EncryptLogBuf,0,0,Log_Successful);
}




