
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"
#include "arch.h"
#include "driver.h"
#include "l_fingprint.h"
#include "func_idle.h"
#include "f_Unlock.h"
#include "f_finger.h"		  
#include "f_nokey.h"			  
#include "f_key.h"	
#include "charQue.h"	 
#include "main.h"	
 
/*******************************************************************************
 * local function
 *******************************************************************************/
int func_idle(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
//	int i;

//	static unsigned char  ucTmp = 0;
	charData_t charData;

	//msg.msgType = ((msg_t *)pMsg)->msgType;
	//msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType) 
    //switch(msg.msgType)
	{
	case CACT_OVER:
		actionInit();
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)	/** 点亮背光后等待时间, 考虑兼顾指纹模板为空的情况下电机的运转时间 **/
		{
			MPKEPower_off();			/** 无线钥匙功能断电 **/
			MBlackLED_off();			/** 背光灯灭 **/
			MFingerPower_off();			/** 指纹头断电 **/
			MOPBlue_off();
			MOPRed_off();
			MMasterPower_off();			/** 主控制IC断电 **/
			
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_TMR);
		}
		
		charData.ucVal = g_tick;
		charQueueIn_irq(&g_com1TxQue, &charData); 
		//charQueueIn_irq(&g_com2TxQue, &charData);

		#if	0
			AD_value = ADC_GetConversionValue(ADC1);
			charData.ucVal = AD_value & 0xff;
			charQueueIn_irq(&g_com1TxQue, &charData); 
			charQueueIn_irq(&g_com2TxQue, &charData);
			charData.ucVal = (AD_value >> 8) & 0xff;
			charQueueIn_irq(&g_com1TxQue, &charData); 
			charQueueIn_irq(&g_com2TxQue, &charData);
		#endif
		break;
		
	case CMSG_INIT:		/** nothing to be done **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
	case CNOKEY_ENTER:
		#if	0
		/** 进入无钥匙开锁状态 **/
			init_fstack(&g_fstack);
    		func.func = f_nokeyEnter;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	#else
    		/** 遥控接收 **/
    		MMasterPower_on();			/** 主控制IC供电 **/
    		init_fstack(&g_fstack);
    		func.func = f_remote;
    		stackpush(&g_fstack, &func);
    	
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	#endif
		break;
		
	case CMSG_BACK:
		MMasterPower_on();			/** 主控制IC供电 **/
		MBlackLED_on();			/** 背光 **/
		MOPBlue_on();			/** 与背光一起的指示灯 **/
		
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	
    	if(fingerTab_isEmpty()) {			/** 指纹模板空，直接开锁 **/
			init_fstack(&g_fstack);
			func.func = f_keyFirstTouch;
    		stackpush(&g_fstack, &func);
		} else {
			if(MFP_isInductionPower()) {	/** 指纹头有感应上电功能时，按键进入密码开锁状态 **/
				init_fstack(&g_fstack);
				func.func = f_key;
    			stackpush(&g_fstack, &func);
    		} else {
    			/**  指纹头没有感应上电功能时，数字按键首先进入指纹开锁状态，在指纹开锁状态中如果有按键按下才进入密码开锁状态  **/
    			goto CAddrFinger_on;
    		}
		}
		break;	
		
	case CFIGER_ON:		/** 检测到指纹按下 **/
	  CAddrFinger_on:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
    	if(fingerTab_isEmpty())	{ /** 指纹模板空，直接开锁 **/
			MMasterPower_on();			/** 主控制IC供电 **/
			init_fstack(&g_fstack);
			func.func = f_keyFirstTouch;
    		stackpush(&g_fstack, &func);
		} else {
			MMasterPower_on();			/** 主控制IC供电 **/
			MFingerPower_on();			/** 给指纹头供电 **/
        	
			/** 进入指纹比对状态 **/
			init_fstack(&g_fstack);
    		func.func = f_finger;
    		stackpush(&g_fstack, &func);
    	}
		break;

	case CMSG_DKEY:							/** 检测到按键按下 **/
		switch(((msg_t *)pMsg)->msgValue)
		{	
		case CKEY_GOOUT:				/** 摸室内把手，直接开锁 **/
			{
				MMasterPower_on();			/** 主控制IC供电 **/
				#if	1
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				g_subState = 0;
				init_fstack(&g_fstack);
				func.func = f_KEYToOut;
    			stackpush(&g_fstack, &func);
				#endif
			}
    		break;
    		
		case CKEY_SET:				/** 室内设置 **/
			{
				MMasterPower_on();			/** 主控制IC供电 **/
				#if	1
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				g_subState = 0;
				init_fstack(&g_fstack);
				func.func = f_KEYSET;
    			stackpush(&g_fstack, &func);
				#endif
			}
    		break;
		
		case CKEY_GOIN:					/** 摸室外把手，查无线钥匙 **/
			{
				if(fingerTab_isEmpty())	/** 指纹模板空，直接进入开锁流程 **/
				{
					#if	1
					MMasterPower_on();
					lockAction();
    				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
					#endif
				}
				else
				{
					MMasterPower_on();		/** 主控制IC供电 **/
					MPKEPower_on();			/** 无线钥匙功能供电 **/
					g_tick = 0;					/** 同时开始计数 **/
    				SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
				}
			}
			break;
			
			#if	0
		case CKEY_1:			/** 进入指纹操作模式 **/
		case CKEY_2:			/** 进入密码操作模式 **/
		case CKEY_3:	
		case CKEY_4:	
		case CKEY_5:	
		case CKEY_6:	
		case CKEY_7:	
		case CKEY_8:	
		case CKEY_9:	
		case CKEY_asterisk:		/** *-asterisk **/
		case CKEY_0:		
		case CKEY_pound:		/** #-pound **/
			{
				actionInit();
				promptInit();
				beep();
				MBlackLED_on();				/** 背光灯亮 **/
				MMasterPower_on();			/** 主控制IC供电 **/
				g_tick = 0;					/** 同时开始计数 **/
    			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
				
			}	
			break;
			#endif
			
		default:					/** 可能的错误 **/
			break;
		}
		break;

	default:
		/** 未知 **/
			
		break;
	}  

    return  0;
}
/////////////////////////////////////////////////////

