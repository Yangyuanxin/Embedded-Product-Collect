
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
 * f_key
 * 只发出一个beep提示音，完成后进入下一步动作
 * 如果有按键按下，立即进入下一步动作
 *******************************************************************************/
int f_key(unsigned *pMsg)
{
	msg_t msg;
	func_t func;

	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;
	
    switch(msg.msgType)
	{
	case CMSG_INIT:
		if(getpassword_errno() >= CMAX_PSERR)		//错误计数有变化
    	{
    		init_fstack(&g_fstack);
			func.func = f_keyFuncInvalid;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
		else
		{
			promptDelay(TIMER_100MS);
    		vp_stor(CVOPID_PLEASEINPUTPSWORD);
    	
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	}
		break;
			
	case CPMT_OVER:
		promptInit();
		
		init_fstack(&g_fstack);
		func.func = f_keyInput;
    	stackpush(&g_fstack, &func);
    
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		promptInit();
		switch(msg.msgValue)
		{
		case CKEY_GOIN:			/** 摸室外把手，无关密码操作 **/
			break;
			
		case CKEY_GOOUT:			/** 摸室内把手, 进入密码管理选择状态 **/
			init_fstack(&g_fstack);
			func.func = f_KEYToOut;
    		stackpush(&g_fstack, &func);
			//beep();				/** 原则上摸室内把手没有beep声音，但此是管理状态，需要提示操作有效性 **/
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			break;
		
		case CKEY_1:			/** 进入密码开锁模式 **/
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
			{
				/** 常规数字按键，进入密码开锁流程 **/
				init_fstack(&g_fstack);
				func.func = f_keyInput;
    			stackpush(&g_fstack, &func);
    			
    			inq_irq(&g_msgq, &msg);		/** 在下一个状态，将此时键值重发一次 **/
    		}
			break;
		default:
			break;
		}
		break;
			
	case CMSG_TMR:
		g_tick++;		
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * f_keyFuncInvalid
 * 密码开锁功能(被锁定)无效
 * 只响应beep提示音,只有提示音影响状态转换
 *******************************************************************************/
int f_keyFuncInvalid(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		promptInit();
    	beep();
    	vp_stor(CVOPID_PSWORD);
    	vp_stor(CVOPID_INVALID);
    	
    	bbbeep();
		ledWarn(TIMER_100MS);		/** 警告闪烁 **/
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			goto GOTO_FUNCOVER;
		}
		break;
		
	case CPMT_OVER:
	GOTO_FUNCOVER:
		promptInit();
		//MOPBlue_on();			/** 闪烁之后恢复背光 **/		
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
					
	default:
		break;
	}
    return  0;
}


/*******************************************************************************
 * 密码管理
 * 查按键(3,4)判断输入数据是密码录入还是密码删除状态
 *******************************************************************************/
func_t g_next_func;
#if	0
int f_keyAdminSelect(unsigned *pMsg)
{
//	int i = 0;
	msg_t msg;
	func_t func;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{	
	case CMSG_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		beep();
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
	
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)				/** 选择过程超时退出 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;	
	case CMSG_DKEY:					/** 检测到按键按下 **/
		{
			switch(msg.msgValue)
			{
			case CKEY_GOIN:				/** 摸室外把手，无关密码操作 **/
				break;
				
			case CKEY_GOOUT:
				/** 摸室内把手, 退出 **/
				init_fstack(&g_fstack);
				func.func = f_Unlock;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
				
			case CKEY_3:
				g_next_func.func  = f_keyAdminRgstPSWord;
				init_fstack(&g_fstack);
				func.func = f_keyAdminAuthen;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
				
			case CKEY_4:
				g_next_func.func  = f_keyAdminDergstPSWord;
				init_fstack(&g_fstack);
				func.func = f_keyAdminAuthen;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
				
			case CKEY_1:		/** 其它按键不理会 **/
			case CKEY_2:
			case CKEY_5:	
			case CKEY_6:	
			case CKEY_7:	
			case CKEY_8:	
			case CKEY_9:	
			case CKEY_asterisk:		/** *-asterisk **/
			case CKEY_0:		
			case CKEY_pound:		/** #-pound **/
				break;
			default:
				break;
			}
		}
		break;
		
	default:
		break;
	}
    
    return  0;
}
#endif

