
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"
#include "arch.h"	  
#include "driver.h"		
#include "l_voicePrompt.h"	  
#include "l_fingprint.h"
#include "func_idle.h"
#include "f_Unlock.h "
#include "f_finger.h "	
#include "f_key.h "	  
#include "charQue.h"	
#include "main.h"
 
/**
 *	只有录入过指纹才会运行到这里
 * f_finger -> f_fingerInit -> f_fingerSearch(1、开锁　2、录指纹 3、删指纹)
 *　　　　　　　　　　　　　-> f_fingerError(指纹头故障)
 **/
 
//static unsigned char ManagerId = 0;		/** 录入的指纹对应的id号 **/
static unsigned char ucRgstCnt = 0;			/** 录入指纹步骤号 **/
/*******************************************************************************
 * 通过按键1进入指纹开锁状态
 *******************************************************************************/
#if	0
int f_fingerStart(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		break;
		
	case CMSG_INIT:	
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		MFingerPower_on();			/** 给指纹头供电 **/
		beep();
		break;
		
	case CPMT_OVER:
		promptInit();
		g_tick = 0;
		FP_INIT();		/** reset fingerPrint(The command have no responsed) **/

		func.func = f_fingerInit;
    	stackpush(&g_fstack, &func);
		
		break;
		
	default:
		break;
	}
    
    return  0;
}
#endif

/*******************************************************************************
 * step 1: search fingerPrint for Unlock only　通过按指纹头进入开锁状态
 *******************************************************************************/
int f_finger(unsigned *pMsg)
{
	func_t func;
	   
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		break;
		
	case CMSG_INIT:	
		g_tick = 0;
		promptInit();
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);

		init_fstack(&g_fstack);
		func.func = f_fingerInit;
    	stackpush(&g_fstack, &func);
		FP_INIT();		/** reset fingerPrint(The command have no responsed) **/
		break;
		
	default:
		break;
	}
    
    return  0;
}

static int iSearchCount = 0;	/** 比对次数 **/
/** ********************************************************
 * initial finger print 
 * Description: 初始化指纹头
 * 				sucess: 查指纹模板: 无指纹就开锁, 有指纹就发送查指纹命令
 * 				fail: 继续偿试, 如果偿试3次都失败, 就断电退出
 ***********************************************************/
int f_fingerInit(unsigned *pMsg)
{
	func_t func;
//	msg_t msg; 
	//charData_t charData;
//	unsigned char ucArr[12];
//	unsigned char ucResp;   
//	unsigned char ucRet;
	
//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick < 3)
		{
			FP_INIT();		/** reset fingerPrint again(The command have no responsed) **/
		}
		else	/** initial failed more than 3 times **/
		{
			/** 指纹头初始化失败 **/
			init_fstack(&g_fstack);
			//func.func = f_Unlock;
			func.func = f_fingerError;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
	
	case CPMT_OVER:		/** 开锁前的提示音完成 **/
		promptInit();
		break;
		
	case CMSG_FGINIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_fingerSearch;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}
	return	0;
}

