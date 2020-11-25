
#include "stm32f10x_lib.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h"
#include "../inc/global.h"	
#include "../inc/debug.h"
#include "src/charQue.h"
#include "../main.h"
#include "arch.h"



/*******************************************************************************
 * IRQ_disable()/IRQ_enable()
 *******************************************************************************/
void IRQ_disable(void)
{
    #if 0
        NVIC_SETPRIMASK();
        //NVIC_SETFAULTMASK();
    #else
        //CAN_ITConfig(CAN_IT_FMP0, DISABLE);
        //CAN_ITConfig(CAN_IT_TME, DISABLE);
        TIM_ITConfig(TIM2, TIM_IT_CC1/** | TIM_IT_CC4 **/, DISABLE);
        //USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    #endif
}

void IRQ_enable(void)
{
    #if 0
        NVIC_RESETPRIMASK();
        //NVIC_RESETFAULTMASK();
    #else
        //CAN_ITConfig(CAN_IT_FMP0, ENABLE);
        //CAN_ITConfig(CAN_IT_TME, ENABLE);
        TIM_ITConfig(TIM2, TIM_IT_CC1/** | TIM_IT_CC4 **/, ENABLE);
        //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    #endif
}


/*******************************************************************************
 *
 * function stack operation
 *
 *******************************************************************************/
void init_fstack(fstack_t *s)
//void fstack_init(fstack_t *s)
{
	s->top = 0;
}

int	stackpop(fstack_t *s)
{
	if((s->top <= 0) || (s->top >= STACKSIZE))    /** make sure ... top [1..STACKSIZE - 1]  **/
	{
		return	FALSE;
	}
	
	s->top--;
	return	TRUE;
}

int	stackpush(fstack_t *s, func_t *func)   /** make sure ... top [0..STACKSIZE-2]  **/
{
	if((s->top < 0) || (s->top >= STACKSIZE - 1))
	{
		/**
			s->func[s->top].func = func->func;
			s->func[s->top].arg = func->arg;
		**/
		return	FALSE;
	}
	s->top++;
	s->func[s->top].func = func->func;
	s->func[s->top].arg = func->arg;
	
	return	TRUE;
}

int stackupdate(fstack_t *s, func_t *func)
{
	/** Same As: init_fstack()/stackpush() **/
	
	s->top = 1;
	s->func[s->top].func = func->func;
	s->func[s->top].arg = func->arg;
	
	return	TRUE;
}
/**
 * get the data at the top of the stack
 *
 **/
int	stacktop(const fstack_t *s, func_t *f)   /** make sure ... top [1..STACKSIZE - 1]  **/
{
	if((s->top <= 0) || (s->top >= STACKSIZE))
	{
		return	FALSE;
	}
	f->func = s->func[s->top].func;
	f->arg = s->func[s->top].arg;

	return	TRUE;
}
/*******************************************************************************
 *
 * msg queue operation
 *
 *******************************************************************************/
int msg_init(msg_t *pMsg)
{
	if((pMsg != 0))
	{
		pMsg->msgType = CMSG_NONE;
		return	TRUE;
	}
	else
	{
		return	FALSE;
	}
}

void init_queue(msgq_t *q)
{
	q->tail = q->head = 0;
}

static int isempty(msgq_t *q)
{
	if(q->tail == q->head)
	{
		return TRUE;
	}
	return	FALSE;
}

static int isfull(msgq_t *q)
{
	if((q->tail + 1 == q->head) || (((q->tail + 1) % QUEUESIZE) == (q->head)))
	{
		return TRUE;
	}
	return	FALSE;
}

int	inq(msgq_t *q, msg_t *val)		    //Note: check queue full is necessary before invoke this routine
{
    if(isfull(q))
    {
        return  FALSE;
    }
    
	//q->buf[q->tail] = val;
	//q->type[q->tail] = val;	
	
	q->msg[q->tail].msgType = val->msgType;	
	q->msg[q->tail].msgValue = val->msgValue;
	
	q->tail = (q->tail + 1) % QUEUESIZE;

	return TRUE;
}

int	inq_irq(msgq_t *q, msg_t *val)		//Note: check queue full is necessary before invoke this routine
{
    int iRet;
    
    IRQ_disable();
    iRet = inq(q, val);
    IRQ_enable();

	return iRet;
}

