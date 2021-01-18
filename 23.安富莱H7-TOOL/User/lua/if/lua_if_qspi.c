#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int qspi_readid(lua_State* L);
static int qspi_write(lua_State* L);
static int qspi_read(lua_State* L);
static int qspi_erase4k(lua_State* L);

void lua_qspi_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "qspi_readid", qspi_readid);    
    lua_register(g_Lua, "qspi_read", qspi_read);    
    lua_register(g_Lua, "qspi_write", qspi_write);
    lua_register(g_Lua, "qspi_erase4k", qspi_erase4k);
}

/*
*********************************************************************************************************
*    函 数 名: qspi_readid
*    功能说明: 读芯片ID
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int qspi_readid(lua_State* L)
{
    uint32_t id;
    
    id = QSPI_ReadID();
    lua_pushnumber(L, id);    /* 返回值 */
        
    return 1;
}


/*
*********************************************************************************************************
*    函 数 名: qspi_erase4k
*    功能说明: 擦除一个扇区，4KB
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int qspi_erase4k(lua_State* L)
{
    uint32_t addr;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        addr = luaL_checknumber(L, 1);
        
        addr = addr / 4096;
        addr = addr * 4096;
    }

    QSPI_EraseSector(addr);
    lua_pushnumber(L, 1);    /* 成功 */

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: qspi_write
*    功能说明: 写一包数据，小于256字节
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int qspi_write(lua_State* L)
{
    size_t len;
    const char *data;
    uint32_t addr;

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        addr = luaL_checknumber(L, 1);
    }
    
    if (lua_type(L, 2) == LUA_TSTRING)     /* 判断第1个参数 */
    {        
        data = luaL_checklstring(L, 2, &len); /* 1是参数的位置， len是string的长度 */        
    }
    
    if (len > QSPI_PAGE_SIZE)
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    if (QSPI_WriteBuffer((uint8_t *)data, addr, len) == 0)
    {
        lua_pushnumber(L, 0);    /* 出错 */
    }
    else
    {
        lua_pushnumber(L, 1);    /* 成功 */
    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: qspi_read
*    功能说明: 读一包数据，可大于256字节
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int qspi_read(lua_State* L)
{
    uint32_t addr;
    uint32_t num;

    if (lua_type(L, 1) == LUA_TNUMBER)     /* 判断第1个参数 */
    {        
        addr = luaL_checknumber(L, 1); /* 1是参数的位置， len是string的长度 */        
    }
    
    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        num = luaL_checknumber(L, 2);
        
        memset(s_lua_read_buf, 0, num);
        
    }
    
    if (num > LUA_READ_LEN_MAX)
    {
        return 0;
    }
        
    QSPI_ReadBuffer(s_lua_read_buf, addr, num);

    lua_pushlstring(L, (char *)s_lua_read_buf, num); 
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
