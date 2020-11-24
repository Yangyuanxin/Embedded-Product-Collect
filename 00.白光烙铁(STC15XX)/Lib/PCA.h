
#ifndef	__PCA_H
#define	__PCA_H

#include	"config.h"

/***************************用户宏定义*******************************************************/
#define		PWM0_DUTY		4000		//定义PWM的周期，数值为PCA所选择的时钟脉冲个数。
#define		PWM0_HIGH_MIN	80			//限制PWM输出的最小占空比。
#define		PWM0_HIGH_MAX	(PWM0_DUTY - PWM0_HIGH_MIN)		//限制PWM输出的最大占空比。

#define		PWM1_DUTY		3000		//定义PWM的周期，数值为PCA所选择的时钟脉冲个数。
#define		PWM1_HIGH_MIN	80			//限制PWM输出的最小占空比。
#define		PWM1_HIGH_MAX	(PWM1_DUTY - PWM1_HIGH_MIN)		//限制PWM输出的最大占空比。

#define		PWM2_DUTY		2000		//定义PWM的周期，数值为PCA所选择的时钟脉冲个数。
#define		PWM2_HIGH_MIN	80			//限制PWM输出的最小占空比。
#define		PWM2_HIGH_MAX	(PWM2_DUTY - PWM2_HIGH_MIN)		//限制PWM输出的最大占空比。

/********************************************************************************************/

#define	PCA0			0
#define	PCA1			1
#define	PCA2			2
#define	PCA_Counter		3
#define	PCA_P12_P11_P10_P37	(0<<4)
#define	PCA_P34_P35_P36_P37	(1<<4)
#define	PCA_P24_P25_P26_P27	(2<<4)
#define	PCA_Mode_PWM				0x42	//B0100_0010
#define	PCA_Mode_Capture			0
#define	PCA_Mode_SoftTimer			0x48	//B0100_1000
#define	PCA_Mode_HighPulseOutput	0x4c	//B0100_1100
#define	PCA_Clock_1T	(4<<1)
#define	PCA_Clock_2T	(1<<1)
#define	PCA_Clock_4T	(5<<1)
#define	PCA_Clock_6T	(6<<1)
#define	PCA_Clock_8T	(7<<1)
#define	PCA_Clock_12T	(0<<1)
#define	PCA_Clock_Timer0_OF	(2<<1)
#define	PCA_Clock_ECI	(3<<1)
#define	PCA_Rise_Active	(1<<5)
#define	PCA_Fall_Active	(1<<4)
#define	PCA_PWM_8bit	(0<<6)
#define	PCA_PWM_7bit	(1<<6)
#define	PCA_PWM_6bit	(2<<6)


typedef struct
{
	u8	PCA_IoUse;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
	u8	PCA_Clock;	//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
	u8	PCA_Mode;	//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	u8	PCA_PWM_Wide;	//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	u8	PCA_Interrupt_Mode;	//PCA_Rise_Active, PCA_Fall_Active, ENABLE, DISABLE
	u8	PCA_Polity;	//优先级设置	PolityHigh,PolityLow
	u16	PCA_Value;
} PCA_InitTypeDef;

extern	bit		B_Capture0,B_Capture1,B_Capture2;
extern	u8		PCA0_mode,PCA1_mode,PCA2_mode;
extern	u16		CCAP0_tmp,PCA_Timer0;
extern	u16		CCAP1_tmp,PCA_Timer1;
extern	u16		CCAP2_tmp,PCA_Timer2;
void	PCA_Init(u8 PCA_id, PCA_InitTypeDef *PCAx);
void	UpdatePwm(u8 pwm_duty);
void 	PWMn_Update(u8 PCA_id, u16 high);


#endif

