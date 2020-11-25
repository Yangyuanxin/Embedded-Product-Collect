#define _VIEW_C
//#include <stdio.h>
#include <math.h>

#include "head.h"
/******************EDIT:chang**********************/

#define MEASURE_DATA_X      64

static u8 firstcominto;


u32 turn_time;

YC_PAGE yc_disp;

//定义遥测类型
INT8 *yc_type[] = 
{				
	"1#主温度",
	"1#主值",
	"1#从温度",
	"1#从值",   
    
	"2#主温度",
	"2#主值",
	"2#从温度",
	"2#从值",       
	
    "2#再温度",
	"2#再值", 
    "3#再值",
    "流量",
    
	"3#主值",
    "3#从值",
};

INT8 dis_xiaoshudian[] = 
{
	1,//        "1#主温度",
	0,// 		"1#主值",
	1,// 		"1#从温度",		
	0,// 		"1#从值",,
    
	1,// 		"2#主温度",
	1,// 		"2#主值",,
	1,// 		"2#从温度",
	1,// 		"2#从值",,    
    
    1,//"2#再温度",
	1,//"2#再值", 
    0,//"3#再值",
    3,//"流量",
    
	3,//"3#主值",
    3,//"3#从值",
    
	
		
};

/******************************************
函数功能：显示主界面

*******************************************/
//hyman2011-4
void MainView(void)
{
	
	if(firstcominto == 0)
	{
		firstcominto = 1;
        
        turn_time = sys_tick;
		
		yc_disp.number_page=0;
		
		Yc_Disppage(&yc_disp);
		
	}
	
    if (sys_tick - turn_time >= 500)
	{        
		yc_disp.number_page++;
        
        if (NUM_YC%4)
        {
            if(yc_disp.number_page== ((NUM_YC/4) + 1)) //guohui.2011.01
			{  
				yc_disp.number_page=0;
			}
        }
        else
        {
			if(yc_disp.number_page== (NUM_YC/4)) //guohui.2011.01
			{  
				yc_disp.number_page=0;
			}
        }
        
		
		Yc_Disppage(&yc_disp);
		
        
        turn_time = sys_tick;
	}    
	
}

/*******************************************
函数功能：整屏显示遥测数据
参数声明：显示第几页遥测
返回值：无返回值
********************************************/
void Yc_Disppage (YC_PAGE* yc_information)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
    char strbuf[32];
	u8 i;
	u8 yc_index;
	u8 num;//剩余的遥测个数
	
	
	num=NUM_YC-yc_information->number_page*4;
	if(num>=4)
	{
		num=4;
	}
	
	GUI_ClrScr();//清屏
    
    for(i=0;i<num;i++)
	{
		yc_index=yc_information->number_page*4+i;
		
		GUI_PutString16_16(0, 16*i, yc_type[yc_index], 0);
		
		sprintf(strbuf,"%05d",lcd_disp_measure[yc_index]);
				
		Adj_xiaoshudian(strbuf,dis_xiaoshudian[yc_index]);
		//
		if (dis_xiaoshudian[yc_index])
		{
			GUI_PutString8_16(MEASURE_DATA_X, i*16, strbuf);
		}
		else
		{
			GUI_PutString8_16(MEASURE_DATA_X+8, i*16, strbuf);
		}
	}
	
	REFURBISH_LCD();				//刷新显示屏
	
}


void Adj_xiaoshudian(char *buf,u8 dot_num)
{
	UINT8 temp1,temp2;	
	
	if (dot_num)
	{
		temp1 = strlen(buf);
		buf[temp1+1] = '\0';
		
		for (temp2=0; temp2<dot_num; temp2++)
		{
			buf[temp1] = buf[temp1-1];
			temp1--;
			
			
		}  		
		buf[temp1] = '.';
		
		//前置0设定，前置0消隐，小数点前0设置
		temp2 = 0;
		temp1--;
		while (temp2 != temp1)
		{
			if (buf[temp2] == '0')
			{
				buf[temp2] =' ';
			}
			else
			{
				break;
			}
			
			temp2++;
		}
		
		
		
	}
	//
	else
	{
		temp2 = 0;
		temp1 = strlen(buf) - 1;
		
		while (temp2 != temp1)
		{
			if (buf[temp2] == '0')
			{
				buf[temp2] =' ';
			}
			else
			{
				break;
			}
			
			temp2++;
		}
	}
	
	
}