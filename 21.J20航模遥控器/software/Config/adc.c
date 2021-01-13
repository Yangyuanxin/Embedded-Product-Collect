#include "adc.h"
#include "main.h"
u16 volatile chValue[adcNum * sampleNum]; //ADC采样值*10
u16 volatile chResult[chNum];//滤波后的ADC采样值
u16 volatile PWMvalue[chNum];//控制PWM占空比

float volatile batVolt;//电池电压
u8 volatile batVoltSignal = 0; //是否报警，1为报警，0为正常

#define ADC1_DR_Address    ((u32)0x4001244C)		//ADC1的地址
//通用定时器2中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用TIM2_CH2控制ADC1定时采样，见STM32中文参考手册P156
void TIM2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 		//时钟使能

    //定时器TIM2初始化
    TIM_TimeBaseStructure.TIM_Period = arr; 		//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc; 			//设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//根据指定的参数初始化TIMx的时间基数单位

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 9;							//计数达到9产生中断
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
    TIM_OC2Init(TIM2, & TIM_OCInitStructure);		//初始化外设TIM2_CH2

    TIM_Cmd(TIM2, ENABLE); 			//使能TIMx
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
}


//DMA1配置
void DMA1_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	  			//使能ADC1通道时钟

    //DMA1初始化
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;		//指定DMA1的外设地址-ADC1地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&chValue; 		//chValue的内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 				//方向(从外设到内存)
    DMA_InitStructure.DMA_BufferSize = adcNum * sampleNum; 				//DMA缓存大小，存放90次采样值
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	//外设地址固定，接收一次数据后，设备地址禁止后移
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//内存地址不固定，接收多次数据后，目标内存地址后移
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ; //外设数据单位，定义外设数据宽度为16位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;    //内存数据单位，HalfWord就是为16位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; 		//DMA模式：循环传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High ; 	//DMA优先级：高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   		//禁止内存到内存的传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //配置DMA1

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);		//使能传输完成中断

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
}

//DMA中断处理函数
void  DMA1_Channel1_IRQHandler(void)
{
    u16 chIndex;
    u16 PWM_Sum = 0;//8个PWM通道值的和

    if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
    {

        //中断处理代码
        //通道映射,判断设置的左/右手油门
        for(chIndex = 0; chIndex < 4; chIndex++)
        {
            if(setData.throttlePreference) chResult[chIndex] = GetMedianNum(chValue, chIndex);
            else chResult[chIndex] = GetMedianNum(chValue, 3 - chIndex);
        }

        for(chIndex = 4; chIndex < chNum; chIndex++)
        {
            chResult[chIndex] = GetMedianNum(chValue, chIndex); //后4个通道映射
        }

        //数值映射
        for(chIndex = 0; chIndex < chNum; chIndex++)
        {
            PWMvalue[chIndex] = setData.PWMadjustValue[chIndex] + mapChValue(chResult[chIndex],
                                setData.chLower[chIndex],
                                setData.chMiddle[chIndex],
                                setData.chUpper[chIndex],
                                setData.chReverse[chIndex]);
            PPM_Array[chIndex * 2 + 1] = PWMvalue[chIndex] - MS05;
            PWM_Sum += PWMvalue[chIndex];
        }

        PPM_Array[PPM_NUM - 1] = MS20 - PWM_Sum;
        sendDataPacket();//发送数据包,采集完即发送到接收机

        batVolt = GetMedianNum(chValue, 8) * 3.3 * 3 * setData.batVoltAdjust / 4095000; //电池电压采样

        if(batVolt < setData.warnBatVolt) batVoltSignal = 1;// 报警信号
        else batVoltSignal = 0;

        DMA_ClearITPendingBit(DMA1_IT_TC1);//清除标志
    }
}
//GPIO配置模拟输入，PA0-7,PB0
void GPIOA_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);	 //使能GPIOA B时钟

    //PA0-7 PB0作为模拟通道输入引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//初始化ADC
