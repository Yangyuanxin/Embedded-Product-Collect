#include "setting.h"
#include "main.h"

volatile set_Config setData;
volatile u32 setDataSize=sizeof(setData)/2;
	
//恢复默认设置
void resetData(void)
{
	setData.writeFlag = 0x11;
	setData.setTemp = 280;//设置温度℃
	setData.sleepTime = TIME_SLEEP;//休眠时间0-60min，0min为不自动休眠
	setData.shutTime = TIME_SHUT;//关机时间0-60min，0min为不自动关机
	setData.workMode = 1;//工作模式,1为标准,0为强力
	setData.beeperFlag = 1;//蜂鸣器是否开启,1为开启,0为关闭
	setData.langFlag = 1;//语言设置,1为中文,0为英文
}
void set_Init(void)
{
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//从FLASH中读取结构体
	if(setData.writeFlag!=0x11){//是否第一次写入
		resetData();
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//写入FLASH
	}
}

