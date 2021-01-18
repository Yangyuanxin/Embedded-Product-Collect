#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "param.h"
#include "modbus_reg_addr.h"
#include "modbus_register.h"

/* 为了避免和DAP驱动中的函数混淆，本模块函数名前缀用 h7swd */
static int lua_WriteReg16(lua_State* L);
static int lua_WriteReg32(lua_State* L);
static int lua_WriteRegFloat(lua_State* L);

static int lua_ReadReg16(lua_State* L);
static int lua_ReadReg32(lua_State* L);
static int lua_ReadRegFloat(lua_State* L);
static int lua_SaveParam(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_reg_RegisterFun
*    功能说明: 注册lua C语言接口函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void lua_reg_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "write_reg16", lua_WriteReg16);    
    lua_register(g_Lua, "write_reg32", lua_WriteReg32);
    lua_register(g_Lua, "write_regfloat", lua_WriteRegFloat);

    lua_register(g_Lua, "read_reg16", lua_ReadReg16);    
    lua_register(g_Lua, "read_reg32", lua_ReadReg32);
    lua_register(g_Lua, "read_regfloat", lua_ReadRegFloat);    
    
    lua_register(g_Lua, "save_param", lua_SaveParam);
}

/*
*********************************************************************************************************
*    函 数 名: lua_SaveParam
*    功能说明: 写参数。将参数刷新到eeprom
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_SaveParam(lua_State* L)
{
    SaveParam();
    
    if (g_tVar.CalibEnable == 1)
    {
        SaveCalibParam();
    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteReg16
*    功能说明: 写寄存器 16bit
*    形    参: Addr地址 和 寄存器值
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteReg16(lua_State* L)
{
    uint16_t addr;
    int16_t value;


    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);    /* 寄存器地址 */
    }

    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        value = luaL_checknumber(L, 2);    /* 寄存器值 */
    }    
    
    WriteRegValue_06H(addr, (uint16_t)value);
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadReg16
*    功能说明: 读寄存器 16bit
*    形    参: Addr地址
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
static int lua_ReadReg16(lua_State* L)
{
    uint16_t addr;
    uint16_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);    /* 寄存器地址 */
    }
    
    if (ReadRegValue_03H(addr, &value) == 1)
    {
        lua_pushnumber(L, value);    /* 成功,返回数据 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 成功,返回数据 */
    }
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteReg32
*    功能说明: 写寄存器 32Bit整数
*    形    参: Addr地址 和 寄存器值
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteReg32(lua_State* L)
{
    uint16_t addr;
    int32_t value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);    /* 寄存器地址 */
    }

    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        value = luaL_checknumber(L, 2);    /* 寄存器地址 */
    }    
    
    WriteRegValue_06H(addr, (uint32_t)value >> 16);
    WriteRegValue_06H(addr + 1, (uint32_t)value);
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadReg32
*    功能说明: 读寄存器 32bit 有符号
*    形    参: Addr地址
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
static int lua_ReadReg32(lua_State* L)
{
    uint16_t addr;
    uint16_t value1;
    uint16_t value2;
    int32_t value32;
    uint8_t re = 0;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);    /* 寄存器地址 */
    }
    
    re = ReadRegValue_03H(addr, &value1);
    re += ReadRegValue_03H(addr + 1, &value2);
    
    if (re == 2)
    {
        value32 =(value1 << 8) + value2;
        lua_pushnumber(L, value32);    /* 成功,返回数据 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 成功,返回数据 */
    }
    
    return 1;
}


/*
*********************************************************************************************************
*    函 数 名: lua_WriteRegFloat
*    功能说明: 写寄存器 32Bit浮点
*    形    参: Addr地址 和 寄存器值
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteRegFloat(lua_State* L)
{
    uint16_t addr;
    float ff;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);    /* 寄存器地址 */
    }

    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        ff = luaL_checknumber(L, 2);    /* 寄存器地址 */
    }    
    
    WriteRegValue_06H(addr, GetHigh16OfFloat(ff));
    WriteRegValue_06H(addr + 1, GetLow16OfFloat(ff));
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadRegFloat
*    功能说明: 读寄存器 浮点
*    形    参: Addr地址
*    返 回 值: 寄存器值
*********************************************************************************************************
*/
static int lua_ReadRegFloat(lua_State* L)
{
    uint16_t addr;
    uint16_t value1;
    uint16_t value2;
    float ff;
    uint8_t re = 0;
    uint8_t buf[4];

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);    /* 寄存器地址 */
    }
    
    re = ReadRegValue_03H(addr, &value1);
    re += ReadRegValue_03H(addr + 1, &value2);
    
    if (re == 2)
    {
        buf[0] = value1 >> 8;
        buf[1] = value1;
        buf[2] = value2 >> 8;
        buf[3] = value2;
        
        ff = BEBufToFloat(buf);
        
        lua_pushnumber(L, ff);    /* 成功,返回数据 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 成功,返回数据 */
    }
    
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
