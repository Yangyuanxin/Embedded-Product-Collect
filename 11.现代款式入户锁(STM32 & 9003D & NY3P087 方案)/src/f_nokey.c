
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"

#include "func_idle.h"
#include "f_Unlock.h"
#include "arch.h"
#include "charQue.h"
#include "driver.h"	 
#include "main.h"	
 
/*******************************************************************************
 * no key enter
 * 无钥匙进入
 *******************************************************************************/
int f_nokeyEnter(unsigned *pMsg)
{
//	msg_t msg;
    func_t func;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:		/** 低电报警提示计时 **/
		g_tick++;
		if(g_tick > 10)
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
			
   	    	MLockPWR_off();
   	    	MPKEPower_off();	/** 感应电路断电 **/
   			MBlackLED_off();	/** 背光灯灭 **/
			MFingerPower_off();	/** 指纹头断电 **/
			MMasterPower_off();	/** 最后断电 **/
		}
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	promptDelay(TIMER_100MS);	/** 可在此加入提示音，如果需要 **/
		break;

	case CPMT_OVER:					/** 提示音结束，开锁 **/
    	promptInit();
    	lockAction();				/** 开锁 **/
		break;
			
	case CACT_OVER:					/** 锁体操作过程结束 **/
    	actionInit();
    	init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
		
	default:
		break;
	}
    return  0;
}

/////////////////////////////////////////////////////
/*******************************************************************************
 * 遥控操作
 *******************************************************************************/
int f_remote(unsigned *pMsg)
{
    func_t func;
    
    switch(((msg_t *)pMsg)->msgType)
	{	
	case CMSG_TMR:		/** 低电报警提示计时 **/
		g_tick++;
		if(g_tick > 10)
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	beep();
    	promptDelay(TIMER_200MS);	/** 可在此加入提示音，如果需要(改为NY3P087后,参数TIMER_100MS工作不正常，原因不明 **/
		break;

	case CPMT_OVER:					/** 提示音结束，开锁 **/
    	promptInit();
    	lockAction();				/** 开锁 **/
		break;
			
	case CACT_OVER:					/** 锁体操作过程结束 **/
    	actionInit();
    	init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
			
	default:
		break;
	}
	return 0;
}
