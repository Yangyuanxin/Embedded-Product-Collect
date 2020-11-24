/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     除注明出处外，以下所有内容版权均属王志浩所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留作者的版权声明。
 *
 * @file			系统初始化
 * @brief			
 * @author		王志浩
 * @version		V2.2
 * @date			2017-8-30
 */
#ifndef  __SYS_INIT_H_
#define  __SYS_INIT_H_

#include "Sys_Init.h"
/************************声明全局变量*****************************/
#define	BEEP 				(P03)
#define BEEP_ON			(P03 = 0)
#define BEEP_OFF		(P03 = 1)

#define	EC11_A			(P32)
#define	EC11_B			(P31)
#define	EC11_KEY		(P30)

/************************声明外部函数*****************************/
void Sys_Init(void);
void Iron_Off(void);
void Iron_Run(void);

#endif