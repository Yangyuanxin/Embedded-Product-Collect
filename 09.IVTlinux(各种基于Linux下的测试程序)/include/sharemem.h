#ifndef _SHARE_MEM_H_
#define _SHARE_MEM_H_

#include <stdint.h>
int frameShareMemInit(size_t size,size_t count);
void frameShareMemWrite(int index, uint8_t *yuv_frame, size_t yuv_length);

#endif