/** 管理员身份验证 **/
int	iSearchCount = 0;
int f_keyAdminAuthen(unsigned *pMsg)
{
	msg_t msg;
	func_t func;
//	unsigned char ucResp; 
//	unsigned char ucArr[12]; 
//	unsigned char ucRet;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{	
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	MFingerPower_on();	/** 指纹头通电 **/
    	
    	beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_MANAGE);
    	vp_stor(CVOPID_FINGER);
    	vp_stor(CVOPID_PRESSFINGER);
		beep();
		promptDelay(TIMER_1SEC);
		break;
		 
	case CPMT_OVER:		/** 提示音结束，可以开始进行认证 **/
		//ClrTimer_irq(&g_timer[1]);
		promptInit();
		FP_Query(0, 0);
		break;
	
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			init_fstack(&g_fstack);
			func.func = f_keyUnlockFail;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_FGQUERY:
		/** 权限通过,开始录入、删除密码 **/
		MFingerPower_off();				/** 指纹头断电 **/
		g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	
		init_fstack(&g_fstack);
    	stackpush(&g_fstack, &g_next_func);
		break;
	
	case CMSG_FGOPFAIL:
		iSearchCount++;
		if(iSearchCount < 3)
		{
			FP_Query(0, 0);	/** 比对失败，重新发送指纹比对命令 **/
		}
		else
		{
			init_fstack(&g_fstack);
			func.func = f_keyUnlockFail;
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

int f_keyAdminDergstPSWord(unsigned *pMsg)
{
	msg_t msg;
	func_t func;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

	
    switch(msg.msgType)
	{	 	
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		password_del();
		flashWrite(g_flash.arrInt);
		
		promptInit();
		bbeep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_PASSED);		
    			
		//vp_stor(CVOPID_PSWORD);
		promptDelay(TIMER_300MS);
		vp_stor(CVOPID_DEL);
		vp_stor(CVOPID_SUCESS);
		bbeep();
		ledPrompt(TIMER_100MS);
		break;

	case CMSG_TMR:
		g_tick++;
		break;
		
	case CPMT_OVER:
		//ClrTimer_irq(&g_timer[1]);
		promptInit();
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
			
	default:
		break;
	}		
	return	0;
}
/*******************************************************************************
 * 注册密码
 *******************************************************************************/
