;/*****************************************************************************/
;/* STM32F10x.s: Startup file for ST STM32F10x device series                  */
;/*****************************************************************************/
;/* <<< Use Configuration Wizard in Context Menu >>>                          */
;/*****************************************************************************/
;/* This file is part of the uVision/ARM development tools.                   */
;/* Copyright (c) 2005-2007 Keil Software. All rights reserved.               */
;/* This software may only be used under the terms of a valid, current,       */
;/* end user licence from KEIL for a compatible version of KEIL software      */
;/* development tools. Nothing else gives you the right to use this software. */
;/*****************************************************************************/


;// <h> Stack Configuration
;//   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;// </h>

Stack_Size      EQU     0x00000200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


;// <h> Heap Configuration
;//   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;// </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                
                EXTERN  NMIException
                EXTERN  HardFaultException
                EXTERN  MemManageException
                EXTERN  BusFaultException
                EXTERN  UsageFaultException
                EXTERN  SVCHandler
                EXTERN  DebugMonitor
                EXTERN  PendSVC
                EXTERN  SysTickHandler

                EXPORT  __Vectors

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMIException              ; NMI Handler
                DCD     HardFaultException        ; Hard Fault Handler
                DCD     MemManageException        ; MPU Fault Handler
                DCD     BusFaultException         ; Bus Fault Handler
                DCD     UsageFaultException       ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVCHandler                ; SVCall Handler
                DCD     DebugMonitor              ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     PendSVC                   ; PendSV Handler
                DCD     SysTickHandler            ; SysTick Handler

                ; External Interrupts

                EXTERN  WWDG_IRQHandler
                EXTERN  PVD_IRQHandler
                EXTERN  TAMPER_IRQHandler
                EXTERN  RTC_IRQHandler
                EXTERN  FLASH_IRQHandler
                EXTERN  RCC_IRQHandler
                EXTERN  EXTI0_IRQHandler
                EXTERN  EXTI1_IRQHandler
                EXTERN  EXTI2_IRQHandler
                EXTERN  EXTI3_IRQHandler
                EXTERN  EXTI4_IRQHandler
                EXTERN  DMAChannel1_IRQHandler
                EXTERN  DMAChannel2_IRQHandler
                EXTERN  DMAChannel3_IRQHandler
                EXTERN  DMAChannel4_IRQHandler
                EXTERN  DMAChannel5_IRQHandler
                EXTERN  DMAChannel6_IRQHandler
                EXTERN  DMAChannel7_IRQHandler
                EXTERN  ADC_IRQHandler
                EXTERN  USB_HP_CAN_TX_IRQHandler
                EXTERN  USB_LP_CAN_RX0_IRQHandler
                EXTERN  CAN_RX1_IRQHandler
                EXTERN  CAN_SCE_IRQHandler
                EXTERN  EXTI9_5_IRQHandler
                EXTERN  TIM1_BRK_IRQHandler
                EXTERN  TIM1_UP_IRQHandler
                EXTERN  TIM1_TRG_COM_IRQHandler
                EXTERN  TIM1_CC_IRQHandler
                EXTERN  TIM2_IRQHandler
                EXTERN  TIM3_IRQHandler
                EXTERN  TIM4_IRQHandler
                EXTERN  I2C1_EV_IRQHandler
                EXTERN  I2C1_ER_IRQHandler
                EXTERN  I2C2_EV_IRQHandler
                EXTERN  I2C2_ER_IRQHandler
                EXTERN  SPI1_IRQHandler
                EXTERN  SPI2_IRQHandler
                EXTERN  USART1_IRQHandler
                EXTERN  USART2_IRQHandler
                EXTERN  USART3_IRQHandler
                EXTERN  EXTI15_10_IRQHandler
                EXTERN  RTCAlarm_IRQHandler
                EXTERN  USBWakeUp_IRQHandler
                
                DCD     WWDG_IRQHandler           ; Window Watchdog
                DCD     PVD_IRQHandler            ; PVD through EXTI Line detect
                DCD     TAMPER_IRQHandler         ; Tamper
                DCD     RTC_IRQHandler            ; RTC
                DCD     FLASH_IRQHandler          ; Flash
                DCD     RCC_IRQHandler            ; RCC
                DCD     EXTI0_IRQHandler          ; EXTI Line 0
                DCD     EXTI1_IRQHandler          ; EXTI Line 1
                DCD     EXTI2_IRQHandler          ; EXTI Line 2
                DCD     EXTI3_IRQHandler          ; EXTI Line 3
                DCD     EXTI4_IRQHandler          ; EXTI Line 4
                DCD     DMAChannel1_IRQHandler    ; DMA Channel 1
                DCD     DMAChannel2_IRQHandler    ; DMA Channel 2
                DCD     DMAChannel3_IRQHandler    ; DMA Channel 3
                DCD     DMAChannel4_IRQHandler    ; DMA Channel 4
                DCD     DMAChannel5_IRQHandler    ; DMA Channel 5
                DCD     DMAChannel6_IRQHandler    ; DMA Channel 6
                DCD     DMAChannel7_IRQHandler    ; DMA Channel 7
                DCD     ADC_IRQHandler            ; ADC
                DCD     USB_HP_CAN_TX_IRQHandler  ; USB High Priority or CAN TX
                DCD     USB_LP_CAN_RX0_IRQHandler ; USB Low  Priority or CAN RX0
                DCD     CAN_RX1_IRQHandler        ; CAN RX1
                DCD     CAN_SCE_IRQHandler        ; CAN SCE
                DCD     EXTI9_5_IRQHandler        ; EXTI Line 9..5
                DCD     TIM1_BRK_IRQHandler       ; TIM1 Break
                DCD     TIM1_UP_IRQHandler        ; TIM1 Update
                DCD     TIM1_TRG_COM_IRQHandler   ; TIM1 Trigger and Commutation
                DCD     TIM1_CC_IRQHandler        ; TIM1 Capture Compare
                DCD     TIM2_IRQHandler           ; TIM2
                DCD     TIM3_IRQHandler           ; TIM3
                DCD     TIM4_IRQHandler           ; TIM4
                DCD     I2C1_EV_IRQHandler        ; I2C1 Event
                DCD     I2C1_ER_IRQHandler        ; I2C1 Error
                DCD     I2C2_EV_IRQHandler        ; I2C2 Event
                DCD     I2C2_ER_IRQHandler        ; I2C2 Error
                DCD     SPI1_IRQHandler           ; SPI1
                DCD     SPI2_IRQHandler           ; SPI2
                DCD     USART1_IRQHandler         ; USART1
                DCD     USART2_IRQHandler         ; USART2
                DCD     USART3_IRQHandler         ; USART3
                DCD     EXTI15_10_IRQHandler      ; EXTI Line 15..10
                DCD     RTCAlarm_IRQHandler       ; RTC Alarm through EXTI Line
                DCD     USBWakeUp_IRQHandler      ; USB Wakeup from suspend


                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main
                LDR     R0, =__main
                BX      R0
                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB
                
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit
                
                ELSE
                
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
