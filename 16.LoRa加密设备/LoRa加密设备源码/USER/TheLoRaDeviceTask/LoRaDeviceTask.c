#include "LoRaDeviceTask.h"


/**************************************************
	* 功能描述：LoRa加密设备任务处理
	* 入口参数：
	* 返回值：
	* 备注：
***************************************************/
void TheLoRaEncryptDeviceTask(void)
{	
	//Master处理任务
	if( LoRaDeviceIs == MASTER )
	{
//		WatchDogHandle();					//看门狗任务
		//-----------------------------------------------
		ScanComStatusKey();					//检测串口状态切换按键
		//---------------------------------------------------------
		HostComTimeOutHandle();				//上位机超时等待
		//---------------------------------------------------
		ExportLogHandle();					//导出日志任务
		//------------------------------------------------
		LoRaNodeHandleCommunicateData();	//LoRa节点处理通信数据
		LoRaNodeHandleHostComData();		//LoRa节点处理上位机串口数据
		//---------------------------------------------------------------
		HostComProtocol(&hostcom_struct);	//解析上位机指令任务
		//-------------------------------------------------------
		IndicatorLightMainHandle();			//运行系统指示灯
//		TheTimeUptoUpdateSM4KeyHandle();	//更新密钥
	}
	
	//Slave处理任务
	else if( LoRaDeviceIs == SLAVE )
	{
//		WatchDogHandle();					//看门狗任务
		//-----------------------------------------------
		ScanComStatusKey();					//检测串口状态切换按键
		//---------------------------------------------------------
		HostComTimeOutHandle();				//上位机超时等待
		//---------------------------------------------------
		ExportLogHandle();					//导出日志任务
		//------------------------------------------------
		LoRaNodeHandleCommunicateData();	//LoRa节点处理通信数据
		LoRaNodeHandleHostComData();		//LoRa节点处理上位机串口数据
		LoRaSlaveHandleSm2KeyAgreement();	//从机密钥协商
		//---------------------------------------------------------------
		HostComProtocol(&hostcom_struct);	//解析上位机指令任务
		//-------------------------------------------------------
		IndicatorLightMainHandle();			//运行系统指示灯
	}
}





