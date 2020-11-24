#include "backlit.h"


void TIM3_PWM_Init(u32 arr,u32 psc)
{
	RCC->APB1ENR|=1<<1; 	//TIM3时钟使能    
	RCC->AHB1ENR|=1<<1;   	//使能PORTB时钟	
	GPIO_Set(GPIOB,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOB,0,2);	//PB0,AF2 
	
	TIM3->ARR=arr;			//设定计数器自动重装值 
	TIM3->PSC=psc;			//预分频器不分频 
	TIM3->CCMR2|=6<<4;  	//CH3 PWM1模式		 
	TIM3->CCMR2|=1<<3; 		//CH3 预装载使能	   
	TIM3->CCER|=1<<8;   	//OC1 输出使能	
	TIM3->CCER &= ~(1<<9);   	//OC1 低电平有效	   
	TIM3->CR1|=1<<7;   		//ARPE使能 
	TIM3->CR1|=1<<0;    	//使能定时器3
}



//初始化ADC3
//这里我们仅以规则通道为例
//我们默认仅开启ADC3_CH5																	   
void  Adc3_Init(void)
{    
	//先初始化IO口
 	RCC->APB2ENR|=1<<10;   	//使能ADC3时钟  
	RCC->APB2RSTR|=1<<8;   	//ADCs复位(注意,所有ADC都会被复位.)
	RCC->APB2RSTR&=~(1<<8);	//复位结束	 
	ADC->CCR=1<<16;			//ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
 	 
	ADC3->CR1=0;   			//CR1设置清零
	ADC3->CR2=0;   			//CR2设置清零
	ADC3->CR1|=0<<24;      	//12位模式
	ADC3->CR1|=0<<8;    	//非扫描模式	
	
	ADC3->CR2&=~(1<<1);    	//单次转换模式
 	ADC3->CR2&=~(1<<11);   	//右对齐	
	ADC3->CR2|=0<<28;    	//软件触发
	
	ADC3->SQR1&=~(0XF<<20);
	ADC3->SQR1|=0<<20;     	//1个转换在规则序列中 也就是只转换规则序列1 			   
	//设置通道5的采样时间
	ADC3->SMPR1&=~(7<<(3*1));//通道11采样时间清空	  
 	ADC3->SMPR1|=7<<(3*1); 	//通道11  480个周期,提高采样时间可以提高精确度	
 	ADC3->CR2|=1<<0;	   	//开启AD转换器	  
}	 
//获得ADC值
//ch:通道值 0~16
//返回值:转换结果
u16 Get_Adc3(u8 ch)   
{
	//设置转换序列	  		 
	ADC3->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC3->SQR3|=ch;		  			    
	ADC3->CR2|=1<<30;       //启动规则转换通道 
	while(!(ADC3->SR&1<<1));//等待转换结束	 	   
	return ADC3->DR;		//返回adc值	
} 
 
 
//初始化光敏传感器
void Lsens_Init(void)
{
	RCC->AHB1ENR|=1<<2;    	//使能PORTC时钟	  
	GPIO_Set(GPIOC,PIN1,GPIO_MODE_AIN,0,0,GPIO_PUPD_PU);//PF7,模拟输入,下拉   
	Adc3_Init();
}
//读取Light Sens的值
//0~100:0,最暗;100,最亮 
u8 Lsens_Get_Val(void)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<LSENS_READ_TIMES;t++)
	{
		temp_val+=Get_Adc3(LSENS_ADC_CHX);	//读取ADC值
		delay_ms(5);
	}
	temp_val/=LSENS_READ_TIMES;//得到平均值 
	if(temp_val>4000)temp_val=4000;
	return (u8)(100-(temp_val/40));
}

