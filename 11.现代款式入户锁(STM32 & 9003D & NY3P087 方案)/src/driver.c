
#include "stm32f10x_lib.h"

#include "../inc/CONST.h"
#include "../inc/ctype.h"  
#include "../inc/macro.h"
#include "../inc/global.h"	
#include "../inc/debug.h"

#include "arch.h"	
#include "driver.h"		  
#include "charQue.h"	  
#include "l_fingprint.h"

/*******************************************************************************
 *
 * sysProcess
 * input: data - msg
 * output: none
 * return: TRUE - process ok
 *         FALSE - Can't process
 * descript: 
 *
 *******************************************************************************/
int sysProcess(msg_t *val)
{
    int iRet = TRUE;
    msg_t msg;
    static unsigned int samplingVolt[16] = {0};
    static int	samplingCNT = 0;
    
    unsigned char ucResp;
	unsigned char ucRet;
	unsigned char ucArr[12];
    
    //charData_t charData;
    //int	iADCref = 0;
    //int iADCCurr = 0;
	//int i;
   
    switch(val->msgType)
	{
	case CACT_TOUT:			/** 一段动作完成 **/
		actProcess(&g_actionQueue);
		break;
		
	case CPMT_TOUT:			/** 一段提示动作完成 **/
		actProcess(&g_promptQueue);
		break;
	case CMSG_CALC:
		samplingCNT = 0;
		g_flag &= ~(1<<8);
		break;
		
	case CMSG_ADC:			/** 0.512s的定时器 **/
		if((g_flag & (1<<8)) == 0)		/** 如果没有发生低电报警, 就一直读取数据 **/
		{
			samplingVolt[samplingCNT & 0x0f] = ADC_GetConversionValue(ADC1);
			samplingCNT++;
			
			if((samplingCNT & 0x0f) == 0)		/** 完成16次采样 **/
			{
				g_flag |= (1<<8);		/** 电压采样完成 **/
				arrSort(samplingVolt, 16);
				samplingCNT = 0;
				g_iADCCurr = arrAverage(samplingVolt, 4, 11);	/** the average volt **/
				
				g_iADCref = ADC_getValue();
				if(g_iADCCurr < g_iADCref)
				{
					g_flag |= (1 << 9);		/** 低电报警 **/
				}
			}
		}
		break;
		
	#if	1
	case CMSG_COMRX:	/** 处理串口数据 **/
		ucRet = FP_GetRespData(&g_comRevBuf, ucArr);
		if(ucRet != 0)			/** 收到有效数据 **/
		{
				#if	0
					charData.ucVal = 0xAA;
					charQueueIn_irq(&g_com1TxQue, &charData); 
					for(i = 0; i < 12; i++)
					{
						charData.ucVal = ucArr[i];
						charQueueIn_irq(&g_com1TxQue, &charData);
					} 
                	                    
					charData.ucVal = 0x55;                    
					charQueueIn_irq(&g_com1TxQue, &charData); 
				#endif
				
			ucRet = FP_GetRespType(ucArr);
			switch(ucRet)
			{
			case CFP_INIT:
				ucRet = FP_RespInit(ucArr, ucRet);
				if(ucRet  == 0xaa)	/** 初始化-正常 **/ 
				{
					/** 指纹头初始化-成功 **/
					msg.msgType = CMSG_FGINIT;
					inq_irq(&g_msgq, &msg);
				}
				else if(ucRet  == 0xab)
				{
					/** busy, wait for responsed **/
					
				}
				else
				{
					/** 指纹头初始化失败 **/
					msg.msgType = CMSG_FGOPFAIL;
					inq_irq(&g_msgq, &msg);
				}
				break;
				
			case CFP_ENROLL:
				ucRet = FP_RespRgst(ucArr, &ucResp);
				if(ucRet == 0xaa)
				{
					/** 注册指纹-正常 **/
					msg.msgType = CMSG_FGRGST;
					inq_irq(&g_msgq, &msg);
				}
				else if(ucRet  == 0xab)
				{
					/** busy, wait for responsed **/
				}
				else
				{
					/** 指纹注册-失败 **/
					msg.msgType = CMSG_FGOPFAIL;
					inq_irq(&g_msgq, &msg);
				}
				break;
				
			case CFP_MATCH:
				ucRet = FP_RespSearch(ucArr, &g_ucUserId);
				if(ucRet == 0xaa)
				{
					/** 查找指纹-成功 **/
					msg.msgValue = g_ucUserId;
					msg.msgType = CMSG_FGQUERY;
					inq_irq(&g_msgq, &msg);
				}
				else if(ucRet  == 0xab)
				{
					/** busy, wait for responsed **/
				}
				else
				{
					/** 查找指纹-失败 **/
					msg.msgType = CMSG_FGOPFAIL;
					inq_irq(&g_msgq, &msg);
				}
				break;
				
			case CFP_DELETE:
				ucRet = FP_RespDeRgst(ucArr, &g_ucUserId);
				if(ucRet == 0xaa)
				{
					/** 删除指纹-成功 **/
					msg.msgType = CMSG_FGDERG;
					inq_irq(&g_msgq, &msg);
				}
				else if(ucRet  == 0xab)
				{
					/** busy, wait for responsed **/
				}
				else
				{
					/** 删除指纹-失败 **/
					msg.msgType = CMSG_FGOPFAIL;
					inq_irq(&g_msgq, &msg);
				}
				break;
							
			default:
				break;
			}
		}
		break;
	#endif
		
	default:
		/** Can't process the msg here **/
		iRet = FALSE;

		break;
	}
	
	return  iRet;
}

