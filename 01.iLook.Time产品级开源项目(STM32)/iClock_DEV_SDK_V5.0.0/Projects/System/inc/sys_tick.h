/***********************************************************************/
/*                         SZQVC.Lighthouse                            */
/*                           www.szqvc.com                             */
/***********************************************************************/

#ifndef __SYS_TICK_H

#define __SYS_TICK_H

void SysTick_Ctrl(uint16_t cmd);
void SysTick_Init(void);

uint32_t GetSysTick_ms(void);
uint32_t GetSysTick_Sec(void);

void MarkSysTick_ms(uint32_t *t);
void MarkSysTick_Sec(uint32_t *t);

char TimeOutCheck_Sec(uint32_t i);
char TimeOutCheck_ms(uint32_t i);

void delay_ms(uint32_t i);
void delay_us(uint32_t i);

#endif


/*********************** (C) COPYRIGHT SZQVC **************************/
/*                          END OF FILE                               */
/**********************************************************************/
