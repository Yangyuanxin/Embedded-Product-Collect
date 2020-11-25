#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"
#include "delay.h"


//蜂鸣器端口定义
#define BEEP PEout(2)	// BEEP,蜂鸣器接口		   

void BEEP_Init(void);	//初始化
void BEEP_Ring(uint16_t time);


#endif //__BEEP_H

