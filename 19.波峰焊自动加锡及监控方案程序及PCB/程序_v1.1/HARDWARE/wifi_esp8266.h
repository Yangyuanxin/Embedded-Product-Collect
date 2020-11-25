#ifndef _WIFI_ESP8266_h
#define _WIFI_ESP8266_h

#include "sys.h"
#include "main.h"

extern char WifiCmd_Mode[];
extern char WifiCmd_RST[];
extern char WifiCmd_SendSSID[];
extern char WifiCmd_MuxMode[];
extern char WifiCmd_ConnetWIFI[];
extern char WifiCmd_ConnetSever[];

u8 WIFI_SendCmd(char* cmd,char* ack,char* orack1,char* orark2,u16 waitTime);
u8 WIFI_SendData(char* dat,char* ack,u16 waittime);
u8 WIFI_Config(void);
u8 WIFI_ConnetSever(void);

void Str_DoubleToStr(char* p, double x, u8 num);

void StrToCmd(char* p,char* cmds);
#endif



