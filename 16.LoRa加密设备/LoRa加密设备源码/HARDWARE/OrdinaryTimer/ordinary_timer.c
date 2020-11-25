#include "ordinary_timer.h"


/****************************************************
	* 功能描述：定时器2初始化
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
*****************************************************/ 
void OrdinaryTimerInit(u8 WhichTimer,u16 period, u16 prescaler)
{
	
	u8 priority;			//从优先级
	u8 NVIC_IRQChannel;		//定时器通道
   
	//定义结构体
    TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    NVIC_InitTypeDef 			NVIC_InitStructure;
	TIM_TypeDef 				*TIMx;
	
	switch( WhichTimer )
	{
		case 0x02:
			//TIM3时钟使能 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
			TIMx = TIM2;
			NVIC_IRQChannel = TIM2_IRQn;
			priority = 1;
		break;
		
		case 0x03:
			//TIM3时钟使能 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
			TIMx = TIM3;
			NVIC_IRQChannel = TIM3_IRQn;
			priority = 2;
		break;
		
		case 0x04:
			//TIM3时钟使能 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
			TIMx = TIM4;
			NVIC_IRQChannel = TIM4_IRQn;
			priority = 3;
		break;
		
		default:
			//... ...
		break;
	}
	
    //TIM3初始化设置
    TIM_TimeBaseStructure.TIM_Period 		= period - 1;			//设置自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler 	= prescaler - 1;		//时钟频率除数的预分频值 设置计数频率为10kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     	//设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up; 	//TIM向上计数模式
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);             	//根据指定的参数初始化TIMx的时间基数单位

    TIM_ClearFlag(TIMx, TIM_FLAG_Update);		//清除更新标志位			
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);	//允许TIM3更新中断

    //TIM3中断配置
    NVIC_InitStructure.NVIC_IRQChannel 					 = NVIC_IRQChannel;	//TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   			//先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = priority;		//从优先级1级
    NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;			//IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);                             			//根据指定的参数初始化外设NVIC寄存器

    //使能TIM3
    TIM_Cmd(TIMx, ENABLE);
}


/***************************************
	* 功能描述：定时器3中断服务函数
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
****************************************/
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
		//清除中断标志
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		RunLEDCounter();			//LED翻转计数器
		RunWaitIDCounter();			//等待对方ID回应
		RunLoRaTransCounter();		//LoRa传输等待(接收方)
		JudgeTheHostComWorkMode();	//串口状态扫描
	}
}


void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
		//清除中断标志
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		//... ...
	}
}


void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
		//清除中断标志
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		//... ...
	}
}