/*******************************************************************************
 * DAEMON_USARTx_Send()
 *******************************************************************************/
void DAEMON_USARTx_Send(USART_TypeDef* USARTx, charBuf_queue_t* comTxQue)
{
    charData_t   comData;
    //static unsigned char iTmp;

    if(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == SET)
    {
        if(charQueueOut_irq(comTxQue, &comData) == TRUE)
        {
            USART_SendData(USARTx, comData.ucVal);
        }
    }
}

/*******************************************************************************
 * keyscan()
 * PA.5 --- INT(数字按键)
 * PC.5 --- TO()
 * PA.7 --  SET(设置键)
 *******************************************************************************/
#define CKEYINIT		(0xff)
const unsigned char keyMap[] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f
};

void keyscan(void)
{
	static unsigned char key_old = CKEYINIT; 
	static unsigned char key = CKEYINIT;
//	charData_t charData;
	static unsigned char ucKey = CKEYINIT;
	msg_t msg;
//	int i;
	
	/** 检测按键状态的变化 **/
	if((g_flag & (1 << 7)) == 0) {	/** 以前没有触摸过数字按键 **/
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 1) {	/** 按下按键 **/
			if((g_flag & (1 << 14)) == 0) {		/** 还没检测到按下按键 **/
				msg.msgType = CMSG_BACK;	//!!!!!!!!!!!!!!!!!
				inq_irq(&g_msgq, &msg);		//!!!!!!!!!!!!!!!!!
			}
		} else {	/** 松开按键 **/
			if((g_flag & (1 << 14)) != 0) {		/** 还没检测到松开按键 **/
				g_flag |= (1 << 7);
			}
		}	
	} else {
		if(key != key_old) {					/** 按键动作 **/
			msg.msgValue = keyMap[ucKey];	/** 重新映射键值 **/
			
			if(key != CKEYINIT) {			/** 按下按键 **/
				if(g_tmr_key == TIMER_80MS) {
					msg.msgType = CMSG_DKEY;
					inq_irq(&g_msgq, &msg);
				
					g_tmr_key = TIMER_100MS;
					key_old = key;
				}
			} else {					/** 松开按键 **/
				key_old = key;		
				//g_tmr_key = TIMER_200MS;
				
				msg.msgType = CMSG_UKEY;
				inq_irq(&g_msgq, &msg);
			}
		}
	}
	
	/** 取数字按键当前状态及键值PC(3..0) **/
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 1) {	/** key pressed(高有效) **/
		if((g_flag & (1 << 14)) != 0) {		/** 状态是持续的吗? **/
			if(g_tmr_key == TIMER_60MS) {	/** 已持续60ms吗? **/
				g_tmr_key = TIMER_80MS;
				
				key = (GPIO_ReadInputData(GPIOC) & 0x0f);
				ucKey = key;
				
			}
		} else {
			g_flag |= (1 << 14);
			g_tmr_key = 0;		/** 开始计时 **/
		}
	} else {	/** key released **/
		if((g_flag & (1 << 14)) == 0) {		/** 状态是持续的吗? **/
			if(g_tmr_key == TIMER_60MS) {	/** 已持续60ms吗? **/
				g_tmr_key = TIMER_80MS;
				key = CKEYINIT;
			}
		} else {
			g_flag &= ~(1 << 14);
			g_tmr_key = 0;		/** 开始计时 **/
		}
	}
	
	/** 取室内出门按键TO当前状态 **/
	//if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 0)	/** key pressed(低有效) **/
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 1)	/** key pressed(高有效) **/
	{
		if((g_flag & (1 << 4)) == 0)
		{
			//msg.msgValue = CTOKEY;
			msg.msgValue = CKEY_GOOUT;
			msg.msgType = CMSG_DKEY;
			inq_irq(&g_msgq, &msg);
			
			#if	0
			g_keys.iCount++;
			for(i = CKEYBUFSIZE - 1; i >= 1; i--)		/** 移位，腾出空位 **/
			{
				g_keys.buf[i] = g_keys.buf[i - 1];
			}
			g_keys.buf[0] = keyMap[ucKey];	/** 新键值 **/
			#endif
		}
		g_flag |= (1 << 4);
	}
	else
	{
		g_flag &= ~(1 << 4);
	}
	/** 取室外进门按键TI当前状态 **/
	//if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == 0)	/** key pressed(低有效) **/
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == 1)	/** key pressed(高有效) **/
	{
		if((g_flag & (1 << 5)) == 0)
		{
			//msg.msgValue = CTIKEY;
			msg.msgValue = CKEY_GOIN;
			msg.msgType = CMSG_DKEY;
			inq_irq(&g_msgq, &msg);
		}
		g_flag |= (1 << 5);
	}
	else
	{
		g_flag &= ~(1 << 5);
	}
	
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1)	/** 设置键 **/
	{
		if((g_flag & (1 << 12)) == 0)
		{
			msg.msgValue = CKEY_SET;
			msg.msgType = CMSG_DKEY;
			inq_irq(&g_msgq, &msg);
		}
		g_flag |= (1 << 12);
	}
	else
	{
		g_flag &= ~(1 << 12);
	}
}

