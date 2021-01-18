#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "usart.h"
#include "setting.h"
#include "menu.h"
#include "oled.h"
#include "main.h"
//参考链接https://blog.csdn.net/qq_42679566/article/details/105892105，原文错误已修正

Key_Config Key_Buf[KEY_NUM];	// 创建按键数组
u16 volatile menuEvent[4];


#define KEY_LONG_DOWN_DELAY 30 	// 设置30个TIM3定时器中断=600ms算长按	
#define DBGMCU_CR  (*((volatile u32 *)0xE0042004))
	
/*通用定时器3中断初始化，使用TIM3控制按键定时检测
  时钟选择为APB1的2倍，而APB1为36M
* 参数：arr：自动重装值。
		psc：时钟预分频数
*/
void TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 		//时钟使能

	TIM_TimeBaseInitStructure.TIM_Period = arr; //自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //预分频值
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; // 向上计数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; //时钟分割为0,仍然使用72MHz
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//允许更新中断
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
	
	TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)   //TIM3中断服务函数
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		// 中断处理代码
		ReadKeyStatus();  //调用状态机
		volatile u8 i,status;
		for(i = 0;i < KEY_NUM;i++)
    	{
			status = Key_Buf[i].Status.KEY_EVENT;
			if(status==KEY_DOWN)
			{
				menuEvent[0]=1;//菜单事件
				menuEvent[1]=KEY_enter; //旋转编码器短按
			}
			if(status==KEY_LONG)
			{
				menuEvent[0]=1;//菜单事件
				menuEvent[1]=KEY_home; //旋转编码器长按home
			}
//			if(status!=KEY_NULL) printf("%d,%d\n",i,status);
		}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志 
	}
}

//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
	encoder_Init();//编码器引脚初始化
	Key_Init KeyInit[KEY_NUM]=
	{ 
		{GPIO_Mode_IPU, GPIOB, GPIO_Pin_1, RCC_APB2Periph_GPIOB}, 	// 初始化旋转编码器SW-PB1
	};
	Creat_Key(KeyInit); // 调用按键初始化函数
	
	//STM32没有彻底释放PB3作为普通IO口使用，切换到SW调试可释放PB3、PB4、PA15
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	DBGMCU_CR &=0xFFFFFFDF;  //如果没有这段代码，PB3就会一直是低电平
}

static BitAction KEY_ReadPin(Key_Init Key) //按键读取函数
{
  return (BitAction)GPIO_ReadInputDataBit(Key.GPIOx,Key.GPIO_Pin_x);
}

void Creat_Key(Key_Init* Init)
{
	uint8_t i; 
	GPIO_InitTypeDef  GPIO_InitStructure[KEY_NUM];
  	for(i = 0;i < KEY_NUM;i++)
	{
		Key_Buf[i].Key = Init[i]; // 按钮对象的初始化属性赋值
		RCC_APB2PeriphClockCmd(Key_Buf[i].Key.RCC_APB2Periph_GPIOx, ENABLE);//使能相应时钟
		GPIO_InitStructure[i].GPIO_Pin = Key_Buf[i].Key.GPIO_Pin_x;	//设定引脚			
		GPIO_InitStructure[i].GPIO_Mode = Key_Buf[i].Key.GPIO_Mode; 	//设定模式		
		GPIO_Init(Key_Buf[i].Key.GPIOx, &GPIO_InitStructure[i]);       //初始化引脚
		// 初始化按钮对象的状态机属性
		Key_Buf[i].Status.KEY_SHIELD = ENABLE;
		Key_Buf[i].Status.KEY_COUNT = 0;
		Key_Buf[i].Status.KEY_LEVEL = Bit_RESET;
		if(Key_Buf[i].Key.GPIO_Mode == GPIO_Mode_IPU) // 根据模式进行赋值
			Key_Buf[i].Status.KEY_DOWN_LEVEL = Bit_RESET;
		else
			Key_Buf[i].Status.KEY_DOWN_LEVEL = Bit_SET;
		Key_Buf[i].Status.KEY_STATUS = KEY_NULL;
		Key_Buf[i].Status.KEY_EVENT = KEY_NULL;
		Key_Buf[i].Status.READ_PIN = KEY_ReadPin;	//赋值按键读取函数
	}
}

