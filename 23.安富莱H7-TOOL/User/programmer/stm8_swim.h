/*
*********************************************************************************************************
*
*    模块名称 : SWIM GPIO驱动程序
*    文件名称 : bsp_swim_gpio.h
*    说    明 : STM8 SWIM接口底层驱动函数
*
*********************************************************************************************************
*/

#ifndef _BSP_SWIM_GPIO_H
#define _BSP_SWIM_GPIO_H

#define SWIM_CMD_RESET      0
#define SWIM_CMD_READ       1
#define SWIM_CMD_WRITE      2

#define SWIM_RESET_1()      BSP_SET_GPIO_1(GPIOE, GPIO_PIN_4 | GPIO_PIN_2);
#define SWIM_RESET_0()      BSP_SET_GPIO_0(GPIOE, GPIO_PIN_4 | GPIO_PIN_2);

void SWIM_InitHard(void);
void SWIM_SetResetPin(uint8_t _state);
uint8_t SWIM_EntrySequence(void);
uint8_t SWIM_WriteBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len);
uint8_t SWIM_WriteByte(uint32_t _Addr, uint8_t _data);
uint8_t SWIM_ReadBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len);
uint8_t SWIM_ReadByte(uint32_t _Addr);

uint8_t SWIM_DetectIC(uint32_t *_id);

#endif
