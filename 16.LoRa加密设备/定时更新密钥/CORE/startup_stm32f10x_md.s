;******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
;* File Name          : startup_stm32f10x_md.s
;* Author             : MCD Application Team
;* Version            : V3.5.0
;* Date               : 11-March-2011
;* Description        : STM32F10x Medium Density Devices vector table for MDK-ARM 
;*                      toolchain.  
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == Reset_Handler
;*                      - Set the vector table entries with the exceptions ISR address
;*                      - Configure the clock system
;*                      - Branches to __main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the CortexM3 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;* <<< Use Configuration Wizard in Context Menu >>>   
;*******************************************************************************
; THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
; WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
; AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
; INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
; CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
; INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400							;给数字量取一个符号名

                AREA    STACK, NOINIT, READWRITE, ALIGN=3	;汇编一个新的代码段或者数据段
Stack_Mem       SPACE   Stack_Size							;分配空间
__initial_sp												;初始化堆栈指针


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size							;定义堆大小512Byte			
__heap_limit

                PRESERVE8									;当前文件堆需要按照8字节对齐
                THUMB										;


; Vector Table Mapped to Address 0 at Reset 初始化中断向量表
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors							;声明一个标号具有全局性
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
				; 以字为单位分配内存，4字节对齐，并初始化这些内存
