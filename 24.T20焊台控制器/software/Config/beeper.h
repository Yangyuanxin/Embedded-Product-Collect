#ifndef __BEEPER_H
#define __BEEPER_H	 
#include "sys.h"

#define Beeper PBout(9)	//PB9

void BEEPER_Init(void);//初始化
void beeperOnce(void);//蜂鸣器短响一次
		 				    
#endif
