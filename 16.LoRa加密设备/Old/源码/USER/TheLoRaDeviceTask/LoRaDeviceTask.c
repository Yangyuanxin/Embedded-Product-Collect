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
	if(LoRaDeviceIs == MASTER)
	{
		ScanComStatusKey();
		HostComTimeOutHandle();				//上位机超时等待
		ExportLogHandle();					//导出日志任务
		LoRaMasterMainProcess();			//LoRa主机传输任务
		HostComProtocol(&hostcom_struct);	//被上位机处理任务
		IndicatorLightMainHandle();			//运行系统指示灯
	}
	
	//Slave处理任务
	else if( LoRaDeviceIs == SLAVE )
	{
		ExportLogHandle();					//导出日志任务
		LoRaSlaveMainPorcess();				//LoRa从机协议解析任务
		SlaveHandleSm2KeyAgreement();		//从机密钥协商
		HostComProtocol(&hostcom_struct);	//解析上位机指令任务
		IndicatorLightMainHandle();			//运行系统指示灯
	}
}



