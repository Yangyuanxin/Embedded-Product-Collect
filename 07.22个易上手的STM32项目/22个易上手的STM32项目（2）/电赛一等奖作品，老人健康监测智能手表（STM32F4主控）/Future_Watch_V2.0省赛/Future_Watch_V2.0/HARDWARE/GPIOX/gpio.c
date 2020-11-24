#include "gpio.h"
/*
* 函数名称：GPIO_Init；
* 功能说明：GPIO初始化；
* 入口参数：IO：哪个端口
*           PORT_NUM：端口位子
*           MODE：端口模式
*           SPEED：端口速度
*			OTY	 :推挽或开漏
*			PUPD：上拉或者下拉
*           IO_DR：端口电平
* 返回值  ：无；
* 备注    ：无；
*/
void GPIO_Init(GPIO_TypeDef * IO,u8 PORT_NUM,u8 MODE,u8 SPEED,u8 OTY,u8 PUPD,u8 IO_DR)
{
	//端口模式设置
	IO->MODER &= (~(1<<(PORT_NUM*2))) & (~(1<<(PORT_NUM*2+1)));		//清除对应位
	IO->MODER |= (MODE<<(PORT_NUM*2));							//输入or输出设置
	//端口速度设置
	IO->OSPEEDR &= (~(1<<(PORT_NUM*2))) & (~(1<<(PORT_NUM*2+1)));		//清除对应位
	IO->OSPEEDR |= (SPEED<<(PORT_NUM*2));					//设置速度
	//设置推挽或者开漏
	if(MODE == GPIO_MODE_OUT)
	{
		if(OTY)
		{
			IO->OTYPER |= (1<<PORT_NUM);		//开漏
		}
		else
		{
			IO->OTYPER &= ~(1<<PORT_NUM);		//推挽
		}
		if(IO_DR)
		{
			IO->ODR |= (1<<PORT_NUM);	//置位
		}
		else
		{
			IO->ODR &= ~(1<<PORT_NUM);//清0
		}
	}
	else
	{
		if(IO_DR)
		{
			IO->IDR |= (1<<PORT_NUM);	//置位
		}
		else
		{
			IO->IDR &= ~(1<<PORT_NUM);//清0
		}
	}
	//设置上拉或者下拉
	IO->PUPDR &= (~(1<<(PORT_NUM*2))) & (~(1<<(PORT_NUM*2+1)));		//清除对应位
	IO->PUPDR |= (PUPD<<(PORT_NUM*2));

}

/*
* 函数名称：GPIO_Clock_Set；
* 功能说明：GPIO时钟使能；
* 入口参数：IOPxEN->IOPAEN--IOPEEN,端口时钟；
*           EN_PULL_OR_OUT-->ODR设置；
* 返回值  ：无；
* 备注    ：无；
*/
void GPIO_Clock_Set(u8 IOPxEN)
{
	RCC->AHB1ENR|=(1<<IOPxEN);                       //开启端口时钟；
}