/** 指纹头故障报警 **/
int f_fingerError(unsigned *pMsg)
{
	func_t func;
//	msg_t msg; 

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		promptInit();
		lbeep();
		promptDelay(TIMER_500MS);
		lbeep();
		promptDelay(TIMER_500MS);
		lbeep();
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10) {
			
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
	
	case CPMT_OVER:		/** 开锁前的提示音完成 **/
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


/***********************************************************
 * function: f_fingerSearch
 * Description: search fingerPrint unlock if passed or retry if failed
 *
 * 指纹比对并开锁
 * 
 ***********************************************************/
int f_fingerSearch(unsigned *pMsg)
{
	func_t func;
//	msg_t msg; 

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		
		FP_Query(0, CTOTALFINGER - 1);	/** 如果有指纹模板，发送指纹比对命令 **/
		iSearchCount = 0;	/** 比对次数 **/
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** 超时 **/
		{
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 定时 **/
			init_fstack(&g_fstack);
			func.func = f_fingerSearchFail;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CMSG_FGQUERY:
		/** 权限通过,开锁 **/
		MFingerPower_off();				/** 指纹头断电 **/
		g_tick = 0;						/** 防止可能的操作途中，超时退出 **/
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 定时 **/
    	
    	init_fstack(&g_fstack);
		func.func = f_fingerSearchSuccess;
    	stackpush(&g_fstack, &func);
		break;
		
	case CMSG_FGOPFAIL:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);	/** 定时 **/
		iSearchCount++;
		if(iSearchCount < 3)
		{
			FP_Query(0,  CTOTALFINGER - 1);	/** 比对失败，重新发送指纹比对命令 **/
		}
		else
		{
			MFingerPower_off();
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 定时 **/
			init_fstack(&g_fstack);
			func.func = f_fingerSearchFail;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** 开锁前的提示音完成 **/
		promptInit();
		break;
		
	#if	0
	case CACT_OVER:		/** 开锁动作完成 **/
		actionInit();
		#if	1
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_INIT);
    	#endif
		break;
   	#endif
		
	case CMSG_BACK:
		MBlackLED_on();			/** 背光 **/
		MOPBlue_on();			/** 同背光位置的指示灯光 **/	
		break;
		
	case CMSG_DKEY:						/** 查询指纹过程中检测到按键按下 **/
		switch(((msg_t *)pMsg)->msgValue)
		{
		case CKEY_GOOUT:
		case CKEY_SET:
			/** 刷指纹时，有人在室内握把手，或设置键开锁 **/
			MFingerPower_off();				/** 指纹头断电 **/
			init_fstack(&g_fstack);
			func.func = f_KEYToOut;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
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
			if(MFP_isInductionPower()) {	/** 指纹头有感应上电功能时，按键无反应 **/
				promptInit();
				beep();
			} else {		/** 指纹头没有感应上电功能时，在指纹开锁状态中如果有按键按下进入密码开锁状态  **/
				MFingerPower_off();				/** 指纹头断电 **/
				init_fstack(&g_fstack);
				func.func = f_keyInput;
    			stackpush(&g_fstack, &func);
    			
				//inq_irq(&g_msgq, &msg);
				inq_irq(&g_msgq, (msg_t *)pMsg);	/** !!!将按键消息重发一次 **/
				
    			//g_tick = 0;
    			//SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			}
			break;
				
		default:
			
			break;
		}
	
	default:
		break;
	}
	return	0;
}


/*******************************************************************************
 * 指纹比对成功 
 *******************************************************************************/
int f_fingerSearchSuccess(unsigned *pMsg)
{
	func_t func;
	
    switch(((msg_t *)pMsg)->msgType)
	{		
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT消息 **/
		
		promptInit();
		beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_PASSED);
    	vp_stor(CVOPID_OK);
    	ledPrompt(TIMER_100MS);
		lockAction();	/** 开锁 **/
		
		if(getpassword_errno() != 0)		//错误计数需要清零
    	{
			setpassword_errno(0);
			flashWrite(g_flash.arrInt);
		}
		
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** 超时 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** 开锁前的提示音完成 **/	
		promptInit();
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		break;

	case CACT_OVER:		/** 开锁前的提示音完成 **/	
		actionInit();	
		g_tick = 0;	
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
    	func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}

	return	0;
}


/*******************************************************************************
 * 指纹比对失败 
 *******************************************************************************/
int f_fingerSearchFail(unsigned *pMsg)
{
	func_t func;
	
    switch(((msg_t *)pMsg)->msgType)
	{		
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT消息 **/
		g_tick = 0;
		MFingerPower_off();			/** 指纹头断电 **/
		MBlackLED_off();			/** 背光灭 **/
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		promptInit();
		bbbeep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_FAILED);
		ledWarn(TIMER_100MS);		/** 警告闪烁 **/
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** 超时 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** 开锁前的提示音完成 **/	
		promptInit();
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}

	return	0;
}


/*******************************************************************************
 * 指纹录入失败,报警(其它的管理功能失败也可执行到此)
 *******************************************************************************/
