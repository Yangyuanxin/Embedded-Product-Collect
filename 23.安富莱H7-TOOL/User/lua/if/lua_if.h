/*
*********************************************************************************************************
*
*    模块名称 : lua接口模块
*    文件名称 : lua_if.h
*    版    本 : V1.0
*    说    明 : 。
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2015-04-25 armfly  ST固件库版本 V2.1.0
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __LUA_IF_H
#define __LUA_IF_H

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if_i2c.h"
#include "lua_if_gpio.h"
#include "lua_if_spi.h"
#include "lua_if_qspi.h"
#include "lua_if_tcp_udp.h"
#include "lua_if_tim.h"
#include "lua_if_fatfs.h"
#include "lua_if_swd.h"
#include "lua_if_swim.h"
#include "lua_if_adc.h"
#include "lua_if_dac.h"
#include "lua_if_register.h"
#include "lua_if_extio.h"
#include "lua_if_uart.h"

#define LUA_PROG_LEN_MAX    (32 * 1024)

#define LUA_READ_LEN_MAX    (4 * 1024)

void lua_Test(void);
void lua_Init(void);
void lua_PowerOnLua(void);
void lua_DeInit(void);
void lua_StackDump(lua_State *L);
void lua_DownLoad(uint32_t _addr, uint8_t *_buf, uint32_t _len, uint32_t _total_len);
void lua_Run(void);
int lua_CheckGlobal(const char *name);
void lua_do(char *buf);
void lua_DownLoadFile(char *_path);
void lua_RunLuaProg(void);

uint32_t lua_GetVarUint32(const char *_VarName, uint32_t _Default);

extern lua_State *g_Lua;

extern uint8_t s_lua_read_buf[LUA_READ_LEN_MAX];
extern uint32_t s_lua_read_len;

extern char s_lua_prog_buf[LUA_PROG_LEN_MAX + 1];
extern uint32_t s_lua_prog_len;



#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
