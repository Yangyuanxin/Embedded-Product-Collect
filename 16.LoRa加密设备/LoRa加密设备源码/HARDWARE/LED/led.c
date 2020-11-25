#include "led.h"


/*********************************************
	* 功能描述：指示灯初始化程序
	* 入口参数：无
	* 返回值：	无
	* 备注：	初始化时拉低电平为点亮LED
**********************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_1;			//LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;		//IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//根据设定参数初始化GPIOB.5
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2;			//LED0-->PB.5 端口配置
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//根据设定参数初始化GPIOB.5
	
	//打开指示灯
	RUN_LED_ON();	
	NET_LED_ON();
}


static u16 LED_Counter = 0x00;
//运行LED计数器
void RunLEDCounter(void)
{
	if(LED_Counter<LED_MAX_VALUE)
	{
		LED_Counter ++;
	}
}


/**************************************************
	* 功能描述：LED主处理程序
	* 入口参数：无
	* 返回值：	无
	* 备注：	通过更改宏LED_FLASH_FRE来
				改变LED到闪烁频率，默认为500ms
***************************************************/
void IndicatorLightMainHandle(void)
{
	if( LED_Counter<LED_FLASH_FRE )
		return;
	
	LED_Counter = 0x00;		//将计数器清零
	RUN_LED = !RUN_LED;		//翻转指示灯状态
}



