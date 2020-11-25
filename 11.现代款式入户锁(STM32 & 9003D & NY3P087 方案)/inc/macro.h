#ifndef __MACRO_H__
#define __MACRO_H__
/*******************************************************************************/
/** 数组大小 **/
#define	TABSIZE(xxx)	(sizeof(xxx)/sizeof(xxx[0]))

/** 背光LED **/
#define	MBlackLED_off() (GPIO_ResetBits(GPIOB, GPIO_Pin_0))
#define	MBlackLED_on() (GPIO_SetBits(GPIOB, GPIO_Pin_0))
#define	MBlackLED_status() (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0))

/** 指纹头供电 **/
#define	MFingerPower_off() (GPIO_ResetBits(GPIOB, GPIO_Pin_2))
#define	MFingerPower_on() (GPIO_SetBits(GPIOB, GPIO_Pin_2))

/** 主IC供电线路 **/
//#define	MMasterPower_off() (GPIO_ResetBits(GPIOB, GPIO_Pin_9))
#define	MMasterPower_off() do{GPIO_ResetBits(GPIOB, GPIO_Pin_9);}while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))
#define	MMasterPower_on() (GPIO_SetBits(GPIOB, GPIO_Pin_9))
 /*****************************************************************/
/** 操作指示灯 **/
//#define	MOPGreen_off() (GPIO_SetBits(GPIOA, GPIO_Pin_0))
//#define	MOPGreen_on() do{GPIO_ResetBits(GPIOA, GPIO_Pin_0); GPIO_SetBits(GPIOA, GPIO_Pin_1);}while(0)

#define	MOPBlue_off() do{GPIO_SetBits(GPIOA, GPIO_Pin_0);}while(0)
#define	MOPBlue_on() do{GPIO_ResetBits(GPIOA, GPIO_Pin_0); GPIO_SetBits(GPIOA, GPIO_Pin_1);}while(0)
#define	MOPBlue_setDefaultStatus() do{\
	if(MBlackLED_status()){MOPBlue_on();}else{MOPBlue_off();}\
							}while(0)

#define	MOPRed_off() do{GPIO_SetBits(GPIOA, GPIO_Pin_1);}while(0)	
#define	MOPRed_on() do{GPIO_ResetBits(GPIOA, GPIO_Pin_1); GPIO_SetBits(GPIOA, GPIO_Pin_0);}while(0)

/** 语音控制 **/
#if	1
//#define	VOP_H() (GPIO_SetBits(GPIOA, GPIO_Pin_4))
//#define	VOP_L() (GPIO_ResetBits(GPIOA, GPIO_Pin_4))

#define	VOPDATA_H() (GPIO_SetBits(GPIOA, GPIO_Pin_4))
#define	VOPDATA_L() (GPIO_ResetBits(GPIOA, GPIO_Pin_4))

#define	VOPRESET_H() (GPIO_SetBits(GPIOA, GPIO_Pin_6))
#define	VOPRESET_L() (GPIO_ResetBits(GPIOA, GPIO_Pin_6))

#endif
/** 锁体电机 **/
#define	MLock()  do{\
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);\
		GPIO_SetBits(GPIOC, GPIO_Pin_7);\
				}while(0)
#define	MUnLock()  do{\
	GPIO_SetBits(GPIOC, GPIO_Pin_6);\
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);\
				}while(0)
#define	MLockPWR_off()  do{\
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);\
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);\
			}while(0)

/** 感应供电开关 **/
#define	MPKEPower_off() (GPIO_ResetBits(GPIOC, GPIO_Pin_9))
#define	MPKEPower_on() (GPIO_SetBits(GPIOC, GPIO_Pin_9))
/** 无钥匙进入感应信号 **/


/** 指纹头感应上电功能 **/
#define MFP_InductionPower_disable()	do{g_flash.arrChar[CFLASH_INDUCTION_POWER] = CINDUCTION_POWER_DISABLE;}while(0)
#define MFP_InductionPower_enable()		do{g_flash.arrChar[CFLASH_INDUCTION_POWER] = CINDUCTION_POWER_ENABLE;}while(0)
#define	MFP_isInductionPower()		(g_flash.arrChar[CFLASH_INDUCTION_POWER] != CINDUCTION_POWER_DISABLE)
#define	MFP_isON()		((g_flag & (1 << 6)) == (1 << 6))
/*******************************************************************************/
#endif
