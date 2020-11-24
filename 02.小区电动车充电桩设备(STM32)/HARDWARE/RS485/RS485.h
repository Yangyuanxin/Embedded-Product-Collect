#ifndef _RS485_H_
#define _RS485_H_

#include "sys.h"



#define RS485_Recv_En()		GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define RS485_Send_En()		GPIO_SetBits(GPIOA,GPIO_Pin_8)


extern uint8_t u8_AlarmStatus;
extern uint8_t u8_RelayStatus1_8;
extern uint8_t u8_RelayStatus9_16;
extern uint8_t u8_RelayStatus17_24;
extern uint8_t u8_RelayStatus25_32;
extern uint8_t u8_RelayStatus1_8Bak;
extern uint8_t u8_RelayStatus9_16Bak;
extern uint8_t u8_RelayStatus17_24Bak;
extern uint8_t u8_RelayStatus25_32Bak;

extern uint8_t u8_RelayOverCurrentStatus1_8;
extern uint8_t u8_RelayOverCurrentStatus9_16;
extern uint8_t u8_RelayOverCurrentStatus17_24;
extern uint8_t u8_RelayOverCurrentStatus25_32;


extern uint8_t u8_RemoveStatus1_8;
extern uint8_t u8_RemoveStatus9_16;
extern uint8_t u8_RemoveStatus17_24;
extern uint8_t u8_RemoveStatus25_32;


extern uint8_t u8_RemoveStatus1_8Bak;
extern uint8_t u8_RemoveStatus9_16Bak;
extern uint8_t u8_RemoveStatus17_24Bak;
extern uint8_t u8_RemoveStatus25_32Bak;

extern uint8_t u8_RemoveStatus1_8Qury;
extern uint8_t u8_RemoveStatus9_16Qury;
extern uint8_t u8_RemoveStatus17_24Qury;
extern uint8_t u8_RemoveStatus25_32Qury;


extern uint8_t u8_TrickleCurrentStatus1_8;
extern uint8_t u8_TrickleCurrentStatus9_16;
extern uint8_t u8_TrickleCurrentStatus17_24;
extern uint8_t u8_TrickleCurrentStatus25_32;

//extern uint8_t u8_RelayWorkMode[32];
//extern uint8_t u8_RelaySetWorkTime[32];
//extern uint8_t u8_RelayRealWorkTime[32];
//extern uint8_t u8_MaxPowerSupply;

//extern uint8_t u8_TrickleDetectionThreshold;
//extern uint8_t u8_TricklePowerSupplyTime;
//extern uint8_t u8_MaxPowerSupplyTime;
//extern uint8_t u8_JitterTime;

//extern uint8_t u8_KeyAlarmEnable;
//extern uint8_t u8_ACPowerOutagesAlarmEnable;
//extern uint8_t u8_WatchDogEnable;
//extern uint8_t u8_CurrentDetectionPowerTime;

//extern uint8_t u8_DeviceAddress;
//extern uint8_t u8_CurrentBRT;

extern uint8_t u8_SendBuf[50];
extern uint8_t u8_SendLength;

void RS485_Init(void);
void RS485_GPIO_Init(void);
void RS485_Process(void);
void RS485_Timer_Ms(void);















#endif

