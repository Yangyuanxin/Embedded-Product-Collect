#ifndef __ARCH_H__
#define __ARCH_H__

void IRQ_disable(void);
void IRQ_enable(void);

void	init_fstack(fstack_t *s);
int	stackpop(fstack_t *s);
int	stackpush(fstack_t *s, func_t *func);
int stackupdate(fstack_t *s, func_t *func);
int	stacktop(const fstack_t *s, func_t *f);
void	init_queue(msgq_t *q);

int msg_init(msg_t *pMsg);
int	inq(msgq_t *q, msg_t *val);
int	outq(msgq_t *q, msg_t *val);
int	inq_irq(msgq_t *q, msg_t *val);
int	outq_irq(msgq_t *q, msg_t *val);

void SetTimer(Timer_t * timer, int tick, msgType_t msgType); 
void SetTimer_irq(Timer_t * timer, int tick, msgType_t msgType);



void ClrTimer(Timer_t * timer);
void ClrTimer_irq(Timer_t * timer);
void delay(int __ms);

//void actionQueueInit(actionQueue_t *q);
//void actionQueueInit(actionQueue_t *q, Timer_t *timer, msgType_t msgType);
void actionQueueInit(actionQueue_t *q, Timer_t *timer, msgType_t stepMsgType, msgType_t overMsgType);
int actionQueueIn(actionQueue_t *q, action_t *action);
int actionQueueOut(actionQueue_t *q, action_t *action);

#endif
