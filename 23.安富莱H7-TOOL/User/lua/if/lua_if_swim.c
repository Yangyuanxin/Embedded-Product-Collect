#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "elf_file.h"
#include "file_lib.h"
#include "prog_if.h"
#include "target_reset.h"
#include "target_config.h"
#include "swd_host.h"
#include "Systick_Handler.h"
#include "main.h"
#include "target_family.h"
#include "stm8_swim.h"
#include "stm8_flash.h"

/* 为了避免和DAP驱动中的函数混淆，本模块函数名前缀用 h7swim */

//static int h7swim_Init(lua_State* L);
//static int h7swim_WriteMemory(lua_State* L);
//static int h7swim_ReadMemory(lua_State* L);
//static int h7swim_ProgFlash(lua_State* L);
//static int h7swim_ProgOptionBytes(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_swd_RegisterFun
*    功能说明: 注册lua C语言接口函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void PG_PrintText(char *_str);
void lua_swim_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
//    lua_register(g_Lua, "swim_init", h7swim_Init);    
//    lua_register(g_Lua, "swim_write", h7swim_WriteMemory);    
//    lua_register(g_Lua, "swim_read", h7swim_ReadMemory); 
//    lua_register(g_Lua, "swim_prog_flash", h7swim_ProgFlash);  
//    lua_register(g_Lua, "swim_prog_ob", h7swim_ProgOptionBytes);  
}

/*
*********************************************************************************************************
*    函 数 名: h7swim_ReadLuaVar
*    功能说明: 读lua文件中的全局变量
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void h7swim_ReadLuaVar(void)
{
    lua_getglobal(g_Lua, "STM8_SERIAL");  
    if (lua_isstring(g_Lua, -1)) 
    {
        const char *str;
        str = lua_tostring(g_Lua, -1);
        
        if (strcmp(str, "STM8L") == 0)
        {
            s_STM8_SerialType = STM8L;
        }
        else
        {
            s_STM8_SerialType = STM8S;
        }
    }
    else
    {
        PG_PrintText("脚本错误 STM8_SERIAL"); 
    }
    lua_pop(g_Lua, 1);    
            
    lua_getglobal(g_Lua, "FLASH_BLOCK_SIZE");  
    if (lua_isinteger(g_Lua, -1)) 
    {
        s_STM8_BlockSize = lua_tointeger(g_Lua, -1);
    }
    else
    {
        PG_PrintText("脚本错误 FLASH_BLOCK_SIZE"); 
    } 
    lua_pop(g_Lua, 1);    

    lua_getglobal(g_Lua, "FLASH_SIZE");  
    if (lua_isinteger(g_Lua, -1)) 
    {
        s_STM8_FlashSize = lua_tointeger(g_Lua, -1);
    }
    else
    {
        PG_PrintText("脚本错误 FLASH_SIZE"); 
    } 
    lua_pop(g_Lua, 1);    
    
    lua_getglobal(g_Lua, "EEPROM_SIZE");  
    if (lua_isinteger(g_Lua, -1)) 
    {
        s_STM8_EEPromSize = lua_tointeger(g_Lua, -1);
    }
    else
    {
        PG_PrintText("脚本错误 EEPROM_SIZE"); 
    }
    lua_pop(g_Lua, 1);

    lua_getglobal(g_Lua, "STM8_HVOFF");  
    if (lua_isinteger(g_Lua, -1)) 
    {
        s_STM8_HVOFF = lua_tointeger(g_Lua, -1);
    }
    else
    {
        s_STM8_HVOFF = 0;
    }
    lua_pop(g_Lua, 1);
}

///*
//*********************************************************************************************************
//*    函 数 名: h7swim_Init
//*    功能说明: 读芯片ID
//*    形    参: 无
//*    返 回 值: 无
//*********************************************************************************************************
//*/
//static int h7swim_Init(lua_State* L)
//{
////    float vcc;

////    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
////    {
////        vcc = luaL_checknumber(L, 1);    /* VCC电压，浮点，单位V */
////    }
////    
////    h7swim_ReadLuaVar();        /* 读取LUA中的全局变量 */
////    
////    bsp_SetTVCC(vcc * 1000);    /* 设置接口电平3.3V */
////    bsp_DelayUS(100 * 100);     /* 延迟100ms */
//        
//    SWIM_InitHard();           /* 进入swd debug状态 */
//    
//    SWIM_EntrySequence();    
//    
//    return 1;
//}

///*
//*********************************************************************************************************
//*    函 数 名: h7swim_WriteMemory
//*    功能说明: 写CPU内存（或寄存器）
//*    形    参: addr : 目标地址
//*                data : 数据缓冲区，含长度
//*    返 回 值: 0 失败   1 成功
//*********************************************************************************************************
//*/
//static int h7swim_WriteMemory(lua_State* L)
//{
//    uint32_t addr = 0;
//    const char *data;
//    size_t len = 0;

