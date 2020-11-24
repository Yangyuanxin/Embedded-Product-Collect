#include "key.h"
#include "delay.h" 	
#include "usart.h"
#include "RS485.h"

uint8_t KeyStatus;

//按键初始化函数
void KEY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOA,GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //KEY对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOB0
} 
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
//	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1))
	if(key_up && (KEY == 0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY == 0)		return 1;
//		if(KEY0==0)return 1;
//		else if(KEY1==0)return 2;
//		else if(KEY2==0)return 3;
//		else if(WK_UP==1)return 4;
	}
//	else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)
	else if(KEY == 1)
	{
		key_up=1; 
	}	
	
 	return 0;// 无按键按下
}
void Key_Process(void)
{
		KeyStatus = PBin(0);//KEY_Scan(0);
	
		if(KeyStatus == 0)		//KEY1按下,门禁报警使能，端口后，报警信号恢复
		{
			u8_AlarmStatus |= 0x40;
		}
		else
		{
			u8_AlarmStatus &= ~0x40;
		}
}



















