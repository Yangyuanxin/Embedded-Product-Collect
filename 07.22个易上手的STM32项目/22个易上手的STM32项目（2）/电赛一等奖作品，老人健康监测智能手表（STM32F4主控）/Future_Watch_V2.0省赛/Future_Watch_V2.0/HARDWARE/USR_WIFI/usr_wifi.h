#ifndef _USRWIFI_H_
#define _USRWIFI_H_

#include "sys.h"
#include "delay.h"
#include "usart_2.h"
#include "stdio.h"
#include "gpio.h"

typedef struct 
{
	u8 Wifi_Id[16];		//WIFI的账号
	u8 Wifi_Key[30];	//WIFI的密码
	u8 Wifi_Sta;		//WIFI的状态
						//第7位 表示链接状态	    1链接  0没有链接
						//第6位 表示密码是否保存 1保存  0没有保存
						//第5位 表示账号是否保存 1保存  0没有保存
						//第4位 表示当前数据模式 1命令  0数据
						//第3位 WEBSOCKET 是否建立连接
						//第2位 表示BPTCP链接是否设置	1建立 0关闭
						//第1位 表示WEBTCP链接是否设置	1建立 0关闭
						//第0位 表示WEBTCP是否建立成功
				
}_Wifi_Information;

int Wifi_SendAt(const u8 Commond[]);//wifi发送AT指令
int Wifi_Send_Str(const u8* p);//WIFI发送字符串
int Wait_AtRxd(u16 time);//WIFI等待接收AT的OK
int Wifi_Init(void);//WIFI初始化
int Wifi_DataWait(u16 times);//WIFI等待接收数据{}
int Wifi_CheckSta(u16 times);//WIFI检查自身设置
int Wifi_DataGet(u8 *pstr);//提取获取到的数据
int Wifi_Send2Sever(u8 *pstr);//发送数据到服务器
int Wifi_OutUDP(void);//退出透传模式
int Wifi_InUDP(void);//进入透传模式
int Wifi_Link(u8 *IdStr,u8 *KeyStr);//WIFI链接WIFI
int Wifi_ReStart(void);//WIFI重启
int Wifi_SmartLink(void);//WIFI启动智能链接
int Wifi_GetKey(void);//WIFI获取密码
int Wifi_GetId(void);//WIFI获取账号
int Wifi_SendCheck(const u8 *Str,u8 times);	//WIFI发送检测命令
int Wifi_LinkBP(void);//WIFI连接至血压模块
int Wifi_StartBp(u8 *Bpdata);//WIFI发送开始测量命令并等待返回
int Wifi_LinkServer(void);//WIFI链接服务器
int Set_WebSocket(void);//WIFI建立WEBSOCKET连接

#endif