//    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
//    {
//        addr = luaL_checknumber(L, 1);    /* 目标内存地址 */
//    }
//    else
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//        return 1;        
//    }

//    if (lua_type(L, 2) == LUA_TSTRING)     /* 判断第2个参数 */
//    {        
//        data = luaL_checklstring(L, 2, &len); /* 1是参数的位置， len是string的长度 */        
//    }
//    
//    if (len > 128 * 1024)
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//        return 1;
//    }

//    if (SWIM_WriteBuf(addr, (uint8_t *)data, len) == 0)
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//    }
//    else
//    {
//        lua_pushnumber(L, 1);    /* 成功 */
//    }
//        
//    return 1;
//}

///*
//*********************************************************************************************************
//*    函 数 名: h7swim_ProgFlash
//*    功能说明: 编程flash
//*    形    参: addr : 目标地址
//*                data : 数据缓冲区，含长度
//*    返 回 值: 0 失败   1 成功
//*********************************************************************************************************
//*/
//static int h7swim_ProgFlash(lua_State* L)
//{
//    uint32_t addr = 0;
//    const char *data;
//    size_t len = 0;

//    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
//    {
//        addr = luaL_checknumber(L, 1);    /* 目标内存地址 */
//    }
//    else
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//        return 1;        
//    }

//    if (lua_type(L, 2) == LUA_TSTRING)     /* 判断第2个参数 */
//    {        
//        data = luaL_checklstring(L, 2, &len); /* 1是参数的位置， len是string的长度 */        
//    }
//    
//    if (len > 128 * 1024)
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//        return 1;
//    }
//    
//    STM8_FLASH_Unlock();
//    
//    if (STM8_FLASH_ProgramBuf(addr, (uint8_t *)data, len) == 0)
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//    }
//    else
//    {
//        lua_pushnumber(L, 1);    /* 成功 */
//    }
//        
//    return 1;
//}

///*
//*********************************************************************************************************
//*    函 数 名: h7swim_ProgOptionBytes
//*    功能说明: 编程option bytes
//*    形    参: addr : 目标地址
//*                data : 数据缓冲区，含长度
//*    返 回 值: 0 失败   1 成功
//*********************************************************************************************************
//*/
//static int h7swim_ProgOptionBytes(lua_State* L)
//{
//    uint32_t addr = 0;
//    const char *data;
//    size_t len = 0;
//    uint8_t buf[2048];

//    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
//    {
//        addr = luaL_checknumber(L, 1);    /* 目标内存地址 */
//    }
//    else
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//        return 1;        
//    }

//    if (lua_type(L, 2) == LUA_TSTRING)     /* 判断第2个参数 */
//    {        
//        data = luaL_checklstring(L, 2, &len); /* 1是参数的位置， len是string的长度 */        
//    }
//    
//    if (len > 128 * 1024)
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//        return 1;
//    }    
//     
//    /* 将字符换转换为2进制数组 
//        "FF 00 12 34"   -> 0xFF,0x00,0x12,0x34
//    */
//    len = AsciiToHex((char *)data, buf, sizeof(buf));    
//    
//    if (STM8_ProgramOptionBytes(addr, (uint8_t *)buf, len) == 0)
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//    }
//    else
//    {
//        lua_pushnumber(L, 1);    /* 成功 */
//    }
//        
//    return 1;
//}

///*
//*********************************************************************************************************
//*    函 数 名: h7swim_ReadMemory
//*    功能说明: 读CPU内存（或寄存器）
//*    形    参: addr : 目标地址
//*                data : 数据缓冲区，含长度
//*    返 回 值: 0 失败   1 成功
//*********************************************************************************************************
//*/
//static int h7swim_ReadMemory(lua_State* L)
//{
//    uint32_t addr;
//    uint32_t num;

//    if (lua_type(L, 1) == LUA_TNUMBER)     /* 判断第1个参数 */
//    {        
//        addr = luaL_checknumber(L, 1); /* 1是参数的位置， len是string的长度 */        
//    }
//    else
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//        return 1;
//    }
//    
//    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
//    {
//        num = luaL_checknumber(L, 2);
//        
//        memset(s_lua_read_buf, 0, num);        
//    }
//    
//    if (num > LUA_READ_LEN_MAX)
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//        return 1;
//    }

//    if (SWIM_ReadBuf(addr, s_lua_read_buf, num) == 0)
//    {
//        lua_pushnumber(L, 0);    /* 出错 */
//    }
//    else
//    {
//        lua_pushnumber(L, 1);    /* 成功 */
//    }

//    lua_pushlstring(L, (char *)s_lua_read_buf, num); 
//    return 2;
//}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
