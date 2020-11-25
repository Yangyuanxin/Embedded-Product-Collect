#include "update_sm4key.h"


extern void GetTheAID(u8 *AID_Buf,u16 AID_Size);

/*****************************************************************
							变量区
******************************************************************/
	extern u8 SelfID_Buf[16];
	extern u8 SM4_KEY[16];
	extern u8 AgreeStatus;
	extern u8 GetBidStatus;
	//-------------------------
	u8 	UpdateSM4KeyFlag    = 0;		//开始更新密钥标志
	u8  SoftWareResetFlag   = DISABLE;	//软件复位标志
	u8 	UpdateSM4KeyAckFlag = DISABLE;	//更新密钥应答标志(从机)
	u32	CalcOneHourCounter  = 0x00;		//更新密钥计时器
/*****************************************************************/


/****************************************************
	* 功能描述：更新SM4密钥
	* 入口参数：无
	* 返回值：	无
	* 备注：
*****************************************************/
void TheTimeUptoUpdateSM4KeyHandle(void)
{
	//检测到更换密钥标志
	if( UpdateSM4KeyFlag!=ENABLE )
		return ;

	//软件复位处理(更新到来只执行1次)
	if( SoftWareResetFlag )
	{
		SoftWareResetFlag = DISABLE;	//重置软件复位标志
		AgreeStatus       = DISABLE;	//重置密钥协商标志	

	#if 1
		HostComSoftwareReset(hostcom_struct.WhichUart);	//复位上位机串口参数
		LoRaSoftWareParaReset();						//复位LoRa软件参数
	#endif

		RestHSC32Module();				//复位加密模
		SetSM2AgreementProtocol();		//设置密钥协商方
		TimerDelay_Ms(10);				//延时10ms读取数据
		GetTheAID(SelfID_Buf,16);		//重新获取本方随机数

	#if TEST_AGREE
		//发送本方标识符
		RS485Uart1SendData(hostcom_struct.WhichUart,SelfID_Buf,16);
	#endif
	}
	
	//检测更新密钥应答标志
	if( UpdateSM4KeyAckFlag!=ENABLE )
	{
		//?通知对方更新SM4密钥(加入此时从机正在发送加密数据，如何解决冲突?)
		LoRaRequestUpdateSM4KeyCmd(LoRaPact.TxBuf,0);	
		SX1276RxStateEnter();
		TimerDelay_Ms(160);
	}
}


//重发获取从机标识符的机制
void ResendGetSlaveIdCmdProcess(void)
{
	//检测更新密钥应答标志
	if( UpdateSM4KeyAckFlag!=ENABLE )
		return ;
	
	if( GetBidStatus==ENABLE )
		return;
	
	GetSlaveID();	//获取从机的标识符	
	TimerDelay_Ms(100);
}


//运行1小时计时器
void RunCalcOneHourCounter(void)
{
	if(LoRaDeviceIs!=MASTER)
		return;
	
	if( CalcOneHourCounter>UPDATE_SM4_TIME_VALUE )
	{
		CalcOneHourCounter = 0x00;		//将1小时计时器清零
		UpdateSM4KeyFlag   = ENABLE;	//置位更新密钥标志
		SoftWareResetFlag  = ENABLE;	//置位软件复位标志	
	}
	else
		CalcOneHourCounter ++;			//计时
}


void DisableDMA1Channelx(u8 _which_uart)
{
	switch(_which_uart)
	{
		case UART1:		
			//关闭通道5接收			
			DMA_Cmd(DMA1_Channel5,DISABLE);  
		break;
		
		case UART3:		
			//关闭通道3接收			
			DMA_Cmd(DMA1_Channel3,DISABLE);  	
		break;
		
		default:
		break;
	}
}


void EnableDMA1Channelx(u8 _which_uart)
{
	switch(_which_uart)
	{
		case UART1:
			Usart_X_Init(COM1);
			DMA_RX_Init(COM1,hostcom_struct.Comx_RxBuf);	//DMA1串口1初始化
			DMA_TX_Init(COM1,hostcom_struct.Comx_TxBuf);	//DMA1串口1初始化 
		break;
		
		case UART3:	
			Usart_X_Init(COM3);
			DMA_RX_Init(COM3,hostcom_struct.Comx_RxBuf);	//DMA1串口1初始化
			DMA_TX_Init(COM3,hostcom_struct.Comx_TxBuf);	//DMA1串口1初始化	
		break;
		
		default:
		break;
	}
}


//复位上位机串口软件参数
void HostComSoftwareReset(u8 _which_uart)
{
	u16 Temp;
	
	if( _which_uart == UART1)
	{
		Temp = USART1->SR;			//先读SR,然后DR才能清除
		Temp = USART1->DR;			//清USART_IT_IDLE标志
		USART_ClearFlag(USART1, USART_FLAG_TC);
		USART_Cmd(USART1,DISABLE);
	}
	else if( _which_uart == UART3)
	{
		Temp = USART3->SR;			//先读SR,然后DR才能清除
		Temp = USART3->DR;			//清USART_IT_IDLE标志
		USART_ClearFlag(USART3, USART_FLAG_TC);
		USART_Cmd(USART3,DISABLE);
	}
	
	hostcom_struct.Comx_RevCompleted = DISABLE;
	memset(hostcom_struct.Comx_RxBuf,0,HOST_RX_SIZE);
	hostcom_struct.Comx_RxCnt = 0;	//将接收的字节长度赋值
}


void LoRaSoftWareParaReset(void)
{
	LoRaPact.RxCnt = 0x00;
	LoRaPact.TxCnt = 0x00;
	memset(LoRaPact.RxBuf,0,LORA_RX_SIZE);
	memset(LoRaPact.TxBuf,0,LORA_TX_SIZE);
}




