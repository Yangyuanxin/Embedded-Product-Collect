#ifndef _RGBLED_H 
#define _RGBLED_H

#include "sys.h"
#include "delay.h"
#include "main.h"

#define LED_RGB     PBout(7)

#define RGB_NOCHANGE    1

//RGB颜色定义
#define COLOR_RED		0xFF0000	//-纯红
#define COLOR_GREEN		0x008000	//-纯绿
#define COLOR_BLUE		0x0000FF	//-纯蓝
#define COLOR_YELLOW	0xFFFF00	//-纯黄   //准备阶段
#define COLOR_WHITE		0xFFFFFF	//-纯白
#define COLOR_BLACK		0x000000	//-纯黑

void RGB_Set_Down(void); //发送0
void RGB_Set_Up(void);   //发送1
void RGB_Led_Init(void);
void RGB_SendData(u32 dat);  //值以RGB顺序存储，发送时需以GRB顺序发送
void RGB_SendAllData(u32 dat[],u8 size);
void RGB_Control(u32 rgb_dev,u32 rgb_warning);  

#endif



