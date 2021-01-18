/*
*********************************************************************************************************
*
*    模块名称 : 消息处理模块
*    文件名称 : bsp_msg.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_MSG_H
#define __BSP_MSG_H

#define MSG_FIFO_SIZE    40               /* 消息个数 */

enum 
{
    MSG_NONE = 0,
    
    MSG_CAN1_RX,
    MSG_CAN2_RX,
    
    MSG_RESET_CPU,              /* 复位CPU */
    
    MSG_SAVE_PARAM,             /* 保存参数消息 */
    
    MSG_PG_START = 0x00001001,   /* 开始烧录 */
    MSG_PG_ABORT = 0x00001002,   /* 终止烧录 */    
    
    MSG_MODBUS_LUA_RUN,         /* modbus 收到lua程序，不是立即执行，放到bsp_Idle执行. 未用，用的 g_tVar.LuaRunOnce */ 
};

/* 按键FIFO用到变量 */
typedef struct
{
    uint32_t MsgCode;        /* 消息代码 */
    uint32_t MsgParam;        /* 消息的数据体, 也可以是指针（强制转化） */
}MSG_T;

/* 按键FIFO用到变量 */
typedef struct
{
    MSG_T Buf[MSG_FIFO_SIZE];    /* 消息缓冲区 */
    uint8_t Read;                    /* 缓冲区读指针1 */
    uint8_t Write;                    /* 缓冲区写指针 */
    uint8_t Read2;                    /* 缓冲区读指针2 */
}MSG_FIFO_T;

/* 供外部调用的函数声明 */
void bsp_InitMsg(void);
void bsp_PutMsg(uint32_t _MsgCode, uint32_t _MsgParam);
uint8_t bsp_GetMsg(MSG_T *_pMsg);
uint8_t bsp_GetMsg2(MSG_T *_pMsg);
void bsp_ClearMsg(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
