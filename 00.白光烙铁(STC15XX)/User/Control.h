/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     除注明出处外，以下所有内容版权均属王志浩所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留作者的版权声明。
 *
 * @file			Control.h
 * @brief			
 * @author		王志浩
 * @version		V2.2
 * @date			2017-8-30
 */
#ifndef	_CONTROL_H_
#define	_CONTROL_H_

/*中值滤波函数参数定义*/
#define NUM 34 //采样次数(必须是二的倍数次+2)
#define DIV 5 //右移代替除法 相当于 例如( z = y/(2^div) )

extern unsigned int IronTemp_Now;
extern unsigned int IronTemp_AM;
extern unsigned int chn;
extern unsigned char SetTempFlag;
extern unsigned char FtyModeFlag;

extern void IronTempControl(unsigned int temp);
extern void Sys_Monitor(void);


#endif