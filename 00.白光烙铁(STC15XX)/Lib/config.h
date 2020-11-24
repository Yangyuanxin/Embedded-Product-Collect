/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     除注明出处外，以下所有内容版权均属王志浩所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留作者的版权声明。
 *
 * @file			配置文件
 * @brief			
 * @author		王志浩
 * @version		V2.2
 * @date			2017-8-30
 */
#ifndef		__CONFIG_H
#define		__CONFIG_H


/************************定义系统时钟*****************************/
//#define MAIN_Fosc		22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
//#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
#define MAIN_Fosc		24000000L	//定义主时钟
#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)

/****************************接口定义****************************/



/***************************包含库函数***************************/
#include	"STC15Fxxxx.H"
#include 	"stdio.h"
#include 	"delay.h"
#include 	"GPIO.h"
#include 	"ADC.h"
#include 	"Exti.h"
#include 	"timer.h"
#include 	"PCA.h"
#include  "EEPROM.h"
//#include  "USART.h"

/***************************包含用户函数***************************/
#include "Sys_Init.h"
#include "LQ12864.h"
#include "EC11.h"
#include "Control.h"
//#include "Kalman.h"

#endif
