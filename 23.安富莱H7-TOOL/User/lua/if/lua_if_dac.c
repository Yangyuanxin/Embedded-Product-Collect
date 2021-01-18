#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "param.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"

/* 为了避免和DAP驱动中的函数混淆，本模块函数名前缀用 h7swd */

static int lua_WriteDac(lua_State* L);
static int lua_WriteVolt(lua_State* L);
static int lua_WriteCurr(lua_State* L);
static int lua_WriteTvccDac(lua_State* L);
static int lua_WriteTvccVolt(lua_State* L);
static int lua_PowerOnDac(lua_State* L);
static int lua_PowerOffDac(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_adc_RegisterFun
*    功能说明: 注册lua C语言接口函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void lua_dac_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "dac_on", lua_PowerOnDac);    
    lua_register(g_Lua, "dac_off", lua_PowerOffDac);
    lua_register(g_Lua, "dac_write", lua_WriteDac);    
    lua_register(g_Lua, "dac_volt", lua_WriteVolt);
    lua_register(g_Lua, "dac_curr", lua_WriteCurr);
    lua_register(g_Lua, "write_tvcc_dac", lua_WriteTvccDac);
    lua_register(g_Lua, "write_tvcc_volt", lua_WriteTvccVolt);
}
        
/*
*********************************************************************************************************
*    函 数 名: lua_PowerOnDac
*    功能说明: 打开DAC电源
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_PowerOnDac(lua_State* L)
{    
    WriteRegValue_06H(REG03_DAC_WAVE_TYPE, 0);    /* 直流 */
    WriteRegValue_06H(REG03_DAC_WAVE_START, 1);
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_PowerOffDac
*    功能说明: 关闭DAC电源
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_PowerOffDac(lua_State* L)
{    
    WriteRegValue_06H(REG03_DAC_WAVE_TYPE, 0);    /* 直流 */
    WriteRegValue_06H(REG03_DAC_WAVE_START, 0);
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteDac
*    功能说明: 写DAC
*    形    参: dac值 0-4095
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteDac(lua_State* L)
{
    uint16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        value = luaL_checknumber(L, 1);    /* dac */
    }
    
    WriteRegValue_06H(REG03_OUT_VOLT_DAC, value);
    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteVolt
*    功能说明: 写电压值
*    形    参: 电压mv
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteVolt(lua_State* L)
{
    int16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        value = luaL_checknumber(L, 1);    /* 电压 */
    }
    
    WriteRegValue_06H(REG03_OUT_VOLT_MV, value);
    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteCurr
*    功能说明: 写电流值
*    形    参: 电流值uA
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteCurr(lua_State* L)
{
    uint16_t value;


    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        value = luaL_checknumber(L, 1);    /* dac */
    }
    
    WriteRegValue_06H(REG03_OUT_CURR_UA, value);
    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteTvccDac
*    功能说明: 写TVCC DAC
*    形    参: dac 0-127
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteTvccDac(lua_State* L)
{
    uint16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        value = luaL_checknumber(L, 1);    /* dac */
    }
    
    WriteRegValue_06H(REG03_OUT_TVCC_DAC, value);
    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteTvccVolt
*    功能说明: 写TVCC 电压
*    形    参: dac 1200-5000
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteTvccVolt(lua_State* L)
{
    uint16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        value = luaL_checknumber(L, 1);    /* dac */
    }
    
    WriteRegValue_06H(REG03_OUT_TVCC_MV, value);
    
    return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
