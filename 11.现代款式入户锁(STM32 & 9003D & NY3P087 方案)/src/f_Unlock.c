
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"

#include "l_fingprint.h"
#include "func_idle.h"
#include "f_Unlock.h"
#include "arch.h"
#include "charQue.h"
#include "driver.h"	 
#include "main.h"	
 
/*******************************************************************************
 * Description: 开锁操作完成
 * 低电报警
 *******************************************************************************/
 
 //#define	CMOTORACT	(4)		/**电机转动时间长度**/
 //#define	CMOTORPAUSE (15)	/**电机反转前的等待时间长度**/
int f_Unlock(unsigned *pMsg)
{
//	msg_t msg;
    func_t func;
	int iTmp = 0;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:		/** 低电报警提示计时 **/
		g_tick++;
		if(g_tick >= 10)
		{
		GOTO_ALLOFF:
			init_fstack(&g_fstack);
			func.func = func_idle;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
			
			/*******************************************************************/
    		iTmp = resetOP_getValue();	/** 恢复出厂值检查 **/
    		if(iTmp != 0xa0)
    		{
				resetOP_setValue(0xa0);
				flashWrite(g_flash.arrInt);
    		}
    		/*******************************************************************/
			
   	    	MLockPWR_off();
   	    	MPKEPower_off();	/** 感应电路断电 **/
   			MBlackLED_off();	/** 背光灯灭 **/
   			MOPBlue_off();		/** 动作指示灯灭 **/
   			MOPRed_off();		/** 警告指示灯灭 **/
			MFingerPower_off();	/** 指纹头断电 **/
			MMasterPower_off();	/** 最后断电 **/
		}
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	if(g_flag & (1 << 9))	/** 查低电报警 **/
    	{
    		/** 报警提示 **/
    		promptInit();
    		MOPBlue_setDefaultStatus();		/** 恢复指示灯与背光一致 **/
    		//bbbeep();
    		vp_stor(CVOPID_POWERLOW);
    		promptDelay(TIMER_500MS);
    	}
    	else
    	{
    		goto GOTO_ALLOFF;				/** 无报警动作，直接结束 **/
    	}
		break;

	case CPMT_OVER:							/** 锁体操作过程结束 **/
    	promptInit();
    	bbbeep();
    	promptDelay(TIMER_500MS);
		break;
			
	default:
		
		break;
	}
    return  0;
}
/*******************************************************************************
 * Description: 立即进入断电流程
 * 
 *******************************************************************************/
int f_UnlockFail(unsigned *pMsg)
{
	func_t func;
	msg_t msg;
   	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		if(g_tick < 1)
		{
		}
		else 
		{
			init_fstack(&g_fstack);
			func.func = func_idle;
    		stackpush(&g_fstack, &func);
    		
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
			
			MLockPWR_off();		/** 等待 **/
			MBlackLED_off();	/** 背光灯灭 **/
   			MOPBlue_off();		/** 动作指示灯灭 **/
   			MOPRed_off();		/** 警告指示灯灭 **/
		    MFingerPower_off();	/** 指纹头断电 **/
			MPKEPower_off();	/** 感应电路断电 **/
			MMasterPower_off();	/** 最后断电 **/
		}
		g_tick++;
		break;
		
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_TMR);
		break;
	
	default:
		break;
	}
    
    return  0;	
}
/////////////////////////////////////////////////////

