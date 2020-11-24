
/*************	功能说明	**************

本文件为STC15xxx系列的定时器初始化和中断程序,用户可以在这个文件中修改自己需要的中断程序.


******************************************/



#include	"timer.h"
int temp = 0;

/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	static unsigned int BeepCounter = 0;
	
	//旋转或按下后BeepCounterFlag被置位，开始计数
	if(BeepCounterFlag)
	{
		BeepCounter++;
	}

	//计数小于设定值时打开，大于时关闭,Beep_Volume的值决定了占空比
	if((BeepCounter > 0) && (BeepCounter <= MENU_DATA_Table.Other_s.Beep_Volume*150))
	{
		BEEP_ON;
	}
	else
	{
		BEEP_OFF;
	}
	//清零
	if(BeepCounter == 1500)
	{
		BeepCounter = 0;
		BeepCounterFlag = 0;
	}
}
unsigned char IronUseFlag = 0;
unsigned char CoderUseFlag = 0;
unsigned char IronSleepFlag = 0;
unsigned char IronScrSaverFlag = 0;
unsigned char IronShutdownFlag = 0;

/********************* Timer1中断函数************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
	static unsigned int Timer0_Counter=0,IronUseCounter=0;
	static unsigned char Sec_Conter=0,Min_Conter=0;
	static unsigned char P01_Old = 0;
	//定时器0分段计数器自加
	Timer0_Counter++;
	
	//1ms处理部分
	EC11_Capture();//捕捉EC11信息	

	//20ms处理部分
	if(Timer0_Counter%10 == 0)
	{
		if(P01 != P01_Old)
		{
			IronUseCounter++;
		}
		
		if(IronUseFlag || CoderUseFlag)//烙铁或编码器用了，那要看看现在是不是休眠什么的状态了
		{
			if(IronSleepFlag)//从休眠中唤醒
			{
				Sec_Conter = 0;
				Min_Conter = 0;
				ec11_state.Key = 0;
				ec11_state.Coder = 0;
				IronSleepFlag = 0;
			}
			else if(IronScrSaverFlag)//从关屏中唤醒
			{
				Sec_Conter = 0;
				Min_Conter = 0;
				ec11_state.Key = 0;
				ec11_state.Coder = 0;
				IronScrSaverFlag = 0;
				IronSleepFlag = 0;
			}
			else if(IronShutdownFlag)//掉电唤醒
			{
				Sec_Conter = 0;
				Min_Conter = 0;
				ec11_state.Key = 0;
				ec11_state.Coder = 0;
				IronShutdownFlag = 0;
			}
		}
	}
	//100ms处理部分
	if(Timer0_Counter%100 == 0)
	{

		//温度控制

		if(IronSleepFlag || IronScrSaverFlag)
		{
			IronTempControl(MENU_DATA_Table.Temp_s.Slp_Temp);
		}
		else
		{
			IronTempControl(IronTemp_AM);
		}
		
		P01_Old = P01;
	}
	

	
	//1000ms处理部分
	if(Timer0_Counter == 200)
	{
		//一秒内看10次，如果这10次有N次烙铁手柄状态发生变化，那么认为是确实在使用
		if(IronUseCounter>MENU_DATA_Table.Other_s.Shock_Sensor)
		{
			IronUseFlag = 1;
			IronUseCounter = 0;
		}
		else
		{
			IronUseFlag = 0;
			IronUseCounter = 0;
		}

		//如果没用，那么开始计时，只要用了就清零
		if(IronUseFlag==0 && CoderUseFlag==0 && ShutdownCounterFlag==0)
		{
			Sec_Conter++;
		}
		else
		{
			Sec_Conter = 0;
			Min_Conter = 0;
			CoderUseFlag = 0;
		}
		Timer0_Counter = 0;
	}
	
	//每分钟
	if(Sec_Conter == 250)//定时不准
	{
		Min_Conter++;
		Sec_Conter = 0;
		//以下是休眠、关屏、和软关机
		//如果睡眠时间不为0且不在休眠或关屏状态
		if((MENU_DATA_Table.Time_s.Slp_Time != 0) && (IronSleepFlag == 0) && (IronScrSaverFlag == 0))
		{
			if(Min_Conter == MENU_DATA_Table.Time_s.Slp_Time)
			{
				IronSleepFlag = 1;
				IronScrSaverFlag = 0;
				BEEP_ON;
				Delay_ms(150);
				BEEP_OFF;
				//清计时器为关屏做准备
				Sec_Conter = 0;
				Min_Conter = 0;
			}
		}
		//已经休眠,且尚未关屏,不管关屏时间，如果关屏时间是0，那直接休眠的时候就把屏关了
		if((IronSleepFlag == 1) && (IronScrSaverFlag == 0))
		{
			if(Min_Conter == MENU_DATA_Table.Time_s.ScrSaver_Time)
			{
				IronSleepFlag = 0;
				IronScrSaverFlag = 1;
				BEEP_ON;
				Delay_ms(150);
				BEEP_OFF;
				//清计时器为软关机做准备
				Sec_Conter = 0;
				Min_Conter = 0;
			}
		}
		//如果软关机时间不为0且已经关屏
		else if((MENU_DATA_Table.Time_s.Shutdown_Time != 0) && (IronScrSaverFlag == 1))
		{
			if(Min_Conter == MENU_DATA_Table.Time_s.Shutdown_Time)
			{
				//清计时器为掉电唤醒做准备
				Sec_Conter = 0;
				Min_Conter = 0;
				//清休眠、关屏标志位
				IronScrSaverFlag = 0;
				IronSleepFlag = 0;
				IronShutdownFlag = 1;
				//蜂鸣器响
				BEEP_ON;
				Delay_ms(250);
				Delay_ms(50);
				BEEP_OFF;
				//软关机
				P10 = 0;//确保烙铁关到位了
				PCON |= 0x02;
			}
		}

	}
}
/********************* Timer2中断函数************************/
void timer2_int (void) interrupt TIMER2_VECTOR
{
	static unsigned char SetTempCounter = 0;
	
	if(SetTempFlag == 1)
	{
		SetTempCounter++;
	}
	if(SetTempCounter == 60)
	{
		SetTempCounter = 0;
		SetTempFlag = 0;
		if((MENU_DATA_Table.Temp_s.Default_Temp == 0) || (MENU_DATA_Table.Temp_s.Default_CH == 0))
		EEPROM_SectorErase(0x0);
		EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
	}
}