int f_fingerEnrollFail(unsigned *pMsg)
{
	func_t func;
	
    switch(((msg_t *)pMsg)->msgType)
	{
		
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT消息 **/
		g_tick = 0;
		MFingerPower_off();			/** 指纹头断电 **/
		promptInit();
		
    	vp_stor(CVOPID_EROLL);
    	vp_stor(CVOPID_FAILED);
		bbbeep();
		ledWarn(TIMER_100MS);		/** 警告闪烁 **/
		
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** 超时 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** 开锁前的提示音完成 **/	
		promptInit();
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}

	return	0;
}

/*******************************************************************************
 * 指纹删除失败,报警
 *******************************************************************************/
int f_fingerDelFail(unsigned *pMsg)
{
	func_t func;
	
    switch(((msg_t *)pMsg)->msgType)
	{
		
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT消息 **/
		g_tick = 0;
		MFingerPower_off();			/** 指纹头断电 **/
		promptInit();
		
    	vp_stor(CVOPID_DEL);
    	vp_stor(CVOPID_FAILED);
		bbbeep();
		ledWarn(TIMER_100MS);		/** 警告闪烁 **/
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** 超时 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** 开锁前的提示音完成 **/	
		promptInit();
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}

	return	0;
}

/*******************************************************************************
 * 等待后续处理(只等下一个按键:室内按键)
 *******************************************************************************/
//int f_fingerInitAll(unsigned *pMsg)
int f_fingerDelAllFP(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;
//	unsigned char ucResp;
//	unsigned char ucRet;
//	unsigned char ucArr[12];
	int iTmp = 0;
//	charData_t charData;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
    		init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
			
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		FP_INIT();
		beep();
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		switch(((msg_t *)pMsg)->msgValue) {
		case CKEY_GOOUT:	//室内把手作为正式开始
		case CKEY_SET:		//室内把手或设置键作为正式开始
    		init_fstack(&g_fstack);
			func.func = f_fingerDelAllFPDone;
    		stackpush(&g_fstack, &func);
    		
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    		
    		iTmp = resetOP_getValue();
			iTmp++;
    		if(iTmp == 0xa2) {
				init_fstack(&g_fstack);
				func.func = f_fingerDelAllFPDoing;
    			stackpush(&g_fstack, &func);
    			
			} else if((iTmp < 0xa2) && (iTmp > 0xa0)) {	/** 0xa1 **/
				resetOP_setValue(iTmp);
				flashWrite(g_flash.arrInt);
			} else {									/** 未知数值 **/
				resetOP_setValue(0xa0);
				flashWrite(g_flash.arrInt);
			}
			break;
		default:
			break;
		}
		break;
		
	case CMSG_FGINIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
				
	default:
		break;
    }	

	return	0;
}


/*******************************************************************************
 * 删除全部指纹
 * 从表现上看，“删除全部指纹及指纹的全部索引”与“删除第一管理者管辖的全部指纹及全部索引”效果相同
 *******************************************************************************/
//int f_fingerInitAllDoing(unsigned *pMsg)
int f_fingerDelAllFPDoing(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;
	
    switch(((msg_t *)pMsg)->msgType) {		
	case CMSG_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_DEL);
   		MFingerPower_on();			/** 给指纹头供电, 并等待1sec后开始动作 **/
   		beep();
   		promptDelay(TIMER_100MS);
		break;
		
	case CFIGER_DEL:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		g_tick = 0;	
   		iSearchCount = 0;		/** 命令执行次数 **/
   		
    	fingerTab_fill(0xff, 0xff);				/** delte all FP **/
		resetOP_setValue(0xa0);
		flashWrite(g_flash.arrInt);	
		#if	1
   			FP_DelByManager(0);
   		#else
   			FP_DelAll();
   		#endif
		break;
		
	case CMSG_TMR:
		g_tick++;
		
		if(g_tick >= 10) {
		GOTO_DELETE_FAILED:
    		init_fstack(&g_fstack);
			func.func = f_fingerDelFail;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
		
	case CMSG_FGDERG:				//指纹删除成功
		MFingerPower_off();			/** 指纹头断电 **/
		init_fstack(&g_fstack);
		func.func = f_fingerDelAllFPDone;
    	stackpush(&g_fstack, &func);
    		
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	
		break;
		
	case CMSG_FGOPFAIL:	
		iSearchCount++;
		if(iSearchCount < 3) {
			#if	1
   				FP_DelByManager(0);
   			#else
   				FP_DelAll();
   			#endif
   		} else {
   			goto GOTO_DELETE_FAILED;
   		}
		break;
		
	default:
		break;
    }	

	return	0;
}

