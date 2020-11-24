#ifndef __HONG_H__
#define __HONG_H__
#include"sys.h"
#include"delay.h"

#define _gaizi_YOUXIAO 0
#define gaizi_delay() delay_ms(160)

#define GAIZI PBin(7)
void HONG_Init(void);
u8 Gaizi(void);
#endif

