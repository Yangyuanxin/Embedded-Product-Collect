#ifndef _ACPOWERDETECTION_H_
#define _ACPOWERDETECTION_H_


#include "sys.h"

////LED端口定义
//#define LED0 PFout(9)	// DS0
//#define LED1 PFout(10)	// DS1	

#define ACPowerIN()		PAin(4)


extern uint8_t u8_ACPowerStatus;

void ACPowerDetection_GPIO_Init(void);//初始化	
void ACPowerDetection_Process(void);
void ACPowerDetection_Timer_Ms(void);



#endif
