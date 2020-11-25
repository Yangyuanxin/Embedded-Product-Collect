#ifndef __FLASH_H
#define __FLASH_H	

#include "sys.h" 
#include "usart.h"
#include "delay.h"
#include "w25qxx_init.h"


#define		UART_DUBUG
#define		PAGE_SIZE			1024


//W25X系列/Q系列芯片列表	   
#define 	W25Q80 						0XEF13 	
#define 	W25Q16 						0XEF14
#define 	W25Q32 						0XEF15
#define 	W25Q64 						0XFFFF
#define 	W25Q128						0XEF17
 
//--------->指令表
#define 	W25X_WriteEnable			0x06 
#define 	W25X_WriteDisable			0x04 
#define 	W25X_ReadStatusReg			0x05 
#define 	W25X_WriteStatusReg			0x01 
#define 	W25X_ReadData				0x03 
#define 	W25X_FastReadData			0x0B 
#define 	W25X_FastReadDual			0x3B 
#define 	W25X_PageProgram			0x02 
#define 	W25X_BlockErase				0xD8 
#define 	W25X_SectorErase			0x20 
#define 	W25X_ChipErase				0xC7 
#define 	W25X_PowerDown				0xB9 
#define 	W25X_ReleasePowerDown		0xAB 
#define 	W25X_DeviceID				0xAB 
#define 	W25X_ManufactDeviceID		0x90 
#define 	W25X_JedecDeviceID			0x9F 

extern u16 W25QXX_TYPE;	//声明变量	   


void 	W25QxxFlashInit(void);													//W25Qxx初始化
u16  	W25QXX_ReadID(void);  	    											//读取FLASH ID
u8	 	W25QXX_ReadSR(void);        											//读取状态寄存器 
void 	W25QXX_Write_SR(u8 sr);  												//写状态寄存器
void 	W25QXX_Write_Enable(void);  											//写使能 
void 	W25QXX_Write_Disable(void);												//写保护
void 	W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void 	W25QXX_Wait_Busy(void);           										//等待空闲
void 	W25QXX_PowerDown(void);        											//进入掉电模式
void 	W25QXX_WAKEUP(void);													//唤醒

void 	W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   				//读取flash
void 	W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);				//写入flash
void 	W25QXX_Erase_Chip(void);    	  										//整片擦除
void 	W25QXX_Erase_Sector(u32 Dst_Addr);										//扇区擦除

/*******************************************/
void 	W25QxxCheckIsWrongProcess(void);
#endif
















