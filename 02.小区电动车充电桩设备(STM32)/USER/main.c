#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "spi.h"
#include "w25qxx.h"
#include "key.h" 
#include "timer.h"
#include "Current.h"
#include "WatchDog.h"
#include "BL8025.h" 
#include "RS485.h" 
#include "ACPowerDetection.h" 
#include "Relay.h"

void SysPara_Init(void)   //系统参数
{
	uint8_t i;
	
	W25QXX_Read(uni_SysStatus.u8_buf,FLASH_START_ADD,FLASH_LENGTH);		//读取Flash数据
//W25QXX_Read(uni_SysStatusBak.u8_buf,FLASH_START_ADD,FLASH_LENGTH);
	if(uni_SysStatus.Str.u8_FirstPowerOn == 0xff)		//判断是否第一次上次，flash没写之前为0xff
	{
		uni_SysStatus.Str.u8_FirstPowerOn = 0xAA;			//置位判断标志，下次上电读取时，即为非第一次，不执行该步
		
		uni_SysStatus.Str.u8_CurrentBRT = 2;			//初始化波特率为9600
		uni_SysStatus.Str.u8_DeviceAddress = 1;		//默认地址为1
		uni_SysStatus.Str.u8_TrickleDetectionThreshold = 13;		//涓流检测阈值
		uni_SysStatus.Str.u8_TricklePowerSupplyTime = 2;			//涓流供电时间
		uni_SysStatus.Str.u8_MaxPowerSupply = 14;			//最大供电时间
		uni_SysStatus.Str.u8_JitterTime = 3;			//防抖动时间
		
		uni_SysStatus.Str.u8_KeyAlarmEnable = 0x01;		//  开启门禁报警
		uni_SysStatus.Str.u8_ACPowerOutagesAlarmEnable = 0x01;		//开启交流断电报警
		uni_SysStatus.Str.u8_WatchDogEnable = 0x01;			//开启看门狗报警
		uni_SysStatus.Str.u8_CurrentDetectionPowerTime = 0x02;		//电流检测时间
		
		uni_SysStatus.Str.u8_RelayStatusBuf[0] = 0;			//所有继电器关闭
		uni_SysStatus.Str.u8_RelayStatusBuf[1] = 0;	
		uni_SysStatus.Str.u8_RelayStatusBuf[2] = 0;	
		uni_SysStatus.Str.u8_RelayStatusBuf[3] = 0;	
		
		for(i=0;i<32;i++)			//继电器运行时间清零
		{
			uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0;
		}
	}
	
	W25QXX_Write(uni_SysStatus.u8_buf,FLASH_START_ADD,FLASH_LENGTH);		 	//将参数保持到Flash
	delay_ms(5);
	W25QXX_Read(uni_SysStatusBak.u8_buf,FLASH_START_ADD,FLASH_LENGTH);		//再次读出到备份数据
	
	u8_RelayStatus1_8 = uni_SysStatus.Str.u8_RelayStatusBuf[0];				//将继电器状态赋值给控制变量
	u8_RelayStatus9_16 = uni_SysStatus.Str.u8_RelayStatusBuf[1];
	u8_RelayStatus17_24 = uni_SysStatus.Str.u8_RelayStatusBuf[2];
	u8_RelayStatus25_32 = uni_SysStatus.Str.u8_RelayStatusBuf[3];
}
int main(void)
{ 	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);     //初始化延时函数
	TIM3_Int_Init(1000-1,84-1);	//定时器时钟84M，分频系数84，所以84M/84=1Mhz的计数频率，计数1000次为1ms     
		
	KEY_Init(); 				//按键初始化    门禁检测
	Relay_GPIO_Init();
	Current_Init();
	ACPowerDetection_GPIO_Init();
	
	W25QXX_Init();			//W25QXX初始化
//W25QXX_Erase_Chip();
	SysPara_Init();
	
	RS485_Init();	
	BL8025_Init();			//时钟芯片初始化

	WatchDogGPIO_Init();
	WatchDogGPIO_Enable();
	
	while(1)
	{
		BL8025_Process();  //初始化实时时间，设置时间,抖动计时,工作时间计时,涓流计时,
		
		ACPowerDetection_Process(); //交流电检测
		
		
	  Key_Process(); //检测是个有门禁告警
		
		
		RS485_Process(); //接收主机发下来的数据  //有两句不理解
		
		
		Relay_Process(); //判断继电器的状态
		
		
		WatchDog_Feed(); //喂养看门狗
		
		
		Current_Process();//电流进程
		
		
		W25QXX_Process();	//系统状态写入FLASH
	}       
}