__Vectors       DCD     __initial_sp				; Top of Stack
                DCD     Reset_Handler              	; Reset Handler
                DCD     NMI_Handler                	; NMI Handler
                DCD     HardFault_Handler          	; Hard Fault Handler
                DCD     MemManage_Handler          	; MPU Fault Handler
                DCD     BusFault_Handler           	; Bus Fault Handler
                DCD     UsageFault_Handler         	; Usage Fault Handler
                DCD     0                          	; Reserved
                DCD     0                          	; Reserved
                DCD     0                          	; Reserved
                DCD     0                          	; Reserved
                DCD     SVC_Handler                	; SVCall Handler
                DCD     DebugMon_Handler           	; Debug Monitor Handler
                DCD     0                          	; Reserved
                DCD     PendSV_Handler             	; PendSV Handler
                DCD     SysTick_Handler            	; SysTick Handler

                ; External Interrupts 外部中断
                DCD     WWDG_IRQHandler            	; Window Watchdog
                DCD     PVD_IRQHandler             	; PVD through EXTI Line detect
                DCD     TAMPER_IRQHandler          	; Tamper
                DCD     RTC_IRQHandler             	; RTC
                DCD     FLASH_IRQHandler           	; Flash
                DCD     RCC_IRQHandler             	; RCC
                DCD     EXTI0_IRQHandler           	; EXTI Line 0
                DCD     EXTI1_IRQHandler           	; EXTI Line 1
                DCD     EXTI2_IRQHandler           	; EXTI Line 2
                DCD     EXTI3_IRQHandler           	; EXTI Line 3
                DCD     EXTI4_IRQHandler           	; EXTI Line 4
                DCD     DMA1_Channel1_IRQHandler  	; DMA1 Channel 1
                DCD     DMA1_Channel2_IRQHandler   	; DMA1 Channel 2
                DCD     DMA1_Channel3_IRQHandler   	; DMA1 Channel 3
                DCD     DMA1_Channel4_IRQHandler   	; DMA1 Channel 4
                DCD     DMA1_Channel5_IRQHandler   	; DMA1 Channel 5
                DCD     DMA1_Channel6_IRQHandler   	; DMA1 Channel 6
                DCD     DMA1_Channel7_IRQHandler   	; DMA1 Channel 7
                DCD     ADC1_2_IRQHandler          	; ADC1_2
                DCD     USB_HP_CAN1_TX_IRQHandler  	; USB High Priority or CAN1 TX
                DCD     USB_LP_CAN1_RX0_IRQHandler 	; USB Low  Priority or CAN1 RX0
                DCD     CAN1_RX1_IRQHandler        	; CAN1 RX1
                DCD     CAN1_SCE_IRQHandler        	; CAN1 SCE
                DCD     EXTI9_5_IRQHandler         	; EXTI Line 9..5
                DCD     TIM1_BRK_IRQHandler        	; TIM1 Break
                DCD     TIM1_UP_IRQHandler         	; TIM1 Update
                DCD     TIM1_TRG_COM_IRQHandler    	; TIM1 Trigger and Commutation
                DCD     TIM1_CC_IRQHandler         	; TIM1 Capture Compare
                DCD     TIM2_IRQHandler            	; 定时器2中断
                DCD     TIM3_IRQHandler            	; 定时器3中断
                DCD     TIM4_IRQHandler            	; 定时器4中断
                DCD     I2C1_EV_IRQHandler         	; I2C1 Event
                DCD     I2C1_ER_IRQHandler         	; I2C1 Error
                DCD     I2C2_EV_IRQHandler        	; I2C2 Event
                DCD     I2C2_ER_IRQHandler         	; I2C2 Error
                DCD     SPI1_IRQHandler            	; SPI1
                DCD     SPI2_IRQHandler            	; SPI2
                DCD     USART1_IRQHandler          	; 串口1中断
                DCD     USART2_IRQHandler          	; 串口2中断
                DCD     USART3_IRQHandler          	; 串口3中断
                DCD     EXTI15_10_IRQHandler       	; EXTI Line 15..10
                DCD     RTCAlarm_IRQHandler        	; RTC Alarm through EXTI Line
                DCD     USBWakeUp_IRQHandler       	; USB Wakeup from suspend
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler 初始化PC指针
Reset_Handler    PROC
                 EXPORT  Reset_Handler             [WEAK]
     IMPORT  __main
				; 库函数版本代码，建议加上这里（外部必须实现SystemInit函数），以初始化stm32时钟等。
                 IMPORT  SystemInit			
                 LDR     R0, =SystemInit	; 配置系统时钟
                 BLX     R0        
                 LDR     R0, =__main		; 调用C库函数_main初始化用户堆栈
                 BX      R0
                 ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC										; 定义子程序 开始
                EXPORT  NMI_Handler                [WEAK]	; NMI_Handler可被外部调用，WEAK -- 弱定义
                B       .									; 跳转到一个标号
                ENDP										; 子程序结束
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler          [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler           [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler           [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler             [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]	; 系统时钟中断
                B       .
                ENDP

Default_Handler PROC

                EXPORT  WWDG_IRQHandler            [WEAK]
                EXPORT  PVD_IRQHandler             [WEAK]
                EXPORT  TAMPER_IRQHandler          [WEAK]
                EXPORT  RTC_IRQHandler             [WEAK]
                EXPORT  FLASH_IRQHandler           [WEAK]
                EXPORT  RCC_IRQHandler             [WEAK]
                EXPORT  EXTI0_IRQHandler           [WEAK]
                EXPORT  EXTI1_IRQHandler           [WEAK]
                EXPORT  EXTI2_IRQHandler           [WEAK]
                EXPORT  EXTI3_IRQHandler           [WEAK]
                EXPORT  EXTI4_IRQHandler           [WEAK]
                EXPORT  DMA1_Channel1_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel2_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel3_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel4_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel5_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel6_IRQHandler   [WEAK]
                EXPORT  DMA1_Channel7_IRQHandler   [WEAK]
                EXPORT  ADC1_2_IRQHandler          [WEAK]
                EXPORT  USB_HP_CAN1_TX_IRQHandler  [WEAK]
                EXPORT  USB_LP_CAN1_RX0_IRQHandler [WEAK]
                EXPORT  CAN1_RX1_IRQHandler        [WEAK]
                EXPORT  CAN1_SCE_IRQHandler        [WEAK]
                EXPORT  EXTI9_5_IRQHandler         [WEAK]
                EXPORT  TIM1_BRK_IRQHandler        [WEAK]
                EXPORT  TIM1_UP_IRQHandler         [WEAK]
                EXPORT  TIM1_TRG_COM_IRQHandler    [WEAK]
                EXPORT  TIM1_CC_IRQHandler         [WEAK]
                EXPORT  TIM2_IRQHandler            [WEAK]
                EXPORT  TIM3_IRQHandler            [WEAK]
                EXPORT  TIM4_IRQHandler            [WEAK]
                EXPORT  I2C1_EV_IRQHandler         [WEAK]
                EXPORT  I2C1_ER_IRQHandler         [WEAK]
                EXPORT  I2C2_EV_IRQHandler         [WEAK]
                EXPORT  I2C2_ER_IRQHandler         [WEAK]
                EXPORT  SPI1_IRQHandler            [WEAK]
                EXPORT  SPI2_IRQHandler            [WEAK]
                EXPORT  USART1_IRQHandler          [WEAK]
                EXPORT  USART2_IRQHandler          [WEAK]
                EXPORT  USART3_IRQHandler          [WEAK]
                EXPORT  EXTI15_10_IRQHandler       [WEAK]
                EXPORT  RTCAlarm_IRQHandler        [WEAK]
                EXPORT  USBWakeUp_IRQHandler       [WEAK]

WWDG_IRQHandler
PVD_IRQHandler
TAMPER_IRQHandler
RTC_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA1_Channel1_IRQHandler
DMA1_Channel2_IRQHandler
DMA1_Channel3_IRQHandler
DMA1_Channel4_IRQHandler
DMA1_Channel5_IRQHandler
DMA1_Channel6_IRQHandler
DMA1_Channel7_IRQHandler
ADC1_2_IRQHandler
USB_HP_CAN1_TX_IRQHandler
USB_LP_CAN1_RX0_IRQHandler
CAN1_RX1_IRQHandler
CAN1_SCE_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_IRQHandler
TIM1_UP_IRQHandler
TIM1_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_EV_IRQHandler
I2C1_ER_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
USART3_IRQHandler
EXTI15_10_IRQHandler
RTCAlarm_IRQHandler
USBWakeUp_IRQHandler

                B       .

                ENDP

                ALIGN

;*******************************************************************************
					; User Stack and Heap initialization
;*******************************************************************************
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

;******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE*****
