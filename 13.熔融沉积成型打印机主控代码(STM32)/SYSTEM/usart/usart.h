#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"  //要用到printf函数，因此要包含C语言的标准输入输出文件



#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
#define RX_BUFFER_SIZE 128  //定义接收缓冲区的大小

typedef struct ring_buffer
{
  unsigned char buffer[RX_BUFFER_SIZE];
  int head;
  int tail;
}ring_buffer;

extern  ring_buffer rx_buffer;	 
extern uint16_t USART_RX_STA;         		//接收状态标记	


void uart1_init(u32 bound);
void checkRx(void);
unsigned int MYSERIAL_available(void);
uint8_t MYSERIAL_read(void);
void MYSERIAL_flush(void);


#endif //__USART_H
