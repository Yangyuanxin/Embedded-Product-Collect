#include "dma_usart.h"


/****************************************************
	* 功能描述：通过DMA1串口发送数据
	* 入口参数：Str -- 字符串地址
				Size -- 字符串大小
	* 返回值：	无
	* 备注：	最大长度(英文)为HOST_TX_SIZE
*****************************************************/
void DMA1_UartSendData(u8 *Str,u16 Size,u8 which_uart)
{
	//数据长度不合法
	if(Size<HOST_TX_SIZE)
	{
		switch(which_uart)
		{
			case 1:
				memcpy(hostcom_struct.Comx_TxBuf,Str,Size);	//拷贝数据到发送缓冲区
				DMA_SetCurrDataCounter(DMA1_Channel4,Size);	//设置DMA1发送数据大小
				DMA_Cmd(DMA1_Channel4, ENABLE);				//使能通道4发送
			break;
			
			case 2:
				memcpy(hostcom_struct.Comx_TxBuf,Str,Size);	//拷贝数据到发送缓冲区
				DMA_SetCurrDataCounter(DMA1_Channel7,Size);	//设置DMA1发送数据大小
				DMA_Cmd(DMA1_Channel7, ENABLE);				//使能通道4发送
			break;
			
			case 3:
				memcpy(hostcom_struct.Comx_TxBuf,Str,Size);	//拷贝数据到发送缓冲区
				DMA_SetCurrDataCounter(DMA1_Channel2,Size);	//设置DMA1发送数据大小
				DMA_Cmd(DMA1_Channel2, ENABLE);				//使能通道4发送
			break;
			
			default:
				//... ...
			break;
		}
	}
	
}




