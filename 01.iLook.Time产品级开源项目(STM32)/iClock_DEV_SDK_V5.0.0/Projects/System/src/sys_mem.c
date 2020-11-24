#include "hw_config.h"
#include "sys_define.h"

uint8_t SysMainBuffer_0[4096];       //主缓冲,用于U盘进程和FATFS进程共享

uint8_t FrameBuffer[LCM_WIDTH][LCM_HIGH/8];         //显示缓冲, 128X32

void fifo_in(uint8_t *in, uint8_t *bf, uint16_t bf_size, uint8_t bf_n)
{
  uint8_t i,j,*p1,*p2;
    
  for(i=0; i<(bf_size-1); i++){
    p1 = bf+(bf_size-i-1)*bf_n;
    p2 = p1-bf_n;
    for(j=0;j<bf_n;j++){
     *p1++ = *p2++;
    }
  }
  //
  for(j=0; j<bf_n; j++){
    *bf++ = *in++;
  }
}

void fifo_clr(uint8_t *bf,uint16_t bf_size,uint8_t bf_n)
{
  uint16_t i;
  
  for(i=0; i<bf_size; i++){
    *bf++ = 0;
  }
}