/*******************************************************************************
 * 处理完成，等待提示音结束
 *******************************************************************************/
//int f_fingerInitAllDone(unsigned *pMsg)
int f_fingerDelAllFPDone(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;
	
	int iTmp = 0;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
		GOTO_INITALL:
    		init_fstack(&g_fstack);
			func.func = f_UnlockFail;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		goto GOTO_INITALL;
		//break;
		
	case CMSG_INIT:
		iTmp = resetOP_getValue();
    	if(iTmp == 0xa0) {		/** 0xa1->0xa2(then set 0xa0), then delte all FP **/
    	
    		vp_stor(CVOPID_DEL);
    		vp_stor(CVOPID_ALL);
    		vp_stor(CVOPID_FINGER);
    		vp_stor(CVOPID_SUCESS);
			lbeep();
			promptDelay(TIMER_1SEC);
			lbeep();
			ledPrompt(TIMER_100MS);
		} else if((iTmp < 0xa2) && (iTmp > 0xa0)) {		/** 0xa0->0xa1 **/
			lbeep();
		} else {
			bbbeep();
		}

    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		g_tick = 0;
		break;
		
	default:
		break;
    }	

	return	0;
}

/*******************************************************************************
 * Only search fingerPrint before register fingerPrint
 * Description:
 * initial FingerPrint and play VOP. VOP always longer than initial.
 * end of the VOP, then compare the FP
 * 菜单操作--->进入录指纹流程之前：指纹头上电，初始华指纹头
 *******************************************************************************/
//int f_RgstfingerBefore(unsigned *pMsg)
int f_RgstfingerInit(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;

//	unsigned char ucRet;
//	unsigned char ucArr[12];
	
//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)		/** 超时退出 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		g_tick = 0;
		
		if(g_subState >= 4) {	//the FP initial OK
			#if	1
			init_fstack(&g_fstack);
			func.func = f_Rgstfinger;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 初始化指纹头完成1sec后, 再开始后续操作 **/
    		#endif
    	} else {
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
		break;
		
	case CMSG_INIT:
		promptInit();
    	if(fingerTab_isEmpty()) {	/** 指纹模板空，直接开始录指纹 **/
			init_fstack(&g_fstack);
			func.func = f_RgstAdminfingerInit;
    		stackpush(&g_fstack, &func);
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		} else {
    		MFingerPower_on();			/** 指纹头供电 **/
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_INIT);
    		g_subState = 0;			//initial
    		
    		vp_stor(CVOPID_COMPARISON);
    		vp_stor(CVOPID_MANAGE);
    		vp_stor(CVOPID_FINGER);
    		vp_stor(CVOPID_PRESSFINGER);
			beep();
		}
		break;
		
	case CFIGER_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	g_subState++;				//Fingerprint initial ok
		FP_INIT();					/** 指纹头初始化 **/	
		break;
		
	case CMSG_FGINIT:	/** 指纹头初始化过程完成 **/
		#if	0
		init_fstack(&g_fstack);
		func.func = f_Rgstfinger;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 初始化指纹头完成1sec后, 再开始后续操作 **/
    	#endif
    	
    	g_subState += 4;	//Fingerprint initial ok
		break;
				
	default:
		break;
    }	

	return	0;
}

/*******************************************************************************
 * Only play voice prompt register administrator fingerPrint
 * 查询指纹模板过程中检查到3键--->进入录指纹流程之前重新初始化指纹头
 *******************************************************************************/
