#define _MEASURE_C
#include "head.h"
//#include "uart.h"

//
//ad_measure[]的变量约定
//0123，4个4-20MA的通道值，流量值在4，


//u16 Userdefined_Period = 625;
u16 Userdefined_Period3 = 250;//1s钟8次
void Updata_Lcd_YC(void);
void time_control(void)
{
	u8 i;
	if(measure_flag>=6)
	{
		ad_calucate();
		measure_flag=0;
		UpdataModbusYc();
		Updata_Lcd_YC();
	}
	else
	{ 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);
		i=10;
		while(--i);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
		ad_buf[measure_flag].ad_group[3]=ADC_GetConversionValue(ADC1);
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_28Cycles5);
		i=10;
		while(--i);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
		ad_buf[measure_flag].ad_group[2]=ADC_GetConversionValue(ADC1);
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_28Cycles5);
		i=10;
		while(--i);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
		ad_buf[measure_flag].ad_group[1]=ADC_GetConversionValue(ADC1);
		// printf("i=%d\r\n",ad_buf[measure_flag].ad_group[2]);
		
		
		
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_28Cycles5);
		i=10;
		while(--i);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
		ad_buf[measure_flag].ad_group[0]=ADC_GetConversionValue(ADC1);    
		measure_flag++;
	}
	
	
}


/*
AD计算函数
*/


void ad_calucate(void)
{
	u32 temp;
	u8 i,j;
    
	//算出4-20MA的当前值
	for(i=0;i<4;i++)
	{
		u32 temp_buf[6];
		u8 min_index,max_index;		
		for(max_index=0; max_index<6; max_index++)
		{
			temp_buf[max_index] = ad_buf[max_index].ad_group[i];
		}
		//舍弃最小值
		min_index = 0;
		for (max_index=0; max_index<6; max_index++)
		{
            if (temp_buf[min_index] >temp_buf[max_index])
            {
				min_index = max_index;
            }
		}
		temp_buf[min_index] = 0;
		//舍弃最大值
		min_index = 0;
		for (max_index=0; max_index<6; max_index++)
		{
            if (temp_buf[min_index] < temp_buf[max_index])
            {
				min_index = max_index;
            }
		}
		temp_buf[min_index] = 0;
		
		
		//求平均,AD转换出来的原始值。
		temp = 0;
		for (j=0; j<6; j++)
		{
			temp += temp_buf[j];
		}
		
		temp = temp/4;	
        
		
		//开始计算实际4-20MA电流值
		ad_measure[i].ad_samp=temp*336000/(4096*148);//单位为ma的100倍,测电压为3.36
		
        
		if(ad_measure[i].ad_samp >= 400)
		{
			ad_measure[i].ad_samp = ad_measure[i].ad_samp-400;
			
		}
		else
		{
			ad_measure[i].ad_samp = 0;
		}
		
	}
	
    //test.此处以后要做到根据调试标志处理，如果有调试标志时，便装载上位机下传的数据，达到模拟调试的目的。
	//for debug
	//     ad_measure[0].ad_samp = 150;	
	//     ad_measure[1].ad_samp = 1000;
	//     ad_measure[2].ad_samp = 800;
    //ad_measure[3].ad_samp = 1600;
    
	//把4-20MA进行系数转换。转换公式是，4-20MA=>0-??,如果是0-2000，则系数是2000/（2000-400）=
	ad_measure[0].ad_value = ad_measure[0].ad_samp * AD1_xishu;
	ad_measure[1].ad_value = ad_measure[1].ad_samp * AD2_xishu; 
	ad_measure[2].ad_value = ad_measure[2].ad_samp * AD3_xishu;
	ad_measure[3].ad_value = ad_measure[3].ad_samp * AD4_xishu;
    //按理应该缩小1600倍，但此时因为显示时会有一位小数点，所以少缩小10倍
    ad_measure[0].ad_value /= 160;
    ad_measure[1].ad_value /= 160;
    ad_measure[2].ad_value /= 160;
    ad_measure[3].ad_value /= 160;
	
	//------------------------------------------------------------------------------------------
	
	////hyman2011-4，计算流量
	// 方法：
	// 230个脉冲是一升水。5S累计的脉冲数推算出1分钟加入的水量（单位L/分），扩大了 1000 倍。(X/230 * 12 * 1000) => X*52)
	if(fulx_measure_flag==1)
	{
		fulx_measure_flag=0;
		yc_buf[ZJ_AI_LL] = pulse_minus*52;	        
		
	}
	//
	// 重新计算每个流量信号，需要加多少药。程序写死，值根据客户提供，特殊工程需要更改 CHANGE
	//yc_buf[26] = 172;//360*0.48;
	//yc_buf[27] = 91;//190*0.48;
    float temp2 = yc_buf[26];
	ll_single_jiayao3_value = temp2/(230 * 1000);
	temp2 = yc_buf[27];
	ll_single_jiayao4_value = temp2/(230 * 1000);
	    
}

