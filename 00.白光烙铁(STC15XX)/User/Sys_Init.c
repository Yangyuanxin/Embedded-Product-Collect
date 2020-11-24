/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     除注明出处外，以下所有内容版权均属王志浩所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留作者的版权声明。
 *
 * @file			系统初始化
 * @brief			
 * @author		王志浩
 * @version		V2.2
 * @date			2017-8-30
 */
#include "config.h"

/************************定义全局变量*****************************/
void GPIO_Init(void)
{
	//结构定义
	GPIO_InitTypeDef	GPIO_InitStructure;		
	
	//配置P01（sleep）,P03（beep）为上拉
	GPIO_InitStructure.Pin  = GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);

	
	//配置P20~P24（OLED屏）为上拉
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);
	
	//配置P30~P32（EC11编码器）为开漏
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	
	//配置P13、P15、P17（ADC）为高阻
	GPIO_InitStructure.Pin  = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);
	
	//配置P10（PWM）为推挽
	GPIO_InitStructure.Pin  = GPIO_Pin_0;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);

}

void ADC_Init(void)
{
	//结构定义
	ADC_InitTypeDef		ADC_InitStructure;		
	
	ADC_InitStructure.ADC_Px        = ADC_P13|ADC_P15|ADC_P17;	
	ADC_InitStructure.ADC_Speed     = ADC_180T;				//ADC速度			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;					//ADC功率			ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H2L8;		//ADC结果调整,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;			//优先级设置	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;				//中断允许		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);									//初始化
	ADC_PowerControl(ENABLE);													//单独的ADC电源操作函数, ENABLE或DISABLE
}

void Iron_Init(void)
{
	//结构定义
	PCA_InitTypeDef		PCA_InitStructure;

	PCA_InitStructure.PCA_Clock    = PCA_Clock_Timer0_OF;		//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
	PCA_InitStructure.PCA_IoUse    = PCA_P12_P11_P10_P37;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//ENABLE, DISABLE
	PCA_InitStructure.PCA_Polity   = PolityLow;			//优先级设置	PolityHigh,PolityLow
	PCA_Init(PCA_Counter,&PCA_InitStructure);

	PCA_InitStructure.PCA_Mode     = PCA_Mode_PWM;		//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_PWM_Wide = PCA_PWM_8bit;		//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	PCA_InitStructure.PCA_Value    = 128 << 8;			//对于PWM,高8位为PWM占空比
	PCA_Init(PCA1,&PCA_InitStructure);
	
	CR = 1;
}
void Iron_Run()
{
	AUXR1 = (AUXR1 & ~(3<<4)) | (0<<4);			//其实是把输出口转移了，我不知道怎么关
}


void Iron_Off(void)
{
	AUXR1 = (AUXR1 & ~(3<<4)) | (2<<4);			//其实是把输出口转移了，我不知道怎么关
	P10 = 0;
}
void EXTI_Init(void)
{
	EXTI_InitTypeDef	EXTI_InitStructure;					//结构定义
	
	//编码器的按键配置为高优先级中断
	EXTI_InitStructure.EXTI_Mode = EXT_MODE_Fall;
	EXTI_InitStructure.EXTI_Polity = PolityLow;
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;
	Ext_Inilize(EXT_INT4,&EXTI_InitStructure);

}

void Timer_Init(void)
{
	//结构定义
	TIM_InitTypeDef		TIM_InitStructure;	
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 0xFF9C;				//初值,50us
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 0xA240;				//初值,1ms
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 0x3CB0;				//初值,25ms
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2
}

void ParaInit(void)
{
	unsigned char temp[5],i;
	
	//先读EEPROM前五位
	EEPROM_read_n(0x0,(unsigned char *)&temp,sizeof(temp));
	for(i=0;i<5;i++)
	{
		temp[0] &= temp[i];
	}
	
	//如果都是OXFF，那么重新刷EEPROM，这样的话下载的时候只需要勾选本次将EEPROM全部填充为FF的话，就可以实现重新刷EEPROM
	if(temp[0] == 0xff)
	{
		EEPROM_SectorErase(0x0);
		EEPROM_SectorErase(0x200);
		EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
		EEPROM_write_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));
	}
	
	EEPROM_read_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
	EEPROM_read_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));
}

void Sys_Init(void)

{
	//顾名思义，就不一一写了
	EA = 0;
	GPIO_Init();
	BEEP_ON;
	OLED_Init();
	Draw_BMP(Logo);
	ADC_Init();
	Iron_Init();
	EXTI_Init();
	Timer_Init();
	ParaInit();
	//KalmanFilterInit();
	BEEP_OFF;
	Delay_ms(200);Delay_ms(200);
	Delay_ms(200);Delay_ms(200);
	Delay_ms(200);Delay_ms(200);
	OLED_CLS();
	//如果默认温度不为0，则将默认温度设定为开机温度
	if(MENU_DATA_Table.Temp_s.Default_Temp != 0)
	{
		IronTemp_AM = MENU_DATA_Table.Temp_s.Default_Temp;
	}
	if(MENU_DATA_Table.Temp_s.Default_CH != 0)
	{
		chn = MENU_DATA_Table.Temp_s.Default_CH;
	}
	//如果默认温度为0，则将上次温度设定为开机温度
	if(MENU_DATA_Table.Temp_s.Default_Temp == 0)
	{
		IronTemp_AM = MENU_DATA_Table.Temp_s.Last_Temp;
	}
	if(MENU_DATA_Table.Temp_s.Default_CH == 0)
	{
		chn = MENU_DATA_Table.Temp_s.Last_CH;
	}
	EA = 1;
}