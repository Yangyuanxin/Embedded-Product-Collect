#ifndef __SYS_MEM_H
#define __SYS_MEM_H


extern uint8_t SysMainBuffer_0[4096];       //主缓冲,用于U盘进程和FATFS进程共享

extern uint8_t FrameBuffer[LCM_WIDTH][LCM_HIGH/8];         //显示缓冲, 128X32

void fifo_in(uint8_t *in, uint8_t *bf, uint16_t bf_size, uint8_t bf_n);
void fifo_clr(uint8_t *bf,uint16_t bf_size,uint8_t bf_n);

#endif


