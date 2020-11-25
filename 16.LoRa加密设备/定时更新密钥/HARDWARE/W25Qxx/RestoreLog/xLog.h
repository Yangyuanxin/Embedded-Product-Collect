#ifndef __XLOG_H
#define	__XLOG_H

#include "sys.h"
//#include "logs.h"
#include "xLog.h"
#include "sx1276-LoRa.h"
#include "MasterLoRaHandle.h"
#include "w25qxx_flash.h"



//操作日志代码
#define		SYS_CODE						0			//系统管理员代码
#define		AUDIT_CODE						1			//安全管理员代码
#define		SEC_CODE						2			//审计管理员代码
//--------------------------------------------------------------------
#define		USER_CONFIRM_CODE				0x00		//用户确认
#define		CHANGE_PWD_CODE					0x01		//更改密码
#define		SET_SYS_TIME_CODE				0x02		//设置系统时间
#define		READ_SYS_TIME_CODE				0x03		//读取系统时间
#define		SET_LORA_PARA_CODE				0x04		//设置LoRa参数
#define		READ_LORA_PARA_CODE				0x05		//读取LoRa参数
#define		READ_SM4_KEY_CODE				0x06		//读取SM4密钥
#define		READ_RANGDOM_CODE				0x07		//读取随机数
#define		READ_OL_CODE					0x08		//读取操作日志
#define		READ_SL_CODE					0x09		//读取系统日志
#define		READ_CL_CODE					0x0A		//读取通信日志
#define		READ_EL_CODE					0x0B		//读取加密日志
#define		SET_DEST_ID_CODE				0x0C		//设置目标设备的ID
#define		READ_DEST_ID_CODE				0x0D		//读取目标设备的ID
#define		READ_UART_PARA_CODE				0x0E		//读取串口参数
#define		USER_REST_CODE					0x0F		//恢复出厂设置
#define		STOP_READ_LOG_CODE				0x10		//停止读取日志
#define		UPDATE_FIRM_CODE				0X11		//升级固件


//系统日志代码
#define		DEVICE_POWER_CODE				0x00		//设备上电
#define		HSC32_CHECK_CODE				0x01		//加密芯片自检
#define		SET_DEVICE_ADDR_CODE			0x03		//设置设备地址
#define		START_WTACH_DOG_CODE			0x04		//启动看门狗功能
#define		START_HOST_MANGE_CODE			0x05		//启动上位机管理功能
#define		STATR_LORA_COMM_CODE			0x06		//启动LORA通信功能
#define 	SET_UART_PARA_CODE				0x07		//设置串口参数
#define		CHECK_ILLEGAL_DISABLE_CODE		0x13		//检测到非法拆卸
#define		DEVICE_RESET_CODE				0x16		//恢复出厂设置
#define		LORA_MOD_ERROR					0x17		//LoRa模块异常


/***************************************************************************************************
										存储日志地址划分
****************************************************************************************************/
#define		MIN_LOG_PACK_SIZE				16		//小日志包大小
#define		MAX_LOG_PACK_SIZE				1100	//大日志包大小
#define		ENCRYPT_DATA					1
#define		DECRYPT_DATA					0

//保存设置参数地址
#define		SAVE_SET_PARA_ADDR				0X00
//保存设置参数所用存储空间大小
#define		SAVE_SET_PARA_SIZE				(SAVE_SET_PARA_ADDR+0X19000)			//100KB

#define		SETED_USER_INFO					0x0088
#define		SYS_INFO_ADDR					0X0000
#define  	AUDIT_INFO_ADDR					0X0400
#define		SEC_INFO_ADDR					0X0800
#define		LORA_INFO_ADDR					0X0C00
#define		SAVE_OPERATE_ADDR				0X


//升级APP地址
#define		UPDATE_APP_ADDR					0X19000
//升级APP所用存储空间大小
#define		UPDATE_APP_SIZE					(UPDATE_APP_ADDR+0x7D000)				//500KB

//操作日志基地址
#define		OPERATE_LOG_BASE_ADDR			0x96000	
//操作日志大小
#define		OPERATE_LOG_SZIE				(OPERATE_LOG_BASE_ADDR + 0x35000)		//212KB

//系统日志基地址
#define		SYSTEM_LOG_BASE_ADDR			0xCB400		//0XCB000 + 0X400
//系统日志大小(100*100=10000)
#define		SYSTEM_LOG_SZIE					(SYSTEM_LOG_BASE_ADDR + 0X35000) 		//212KB

//通信日志基地址
#define		COMMUNICATE_LOG_BASE_ADDR		0X100800	//0X100400 +0X400
//通信日志大小
#define		COMMUNICATE_LOG_SIZE			(COMMUNICATE_LOG_BASE_ADDR+0x177FA0)	//1100*1400
//#define		COMMUNICATE_LOG_SIZE			(COMMUNICATE_LOG_BASE_ADDR+1100*10)

//加密日志基地址
#define		ENCRYPT_LOG_BASE_ADDR			0X278BA0 	//0X2787A0 + 0X400
//加密日志大小
#define		ENCRYPT_LOG_SZIE				(ENCRYPT_LOG_BASE_ADDR+0x177FA0)		//1100*1400
//#define		ENCRYPT_LOG_SZIE				(ENCRYPT_LOG_BASE_ADDR+1100*10)
/***************************************************************************************************/

typedef enum
{
	Log_Successful = 1,
	Log_Wrong = 0,
	Log_None = 0xf
}_LogResult;


//日志类型
typedef enum
{
	OperateLogFlag = 0xDE,
	SystemLogFlag,
	CommunicateLogFlag,
	EncryptFlag,
	NoneLogFlag,
}_LogTypes;


//日志类型
typedef enum
{
	LogWriteAddr_Operate = 0x1E,
	LogWriteAddr_System,
	LogWriteAddr_Communicate,
	LogWriteAddr_Encrypt,
	LogWriteAddr_None,
}_LogWriteAddr;


//声明变量
extern u8 	OperateLogBuf[16];			
extern u8 	SystemLogBuf[16];	
extern u8 	CommunicateLogBuf[1100];		
extern u8  	EncryptLogBuf[1100];			
extern u32 	OperateWriteAddr;			
extern u32	OperateReadAddr;				
extern u32 	SystemWriteAddr;				
extern u32	SystemReadAddr;				
extern u32 	CommunicateWriteAddr;		
extern u32	CommunicateReadAddr;			
extern u32 	EncryptWriteAddr;			
extern u32	EncryptReadAddr;				


void 	ExportWhichLog(u8 LogType,u8 *DestBuf);
void 	RestoreWhichLog(u8 LogType,u8 *SourceBuf,u16 RestoreCode,u8 AdminCode,u8 Result);
void 	RestoreWriteAddr(u8 WhichWriteAddr);
void 	LoadWriteAddrPowerOnDevice(void);
void 	RestoreUserPassword(u8 *PwdBuf, u8 PwdSize,u32 where_addr);

#endif







