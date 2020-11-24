#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"

#define COM0_H() GPIOD->BSRR = GPIO_Pin_3
#define COM0_L() GPIOD->BRR = GPIO_Pin_3
#define COM1_H() GPIOD->BSRR = GPIO_Pin_4
#define COM1_L() GPIOD->BRR = GPIO_Pin_4


static u8 State_open;
static u8 Code;

//OS_SEM LED_SEM;

//0 ~ 9
const u8 char_code[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};


void LED_Open()
{
	State_open = 1;
	//os_sem_send(&LED_SEM);
}

void LED_Close()
{
	State_open = 0;
}

void LED_Show(u8 code)
{
	Code = code;
}

__task void task_led (void)
{
	u32 temp;
	u8 cnt = 0;

	//os_sem_init(&LED_SEM, 0);

	State_open = 1;

	while(1)
	{
		//os_sem_wait(&LED_SEM, 0xffff);

		while(State_open)
		{
			temp = GPIOE->ODR & 0x00ff;
			temp |= char_code[Code & 0x0f] << 8;
			GPIOE->ODR = temp;
			COM0_H();

			os_dly_wait(10);


			if(cnt++ == 50)
			{
				cnt = 0;
				Code++;
				Code %= 100;
			}



			COM0_L();

			temp = GPIOE->ODR & 0x00ff;
			temp |= char_code[Code >> 4] << 8;
			GPIOE->ODR = temp;
			COM1_H();

			os_dly_wait(10);

			COM1_L();
		}

		COM0_L();
		COM1_L();
	}
}


