int f_keyAdminRgstPSWord(unsigned *pMsg)
{
//	int i = 0;
	msg_t msg;
	func_t func;
	//static unsigned char password[10];
	static unsigned char password[18];
	static int iCount = 0;
//	charData_t charData;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{	 
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		
		promptInit();
		bbeep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_PASSED);				
		vp_stor(CVOPID_PLEASEINPUTPSWORD);
		lbeep();
		ledPrompt(TIMER_100MS);
		break;
		
	case CPMT_OVER:
		promptInit();
		
		//MOPBlue_on();			/** 闪烁之后恢复背光 **/		
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		break;
	
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 30)				/** 密码输入过程超时退出 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_DKEY:						/** 检测到按键按下 **/
		{
			switch(msg.msgValue)
			{
			case CKEY_GOIN:				/** 摸室外把手，无关密码操作 **/
			//case CTIKEY:
				break;
				
			case CKEY_GOOUT:
			//case CTOKEY:
				/** 摸室内把手, 退出 **/
				init_fstack(&g_fstack);
				func.func = f_Unlock;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
			
			case CKEY_1:		/** 其它按键不理会 **/
			case CKEY_2:
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
				/** 先移位 **/
				#if	1
				password[iCount] = msg.msgValue;	
				iCount++;
				#endif
				
				if(iCount >= 16) {
					init_fstack(&g_fstack);
				   	func.func = f_keyAdminRgstPSWordDone;
    				stackpush(&g_fstack, &func);
					g_tick = 0;
    				SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
					
					if((1 != psword_isFull()) && psword_inputConfirm(password) && password_add(password)) {
						promptInit();
						beep();
						vp_stor(CVOPID_EROLL);
						vp_stor(CVOPID_SUCESS);
						bbeep();
						ledPrompt(TIMER_100MS);
					} else {
						promptInit();
						beep();
						vp_stor(CVOPID_EROLL);
						vp_stor(CVOPID_FAILED);
						bbbeep();	/*** 添加失败或覆盖了前面的密码数据 ***/
						ledWarn(TIMER_100MS);
					}
					flashWrite(g_flash.arrInt);
				} else if(iCount == 8) {
					beep();
					vp_stor(CVOPID_PLEASEEROLLAGAIN);
				} else {
					beep();
				}
				break;
				
			default:
				break;
			}
		}
		break;
		
	default:
		break;
	}
    
    return  0;
}

int f_keyAdminRgstPSWordDone(unsigned *pMsg)
{
	msg_t msg;
	func_t func;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

	switch(msg.msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)			/** 超时退出 **/
		{
			goto GOTO_FUNCOVER;
		}
		break;
		
	case CPMT_OVER:
		GOTO_FUNCOVER:
			
		promptInit();
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    		
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);		
		break;
	
	default:
		break;
	}	
	return	0;
}

/*******************************************************************************
 * f_keyPasswordError
 * 密码错误(错误统计)
 * 只响应beep提示音,只有提示音影响状态转换
 *******************************************************************************/
 static unsigned char ucErrCount = 0;	//到目前累计密码连续出错次数
int f_keyPasswordError(unsigned *pMsg)
{
	func_t func; 
	static unsigned char ucErrCount_bak = 0;
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		promptInit();
    	beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_FAILED);
    	
    	bbbeep();
		ledWarn(TIMER_100MS);		/** 警告闪烁 **/
			
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			init_fstack(&g_fstack);
			func.func = f_keyUnlockFail;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		
		if(ucErrCount_bak == 0)		//第一次进入
    	{
    		ucErrCount_bak = getpassword_errno();
    		ucErrCount =  getpassword_errno();
    	}
    	ucErrCount++;
    	ucErrCount_bak = ucErrCount;
    	
    	if(ucErrCount >= CMAX_PSERR)
    	{
    		if(getpassword_errno() != ucErrCount)		//错误计数有变化
    		{
				setpassword_errno(ucErrCount);
				flashWrite(g_flash.arrInt);
			}
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    	}
    	else
    	{
    		init_fstack(&g_fstack);
			func.func = f_keyInput;
    		stackpush(&g_fstack, &func);
    		
    		vp_stor(CVOPID_PLEASEINPUTPSWORD);
    	}
		
		
		
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	
		break;
					
	default:
		break;
	}
    return  0;
}

/*******************************************************************************
 * 密码开锁状态
 * 计时器工作，超时退出
 *******************************************************************************/
