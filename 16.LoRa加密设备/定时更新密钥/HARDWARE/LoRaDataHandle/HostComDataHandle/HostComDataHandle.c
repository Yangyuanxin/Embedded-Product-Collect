/***********************************************************************************************
	* 文件说明：此文件针对串口数据频繁发送，暂时将数据缓存起来，以待后续发送；
				实现原理为队列操作；
	* 作者：	ZWQ
	* 时间：	2019-3-1
	* 备注：	注意在LoRa为半双工的条件下实现伪半双工，要求MCU的串口具备全双工功能、
				外部串口设备具备全双工，在此前提下使用循环队列缓存数据，利用时间片的原理
				发送数据，与上位机协调好时间。
*************************************************************************************************/
#include "HostComDataHandle.h"


/**************************************************************************
						变量区
***************************************************************************/
		_ComDataQueue com_data_queue;
/**************************************************************************/


/***************************************************
	* 功能描述：串口数据队列初始化
	* 入口参数：_ComDataQueue -- 结构体指针
	* 返回值：	
	* 备注：	
****************************************************/
void ComDataQueueParaInit(_ComDataQueue *_com_data_queue)
{
	_com_data_queue->QueueCount      = 0x00;
	_com_data_queue->QueueNowPoint   = 0x00;
	_com_data_queue->EnterOffsetAddr = 0x00;
	_com_data_queue->OutOffsetAddr   = 0x00;
}


/*********************************************
	* 功能描述：进入队列
	* 入口参数：
	* 返回值：
	* 备注：
**********************************************/
void EnterComDataQueue(u8 *SourceBuf,_ComDataQueue *_com_data_queue,u16 DataLen)
{
	//判断队列已满
	if( _com_data_queue->QueueCount >= TEMP_COM_PACK_SIZE )
	{
		_com_data_queue->QueueCount = 0;
		_com_data_queue->EveryPackSize[0] = 0x00;
		_com_data_queue->EnterOffsetAddr = 0x00;
	}
	
	//记录进队列数
	_com_data_queue->QueueCount ++;
	
	//缓存串口数据
	if(DataLen<130)
	{
		if(_com_data_queue->QueueCount<=1)
		{
			memcpy( &_com_data_queue->TempComDataBuffer[0],
					SourceBuf,
					DataLen);
		}
		else
		{
			_com_data_queue->EnterOffsetAddr += _com_data_queue->EveryPackSize[_com_data_queue->QueueCount-2];
			
			//使用上一次数值进行地址偏移
			memcpy( &_com_data_queue->TempComDataBuffer[_com_data_queue->EnterOffsetAddr],
					SourceBuf,
					DataLen);
		}
	}
		
	//记录当前数据包的大小
	_com_data_queue->EveryPackSize[_com_data_queue->QueueCount-1] = hostcom_struct.Comx_RxCnt;
}



/****************************************************
	* 功能描述：出队列
	* 入口参数：
	* 返回值：
	* 备注
******************************************************/
void OutComDataQueue(_ComDataQueue *_com_data_queue,u8 *DestBuf)
{
	//判断全部出队列
	if(_com_data_queue->QueueNowPoint >= TEMP_COM_PACK_SIZE)
	{
		_com_data_queue->QueueNowPoint = 0x00;
		_com_data_queue->OutOffsetAddr = 0x00;
	}
	
	//偏移队列指针
	_com_data_queue->QueueNowPoint ++;
	
	if(_com_data_queue->QueueNowPoint<=1)
	{
		memcpy(	DestBuf,
				&_com_data_queue->TempComDataBuffer[0],
				_com_data_queue->EveryPackSize[_com_data_queue->QueueNowPoint-1]);
	}

	else
	{
		_com_data_queue->OutOffsetAddr += _com_data_queue->EveryPackSize[_com_data_queue->QueueNowPoint-2];
		
		//使用上一次的偏移，大小为当前的记录
		memcpy(	DestBuf,
				&_com_data_queue->TempComDataBuffer[_com_data_queue->OutOffsetAddr],
				_com_data_queue->EveryPackSize[_com_data_queue->QueueNowPoint-1]);
	}	
}


//保存串口数据
void SaveHostComDataToQueueHandle()
{
	if(hostcom_struct.Comx_RevCompleted == ENABLE)
	{
		hostcom_struct.Comx_RevCompleted = DISABLE;
		EnterComDataQueue(hostcom_struct.Comx_RxBuf,&com_data_queue,hostcom_struct.Comx_RxCnt);
		
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





