
#define _KI_C
#include "head.h"


//#ifdef DEBUG_KEY
	 
//#endif
UINT16 ki1,ki2,ki3,ki_times;


//vu32 F_Count;

/*
配置开入0 中断方式，用于计数流量仪传送的脉冲数
*/
void KIEXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource6);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7);



    EXTI_InitStructure.EXTI_Line = EXTI_Line6|EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void Init_Ki(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        
       	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

/*开入端口的配置*/
	
        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	ki1 = 0;
	ki2 = 0;
	ki3 = 0;
	ki_times = 0;
        
    KIEXTI_Configuration();
}




void ProKi(void)
{
	//INT16 temp=0;		
	ki3 = ki2;
	ki2 = ki1;	
	ki1 = 0;

	//开始扫描键值线
	if (GET_KI0() == RESET)
	{
		ki1 |= KI0;
	//	send_str(0,"ki0 set\r\n");

	}
	else		
	{
		ki1 &= (~KI0);
	//	send_str(0,"ki0 reseet\r\n");
	}
	if (GET_KI1() == RESET)
	{
		ki1 |= KI1;
	

	}
	else		
	{
		ki1 &= (~KI1);
	}
	if (GET_KI2() == RESET)
	{
		ki1 |= KI2;
//			send_str(0,"ki2\r\n");

	}
	else		
	{
		ki1 &= (~KI2);
	}
	if (GET_KI3() == RESET)
	{
		ki1 |= KI3;

	}
	else		
	{
		ki1 &= (~KI3);
	}
	if (GET_KI4() == RESET)
	{
		ki1 |= KI4;

	}
	else		
	{
		ki1 &= (~KI4);
	}
	if (GET_KI5() == RESET)
	{
		ki1 |= KI5;

	}
	else		
	{
		ki1 &= (~KI5);
	}
	if (GET_KI6() == RESET)
	{
		ki1 |= KI6;

	}
	else		
	{
		ki1 &= (~KI6);
	}
	if (GET_KI7() == RESET)
	{
		ki1 |= KI7;

	}
	else		
	{
		ki1 &= (~KI7);
	}
	if (GET_KI8() == RESET)
	{
		ki1 |= KI8;

	}
	else		
	{
		ki1 &= (~KI8);
	}
	if (GET_KI9() == RESET)
	{
		ki1 |= KI9;

	}
	else		
	{
		ki1 &= (~KI9);
	}
	if (GET_KIA() == RESET)
	{
		ki1 |= KIA;

	}
	else		
	{
		ki1 &= (~KIA);
	}
	if (GET_KIB() == RESET)
	{
		ki1 |= KIB;

	}
	else		
	{
		ki1 &= (~KIB);
	}
	if (GET_KIC() == RESET)
	{
		ki1 |= KIC;

	}
	else		
	{
		ki1 &= (~KIC);
	}
	if (GET_KID() == RESET)
	{
		ki1 |= KID;

	}
	else		
	{
		ki1 &= (~KID);
	}
	if (GET_KIE() == RESET)
	{
		ki1 |= KIE;

	}
	else		
	{
		ki1 &= (~KIE);
	}
	if (GET_KIF() == RESET)
	{
		ki1 |= KIF;

	}
	else		
	{
		ki1 &= (~KIF);
	}
	//	
	if ((ki1 == ki2) && (ki2 == ki3) ) 
	{


		ki_times++;
                
		if (ki_times < 25)
		{
			return;
		}
		else
		{
			yx_buf[0]=ki1;
		//	printf("ki_buf=%d\r\n",ki_buf); 
		}
                		
	 }
	//Process_Yx_Change_Flag();
 	
}
void Process_Yx_Change_Flag(void)
{
	u8 yx_num;//开入有几个半字
	for (yx_num=0;yx_num<KI_NUM;yx_num++)
	{
		if (yx_OldBuf[yx_num] != yx_buf[yx_num])
		{
			yx_OldBuf[yx_num] = yx_buf[yx_num];
			yx_change_flag = 1;//主要用于刷新液晶显示，若有开入改变
		}
	}
}