//========================================================================
// 函数: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// 描述: 定时器初始化程序.
// 参数: TIMx: 结构参数,请参考timer.h里的定义.
// 返回: 成功返回0, 空操作返回1,错误返回2.
// 版本: V1.0, 2012-10-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	if(TIM > Timer2)	return 1;	//空操作

	if(TIM == Timer0)
	{
		TR0 = 0;		//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		ET0 = 1;	//允许中断
		else									ET0 = 0;	//禁止中断
		if(TIMx->TIM_Polity == PolityHigh)		PT0 = 1;	//高优先级中断
		else									PT0 = 0;	//低优先级中断
		if(TIMx->TIM_Mode >  TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TMOD = (TMOD & ~0x03) | TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~0x80;	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x80;	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x04;	//对外计数或分频
		else										TMOD &= ~0x04;	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x01;	//输出时钟
		else							INT_CLKO &= ~0x01;	//不输出时钟
		
		TH0 = (u8)(TIMx->TIM_Value >> 8);
		TL0 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR0 = 1;	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer1)
	{
		TR1 = 0;		//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)		ET1 = 1;	//允许中断
		else									ET1 = 0;	//禁止中断
		if(TIMx->TIM_Polity == PolityHigh)		PT1 = 1;	//高优先级中断
		else									PT1 = 0;	//低优先级中断
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//错误
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//工作模式,0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~0x40;	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x40;	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x40;	//对外计数或分频
		else										TMOD &= ~0x40;	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x02;	//输出时钟
		else							INT_CLKO &= ~0x02;	//不输出时钟
		
		TH1 = (u8)(TIMx->TIM_Value >> 8);
		TL1 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR1 = 1;	//开始运行
		return	0;		//成功
	}

	if(TIM == Timer2)		//Timer2,固定为16位自动重装, 中断无优先级
	{
		AUXR &= ~(1<<4);	//停止计数
		if(TIMx->TIM_Interrupt == ENABLE)			IE2  |=  (1<<2);	//允许中断
		else										IE2  &= ~(1<<2);	//禁止中断
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~(1<<2);	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  (1<<2);	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	AUXR |=  (1<<3);	//对外计数或分频
		else										AUXR &= ~(1<<3);	//定时
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x04;	//输出时钟
		else							INT_CLKO &= ~0x04;	//不输出时钟

		TH2 = (u8)(TIMx->TIM_Value >> 8);
		TL2 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	AUXR |=  (1<<4);	//开始运行
		return	0;		//成功
	}
	return 2;	//错误
}
