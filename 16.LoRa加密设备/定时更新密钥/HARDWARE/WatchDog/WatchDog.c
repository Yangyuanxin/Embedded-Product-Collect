#include "WatchDog.h"

u16 WatchDogTimer = 0x00;

/*********************************************
	* 功能描述：指示灯初始化程序
	* 入口参数：无
	* 返回值：	无
	* 备注：	初始化时拉低电平为点亮LED
**********************************************/
void WatchDogPinInit(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( WATCHDOG_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= WATCHDOG_PIN;			//LED0-->PB.5 端口配置
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;		//IO口速度为50MHz
	GPIO_Init(WATCHDOG_PORT, &GPIO_InitStructure);			//根据设定参数初始化GPIOA_0
}


void RunWatchDogTimer(void)
{
	if(WatchDogTimer<2000)
	{
		WatchDogTimer ++;
	}
}


/*******************************************
	* 功能描述：喂狗，检测主程序是否崩溃
	* 入口参数：无
	* 返回值：	无
	* 备注：	在1秒内改变1次电平
********************************************/
void WatchDogHandle(void)
{
	if( WatchDogTimer>FEED_DOG_CYCLE )
	{
		WatchDogTimer = 0x00;
		WATCH_DOG = !WATCH_DOG;
	}
}





