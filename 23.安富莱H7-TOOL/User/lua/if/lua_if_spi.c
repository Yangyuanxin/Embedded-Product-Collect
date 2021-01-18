#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"

/*
    spi_bus("init", 0, 0, 0)    --初始化, 时钟频率id, 相位, 极性)
    spi_bus("send", "1234")     --发送二进制字符串
    spi_bus("send", 0x88)       --发送1个字节整数
    spi_bus("recive", 128)      --接收128个字节数据
*/

static int lua_SPI_Init(lua_State* L);
static int lua_SPI_SendBytes(lua_State* L);
static int lua_SPI_ReciveBytes(lua_State* L);
static int lua_SPI_SendRecive(lua_State* L);
    
static int lua_SPI_BusCmd(lua_State* L);
    
void lua_spi_RegisterFun(void)
{
    lua_register(g_Lua, "spi_bus", lua_SPI_BusCmd);
}

static int lua_SPI_BusCmd(lua_State* L)
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
        return lua_SPI_Init(L);
    }    
    else if (strcmp(StrCmd, "send") == 0)
    {
        return lua_SPI_SendBytes(L);
    }
    else if (strcmp(StrCmd, "recive") == 0)
    {
        return lua_SPI_ReciveBytes(L);
    }
    else if (strcmp(StrCmd, "sendrecive") == 0)
    {
        return lua_SPI_SendRecive(L);
    }    

    return 0;      
}

static int lua_SPI_Init(lua_State* L)
{
    uint32_t temp;
    uint32_t baud;
    uint32_t phase;
    uint32_t polarity;
    
    if (lua_type(L, 2) == LUA_TNUMBER)              /* 判断第2个参数 */
    {
        uint32_t TableBaud[8] = 
        {
            SPI_BAUDRATEPRESCALER_390_625K,
            SPI_BAUDRATEPRESCALER_781_25K,
            SPI_BAUDRATEPRESCALER_1_5625M,
            SPI_BAUDRATEPRESCALER_3_125M,
            SPI_BAUDRATEPRESCALER_6_25M,
            SPI_BAUDRATEPRESCALER_12_5M,
            SPI_BAUDRATEPRESCALER_25M,
            SPI_BAUDRATEPRESCALER_50M,
        };
        temp = luaL_checknumber(L, 2);
        if (temp < 8)
        {
            baud = TableBaud[temp];
        }
        else
        {
            baud = SPI_BAUDRATEPRESCALER_1_5625M;
        }
    }

    if (lua_type(L, 3) == LUA_TNUMBER)              /* 判断第3个参数 相位 */
    {
        temp = luaL_checknumber(L, 3);
        if (temp == 0)
        {
            phase = SPI_PHASE_1EDGE;
        }
        else
        {
            phase = SPI_PHASE_2EDGE; 
        }
    } 

    if (lua_type(L, 4) == LUA_TNUMBER)              /* 判断第4个参数 极性 */
    {
        temp = luaL_checknumber(L, 4);
        if (temp == 0)
        {
            polarity = SPI_POLARITY_LOW;
        }
        else
        {
            polarity = SPI_POLARITY_HIGH; 
        }            
    }         
    
    bsp_InitSPIParam(SPI4, baud, phase, polarity);
    return 0;
}

static int lua_SPI_SendBytes(lua_State* L)
{
    size_t len;
    const char *data;
    uint8_t buf[1];

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
    
    bsp_SpiSendBuf(SPI4, data, len);
    return 0;
}

static int lua_SPI_ReciveBytes(lua_State* L)
{
    size_t len;

    len = luaL_checknumber(L, 2);
    if (len == 0 || len > LUA_READ_LEN_MAX)
    {
        lua_pushnumber(L, 0);    /* 返回值 */
        return 1;
    }

    bsp_SpiReciveBuf(SPI4, (char *)s_lua_read_buf, len);    
    
    lua_pushlstring(L, (char *)s_lua_read_buf, len); 
    return 1;
}

//发送同时接收  spi_bus("sendrecive", txdata, rxlen)
static int lua_SPI_SendRecive(lua_State* L)
{
    size_t txlen;
    const char *txdata;
    uint16_t rxlen;

    if (lua_type(L, 2) == LUA_TSTRING)     /* 判断第2个参数 */
    {        
        txdata = luaL_checklstring(L, 2, &txlen); /* 1是参数的位置， len是string 的长度 */        
    }
    else
    {
        return 0;    
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER) /* 判断第13参数 */
    {
        rxlen = luaL_checknumber(L, 3);
    }
    else
    {
        return 0;    
    }    
    
    bsp_SpiSendRecive(SPI4, txdata, txlen, (char *)s_lua_read_buf, rxlen);
    
    lua_pushlstring(L, (char *)s_lua_read_buf, rxlen); 
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
