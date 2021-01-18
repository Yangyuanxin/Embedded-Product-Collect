/*
*********************************************************************************************************
*
*    模块名称 : TOOL扩展IO板驱动
*    文件名称 : bsp_ext_io.h
*    版    本 : V1.0
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_EXT_IO_H
#define _BSP_EXT_IO_H

void EXIO_Start(void);
void EXIO_Stop(void);

void EX595_WritePin(uint8_t _pin, uint8_t _value);
void EX595_WritePort(uint32_t _value);

uint8_t EX165_GetPin(uint8_t _pin);

void DAC8562_SetDacData(uint8_t _ch, uint16_t _dac);

int16_t AD7606_ReadAdc(uint8_t _ch);

#endif