int f_keyInput(unsigned *pMsg)
{
//	int i;
	msg_t msg;
	func_t func;
	static unsigned char password[10];
	static int iCount = 0;	   
	
//	static unsigned char ucErrCount_bak = 0;
//	charData_t charData;

	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType) 
	{
		
	case CMSG_INIT:
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	iCount = 0;
		break; 
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)			/** 超时退出 **/
		{
			if(getpassword_errno() != ucErrCount)		//错误计数有变化
    		{
				setpassword_errno(ucErrCount);
				flashWrite(g_flash.arrInt);
			}
			
			init_fstack(&g_fstack);
			func.func = f_keyUnlockFail;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		promptInit();
		#if	0	/** debug **/
			charData.ucVal = msg.msgValue & 0xff;
			charQueueIn_irq(&g_com1TxQue, &charData); 				
		#endif
		
		switch(msg.msgValue)
		{
		case CKEY_GOOUT:
		case CKEY_SET:
			
			/** 后续的按键输入过程中按到室内把手，或设置键开锁 **/
			init_fstack(&g_fstack);
			func.func = f_KEYToOut;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			break;
			
		case CKEY_GOIN:				/** 摸室外把手，无关密码操作，直接无视之 **/
			break;
		
		case CKEY_1:		/** 密码输入模式 **/
		case CKEY_2:	
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
			#if	1
			password[iCount] = msg.msgValue;	
			iCount++;
			#endif
			
			if(iCount >= 8)
			{
				if(password_Query(password) == 1)
				{
					init_fstack(&g_fstack);
			   		func.func = f_keyUnlockDoing;
    				stackpush(&g_fstack, &func);
    				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				
    				ucErrCount = 0;
    				if(getpassword_errno() != 0)		//错误计数有变化
    				{
						setpassword_errno(0);
						flashWrite(g_flash.arrInt);
					}
				}
				else	/** 没有通过密码比对 **/
				{
					init_fstack(&g_fstack);
			   		func.func = f_keyPasswordError;
    				stackpush(&g_fstack, &func);
    				
					g_tick = 0;
    				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				
				}
			}
			else
			{	
				beep();
			}
			break;
			
		default:
			break;
		}
		break;
		
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * f_keyUnlockDoing
 * 开锁过程(先开始一个beep+led Flash，再开锁)
 * 只有开锁动作影响状态转换
 *******************************************************************************/
int f_keyUnlockDoing(unsigned *pMsg)
{
	//msg_t msg;
	func_t func;

	//msg.msgType = ((msg_t *)pMsg)->msgType;
	//msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
   		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		promptInit();
		beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_PASSED);
    	vp_stor(CVOPID_OK);
    	
    	ledPrompt(TIMER_100MS);		/** 成功提示闪烁 **/	
    	lockAction();
    	
    	if(getpassword_errno() != 0)		//错误计数需要清零
    	{
			setpassword_errno(0);
			flashWrite(g_flash.arrInt);
		}
    	
		break;
			
	case CACT_OVER:
		actionInit();
		
		init_fstack(&g_fstack);
		func.func = f_keyInputDone;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	case CPMT_OVER:
		promptInit();
		
		//MOPBlue_on();			/** 闪烁之后恢复背光 **/		
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		break;
					
	default:
		break;
	}
    return  0;
}

/*******************************************************************************
 * f_keyInputDone
 * 开锁过程已结束
 * 只响应beep提示音,只有提示音影响状态转换
 *******************************************************************************/
int f_keyInputDone(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	//bbeep();
    	promptDelay(TIMER_100MS);
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			goto GOTO_FUNCOVER;
		}
		break;
		
	case CPMT_OVER:
	GOTO_FUNCOVER:
		promptInit();
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
					
	default:
		break;
	}
    return  0;
}

/*******************************************************************************
 * f_keyUnlockFail
 * 开锁过程失败
 * 只响应beep提示音,只有提示音影响状态转换
 *******************************************************************************/
int f_keyUnlockFail(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		promptInit();
    	beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_FAILED);
    	
    	bbbeep();
		ledWarn(TIMER_100MS);		/** 警告闪烁 **/
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			goto GOTO_FUNCOVER;
		}
		break;
		
	case CPMT_OVER:
	GOTO_FUNCOVER:
		promptInit();
		//MOPBlue_on();			/** 闪烁之后恢复背光 **/		
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
					
	default:
		break;
	}
    return  0;
}

/*******************************************************************************
 * 室内把手(功能键)开锁
 *******************************************************************************/
