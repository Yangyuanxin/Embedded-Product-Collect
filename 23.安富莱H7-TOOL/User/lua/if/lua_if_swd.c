/*
*********************************************************************************************************
*
*    模块名称 : lua接口SWD烧录器器
*    文件名称 : lua_if_swd.c
*    版    本 : V1.0
*    说    明 : 
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-10-06 armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "elf_file.h"
#include "target_reset.h"
#include "target_config.h"
#include "swd_host.h"
#include "swd_host_multi.h"
#include "Systick_Handler.h"
#include "main.h"
#include "target_family.h"
#include "stm8_flash.h"
#include "stm8_swim.h"
#include "swd_flash.h"
#include "SW_DP_Multi.h"
#include "debug_cm.h"
#include "n76e003_flash.h"
#include "cx32_isp.h"
#include "w25q_flash.h"

/* 为了避免和DAP驱动中的函数混淆，本模块函数名前缀用 h7swd */

static int h7swd_Init(lua_State* L);
static int h7swd_ReadID(lua_State* L);
static int h7swd_WriteMemory(lua_State* L);
static int h7swd_ReadMemory(lua_State* L);

static int h7swd_Write32(lua_State* L);
static int h7swd_Read32(lua_State* L);
static int h7swd_Write16(lua_State* L);
static int h7swd_Read16(lua_State* L);
static int h7swd_Write8(lua_State* L);
static int h7swd_Read8(lua_State* L);

static int h7_LoadAlgoFile(lua_State* L);
static int h7_ProgFile(lua_State* L);
static int h7_ProgBuf(lua_State* L);
static int h7_ProgBuf_OB(lua_State* L);
static int h7_Read_OptionBytes(lua_State* L);
static int h7_reset(lua_State* L);
static int h7_reset_pin(lua_State* L);
static int h7_DetectIC(lua_State* L);
static int h7_PrintText(lua_State* L);
static int h7_EraseChip(lua_State* L);
static int h7_EraseSector(lua_State* L);

static int h7_Read_ProductSN(lua_State* L);
static int h7_Write_ProductSN(lua_State* L);
static int h7_ReloadLuaVar(lua_State* L);
static int h7_ReadCVar(lua_State* L);

static int h7swd_ReadExtID(lua_State* L);

static int h7_N76E_Iap(lua_State* L);
static int h7_Iap(lua_State* L);

/*
*********************************************************************************************************
*    函 数 名: lua_swd_RegisterFun
*    功能说明: 注册lua C语言接口函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void PG_PrintText(char *_str);
void lua_swd_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
//    lua_register(g_Lua, "swd_init", h7swd_Init);    
//    lua_register(g_Lua, "swd_getid", h7swd_ReadID);
//    
//    lua_register(g_Lua, "swd_write", h7swd_WriteMemory);    
//    lua_register(g_Lua, "swd_read", h7swd_ReadMemory);

    lua_register(g_Lua, "pg_init", h7swd_Init);    
    lua_register(g_Lua, "pg_get_chip_id", h7swd_ReadID);    
    lua_register(g_Lua, "pg_write_mem", h7swd_WriteMemory); 	
    lua_register(g_Lua, "pg_read_mem", h7swd_ReadMemory);  

    lua_register(g_Lua, "pg_write32", h7swd_Write32);    
    lua_register(g_Lua, "pg_read32", h7swd_Read32);  
    lua_register(g_Lua, "pg_write16", h7swd_Write16);    
    lua_register(g_Lua, "pg_read16", h7swd_Read16);  
    lua_register(g_Lua, "pg_write8", h7swd_Write8);    
    lua_register(g_Lua, "pg_read8", h7swd_Read8);
    
    lua_register(g_Lua, "pg_get_ext_id", h7swd_ReadExtID);       

    lua_register(g_Lua, "pg_load_algo_file", h7_LoadAlgoFile);
    lua_register(g_Lua, "pg_prog_file", h7_ProgFile);
    lua_register(g_Lua, "pg_prog_buf", h7_ProgBuf);
    lua_register(g_Lua, "pg_prog_buf_ob", h7_ProgBuf_OB);
    lua_register(g_Lua, "pg_read_ob", h7_Read_OptionBytes);    
    lua_register(g_Lua, "pg_reset", h7_reset);
    lua_register(g_Lua, "pg_reset_pin", h7_reset_pin);    
    lua_register(g_Lua, "pg_detect_ic", h7_DetectIC);
    lua_register(g_Lua, "pg_erase_chip", h7_EraseChip);   
    lua_register(g_Lua, "pg_erase_sector", h7_EraseSector);       
    
    lua_register(g_Lua, "pg_print_text", h7_PrintText);
    lua_register(g_Lua, "pg_read_sn", h7_Read_ProductSN);
    lua_register(g_Lua, "pg_write_sn", h7_Write_ProductSN);
    
    lua_register(g_Lua, "pg_reload_var", h7_ReloadLuaVar);
    
    lua_register(g_Lua, "pg_read_c_var", h7_ReadCVar);
    
    /* N76E003 */
    lua_register(g_Lua, "pg_n76e_iap", h7_N76E_Iap);
    
    /* 通用的IAP指令接口，第1个形参是CPU型号 第2个形参是指令代码 */
    lua_register(g_Lua, "pg_iap", h7_Iap);
}
   

