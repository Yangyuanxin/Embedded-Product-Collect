//*
//*********************************************************************************************************
//*
//*                                                 LOGO
//*
//*
//* 文 件 名 : stm3210x_vector.c
//*
//* 文件描述 : STM32系统向量表
//*
//* 作    者 : 
//* 版    本 : 
//* 编 译 器 : IAR EWARM 5.20
//*********************************************************************************************************
//*

#include "stm32f10x_lib.h"
#include "stm32f10x_it.h"

typedef void( *intfunc )( void );
typedef union { intfunc __fun; void * __ptr; } intvec_elem;

#pragma language=extended
#pragma segment="CSTACK"

void __iar_program_start( void );

#pragma location = ".intvec"
// STM32F10x向量表
const intvec_elem __vector_table[] =
{
  { .__ptr = __sfe( "CSTACK" ) },           // 00 堆栈
  __iar_program_start,                      // 01 复位
  NMIException,                             // 02
  HardFaultException,                       // 03 
  MemManageException,                       // 04
  BusFaultException,                        // 05
  UsageFaultException,                      // 06
  0, 0, 0, 0,                               // 07 08 09 10保留
  SVCHandler,                               // 11
  DebugMonitor,                             // 12
  0,                                        // 13 保留
  OS_CPU_PendSVHandler,                     // 14 PendSV 用于系统调度
  OS_CPU_SysTickHandler,                    // 15 系统Tick
  WWDG_IRQHandler,                          // 16
  PVD_IRQHandler,                           // 17
  TAMPER_IRQHandler,                        // 18
  RTC_IRQHandler,                           // 19
  FLASH_IRQHandler,                         // 20
  RCC_IRQHandler,                           // 21
  EXTI0_IRQHandler,                         // 22
  EXTI1_IRQHandler,                         // 23
  EXTI2_IRQHandler,                         // 24
  EXTI3_IRQHandler,                         // 25
  EXTI4_IRQHandler,                         // 26
  DMA1_Channel1_IRQHandler,                 // 27
  DMA1_Channel2_IRQHandler,                 // 28
  DMA1_Channel3_IRQHandler,                 // 29
  DMA1_Channel4_IRQHandler,                 // 30
  DMA1_Channel5_IRQHandler,                 // 31
  DMA1_Channel6_IRQHandler,                 // 32
  DMA1_Channel7_IRQHandler,                 // 33
  ADC1_2_IRQHandler,                        // 34
  USB_HP_CAN_TX_IRQHandler,                 // 35
  USB_LP_CAN_RX0_IRQHandler,                // 36
  CAN_RX1_IRQHandler,                       // 37
  CAN_SCE_IRQHandler,                       // 38
  EXTI9_5_IRQHandler,                       // 39
  TIM1_BRK_IRQHandler,                      // 40
  TIM1_UP_IRQHandler,                       // 41
  TIM1_TRG_COM_IRQHandler,                  // 42
  TIM1_CC_IRQHandler,                       // 43
  TIM2_IRQHandler,                          // 44
  TIM3_IRQHandler,                          // 45
  TIM4_IRQHandler,                          // 46
  I2C1_EV_IRQHandler,                       // 47
  I2C1_ER_IRQHandler,                       // 48
  I2C2_EV_IRQHandler,                       // 49
  I2C2_ER_IRQHandler,                       // 50
  SPI1_IRQHandler,                          // 51
  SPI2_IRQHandler,                          // 52
  USART1_IRQHandler,                        // 53
  USART2_IRQHandler,                        // 54
  USART3_IRQHandler,                        // 55
  EXTI15_10_IRQHandler,                     // 56
  RTCAlarm_IRQHandler,                      // 57
  USBWakeUp_IRQHandler,                     // 58
  TIM8_BRK_IRQHandler,                      // 59
  TIM8_UP_IRQHandler,                       // 60
  TIM8_TRG_COM_IRQHandler,                  // 61
  TIM8_CC_IRQHandler,                       // 62
  ADC3_IRQHandler,                          // 63
  FSMC_IRQHandler,                          // 64
  SDIO_IRQHandler,                          // 65
  TIM5_IRQHandler,                          // 66
  SPI3_IRQHandler,                          // 67
  UART4_IRQHandler,                         // 68
  UART5_IRQHandler,                         // 69
  TIM6_IRQHandler,                          // 70
  TIM7_IRQHandler,                          // 71
  DMA2_Channel1_IRQHandler,                 // 72
  DMA2_Channel2_IRQHandler,                 // 73
  DMA2_Channel3_IRQHandler,                 // 74
  DMA2_Channel4_5_IRQHandler,               // 75
};

