#include "relay_init.h"

u16 RelayCounter = 0x0;


void RelayInit(void)
{	
	GPIO_InitTypeDef	GPIO_InitStructure;					//定义I/O口结构体
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 	//开启GPIOB的外设时钟
													   
	GPIO_InitStructure.GPIO_Pin 	= 	GPIO_Pin_13|
										GPIO_Pin_14;		//选择要控制的GPIOB引脚
	GPIO_InitStructure.GPIO_Mode 	= 	GPIO_Mode_Out_PP;  	//设置引脚模式为通用推挽输出 
	GPIO_InitStructure.GPIO_Speed 	= 	GPIO_Speed_50MHz; 	//设置引脚速率为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//根据指定参数初始化GPIO	  
	
	//关继电器
	RelayP = 0;	//继电器正开关
	RelayN = 0;	//继电器负开关
}


/**********************************************
	* 功能描述：运行继电器计时器
	* 入口参数：无
	* 返回值：	无
	* 备注：	无
***********************************************/
void RunRelayCounter(void)
{
	if(RelayCounter<2000)
	{
		RelayCounter ++;
	}
}


