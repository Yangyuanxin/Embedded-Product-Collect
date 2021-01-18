/*
*********************************************************************************************************
*
*    模块名称 : MODBUS从站寄存器操作
*    文件名称 : modbus_register.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MODBUS_REGISTER_H
#define __MODBUS_REGISTER_H

uint8_t WriteRegValue_06H(uint16_t reg_addr, uint16_t reg_value);
uint8_t ReadRegValue_03H(uint16_t _reg_addr, uint16_t *_reg_value);
uint8_t ReadRegValue_04H(uint16_t _reg_addr, uint16_t *_reg_value);

uint8_t MODS_GetDIState(uint16_t _reg, uint8_t *_value);
uint8_t MODS_GetDOState(uint16_t _reg, uint8_t *_value);
uint8_t MODS_WriteRelay(uint16_t _reg, uint8_t _on);

extern uint8_t fSaveReq_06H;    /* 保存参数请求，用于06H写寄存器函数 */
extern uint8_t fResetReq_06H;   /* 需要复位CPU，因为网络参数变化 */
extern uint8_t fSaveCalibParam; /* 保存校准参数请求，用于06H和10H写寄存器函数 */
extern uint8_t fDisableAck;    /* 强制无需应答，PC发虚拟按键时用到 */

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