int f_KEYToOut(unsigned *pMsg)		/** 出门 **/
{
	func_t func;
	
	//switch(((msg_t *)pMsg)->msgType) 
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{	   
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CMSG_INIT:		/** 开锁, 并初始化计时 **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		lockAction();
		
		if(getpassword_errno() != 0)		//错误计数需要清零
    	{
			setpassword_errno(0);
			flashWrite(g_flash.arrInt);
		}
		
		break;
	
	case CMSG_BACK:
		MBlackLED_on();				/** 背光灯亮 **/
		MOPBlue_on();				/** 与背光一起的指示灯 **/
		break;
	
	case CMSG_DKEY:			/** 检测到按键按下 **/
		switch(((msg_t *)pMsg)->msgValue)
		{
			case CKEY_1:		/** 其它按键不理会 **/
			case CKEY_2:
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
				MBlackLED_on();				/** 背光灯亮 **/
				MOPBlue_on();				/** 与背光一起的指示灯 **/
				promptInit();
				beep();
				break;
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
		
	case CACT_OVER:
		actionInit();
		
		MBlackLED_off();	/** 背光灯灭 **/
		MFingerPower_off();	/** 指纹头断电 **/
		MMasterPower_off();	/** 最后断电 **/
		  
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);	
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	default:
		break;
	}

	return	0;
}

/*******************************************************************************
 * 设置键开锁-开锁期间可以进行一些其它操作(如:1、系统初始化; 2、ADC校验; 3、进入test模式)
 *　
 * 其中到的初始化指纹模板, 或许只需要改一个字节的索引表项即可
 *******************************************************************************/
 unsigned char menuVOPTab[] = {
 	#if	CSETOP_asterisk_pound
 	CVOPID_EROLL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_pound, CVOPID_1,
 	CVOPID_DEL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_asterisk, CVOPID_1, 
 	CVOPID_DEL, CVOPID_ALL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_asterisk, CVOPID_pound,
 	CVOPID_ADD, CVOPID_PSWORD, CVOPID_PLEASEPRESS, CVOPID_pound, CVOPID_2,
 	CVOPID_DEL, CVOPID_PSWORD, CVOPID_PLEASEPRESS, CVOPID_asterisk, CVOPID_2,
 	#else
 	CVOPID_EROLL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_1, CVOPID_3,
 	CVOPID_DEL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_1, CVOPID_4,
 	CVOPID_DEL, CVOPID_ALL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_1, CVOPID_5,
 	CVOPID_ADD, CVOPID_PSWORD, CVOPID_PLEASEPRESS, CVOPID_2, CVOPID_3,
 	CVOPID_DEL, CVOPID_PSWORD, CVOPID_PLEASEPRESS, CVOPID_2, CVOPID_4,
 	#endif
};	

