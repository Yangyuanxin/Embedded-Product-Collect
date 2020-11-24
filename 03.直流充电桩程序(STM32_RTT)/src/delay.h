#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f10x.h"

void delay_reset(u32 *delay_base);
u32 delay_timeout(u32 *delay_base, u32 ms);
u32 delay_timeout_reload(u32 *delay_base, u32 ms);

#endif