void  ADC_Pin_Init(void)
{
    GPIOA_Init();
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟

    //ADC1初始化
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 			//独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;  				//开启扫描方式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//开启连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   	//使用外部触发模式ADC_ExternalTrigConvEdge_None
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 		//采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = adcNum; 			//要转换的通道数目
    ADC_Init(ADC1, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);				//配置ADC时钟，为PCLK2的6分频，即12MHz
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);		//配置ADC1通道0为239.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);		//配置ADC1通道1为239.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5);		//配置ADC1通道2为239.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5);		//配置ADC1通道3为239.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_239Cycles5);		//配置ADC1通道4为239.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_239Cycles5);		//配置ADC1通道5为239.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_239Cycles5);		//配置ADC1通道6为239.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_239Cycles5);		//配置ADC1通道7为239.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_239Cycles5);		//配置ADC1通道8为239.5个采样周期


    ADC_DMACmd(ADC1, ENABLE); //ADC向DMA发出请求，请求DMA传输数据
    ADC_Cmd(ADC1, ENABLE); //使能ADC

    ADC_ResetCalibration(ADC1);				//复位校准寄存器

    while(ADC_GetResetCalibrationStatus(ADC1));	//等待校准寄存器复位完成

    ADC_StartCalibration(ADC1);				//ADC校准

    while(ADC_GetCalibrationStatus(ADC1));	//等待校准完成

    ADC_ExternalTrigConvCmd(ADC1, ENABLE);	//设置外部触发模式使能
}

//获得ADC值，此函数未使用
//ch:通道值 0~8
u16 Get_Adc(u8 ch)
{
    //设置指定ADC的规则组通道，一个序列，采样时间
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5个周期
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

    return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

//ch:通道值 0~8，采样times次后作均值滤波，此函数未使用
u16 Get_Adc_Average(u8 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;

    for(t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ch);
        delay_ms(5);
    }

    return temp_val / times;
}


/*函数：float map(float value,float fromLow,float fromHigh,float toLow,float toHigh)
* 说明：仿Arduino,将一个数字从一个范围重新映射到另一个范围
		也就是说，fromLow的值将映射到toLow，fromlhigh到toHigh的值等等。
* 参数：value：待映射的数值；
		fromLow：原范围的最小值
		fromHigh：原范围的最大值
		toLow：要转换的范围的最小值
		toHigh：要转换的范围的最大值
* 返回：转换后的数值
*/
float map(float value, float fromLow, float fromHigh, float toLow, float toHigh)
{
    return ((value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow);
}

/*函数：int mapChValue(int val, int lower, int middle, int upper, int reverse)
* 说明：将ADC获取的采样值转换到1000~2000，lower~middle~upper适配遥杆的范围，
		并加入正反开关的控制。
* 参数：val：该通道ADC当前采样值；
		lower：该通道遥杆最低位置时的ADC采样值；
		middle：该通道遥杆回中时的ADC采样值；
		upper：该通道遥杆最高位置时的ADC采样值；
		reverse：该通道正反开关状态，1为正常，0为反转
* 返回：该通道变换后的值(1000~2000)
*/
int mapChValue(int val, int lower, int middle, int upper, int reverse)
{
    if(val > upper) val = upper;

    if(val < lower) val = lower; //将val限制在lower~upper范围内

    if ( val < middle )
    {
        val = (int)map(val, lower, middle, 1000, 1500);
    }
    else
    {
        val = (int)map(val, middle, upper, 1500, 2000);
    }

    return ( reverse ? 3000 - val : val );
}
/*函数：int GetMedianNum(volatile u16 * bArray, int ch)
* 说明：对数组中某个通道的采样值进行中值滤波，使采样值更稳定
* 参数：bArray：待滤波的数组；
		ch：采样通道0~adcNum-1；
* 返回：该通道的采样值的中值
*/
int GetMedianNum(volatile u16 * bArray, int ch)
{
    int i, j; // 循环变量
    int bTemp;
    u16 tempArray[sampleNum];

    for(i = 0; i < sampleNum; i++)
    {
        tempArray[i] = bArray[ch + adcNum * i];
    }

    // 用冒泡法对数组进行排序
    for (j = 0; j < sampleNum - 1; j ++)
    {
        for (i = 0; i < sampleNum - j - 1; i ++)
        {
            if (tempArray[i] > tempArray[i + 1])
            {
                // 互换
                bTemp = tempArray[i];
                tempArray[i] = tempArray[i + 1];
                tempArray[i + 1] = bTemp;
            }
        }
    }

    // 计算中值
    if ((sampleNum & 1) > 0)
    {
        // 数组有奇数个元素，返回中间一个元素
        bTemp = tempArray[(sampleNum + 1) / 2];
    }
    else
    {
        // 数组有偶数个元素，返回中间两个元素平均值
        bTemp = (tempArray[sampleNum / 2] + tempArray[sampleNum / 2 + 1]) / 2;
    }

    return bTemp;
}


