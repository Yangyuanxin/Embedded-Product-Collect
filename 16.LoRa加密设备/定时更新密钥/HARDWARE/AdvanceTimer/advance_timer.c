#include "advance_timer.h"

extern void RunCalcOneHourCounter(void);


/************************************************
	* 功能描述：高级定时器初始化
	* 入口参数：WhichTimer 	-- 定时器1/2/3
				period	   	-- 自动重装值
				prescaler	-- 预分频系数
	* 返回值：
	* 备注：	
*************************************************/
void AdvanceTimerInit(u8 WhichTimer, u16 period, u16 prescaler)
{
    TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
    NVIC_InitTypeDef 			NVIC_InitStructure;
    TIM_TypeDef					*TIMx;
	
    u8 NVIC_IRQChannel;
    u8 priority;
	
	//使能定时器的时钟及选择通道
    switch(WhichTimer)
    {
		//定时器1
		case 1:
			TIMx = TIM1;
			NVIC_IRQChannel = TIM1_UP_IRQn;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
			priority = 2;
		break;
		
		//定时器8
		case 8:
			TIMx = TIM8;
			NVIC_IRQChannel = TIM8_UP_IRQn;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
			priority = 1;
		break;
		
		//其他情况
		default:
			return ;
		break;
    }
	
	//定时器基本配置
    TIM_TimeBaseStructure.TIM_Period 			= period - 1;	 		//设置自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler 		= prescaler - 1;		//时钟频率除数的预分频值		
    TIM_TimeBaseStructure.TIM_ClockDivision 	= TIM_CKD_DIV1;			//时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;					//设置重复溢出次数为0(高级定时器需配置)
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	
	//清除
    TIM_ClearFlag(TIMx, TIM_FLAG_Update);		//清除更新标志
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE );	//开启定时器溢出中断
	
	//定时器中断参数配置
    NVIC_InitStructure.NVIC_IRQChannel 					 = NVIC_IRQChannel;	//选择中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;				//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = priority;		//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;			//使能通道
    NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM1,ENABLE);
}


/********************************************
	* 功能描述：使能定时器
	* 入口参数：which -- 定时器编号
				state -- 状态
	* 返回值：	无
	* 备注：	无
*********************************************/
void SetAdvanceTimerState(u8 which,FunctionalState state)
{
	switch(which)
	{
		case 1:
			TIM_ITConfig(TIM1,TIM_IT_Update,state);
			TIM_Cmd(TIM1,state);
		break;
		
		case 8:
			TIM_ITConfig(TIM8,TIM_IT_Update,state);
			TIM_Cmd(TIM8,state);
		break;
		
		default:
		break;
	}
}


//定时器1中断服务程序
void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
		//除清中断标志位
		TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update); 
		
		RunComAtModeTimer();		//上位机指令计时
		RunComStatusChangeTimer();	//按键时间检测(串口状态切换)
		RunDelayCounter();			//延时函数(ms)
//		RunWatchDogTimer();			//喂狗计时器
		RunCalcOneHourCounter();
    }
	
}


//定时器1中断服务程序
void TIM8_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
    {
		//除清中断标志位
		TIM_ClearITPendingBit(TIM8, TIM_FLAG_Update); 
    }
	
}



