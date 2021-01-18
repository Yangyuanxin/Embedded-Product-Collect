/*
*********************************************************************************************************
*
*    模块名称 : 12bit DAC芯片MCP4725驱动模块
*    文件名称 : bsp_i2c_mcp4725.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_MCP4725_H
#define _BSP_MCP4725_H

#define MCP4025_SLAVE_ADDRESS 0xC0 /* I2C从机地址 */

void bsp_InitMCP4725(void);
void MCP4725_WriteData(uint8_t _ch, uint16_t _usDac);
void MCP4725_SetVolt(uint8_t _ch, uint16_t _volt);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
