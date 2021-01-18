#include "pid.h"
#include "main.h"
#include "adc.h"
#include "usart.h"
#include "key.h"
#include "menu.h"
#include "setting.h"

#define DBGMCU_CR  (*((volatile u32 *)0xE0042004))
u16 timecount,g_bPIDRunFlag;
volatile int uk;//uk为当前输出值
int duk;//duk为PID增量
float aggKp,aggKi,aggKd;//激进的PID参数
float consKp,consKi,consKd;//保守的PID参数
float e0,e1,e2;
float st,pt;
u16 PWM_T = 255;//PWM输出周期
volatile u32 nowTime = 0;//程序运行时间，单位0.01s
volatile u32 sleepCount = 0;
volatile u32 shutCount = 0;
//PID参数初始化
void PID_Setup(void)
{
	aggKp = 11;//比例参数，设置调节力度
	aggKi = 0.5;//积分参数T/Ti，可以消除稳态误差
	aggKd = 1;//微分参数Td/T，可以预测误差的变化，做到提前决策
	consKp=11;//保守的PID参数
	consKi=3;
	consKd=5;
}
//计算PID输出uk
void PID_Operation(void)
{
	pt = T12_temp;//当前温度值
	if(sleepFlag) e0 = TEMP_SLEEP - pt;//睡眠模式下的温度
	else if(setData.workMode==0) //强力模式下增加温度
	{
		if(setData.setTemp + TEMP_ADD > TEMP_MAX) e0 = TEMP_MAX - pt;//不能超过最大值
		else  e0 = setData.setTemp + TEMP_ADD - pt;
	}
	else e0 = setData.setTemp - pt;//标准模式
	if(e0>50) uk=PWM_T;//温差>50℃时，全速加热
	else if(e0>30)//温差30~50℃时，进行激进的PID解算
	{
		duk=aggKp*(e0-e1)+aggKp*aggKi*e0+aggKp*aggKd*(e0-2*e1+e2);
		uk=uk+duk;
	}
	else//温差<30℃时，进行保守的PID解算
	{
		duk=consKp*(e0-e1)+consKp*consKi*e0+consKp*consKd*(e0-2*e1+e2);
		uk=uk+duk;
	}
	if(uk > PWM_T) uk = PWM_T;//防止饱和
	if(uk < 0) uk = 0;
	e2=e1;
	e1=e0;
	
}
/* ********************************************************
* 函数名称：PID_Output()                                                                         
* 函数功能：PID输出控制                                         
* 入口参数：无（隐形输入，U(k)）                                                 
* 出口参数：无（控制端）                                                                               
******************************************************** */

void PID_Output(void)
{
	T12_temp = get_T12_temp();//当前温度值
	if(shutFlag==0) PID_Operation();//调用PID运算
	else uk = 0;//不加热
	TIM_SetCompare1(TIM2,uk); //输出给PWM-PA0，加热
}
//加热头控制引脚初始化-PB4
void HEAT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIO外设时钟
	    
 
   //设置PA0引脚为复用输出功能,输出PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //TIM2_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA
	
	
   //初始化TIM2
	TIM_TimeBaseStructure.TIM_Period = PWM_T-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =719; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM2 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1，计数值<自动重装载值时，输出高电平
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC1

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR1上的预装载寄存器

	TIM_Cmd(TIM2, ENABLE);  //使能TIM2
}
	
//定时器4初始化，为PID提供计时
void TIM4_Counter_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); 
	
    TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //预分频值
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //时钟分割为0,仍然使用72MHz
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//允许更新中断
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM_Cmd(TIM4,ENABLE);
}
//TIM4定时器中断服务函数
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)!=RESET)//检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//清除TIM4更新中断标志
		nowTime++;//100ms更新一次计时器
		sleepCount++;
		shutCount++;
		sleepCheck();//检测振动开关
	}
}
//获取时间字符串
void getClockTime(char timeStr[])
{
	u32 min=0,sec=0;
	sec = nowTime/10;
//	hour = sec/3600;
	min = sec/60;
	sec = sec-min*60;
	sprintf((char *)timeStr,"%03d:%02d",min,sec);//组合时间字符串
}

