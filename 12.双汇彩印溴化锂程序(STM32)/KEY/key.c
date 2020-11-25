#define _KEY_C

#include "head.h"

//#define DEBUG_KEY

//#define DEBUG_KEY

#ifdef DEBUG_KEY
UINT8 key_buf[20];	 
#endif

static void KEYEXTI_Configuration(void);

void InitKey(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

// 	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQChannel;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);

	/* 按键的4个管脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
        
//     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
// 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
// 	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz;
// 	GPIO_Init(GPIOC,&GPIO_InitStructure);

	KEYEXTI_Configuration();
	key1 = 0;
	key2 = 0;
	key3 = 0;
	key_times = 0;

}
u8 take_key()
{
	u8 model;
	model=key_buffer;
	key_buffer=0;
//	printf("model=%d",model);
	return model;
}



void KEYEXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource13);



    EXTI_InitStructure.EXTI_Line = EXTI_Line10|EXTI_Line11|EXTI_Line12|EXTI_Line13;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

