/*
*********************************************************************************************************
*
*    模块名称 : FDCAN驱动模块
*    文件名称 : bsp_can.c
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#ifndef __BSP_CAN_H
#define __BSP_CAN_H

void bsp_InitCan1(void);
void bsp_InitCan2(void);
void bsp_DeInitCan1(void);
void bsp_DeInitCan2(void);

void can1_SendPacket(uint8_t *_DataBuf, uint8_t _Len);
void can2_SendPacket(uint8_t *_DataBuf, uint8_t _Len);

extern FDCAN_RxHeaderTypeDef g_Can1RxHeader;
extern uint8_t g_Can1RxData[8];

extern FDCAN_RxHeaderTypeDef g_Can2RxHeader;
extern uint8_t g_Can2RxData[8];

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