int f_RgstAdminfingerInit(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)		/** 超时退出 **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		g_tick = 0;
		
		if(g_subState >= 4) {	//the FP initial OK
    		init_fstack(&g_fstack);
			func.func = f_RgstAdminfingerDoing;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_INIT);	/** 初始化指纹头完成1sec后, 再开始后续操作 **/
    		//bbeep();
    	} else {
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
		break;
		
	case CACT_OVER:
		actionInit();
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_INIT);
    	MFingerPower_on();			/** 指纹头供电 **/
    	g_subState = 0;
    	
		promptInit();
    	vp_stor(CVOPID_EROLL);
    	vp_stor(CVOPID_MANAGE);
    	vp_stor(CVOPID_FINGER);
    	vp_stor(CVOPID_PRESSFINGER);
		beep();
		break;
		
	case CFIGER_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	g_subState++;				//Fingerprint initial ok
		FP_INIT();					/** 指纹头初始化 **/	
		break;
		
	case CMSG_FGINIT:
		#if	0
		init_fstack(&g_fstack);
		func.func = f_RgstAdminfingerDoing;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_INIT);	/** 初始化指纹头完成1sec后, 再开始后续操作 **/
    	#endif
    	
    	g_subState += 4;	//Fingerprint initial ok
		break;
				
	default:
		break;
    }	

	return	0;
}


/*******************************************************************************
 * Only search fingerPrint before register fingerPrint
 * 查询指纹模板过程中检查到3键-进入录指纹流程(重新上电，重新初始化过程已经完成)
 *******************************************************************************/
int f_Rgstfinger(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		break;
		
	case CPMT_OVER:
		promptInit();
		break;		
		
	case CACT_OVER:
		actionInit();
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		FP_Query(0, 2);		/** 发送(管理者)指纹比对命令 **/
		iSearchCount = 0;	/** 比对次数清零 **/
		break;
		
	case CMSG_FGQUERY:
		if(g_ucUserId < 3)
		{
			/** 管理员权限通过，依照管理员id录指纹 **/
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_4SEC, CMSG_INIT);	/** 3秒钟后产生一个CMSG_INIT消息 **/
			g_subState = 0;
			
			promptInit();
			bbeep();
    		vp_stor(CVOPID_COMPARISON);
    		vp_stor(CVOPID_PASSED);		
			ledPrompt(TIMER_100MS);
			vp_stor(CVOPID_PRESSFINGER);	
			
			lbeep();
			//promptDelay(TIMER_1SEC);
			
			init_fstack(&g_fstack);
			func.func = f_RgstfingerDoing;
    		stackpush(&g_fstack, &func);
		}
		else
		{
			iSearchCount++;
			if(iSearchCount < 3)
			{
				FP_Query(0, 2);	/** 比对失败，重新发送指纹比对命令 **/
			}
			else
			{
				init_fstack(&g_fstack);
				func.func = f_fingerEnrollFail;
    			stackpush(&g_fstack, &func);
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);		/** CMSG_INIT消息 **/
			}
		}
		
		break;
		
	case CMSG_FGOPFAIL:		/** 操作失败 **/
		iSearchCount++;
		if(iSearchCount < 3)
		{
			FP_Query(0, 2);	/** 比对失败，重新发送指纹比对命令 **/
		}
		else
		{
			init_fstack(&g_fstack);
			func.func = f_fingerEnrollFail;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);		/** CMSG_INIT消息 **/
		}
		break;
		
	default:
		break;
	}
	
    return  0;
}

/*******************************************************************************
 * register Administrator fingerPrint
 *  发现没有指纹，到这里开始直接录第一管理者指纹
 *******************************************************************************/