int f_KEYSET(unsigned *pMsg)		/** 设置&管理 **/
{
	func_t func;
	msg_t msg;
	//static int iTimeOut = 0;
	static int iCount = 0;
	static unsigned char VOPIdx = 0;
	static unsigned char ucInputKey[10];

	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

	//switch(((msg_t *)pMsg)->msgType) 
	switch(msg.msgType)
	{
	case CMSG_TMR:
		g_tick++;
		//if(((g_tick > 10) && (iTimeOut == 0)) || ((g_tick > 20) && (iTimeOut != 0))) 
		if(g_tick > 15)
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CMSG_INIT:		/** 开锁, 并初始化计时 **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		beep();
    	vp_stor(menuVOPTab[VOPIdx]);
		break;
	
	case CMSG_BACK:
		MBlackLED_on();				/** 背光灯亮 **/
		MOPBlue_on();				/** 与背光一起的指示灯 **/
		break;
				
	case CMSG_DKEY:			/** 检测到按键按下 **/
		switch(msg.msgValue)
		{
		case CKEY_SET:				/** 室内设置键,室内把手失效，可作为应急开锁方式 **/
			if(g_tick < 1) {		/** 进入管理状态后，1秒钟以内不处理set键动作 **/
				break;
			}
			
			if(iCount != 8) {
				init_fstack(&g_fstack);
				func.func = f_KEYToOut;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
			}
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
		case CKEY_asterisk:		/** *-asterisk **/
		case CKEY_0:		
		case CKEY_pound:		/** #-pound **/	
			
//			iTimeOut = 20;		/** 设定超时时间 **/
			MBlackLED_on();				/** 背光灯亮 **/
			MOPBlue_on();				/** 与背光一起的指示灯 **/
			ucInputKey[iCount] = msg.msgValue;	
			iCount++;
			
			VOPIdx = TABSIZE(menuVOPTab);
			promptInit();			
			beep();
			
			if(iCount == 2)
			{
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_pound) && (ucInputKey[1] == CKEY_1))	/** 录入指纹 **/
				#else
				if((ucInputKey[0] == CKEY_1) && (ucInputKey[1] == CKEY_3))	/** 录入指纹 **/
				#endif
				{
    				g_tick = 0;
					SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_INIT);	/** 延缓消息发送过程,等待beep完成 **/
    				
					promptInit();
					beep();
					init_fstack(&g_fstack);
					func.func = f_RgstfingerInit;
    				stackpush(&g_fstack, &func);
				}
				
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_asterisk) && (ucInputKey[1] == CKEY_1))	/** 删除指纹 **/
				#else
				if((ucInputKey[0] == CKEY_1) && (ucInputKey[1] == CKEY_4))	/** 删除指纹 **/
				#endif
				{
					if(fingerTab_isEmpty())	{	/** 指纹模板空，警告提示 **/
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** 延缓消息发送过程 **/
						promptInit();
						
						init_fstack(&g_fstack);
						func.func = f_fingerNoFPWarn;
    					stackpush(&g_fstack, &func);
					} else {
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** 延缓消息发送过程 **/
						MFingerPower_on();	/** 指纹头上电 **/	
						promptInit();
						
						init_fstack(&g_fstack);
						//func.func = f_DegstfingerInit;
						func.func = f_fingerDeleteInit;
    					stackpush(&g_fstack, &func);
    				}
				}
				
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_asterisk) && (ucInputKey[1] == CKEY_pound))	/** 删除全部指纹 **/
				#else
				if((ucInputKey[0] == CKEY_1) && (ucInputKey[1] == CKEY_5))	/** 删除全部指纹 **/
				#endif
				{
					if(fingerTab_isEmpty())	{	/** 指纹模板空，警告提示 **/
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** 延缓消息发送过程 **/
						promptInit();
						
						init_fstack(&g_fstack);
						func.func = f_fingerNoFPWarn;
    					stackpush(&g_fstack, &func);
					} else {
						promptInit();
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 延缓消息发送过程 **/
						//MFingerPower_off();			/** 指纹头断电无必要 **/
						
						g_subState = 0;
						init_fstack(&g_fstack);
						func.func = f_fingerDelAllFP;
    					stackpush(&g_fstack, &func);
    				}
				}
				
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_pound) && (ucInputKey[1] == CKEY_2))	/** 录入密码 **/
				#else
				if((ucInputKey[0] == CKEY_2) && (ucInputKey[1] == CKEY_3))	/** 录入密码 **/
				#endif
				{
					if(fingerTab_isEmpty())	{	/** 指纹模板空，警告提示 **/
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** 延缓消息发送过程 **/
						promptInit();
						
						init_fstack(&g_fstack);
						func.func = f_fingerNoFPWarn;
    					stackpush(&g_fstack, &func);
					} else {
						g_next_func.func  = f_keyAdminRgstPSWord;
						init_fstack(&g_fstack);
						func.func = f_keyAdminAuthen;
    					stackpush(&g_fstack, &func);
    					
						promptInit();
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
				
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_asterisk) && (ucInputKey[1] == CKEY_2))	/** 删除密码 **/
				#else
				if((ucInputKey[0] == CKEY_2) && (ucInputKey[1] == CKEY_4))	/** 删除密码 **/
				#endif
				{
					if(fingerTab_isEmpty())	{	/** 指纹模板空，警告提示 **/
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** 延缓消息发送过程 **/
						promptInit();
						
						init_fstack(&g_fstack);
						func.func = f_fingerNoFPWarn;
    					stackpush(&g_fstack, &func);
					} else {
						g_next_func.func  = f_keyAdminDergstPSWord;
						init_fstack(&g_fstack);
						func.func = f_keyAdminAuthen;
    					stackpush(&g_fstack, &func);
    					
						promptInit();
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
				
				if((ucInputKey[0] == CKEY_3) && (ucInputKey[1] == CKEY_5)) {	/** 低电校验 **/
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1) {			/** 要求一直压着set键 **/
					//if(1) {	/** test for function **/
						init_fstack(&g_fstack);
						func.func = f_keyLowPower;
    					stackpush(&g_fstack, &func);
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
				
				if((ucInputKey[0] == CKEY_3) && (ucInputKey[1] == CKEY_6)) {	/** 测试模式testMode **/
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1) {			/** 要求一直压着set键 **/
						init_fstack(&g_fstack);
						func.func = f_testMode;
    					stackpush(&g_fstack, &func);
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
				
				if((ucInputKey[0] == CKEY_3) && (ucInputKey[1] == CKEY_7)) {	/** 恢复出厂值 **/
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1) {			/** 要求一直压着set键 **/
						init_fstack(&g_fstack);
						func.func = f_keyRestorFactorySetting;
    					stackpush(&g_fstack, &func);
    					promptInit();
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
			} else if(iCount == 9) {
				#if	0
				if((ucInputKey[0] == CKEY_3) && (ucInputKey[1] == CKEY_5))	
				#else
				if(	(ucInputKey[0] == CKEY_1)
					 && (ucInputKey[1] == CKEY_8)
					 && (ucInputKey[2] == CKEY_9) 
					 && (ucInputKey[3] == CKEY_3)
					 && (ucInputKey[4] == CKEY_1) 
					 && (ucInputKey[5] == CKEY_2)
					 && (ucInputKey[6] == CKEY_2)
					 && (ucInputKey[7] == CKEY_6) 
					 && ((ucInputKey[8] == CKEY_GOOUT) || (ucInputKey[8] == CKEY_SET)))
					 //&& (ucInputKey[8] == CKEY_GOOUT)))
				#endif
				{
					init_fstack(&g_fstack);
					func.func = f_keyLowPower;
    				stackpush(&g_fstack, &func);
					g_tick = 0;
    				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				}
			}
			
			if(iCount > 9) {
				init_fstack(&g_fstack);
				func.func = f_KEYSETINVALID;
    			stackpush(&g_fstack, &func);
    				
				promptInit();
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			}
			break;
			
		default:
			break;
		}
		break;
			
	case CPMT_OVER:
		promptInit();
		VOPIdx++;
		if(VOPIdx < TABSIZE(menuVOPTab)) {
			if(g_tick > 7) {
				g_tick = 7;
			}
    		vp_stor(menuVOPTab[VOPIdx]);
    	} else {
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	}
		break;
		
		#if	0
	case CACT_OVER:
		actionInit();
		if(iTimeOut == 0)		/** 没有过按键 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			
			MBlackLED_off();	/** 背光灯灭 **/
			MFingerPower_off();	/** 指纹头断电 **/
			MMasterPower_off();	/** 最后断电 **/
		}
		break;
		#endif
		
	default:
		break;
	}

	return	0;
}

