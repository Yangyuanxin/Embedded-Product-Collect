#include <stdio.h>
#include  "N76E003.h"
#include "Common.h"
#include "SFR_Macro.h"
#include "Function_Define.h" 
#include "adc.h"
uint16_t Bat_Measure(void)
{
	  
	  uint16_t ad_result;
    uint8_t temp;
    ADC4;
    clr_ADCF;
    set_ADCS;	
    while(ADCF == 0);
	  temp=ADCRL&0xf;	  
	  ad_result=ADCRH<<4;
	  ad_result=ad_result+temp;
    return ad_result;
}