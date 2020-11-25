//*
//*********************************************************************************************************
//*
//*                                                 LOGO
//*
//*
//* 文 件 名 : main.c
//*
//* 文件描述 : STM32系统主文件
//*
//* 作    者 : 
//* 版    本 : 
//* 编 译 器 : IAR EWARM 5.20
//*********************************************************************************************************
//*
#define  OS_GLOBALS
#define _DLIB_FILE_DESCRIPTOR 0//使能fputc()
#include "head.h"
#include <stdio.h>

//---
#define TASK_REF_DISPLAY_STK_SIZE 512
OS_STK	Task_Ref_Display_Stk[TASK_REF_DISPLAY_STK_SIZE];
void Task_Ref_Display(void *pdata);
//---
#define TASK_RUN_LED_STK_SIZE 512
OS_STK	Task_Run_Led_Stk[TASK_RUN_LED_STK_SIZE];
void Task_Run_Led(void *pdata);
//---
#define TASK_LOGIC_STK_SIZE 512
OS_STK	Task_logic_Stk[TASK_LOGIC_STK_SIZE];
void Task_Logic(void *pdata);



//int fputc(int ch, FILE *f);
//int fputc(int ch, FILE *f)
//{
 //   USART1->DR = (u8) ch;
    
    /* Loop until the end of transmission */
   // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    //{
   // }
   // return ch;
//}