int f_RgstAdminfingerDoing(unsigned *pMsg)
{
	func_t func;
	msg_t msg; 
//	charData_t charData;

	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		ucRgstCnt = 1;
		FP_Register(0, ucRgstCnt);
		
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** 超时退出 **/
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CFIGER_TOUT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);	/**  **/
		break;
		
	case CMSG_FGOPFAIL:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		init_fstack(&g_fstack);
		func.func = f_fingerEnrollFail;
    	stackpush(&g_fstack, &func);
		break;	
	
	case CFIGER_ON:		/** 检测到指纹按下, 关闭声音提示，减少干扰 **/
		//promptInit();
		break;
			
	case CFIGER_FGPRPRD:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		ucRgstCnt++;
		FP_Register(0, ucRgstCnt);
		break;	
		
	case CMSG_FGRGST:	
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_3SEC, CFIGER_FGPRPRD);	/** 定时 **/
		if(ucRgstCnt == 1) {
			promptInit();
			beep();
			vp_stor(CVOPID_PLEASEEROLLAGAIN);
			beep();
			//ucRgstCnt++;
			//FP_Register(0, ucRgstCnt);
		} else if(ucRgstCnt == 2) {
			/** 注册成功 **/
			fingerTab_setEntry(0, 0xa5);
			if(MFP_isON()) {		/** 查指纹头是否有感应上电功能 **/
				MFP_InductionPower_enable();
			} else {
				MFP_InductionPower_disable();
			}
			flashWrite(g_flash.arrInt);
			MFingerPower_off();			/** 指纹头断电 **/
					
			promptInit();
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			func.func = f_RgstAdminfingerDone;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:
		promptInit();		/** 停止此消息的发送 **/
		break;
		
	case CACT_OVER:
		actionInit();		/** 停止此消息的发送 **/
		break;
		
	default:
		break;
	}

	return	0;
}

/*******************************************************************************
 * 第一次录入指纹完成。其中有开锁动作，需要开锁动作及提示音都完成，才能退出
 *******************************************************************************/
int f_RgstAdminfingerDone(unsigned *pMsg)
{
	func_t func;
//	msg_t msg; 
//	static int complete = 0;
//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		
		promptInit();
		beep();
    	vp_stor(CVOPID_FINGER);
		vp_stor(CVOPID_EROLL);
    	vp_stor(CVOPID_SUCESS);
		bbeep();
		ledPrompt(TIMER_100MS);		/** 注册成功led提示 **/
		break;	
		
	case CMSG_TMR:
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
				
	case CPMT_OVER:
		promptInit();		/** 停止此消息的发送 **/
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

	return 0;
}
/*******************************************************************************
 * register　fingerPrint
 *******************************************************************************/
int f_RgstfingerDoing(unsigned *pMsg)
{
	func_t func;
	msg_t msg; 	
			   
//	int i;
	unsigned char ucRet;
//	unsigned char ucResp;
//	unsigned char ucArr[12];
//	charData_t charData;
	
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** 超时退出，警告之 **/
			bbbeep();
			
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CMSG_INIT:		/** 开始录入指纹 **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);	/** 初始化定时器 **/
		ucRgstCnt = 1;
		g_ucAddUserId = FP_AddByManager(g_ucUserId, ucRgstCnt);
		if(g_ucAddUserId == 0)
		{
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			
			func.func = f_fingerEnrollFail;
    		stackpush(&g_fstack, &func);
		}
		else
		{
		}		

		#if	0	/** debug **/
		charData.ucVal = 0xAA;
		charQueueIn_irq(&g_com1TxQue, &charData); 
		
		charData.ucVal = g_ucUserId;  
		charQueueIn_irq(&g_com1TxQue, &charData);
		charData.ucVal = g_ucAddUserId;  
		charQueueIn_irq(&g_com1TxQue, &charData);
                            
		charData.ucVal = 0x55;                    
		charQueueIn_irq(&g_com1TxQue, &charData); 
		#endif
		
		break;
			
	case CMSG_FGOPFAIL:		/** 录入操作失败 **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 初始化定时器 **/

		init_fstack(&g_fstack);		
		func.func = f_fingerEnrollFail;
    	stackpush(&g_fstack, &func);
		break;	
	
	case CFIGER_ON:		/** 检测到指纹按下, 关闭声音提示，减少干扰 **/
		//promptInit();
		break;
			
	case CFIGER_FGPRPRD:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		
		ucRgstCnt++;
		ucRet = FP_AddByManager(g_ucUserId, ucRgstCnt);
		if(ucRet != g_ucAddUserId)
		{
			g_ucAddUserId = ucRet;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 初始化定时器 **/
				
			init_fstack(&g_fstack);	
			func.func = f_fingerEnrollFail;
    		stackpush(&g_fstack, &func);
		}
		break;	
			
	case CMSG_FGRGST:
		if(ucRgstCnt == 1)
		{
			#if	1
			promptInit();
			vp_stor(CVOPID_PLEASEEROLLAGAIN);
			beep();
			SetTimer_irq(&g_timer[0], TIMER_3SEC, CFIGER_FGPRPRD);
			#else
			ucRgstCnt++;
			ucRet = FP_AddByManager(g_ucUserId, ucRgstCnt);
			if(ucRet != g_ucAddUserId)
			{
				g_ucAddUserId = ucRet;
				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** 初始化定时器 **/
				
				init_fstack(&g_fstack);	
				func.func = f_fingerEnrollFail;
    			stackpush(&g_fstack, &func);
			}
			#endif
		}
		else if(ucRgstCnt == 2)
		{
			/** 注册成功, 修改索引表 **/
			fingerTab_setEntry(g_ucAddUserId, 0xa5);
			flashWrite(g_flash.arrInt);
			
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			
			//bbeep();
			init_fstack(&g_fstack);
			func.func = f_fingerEnrollSuccess;
    		stackpush(&g_fstack, &func);
		}
		
		break;
			
	case CPMT_OVER:
    	promptInit();		/** 停止此消息的发送 **/
		//MOPBlue_on();			/** 闪烁之后恢复背光 **/
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		break;
		
	default:
		break;
	}

	return	0;
}