/*******************************************************************************
 * fingerOn()
 *******************************************************************************/
void fingerCheck(void)
{
	msg_t msg;
	/** 按下指纹头 **/
	//if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)	/** finger pressed(低有效) **/
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 1)	/** finger pressed(高有效) **/
	{
		if((g_flag & (1 << 6)) == 0)
		{
			msg.msgType = CFIGER_ON;
			inq_irq(&g_msgq, &msg);
			
			//MFingerPower_on();			/** 给指纹头供电。不适合在此打开 **/
		}
		g_flag |= (1 << 6);
	}
	else
	{
		g_flag &= ~(1 << 6);
	}
}


/*******************************************************************************
 * noKeyEnter()无钥匙进入
 *******************************************************************************/
void noKeyEnter(void)
{
	msg_t msg;
	/** 查无钥匙开锁信号 **/
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == 1)	/** 无钥匙进入(高有效) **/
	//if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == 0)	/** 无钥匙进入(低有效) **/
	{
		if((g_flag & (1 << 11)) == 0)
		{
			msg.msgType = CNOKEY_ENTER;
			inq_irq(&g_msgq, &msg);
		}
		g_flag |= (1 << 11);
	}
	else
	{
		g_flag &= ~(1 << 11);
	}
}

/*******************************************************************************
 * vop_busy()语音播放
 *	(CVOP_WT588D --- 一个由低到高的跳变，表示播放结束)
 *	(CVOP_GQPP5 --- 一个由高到低的跳变，表示播放结束)
 *	(CVOP_NY3P --- 一个由高(检到2次)到低(检到200次)的跳变，表示播放结束)
 *******************************************************************************/