/** 按set键进入后，无效按键操作 **/
int f_KEYSETINVALID(unsigned *pMsg)		/** 设置&管理无效 **/
{
	func_t func;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		promptInit();
    	beep();
    	vp_stor(CVOPID_SET);
    	vp_stor(CVOPID_INVALID);
    	bbbeep();
		ledWarn(TIMER_100MS);
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{
			/** 超时退出 **/
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		
		break;

	case CPMT_OVER:
		promptInit();
				
		//MOPBlue_on();			/** 闪烁之后恢复背光 **/		
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
			
	default:
		break;
	}

	return	0;
}

/******************************************************************************* 
 * 低电报警-校准
 *******************************************************************************/
int f_keyLowPower(unsigned *pMsg)
{
	func_t func;
	msg_t msg;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		msg.msgType = CMSG_CALC;	/** 电压ADC采样。如果已完成也重来一次 **/
		inq_irq(&g_msgq, &msg);
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{
			/** 超时退出 **/
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		
		if(g_flag & (1 << 8))	/** 采样完成 **/
		{
			ADC_setValue(g_iADCCurr);
			flashWrite(g_flash.arrInt);
			
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    		
    		lbeep();	
			promptDelay(TIMER_1SEC);
    		bbeep();
		}
		break;

	case CPMT_OVER:
		promptInit();
		break;
			
	default:
		break;
	}

	return	0;
}

