#ifndef _KI_H
#define _KI_H


#ifndef _KI_C
#define EXT_KI	extern 
#else
#define EXT_KI
#endif
/******************开入序号，从0开始,存放在yx缓冲区内********************/
//#define POWER_RUNNING       0 流量脉冲1
//#define LENGQUE_H           1流量脉冲2
#define LENGQUE_L           2
/***********
#define BY_KI1            3
#define BUSHUI_L            4
**************/
#define JIAYAO1_L           5
#define JIAYAO2_L           6
#define JIAYAO3_L           7
#define JIAYAO4_L           8

#define PAIWU_G             9
#define PAIWU_K             10
//#define PAIWU_MANUAL       11
//#define BUSHUI_MANUAL       12
//#define GONGSHUUI_SELECT       13
//#define E_STOP       14
//#define POWER_DROP    15
/****虚拟遥信******/
#define JIAYAO1_STATE         16
#define JIAYAO2_STATE         17
#define JIAYAO3_STATE         18
#define JIAYAO4_STATE         19
#define PAIWU_HUANSHUI        20
#define KAIPAIWU_FAIL         33

#define KI0		(1<<0)
#define KI1		(1<<1)
#define KI2		(1<<2)
#define KI3		(1<<3)
#define KI4		(1<<4)
#define KI5		(1<<5)
#define KI6		(1<<6)
#define KI7		(1<<7)
#define KI8		(1<<8)
#define KI9		(1<<9)
#define KIA		(1<<10)
#define KIB		(1<<11)
#define KIC		(1<<12)
#define KID		(1<<13)
#define KIE		(1<<14)
#define KIF 	        (1<<15)


#define GET_KI0()		GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6)	
#define GET_KI1()		GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7)
#define GET_KI2()		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)
#define GET_KI3()		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4)
#define GET_KI4()		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)	
#define GET_KI5()		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)
#define GET_KI6()		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
#define GET_KI7()		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)
#define GET_KI8()		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)	
#define GET_KI9()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0)
#define GET_KIA()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)
#define GET_KIB()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)
#define GET_KIC()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)	
#define GET_KID()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)
#define GET_KIE()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)
#define GET_KIF()		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6)

#define KI_NUM       6//开入有几个半字个数

EXT_KI vu32 F_Count;
EXT_KI vu32 F_Count1;
EXT_KI u8 yx_change_flag;//供液晶显示用
//EXT_KI UINT16 ki_buf[KI_NUM],ki_OldBuf[KI_NUM];
//EXT_KI u16 io_buffer[3];
EXT_KI void Init_Ki(void);
EXT_KI void ProKi(void);
EXT_KI void Process_Yx_Change_Flag(void);

#endif


