/*******************************************************************************
* File Name          : main.c
* Author             : HuNan HuaRain Science & Technology Co.Ltd, zhanglong
* Date First Issued  : 2012-7-25
* Description        : Adapter
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"														
#include "string.h"
#include "inc/CONST.h"
#include "inc/ctype.h"
#include "inc/global.h"
#include "inc/macro.h"
#include "src/init.h"  
#include "src/l_fingprint.h"
#include "src/arch.h"
//#include "src/bitmapOp.h"
#include "src/charQue.h"   
#include "src/driver.h"	
//#include "src/canTxQue.h"
//#include "src/wdg.h"
#include "main.h"  
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//RetStatus CAN_loop(void);

#include "src/func_idle.h"
/** global variable **/

fstack_t g_fstack;
msgq_t  g_msgq;
Timer_t g_timer[TIMER_NUM];		/** 约定g_timer[0]不用在sysProcess()中， g_timer[1]只用在sysProcess()中 **/

charBuf_queue_t g_com1TxQue;
charBuf_queue_t g_com2TxQue;

//int g_channel;
actionQueue_t g_actionQueue;
actionQueue_t g_promptQueue;
//actionQueue_t g_promptQueue;
/*********************************************
 * g_flag 
 * g_flag.4 = 1.TO状态.触摸按键按下.
 * g_flag.5 = 1.TI状态.触摸按键按下.
 * g_flag.6 = 1.finger状态.指纹头有手指按下.
 * g_flag.7 = 1.第一次触摸(激活)数字按键.
 * g_flag.8 = 1.低电检测. ADC低电检测采样完成.
 * g_flag.9 = 1.低电确认. 
 * g_flag.10 = 1. 空指纹. 且由1键进入指纹操作模式
 * g_flag.11 = 1. 收到无线钥匙信号
 * g_flag.12 = 1. 按下设置键
 * g_flag.13 = 1. 语音busy上次电平
 * g_flag.14 = 1/0. 按下数字键
 *
 *********************************************/
unsigned char g_ucUserId = 0xff;		/** 当前用户id，也是管理者id **/
unsigned char g_ucAddUserId = 0xff;		/** 添加的id号，也是管理者id **/
u32 g_flag;
int g_tick;			/** 嘀嗒。一般在状态转换时清零 **/

int	g_iADCref = 0;
int g_iADCCurr = 0;

unsigned int g_subState;		/** 工作子状态, 暂未用到 **/
//unsigned char g_oldKey[10] = {0};
//key_t g_keys;

const char g_magic[MAGIC_SIZE] = {0x48, 0x55, 0x41, 0x52}; 		/** 'H', 'U', 'A', 'R'**/

//unsigned char g_flash[CFLASH_PAGE_SIZE];
flashPage_t g_flash;
/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
    msg_t msg;
    func_t func;
	int	i;

    RCC_Configuration();	/* System Clocks Configuration */
    NVIC_Configuration();	/* NVIC Configuration */
    GPIO_Configuration();	/* GPIO ports pins Configuration */

	/** 初始化各通用端口状态 **/
	MMasterPower_off();	/** 主控制断电，在检测到相关动作后才能确定供电 **/	
	MBlackLED_off();	/** 背光灯，状态未知-灭 **/
	MFingerPower_off();	/** 指纹头，状态未知-灭 **/
	//MOPGreen_off();		/** 绿色指示灯，状态未知-灭 **/
	MOPBlue_off();		/** 篮色指示灯，状态未知-灭 **/
	MOPRed_off();		/** 红色指示灯，状态未知-灭 **/
	
	#if	CENABLEVOP			/** 声音提示，状态未知-??? **/
	VOPDATA_H();
	#else
	VOPDATA_L();
	#endif
	VOPRESET_H();
	
	MLockPWR_off();		/** 锁体电机，状态未知-静止 **/
	MPKEPower_off();	/** 感应供电，状态未知-断开 **/			

    USART_Configuration();    /* Configure the USART1 */
    TIM_Configuration();      /* Configure the TIM */
    EXTI_Configuration();			/** exti **/
	ADC_Configuration();
//    bitmap_clean(&bmpRcvCan);

	g_tick = 0;
	g_flag = 0;
//	g_keys.iCount = 0;
	
		#if	1
	/** 如果有，就读取，否则取默认值 **/
	//for(i = 0; i < CFLASH_PAGE_SIZE; i++)g_flash.arrChar[i] = 0xff;
	flashCache_init();
	if(memcmp((char *)CFLASHSTARTADDR, g_magic, MAGIC_SIZE) == 0)		/** flash中已有数据 **/
	{	
		u32 *iPtr = (u32 *)CFLASHSTARTADDR;

	 	//flashWrite((u32 *)g_flash.arrInt);
		
		for(i = 0; i < (CFLASH_PAGE_SIZE / 4); i++)g_flash.arrInt[i] = iPtr[i];			/** data valid! read it first **/
	}
	else
	{
		flashWrite((u32 *)g_flash.arrInt);
		memcpy(g_flash.arrChar, g_magic, CFLASH_PAGE_SIZE);						/** data valid! read it first **/
	}
		#endif

    charQueueInit(&g_com1TxQue);
    charQueueInit(&g_com2TxQue);
    actionQueueInit(&g_actionQueue, &(g_timer[1]), CACT_TOUT, CACT_OVER);
    actionQueueInit(&g_promptQueue, &(g_timer[2]), CPMT_TOUT, CPMT_OVER);
    
    init_fstack(&g_fstack);
    init_queue(&g_msgq);
    
    func.func = func_idle;
    stackpush(&g_fstack, &func);

    for(i = 0; i < TIMER_NUM; i++)
    {
        ClrTimer(&g_timer[i]);
    }
    
    SetTimer(&g_timer[0], TIMER_1SEC, CMSG_TMR);

    /** WDG **/
    //wdg_init();
    /**
    * Now all initial ok. then enable interrupt
    **/
	msg.msgType = CMSG_INIT;
    inq(&g_msgq, &msg);
	
    IRQ_enable();
	wdg_init();

    while(1)
    {
	    wdg_feed();
        keyscan();
        fingerCheck();
        noKeyEnter();
        vop_busy();
        DAEMON_USARTx_Send(USART1, &g_com1TxQue);   /** output for debug **/
        DAEMON_USARTx_Send(USART2, &g_com2TxQue);
        actionDoing(&g_actionQueue);
        actionDoing(&g_promptQueue);
        //DAEMON_CAN_Send();
        
        if(outq_irq(&g_msgq, &msg) == FALSE)    /** 有消息吗? **/
        {
            continue;
        }
        if(sysProcess(&msg) == TRUE)    /** 是系统消息吗? **/
        {
            continue;
        }
        if(stacktop(&g_fstack, &func) == FALSE)     /** 当前处于工作状态吗? **/
        {
        	/** something wrong happend, Power Down or recover it **/
        	#if	0
    			MMasterPower_off();
    		#else
        		func.func = func_idle;
    			stackpush(&g_fstack, &func);
    			g_tick = 0;
    			
    			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
				
            	continue;
    		#endif
        }

        func.func((unsigned *)&msg);
    }
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