/*
*********************************************************************************************************
*    函 数 名: h7_ReloadLuaVar
*    功能说明: lua通知程序更新变量
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern uint8_t PG_LuaFixData(void);
static int h7_ReloadLuaVar(lua_State* L)
{
    const char *pVarName;
    size_t len;
    
    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {
        pVarName = luaL_checklstring(L, 1, &len);   /* 1是参数的位置， len是string的长度 */   
            
        if (strcmp(pVarName, "FixData") == 0)
        {
            PG_LuaFixData(); 
        }
        else if (strcmp(pVarName, "ChipInfo") == 0)
        {
            PG_ReloadLuaVar();
        }
        else if (strcmp(pVarName, "StarPorgTime") == 0)
        {
            g_tProg.Time = bsp_GetRunTime();    /* 记录开始时间 */
        }         
    }
    else
    {
        PG_ReloadLuaVar();
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: h7_ReadCVar
*    功能说明: lua读取c全局变量
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int h7_ReadCVar(lua_State* L)
{
    const char *pVarName;
    size_t len;
        
    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {        
        pVarName = luaL_checklstring(L, 1, &len);   /* 1是参数的位置， len是string的长度 */   
        
        if (strcmp(pVarName, "MultiProgMode") == 0)
        {
            lua_pushnumber(L, g_gMulSwd.MultiMode);   
        }
        else if (strcmp(pVarName, "MultiProgError") == 0)
        {
            lua_pushnumber(L, g_gMulSwd.Error[0]);
            lua_pushnumber(L, g_gMulSwd.Error[1]);
            lua_pushnumber(L, g_gMulSwd.Error[2]);
            lua_pushnumber(L, g_gMulSwd.Error[3]);
            return 4;
        }        
        else if (strcmp(pVarName, "ToolSn") == 0)
        {
            lua_pushnumber(L, g_tParam.ToolSn);   
        }
        else if (strcmp(pVarName, "FactoryId") == 0)
        {
            lua_pushnumber(L, g_tParam.FactoryId);   
        }         
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }        
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
    }  
    
    return 1;
}
/*
*********************************************************************************************************
*    函 数 名: h7swd_Init
*    功能说明: 初始化swd. swim. iap.  pg_init()
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int h7swd_Init(lua_State* L)
{
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {   
        sysTickInit();          /* 这是DAP驱动中的初始化函数,全局变量初始化 */

        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            MUL_swd_init_debug();   /* 进入swd debug状态 */    
        }
        else
        {
            swd_init_debug();       /* 进入swd debug状态 */
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {        
        SWIM_InitHard();           /* 进入swd debug状态 */
        
        SWIM_EntrySequence();            
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        N76E_ExitIAP();            /* 进入IAP状态 */

        N76E_EnterIAP();           /* 进入IAP状态 */
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        W25Q_InitHard();
    }
            
    return 0;
}

/* 用于PC机读Flash到文件. 读之前执行一次 */
void pg_init(void)
{
    GetChipTypeFromLua(g_Lua);
    h7swd_Init(0);
}
   
/*
*********************************************************************************************************
*    函 数 名: h7swd_ReadID
*    功能说明: 读芯片ID
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int h7swd_ReadID(lua_State* L)
{
//    uint32_t id;
//    uint32_t id_buf[4];

//    if (g_tProg.ChipType == CHIP_SWD_ARM)
//    {
//		if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
//        {
//	        if (MUL_swd_read_idcode(id_buf) == 0)        
//	        {
//	            lua_pushnumber(L, 0);    /* 出错 */
//	        }
//	        else
//	        {
//	            lua_pushnumber(L, id);    /* 成功,返回ID */
//	        }             
//		}
//		else
//		{
//	        if (swd_read_idcode(&id) == 0)   
//	        {
//	            lua_pushnumber(L, 0);    /* 出错 */
//	        }
//	        else
//	        {
//	            lua_pushnumber(L, id);    /* 成功,返回ID */
//	        }			
//		}
//    }
//    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
//    {
//        id = 0x00000800;            /* STM8没有ID, 填一个固定值 */
//        lua_pushnumber(L, id);      /* 成功,返回ID */        
//    }
//    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
//    {
//        W25Q_DetectIC(id_buf);
//		if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
//        {
//	        if (MUL_swd_read_idcode(id_buf) == 0)        
//	        {
//	            lua_pushnumber(L, 0);    /* 出错 */
//	        }
//	        else
//	        {
//	            lua_pushnumber(L, id);    /* 成功,返回ID */
//	        }             
//		}
//		else
//		{
//	        if (swd_read_idcode(&id) == 0)   
//	        {
//	            lua_pushnumber(L, 0);    /* 出错 */
//	        }
//	        else
//	        {
//	            lua_pushnumber(L, id);    /* 成功,返回ID */
//	        }			
//		}        
//    }    
//    else
//    {
//        id = 0;
//        lua_pushnumber(L, 0);    /* 出错 */
//    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_WriteMemory
*    功能说明: 写CPU内存（或寄存器）
*    形    参: addr : 目标地址
*                data : 数据缓冲区，含长度
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7swd_WriteMemory(lua_State* L)
{
    uint32_t addr = 0;
    const char *data;
    size_t len = 0;
    
//    sysTickInit();    /* 这是DAP驱动中的初始化函数,全局变量初始化 */

    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);    /* 目标内存地址 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;        
    }

    if (lua_type(L, 2) == LUA_TSTRING)     /* 判断第2个参数 */
    {        
        data = luaL_checklstring(L, 2, &len); /* 1是参数的位置， len是string的长度 */        
    }
    
    if (len > 128 * 1024)
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    if (g_tProg.ChipType == CHIP_SWD_ARM) 
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            if (MUL_swd_write_memory(addr, (uint8_t *)data, len) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }
        }
        else
        {
            if (swd_write_memory(addr, (uint8_t *)data, len) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }                       
        }            
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_WriteBuf(addr, (uint8_t *)data, len) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */       
    }    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_ReadMemory