void vop_busy(void)
{
	#define	CMAXH	2
	#define	CMAXL	200
	msg_t msg;
	static int iCount = 0;
	/* bit0: L
	 * bit1: H
	 * bit2: first L -> H 
	 * bit3: first H -> L
	 */
	static int iVopBusyFlag = 0;
	/** 查语音busy信号 **/
	if(g_tmr_iVopBusy >= TIMER_VOPBUSY/** delay for busy check**/) 
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)		/** MSK3P/NY3P语音busy(H->L有效) **/
		//if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)		/** WT588D语音busy(L->H有效) **/
		{
			iVopBusyFlag |= (1 << 0);
			if((g_flag & (1 << 13)) == 0)
			{
				iCount++;
				if(iCount >= CMAXL)
				{
					msg.msgType = CPMT_TOUT;
					inq_irq(&g_msgq, &msg);
				
					g_flag |= (1 << 13);
					iCount = 0;
				}
			}
			else
			{
				
			}
		}
		else
		{
			iVopBusyFlag |= (1 << 1);
			if((g_flag & (1 << 13)) != 0)
			{
				iCount++;
				if(iCount >= CMAXH)
				{
					g_flag &= ~(1 << 13);
					iCount = 0;
				}
			}
		}
	}
}

/*******************************************************************************
 * 动作处理: 启动新的动作
 *******************************************************************************/
static void startAction(actionQueue_t *q, action_t * pAction)
{
	SetTimer_irq(q->timer, pAction->actionTime, q->stepMsgType);
	
	switch(pAction->actionType)
	{
	case	CACT_UNLOCK:
		q->flag = (1<<7);
		MUnLock();
		break;
		
	case	CACT_LOCK:
		q->flag = (1<<7);
		MLock();
		break;
		
	case	CACT_LEDPMTON:
		q->flag = (1<<6);	  
		MOPBlue_on();
		break;
		
	case	CACT_LEDPMTOFF:
		q->flag = (1<<6);
		MOPBlue_off();
		break;
		
	case	CACT_LEDWRNON:
		q->flag = (1<<5);
		MOPRed_on();
		break;
		
	case	CACT_LEDWRNOFF:
		q->flag = (1<<5);
		MOPRed_off();
		break;
		
	case	CACT_BEEPON:
		q->flag = (1<<4);
		VOPDATA_H();
		break;
		
	case	CACT_BEEPOFF:
		q->flag = (1<<4);
		VOPDATA_L();
		break;
		
	case	CACT_DELAY:
		q->flag = (1<<3);
		break;
	
	case	CACT_VOPON:
		q->flag = (1<<2);
		vp_play((unsigned char)pAction->actionPara);
		break;
			
	default:
		/** 不可识别的动作 **/
		q->flag = 0;
		break;
	}
}

 /*******************************************************************************
 * 动作处理：停止当前的动作
 *******************************************************************************/
static void stopAction(actionQueue_t *q)
{
	ClrTimer_irq(q->timer);
		/** stop the timeout action  **/
	if(q->flag & (1<<7)) {
		MLockPWR_off();
	} else if(q->flag & (1<<6)) {
		#if	1
			MOPBlue_off();
		#else
			if(MBlackLED_status()/** 如果背光亮，完成指纹示后，蓝色指示灯亮 **/) {
				MOPBlue_on();
			} else {			 /** 如果背光灭...... **/
				MOPBlue_off();
			}
		#endif
	} else if(q->flag & (1<<5)) {
		MOPRed_off();
		#if	0
			if(MBlackLED_status()/** 背光亮 **/) {
				MOPBlue_on();
			} else {			 /** 背光灭 **/
				MOPBlue_off();
			}
		#endif
	} else if(q->flag & (1<<4)) {
		VOPDATA_L();
	} else if(q->flag & (1<<3)) {
		/** delay... do Nothing **/
	} else if(q->flag & (1<<2)) {
		/** stop VP, send stop command when playing **/
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)vp_play((unsigned char)CVOP_STOP);
	} else {
	}
}
 /** 清空动作队列，并且停止当前动作 **/
void promptInit(void)
{
	actionQueueInit(&g_promptQueue, &(g_timer[2]), CPMT_TOUT, CPMT_OVER);
	stopAction(&g_promptQueue);
}

 /** 清空动作队列，并且停止当前动作 **/
