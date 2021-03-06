#include "lora.h"


/***********************************************************************************
									变量区
************************************************************************************/
	extern u8 	LoRaDeviceIs;
	//--------------------------------------------------------
	u16	LoRaTransCounter = 0x00;	//传输计时器(收/发总时长)
	u16 WaitIDCounter = 0x00;		//等待对方ID计时器
	static u8	UPwr = 20;			//发射功率	--	5~20dBm
	static u32 	Freq = 0;			//发射频率	
/***********************************************************************************/


	
/**************************************************
	* 功能描述：LoRa模块初始化
	* 入口参数：无
	* 返回值：	无
	* 备注：	检测设备是否正常；配置LoRa参数
***************************************************/
void MyLoRaInit(void)
{
	UPwr 	= 20;			//发射功率为20dBm
	Freq 	= 433000000;	//设置频率470Mhz 433

	LoRaPWR_Init();			//开启LoRa电源
	SetLoRaStruPara();		//LoRa结构体初始化
	SX1276Reset();			//重启SX1276模块

	if( SX1276CheckSPI() )
	{
//		printf("xxxxxxxxx未检测到ZM470SX设备！请检查设备...\r\n");
		
		//****保存系统日志(LORA模块异常)
		RestoreWhichLog(SystemLogFlag,SystemLogBuf,LORA_MOD_ERROR,0,Log_Wrong);
	}
	else
	{
//		printf("*********检测到ZM470SX设备！\r\n");
		
		//****保存系统日志(启动LoRa通信功能)
		RestoreWhichLog(SystemLogFlag,SystemLogBuf,STATR_LORA_COMM_CODE,0,Log_Successful);
	}
	SX1276Init();			//初始化SX1276模块	
    SX1276FreqSet(Freq);	//设置频率为470MHz
    SX1276TxPower(UPwr);	//设置发射功率为20dBM
	SX1276RxStateEnter();	//进入接收状态
}


//进入待机状态,电流
void SetLoRaStandBy(void)
{
	SX1276SetOpMode(RFLR_OPMODE_STANDBY);	
}


//进入休眠,电流1.2uA
void SetLoRaSleep(void)
{
	SX1276SetOpMode(RFLR_OPMODE_SLEEP);	   
}


/*****************************************
	* 功能描述：运行LoRa模块传输计时器
	* 入口参数：无
	* 返回值：	无
	* 备注：	无(时间阈值为500ms)
******************************************/
void RunLoRaTransCounter(void)
{
	if(LoRaTransCounter<6000)
	{
		LoRaTransCounter ++;
	}
}


/*****************************************
	* 功能描述：运行等待ID计时器
	* 入口参数：无
	* 返回值：	无
	* 备注：	无(时间阈值为500ms)
******************************************/
void RunWaitIDCounter(void)
{
	if( WaitIDCounter<5000 )
	{
		WaitIDCounter ++;
	}
}


/****************************************
	* 功能描述：LoRa模块接收数据
	* 入口参数：
	* 返回值：
	* 备注：	
*****************************************/
void ReceiveData(u8 *LoRaBuf,u8 *RevCnt)
{
    //如果DIO0输出高电平，则接收到数据
    if( DIO0 == 1 )
    {
		//读取RF芯片FIFO中的数据(使用CRC32校验)
        SX1276RxDataReadCRC32(LoRaBuf, RevCnt);
	}
    else
    {
		//... ...
    }
}
//--------->以前的版本
//void ReceiveData(u8 *LoRaBuf,u8 *RevCnt)
//{
//	u8 i = 0;
//	u8 rev_lora_data_size = 0x00;
//	
//    //如果DIO0输出高电平，则接收到数据
//    if(DIO0 == 1)
//    {
//        printf("开始接收的时间为	--	T1:%dms\r\n", LoRaTransCounter);
//		
//		//读取RF芯片FIFO中的数据(使用CRC32校验)
//        SX1276RxDataReadCRC32( LoRaBuf, RevCnt );
//		
//        printf("接收完成的时间为	--	T2:%dms\r\n", LoRaTransCounter);
//        printf("接收到的数据为：");
//		rev_lora_data_size = *RevCnt;
//		for(i=0;i<rev_lora_data_size;i++)
//			printf(" %2x",LoRaPact.RxBuf[i]);
//    }
//    else
//    {
//        printf("xxxxxxx接收不成功xxxxxxx\r\n");
//    }

//    printf("\r\n总的耗时为		--	T3：%dms\r\n", LoRaTransCounter);
//    printf("**************************************************\r\n");
//    printf("\r\n");
//}