/*******************************************************************************
 * 指纹管理操作成功
 *******************************************************************************/
int f_fingerEnrollSuccess(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
//	charData_t charData;
//	unsigned char ucResp;
//	unsigned char ucRet;
//	unsigned char ucArr[12];
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT消息 **/
		g_tick = 0;
		MFingerPower_off();			/** 指纹头断电 **/
		
		promptInit();
		vp_stor(CVOPID_EROLL);
		vp_stor(CVOPID_SUCESS);
		bbeep();
		ledPrompt(TIMER_100MS);		/** 成功提示闪烁 **/
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** 超时退出 **/
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CPMT_OVER:
    	promptInit();			/** 提示音结束，停止此消息的发送 **/
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
 * 查指纹中检测到4键，进入删除指纹状态。播放提示音
 *******************************************************************************/
//int f_DegstfingerInit(unsigned *pMsg)
int f_fingerDeleteInit(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
//	charData_t charData;
//	unsigned char ucRet;
//	unsigned char ucArr[12];

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;

		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	FP_INIT();
    	beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_MANAGE);
    	vp_stor(CVOPID_FINGER);
    	vp_stor(CVOPID_PRESSFINGER);
    	beep();							/** 提示及等待1sec **/
    	    	
    	g_subState = 0;
		break;

	case CMSG_FGINIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
		
	case CPMT_OVER:
    	promptInit();			/** 提示音结束，停止此消息的发送 **/
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_fingerDelete;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * Deregister　fingerPrint
 *******************************************************************************/
//int f_Degstfinger(unsigned *pMsg)
int f_fingerDelete(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
//	charData_t charData;
//	unsigned char ucResp;
//	unsigned char ucRet;
//	unsigned char ucArr[12];
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** 超时退出 **/
			init_fstack(&g_fstack);
			func.func = f_fingerDelFail;
    		stackpush(&g_fstack, &func);
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);	
		FP_Query(0, 2);		/** 发送(管理者)指纹比对命令 **/
		break;
		
	case CMSG_FGQUERY:
		/** 权限通过, 删除指纹 **/
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		init_fstack(&g_fstack);
		func.func = f_fingerDeleteDoing;
    	stackpush(&g_fstack, &func);
		break;
	
	case CMSG_FGOPFAIL:
		iSearchCount++;
		if(iSearchCount < 3)
		{
			FP_Query(0, 2);	/** 比对失败，重新发送指纹比对命令 **/
		}
		else
		{
			init_fstack(&g_fstack);
			func.func = f_fingerDelFail;
    		stackpush(&g_fstack, &func);
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
				
	case CPMT_OVER:	
    	promptInit();		/** 停止此消息的发送 **/
		SetTimer_irq(&g_timer[0], TIMER_200MS, CMSG_INIT);
		init_fstack(&g_fstack);
		func.func = func_idle;
    	stackpush(&g_fstack, &func);
		
		break;
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * Deregister　fingerPrint
 *******************************************************************************/
//int f_DegstfingerDoing(unsigned *pMsg)
int f_fingerDeleteDoing(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
	//charData_t charData;
//	unsigned char ucResp;
//	unsigned char ucRet;
//	unsigned char ucArr[12];
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** 超时退出 **/
    		init_fstack(&g_fstack);
			func.func = f_fingerDelFail;
    		stackpush(&g_fstack, &func);
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_DEL);
		g_tick = 0;
		break;

	case CFIGER_DEL:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		FP_DelByManager(g_ucUserId);
		fingerTab_fill(g_ucUserId, 0xff);
		flashWrite(g_flash.arrInt);	
		
		promptInit();
    	beep();	
		vp_stor(CVOPID_COMPARISON);
		vp_stor(CVOPID_PASSED);
    	ledPrompt(TIMER_100MS);
		break;
	
	case CMSG_FGOPFAIL:
		init_fstack(&g_fstack);
		func.func = f_fingerDelFail;
    	stackpush(&g_fstack, &func);
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
			
	case CMSG_FGDERG:
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	
		g_subState = 0;
		init_fstack(&g_fstack);
		func.func = f_fingerDeleteDone;
    	stackpush(&g_fstack, &func);
		break;

	case CPMT_OVER:
		promptInit();		/** 提示音结束，停止此消息的发送 **/
		//MOPBlue_on();			/** 闪烁之后恢复背光 **/
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		break;
	default:
		break;
	}
    
    return  0;
}


