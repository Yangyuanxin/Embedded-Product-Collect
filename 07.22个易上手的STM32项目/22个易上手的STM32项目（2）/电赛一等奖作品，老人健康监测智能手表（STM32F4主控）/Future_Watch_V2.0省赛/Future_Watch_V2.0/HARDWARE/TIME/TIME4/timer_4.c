#include "timer_4.h"
#include "led.h"
#include "delay.h"
#include "wdg.h"
#include "rtc.h"
#include "main.h"

extern SYSTEM_STA System_Sta;
//定时器3中断服务程序	 
void TIM4_IRQHandler(void)
{ 		    		  			    
	if(TIM4->SR&0X0001)//溢出中断
	{
		IWDG_Feed();		//喂狗
		System_Sta.Get_Time++;		//时间
		System_Sta.Get_Heart++;		//心率
		System_Sta.Get_Temp++;		//体温
		System_Sta.Get_Tumble++;	//摔倒
		System_Sta.Get_backlit++;	//背光
		System_Sta.Check_Health++;		//体温，心率，检查报警时间
		System_Sta.Help_Time++;
		if((KEY_CHECK) != 0)		//高电平，按键按下
		{
			System_Sta.Check_Key++;		//每+1表示延时了100ms
			if(System_Sta.Check_Key>60)	//i大于60 说明高电平已经持续了6s执行关机
			{
				System_Sta.Motor |= (1<<7);	//触发一次震动
				OSIntEnter();  
				GUI_SetBkColor(GUI_WHITE);	//设置背景色
				GUI_Clear();
				GUI_SetFont(&GUI_Font32_ASCII);
				GUI_SetColor(GUI_BLACK);
				GUI_DispStringAt("turn off..",60,120);
				GUI_Exec();
				delay_ms(500);
				Save_Data();	//保存数据
				STOP_MCU;		//关机
			}
		}
		else			//按键松开
		{
			if(System_Sta.Check_Key>30)//大于3s 触发报警
			{
				System_Sta.Motor |= (1<<7);	//触发一次震动
				System_Sta.Warning |= (1<<4);
				System_Sta.Check_Key=0;
			} 
			else
			{
				if(System_Sta.Check_Key>2)		//开屏和锁屏
				{
					System_Sta.Motor |= (1<<7);	//触发一次震动
					if((System_Sta.Disp_Sta&(1<<5))==0)
					{
						System_Sta.Get_backlit |= (1<<7);	//关闭背光控制
						TIM3->CCR3 = 0;
						System_Sta.Disp_Sta |= (1<<5);		//锁屏
					}
					else
					{
							System_Sta.Get_backlit &= ~(1<<7);		//开启背光控制
							TIM3->CCR3 = 500;
							System_Sta.Disp_Sta &= ~(1<<5);		//开屏
					}
						System_Sta.Check_Key = 0;
				}
				else
				{
					System_Sta.Check_Key = 0;//清0
				}																			 					
			}
		}
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void Time_4_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4时钟使能    
 	TIM4->ARR=arr;  	//设定计数器自动重装值 
	TIM4->PSC=psc;  	//预分频器	  
	TIM4->DIER|=1<<0;   //允许更新中断	  
	TIM4->CR1|=0x01;    //使能定时器3								 
}














