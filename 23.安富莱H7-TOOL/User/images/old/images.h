/*
*********************************************************************************************************
*
*    模块名称 : 图片资源模块。
*    文件名称 : image.H
*    版    本 : V2.0
*    说    明 : 头文件
*    修改记录 :
*        版本号  日期       作者    说明
*        v2.0    2011-10-16 armfly  创建
*
*    Copyright (C), 2010-2012, 安富莱电子
*
*********************************************************************************************************
*/

#ifndef __IMAGE_H
#define __IMAGE_H

#include "stm32f4xx.h"

extern const unsigned char gImage_QQ[48 * 48];
extern const unsigned char gImage_UDisk[48 * 48];
extern const unsigned char gImage_SIM[48 * 48];
extern const unsigned char gImage_Bat[48 * 48];
extern const unsigned char gImage_Spk[48 * 48];

extern const unsigned char gImage_Camera32[48 * 48 * 4];
extern const unsigned char gImage_Phone32[48 * 48 * 4];

#endif
