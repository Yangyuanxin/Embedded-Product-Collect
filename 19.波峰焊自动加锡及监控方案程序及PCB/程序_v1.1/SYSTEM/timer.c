#include "timer.h"

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!

//u8 BeefWorkWay=0;   //0-fail,1-pass,other-no work
void TIMx_Int_Init(TIM_TypeDef* TIMx,u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    if(TIMx == TIM1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
    if(TIMx == TIM2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
    if(TIMx == TIM3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
    if(TIMx == TIM4)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM3初始化
    TIM_DeInit(TIMx);
	TIM_TimeBaseStructure.TIM_Period = arr-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //设置用来作为TIMx时钟频率除数的预分频值 // MHz时钟
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
    
	//中断优先级NVIC设置
    if(TIMx == TIM1)
        NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3中断
    if(TIMx == TIM2)
        NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
    if(TIMx == TIM3)
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
    if(TIMx == TIM4)
        NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
    //清除TIM_IT_Update寄存器能防止中断初始化时便立即进入中断
    TIM_Cmd(TIMx, ENABLE);
    TIM_ClearITPendingBit(TIMx, TIM_IT_Update);//清除更新中断请求位
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);//使能定时器 更新中断
    TIM_Cmd(TIMx, DISABLE);  //使能TIMx    
}


void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM 初始化
    TIM_DeInit(TIM4);
	TIM_TimeBaseStructure.TIM_Period = arr-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //设置用来作为TIMx时钟频率除数的预分频值 // MHz时钟
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

    TIM_Cmd(TIM4, ENABLE);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//清除更新中断请求位
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//使能定时器 更新中断
    TIM_Cmd(TIM4, DISABLE);  //使能TIMx  			 
}

void TIM1_UP_IRQHandler(void)   
{                      
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源   
    {  
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源   
        printf("this TIM1\r\n");
    }          
}

u16 TIM2_Time=0;
void TIM2_IRQHandler(void)   
{                      
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源   
    {  
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源   
        if(USART1_RX_STA&0x8000)
        {         
            
            if(Str_Pos((char*)USART1_RX_BUF,"CMD ERROR") )
            {
                USART1_RX_STA = 0;
                Cmd_SendErrorFlag = 0;
                return;
            }
			if(Str_Pos((char*)USART1_RX_BUF,"CMD OK") )
            {
                USART1_RX_STA = 0;
                Cmd_SendErrorFlag = 0;
                return;
            }
                        
        }
    }          
}

//定时器3中断服务程序
u16 TIM3_Time = 0;
u16 TIM3_SignalTime = 0;
u16 TIM3_TempTime = 0;
u16 TIM3_TempTime2 = 0;

u16 TIM3_Times1=0;
u16 TIM3_Times2=0;
u16 TIM3_Times3=0;
u16 TIM3_Times4=0;
void TIM3_IRQHandler(void)   //TIM3中断
{
	//u8 i=0;
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
        
        if(Timer3_EnableFlag&0x01)  TIM3_Times1++;
            else TIM3_Times1=0;
            
        if(Timer3_EnableFlag&0x02)  TIM3_Times2++;
            else TIM3_Times2=0;
            
        if(Timer3_EnableFlag&0x04)  TIM3_Times3++;
            else TIM3_Times3=0;
        
        if(Timer3_EnableFlag&0x08)  TIM3_Times4++;
            else TIM3_Times4=0;
      
	}
		
}
u8 TIM4_Temp=0;
u16 TIM4_Time=0;
void TIM4_IRQHandler(void)
{ 	
	//u16 i=0;
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //清除TIM4更新中断标志
        TIM_ClearFlag(TIM3, TIM_FLAG_Update);
        TIM4_Time++;
        if(TIM4_Temp)
        {
            if(0 == USART1_RecviceFlag)
            {
                if(TIM4_Time>5) //10*10ms 内没有接收到数据就标定此次接收完成
                {
                    TIM_Cmd(TIM4, DISABLE);  //关闭TIM4 
                    //for(i=USART1_RX_STA;i<USART_REC_LEN;i++) 
                    //{
                    //    USART1_RX_BUF[i] = 0;
                    //}
                    USART1_RX_STA |= 0x8000;	//标记接收完成 
                    //USART1_RecviceFlag = 0;
                    TIM4_Temp = 0; 
                   
                }
            }
            else
            {
                USART1_RecviceFlag = 0;
                //TIM4_Time = 0;
              
            }
        }
        if(USART1_RecviceFlag && (!TIM4_Temp)) 
        {
            USART1_RecviceFlag = 0;
            TIM4_Temp = 1;
    
        } 
	}	    
}

static uint16_t TIM2_CR=0;
static uint16_t TIM3_CR=0;
static u8 TIM_Flag=0;
void TIM_TemporaryClose(u8 NewState)//0-恢复定时器原来的状态,0x11-关闭指定定时器
{
    if(((NewState&0x01) || (NewState&0x10) ) && (0 == TIM_Flag) ) //需要关闭定时器之前先保存原来的状态
    {
        TIM2_CR = TIM2->CR1;
        TIM3_CR = TIM3->CR1;
    }
    
    if(0 == NewState)
    {
        if(0 == TIM_Flag)   return; //没有disable 就不执行
        TIM2->CR1 |= TIM2_CR;
        TIM3->CR1 |= TIM3_CR;
        TIM_Flag = 0;
    }
    else
    {
        TIM_Flag = 1;
        if(NewState&0x01)
            TIM_Cmd(TIM2,DISABLE);//
        if(NewState&0x10)
            TIM_Cmd(TIM3,DISABLE);//
    }
}











