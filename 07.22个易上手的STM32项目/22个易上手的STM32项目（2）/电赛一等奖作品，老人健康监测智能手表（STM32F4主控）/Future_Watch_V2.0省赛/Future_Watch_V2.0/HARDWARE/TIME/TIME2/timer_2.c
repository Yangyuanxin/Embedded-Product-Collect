/*
**************************************************************
*
* 文件名称：time_6.c;
*
* 版本说明：V.10；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：无；
*           
* 完成日期：2016/02/15;
*
* 作者姓名：郑子烁;
*
* 其他备注：基本定时器6使用；
*
**************************************************************
*/


#include "math.h"
#include "timer_2.h"
#include "mpu6050.h"
#include "main.h"



//extern u8 send_ready;                 //获取数据完，可以发送，测试用；
//extern u8 tmp_buf[6];                 //24l01接收单元；
//extern u8 tumble_flag;                //摔倒标志，1->摔倒，0->正常；
extern short buff[3][401];            //储存x、y、z的加速度值，用于分析；
extern short a_x,a_y,a_z;             //加速度值暂存单元；
extern int acc;                       //合加速度；
extern float sin_temp;                //空间正弦值；
extern SYSTEM_STA System_Sta;	


/*
* 函数名称：Time_6_Init；
* 功能说明：基本定时器6初始化；
* 入口参数：arr->自动重装载值,psc->分频值；
* 返回值  ：无；
* 备注    ：定时器的时钟频率CK_CNT等于fCK_PSC/(PSC[15:0]+1)；
*           计数器是从0开始计数的；
*/
void Time_2_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;                 //使能定时器6时钟；
	
	TIM2->PSC =psc;                      //装入分频值；
	TIM2->ARR =arr;                      //装入自动重装载值(若为0，计数器停止工作)；
	
	TIM2->CR1&=~(1<<3);                  //非单脉冲模式；
	TIM2->CR1&=~(1<<1);                  //使能UEV事件的产生(默认)；
	
	TIM2->SR&=~(1<<0);                   //清零中断标志位；
	TIM2->DIER|=1<<0;                    //允许产生更新中断，禁止产生更新DMA请求；
	
	TIM2->CR1|=1<<0;                     //使能定时器6；
	
	
}


/*
* 函数名称：TIM6_IRQHandler；
* 功能说明：基本定时器6中断服务程序，计算；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：先通过定时器的定时检查，查看是否有可疑的加速度信号出现，如果有，等待
*           2S后求取倾角的值，判断是否符合平躺下时地角度，如果是，报警标志置位；
* 
*/
void TIM2_IRQHandler(void)
{
//	static u8 time=0,j=0,flag=0;
	if(TIM2->SR&0X0001)//溢出中断
	{                  
//		MPU_Read_Accel(&a_x,&a_y,&a_z);     //获取加速度；
//		acc=sqrt(a_x*a_x+a_y*a_y+a_z*a_z);  //得到合加速度；
//		if(acc>18000){flag=1;}              //捕获到可疑信息；
//		if(flag)                            //允许连续取值；
//		{
//			if(++time==200)                 //判断是否到达2s，如果是，再连续取值20次求平均；
//			{
//				if(++j<20)
//				{
//					sin_temp+=(int)(sqrt(a_y*a_y+a_z*a_z)/acc*5000); //得到空间正弦值，放大了5000倍(0~5000)；
//					time=199;
//				}
//				else
//				{
//					sin_temp/=20;           //得到20次角度采样的平均值；
//					j=0;                    //计数清零；
//					time=0;
//					flag=0;
//					if(sin_temp>4600)
//					{System_Sta.Tumble_Flag=1;sin_temp=0;}         //摔倒，清除角度；
//					else
//					{System_Sta.Tumble_Flag=0;}        //没摔倒；
//				}
//			}
//		}
	}
	TIM2->SR&=~(1<<0);
}

