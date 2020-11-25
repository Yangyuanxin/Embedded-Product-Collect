
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"
#include "arch.h"		  
#include "charQue.h"	  
#include "driver.h"	 
#include "main.h"	
#include "func_idle.h"
#include "f_Unlock.h"	  
#include "f_finger.h"		  
#include "f_key.h"			  
#include "f_test.h"	
#include "l_fingprint.h"
 

/******************************************************************************* 
 * 测试模式
 * 1.版本
 * 2.按键-cooked
 * 3.按键-raw
 * 4.全部语音(顺带spk)
 * 5.指纹头测试
 * 6.灯光
 * 
 * 60秒超时退出
 *******************************************************************************/
int f_testMode(unsigned *pMsg)
{
	int i;
	//msg_t msg;
	func_t func;
	charData_t charData; 
	char version[13] = {
	#include "../inc/VERSION.h"
	};
	//msg_init(&msg);
	//msg.msgType = ((msg_t *)pMsg)->msgType;
	//msg.msgValue = ((msg_t *)pMsg)->msgValue;
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		promptInit();
		lbeep();
		promptDelay(TIMER_500MS);
		lbeep();
		promptDelay(TIMER_500MS);
		lbeep();
    	
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
			
	case CPMT_OVER:
		g_tick = 0;
		promptInit();
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		promptInit();
		switch(((msg_t *)pMsg)->msgValue)
		{
		case CKEY_GOIN:
		case CKEY_GOOUT:
			break;
		
		case CKEY_1:			/** 进入密码开锁模式 **/
			#if	1
				for(i = 0; i < 12; i++) { 
    				vp_stor(version[i] + CVOPID_BASE); 				  		/** voicePrompt **/
					charData.ucVal = version[i];
					charQueueIn_irq(&g_com1TxQue, &charData);
				}
			#endif
			break;

		case CKEY_2:
			init_fstack(&g_fstack);
			func.func = f_testModeKeyCooked;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			break;

		case CKEY_3:	
			init_fstack(&g_fstack);
			func.func = f_testModeKeyRaw;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			break;
			
		case CKEY_4:
			init_fstack(&g_fstack);
			func.func = f_testModeVOP;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);		
			break;
		case CKEY_5:	
			break;
			
		case CKEY_6:
			init_fstack(&g_fstack);
			func.func = f_testModeLED;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);		
			break;
				
		case CKEY_7:	
		case CKEY_8:	
		case CKEY_9:	
		case CKEY_asterisk:	
		case CKEY_0:	
		case CKEY_pound:
			beep();
			break;
			
		default:
			break;
		}
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 60)
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	default:
		break;
	}
    
    return  0;
}


/** 指示灯动作提示 **/
void ledBlink(int _delay/** 周期 **/)
{
	action_t action;
    action.actionType = CACT_LEDPMTON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
}

int f_testModeKeyRaw(unsigned *pMsg)
{
	func_t func;
	charData_t charData;
	static int iBlinkCnt = 0;	//闪烁次数
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		ledPrompt(TIMER_100MS);
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
			
	case CPMT_OVER:
		g_tick = 0;
		promptInit();
		if(iBlinkCnt > 0)
		{
			iBlinkCnt--;
			ledBlink(TIMER_500MS);
		}
		else
		{
			//MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
			//MOPBlue_on();			/** 同背光位置的指示灯光 **/	
			MOPBlue_off();			/** 同背光位置的指示灯光 **/	
		}
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		promptInit();
		switch(((msg_t *)pMsg)->msgValue)
		{
		case CKEY_SET:
    		vp_stor(CVOPID_SET);
			charData.ucVal = CVOPID_SET;
			charQueueIn_irq(&g_com1TxQue, &charData);
			break;
			
		case CKEY_GOIN:
			break;
			
		case CKEY_GOOUT:
			promptInit();
    		lbeep();
			break;
		
		case CKEY_1:
			iBlinkCnt = 1;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_2:
			iBlinkCnt = 2;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_3:
			iBlinkCnt = 3;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_4:
			iBlinkCnt = 4;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_5:
			iBlinkCnt = 5;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_6:
			iBlinkCnt = 6;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_7:
			iBlinkCnt = 7;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_8:
			iBlinkCnt = 8;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_9:
			iBlinkCnt = 9;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_asterisk:	
			iBlinkCnt = 11;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_0:
			iBlinkCnt = 10;
			promptDelay(TIMER_500MS);
    		break;
		case CKEY_pound:
			iBlinkCnt = 12;
			promptDelay(TIMER_500MS);
    		break;
			
		default:
			break;
		}
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			init_fstack(&g_fstack);
			func.func = f_testMode;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	default:
		break;
	}
    
    return  0;
}
/******************************************************************************* 
 * 测试模式
 * 2.按键-cooked
 * 
 * 10s无动作退回
 *******************************************************************************/
