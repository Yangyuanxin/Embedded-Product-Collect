#ifndef __GLOBAL_H__
#define __GLOBAL_H__
/*******************************************************************************/
extern u8 g_ucCanseq;
extern u8 g_destAddr;
extern int g_channel;

extern int g_tmr_delay;
extern int g_tmr_vop;
extern int g_tmr_key;
extern int g_tmr_iVopBusy;
extern int g_tmr_iVopFault;
extern volatile u8 g_tmr_sbuf;

extern fstack_t g_fstack;
extern msgq_t  g_msgq;
extern Timer_t g_timer[TIMER_NUM];
extern bitmap_t bmpRcvCan;
extern func_t g_next_func;

//extern canFrame_queue_t g_canTxQue; /** canTx from comRx**/
extern charBuf_queue_t g_comRevBuf; /** comRx ==> canTxQue **/

extern charBuf_queue_t g_com1TxQue;
extern charBuf_queue_t g_com2TxQue;

//extern u32 g_flashPageAddr; 	/** 可读写的flash空间的起始地址 **/
extern u8 g_ucKey;

extern unsigned char g_ucUserId;
extern unsigned char g_ucAddUserId;		/** 添加的id号 **/
extern int g_tick;		/** 嘀嗒 **/
extern unsigned int g_TIM2_tick;
extern u32 g_flag;

extern int	g_iADCref;
extern int g_iADCCurr;

extern unsigned int g_subState;
extern actionQueue_t g_actionQueue;
extern actionQueue_t g_promptQueue;
//extern unsigned char g_oldKey[10];
//extern key_t g_keys;

extern const char g_magic[MAGIC_SIZE];
//extern unsigned char g_flash[1024];
extern flashPage_t g_flash;
/*******************************************************************************/
#endif
