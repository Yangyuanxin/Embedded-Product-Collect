/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     除注明出处外，以下所有内容版权均属王志浩所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留作者的版权声明。
 *
 * @file			主程序文件
 * @brief			
 * @author		王志浩
 * @version		V2.2
 * @date			2017-8-30
 */
#include "config.h"

void main(void)
{
	//系统初始化
	Sys_Init();
	while(1)
	{
		//按键设定参数
		ParaSet();
		//显示主界面
		Sys_Monitor();
	}
}