/*
*********************************************************************************************************
*
*    模块名称 : MODBUS发送print数据
*    文件名称 : modbus_print.h
*
*********************************************************************************************************
*/
#ifndef __MODBUS_PRINT_H
#define __MODBUS_PRINT_H

void MODH_Send61H(uint8_t _Ch, uint8_t *_TxBuf, uint16_t _TxLen);
void MODH_PrintByte(char _ch);
void MODH_PrintBuf(uint8_t *_buf, uint16_t _len);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