#if	0
/******************************************************************************* 
 * 低电报警-校准完成
 *******************************************************************************/
int f_keyLowPowerDone(unsigned *pMsg)
{
	func_t func;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CPMT_OVER:
		promptInit();
	
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	default:
		break;
	}

	return	0;
}
#endif


/******************************************************************************* 
 * 恢复出厂值
 *******************************************************************************/
int f_keyRestorFactorySetting(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		promptInit();
		beep();
		promptDelay(TIMER_1SEC);
		
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_2SEC, CFIGER_INIT);
    	
		password_del();
		flashWrite(g_flash.arrInt);		/** 先删除全部密码 **/
    	
    	fingerTab_fill(0xff, 0xff);		/** 删除全部指纹模板索引 **/
		resetOP_setValue(0xa0);
		flashWrite(g_flash.arrInt);	
    	
    	MFingerPower_on();				/** 指纹头通电 **/
		break;
	
	case CFIGER_INIT:		/** 指纹头通电1秒后初始化指纹头 **/
		g_tick++;
		if(g_tick > 2) {
			/** 指纹头初始化失败 **/
			init_fstack(&g_fstack);
			//func.func = f_Unlock;
			func.func = f_fingerError;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);			
		}
    	//SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_INIT);		/**　如果没有　**/
    	FP_INIT();					/** 指纹头初始化 **/
		break;
		
	case CMSG_FGINIT:		/** 指纹头初始化完成, 开始动作 **/
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_TMR);
		FP_DelAll();
		break;
	
	case CMSG_FGDERG:
		MFingerPower_off();				/** 指纹头断电 **/
		
		init_fstack(&g_fstack);
		//func.func = f_Unlock;
		func.func = f_keyRestorFactorySettingDone;
    	stackpush(&g_fstack, &func);
    	g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);			
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{
			/** 超时退出 **/
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CPMT_OVER:
		promptInit();
		break;
			
	default:
		break;
	}

	return	0;
}

/******************************************************************************* 
 * 恢复出厂值
 *******************************************************************************/
int f_keyRestorFactorySettingDone(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		promptInit();
    	vp_stor(CVOPID_RECOVERED);				/** 恢复出厂设置 **/
		lbeep();
		promptDelay(TIMER_1SEC);
		lbeep();
		ledPrompt(TIMER_100MS);
		
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{
			/** 超时退出 **/
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CPMT_OVER:
		promptInit();
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		break;
			
	default:
		break;
	}

	return	0;
}

/////////////////////////////////////////////////////

