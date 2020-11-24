#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
	
#define USART_REC_LEN  			300  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

#define	HEAD_CODE			0x7e
	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

extern __IO uint16_t Rcv_Length;
extern uint8_t CmdCode;
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void USART_Timer_Ms(void);
void USART_Send_Char(uint8_t ch);
void USART_Send_Buf(uint8_t *buf,uint8_t len);
void USART_Send_String(char *s);
#endif


