#include "modules_init.h"

extern u8 SelfID_Buf[16];
float VoltageValue = 0.0;

//硬件模块初始化
void ModulesInit(void)
{
	SysNVIC_Config();				//配置中断分组为2(2:2)
	delay_init();					//初始化延时函数(配置滴答定时器)
	LED_Init();						//初始化LED硬件		
//	WatchDogPinInit();				//看门狗初始化
	DIP_SwitchInit();				//拨码开关初始化
	ComStatusChangeKeyInit();		//初始化串口状态切换按键
	AlamKeyPinInit();				//初始化报警按键
//	Adc_Init();						//++
	OrdinaryTimerInit(3,10,7200);	//初始化定时器3
	AdvanceTimerInit(1,10,7200);	//初始化定时器1(1ms)
	RTC_Init();						//初始化内部时时钟
	LoadWriteAddrPowerOnDevice();	//加载上一次的日志写入地址
	W25QxxFlashInit();				//初始化W25Qxx芯片
	HostComSoftWareInit();			//初始化上位机软件
	MyLoRaInit();					//初始化LoRa模块
	HSC32i1Init(SelfID_Buf,16);		//初始化HSC32以及SM2密钥协商
	TimerDelay_Ms(500);
//	VoltageValue = Get_Adc_Average(15,10);
	//****保存系统日志(设备上电)
	RestoreWhichLog(SystemLogFlag,SystemLogBuf,DEVICE_POWER_CODE,0,Log_Successful);
}






