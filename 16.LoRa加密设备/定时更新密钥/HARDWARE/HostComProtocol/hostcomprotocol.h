#ifndef __HOSTCOMPROTOCOL_H
#define __HOSTCOMPROTOCOL_H

#include <string.h>
#include "sys.h"
#include "usart.h"
#include "updata_crc.h"
#include "MyApiPath.h"
#include "rtc.h"
#include "dip_switch.h"
#include "xLog.h"
#include "lora.h"
#include "sx1276-LoRa.h"
#include "modules_init.h"


#define		HOST_RX_SIZE			1500
#define		HOST_TX_SIZE			1500
#define		WAIT_HOST_MAX_TIME		20000			//等待上位机最大时延
#define		DEST_DEVICE_MAX_ADDR	0X0F			//目标设备最大地址
#define		PASSWORD				"Bon$on@2018"
#define		SEC_USER				"sec"
#define		AUDIT_USER				"audit"
#define		SYS_USER				"sys"

//枚举类型
typedef enum
{
	User_Audit = 0x12,
	User_Sec,
	User_Sys,
	None
}_HostStatus;

typedef enum
{
	UART1 = 0x01,
	UART2 = 0x02,
	UART3 = 0x03,
	NoneUART
}_WhichUART;


//串口工作模式
typedef enum
{
	ComMode_AT = 0x20,	//AT设置模式
	ComMode_Trans,		//透传模式
	NoneComMode
}_ComWorkMode;

//定义上位机结构体
typedef struct HostComStructure
{
	u32 Bound;
	u8  SysUser[6];
	u8  SysPwd[12];
	u8 	AuditUser[6];
	u8  AuditPwd[12];
	u8  SecUser[6];
	u8  SecPwd[12];
	u8 	Comx_RxBuf[HOST_RX_SIZE];
	u8 	Comx_RevCompleted;
	u16 Comx_RxCnt;
	u8 	Comx_TxBuf[HOST_TX_SIZE];
	u16 Comx_TxCnt;
	_HostStatus host_status;
	_WhichUART 	WhichUart;
} _HostComStructure;


//AT模式计时器
typedef struct AT_TimerStruct
{
	u32 SetAtTimer_5s;		//超时时间为20s
	u32 SetParaTimer_30s;
	u8	SetParaTimerFlag;
}_AT_TimerStruct;


extern _HostComStructure hostcom_struct;
extern _AT_TimerStruct	 at_timer_struct;	//at设置模式计时器
extern _ComWorkMode	com_work_mode;
extern u16 SysYear;
extern u8  SysMonth;
extern u8  SysDate;
extern u8  SysHour;
extern u8  SysMin;
extern u8  SysSec;


void 	HostComSoftWareInit(void);
void 	HostComProtocol(_HostComStructure *HostStruct);
void 	RunComAtModeTimer(void);
void 	HostComTimeOutHandle(void);
void 	HostComSetATCmdACK(u8 *buf,u8 datasize);
void 	ExportLogHandle(void);
void 	JudgeTheHostComWorkMode();

#endif



