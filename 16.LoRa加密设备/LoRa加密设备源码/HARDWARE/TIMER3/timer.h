#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"
#include "lora.h"
#include "MasterLoRaHandle.h"

extern u16 Timer3_Conuter;


void TIM3_Int_Init(u16 arr,u16 psc);
 
#endif
