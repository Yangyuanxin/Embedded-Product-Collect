;/*********************** (C) COPYRIGHT 2010 Libraworks *************************
;* File Name	: os_cpu_a.asm 
;* Author		: Librae 
;* Version		: V1.0
;* Date			: 06/10/2010
;* Description	: μCOS-II asm port	for STM32
;*******************************************************************************/

		IMPORT  OSRunning               ; External references
        IMPORT  OSPrioCur
        IMPORT  OSPrioHighRdy
        IMPORT  OSTCBCur
        IMPORT  OSTCBHighRdy
        IMPORT  OSIntNesting
        IMPORT  OSIntExit
        IMPORT  OSTaskSwHook
           
        EXPORT  OSStartHighRdy               
        EXPORT  OSCtxSw
        EXPORT  OSIntCtxSw
		EXPORT  OS_CPU_SR_Save                                      ; Functions declared in this file
    	EXPORT  OS_CPU_SR_Restore       
        EXPORT  PendSV_Handler
        	
     
NVIC_INT_CTRL   	EQU     0xE000ED04  ; 中断控制寄存器
NVIC_SYSPRI2    	EQU     0xE000ED22  ; 系统优先级寄存器(2)
NVIC_PENDSV_PRI 	EQU         0xFFFF  ; PendSV中断和系统节拍中断
                                        ; (都为最低，0xff).
NVIC_PENDSVSET  	EQU     0x10000000  ; 触发软件中断的值.


		PRESERVE8 
		
		AREA    |.text|, CODE, READONLY
        THUMB 
    
           

;********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;
;
; Note(s)    : 1) These functions are used in general like this:
;
;                 void Task (void *p_arg)
;                 {
;                 #if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
;                     OS_CPU_SR  cpu_sr;
;                 #endif
;
;                          :
;                          :
;                     OS_ENTER_CRITICAL();             /* cpu_sr = OS_CPU_SaveSR();                */
;                          :
;                          :
;                     OS_EXIT_CRITICAL();              /* OS_CPU_RestoreSR(cpu_sr);                */
;                          :
;                          :
;                 }
;********************************************************************************************************

OS_CPU_SR_Save
    MRS     R0, PRIMASK  	;读取PRIMASK到R0,R0为返回值 
    CPSID   I				;PRIMASK=1,关中断(NMI和硬件FAULT可以响应)
    BX      LR			    ;返回

OS_CPU_SR_Restore
    MSR     PRIMASK, R0	   	;读取R0到PRIMASK中,R0为参数
    BX      LR				;返回


;/**************************************************************************************
;* 函数名称: OSStartHighRdy
;*
;* 功能描述: 使用调度器运行第一个任务
;* 
;* 参    数: None
;*
;* 返 回 值: None
;**************************************************************************************/  

OSStartHighRdy
        LDR     R4, =NVIC_SYSPRI2      ; set the PendSV exception priority
        LDR     R5, =NVIC_PENDSV_PRI
        STR     R5, [R4]

        MOV     R4, #0                 ; set the PSP to 0 for initial context switch call
        MSR     PSP, R4

        LDR     R4, =OSRunning         ; OSRunning = TRUE
        MOV     R5, #1
        STRB    R5, [R4]

                                       ;切换到最高优先级的任务
        LDR     R4, =NVIC_INT_CTRL     ;rigger the PendSV exception (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]

        CPSIE   I                      ;enable interrupts at processor level
OSStartHang
        B       OSStartHang            ;should never get here

;/**************************************************************************************
;* 函数名称: OSCtxSw
;*
;* 功能描述: 任务级上下文切换         
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/
  
OSCtxSw
		PUSH    {R4, R5}
        LDR     R4, =NVIC_INT_CTRL  	;触发PendSV异常 (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]
		POP     {R4, R5}
        BX      LR

;/**************************************************************************************
;* 函数名称: OSIntCtxSw
;*
;* 功能描述: 中断级任务切换
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/

OSIntCtxSw
		PUSH    {R4, R5}
        LDR     R4, =NVIC_INT_CTRL      ;触发PendSV异常 (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]
		POP     {R4, R5}
        BX      LR
        NOP

;/**************************************************************************************
;* 函数名称: OSPendSV
;*
;* 功能描述: OSPendSV is used to cause a context switch.
;*
;* 参    数: None
;*
;* 返 回 值: None
;***************************************************************************************/

PendSV_Handler
    CPSID   I                                                   ; Prevent interruption during context switch
    MRS     R0, PSP                                             ; PSP is process stack pointer 如果在用PSP堆栈,则可以忽略保存寄存器,参考CM3权威中的双堆栈-白菜注
    CBZ     R0, PendSV_Handler_Nosave		                    ; Skip register save the first time
	
	;Is the task using the FPU context? If so, push high vfp registers.
	TST 	R14, #0x10
	IT 		EQ
	VSTMDBEQ R0!, {S16-S31} 
	
    SUBS    R0, R0, #0x20                                       ; Save remaining regs r4-11 on process stack
    STM     R0, {R4-R11}

    LDR     R1, =OSTCBCur                                       ; OSTCBCur->OSTCBStkPtr = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

                                                                ; At this point, entire context of process has been saved
PendSV_Handler_Nosave
    PUSH    {R14}                                               ; Save LR exc_return value
    LDR     R0, =OSTaskSwHook                                   ; OSTaskSwHook();
    BLX     R0
    POP     {R14} 

    LDR     R0, =OSPrioCur                                      ; OSPrioCur = OSPrioHighRdy;
    LDR     R1, =OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OSTCBCur                                       ; OSTCBCur  = OSTCBHighRdy;
    LDR     R1, =OSTCBHighRdy
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
    LDM     R0, {R4-R11}                                        ; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20

	;Is the task using the FPU context? If so, push high vfp registers.
	TST 	R14, #0x10
	IT 		EQ
	VLDMIAEQ R0!, {S16-S31} 
		
	MSR     PSP, R0                                             ; Load PSP with new process SP
    ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack
    CPSIE   I
    BX      LR                                                  ; Exception return will restore remaining context
	NOP
    end  
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		