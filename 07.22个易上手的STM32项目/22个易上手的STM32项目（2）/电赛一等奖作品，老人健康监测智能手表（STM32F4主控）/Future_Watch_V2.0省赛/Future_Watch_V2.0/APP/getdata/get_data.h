#ifndef _GET_DATA_H_
#define _GET_DATA_H_

#include "main.h"

void Data_GetHeart(void);
void Data_GetTemp(void);
void Data_GetTime(void);
u8 Data_GetBP(void);
int Data_GetServer(void);
int Data_GetServer(void);
int Data_Convert(float Temp,u8 Heart,u8 BP,u8 BD,u8 sta);//将心率、温度、血压数据编码
int Watch_Get_Time(void);
#endif

