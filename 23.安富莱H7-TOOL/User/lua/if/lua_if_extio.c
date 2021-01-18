#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int lua_StartExtIO(lua_State* L);
static int lua_StopExtIO(lua_State* L);

static int lua_WriteDO(lua_State* L);
static int lua_ReadDI(lua_State* L);

static int lua_WriteDAC(lua_State* L);
static int lua_ReadADC(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_extio_RegisterFun
*    功能说明: 注册lua C语言接口函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void lua_extio_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "ex_start", lua_StartExtIO);
    lua_register(g_Lua, "ex_stop", lua_StopExtIO);
    lua_register(g_Lua, "ex_dout", lua_WriteDO);    
    lua_register(g_Lua, "ex_din", lua_ReadDI);
    lua_register(g_Lua, "ex_dac", lua_WriteDAC);
    lua_register(g_Lua, "ex_adc", lua_ReadADC);
}

            
/*
*********************************************************************************************************
*    函 数 名: lua_StartExtIO
*    功能说明: 启动扩展IO任务
*    形    参: 0-23
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_StartExtIO(lua_State* L)
{
    EXIO_Start();
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_StopExtIO
*    功能说明: 停止扩展IO任务
*    形    参: 0-23
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_StopExtIO(lua_State* L)
{
    EXIO_Stop();
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteDO
*    功能说明: 写DO
*    形    参: 0-23
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteDO(lua_State* L)
{
    uint8_t pin;
    uint8_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        pin = luaL_checknumber(L, 1);    /* dac */
    }

    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        value = luaL_checknumber(L, 2);    /* dac */
    }
    
    EX595_WritePin(pin, value);
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadDI
*    功能说明: 读DI
*    形    参: 0-15
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_ReadDI(lua_State* L)
{
    uint8_t pin;
    uint8_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        pin = luaL_checknumber(L, 1);    /* dac */
    }

    value = EX165_GetPin(pin);    
    
    lua_pushnumber(L, value);    /* 成功,返回数据 */
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteDAC
*    功能说明: 写DAC
*    形    参: 0-1
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteDAC(lua_State* L)
{
    uint8_t ch;
    uint16_t dac;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        ch = luaL_checknumber(L, 1);    /* dac */
    }

    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        dac = luaL_checknumber(L, 2);    /* dac */
    }
    
    DAC8562_SetDacData(ch, dac);
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadADC
*    功能说明: 读ADC
*    形    参: 0-7
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_ReadADC(lua_State* L)
{
    uint8_t ch;
    int16_t adc;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        ch = luaL_checknumber(L, 1);    /* dac */
    }

    adc = AD7606_ReadAdc(ch);    /* dac */

    lua_pushnumber(L, adc);    /* 成功,返回数据 */
    
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
