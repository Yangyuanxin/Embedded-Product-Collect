#ifndef __key_H
#define __key_H

#include "sys.h"

//#define KEY0 PCin(0)  //PC0
//#define KEY1 PDin(12)  //PD12

#define KEY_D PEin(7)
#define KEY_R PEin(8)
#define KEY_M PEin(9)
#define KEY_L PEin(10)
#define KEY_U PEin(11)

#define KEY0  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)//读取按键1
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键2 
 

#define KEY0_PRESS	6		//KEY0  
#define KEY1_PRESS	7		//KEY1 
#define WKUP_PRESS	8		//WK_UP 

void KEY_Init(void);
u8 KEY_Scan(u8 mode);

#endif //__key_H
