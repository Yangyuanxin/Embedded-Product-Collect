#include "update_sm4key.h"


extern void GetTheAID(u8 *AID_Buf,u16 AID_Size);
extern u8 SelfID_Buf[16];
extern u8 	AgreeStatus;

u8 	UpdateSM4KeyFlag   = 0;
u16	CalcOneHourCounter = 0x00;
u16 UpdatedCounter = 0x00;		//记录更新次数
char UpdateTempBuf[30] ={ 0,};

/****************************************************
	* 功能描述：更新SM4密钥
	* 入口参数：
	* 返回值：
	* 备注：
*****************************************************/
void TheTimeUptoUpdateSM4KeyHandle(void)
{
	//检测到更换密钥标志
	if( UpdateSM4KeyFlag==ENABLE )
	{
		UpdateSM4KeyFlag = DISABLE;
		
		AgreeStatus = DISABLE;							//重置密钥协商标志								
		LoRaRequestUpdateSM4KeyCmd(LoRaPact.TxBuf,0);	//通知对方更新SM4密钥
		RestHSC32Module();								//复位加密模块
		SetSM2AgreementProtocol();			 			//设置密钥协商方
		TimerDelay_Ms(20);
		GetTheAID(SelfID_Buf,16);						//重新获取本方随机数
		TimerDelay_Ms(50);
		LoRaMasterHandleSm2KeyAgreement();				//进行密钥协商
		
		//打印更新次数
		UpdatedCounter ++;
		sprintf(UpdateTempBuf,"Successful:%d\r\n",UpdatedCounter);
		RS485Uart1SendData(hostcom_struct.WhichUart,(u8 *)UpdateTempBuf,25);
	}
}


void RunCalcOneHourCounter(void)
{
	if( CalcOneHourCounter>UPDATE_SM4_TIME_VALUE )
	{
		CalcOneHourCounter = 0x00;
//		HostComSoftwareReset();
		UpdateSM4KeyFlag = ENABLE;
	}
	else
		CalcOneHourCounter ++;
}


void DisableDMA1Channelx(u8 _which_uart )
{
	switch(_which_uart)
	{
		case UART1:
//			//关闭串口1
//			USART_Cmd(USART1,DISABLE);
			//设置数据接收长度，重新装填，并让数据接收地址偏移地址从0开始
			DMA_SetCurrDataCounter(DMA1_Channel5,HOST_RX_SIZE);		
			//关闭通道5接收			
			DMA_Cmd(DMA1_Channel5,DISABLE);  
		break;
		
		case UART3:
//			//关闭串口3
//			USART_Cmd(USART3,DISABLE);
			//设置数据接收长度，重新装填，并让数据接收地址偏移地址从0开始
			DMA_SetCurrDataCounter(DMA1_Channel3,HOST_RX_SIZE);			
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
//			//开启串口1
//			USART_Cmd(USART1,ENABLE);
			//关闭通道5接收			
			DMA_Cmd(DMA1_Channel5,ENABLE);  
		break;
		
		case UART3:
//			//开启串口3
//			USART_Cmd(USART3,ENABLE);	
			//关闭通道3接收			
			DMA_Cmd(DMA1_Channel3,ENABLE);  	
		break;
		
		default:
		break;
	}
}


void HostComSoftwareReset(void)
{
	u16 Temp;
	
	Temp = USART1->SR;								//先读SR,然后DR才能清除
	Temp = USART1->DR;								//清USART_IT_IDLE标志
	DisableDMA1Channelx(hostcom_struct.WhichUart);
	hostcom_struct.Comx_RxCnt = 0;					//将接收的字节长度赋值
}


//备份此刻串口接收到的数据
void BackUpRevComDataRightNow()
{
	
}