*    功能说明: 读CPU内存（或寄存器）
*    形    参: addr : 目标地址
*                data : 数据缓冲区，含长度
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7swd_ReadMemory(lua_State* L)
{
    uint32_t addr;
    uint32_t num;

    if (lua_type(L, 1) == LUA_TNUMBER)     /* 判断第1个参数 */
    {        
        addr = luaL_checknumber(L, 1); /* 1是参数的位置， len是string的长度 */        
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        num = luaL_checknumber(L, 2);
        
        memset(s_lua_read_buf, 0, num);        
    }
    
    if (num > LUA_READ_LEN_MAX)
    {
        lua_pushnumber(L, 0);    /* 长度溢出，出错 */
        return 1;
    }
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {    
            if (MUL_swd_read_memory(addr, s_lua_read_buf, num) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }
            
            lua_pushlstring(L, (char *)s_lua_read_buf, num); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + num), num); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 2 * num), num); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 3 * num), num); 
            return 5;
        }
        else
        {            
            if (swd_read_memory(addr, s_lua_read_buf, num) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }

            lua_pushlstring(L, (char *)s_lua_read_buf, num); 
            
            return 2;
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_ReadBuf(addr, s_lua_read_buf, num) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
        
        lua_pushlstring(L, (char *)s_lua_read_buf, num); 
        return 2;        
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        if (N76E_ReadBuf(addr, s_lua_read_buf, num) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
        
        lua_pushlstring(L, (char *)s_lua_read_buf, num); 
        return 2;        
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {    
            if (W25Q_ReadBuf(addr, s_lua_read_buf, num) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }
            
            lua_pushlstring(L, (char *)s_lua_read_buf, num); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + num), num); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 2 * num), num); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 3 * num), num); 
            return 5;
        }
        else
        {            
            if (W25Q_ReadBuf(addr, s_lua_read_buf, num) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }

            lua_pushlstring(L, (char *)s_lua_read_buf, num); 
            
            return 2;
        }      
    }     
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */   
        return 1;
    }   
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_Write32
*    功能说明: 写CPU内存（或寄存器）
*    形    参: addr : 目标地址
*              data : 数据
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7swd_Write32(lua_State* L)
{
    uint32_t addr = 0;
    uint32_t data;

    if (lua_type(L, 1) == LUA_TNUMBER) 		/* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);      /* 目标内存地址 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;        
    }

    if (lua_type(L, 2) == LUA_TNUMBER)      /* 判断第2个参数 */
    {        
        data = luaL_checknumber(L, 2);      /* 目标内存地址 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;        
    }    
    
    if (g_tProg.ChipType == CHIP_SWD_ARM) 
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            if (MUL_swd_write_memory(addr, (uint8_t *)&data, 4) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }
        }
        else
        {
            if (swd_write_memory(addr, (uint8_t *)&data, 4) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }                       
        }            
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_WriteBuf(addr, (uint8_t *)&data, 4) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */       
    }    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_Write16
*    功能说明: 写CPU内存（或寄存器）
*    形    参: addr : 目标地址
*              data : 数据
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7swd_Write16(lua_State* L)
{
    uint32_t addr = 0;
    uint16_t data;

    if (lua_type(L, 1) == LUA_TNUMBER) 		/* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);      /* 目标内存地址 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;        
    }

    if (lua_type(L, 2) == LUA_TNUMBER)      /* 判断第2个参数 */
    {        
        data = luaL_checknumber(L, 2);      /* 目标内存地址 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;        
    }    
    
    if (g_tProg.ChipType == CHIP_SWD_ARM) 
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            if (MUL_swd_write_memory(addr, (uint8_t *)&data, 2) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }
        }
        else
        {
            if (swd_write_memory(addr, (uint8_t *)&data, 2) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }                       
        }            
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_WriteBuf(addr, (uint8_t *)&data, 2) == 2)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */       
    }    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_Write8
*    功能说明: 写CPU内存（或寄存器）
*    形    参: addr : 目标地址
*              data : 数据
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7swd_Write8(lua_State* L)
{
    uint32_t addr = 0;
    uint8_t data;

    if (lua_type(L, 1) == LUA_TNUMBER) 		/* 判断第1个参数 */
    {
        addr = luaL_checknumber(L, 1);      /* 目标内存地址 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;        
    }

    if (lua_type(L, 2) == LUA_TNUMBER)      /* 判断第2个参数 */
    {        
        data = luaL_checknumber(L, 2);      /* 目标内存地址 */
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;        
    }    
    
    if (g_tProg.ChipType == CHIP_SWD_ARM) 
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            if (MUL_swd_write_memory(addr, (uint8_t *)&data, 1) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }
        }
        else
        {
            if (swd_write_memory(addr, (uint8_t *)&data, 1) == 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }                       
        }            
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_WriteBuf(addr, (uint8_t *)&data, 1) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */       
    }    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_Read32
*    功能说明: 读CPU内存（或寄存器）
*    形    参: addr : 目标地址
*              data : 数据缓冲区，含长度
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7swd_Read32(lua_State* L)
{
    uint32_t addr;
    uint32_t num = 4;
    uint8_t err = 0;

    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 */
    {        
        addr = luaL_checknumber(L, 1);      /* 1是参数的位置， len是string的长度 */        
    }
    else
    {
        return 0;   /* 返回 nil */
    }
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {    
            memset(s_lua_read_buf, 0, 4 * num);
            if (MUL_swd_read_memory(addr, s_lua_read_buf, num) == 0)
            {
                err = 1;
            }

            lua_pushnumber(L, *(uint32_t *)s_lua_read_buf);
            if (g_gMulSwd.MultiMode >= 2)
            {
                lua_pushnumber(L, *(uint32_t *)(s_lua_read_buf + num));
            }
            if (g_gMulSwd.MultiMode >= 3)
            {            
                lua_pushnumber(L, *(uint32_t *)(s_lua_read_buf + 2 * num));
            }
            if (g_gMulSwd.MultiMode >= 4)
            {            
                lua_pushnumber(L, *(uint32_t *)(s_lua_read_buf + 3 * num));
            }
            if (err == 1)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }            
            return g_gMulSwd.MultiMode + 1;
        }
        else
        {            
            if (swd_read_memory(addr, s_lua_read_buf, num) == 0)
            {
                err = 1;    /* 出错 */
            }
            
            lua_pushnumber(L, *(uint32_t *)s_lua_read_buf); 
            
            if (err == 1)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }             
            return 2;
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_ReadBuf(addr, s_lua_read_buf, num) == 0)
        {
            err = 1;    /* 出错 */
        }
        lua_pushnumber(L, *(uint32_t *)s_lua_read_buf); 
        
        if (err == 1)        
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
        return 2;        
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        lua_pushnumber(L, 0);    /* 出错 */ 
        return 2;
    }
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_Read16
*    功能说明: 读CPU内存（或寄存器）
*    形    参: addr : 目标地址
*              data : 数据缓冲区，含长度
*    返 回 值: 读回的数据。出错也返回0
*********************************************************************************************************
*/
static int h7swd_Read16(lua_State* L)
{
    uint32_t addr;
    uint32_t num = 2;
    uint8_t err = 0;

    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 */
    {        
        addr = luaL_checknumber(L, 1);      /* 1是参数的位置， len是string的长度 */        
    }
    else
    {
        return 0;   /* 返回 nil */
    }
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {    
            memset(s_lua_read_buf, 0, 4 * num);
            if (MUL_swd_read_memory(addr, s_lua_read_buf, num) == 0)
            {
                err = 1;
            }

            lua_pushnumber(L, *(uint16_t *)s_lua_read_buf);
            if (g_gMulSwd.MultiMode >= 2) lua_pushnumber(L, *(uint16_t *)(s_lua_read_buf + num));
            if (g_gMulSwd.MultiMode >= 3) lua_pushnumber(L, *(uint16_t *)(s_lua_read_buf + 2 * num));
            if (g_gMulSwd.MultiMode >= 4) lua_pushnumber(L, *(uint16_t *)(s_lua_read_buf + 3 * num));
            
            if (err == 1)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }            
            return g_gMulSwd.MultiMode + 1;
        }
        else
        {            
            if (swd_read_memory(addr, s_lua_read_buf, num) == 0)
            {
                err = 1;    /* 出错 */
            }
            
            lua_pushnumber(L, *(uint16_t *)s_lua_read_buf); 
            
            if (err == 1)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }             
            return 2;
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_ReadBuf(addr, s_lua_read_buf, num) == 0)
        {
            err = 1;    /* 出错 */
        }
        lua_pushnumber(L, *(uint16_t *)s_lua_read_buf); 
        
        if (err == 1)        
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
        return 2;        
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        lua_pushnumber(L, 0);    /* 出错 */ 
        return 2;
    }
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_Read8
*    功能说明: 读CPU内存（或寄存器）
*    形    参: addr : 目标地址
*              data : 数据缓冲区，含长度
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7swd_Read8(lua_State* L)
{
     uint32_t addr;
    uint32_t num = 1;
    uint8_t err = 0;

    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 */
    {        
        addr = luaL_checknumber(L, 1);      /* 1是参数的位置， len是string的长度 */        
    }
    else
    {
        return 0;   /* 返回 nil */
    }
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {    
            memset(s_lua_read_buf, 0, 4 * num);
            if (MUL_swd_read_memory(addr, s_lua_read_buf, num) == 0)
            {
                err = 1;
            }

            lua_pushnumber(L, *(uint8_t *)s_lua_read_buf);
            if (g_gMulSwd.MultiMode >= 2) lua_pushnumber(L, *(uint8_t *)(s_lua_read_buf + num));
            if (g_gMulSwd.MultiMode >= 3) lua_pushnumber(L, *(uint8_t *)(s_lua_read_buf + 2 * num));
            if (g_gMulSwd.MultiMode >= 4) lua_pushnumber(L, *(uint8_t *)(s_lua_read_buf + 3 * num));
            
            if (err == 1)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }            
            return g_gMulSwd.MultiMode + 1;
        }
        else
        {            
            if (swd_read_memory(addr, s_lua_read_buf, num) == 0)
            {
                err = 1;    /* 出错 */
            }
            
            lua_pushnumber(L, *(uint8_t *)s_lua_read_buf); 
            
            if (err == 1)
            {
                lua_pushnumber(L, 0);    /* 出错 */
            }
            else
            {
                lua_pushnumber(L, 1);    /* 成功 */
            }             
            return 2;
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_ReadBuf(addr, s_lua_read_buf, num) == 0)
        {
            err = 1;    /* 出错 */
        }
        lua_pushnumber(L, *(uint8_t *)s_lua_read_buf); 
        
        if (err == 1)        
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }
        return 2;        
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        lua_pushnumber(L, 0);    /* 出错 */ 
        return 2;
    } 
}

