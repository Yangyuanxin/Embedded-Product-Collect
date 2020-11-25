#define _PARA_C

#include "head.h"
	

UINT8 ReadPara(void)
{
	u16 *p;	
	
	
    UINT8 i;
	
	
    i=0;    
    
	p = (u16*)0x0803fc00;
	//
	if (*p == 0x68)
	{
        
        for(i=0;i<NUM_PARA;i++)
        {
			para.value_group[i]=*p++;             
        }		
        
		return 1;
	}
	
	
	//send_str(0,"读失败");
	return 0;
	
	
}
/*
-----------------------------------------------------------------------------------------
运行参数初始化函数
-----------------------------------------------------------------------------------------
*/
void InitPara(void)
{
	u8 i;
	
	para_enable_flag = 1;
	
	if (!ReadPara())
	{
		para.value.addr = 1;
		
		para_enable_flag = 0;   //参数读成功标志
		
		//load default
		yc_buf[16] = 100; //手动设定计量泵1速度
		yc_buf[17] = 100; //手动设定计量泵2速度
		yc_buf[18] = 100; //手动设定计量泵3速度
		yc_buf[19] = 100; //手动设定计量泵4速度
		
        yc_buf[20] = 500; //主1#传感器高限
		yc_buf[21] = 100; //主1#传感器低限
		yc_buf[22] = 500; //主2#传感器高限
		yc_buf[23] = 100; //主2#传感器低限
		yc_buf[24] = 500; //主3#传感器高限
		yc_buf[25] = 100; //主3#传感器低限
		yc_buf[26] = 200; //3#计量泵每吨水加药量
		yc_buf[27] = 200; //4#计量泵每吨水加药量
		yc_buf[28] = 0; //手动设定计量泵5速度
		yc_buf[29] = 15; //每次排污时间(秒)
		yc_buf[30] = 15; //最少排污周期（秒）
		yc_buf[31] = 1; //远程、自动转换（适用于排污）

        
        for(i=0;i<16;i++)
		{
			para.value_group[i+2] = yc_buf[i+16]; //空出前两个寄存器分别是0x68标识和 本机地址。
		}
	}
	else	//把参数放入寄存器列表中。
	{
		for(i=0;i<64;i++)
		{
			yc_buf[i+16]=para.value_group[i+2]; //空出前两个寄存器分别是0x68标识和 本机地址。
		}
	}	
	
}

void SavePara(void)
{
    u16 temp_buf[128];
	u8 i;
	//     //--------------------------------------------------
    para.value_group[0] = 0x68; 	
    for(i=0;i<NUM_PARA;i++)
    {
		
		temp_buf[i]=para.value_group[i];
    }
	
    //保存信号量
	WritePara((u32 *)temp_buf, 32);
    
}

