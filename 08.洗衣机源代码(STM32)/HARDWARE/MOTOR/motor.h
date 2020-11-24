#ifndef __MOTOR_H__
#define __MOTOR_H__
#include"sys.h"
#define MOTOR_A PBout(14)
#define MOTOR_B PBout(15)
void Motor_Init(void);
void Jinshui(void);
void Chushui(void);
void Tingshui(void);
#endif
