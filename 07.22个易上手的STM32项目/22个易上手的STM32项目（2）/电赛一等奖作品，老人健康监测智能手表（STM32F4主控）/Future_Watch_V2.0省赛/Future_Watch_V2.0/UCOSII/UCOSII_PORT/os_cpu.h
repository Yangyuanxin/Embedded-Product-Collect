/************************ (C) COPYLEFT 2010 Leafgrass *************************

* File Name		: os_cpu_c.c 
* Author		: Librae
* Date			: 06/10/2010
* Description	: μCOS-II在STM32上的移植代码C语言部分，
*				  包括任务堆栈初始化代码和钩子函数等

******************************************************************************/

#ifndef	__OS_CPU_H__
#define	__OS_CPU_H__

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/******************************************************************************
*                    定义与编译器无关的数据类型
******************************************************************************/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;			/* Unsigned  8 bit quantity       */
typedef signed   char  INT8S;			/* Signed    8 bit quantity       */
typedef unsigned short INT16U;			/* Unsigned 16 bit quantity       */
typedef signed   short INT16S;			/* Signed   16 bit quantity       */
typedef unsigned int   INT32U;			/* Unsigned 32 bit quantity       */
typedef signed   int   INT32S;			/* Signed   32 bit quantity       */
typedef float          FP32;			/* Single precision floating point*/
typedef double         FP64;			/* Double precision floating point*/

//STM32是32位位宽的,这里OS_STK和OS_CPU_SR都应该为32位数据类型
typedef unsigned int   OS_STK;			/* Each stack entry is 32-bit wide*/
typedef unsigned int   OS_CPU_SR;		/* Define size of CPU status register*/
/* 
*******************************************************************************
*                             Cortex M3
*                     Critical Section Management
*******************************************************************************
*/


/*
*******************************************************************************
*                          ARM Miscellaneous
*******************************************************************************
*/


//定义栈的增长方向.
//CM3中,栈是由高地址向低地址增长的,所以OS_STK_GROWTH设置为1
#define  OS_STK_GROWTH        1      /* Stack grows from HIGH to LOW memory on ARM    */
//任务切换宏,由汇编实现.
#define  OS_TASK_SW()         OSCtxSw()

/*
*******************************************************************************
*                               PROTOTYPES
*                           (see OS_CPU_A.ASM)
*******************************************************************************
*/
//OS_CRITICAL_METHOD = 1 :直接使用处理器的开关中断指令来实现宏 
//OS_CRITICAL_METHOD = 2 :利用堆栈保存和恢复CPU的状态 
//OS_CRITICAL_METHOD = 3 :利用编译器扩展功能获得程序状态字，保存在局部变量cpu_sr

#define  OS_CRITICAL_METHOD   3	 	//进入临界段的方法

#if OS_CRITICAL_METHOD == 3
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
#endif

void       OSCtxSw(void);
void       OSIntCtxSw(void);
void       OSStartHighRdy(void);

void       OSPendSV(void);

#if OS_CRITICAL_METHOD == 3u                      /* See OS_CPU_A.ASM                                  */
OS_CPU_SR  OS_CPU_SR_Save(void);
void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#endif
OS_CPU_EXT INT32U OSInterrputSum;

#endif

/************************ (C) COPYLEFT 2010 Leafgrass ************************/
