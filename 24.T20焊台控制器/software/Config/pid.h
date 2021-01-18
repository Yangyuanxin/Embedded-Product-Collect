#ifndef __PID_H
#define __PID_H
#include "sys.h"

typedef struct PID_Value
{
	u32 liEkVal[3];                     //差值保存，给定和反馈的差值
	u8 uEkFlag[3];                        //符号，1则对应的为负数，0为对应的为正数   
	u8 uKP_Coe;                                //比例系数
	u8 uKI_Coe;                                //积分常数
	u8 uKD_Coe;                                //微分常数
	u16 iPriVal;             //上一时刻值
	u16 iSetVal;             //设定值
	u16 iCurVal;             //实际值
}PID_ValueStr;

extern volatile u32 nowTime;
extern volatile u32 sleepCount;
extern volatile u32 shutCount;
extern volatile int uk;
void HEAT_Init(void);
void PID_Setup(void);
void PID_Output(void);
void PID_Operation(void);
void TIM4_Counter_Init(u16 arr,u16 psc);
void getClockTime(char timeStr[]);

#endif
