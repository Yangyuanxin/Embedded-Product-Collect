#ifndef __ORDINARY_TIMER_H
#define __ORDINARY_TIMER_H

#include "sys.h"
#include "lora.h"
#include "my_delay.h"
#include "MasterLoRaHandle.h"
#include "hostcomprotocol.h"


void OrdinaryTimerInit(u8 WhichTimer,u16 period, u16 prescaler);

#endif
