/*
*********************************************************************************************************
*
*    模块名称 : lua swim模块
*    文件名称 : lua_if_swim.h
*    版    本 : V1.0
*    说    明 : 。
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2019-09-29 armfly  首发
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __LUA_IF_SWIM_H
#define __LUA_IF_SWIM_H

void lua_swim_RegisterFun(void);
uint16_t PG_SWIM_ProgFile(char *_Path, uint32_t _FlashAddr, uint32_t _EndAddr, uint32_t _CtrlByte, uint32_t _FileIndex);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
