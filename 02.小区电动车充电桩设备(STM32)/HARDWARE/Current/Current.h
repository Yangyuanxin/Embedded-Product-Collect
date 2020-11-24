#ifndef __CURRENT_H
#define __CURRENT_H	

#include "sys.h" 
#include "adc3.h" 
  
#define ADC_READ_TIMES			10	

#define CHECK_READ_TIMES		0//10	

#define MIN_CURRENT_CHECK		90

extern uint16_t u16_RelayCurrentADCBuf[32];
extern uint16_t u16_RelayCurrentBuf[32];
extern uint8_t u8_RelayRemoveCntBuf[32];


extern UniBitDef uniCurrentDectect;

#define	 u1_NeedDectCurrentF				uniCurrentDectect.bits.b0
#define	 u1_ForceDectCurrentEndF		uniCurrentDectect.bits.b1
#define	 u1_isOnlineDectF						uniCurrentDectect.bits.b2
#define	 u1_isOnlineDectEndF				uniCurrentDectect.bits.b3
#define	 u1_isOnlineUpDataF					uniCurrentDectect.bits.b4

extern uint8_t u8_NeedDectCh;
extern __IO uint16_t u16_CurrentCheckTimeMs;
extern __IO uint16_t u16_CurrentForceCheckTimeMs;
extern __IO uint16_t u16_CurrentForceCheckTimeS;

extern uint16_t u16_IsOnlineCheckTimeS;

void Current_Init(void); 				
void ReadSetChCurrentValue(uint8_t ch);
void SetADCCh(uint8_t ch);
uint8_t Current_Get_Val(void);			
void Current_Process(void);
void Current_Timer_Ms(void);

#endif 





















