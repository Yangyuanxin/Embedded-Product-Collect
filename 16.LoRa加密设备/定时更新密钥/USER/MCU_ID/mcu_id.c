#include "mcu_id.h"

u16 ChipFlashSize   = 0x00;		//保存MCU的Flash容量
u32 ChipUniqueID[3] = {0,};		//保存MCU的电子签名(96bit)


/**************************************
	* 功能描述：获取芯片信息
	* 入口参数：无  
	* 返回值：	无
	* 备注：	无
***************************************/
void GetChipInfo(void)
{
	ChipUniqueID[0] = *(__IO u32 *)(0x1FFFF7F0); 	//高字节
	ChipUniqueID[1] = *(__IO u32 *)(0x1FFFF7EC); 	//中间字节
	ChipUniqueID[2] = *(__IO u32 *)(0x1FFFF7E8); 	//低字节

	ChipFlashSize   = *(__IO u16 *)(0x1FFFF7E0);	//闪存容量寄存器(单位:KB)  
}