/*
*********************************************************************************************************
*    函 数 名: h7_LoadAlgoFile
*    功能说明: 装载算法文件
*    形    参: file_path 算法文件路径，是相对路径
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7_LoadAlgoFile(lua_State* L)
{
    // pg_load_algo_file(AlgoFile, AlgoRamAddr, AlgoRamSize)
    const char *data;
    size_t len = 0;

    g_AlgoRam.Valid = 0;
    
    if (lua_type(L, 1) == LUA_TSTRING)     /* 判断第1个参数 */
    {        
        data = luaL_checklstring(L, 1, &len); /* 1是参数的位置， len是string的长度 */   
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    if (lua_type(L, 2) == LUA_TNUMBER) /* 判断第2个参数 */
    {
        g_AlgoRam.Addr = luaL_checknumber(L, 2);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }    
    
    if (lua_type(L, 3) == LUA_TNUMBER) /* 判断第3个参数 */
    {
        g_AlgoRam.Size = luaL_checknumber(L, 3);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    if (lua_type(L, 4) == LUA_TNUMBER) /* 判断第4个参数 (一般不用）*/
    {
        g_tVar.SaveAlgoToCFile = luaL_checknumber(L, 4);
    }
    else
    {
        g_tVar.SaveAlgoToCFile = 0;
    }
    
    g_AlgoRam.Valid = 1;
    
    /* lua程序提供的路径是相对路径 */
    {
        if (data[0] == '0' && data[1] == ':')     /* 是绝对路径 */
        {
            if (ELF_ParseFile((char *)data) != 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
                return 1;
            }
        }
        else    /* 是相对路径 */ 
        {    
            char path[256];
            
            GetDirOfFileName(g_tProg.FilePath, path);   /* 从lua文件名、中获取目录 */
            
            if (strlen(path) + strlen(data) + 1 > sizeof(path))
            {
                printf("FLM file name is too long\r\n");
                lua_pushnumber(L, 0);    /* 路径过长出错 */
                return 1;                
            }
            
            strcat(path, "/");
            strcat(path, data);

            FixFileName(path);  /* 去掉路径中的..上级目录 */ 
            
            if (ELF_ParseFile((char *)path) != 0)
            {
                lua_pushnumber(L, 0);    /* 出错 */
                return 1;
            }            
        }    
    }
    
    lua_pushnumber(L, 1);    /*OK */
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: h7_ProgFile
*    功能说明: 开始编程，从文件读取数据。 
*    形    参: file_path 数据文件名称, 目标flash地址
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
extern uint16_t PG_ProgFile(const char *_Path, uint32_t _FlashAddr, uint32_t _EndAddr, uint32_t _CtrlByte, 
    uint32_t _FileIndex, const char *_AlgoName);
static int h7_ProgFile(lua_State* L)
{
    const char *file_name;
    const char *AlgoName;
    size_t len = 0;
    uint32_t FlashAddr;
    uint32_t EndAddr;
    uint32_t CtrlByte;
	uint32_t FileIndex = 0;
    
    /*
        lua调用:
        pg_prog_file(TaskList[i + 1], TaskList[i + 2], EndAddress, TaskList[i + 3])
    */
    if (lua_type(L, 1) == LUA_TSTRING)      /* 判断第1个参数 - 文件名 */
    {        
        file_name = luaL_checklstring(L, 1, &len); /* 1是参数的位置， len是string的长度 */   
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    if (lua_type(L, 2) == LUA_TNUMBER)      /* 判断第2个参数 - 开始地址 */
    {
        FlashAddr = luaL_checknumber(L, 2);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    } 

    if (lua_type(L, 3) == LUA_TNUMBER)      /* 判断第3个参数 - 结束地址 */
    {
        EndAddr = luaL_checknumber(L, 3);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    } 
    
    if (lua_type(L, 4) == LUA_TNUMBER)      /* 判断第4个参数 - 控制字节， bit0=1表示整片擦除， 0按扇区擦除 */
    {
        CtrlByte = luaL_checknumber(L, 4);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }    

    if (lua_type(L, 5) == LUA_TNUMBER)      /* 判断第5个参数 - 文件序号，用于滚码 */
    {
        FileIndex = luaL_checknumber(L, 5);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }  
       
    if (lua_type(L, 1) == LUA_TSTRING)      /* 判断第6个参数 - 算法文件名 */
    {        
        AlgoName = luaL_checklstring(L, 6, &len); /* 1是参数的位置， len是string的长度 */   
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }     
    
    /* 开始编程文件 */
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {    
        if (PG_SWD_ProgFile((char *)file_name, FlashAddr, EndAddr, CtrlByte, FileIndex) == 0)
        {
            lua_pushnumber(L, 1);    /*OK */
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (PG_SWIM_ProgFile((char *)file_name, FlashAddr, EndAddr, CtrlByte, FileIndex) == 0)
        {
            lua_pushnumber(L, 1);    /*OK */
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }        
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        if (PG_ProgFile(file_name, FlashAddr, EndAddr, CtrlByte, FileIndex, AlgoName) == 0)
        {
            lua_pushnumber(L, 1);    /*OK */
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }        
    }     
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        if (PG_ProgFile(file_name, FlashAddr, EndAddr, CtrlByte, FileIndex, AlgoName) == 0)
        {
            lua_pushnumber(L, 1);    /*OK */
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }        
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
    }    
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: h7_reset
*    功能说明: 执行硬件复位
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void MUL_swd_set_target_reset(uint8_t asserted);
#define NVIC_Addr    (0xe000e000)
static int h7_reset(lua_State* L)
{   
    uint32_t delay;
    
    
    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        delay = luaL_checknumber(L, 1);       
    }
    else
    {
        delay = 20;     /* 没有形参，则用20ms */
    } 
    
    printf("hardware reset %dms\r\n", delay);
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {    
        /* 硬件复位 */
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            /*　V1.34 ： 备份错误标志 */
            uint8_t Bak[4];            
            
            Bak[0] = g_gMulSwd.Error[0];
            Bak[1] = g_gMulSwd.Error[1];
            Bak[2] = g_gMulSwd.Error[2];
            Bak[3] = g_gMulSwd.Error[3];
            
            MUL_swd_set_target_reset(1);
            
            // Perform a soft reset
            {
                uint32_t val[4];
                
                MUL_swd_read_word(NVIC_AIRCR, val);

                MUL_swd_write_word(NVIC_AIRCR, VECTKEY | (val[0] & SCB_AIRCR_PRIGROUP_Msk) | SYSRESETREQ);
            }
            
            osDelay(delay);        
     
            MUL_swd_set_target_reset(0);
            osDelay(delay);
            
            g_gMulSwd.Error[0] = Bak[0];
            g_gMulSwd.Error[1] = Bak[1];
            g_gMulSwd.Error[2] = Bak[2];
            g_gMulSwd.Error[3] = Bak[3];            
        }
        else        
        {
            swd_set_target_reset(1);
            // Perform a soft reset
            {
                uint32_t val;
                
                swd_read_word(NVIC_AIRCR, &val);

                swd_write_word(NVIC_AIRCR, VECTKEY | (val & SCB_AIRCR_PRIGROUP_Msk) | SYSRESETREQ);
            }
            
            osDelay(delay);
            swd_set_target_reset(0);
            osDelay(delay); 
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        SWIM_SetResetPin(0);
        bsp_DelayMS(delay);
        SWIM_SetResetPin(1);
        bsp_DelayMS(delay);
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        N76E_SetResetPin(0);
        N76E_ExitIAP();         /* 退出IAP状态 */
        bsp_DelayMS(delay);         
        N76E_SetResetPin(1);
        bsp_DelayMS(delay);
    }        

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: h7_reset_pin
*    功能说明: 执行硬件复位
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int h7_reset_pin(lua_State* L)
{   
    uint32_t state;
        
    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        state = luaL_checknumber(L, 1);       
    }
    else
    {
        return 0;
    }    
    
    /* 硬件复位 */
    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        MUL_swd_set_target_reset(state);
    }
    else        
    {
        swd_set_target_reset(state);
    }    
    return 0;
}


/*
*********************************************************************************************************
*    函 数 名: h7_DetectIC
*    功能说明: 检测CPU ID
*    形    参: 无
*    返 回 值: 0 失败   > 0 成功
*********************************************************************************************************
*/
static int h7_DetectIC(lua_State* L)
{
    uint32_t mode = 0;

    if (lua_type(L, 1) == LUA_TNUMBER)  /* 判断第1个参数 */
    {
        mode = luaL_checknumber(L, 1);  /* 1表示仅用于检测芯片插入状态, 无需检测3次 */      
    }
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {            
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            uint8_t i;
            uint32_t id[4] = {0}; 
            
            if (mode == 1)  /* 仅仅用于识别芯片是否在位 */
            {
                //MUL_swd_detect_core(id);
                MUL_swd_init_debug();           /* 进入swd debug状态 */
                MUL_swd_read_idcode(id);
            }
            else    /* 用于可靠识别芯片ID */
            {            
                sysTickInit();    /* 这是DAP驱动中的初始化函数,全局变量初始化 */            
                for (i = 0; i < 3; i++)
                {               
                    MUL_swd_init_debug();           /* 进入swd debug状态 */
                    
                    if (MUL_swd_read_idcode(id) == 0)
                    {
                        //id = 0;     /* 出错继续检测 */
                    }
                    else
                    {
                        break;            
                    }
                    bsp_DelayUS(5 * 1000);     /* 延迟5ms */                
                }            
            }
            lua_pushnumber(L, id[0]);    /* 成功,返回ID */
            lua_pushnumber(L, id[1]);    /* 成功,返回ID */
            lua_pushnumber(L, id[2]);    /* 成功,返回ID */
            lua_pushnumber(L, id[3]);    /* 成功,返回ID */            
            return 4;            
        }
        else
        {            
            uint8_t i;
            uint32_t id = 0;
            
            if (mode == 1)  /* 仅仅用于识别芯片是否在位 */
            {
                swd_detect_core(&id);
            }
            else    /* 用于可靠识别芯片ID */
            {                  
                sysTickInit();    /* 这是DAP驱动中的初始化函数,全局变量初始化 */
                for (i = 0; i < 3; i++)
                {           
                    swd_init_debug();           /* 进入swd debug状态 */
                    
                    if (swd_read_idcode(&id) == 0)
                    {
                        id = 0;     /* 出错继续检测 */
                    }
                    else
                    {
                        break;            
                    }
                    bsp_DelayUS(5 * 1000);     /* 延迟5ms */
                }
            }            
            lua_pushnumber(L, id);    /* 成功,返回ID */
            return 1;            
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8) 
    {
        uint32_t id;
        
        if (SWIM_DetectIC(&id) == 0)
        {
            id = 0;     /* 失败 */
        }        
        lua_pushnumber(L, id);      /* 成功,返回ID */
        return 1;
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051) 
    {
        uint32_t id;
        
        if (N76E_DetectIC(&id) == 0)
        {
            id = 0;     /* 失败 */
        }        
        lua_pushnumber(L, id);      /* 成功,返回ID */
        return 1;
    } 
    else if (g_tProg.ChipType == CHIP_SPI_FLASH) 
    {
        uint32_t id[4];
        
        W25Q_InitHard();
        W25Q_DetectIC(id);
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            lua_pushnumber(L, id[0]);    /* 成功,返回ID */
            lua_pushnumber(L, id[1]);    /* 成功,返回ID */
            lua_pushnumber(L, id[2]);    /* 成功,返回ID */
            lua_pushnumber(L, id[3]);    /* 成功,返回ID */            
            return 4;            
        }
        else
        {                
            lua_pushnumber(L, id[0]);    /* 成功,返回ID */
            return 1;
        }
    } 
    
    lua_pushnumber(L, -1);          /* 失败 */
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: h7_PrintText
*    功能说明: 在LCD显示一串文本
*    形    参: 无
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7_PrintText(lua_State* L)
{
    const char *str;
     size_t len = 0;
    
    if (lua_type(L, 1) == LUA_TSTRING)     /* 判断第1个参数 */
    {        
        str = luaL_checklstring(L, 1, &len); /* 1是参数的位置， len是string的长度 */   
        
        if (g_MainStatus == MS_PROG_WORK)
        {
            printf(str);
            printf("\r\n");
                    
            if (g_MainStatus == MS_PROG_WORK)
            {
                DispProgProgress((char *)str, -1, 0xFFFFFFFF);      /* -1表示不刷新进度 */
            }
        }
    }

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: h7_ProgBuf
*    功能说明: 开始编程，从新参读入数据。
*    形    参: Flash地址
*			   数据
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7_ProgBuf(lua_State* L)
{
    const char *str;
    uint8_t buf[2048];
    size_t len = 0;
    uint32_t FlashAddr;   
    uint8_t mode;    
    
    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 */
    {
        FlashAddr = luaL_checknumber(L, 1);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    } 
    
    if (lua_type(L, 2) == LUA_TSTRING)     /* 判断第2个参数 */
    {        
        str = luaL_checklstring(L, 2, &len); /* 2是参数的位置， len是string的长度 */   
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    /* 将字符换转换为2进制数组 
        "FF 00 12 34"   -> 0xFF,0x00,0x12,0x34
    */
    len = AsciiToHex((char *)str, buf, sizeof(buf));

    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        if (lua_type(L, 3) == LUA_TNUMBER)      /* 判断第3个参数 */
        {
            mode = luaL_checknumber(L, 3);       
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
            return 1;
        }
    
        if (PG_SWD_ProgBuf(FlashAddr, buf, len, mode) == 0)
        {    
            lua_pushnumber(L, 1);    /*OK */
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        STM8_FLASH_Unlock();
        
        if (STM8_FLASH_ProgramBuf(FlashAddr, buf, len) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
    }
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: h7_ProgBuf_OB
*    功能说明: 开始编程，从新参读入数据。 用于option bytes
*    形    参: Flash地址
*			   数据
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7_ProgBuf_OB(lua_State* L)
{
    const char *str;
    uint8_t DataBuf[1024];
    uint8_t DataBuf2[1024];
    size_t DataLen = 0;   
    uint8_t AddrBuf[2048];  /* 选项字的地址表，大端模式 */
    size_t AddrLen = 0;

    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {
        str = luaL_checklstring(L, 1, &AddrLen);    /* 1是参数的位置， len是string的长度 */ 
        AddrLen = AsciiToHex((char *)str, AddrBuf, sizeof(AddrBuf));
        
        if ((AddrLen % 2) != 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
            return 1;           
        }
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    if (lua_type(L, 2) == LUA_TSTRING)          /* 判断第2个参数 */
    {        
        str = luaL_checklstring(L, 2, &DataLen); /* 2是参数的位置， len是string的长度 */ 
        /* 将字符换转换为2进制数组 "FF 00 12 34"   -> 0xFF,0x00,0x12,0x34 */
        DataLen = AsciiToHex((char *)str, DataBuf, sizeof(DataBuf));            
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
        
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        uint32_t HeadAddress;
        uint32_t Address;
        uint16_t len = 0;
        uint8_t * pAddr = AddrBuf;
        uint8_t * pData = DataBuf;                    
        
        HeadAddress = BEBufToUint32(AddrBuf);   /* ARM编程OPTION BYTES只需要首地址 */
        while (AddrLen)
        {            
            Address = BEBufToUint32(pAddr); 
            pAddr += 4;               
            AddrLen -= 4;
                            
            if (Address == 0xFFFFFFFF)              /* 遇到0xFFFFFFFF,则插入前第1个数据取反的数据 */
            {
                if (len > 0)
                {
                    DataBuf2[len++] = ~DataBuf2[len - 1];
                }
            }
            else if (Address == 0xFFFFFFFE)          /* 遇到0xFFFFFFFE,则插入前第2个数据取反的数据 */
            {
                if (len >= 2)
                {
                    DataBuf2[len++] = ~DataBuf2[len - 2];
                }
            }
            else if (Address == 0xFFFFFFFD)          /* 遇到0xFFFFFFFD,则插入前第4个数据取反的数据 */
            {
                if (len >= 4)
                {
                    DataBuf2[len++] = ~DataBuf2[len - 4];
                }
            }             
            else
            {    
                DataBuf2[len++] = *pData++;                 
            }
        }
        
        if (PG_SWD_ProgBuf_OB(HeadAddress, DataBuf2, len) == 0)
        {    
            lua_pushnumber(L, 1);    /* OK */
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (STM8_ProgramOptionBytes(AddrBuf, AddrLen, DataBuf, DataLen) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
    }    
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: h7_Read_OptionBytes
*    功能说明: 根据地址列表，读取 option bytes
*    形    参: Flash地址
*			   数据
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
#define MUL_BUF_STEP    (LUA_READ_LEN_MAX / 4)        /* s_lua_read_buf 平分4组存放结果 */
static int h7_Read_OptionBytes(lua_State* L)
{
    const char *str; 
    uint8_t AddrBuf[2048];  /* 选项字的地址表，大端模式 */
    size_t AddrLen = 0;
    uint8_t err = 0;
    uint16_t len = 0;
    uint8_t OneByteMode = 0;
    uint16_t offset = 0;
    uint8_t ByteBuf[4] = {0};   
    
    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {
        str = luaL_checklstring(L, 1, &AddrLen);    /* 1是参数的位置， len是string的长度 */ 
        AddrLen = AsciiToHex((char *)str, AddrBuf, sizeof(AddrBuf));
        
        if ((AddrLen % 2) != 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
            return 1;           
        }
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    /* 如果有第2个参数，则返回1个字节 */
    if (lua_type(L, 2) == LUA_TNUMBER)      /* 判断第3个参数 */
    {
        offset = luaL_checknumber(L, 2);  
        OneByteMode = 1;    /* 单字节模式 */    
    }
    else
    {
        OneByteMode = 0;    /* 完整数据 */
    }    
        
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        uint32_t Address;
        uint32_t LastAddress;
        uint8_t * pAddr = AddrBuf;
        uint32_t TempAddr;
       
        while (AddrLen)
        {            
            Address = BEBufToUint32(pAddr); 
            pAddr += 4;               
            AddrLen -= 4;
                            
            if (Address == 0xFFFFFFFF)              /* 遇到0xFFFFFFFF,则插入一个取反的数据 */
            {
                TempAddr = LastAddress + 1;
            }
            else if (Address == 0xFFFFFFFE)         /* 遇到0xFFFFFFFE,则插入前第2个数据取反的数据 */
            {
                TempAddr = LastAddress + 1;
                LastAddress++;
            }
            else if (Address == 0xFFFFFFFD)         /* 遇到0xFFFFFFFD,则插入前第4个数据取反的数据 */
            {
                TempAddr = LastAddress + 4;
                LastAddress++;
            }            
            else
            {
                TempAddr = Address;               
                LastAddress = Address;               
            }

            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {    
                uint8_t buf[4];
                
                if (MUL_swd_read_memory(TempAddr, buf, 1) == 0)
                {
                    err = 1;
                    break;
                }                    
                s_lua_read_buf[0 * MUL_BUF_STEP + len] = buf[0];
                s_lua_read_buf[1 * MUL_BUF_STEP + len] = buf[1];
                s_lua_read_buf[2 * MUL_BUF_STEP + len] = buf[2];
                s_lua_read_buf[3 * MUL_BUF_STEP + len] = buf[3];

                if (OneByteMode == 1)   /* 单字节模式 */
                {
                    if (len == offset)
                    {
                        ByteBuf[0] = buf[0];
                        ByteBuf[1] = buf[1];
                        ByteBuf[2] = buf[2];
                        ByteBuf[3] = buf[3];
                    }
                }                
            }
            else    /* 单路模式 */
            {
                if (swd_read_memory(TempAddr, &s_lua_read_buf[len], 1) == 0)
                {
                    err = 1;
                    break;
                }
                
                if (OneByteMode == 1)   /* 单字节模式 */
                {
                    if (len == offset)
                    {
                        ByteBuf[0] = s_lua_read_buf[len];
                    }
                }
            }
            len++;            
        }      
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        uint32_t Address;
        uint32_t LastAddress;
        uint8_t * pAddr = AddrBuf;
        
        while (AddrLen)
        {            
            Address = BEBufToUint16(pAddr); 
            pAddr += 2;               
            AddrLen -= 2;
                            
            if (Address == 0xFFFF)          /* 遇到0xFFFFFFFF,则插入一个取反的数据 */
            {
                if (SWIM_ReadBuf(LastAddress + 1, &s_lua_read_buf[len], 1) == 0)
                {
                    err = 1;
                    break;
                }
            }
            else
            {
                if (SWIM_ReadBuf(Address, &s_lua_read_buf[len], 1) == 0)
                {
                    err = 1;
                    break;
                }
                LastAddress = Address;               
            }
                        
            if (OneByteMode == 1)   /* 单字节模式 */
            {
                if (len == offset)
                {
                    ByteBuf[0] = s_lua_read_buf[len];
                }
            }            
            len++;
        }        
    }
    else
    {
        err = 1;
    }
    
    if (err != 0)
    {
        lua_pushnumber(L, 0);    /* 出错 */
    }
    else
    {
        lua_pushnumber(L, 1);    /* 成功 */
    }

    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        if (OneByteMode == 1)   /* 单字节模式 */
        {       
            s_lua_read_buf[0] = ByteBuf[0];
            s_lua_read_buf[1] = ByteBuf[1];
            s_lua_read_buf[2] = ByteBuf[2];            
            s_lua_read_buf[3] = ByteBuf[3];                        
            
            lua_pushlstring(L, (char *)(s_lua_read_buf + 0 * 1), 1); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 1 * 1), 1); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 2 * 1), 1); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 3 * 1), 1);
        }
        else    /* 完整模式 */
        {
            lua_pushlstring(L, (char *)(s_lua_read_buf + 0 * MUL_BUF_STEP), len); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 1 * MUL_BUF_STEP), len); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 2 * MUL_BUF_STEP), len); 
            lua_pushlstring(L, (char *)(s_lua_read_buf + 3 * MUL_BUF_STEP), len);            
        }
        return 5;         
    }
    else
    {    
        if (OneByteMode == 1)   /* 单字节模式 */
        {
            s_lua_read_buf[0] = ByteBuf[0];
            lua_pushlstring(L, (char *)s_lua_read_buf, 1);
        }
        else    /* 完整模式 */
        {
            lua_pushlstring(L, (char *)s_lua_read_buf, len);
        }
        return 2;         
    }
   
}