void Updata_Lcd_YC(void)
{
// "1#主温度",
	// "1#主值",
	// "1#从温度",
	// "1#从值",   
    
	// "2#主温度",
	// "2#主值",
	// "2#从温度",
	// "2#从值",       
	
    // "2#再温度",
	// "2#再值", 
    // "3#再值",
    // "流量",
    
	// "3#主值",
    // "3#从值",

	lcd_disp_measure[0] = yc_buf[ZJ_AI_MST_1_WD];
	lcd_disp_measure[1] = yc_buf[ZJ_AI_MST_1_VAL];
	lcd_disp_measure[2] = yc_buf[ZJ_AI_SLV_1_WD];
	lcd_disp_measure[3] = yc_buf[ZJ_AI_SLV_1_VAL];

	lcd_disp_measure[4] = yc_buf[ZJ_AI_MST_2_WD];
	lcd_disp_measure[5] = yc_buf[ZJ_AI_MST_2_VAL];
	lcd_disp_measure[6] = yc_buf[ZJ_AI_SLV_2_WD];
	lcd_disp_measure[7] = yc_buf[ZJ_AI_SLV_2_VAL];
	
	lcd_disp_measure[8] = yc_buf[ZJ_AI_SLV2_2_WD];
	lcd_disp_measure[9] = yc_buf[ZJ_AI_SLV2_2_VAL];
	lcd_disp_measure[10] = ad_measure[1].ad_value / 10;
	lcd_disp_measure[11] = yc_buf[ZJ_AI_LL];

	lcd_disp_measure[8] = ad_measure[3].ad_value / 10;
    lcd_disp_measure[9] = ad_measure[2].ad_value / 10;	
}

void Adc_Init(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef   ADC_InitStructure;
    
	TIM_Configuration();
	//	DMA_InitTypeDef   DMA_InitStructure;
	/* Enable GPIOA clock                                                       */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/* Enable ADC1 clock                                                        */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	/* ADC1 Configuration (ADC1CLK = 18 MHz) -----------------------------------*/
	ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;//ADC_Mode_RegSimult;//工作在同步规则模式// ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode       = ENABLE;//扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//将4个通道扫描完成后停止转换，在相应函数中再次启动						  
	ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel       = 1;//规则通道转换序列总数
	ADC_Init(ADC1, &ADC_InitStructure);
	
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);//规则序列中第一个转换
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_28Cycles5);
	
	ADC_Cmd(ADC1, ENABLE);// 
	//ADC_ResetCalibration(ADC1);
	//while(ADC_GetResetCalibrationStatus(ADC1));           
	ADC_StartCalibration(ADC1);//xiaozhun 
	//while(ADC_GetCalibrationStatus(ADC1));                      
	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);/* Start ADC1 Software Conversion     */ 
    
    
    //hyman2011-04
    //把计量泵和流量计的相关数据清0或进行必要的初始化。
    jlb1_need_times = 0;  //计量泵动作次数
    jlb1_all_times = 0;  //计量泵动作次数
    jlb2_need_times = 0;  //计量泵动作次数
    jlb2_all_times = 0;  //计量泵动作次数
	jlb7_need_times = 0;  //计量泵动作次数
    jlb7_all_times = 0;  //计量泵动作次数
    
    jlb3_need_times = 0;  //计量泵动作次数
    jlb3_all_times = 0;  //计量泵动作次数
    jlb4_need_times = 0;  //计量泵动作次数
    jlb4_all_times = 0;  //计量泵动作次数
	jlb5_need_times = 0;  //计量泵动作次数
    jlb5_all_times = 0;  //计量泵动作次数
    liuliangjiMaster_all_times = 0;  //主流量计总接收次数
    liuliangjiMaster_all_water = 0;  //主所有的补水量


	ll_single_jiayao3_value = 0;
    ll_single_jiayao4_value = 0;

	F_Count=0;
	F_Count1=0;
    
}
void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; //自动装载寄存器的值         
	TIM_TimeBaseStructure.TIM_Prescaler = 0x8C9F;  //预分频器     
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* Output Compare Active Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;          
	//  TIM_OCInitStructure.TIM_Channel = TIM_Channel_1;          
	TIM_OCInitStructure.TIM_Pulse = PERIOD;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);//禁止预装载功能
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_CC1 , ENABLE); 
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);  
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;	  
	TIM_TimeBaseStructure.TIM_Prescaler = 0x8c9f;//用户自定义
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//TIM_PrescalerConfig(TIM3, 35, TIM_PSCReloadMode_Immediate); 
	
	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing; //
	//输出比较定时模式
	TIM_OCInitStructure.TIM_Pulse = Userdefined_Period3; //
	//用户定义定时的周期
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable); //关闭预载入寄存器
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE); // 打开捕获比较中断
	
	TIM_Cmd(TIM3, ENABLE);//使能	
}





// 根据流量脉冲，更新加药



                                                                     