static void Get_Key_Level(void) // 根据实际按下按钮的电平去把它换算成虚拟的结果
{
    uint8_t i;
    
    for(i = 0;i < KEY_NUM;i++)
    {
        if(Key_Buf[i].Status.KEY_SHIELD == DISABLE)
            continue;
        if(Key_Buf[i].Status.READ_PIN(Key_Buf[i].Key) == Key_Buf[i].Status.KEY_DOWN_LEVEL)
            Key_Buf[i].Status.KEY_LEVEL = Bit_SET;
        else
            Key_Buf[i].Status.KEY_LEVEL = Bit_RESET;
    }
}

void ReadKeyStatus(void)
{
    uint8_t i;
	
    Get_Key_Level();
	
    for(i = 0;i < KEY_NUM;i++)
    {
        switch(Key_Buf[i].Status.KEY_STATUS)
        {
            //状态0：没有按键按下
            case KEY_NULL:
                if(Key_Buf[i].Status.KEY_LEVEL == Bit_SET)//有按键按下
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_SURE;//转入状态1
					Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//空事件
                }
                else
                {
                    Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//空事件
                }
                break;
            //状态1：按键按下确认
            case KEY_SURE:
                if(Key_Buf[i].Status.KEY_LEVEL == Bit_SET)//确认和上次相同
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_DOWN;//转入状态2
					Key_Buf[i].Status.KEY_EVENT = KEY_DOWN;//按下事件
                    Key_Buf[i].Status.KEY_COUNT = 0;//计数器清零
                }
                else
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_NULL;//转入状态0
                    Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//空事件
                }
                break;
            //状态2：按键按下
            case KEY_DOWN:
                if(Key_Buf[i].Status.KEY_LEVEL != Bit_SET)//按键释放，端口高电平
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_NULL;//转入状态0
                    Key_Buf[i].Status.KEY_EVENT = KEY_UP;//松开事件
                }
                else if((Key_Buf[i].Status.KEY_LEVEL == Bit_SET)
					&& (++Key_Buf[i].Status.KEY_COUNT >= KEY_LONG_DOWN_DELAY)) //超过KEY_LONG_DOWN_DELAY没有释放
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_LONG;//转入状态3
                    Key_Buf[i].Status.KEY_EVENT = KEY_LONG;//长按事件
					Key_Buf[i].Status.KEY_COUNT = 0;//计数器清零
                }
                else
                {
                    Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//空事件
                }
                break;
            //状态3：按键连续按下
            case KEY_LONG:
                if(Key_Buf[i].Status.KEY_LEVEL != Bit_SET)//按键释放，端口高电平
                {
                    Key_Buf[i].Status.KEY_STATUS = KEY_NULL;//转入状态0
                    Key_Buf[i].Status.KEY_EVENT = KEY_UP;//松开事件
                }
                else if((Key_Buf[i].Status.KEY_LEVEL == Bit_SET) 
                && (++Key_Buf[i].Status.KEY_COUNT >= KEY_LONG_DOWN_DELAY)) //超过KEY_LONG_DOWN_DELAY没有释放
                {
                    Key_Buf[i].Status.KEY_EVENT = KEY_LONG;//长按事件
                    Key_Buf[i].Status.KEY_COUNT = 0;//计数器清零
                }
                else
                {
                    Key_Buf[i].Status.KEY_EVENT = KEY_NULL;//空事件
                }
                break;
			default:
				break;
        }
	}
}
//旋转编码器CLK,DT引脚初始化
void encoder_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PC端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3; //PB0-BM_CLK、PB3-BM_DT
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化 GPIOB
	
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
	
	//GPIOB0 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//子优先级1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}

//中断服务函数，检测旋转编码器的旋转方向
void EXTI0_IRQHandler(void)
{
	delay_ms(1);	//消抖，很重要
	menuEvent[0]=1;//菜单事件
	if(BM_CLK==BM_DT) menuEvent[1]=BM_up; //顺时针旋转
	else menuEvent[1]=BM_down; //逆时针旋转
 	EXTI_ClearITPendingBit(EXTI_Line0);    //清除LINE1上的中断标志位 
}

