#ifndef __SETTING_H
#define __SETTING_H			  	 
#include "sys.h"
#include "flash.h"

#pragma pack(1)//单字节对齐，很重要！
//不要超过127个16位数
//计数：1+8*3+8*2+8+1+2+1+1
typedef struct _set_Config// 用户设置结构体
{
	u8 writeFlag;//是否第一次写入=1字节8位
	int setTemp;//设置温度
	int sleepTime;//休眠时间
	int shutTime;//关机时间
	u8 workMode;//工作模式
	u8 beeperFlag;//蜂鸣器是否开启
	u8 langFlag;//语言设置
}set_Config;
#pragma pack()


extern volatile set_Config setData;
extern volatile u32 setDataSize;
void set_Init(void);
void resetData(void);

#endif
