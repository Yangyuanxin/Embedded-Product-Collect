#ifndef _MAIN_H
#define _MAIN_H

#include "led.h"
#include "delay.h"
#include "key.h"
#include "motor.h"
#include "sys.h"
#include "usart.h"
#include "weight.h"
#include "adc.h"
#include "usmart.h"	 	 
#include "timer.h"
#include "userdefinefuntion.h"
#include "wifi_esp8266.h"


//-----------命令--------------------------
#define CMD_REQUEST     "CMD,0,L3,END"  //请求
#define CMD_RESPONSE    "CMD,OK,END"    //应答

  

//--------------宏定义----------------------
#define OP_CMD_MIN_LENGTH   27 //命令最小长度
#define OP_OFFSET1    0    //每个扇区第一个机种数据存放首地址
#define OP_VALID    0x27    //扇区存放数据有效标志位
//------------------------------------------

//-----------流量计值设定------------------
//----------------------------------------

//--------------funtion-------------------ENABLE   DISABLE
#define TIM_ENABLE()    TIM_TemporaryClose(0)  //恢复原来的状态
#define TIM_DISABLE()   TIM_TemporaryClose(0x11)  //保存原来的状态并关闭定时器

#define TIM3_ENABLE()   //TIM_TemporaryClose(0)	//恢复原来的状态
#define TIM3_DISABLE()  //TIM_TemporaryClose(0x10)	//关闭定时器3
#define TIM2_ENABLE()   //TIM_TemporaryClose(0)	//恢复原来的状态
#define TIM2_DISABLE()  //TIM_TemporaryClose(0x01)	//关闭定时器2

#define AT24C_EnableWrite()  AT24C_WP=0;   //允许ROM写操作
#define AT24C_DisableWrite()  AT24C_WP=1;   //禁止ROM写操作
//---------------------------------

//-----------常量------------------
#define FLUX_FLAG_H     0x31    //流量超上限
#define FLUX_FLAG_L     0x32    //流量超下限
#define FLUX_FLAG_OK    0x32    //流量正常
//---------------------------------

//-----Error Code ------------------
#define NOERROR     0x00
#define ERRORCODE_CONNECT   0x01    //WiFi模块未连接
#define ERRORCODE_CMDFAIL   0x02    //接收命令格式错误
#define ERRORCODE_COMM      0x04    //WiFi模块发送数据错误
#define ERRORCODE_GETLINIT  0x08    //上下限获取失败或上下限值不正确
#define ERRORCODE_NOCHKDATA 0x10    //在EEPROM中没有获取到上线限数据，等待传输数据进行设置
#define ERRORCODE_NOWIFI    0x20    //无法连接到指定的WIFI，可能WIFI已经关闭或者密码错误
#define ERRORCODE_WATCHDOGFAIL  0x40    //看门狗失败，网页端会判定设备FAIL

#define ERRORCODE_DEVICE1   0x80    //设备错误,电磁阀已关闭,但流量计还有超出规定的示数则产生该错误代码
#define ERRORCODE_FLUX_H   0x100    //流量超上限,流量超出规定的示数则产生该错误代码
#define ERRORCODE_FLUX_L   0x200    //流量超下限,流量超出规定的示数则产生该错误代码
//-----------------------------------

extern u16 DeviceErrorCode;  
extern u8 ResetLinitFlag;   //设置上下限标志重新设置上下限标志,
extern u8 Wifi_ConnetRightFlag;  //wifi连接正常会置位该标志位
extern u8 Wifi_ConnetSeverFlag;  //wifi连接服务器正常会置位该标志位
extern u8 ResetRomFlag;  //TIM2接收到置位ROM信息的数据会置位该标志
extern u8 Cmd_Flag_OK;   //命令标志,应答OK
extern u8 Cmd_SendErrorFlag;    //发送命令失败时置位该标志,可能发送不完整等 
extern u8 SendFluxDoingFlag; //正在发送流量监控信息时会置位该标志,防止回复中所包含的信息频繁访问ROM
extern u8 Timer3_EnableFlag; //定时器3使能标志,置位该标志将使能定时器3做普通定时器
extern u8 SolenoidValveOnFlag;   //电磁阀打开标准,电磁阀打开将置位该标志
extern u8 FluxStartFlag; //开始监控流量数据标志,进入监控时会置位该标志

extern char RecvicePN[32];  //保存接收到的PN
extern u16 FluxValue_Now;    //实时流量值
extern u16 Flowmeter_H;  //流量上下限
extern u16 Flowmeter_L;  //流量上下限
//----------------------------------

#define GET_VOUT1_VALUE()   ADC_GetAverage(ADC_Channel_1,10)
#define GET_VOUT2_VALUE()   ADC_GetAverage(ADC_Channel_0,10)

void SendWatchdog(void);    //看门狗喂狗
void SendErrorCode(u16* pcode); //发送错误代码到串口
u8 SengFluxDataToSever(u8 signal,u8 fluxResult,u16 nowDat,u16 beforeDat); //--发送流量数据及判断结果到网页，并比较机种是否一致-一致返回0,


#endif