//int f_testKeyCookedMode(unsigned *pMsg)
int f_testModeKeyCooked(unsigned *pMsg)
{
	//int i;
	//msg_t msg;
	func_t func;
	charData_t charData;
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		beep();
		promptDelay(TIMER_100MS);
    	
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
			
	case CPMT_OVER:
		g_tick = 0;
		promptInit();
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		promptInit();
		beep();
		switch(((msg_t *)pMsg)->msgValue)
		{
		case CKEY_SET:
    		vp_stor(CVOPID_SET);
			charData.ucVal = CVOPID_SET;
			charQueueIn_irq(&g_com1TxQue, &charData);
			break;
			
		case CKEY_GOIN:
			break;
			
		case CKEY_GOOUT:
			promptInit();
    		lbeep();
			break;
		
		case CKEY_1:			/** 进入密码开锁模式 **/
    		vp_stor(CVOPID_1);
			charData.ucVal = CVOPID_1;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;		
		case CKEY_2:
    		vp_stor(CVOPID_2);
			charData.ucVal = CVOPID_2;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;
		case CKEY_3:
    		vp_stor(CVOPID_3);
			charData.ucVal = CVOPID_3;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;	
		case CKEY_4:
    		vp_stor(CVOPID_4);
			charData.ucVal = CVOPID_4;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;		
		case CKEY_5:
    		vp_stor(CVOPID_5);
			charData.ucVal = CVOPID_5;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;		
		case CKEY_6:
    		vp_stor(CVOPID_6);
			charData.ucVal = CVOPID_6;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;			
		case CKEY_7:
    		vp_stor(CVOPID_7);
			charData.ucVal = CVOPID_7;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;			
		case CKEY_8:
    		vp_stor(CVOPID_8);
			charData.ucVal = CVOPID_8;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;			
		case CKEY_9:
    		vp_stor(CVOPID_9);
			charData.ucVal = CVOPID_9;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;			
		case CKEY_asterisk:	
    		vp_stor(CVOPID_asterisk);
			charData.ucVal = CVOPID_asterisk;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;		
		case CKEY_0:
    		vp_stor(CVOPID_0);
			charData.ucVal = CVOPID_0;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;			
		case CKEY_pound:
    		vp_stor(CVOPID_pound);
			charData.ucVal = CVOPID_pound;
			charQueueIn_irq(&g_com1TxQue, &charData);
    		break;
			
		default:
			break;
		}
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			init_fstack(&g_fstack);
			func.func = f_testMode;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	default:
		break;
	}
    
    return  0;
}

/******************************************************************************* 
 * 测试模式
 * 4.播放全部VOP
 *
 * 10s无动作退回
 *******************************************************************************/
int f_testModeVOP(unsigned *pMsg)
{
	static unsigned char id = 0;
	//msg_t msg;
	func_t func;
	//charData_t charData;
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		promptInit();
		beep();
		id = 0;
		vp_stor(id + CVOPID_BASE);
    	
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
			
	case CPMT_OVER:
		promptInit();
		g_tick = 0;
		id++;
		//if(id <= 38) {
		if(id <= (CVOPID_FINAL - CVOPID_BASE)) {
    		vp_stor(id + CVOPID_BASE);
    		promptDelay(TIMER_500MS);
    	} else {
    		init_fstack(&g_fstack);
			func.func = f_testMode;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		promptInit();
		beep();
		switch(((msg_t *)pMsg)->msgValue)
		{
		case CKEY_SET:
		case CKEY_GOIN:
		case CKEY_GOOUT:
		case CKEY_1:
		case CKEY_2:
		case CKEY_3:
		case CKEY_4:
		case CKEY_5:
		case CKEY_6:
		case CKEY_7:
		case CKEY_8:
		case CKEY_9:
		case CKEY_0:
		case CKEY_pound:
    		if(id > 38) {
    			promptInit();
				beep();
				promptDelay(TIMER_1SEC);
				id = 0;
    			vp_stor(id + CVOPID_BASE);
    		}
    		break;
		
		case CKEY_asterisk:
			init_fstack(&g_fstack);
			func.func = f_testMode;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);			
			break;
		
		default:
			break;
		}
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			init_fstack(&g_fstack);
			func.func = f_testMode;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	default:
		break;
	}
    
	return	0;
}

/******************************************************************************* 
 * 测试模式
 * 6.播放全部VOP
 *
 * 10s无动作退回
 *******************************************************************************/
int f_testModeLED(unsigned *pMsg)
{
	//static unsigned char id = 0;
	//msg_t msg;
	func_t func;
	//charData_t charData;
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		promptInit();
		beep();
		ledWarn(TIMER_100MS);
		ledPrompt(TIMER_100MS);
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
			
	case CPMT_OVER:
		promptInit();
		g_tick = 0;
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		promptInit();
		beep();
		switch(((msg_t *)pMsg)->msgValue)
		{
		case CKEY_SET:
		case CKEY_GOIN:
		case CKEY_GOOUT:
		case CKEY_1:
		case CKEY_2:
		case CKEY_3:
		case CKEY_4:
		case CKEY_5:
		case CKEY_6:
		case CKEY_7:
		case CKEY_8:
		case CKEY_9:
		case CKEY_asterisk:
		case CKEY_0:
		case CKEY_pound:
			init_fstack(&g_fstack);
			func.func = f_testMode;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_500MS, CMSG_TMR);
    		break;
			
		default:
			break;
		}
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick & 0x01) {
			MBlackLED_on();
			MOPBlue_on();
		} else {
			MBlackLED_off();
			MOPBlue_off();
		}
		if(g_tick >= 10)
		{
			init_fstack(&g_fstack);
			func.func = f_testMode;
    		stackpush(&g_fstack, &func);
    		    		
			MBlackLED_on();
			MOPBlue_on();
			
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	default:
		break;
	}
    
	return	0;
}
/////////////////////////////////////////////////////