void actionInit(void)
{
	actionQueueInit(&g_actionQueue, &(g_timer[1]), CACT_TOUT, CACT_OVER);
	stopAction(&g_actionQueue);
}
/** 事件队列处理 **/
void actionDoing(actionQueue_t * q)
{
	action_t action;
	
	if(q->flag == 0)
	{
		if(actionQueueOut(q, &action) == TRUE)
		{
			startAction(q, &action);
		}
	}
}

/** 系统当前事件结束 **/
void actProcess(actionQueue_t * q)
{
	action_t action;
	
	/** stop the timeout action first  **/
	stopAction(q);
	
	/** start the next action **/
	if(actionQueueOut(q, &action) == TRUE)
	{
		startAction(q, &action);
	}
	else
	{
		if(q->flag != 0)
		{
			q->flag = 0;
			SetTimer_irq(q->timer, TIMER_100MS, q->overMsgType);
		}
	}
}

void promptDelay(int _delay)
{
	action_t action;
    action.actionType = CACT_DELAY;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
}

void lbeep(void)
{
	action_t action;
	#if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_LBEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_1SEC;
	#endif
    actionQueueIn(&g_promptQueue, &action);
}

void beep(void)
{
	action_t action;
	#if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
    
    #if	CENABLEVOP
    #else
    action.actionType = CACT_DELAY;			/** 附加一个延时 **/
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_promptQueue, &action);
    #endif
}

void bbeep(void)
{
	action_t action;
	#if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
    
	#if	CENABLEVOP
	#else
    action.actionType = CACT_DELAY;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_promptQueue, &action);
    #endif
    
    #if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
    #endif
    actionQueueIn(&g_promptQueue, &action);
}

void bbbeep(void)
{
	action_t action;
	
    #if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
    
	#if	CENABLEVOP
	#else
    action.actionType = CACT_DELAY;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_promptQueue, &action);
    #endif
    
    #if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
    
	#if	CENABLEVOP
	#else
    action.actionType = CACT_DELAY;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_promptQueue, &action);
    #endif
    
    #if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
}

void vp_stor(unsigned char __vpIdx)
{
	action_t action;
	#if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = __vpIdx;
    #else
	action.actionType = CACT_DELAY;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
}

volatile u8 g_tmr_sbuf;
//void setSbuf(u8 __sbuf)
void vp_play(u8 __vpIdx)
{
    #if	CENABLEVOP
    	IRQ_disable();
    	g_tmr_sbuf = __vpIdx;
   		g_tmr_vop = 0;
    	IRQ_enable();
    #else
    #endif
}

/** 指示灯动作提示 **/
void ledPrompt(int _delay/** 周期 **/)
{
	action_t action;
    action.actionType = CACT_LEDPMTON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
}

/** 指示灯警告提示 **/
void ledWarn(int _delay)	/** 警告提示 **/
{
	action_t action;
    action.actionType = CACT_LEDWRNON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
}

#if	0
void Unlock(void)
{
	action_t action;
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_300MS;
    actionQueueIn(&g_actionQueue, &action);
}

void Lock(void)
{
	action_t action;
	action.actionType = CACT_LOCK;
    action.actionTime = TIMER_300MS;
    actionQueueIn(&g_actionQueue, &action);
}
#endif

void lockAction(void)
{
	action_t action;
	/** 正转过程 **/
	#if	1
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_500MS;
    actionQueueIn(&g_actionQueue, &action);
    #else
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_100MS;
    actionQueueIn(&g_actionQueue, &action);
    action.actionType = CACT_LOCK;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_actionQueue, &action);
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_300MS;
    actionQueueIn(&g_actionQueue, &action);
    #endif
    
    /** 停顿 **/
    action.actionType = CACT_DELAY;
    action.actionTime = TIMER_5SEC;
    actionQueueIn(&g_actionQueue, &action);
    
    #if	1
    action.actionType = CACT_LOCK;
    action.actionTime = TIMER_500MS;
    actionQueueIn(&g_actionQueue, &action);
    #else
    /** 反转过程 **/
    action.actionType = CACT_LOCK;
    action.actionTime = TIMER_100MS;
    actionQueueIn(&g_actionQueue, &action);
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_actionQueue, &action);
    action.actionType = CACT_LOCK;
    action.actionTime = TIMER_300MS;
    actionQueueIn(&g_actionQueue, &action);
    #endif
}

