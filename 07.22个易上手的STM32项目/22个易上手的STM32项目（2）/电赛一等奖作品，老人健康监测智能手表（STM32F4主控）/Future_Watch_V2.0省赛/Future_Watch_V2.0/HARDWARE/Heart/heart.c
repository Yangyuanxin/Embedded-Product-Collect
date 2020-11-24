/*
**************************************************************
*
* 文件名称：time_1.c;
*
* 版本说明：V.13；
*
* 运行环境：(1) STM32F103RCT;
*           (2) 外部晶振8M9倍频；
*           (3) MDK 5.0；
*
* 修改说明：增加输入捕获实验；
*           
* 完成日期：2016/03/04;
*
* 作者姓名：郑子烁;
*
* 其他备注：高级定时器计数实验；
*           PWM产生实验；
*           输入捕获实验；
*           编码器接口模式实验；
*      
**************************************************************
*/

#include "heart.h"
#include "gpio.h"
#include "usart_2.h"
u8  cnt=0;
u8  read_flag=0;                //读取允许标志；
u16 last_data=0;                //储存心率上次数据；
u16 hear_rising[6];             //上升沿记录；


/*
* 函数名称：Time_1_Init；
* 功能说明：高级定时器初始化；
* 入口参数：arr->自动重装值，psc->预分频数；
* 返回值  ：无；
* 备注    ：注意主从模式的时钟源；
*           pwm输出实验时默认向上边沿计数，禁止中断和DMA；
*           输入捕获实验时允许中断，应注意T1和T8的中断服务函数和其他有所不同；
*           在编码器模式下，设置SMS='011'，相当于四倍频；
*/
void Time_1_Init(u16 arr,u16 psc)
{	
	/*端口设置*/ 
	GPIO_Clock_Set(IOPAEN);
	GPIO_Set(GPIOA,PIN8,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,下拉
	GPIO_AF_Set(GPIOA,8,1);	//PA0,AF2
	/*使能时钟*/
	RCC->APB2ENR |=1<<0;                           //使能定时器1时钟；
	
	TIM1->PSC =psc;                                 //预分频；
	TIM1->ARR =arr;                                 //自动重装载；
	

	/*输入捕获设置此处M3与M4相同*/
	TIM1->CCMR1 |=1<<0;                           	  //01：CC1通道配置为输入,IC1映射到TI1上	
	TIM1->CCMR1 &= ~((1<<3)|(1<<2)); 				 //不分频；                      
	TIM1->CCMR1 |=(0<<4);                            //fDTS/32 N=8

	TIM1->CCER &=~(1<<1);                         	 //非反相上升沿触发
	TIM1->CCER &= ~(1<<3); 							
	TIM1->CCER |=(1<<0);                            //允许计数器值到输入捕捉寄存器中；
	
	TIM1->EGR=1<<0;	
	//TIM1->DIER |=1<<0;                              //允许更新中断(溢出中断)；
	TIM1->DIER |=1<<1;                              //允许1捕获中断；
//	TIM1->SR &=~(1<<0);                             //清除更新标志；
//	TIM1->SR &=0;                                   //清除捕获标志；	

	
	/*使能*/
	TIM1->SR =0;  
	TIM1->CR1 |=1<<0;                               //使能定时器1；	

	/*中断优先级设置*/

} 


/*
* 函数名称：TIM1_CC_IRQHandler；
* 功能说明：定时器1比较捕获中断服务函数；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：无；
*/
void TIM1_CC_IRQHandler(void)
{
	u16 buf = TIM1->CCR1;
	TIM1->CNT=0;	
	if((read_flag&0x01) == 0)
	{
		if((buf>4600)&&(buf<12000))		// 这个范围内的心跳才是有效心跳
		{
			hear_rising[cnt]=buf;				//获取上一次心跳的计数值
			cnt++;
		}
		else
		{
			cnt = 0;
		}
	}								
	if(cnt==3)		//连续拿了6次数据
	{read_flag|=(1<<0);cnt=0;}
	TIM1->SR =0;  	//中断标志位清0
}


/*
* 函数名称：Get_Haer_Rate；
* 功能说明：读心率数据；
* 入口参数：无；
* 返回值  ：心率；
* 备注    ：无；
*/
u8 Get_Haer_Rate(void)
{	
	u32 temp = 0;
	u8 i;
	if((read_flag&0x01) == 0x01)                                              //读允许；
	{
		for(i=0;i<3;i++)
		{
			temp += hear_rising[i];
		}

		temp = temp/3;
		temp=CNT_FREQUENCY/temp;                                  //得到心率；
		last_data = temp;
		read_flag &= ~(1<<0);
		read_flag |= (1<<1);	//测量完成标志位
		return temp;
	}
	read_flag &= ~(1<<1);
	return 0;		//读取三次不够，返回上一次的值
}
