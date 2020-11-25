#ifndef __COLLECTVOLTAGE_H
#define	__COLLECTVOLTAGE_H

#include "sys.h"
#include "delay.h"


void 	Adc_Init(void);
u16  	Get_Adc(u8 ch); 
u16	 	Get_Adc_Average(u8 ch,u8 times); 
 
#endif