//*
//********************************************************************************************************
//* 函 数 名 : static void APP_TaskStart(void *pdata)
//**------------------------------------------------------------------------------------------------------
//* 描    述 : main
//**------------------------------------------------------------------------------------------------------
//* 输入参数 : None.
//**------------------------------------------------------------------------------------------------------
//* 输出参数 : None.
//**------------------------------------------------------------------------------------------------------
//* 返   回  : None.
//********************************************************************************************************
//*
static void APP_TaskStart(void *pdata)
{
    pdata = pdata;
    
    //目标板初始化,
    Target_Init();       
    
    // 启动系统Tick
    OS_CPU_SysTickInit();
	
    //统计任务初始化
#if OS_TASK_STAT_EN > 0
    OSStatInit();
#endif
	
    //事件的建立
    CreateEvents();
	
    //系统多任务的建立
    //CreateMultiTask();
    
	
    InitPara();//需在本任务中最先初始化
    
    //开始任务自动挂起
    //OSTaskSuspend(OS_PRIO_SELF);
    while(1)
    {        
		
        //刷新液晶显示
        OSTaskCreateExt(Task_Ref_Display,
			(void *)0,
			&Task_Ref_Display_Stk[TASK_REF_DISPLAY_STK_SIZE-1],
			12,
			12,
			&Task_Ref_Display_Stk[0],
			TASK_REF_DISPLAY_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		//OSTaskNameSet(Task1_LED1_Prio, "Task1-LED1", &err);
		
		//运行灯任务
		
		OSTaskCreateExt(Task_Run_Led,
			(void *)0,
			&Task_Run_Led_Stk[TASK_RUN_LED_STK_SIZE-1],
			13,
			13,
			&Task_Run_Led_Stk[0],
			TASK_RUN_LED_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		//OSTaskNameSet(Task2_LED2_Prio, "Task2-LED2", &err);
		
        
        //人机交互界面任务,此任务的定义在lcd_interface.c中
		
		
        OSTaskCreateExt(TaskLcdInf,
			(void *)0,
			&TaskLcdInfStk[TASK_LCD_INF_STK_SIZE-1],
			14,
			14,
			&TaskLcdInfStk[0],
			TASK_LCD_INF_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		
        //逻辑处理程序，同时处理 KI
        OSTaskCreateExt(Task_Logic,
			(void *)0,
			&Task_logic_Stk[TASK_LOGIC_STK_SIZE-1],
			10,
			10,
			&Task_logic_Stk[0],
			TASK_LOGIC_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
        
        //逻辑处理程序，同时处理 KI
        OSTaskCreateExt(TaskPAIWU,
			(void *)0,
			&TaskPAIWUStk[TASK_PAIWU_STK_SIZE-1],
			11,
			11,
			&TaskPAIWUStk[0],
			TASK_PAIWU_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		
	
        
        
        
        
        
		
        OSTimeDly(OS_TICKS_PER_SEC);
        OSTaskSuspend(OS_PRIO_SELF);
    }
}

//*
//********************************************************************************************************
//* 函 数 名 : void main(void)
//**------------------------------------------------------------------------------------------------------
//* 描    述 : main
//**------------------------------------------------------------------------------------------------------
//* 输入参数 : None.
//**------------------------------------------------------------------------------------------------------
//* 输出参数 : None.
//**------------------------------------------------------------------------------------------------------
//* 返   回  : None.
//********************************************************************************************************
//*
void main(void)
{
#if (OS_TASK_NAME_SIZE > 14) && (OS_TASK_STAT_EN > 0)
    INT8U  err;
#endif
    
    OSInit();
    
    //设置空闲任务名称
#if OS_TASK_NAME_SIZE > 14
    OSTaskNameSet(OS_TASK_IDLE_PRIO, "uC/OS-II Idle", &err);
#endif
    //设置统计任务名称
#if (OS_TASK_NAME_SIZE > 14) && (OS_TASK_STAT_EN > 0)
    OSTaskNameSet(OS_TASK_STAT_PRIO, "uC/OS-II Stat", &err);
#endif
    
    //用任务建立任务
    OSTaskCreateExt(APP_TaskStart,                                        //void (*task)(void *pd) 任务首地址
		(void *)0,                                            //void *pdata            数据指针
		&APP_TaskStartStk[APP_TASK_START_STK_SIZE - 1],       //OS_STK *ptos           指向任务堆栈栈顶的指针
		(INT8U)APP_TASK_START_PRIO,                           //INT8U prio             任务优先级
		(INT16U)APP_TASK_START_ID,                            //INT16U id              任务的ID号
		&APP_TaskStartStk[0],                                 //OS_STK *pbos           指向任务堆栈栈底的指针
		(INT32U)APP_TASK_START_STK_SIZE,                      //INT32U stk_size        堆栈容量
		(void *)0,                                            //void *pnext            数据指针
		OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);           //INT16U opt             设定OSTaskCreateExt的选项
	
#if OS_TASK_NAME_SIZE > 14
    OSTaskNameSet(APP_TASK_START_PRIO, "Task-Start", &err);
#endif
	
    OSStart();
}


//完成LCD刷新显示功能
static void Task_Ref_Display(void *pdata)
{
    u16 base_ref;   //正常运行时，间隔去刷新屏幕
  
    pdata = pdata;    
	
    GUI_Initialize();
    
    base_ref = 0;
    
    
    while(1)
    {
		
      if ((lcd_need_refurbish) || (base_ref++ > 200)) //需要刷新，或定时刷新时间到，便刷屏幕
        {
            lcd_need_refurbish = 0;
            base_ref = 0;
			
            LCD_DispRefurbish();           
            
        }
		
		OSTimeDly(OS_TICKS_PER_SEC / 20);  
        
        		
		//与逻辑处理放到不同的任务
//		Control_Relay();//根据逻辑控制当前继电器
    }    
}

//完成运行灯闪烁任务
//此任务 采集AD,通信处理
void  Task_Run_Led(void *pdata)
{   
    
  
    pdata = pdata;  
	

    Adc_Init(); 
    //InitModbus();
    Init_Mb_Mst();
    
	LED_Init();

    
    while(1)
    {
      
     
        
      
		//
        OSTimeDly(OS_TICKS_PER_SEC/10 );
        		        
       // TaskModbus(COM3);
        TaskModbus(COM1);     
        TaskModbus(COM2);
		TaskModbus(COM3);     
        TaskModbus(COM4);
#ifdef FOR_TEST     
        TaskModbus(COM5);
#else
        //处理MODBUS主站
        Pro_Mb_Mst(); //COM5
#endif	
		LED_Run();       
    }
	
    
}
//逻辑功能处理
void  Task_Logic(void *pdata)
{      
	
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
	//    OS_CPU_SR  cpu_sr = 0;
#endif
	
    pdata = pdata;
    
    
    
	Init_Relay();
	
    Init_Ki();    
        
    ProKi();//扫描开入    
    	 
    while(1)
    {
        OSTimeDly(OS_TICKS_PER_SEC/40); 

        ProLogic();             
		
        ProKi();//扫描开入
    }
    
}



