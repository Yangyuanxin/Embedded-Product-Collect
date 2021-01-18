#ifndef __FLASH_H__
#define __FLASH_H__
#include "sys.h"  

//用户根据自己的需要设置
#define STM32_FLASH_SIZE 64 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不使能;1，使能)

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
//设置FLASH保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
#define FLASH_SAVE_ADDR  0X08000000+1024*63+4 //每页的前4个字节保留，是该页状态标志
//数据以16位格式保存，占用4个字节：前2个字节为数据，后2个字节为该数据的虚拟地址
//若页大小为1k字节的话，只能保存1024/4-1=255个16位数据，最好不要超过50%

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据

//测试写入
void Test_Write(u32 WriteAddr,u16 WriteData);		   
#endif

















