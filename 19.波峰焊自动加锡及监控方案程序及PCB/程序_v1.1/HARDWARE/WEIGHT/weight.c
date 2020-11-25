#include "adc.h"
#include "weight.h"
#include "usart.h"
#include "delay.h"


float w;
void weigh()
{
    char iVol_Buf[128];
	char iWei_Buf[128];
	char iStr[1024];
	u16 adcx=0;
	float temp,k;
	adcx=Get_Adc_Average(ADC_Channel_1,10);
	temp=(float)adcx*(3.3/4096);
	k=temp;
	w=k/0.33;
	Str_DoubleToStr(iWei_Buf,w,2);
	Str_DoubleToStr(iVol_Buf,k,2);
	sprintf(iStr,"ADC=%d,Vol=%s,Weight:%s\r\n",adcx,iVol_Buf,iWei_Buf);
	usart3_printf(iStr);
	temp-=adcx;
	temp*=1000;
}