/*
*********************************************************************************************************
*    函 数 名: h7_EraseChip
*    功能说明: 擦除全片，用于OPTION BYTES编程，去除读保护。
*    形    参: Flash地址
*			   数据
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7_EraseChip(lua_State* L)
{
    uint32_t FlashAddr;     

    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 */
    {
        FlashAddr = luaL_checknumber(L, 1);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }

    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {       
        if (PG_SWD_EraseChip(FlashAddr) == 0)
        {    
            lua_pushnumber(L, 1);    /* OK */
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (STM8_FLASH_EraseChip(FlashAddr) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
    }    
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: h7_EraseSector
*    功能说明: 擦除扇区
*    形    参: Flash地址
*			   数据
*    返 回 值: 0 失败   1 成功
*********************************************************************************************************
*/
static int h7_EraseSector(lua_State* L)
{
    uint32_t FlashAddr;     

    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 */
    {
        FlashAddr = luaL_checknumber(L, 1);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }

    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {       
        if (PG_SWD_EraseSector(FlashAddr) == 0)
        {    
            lua_pushnumber(L, 1);    /* OK */
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (STM8_FLASH_EraseSector(FlashAddr) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }       
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        if (W25Q_EraseSector(FlashAddr) == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, 1);    /* 成功 */
        }       
    }    
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
    }    
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: h7_Read_ProductSN
*    功能说明: 读产品序号
*    形    参: 无
*    返 回 值: 序号（整数）
*********************************************************************************************************
*/
static int h7_Read_ProductSN(lua_State* L)
{
    lua_pushnumber(L, g_tProgIni.ProductSN);
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: h7_Write_ProductSN
*    功能说明: 写产品序号
*    形    参: 无
*    返 回 值: 序号（整数）
*********************************************************************************************************
*/
static int h7_Write_ProductSN(lua_State* L)
{
    uint32_t sn;
    
    if (lua_type(L, 1) == LUA_TNUMBER)      /* 判断第1个参数 */
    {
        sn = luaL_checknumber(L, 1);       
    }
    else
    {
        lua_pushnumber(L, 0);    /* 出错 */
        return 1;
    }
    
    g_tProgIni.ProductSN = sn;    
    lua_pushnumber(L, 1);
    return 1;    
}

/*
*********************************************************************************************************
*    函 数 名: h7swd_ReadExtID
*    功能说明: 读CPU外部芯片的ID
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int h7swd_ReadExtID(lua_State* L)
{
    uint32_t id = 0;

    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        if (swd_init_debug() == 0)
        {
            id = 0;
        }
        
        id = target_flash_init(0x90000000, 0, FLM_INIT_ERASE);

        /* SWD进入debug状态 */
        target_flash_enter_debug_program();
        /* 装载算法代码到目标机内存 */
        LoadAlgoToTarget();
        target_flash_init(0x90000000, 0, FLM_INIT_ERASE);
        
        id = target_flash_read_extid(0x90000000);
    
        if (id == 0)
        {
            lua_pushnumber(L, 0);    /* 出错 */
        }
        else
        {
            lua_pushnumber(L, id);    /* 成功,返回ID */
        } 
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        id = 0x00000000;            /* STM8没有ID, 填一个固定值 */
        lua_pushnumber(L, id);      /* 成功,返回ID */        
    }
    else
    {
        id = 0;
        lua_pushnumber(L, 0);    /* 出错 */
    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: h7_N76E_Iap
*    功能说明: N76E003，发IAP指令. 暂时不支持多路烧录
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int h7_N76E_Iap(lua_State* L)
{
    /*
        ob_read = pg_n76e_iap("read_cfg_all");
        pg_n76e_iap("write_cfg", ob);
        return pg_n76e_iap("erase_chip");
        return pg_n76e_iap("read_cfg_all");
        return pg_n76e_iap("read_cfg_byte", addr);
        return pg_n76e_iap("read_uid");
    */
    const char *StrCmd;
    size_t len;    
    
    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {
        StrCmd = luaL_checklstring(L, 1, &len);    /* 1是参数的位置， len是string的长度 */       
    }
    else
    {
        return 0;
    }
    
    if (strcmp(StrCmd, "enter_iap") == 0)
    {
        N76E_EnterIAP();
        return 0;
    }
    else if (strcmp(StrCmd, "exit_iap") == 0)
    {
        N76E_EnterIAP();
        return 0;
    }    
    else if (strcmp(StrCmd, "read_cfg_all") == 0)
    {
        //uint8_t N76E_ReadCfg(uint8_t _addr, uint8_t _len, uint8_t *_cfg)
        uint8_t ob_buf[8];
        
        N76E_ReadCfg(0, 8, ob_buf);
        
        lua_pushnumber(L, 1);    /* OK */
        lua_pushlstring(L, (char *)ob_buf, 8);
        return 2;
    }
    else if (strcmp(StrCmd, "read_cfg_byte") == 0)
    {
        uint16_t addr;
        uint8_t re;
        
        if (lua_type(L, 2) == LUA_TNUMBER)      /* 判断第1个参数 */
        {
            addr = luaL_checknumber(L, 2);       
        }
        else
        {
            lua_pushnumber(L, 0);    /* 出错 */
            return 1;
        }

        N76E_ReadCfg(addr, 1, &re);
        
        lua_pushnumber(L, 1);       /* OK */
        lua_pushnumber(L, re);      /* 读回字节 */
        return 2;        
    }     
    else if (strcmp(StrCmd, "write_cfg") == 0)
    {
        const char *cfg;
        uint8_t DataBuf[1024];
        
        cfg = luaL_checklstring(L, 2, &len);    /* 1是参数的位置， len是string的长度 */       
        
        /* 将字符换转换为2进制数组 "FF 00 12 34"   -> 0xFF,0x00,0x12,0x34 */
        AsciiToHex((char *)cfg, DataBuf, 8);         
        
        N76E_EraseCfg();
        N76E_ProgramCfg((uint8_t *)DataBuf);
        return 0;
    }
    else if (strcmp(StrCmd, "erase_chip") == 0)
    {
        N76E_EraseChip();       
        return 0;        
    }  
    else if (strcmp(StrCmd, "read_uid") == 0)
    {
        ;
    } 

    return 0;        
}

/*
*********************************************************************************************************
*    函 数 名: h7_Iap
*    功能说明: 发IAP指令. 暂时不支持多路烧录
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int h7_Iap(lua_State* L)
{
    /*

    */
    const char *StrChip;
    const char *StrCmd;
    size_t len;    
    
    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {
        StrChip = luaL_checklstring(L, 1, &len);    /* 1是参数的位置， len是string的长度 */       
    }
    
    if (lua_type(L, 2) == LUA_TSTRING)              /* 判断第1个参数 */
    {
        StrCmd = luaL_checklstring(L, 2, &len);    /* 1是参数的位置， len是string的长度 */       
    }
    else
    {
        return 0;
    }
    
    if (strcmp(StrChip, "CX32L003") == 0)
    {
        if (strcmp(StrCmd, "remove_swd_lock") == 0)
        {
            CX32_RemoveSwdLock();
            return 0;
        }
    }
    else if (strcmp(StrChip, "HS32F0") == 0)
    {
        if (strcmp(StrCmd, "remove_swd_lock") == 0)
        {
            CX32_RemoveSwdLock();
            return 0;
        }
    }
    return 0;        
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
