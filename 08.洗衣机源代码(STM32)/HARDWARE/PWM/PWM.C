#include"PWM.H"
#include"BEEP.H"



void PWM_Init()
{

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t rescalerValue = 0;
	#ifdef CH1_ENABLE
		uint16_t CCR1_Val = 333;
	#endif	
	#ifdef CH2_ENABLE
		uint16_t CCR2_Val = 333;
	#endif	
	#ifdef CH3_ENABLE
		uint16_t CCR3_Val = 166;
	#endif	
	#ifdef CH4_ENABLE
		uint16_t CCR4_Val = 83;
	#endif	
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | 
                          RCC_APB2Periph_AFIO, ENABLE);


 /* GPIOA Configuration:TIM3 Channel1, 2, 3 and 4 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);


  /* Compute the prescaler value */
  rescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = PWM_ARR_REG;
  TIM_TimeBaseStructure.TIM_Prescaler = rescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

#ifdef CH1_ENABLE
  /* WM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	
	
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
//  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	

#endif

#ifdef CH2_ENABLE
  /* WM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif

#ifdef CH3_ENABLE
  /* WM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif

#ifdef CH4_ENABLE
  /* WM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
#endif
	/*使能ARR重载*/
  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
}


#ifdef CH1_ENABLE
void PWM_SetCH1_dura(u8 rate)
{
	u16 camp=0;
	camp=(u16)(PWM_ARR_REG*(rate/100.0));
	TIM_SetCompare1(TIM3,camp);
}
#endif
#ifdef CH2_ENABLE
void PWM_SetCH2_dura(u8 rate)
{
	u16 camp=0;
	camp=(u16)(PWM_ARR_REG*(rate/100.0));
	TIM_SetCompare2(TIM3,camp);
}
#endif
#ifdef CH3_ENABLE
void PWM_SetCH3_dura(u8 rate)
{
	u16 camp=0;
	camp=(u16)(PWM_ARR_REG*(rate/100.0));
	TIM_SetCompare3(TIM3,camp);
}
#endif 
#ifdef CH4_ENABLE
void PWM_SetCH4_dura(u8 rate)
{
	u16 camp=0;
	camp=(u16)(PWM_ARR_REG*(rate/100.0));
	TIM_SetCompare4(TIM3,camp);
}
#endif





void zhengzhuan(void)
{
	PWM_SetCH1_dura(30);
	PWM_SetCH2_dura(100);
}

void zhengzhuan2(void)
{
	PWM_SetCH1_dura(10);
	PWM_SetCH2_dura(100);
}

void fanzhuan(void)
{
	PWM_SetCH1_dura(100);
	PWM_SetCH2_dura(30);
}

void tingzhi(void)
{
	PWM_SetCH1_dura(0);
	PWM_SetCH2_dura(0); 
}
