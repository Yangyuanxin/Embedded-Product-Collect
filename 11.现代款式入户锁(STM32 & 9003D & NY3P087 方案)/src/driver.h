#ifndef __DRIVER_H__
#define __DRIVER_H__

#ifdef DUGARCH
int sysProcess(msg_t *val);
#else
int sysProcess(msgType_t *val);
#endif

void actionInit(void);
void promptInit(void);
void actionDoing(actionQueue_t * q);
void actProcess(actionQueue_t * q);
void DAEMON_USARTx_Send(USART_TypeDef* USARTx, charBuf_queue_t* comTxQue);
void keyscan(void);
void fingerCheck(void);
void noKeyEnter(void);
void vop_busy(void);
void arrSort(unsigned int arr[],unsigned char n);
unsigned int arrAverage(unsigned int arr[], unsigned char start, unsigned char end);
int flashWrite(u32 arr[]);

void beep(void);
void bbeep(void);
void bbbeep(void);
void lbeep(void);
//void setSbuf(u8 __sbuf);
void vp_play(u8 __vpIdx);
void vp_stor(unsigned char __vpIdx);
//void vp_stor(unsigned int __vpIdx);

void actionDelay(int _delay);
void promptDelay(int _delay);

#if	0
void Unlock(void);
void Lock(void);
#endif

void lockAction(void);
void ledWarn(int _delay);
void ledPrompt(int _delay);

void wdg_init(void);
void wdg_feed(void);

#endif
