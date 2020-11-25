/*******************************************************************************************************
* File Name:              main.c
* Copyright:              OTouch->TopWill，All Rights Reserved.
* Author/Corporation:     王元
* Create date:            2016-09-01
* Version:                V1.5
* Abstract Description:   主函数   
*------------------------Revision History----------------------------------------------------------------
*  NO    Version      Date       Revised By    Description
*  1      V1.5      2016/9/6      xiaoyuan     
*********************************************************************************************************/

#include "marlin_main.h"
#include "include_conf.h"


/********************************************************************************************************
* Function name:           main
* Author/Corporation:      王元
* Create date:             2016/12/08
* input parameters:        none                      
* output parameters:       none                
* parameters Description:  none                           
* Return value:            none   
* Abstract Description:    主函数
*------------------------Revision History----------------------------------------------------------------
*  NO    Version      Date       Revised By    Description
*  1      V1.5      2016/12/8      xiaoyuan     规范代码风格
*********************************************************************************************************/
int main(void)
{	
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	 //*****************************初始化内核系统驱动************************//
     delay_init();			     //延时初始化
 	 uart1_init(BAUDRATE); 	 //串口1初始化  
     TIM3_Int_Init(9,7199);  //用于返回系统运行时间（1ms定时中断）
	 TIM2_Int_Init(99,7199); //用于按键的定时检测（10ms定时中断）
     EXTIX_Init();           //用于按键检测，暂且没用到
//	 Adc_Init();			  	   //ADC初始化,内部温度传感器 
	 //****************************初始化底层硬件驱动**************************//
 	 LED_Init();	    	//LED初始化
	BEEP_Init();
 	 KEY_Init();				//按键初始化 
	 AT24CXX_Init();    //AT24CXX初始化 
	 SPI_Flash_Init();  //W25Qxx初始化
	 LCD12864_Init();   //ST7920 lcd12864底层驱动的初始化
	 //*****************************初始化软件层驱动	****************************// 										  
   mem_init();				      //内存池初始化
   exfuns_init();		        //为fatfs相关变量申请内存
   f_mount(fs[0],"0:",1); 	//挂载SD卡  
//   f_mount(fs[1],"1:",1); 	//挂载挂载FLASH.	
	 lcd_menuInit();          //屏幕菜单初始化
   //***************************软件系统初始化*********************************//
 	setup(); //执行打印进程所需的初始化函数
	
	while(1) //非打印状态下的死循环
	{		
		loop(); //打印进程循环体
	}
}
