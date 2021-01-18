#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

static int lua_I2C_SendBytes(lua_State* L);
static int lua_I2C_ReciveBytes(lua_State* L);

static int lua_I2C_BusCmd(lua_State* L);
    
void lua_i2c_RegisterFun(void)
{
    lua_register(g_Lua, "i2c_bus", lua_I2C_BusCmd);
}

static int lua_I2C_BusCmd(lua_State* L)
{
    const char *StrCmd;
    size_t len;    
    
    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {
        StrCmd = luaL_checklstring(L, 1, &len);     /* 1是参数的位置， len是string的长度 */       
    }
    else
    {
        return 0;
    }
    
    if (strcmp(StrCmd, "init") == 0)
    {
        uint32_t freq = 1000000;
        
        if (lua_type(L, 2) == LUA_TNUMBER)              /* 判断第1个参数 */
        {
            freq = luaL_checknumber(L, 2);
        }        
        bsp_InitExtI2C(freq);
        return 0;
    }    
    else if (strcmp(StrCmd, "start") == 0)
    {
        ext_i2c_Start();
        return 0;
    }
    else if (strcmp(StrCmd, "stop") == 0)
    {
        ext_i2c_Stop();
        return 0;
    }
    else if (strcmp(StrCmd, "wait_ack") == 0)
    {
        uint8_t ack;
        
        ack = ext_i2c_WaitAck();
        lua_pushnumber(L, ack);
        return 1;
    }    
    else if (strcmp(StrCmd, "ack") == 0)
    {
        ext_i2c_Ack();
        return 0;
    }
    else if (strcmp(StrCmd, "nack") == 0)
    {
        ext_i2c_NAck();
        return 0;
    }        
    else if (strcmp(StrCmd, "check") == 0)
    {
        uint16_t addr;
        uint8_t ack;
        
        if (lua_type(L, 2) == LUA_TNUMBER)              /* 判断第1个参数 */
        {
            addr = luaL_checknumber(L, 2);

            ack = ext_i2c_CheckDevice(addr);
            
            lua_pushnumber(L, ack);
            return 1;
        }
        lua_pushnumber(L, 1);       /* 1表示总线无应答 */
        return 1;
    } 
    else if (strcmp(StrCmd, "send") == 0)
    {
        return lua_I2C_SendBytes(L);
    }
    else if (strcmp(StrCmd, "recive") == 0)
    {
        return lua_I2C_ReciveBytes(L);
    }

    return 0;      
}

/*
    形式1: i2c_send(buf);
    形式2: i2c_send(3);    
*/
static int lua_I2C_SendBytes(lua_State* L)
{
    size_t i;
    size_t len;
    const char *data;
    uint8_t buf[1];
    int re;

    if (lua_type(L, 2) == LUA_TSTRING)     /* 判断第1个参数 */
    {        
        data = luaL_checklstring(L, 2, &len); /* 1是参数的位置， len是string 的长度 */        
    }
    
    else if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        len = 1;
        buf[0] = luaL_checknumber(L, 2);
        
        data = (const char *)buf;
    }
    
    re = 0;
    for (i = 0; i < len; i++)
    {
        ext_i2c_SendByte(data[i]);    
        if (ext_i2c_WaitAck() != 0)
        {
            ext_i2c_Stop();
            re = 1;
            break;
        }        
    }
    lua_pushnumber(L, re);    /* 返回值 */
    return 1;
}

/*
Lua给C++传buffer时，使用string就行，再C++的，tolua++中使用下面代码读取buffer
        size_t ld;
        const char *data = luaL_checklstring(tolua_S, 2, &ld); // 2是参数的位置， ld是buffer的长度
C++给Lua传buffer时，在C++代码中使用下面代码传入buffer
            LuaStack *stack = LuaEngine::getInstance()->getLuaStack();
            stack->

*/
// lua_I2C_ReciveBytes(2);
static int lua_I2C_ReciveBytes(lua_State* L)
{
    size_t len;
    size_t i;
    
    len = luaL_checknumber(L, 2);
    if (len == 0 || len > LUA_READ_LEN_MAX)
    {
        lua_pushnumber(L, 0);    /* 返回值 */
        return 1;
    }

    for (i = 0; i < len; i++)
    {
        s_lua_read_buf[i] = ext_i2c_ReadByte();    /* 读1个字节 */

        /* 每读完1个字节后，需要发送Ack， 最后一个字节不需要Ack，发Nack */
        if (i != len - 1)
        {
            ext_i2c_Ack();    /* 中间字节读完后，CPU产生ACK信号(驱动SDA = 0) */
        }
        else
        {
            ext_i2c_NAck();    /* 最后1个字节读完后，CPU产生NACK信号(驱动SDA = 1) */
        }
    }
    
    lua_pushlstring(L, (char *)s_lua_read_buf, len); 
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
