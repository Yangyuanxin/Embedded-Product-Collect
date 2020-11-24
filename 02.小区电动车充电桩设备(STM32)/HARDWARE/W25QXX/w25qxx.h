#ifndef __W25QXX_H
#define __W25QXX_H			    
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//W25QXX 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

extern u16 W25QXX_TYPE;					//定义W25QXX芯片型号		   

//#define	W25QXX_CS 		PBout(14)  		//W25QXX的片选信号
#define	W25QXX_CS 		PAout(11)  		//W25QXX的片选信号


#define	FLASH_SIZE 		8388608								//16777216		//16*1024*1024;

#define FLASH_START_ADD		FLASH_SIZE-200		//定义使用的起始地址

#define	FLASH_TrickleDetectionThreshold_ADD			FLASH_START_ADD	+	1
#define	FLASH_TricklePowerSupplyTime_ADD				FLASH_START_ADD	+	2
#define	FLASH_MaxPowerSupplyTime_ADD						FLASH_START_ADD	+	3
#define	FLASH_JitterTime_ADD										FLASH_START_ADD	+	4
#define	FLASH_KeyAlarmEnable_ADD								FLASH_START_ADD	+	5
#define	FLASH_ACPowerOutagesAlarmEnable_ADD			FLASH_START_ADD	+	6
#define	FLASH_WatchDogEnable_ADD								FLASH_START_ADD	+	7
#define	FLASH_CurrentDetectionPowerTime_ADD			FLASH_START_ADD	+	8
#define	FLASH_DeviceAddress_ADD									FLASH_START_ADD	+	9
#define	FLASH_CurrentBRT_ADD										FLASH_START_ADD	+	10


#define FLASH_LENGTH		150

typedef union _flash
{
	uint8_t  u8_buf[FLASH_LENGTH];
	
	struct _str
	{
		uint8_t u8_FirstPowerOn;
		
		uint8_t u8_DeviceAddress;
		uint8_t u8_CurrentBRT;
		uint8_t u8_BL8025TInit;
		uint8_t u8_TrickleDetectionThreshold;
		uint8_t u8_TricklePowerSupplyTime;
		uint8_t u8_MaxPowerSupplyTime;
		uint8_t u8_JitterTime;
		uint8_t u8_KeyAlarmEnable;
		uint8_t u8_ACPowerOutagesAlarmEnable;
		uint8_t u8_WatchDogEnable;
		uint8_t u8_CurrentDetectionPowerTime;
		
		uint8_t u8_MaxPowerSupply;
		
		uint8_t u8_RelayStatusBuf[4];
		uint8_t u8_RelayWorkModeBuf[32];
		uint8_t u8_RelayWorkSetTimeBuf[32];
		uint8_t u8_RelayWorkRunTimeBuf[32];
		
	}Str;
}FlashDataBufDef;


extern FlashDataBufDef uni_SysStatus,uni_SysStatusBak;	

////////////////////////////////////////////////////////////////////////////////// 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

void W25QXX_Init(void);
u16  W25QXX_ReadID(void);  	    		//读取FLASH ID
u8	 W25QXX_ReadSR(void);        		//读取状态寄存器 
void W25QXX_Write_SR(u8 sr);  			//写状态寄存器
void W25QXX_Write_Enable(void);  		//写使能 
void W25QXX_Write_Disable(void);		//写保护
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void W25QXX_Erase_Chip(void);    	  	//整片擦除
void W25QXX_Erase_Sector(u32 Dst_Addr);	//扇区擦除
void W25QXX_Wait_Busy(void);           	//等待空闲
void W25QXX_PowerDown(void);        	//进入掉电模式
void W25QXX_WAKEUP(void);				//唤醒

void W25QXX_Process(void);
#endif
