/*******************************************************************************
 * Deregister　fingerPrint
 *******************************************************************************/
//int f_DegstfingerDone(unsigned *pMsg)
int f_fingerDeleteDone(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** 超时退出 **/
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CMSG_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		g_tick = 0;
 		g_subState = 0;
 		lbeep();
		vp_stor(CVOPID_DEL);
		vp_stor(CVOPID_SUCESS);
		break;

	case CPMT_OVER:
		//MOPBlue_on();			/** 闪烁之后恢复背光 **/
		MOPBlue_setDefaultStatus();	/** 恢复指示灯与背光一致 **/
		
		g_subState++;
 		if(g_subState == 1)
 		{
 			promptInit();			/** 提示音结束，停止此消息的发送 **/
    		bbeep();
    		ledPrompt(TIMER_100MS);
 		}
 		else if(g_subState == 2)
 		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
    	
		break;
		
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * f_keyFirstTouch
 * 无指纹时的第一次触摸(处理可能的录入指纹功能)
 *******************************************************************************/
int f_keyFirstTouch(unsigned *pMsg)
{
//	msg_t msg;
	func_t func;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		lockAction();
		vp_stor(CVOPID_EROLLPSWORDFORYOURSECURITY);
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
	
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10) {
			init_fstack(&g_fstack);
    		func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
	
	case CPMT_OVER:
		promptInit();
		break;
		
	case CACT_OVER:	/** 如果没任何其它操作，就会从此退出 **/
		actionInit();
		init_fstack(&g_fstack);
    	func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	case CMSG_DKEY:					/** 检测到按键按下 **/
		MBlackLED_on();			/** 背光 **/
		MOPBlue_on();			/** 与背光一起的指示灯 **/
		
		promptInit();
		beep();
		break;
					
	default:
		break;
	}
    return  0;
}


/*******************************************************************************
 * 无指纹模板警告
 *******************************************************************************/
int f_fingerNoFPWarn(unsigned *pMsg)
{
	func_t func;
	   
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10) {
			init_fstack(&g_fstack);
    		func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_INIT:	
		promptInit();
		beep();
		vp_stor(CVOPID_EROLLPSWORDFORYOURSECURITY);
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;

	case CPMT_OVER:
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
/////////////////////////////////////////////////////

