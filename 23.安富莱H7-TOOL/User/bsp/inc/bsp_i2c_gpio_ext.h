/*
*********************************************************************************************************
*
*    模块名称 : I2C总线驱动模块
*    文件名称 : bsp_i2c_gpio_ex.h
*    版    本 : V1.0
*    说    明 : 头文件。
*
*    Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_EXT_I2C_GPIO_H
#define _BSP_EXT_I2C_GPIO_H

void bsp_InitExtI2C(uint32_t freq);
void ext_i2c_Start(void);
void ext_i2c_Stop(void);
void ext_i2c_SendByte(uint8_t _ucByte);
uint8_t ext_i2c_ReadByte(void);
uint8_t ext_i2c_WaitAck(void);
void ext_i2c_Ack(void);
void ext_i2c_NAck(void);
uint8_t ext_i2c_CheckDevice(uint8_t _Address);

#endif
