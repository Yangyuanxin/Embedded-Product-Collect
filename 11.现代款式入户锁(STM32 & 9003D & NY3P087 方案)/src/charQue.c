#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h"
#include "../src/arch.h"
#include "../main.h"
#include "charQue.h"
/*******************************************************************************
 * check is the queue empty
 *******************************************************************************/
static int isCharQueEmpty(charBuf_queue_t *q)
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
static int isCharQueFull(charBuf_queue_t *q)
{
	if((q->tail + 1 == q->head) || (((q->tail + 1) % CHARQUEUESIZE) == q->head))
	{
		return TRUE;
	}
	return	FALSE;
}

/*******************************************************************************
 * initial the queue
 *******************************************************************************/
void	charQueueInit(charBuf_queue_t *q)
{
	q->tail = 0;
	q->head = 0;
	q->len = 0;
}
#define charBufInit(x) charQueueInit(x)

void	charQueueInit_irq(charBuf_queue_t *q)
{
    IRQ_disable();
    charQueueInit(q);
	IRQ_enable();
}
/*******************************************************************************
 * put the data into the (loop)queue
 *******************************************************************************/
int charQueueIn(charBuf_queue_t *q, charData_t *chardata)
{
    if(isCharQueFull(q))
    {
        return	FALSE;
    }

    q->buf[q->tail] = chardata->ucVal;
	q->tail = (q->tail + 1) % CHARQUEUESIZE;
    
    return TRUE;
}

/*******************************************************************************
 * put the data into the buf()
 *******************************************************************************/
int charBufIn(charBuf_queue_t *q, charData_t *chardata)
{
    if(q->len >= CHARQUEUESIZE)
    {
        return	FALSE;
    }

    q->buf[q->len] = chardata->ucVal;
	q->len += 1;
    
    return TRUE;
}

int charQueueIn_irq(charBuf_queue_t *q, charData_t *chardata)
{
	int iRet;

    IRQ_disable();
    iRet = charQueueIn(q, chardata);
	IRQ_enable();
	    
	return	iRet;
}

int charQueueOut(charBuf_queue_t *q, charData_t *chardata)
{
    if(isCharQueEmpty(q))
    {
        return	FALSE;
    }
    
    chardata->ucVal = q->buf[q->head];
	q->head = (q->head + 1) % CHARQUEUESIZE;
       
    return TRUE;
}

int charQueueOut_irq(charBuf_queue_t *q, charData_t *chardata)
{
    int iRet;

    IRQ_disable();
    iRet = charQueueOut(q, chardata);
	IRQ_enable();
	    
	return	iRet;
}
/////////////////////////////////////////////////////

