#include "ppm.h"

u16 PPM_Array[PPM_NUM] = {MS05, 1000, MS05, 1000, MS05, 500, MS05, 1000, MS05, 1000, MS05, 1000, MS05, 1000, MS05, 1000, MS05, 8000};
u16 PPM_Index = 0;//PPM数组索引号

/*TIM4的中断初始化*/
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //TIM4的中断号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级，如果不设置为最高优先级，就会乱码
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器
}

/*PPM引脚初始化*/
static void PPM_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //使能GPIO外设时钟
    //设置PPM引脚为推挽输出功能,输出脉冲波形
    GPIO_InitStructure.GPIO_Pin = PPM_Pin; //PPM引脚号
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_Init(PPM_GPIO_Port, &GPIO_InitStructure);//初始化PPM端口
}
/*TIM4初始化*/
void PPM_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);    //使能TIM4时钟

    TIM_TimeBaseStructure.TIM_Period = 500; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 71; //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//允许 更新 产生中断

    PPM_GPIO_Configuration(); //GPIO初始化
    NVIC_Configuration(); //中断初始化

    if(setData.PPM_Out == ON) TIM_Cmd(TIM4, ENABLE); //使能TIM4
    else TIM_Cmd(TIM4, DISABLE); //不使能TIM4

    PPM = 0;//前500us输出低电平
}

/*TIM4中断服务子程序*/
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清除标志位
        TIM4->ARR = PPM_Array[PPM_Index] - 1; //更新TIM4的自动重装载值，减1是给后几行程序留时间
        PPM = !PPM;//更改电平
        PPM_Index++;

        if(PPM_Index >= PPM_NUM)
        {
            PPM_Index = 0;
        }
    }
}

