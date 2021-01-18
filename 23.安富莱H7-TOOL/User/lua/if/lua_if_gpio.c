#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int lua_SetTVCC(lua_State* L);
static int lua_GpioCfg(lua_State* L);
static int lua_GpioWrite(lua_State* L);
static int lua_GpioRead(lua_State* L);
static int lua_ReadFmcBus(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_GpioCfg
*    功能说明: 配置GPIO功能
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
void lua_gpio_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "gpio_cfg",  lua_GpioCfg);          /* gpio_cfg(0, 1) */
    lua_register(g_Lua, "gpio_write", lua_GpioWrite);       /* gpio_write(0, 1) */
    lua_register(g_Lua, "gpio_read", lua_GpioRead);         /* gpio_read(0) */
    lua_register(g_Lua, "read_bus", lua_ReadFmcBus);        /* read_bus() */
    
    lua_register(g_Lua, "set_tvcc", lua_SetTVCC);    /* */
    
}

/*
*********************************************************************************************************
*    函 数 名: lua_GpioCfg
*    功能说明: 配置GPIO功能
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_GpioCfg(lua_State* L)
{
    uint8_t _no;
    uint8_t _dir;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        _no = luaL_checknumber(L, 1);
    }
    else
    {
        return 1;
    }

    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        _dir = luaL_checknumber(L, 2);
    }
    else
    {
        return 1;
    }
    
//    if (_dir == 0)
//    {
//        EIO_ConfigPort(_no, ES_GPIO_IN);
//    }
//    else if (_dir == 1)
//    {
//        EIO_ConfigPort(_no, ES_GPIO_OUT);
//    }    
//    else if (_dir == 2)
//    {
//        EIO_ConfigPort(_no, ES_FMC_OUT);
//    }
    EIO_ConfigPort(_no, (EIO_SELECT_E)_dir);
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_GpioWrite
*    功能说明: 设置GPIO输出电平
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_GpioWrite(lua_State* L)
{
    uint8_t _no;
    uint8_t _value;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        _no = luaL_checknumber(L, 1);
    }
    else
    {
        return 1;
    }

    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        _value = luaL_checknumber(L, 2);
    }
    else
    {
        return 1;
    }
    
    if (_value == 0)
    {
        EIO_SetOutLevel(_no, 0);
    }
    else if (_value == 1)
    {
        EIO_SetOutLevel(_no, 1);
    }    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_GpioRead
*    功能说明: 读取GPIO输出电平
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_GpioRead(lua_State* L)
{
    uint8_t _no;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        _no = luaL_checknumber(L, 1);
    }
    else
    {
        return 0;
    }

    if (EIO_GetInputLevel(_no) == 0)
    {
        lua_pushnumber(L, 0);    /* 返回值 */
    }
    else
    {
        lua_pushnumber(L, 1);    /* 返回值 */
    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadFmcBus
*    功能说明: 通过FMC总线读取口线电平
*    形    参: 无
*    返 回 值: 16bit数据
*********************************************************************************************************
*/
static int lua_ReadFmcBus(lua_State* L)
{
    uint16_t in; 
    
    in = EIO_ReadFMC();
    
    lua_pushnumber(L, in);    /* 返回值 */
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_SetTVCC
*    功能说明: 设置TVCC电压
*    形    参: 输入IO电压. 单位伏。支持浮点
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_SetTVCC(lua_State* L)
{
    float volt;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        volt = luaL_checknumber(L, 1);
        bsp_SetTVCC(volt * 1000);
    }
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
