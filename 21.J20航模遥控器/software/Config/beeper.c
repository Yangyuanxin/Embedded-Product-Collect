#include "beeper.h"

//蜂鸣器IO初始化
void BEEPER_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PC端口时钟

    //初始化 蜂鸣器引脚PA10	  推挽输出
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA10
    GPIO_ResetBits(GPIOA, GPIO_Pin_10); //输出低电平
}
//蜂鸣器短响一次
//freq：频率1~50000
//void beeperOnce(u16 freq)
//{
//	for (u16 i=0; i<(u16)freq/20; i++) {
//		Beeper = 1;
//		delay_us((u16)50000/freq);
//		Beeper = 0;
//		delay_us((u16)50000/freq);
//	}
//}
void beeperOnce(u16 nms)
{
    Beeper = 1;
    delay_ms(nms);
    Beeper = 0;
}
//检测蜂鸣器模式后再响
void keyDownSound(void)
{
    if (setData.keySound == ON)
    {
        beeperOnce(6);
    }
}
