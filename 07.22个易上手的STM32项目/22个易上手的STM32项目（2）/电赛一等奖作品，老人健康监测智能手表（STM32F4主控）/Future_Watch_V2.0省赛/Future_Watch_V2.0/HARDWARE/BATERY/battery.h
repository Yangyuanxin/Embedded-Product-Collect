#ifndef _ADC_H_
#define _ADC_H_

#include "sys.h"
#include "delay.h"


void  Adc_Init(void);
u16 Get_Adc(u8 ch) ;
u16 Get_Adc_Average(u16 adc[],u8 times);
void  Adc3_Init(void);
u16 Get_Adc3(u8 ch) ;
#endif
