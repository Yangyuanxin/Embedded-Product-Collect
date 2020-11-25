//*
//*********************************************************************************************************
//*                                               uC/OS-II
//*                                         The Real-Time Kernel
//*
//*
//*                                (c) Copyright 2006, Micrium, Weston, FL
//*                                          All Rights Reserved
//*
//*                                           ARM Cortex-M3 Port
//*
//* File      : OS_CPU.H
//* Version   : V2.86
//* By        : Jean J. Labrosse
//*
////* For       : ARMv7M Cortex-M3
//* Mode      : Thumb2
//* Toolchain : IAR EWARM 5.20
//*********************************************************************************************************
//*

#ifndef  OS_CPU_H
#define  OS_CPU_H


#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

//*
//*********************************************************************************************************
//*                                              DATA TYPES
//*                                         (Compiler Specific)
//*********************************************************************************************************
//*

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef    char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */

#define UINT8	INT8U
#define INT8	INT8S
#define UINT16	INT16U
#define INT16	INT16S
#define UINT32	INT32U
#define INT32	INT32S
#define uint32	INT32U
#define uint16	UINT16
#define uint8	UINT8
#define int32	INT32

/********GUI*********/
#define int8u  INT8U
#define int32s INT32S
#define int16u INT16U
#define int8s  INT8S
#define int16s INT16S

//*
//*********************************************************************************************************
//*                                              Cortex-M1
//*                                      Critical Section Management
//*
//* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
//*             will be enabled even if they were disabled before entering the critical section.
//*             NOT IMPLEMENTED
//*
//* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
//*             interrupts were disabled before entering the critical section, they will be disabled when
//*             leaving the critical section.
//*             NOT IMPLEMENTED
//*
//* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
//*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
//*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
//*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
//*             into the CPU's status register.
//*********************************************************************************************************
//*

#define  OS_CRITICAL_METHOD   3

#if OS_CRITICAL_METHOD == 3
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
#endif

//*
//*********************************************************************************************************
//*                                        Cortex-M3 Miscellaneous
//*********************************************************************************************************
//*

#define  OS_STK_GROWTH        1                   /* Stack grows from HIGH to LOW memory on ARM        */

#define  OS_TASK_SW()         OSCtxSw()

//*
//*********************************************************************************************************
//*                                              PROTOTYPES
//*********************************************************************************************************
//*

#if OS_CRITICAL_METHOD == 3
OS_CPU_SR  OS_CPU_SR_Save(void);
void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#endif

void       OSCtxSw(void);
void       OSIntCtxSw(void);
void       OSStartHighRdy(void);

void       OS_CPU_SysTickInit(void);

#endif