int	outq(msgq_t *q, msg_t *val)		//Note: check queue empty is necessary before invoke this routine
{
    if(isempty(q))
    {
        return  FALSE;
    }
    
	val->msgType = q->msg[q->head].msgType;
	val->msgValue = q->msg[q->head].msgValue;
	q->head = (q->head + 1) % QUEUESIZE;
	    
	return	TRUE;
}

int	outq_irq(msgq_t *q, msg_t *val)		//Note: check queue empty is necessary before invoke this routine
{
    int iRet;
    
    IRQ_disable();
    iRet = outq(q, val);
	IRQ_enable();
	    
	return	iRet;
}

/*******************************************************************************
 * Timer operation
 *******************************************************************************/
void SetTimer(Timer_t * timer, int tick, msgType_t msgType)
{
    timer->tick = tick;
    timer->tick_bak = tick;
    timer->msgType = msgType;
}
void SetTimer_irq(Timer_t * timer, int tick, msgType_t msgType)
{
    IRQ_disable();
    SetTimer(timer, tick, msgType);
    IRQ_enable();
}

void ClrTimer(Timer_t * timer)
{
    timer->tick = 0;
    timer->tick_bak = 0;
}
void ClrTimer_irq(Timer_t * timer)
{
    IRQ_disable();
    ClrTimer(timer);
    IRQ_enable();
}
/*******************************************************************************
 * delay() operation
 * 
 * wait until timer out
 *******************************************************************************/
int g_tmr_delay;

void delay(int TickCount)
{
    g_tmr_delay = TickCount;
    
    while(g_tmr_delay);
}

void setDelayTimer(u8 TimerNum, int TickCount)
{
    IRQ_disable();
    if(TimerNum == 0)
    {
        g_tmr_delay = TickCount;
    }
    else if(TimerNum == 1)
    {
    	g_tmr_vop = TickCount;
    }
    IRQ_enable();
}

int getDelayTimer(u8 TimerNum)
{
    if(TimerNum == 0) 
    {
        return g_tmr_delay;
    }
    else if(TimerNum == 1)
    {
    	return g_tmr_vop;
    }
    else 
    {
        return	0;
    }
}

/*******************************************************************************
 * check is the queue empty
 *******************************************************************************/
static int isActionQueEmpty(actionQueue_t *q)
{
	if(q->tail == q->head)
	{
		return TRUE;
	}
	return	FALSE;
}

/*******************************************************************************
 * check is the queue full
 *******************************************************************************/
static int isActionQueFull(actionQueue_t *q)
{
	if((q->tail + 1 == q->head) || (((q->tail + 1) % ACTIONQUEUESIZE) == q->head))
	{
		return TRUE;
	}
	return	FALSE;
}

/*******************************************************************************
 * put the data into the (loop)queue
 *******************************************************************************/
//void actionQueueInit(actionQueue_t *q)
//void actionQueueInit(actionQueue_t *q, Timer_t *timer, msgType_t msgType) 
void actionQueueInit(actionQueue_t *q, Timer_t *timer, msgType_t stepMsgType, msgType_t overMsgType)
{
	q->tail = q->head = 0;

//	q->msgType = msgType;	
	q->stepMsgType = stepMsgType;  
	q->overMsgType = overMsgType;
	q->timer = timer;
	ClrTimer(timer);
}

/*******************************************************************************
 * put the data into the (loop)queue
 *******************************************************************************/
int actionQueueIn(actionQueue_t *q, action_t *action)
{
    if(isActionQueFull(q))
    {
        return	FALSE;
    }

    q->buf[q->tail].actionType = action->actionType;
    q->buf[q->tail].actionTime = action->actionTime;
    q->buf[q->tail].actionPara = action->actionPara;
	q->tail = (q->tail + 1) % ACTIONQUEUESIZE;
    
    return TRUE;
}

int actionQueueOut(actionQueue_t *q, action_t *action)
{
    if(isActionQueEmpty(q))
    {
        return	FALSE;
    }
    
    action->actionType = q->buf[q->head].actionType;
    action->actionTime = q->buf[q->head].actionTime;
    action->actionPara = q->buf[q->head].actionPara;
    
	q->head = (q->head + 1) % CHARQUEUESIZE;
       
    return TRUE;
}

/////////////////////////////////////////////////////