void actionDelay(int _delay)
{
	action_t action;
    action.actionType = CACT_DELAY;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
}


/*****************************************************************************
 * function: arrSort
 * Description: array sort
 * input: 	arr - array pointer
 			n - the number of element
 * output: no
 * return: no
****************************************************************************/
void arrSort(unsigned int arr[],unsigned char n)                         //数组排序
{
	unsigned char m,k,j,i;
	unsigned int d;
	k=0;
	m=n-1;
	while(k<m)
	{
		j=m-1;
		m=0;
		for(i=k;i<=j;i++)
		{
			if(arr[i]>arr[i+1])
			{
				d=arr[i];
				arr[i]=arr[i+1];
				arr[i+1]=d;
				m=i;
			}
		}
		j=k+1;
		k=0;
		for(i=m;i>=j;i--)
		{
			if(arr[i-1]>arr[i])
			{
				d=arr[i];
				arr[i]=arr[i-1];
				arr[i-1]=d;
				k=i;
			}
		}
	}
}
/*******************************************************************************
 * function: arrAverage
 * Description: calculate array average
 * input: 	arr - array pointer
 *			start - the start index of element
 *			end -   the start index of element
 * output: no
 * return: average
********************************************************************************/
unsigned int arrAverage(unsigned int arr[], unsigned char start, unsigned char end)       //数组平均值
{
	int	i;
	unsigned int sum = 0;
	if(start > end)
	{
		return	0xffff;		/** error **/
	}
	for(i = start; i < end; i++)
	{
		sum += arr[i];
	}
	
	return	sum /(end - start +1);
}
/*******************************************************************************
数据存贮规划如下：
 * 0..3 - MAGICCODE(暂用HUAR的ASCII码作为识别)
 * 4..7 - ADC(低电)
 * 8..11 - 
 * 12..29 - reserved
 * 30..39 - password 1(第m个密码的第n位所在位置:  (m+2)*10 + n)
 * 40..49 - password 2
 * 50..59 - password 3
 * 60..69 - password 4
 * 70..79 - password 5
 * 80..89 - password 6
 * 90..99 - reserved (作隔离带)
 * 100..500 - 指纹模板索引(第m个指纹的索引标志所在位置:  m + 100)
 *
 *******************************************************************************/

/*******************************************************************************
 * function: flashWrite
 * Description: write 1KB data into a page
 * input: 	arr - the data array pointer
 * output: no
 * return: average
 * 
 * Note: flash Page size FLASH_PAGE_SIZE=1KBytes
********************************************************************************/
int flashWrite(u32 arr[])
{
	u32 FlashAddress;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	
	/* Unlock the Flash Program Erase controller */
    FLASH_Unlock();   
    /* Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	//清标志位
    
    /* Erases the specified Flash Page */
    //FLASHStatus = FLASH_ErasePage(CFLASHSTARTADDR);
    FLASHStatus = FLASH_ErasePage(CFLASHSTARTADDR); 	//擦除这个扇区
	if(FLASHStatus != FLASH_COMPLETE)
	{
		return -1;
	}
    
    /** Writes the Data at the Address **/
    FlashAddress = CFLASHSTARTADDR;
    while(FlashAddress < CFLASHSTARTADDR + CFLASH_PAGE_SIZE)
    {
    	//status = FLASH_ProgramWord(FlashAddress, arr[FlashAddress - CFLASHSTARTADDR]); 
    	FLASH_ProgramWord(FlashAddress, arr[(FlashAddress - CFLASHSTARTADDR) >> 2]);
    	
    	FlashAddress += 4;
	}
	
	FLASH_Lock();
	
	return	0;
}

/*******************************************************************************
 * function: wdg_init
 * Description: 
 * input: no
 * output: no
 * return: no
********************************************************************************/
void wdg_init(void)
{

    /* IWDG timeout equal to 350ms (the timeout may varies due to LSI frequency
    dispersion) -------------------------------------------------------------*/
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: 32KHz(LSI) / 32 = 1KHz */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to 349 */
    IWDG_SetReload(349);
    //IWDG_SetReload(1999);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}

void wdg_feed(void)
{
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
}


////////////////////////////////////////////////////////////
