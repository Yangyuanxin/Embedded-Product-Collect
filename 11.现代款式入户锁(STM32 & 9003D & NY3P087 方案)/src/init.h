#ifndef __INIT_H__
#define __INIT_H__

void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART_Configuration(void);
//void CAN_Configuration(void);
void CAN_Configuration(int filter);
void TIM_Configuration(void);
void EXTI_Configuration(void);
void ADC_Configuration(void);

#endif
