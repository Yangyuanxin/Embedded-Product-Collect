#ifndef __PWM_H__
#define __PWM_H__
#include"sys.h"
#include"delay.h"


/*##################需配置的参数#################*/
/*------------------1、使能通道------------------*/
#define CH1_ENABLE
#define CH2_ENABLE
//#define CH3_ENABLE
//#define CH4_ENABLE
/*------------------2、配置相关寄存器-------------*/
#define PWM_ARR_REG 665  //配置ARR寄存器


 
void PWM_Init(void);
void zhengzhuan(void);
void zhengzhuan2(void);
void fanzhuan(void);
void tingzhi(void);

#ifdef CH1_ENABLE
void PWM_SetCH1_dura(u8 rate);
#endif
#ifdef CH2_ENABLE
void PWM_SetCH2_dura(u8 rate);
#endif
#ifdef CH3_ENABLE
void PWM_SetCH3_dura(u8 rate);
#endif
#ifdef CH4_ENABLE
void PWM_SetCH4_dura(u8 rate);
#endif

#endif

