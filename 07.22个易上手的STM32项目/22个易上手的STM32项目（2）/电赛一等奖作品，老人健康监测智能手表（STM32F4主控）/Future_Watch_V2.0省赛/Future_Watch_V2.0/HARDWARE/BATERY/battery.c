#include "battery.h"


//用来给寄存Adc的值
u16 AdcDma_Buf;//用来存放DMA的值

//初始化ADC
//这里我们仅以规则通道为例
//我们默认仅开启ADC1_CH5																	   
void  Adc_Init(void)
{    
	//先初始化IO口
 	RCC->APB2ENR|=1<<8;    	//使能ADC1时钟 
	RCC->AHB1ENR|=1<<2;    	//使能PORTA时钟	  
	GPIO_Set(GPIOC,PIN0,GPIO_MODE_AIN,0,0,GPIO_PUPD_PU);	//PA5,模拟输入,下拉   

	RCC->APB2RSTR|=1<<8;   	//ADCs复位
	RCC->APB2RSTR&=~(1<<8);	//复位结束	 
	ADC->CCR=3<<16;			//ADCCLK 时钟来源为PLL 168M，8分频21MHz
							//ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
 	
	ADC1->CR1=0;   			//CR1设置清零
	ADC1->CR2=0;   			//CR2设置清零
	
	ADC1->CR1|=0<<24;      	//12位模式
	ADC1->CR1|=0<<8;    	//非扫描模式	
	
	ADC1->CR2|=1<<8;		//使能DMA转化使能
	ADC1->CR2|=1<<9;		//DDS即每次转换完成后当DMA=1时，发出DMA请求
	
	ADC1->CR2|=(1<<1);    	//连续转换模式 23.4us一次
 	ADC1->CR2&=~(1<<11);   	//右对齐	
	ADC1->CR2|=0<<28;    	//软件触发 使能后，由于是连续转换模式所以连续转换
	
	
	
//	ADC1->SQR1&=~(0XF<<20);
//	ADC1->SQR1|=0<<20;     		//1个转换在规则序列中 也就是只转换规则序列1 			   
	 
	//设置通道5的采样时间  23.4us一次 Time = (采样时间+12)*(1/ADCCLK) 
	ADC1->SMPR1&=~(7<<(3*0));	//通道10采样时间清空	  
 	ADC1->SMPR1|=7<<(3*0); 		//通道10  480个周期,提高采样时间可以提高精确度	 
 	ADC1->CR2|=1<<0;	   		//开启AD转换器	
	
	ADC1->SQR3&=0XFFFFFFE0;	//规则序列1 通道ch
	ADC1->SQR3|=10;		  			    
	ADC1->CR2|=1<<30;       //启动规则转换通道 

}				  
//获得ADC值
//ch:通道值 0~16
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
	//设置转换序列	  		 
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=10;		  			    
	ADC1->CR2|=1<<30;       //启动规则转换通道 
	while(!(ADC1->SR&1<<1));//等待转换结束	 	   
	return ADC1->DR;		//返回adc值	
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u16 adc[],u8 times)
{
	u8 i,j;
	u16 adc_buf;
	for(i=0;i<(times-1);i++)
	{
		for(j=0;j<times-i;j++)
		{
			if(adc[j]>adc[j+1])
			{
				adc_buf = adc[j+1];
				adc[j+1]= adc[j];
				adc[j] = adc_buf;
			}
		}
	}
	adc_buf=0;
	for(i=1;(i<times-1);i++)
	{
		adc_buf += adc[i];
	}
	adc_buf = adc_buf/(times-2);
	return adc_buf;
}  
