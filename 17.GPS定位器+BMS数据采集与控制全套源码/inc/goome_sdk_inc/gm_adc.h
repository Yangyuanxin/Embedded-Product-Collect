

#ifndef __GM_ADC_H__
#define __GM_ADC_H__

extern U32 GM_ReadAdc0(void);
extern U32 GM_ReadAdc1(void);
extern U32 GM_AdcRead(U8 adc_channel, U32 *adc_read_data);

#endif

