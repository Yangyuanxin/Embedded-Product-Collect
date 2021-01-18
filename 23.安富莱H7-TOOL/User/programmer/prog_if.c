/*
*********************************************************************************************************
*
*    模块名称 : 编程器接口文件
*    文件名称 : prog_if.c
*    版    本 : V1.0
*    说    明 : 
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-03-19  armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"

#include "swd_host.h"
#include "swd_flash.h"
#include "elf_file.h"
#include "stm8_flash.h"
#include "n76e003_flash.h"
#include "SW_DP_Multi.h"
#include "swd_host_multi.h"
#include "w25q_flash.h"

extern const program_target_t flash_algo;

OFFLINE_PROG_T g_tProg = {0};

PROG_INI_T g_tProgIni = {0};

FIX_DATA_T g_tFixData = {0};

uint8_t flash_buff[sizeof(FsReadBuf)];

uint8_t PG_CheckFlashFix(uint32_t _FlashAddr, uint32_t _BuffSize, uint32_t _FileIndex);
uint8_t PG_CheckFixSplit(uint32_t _FlashAddr, uint32_t _BuffSize, uint32_t _FileIndex);
uint8_t PG_FixFlashMem(uint32_t _FlashAddr, char *_Buff, uint32_t _BuffSize, uint32_t _FileIndex, uint8_t _chan);

/*
*********************************************************************************************************
*    函 数 名: CancelKey
*    功能说明: 终止键按下
*    形    参: 无
*    返 回 值: 1表示需要立即终止
*********************************************************************************************************
*/
uint8_t ProgCancelKey(void)
{    
    if (bsp_GetKey() == KEY_LONG_DOWN_C)
    {
        return 1;
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: GetChipTypeFromLua
*    功能说明: 从lua中解析芯片类型. 存放在全局变量 g_tProg.ChipType
*    形    参: 无
*    返 回 值: 1表示需要立即终止
*********************************************************************************************************
*/
uint32_t GetChipTypeFromLua(lua_State *L)
{
    g_tProg.ChipType = CHIP_SWD_ARM;
    
    if (L > 0)
    {
        /* 器件接口类型: "SWD", "SWIM", "SPI", "I2C" */
        if (lua_getglobal(L, "CHIP_TYPE") != 0)
        {
            const char *name;
             
            if (lua_isstring(g_Lua, -1)) 
            {
                name = lua_tostring(g_Lua, -1);
                if (strcmp(name, "SWD") == 0)
                {
                    g_tProg.ChipType = CHIP_SWD_ARM;
                }
                else if (strcmp(name, "SWIM") == 0)
                {
                    g_tProg.ChipType = CHIP_SWIM_STM8;
                }            
                else if (strcmp(name, "SPI") == 0)
                {
                    g_tProg.ChipType = CHIP_SPI_FLASH;
                } 
                else if (strcmp(name, "I2C") == 0)
                {
                    g_tProg.ChipType = CHIP_I2C_EEPROM;
                }  
                else if (strcmp(name, "NUVOTON_8051") == 0)
                {
                    g_tProg.ChipType = CHIP_NUVOTON_8051;
                }                 
            }           
        }
        lua_pop(L, 1);
    }
    return g_tProg.ChipType;
}        

/*
*********************************************************************************************************
*    函 数 名: WaitChipRemove
*    功能说明: 检测芯片移除状态
*    形    参: 无
*    返 回 值: 1表示已经移除，0表示芯片还在位
*********************************************************************************************************
*/
extern void sysTickInit(void);
uint8_t WaitChipRemove(void)
{    
    /* 由LUA程序提供函数 */ 
    const char *ret_str;

    lua_do("ret_str = CheckChipRemove()"); 
    lua_getglobal(g_Lua, "ret_str"); 
    if (lua_isstring(g_Lua, -1))
    {
        ret_str = lua_tostring(g_Lua, -1); 
    }
    else
    {
        ret_str = "";
    }
    lua_pop(g_Lua, 1);
        
    if (strcmp(ret_str, "removed") == 0)
    {
        return 1;
    }

    return 0; 
}

/*
*********************************************************************************************************
*    函 数 名: WaitChipInsert
*    功能说明: 检测芯片插入状态
*    形    参: 无
*    返 回 值: 1表示已经插入，0表示未检测到
*********************************************************************************************************
*/
uint8_t WaitChipInsert(void)
{
    const char *ret_str;

    lua_do("ret_str = CheckChipInsert()"); 
    lua_getglobal(g_Lua, "ret_str"); 
    if (lua_isstring(g_Lua, -1))
    {
        ret_str = lua_tostring(g_Lua, -1); 
    }
    else
    {
        ret_str = "";
    }
    lua_pop(g_Lua, 1);
        
    if (strcmp(ret_str, "inserted") == 0)
    {
        return 1;
    }

    return 0;   
}

/*
*********************************************************************************************************
*    函 数 名: PG_ReloadLuaVar
*    功能说明: 读取lua文件全局变量. 
*    形    参: _str : 字符串
*    返 回 值: 无
*********************************************************************************************************
*/
extern void h7swim_ReadLuaVar(void);
void PG_ReloadLuaVar(void)
{
    GetChipTypeFromLua(g_Lua);

    /* VERIFY_MODE;  校验模式 */
    g_tProg.VerifyMode = lua_GetVarUint32("VERIFY_MODE", VERIFY_READ_BACK);

    /* ERASE_CHIP_TIME;  全片擦除时间ms */
    g_tProg.EraseChipTime = lua_GetVarUint32("ERASE_CHIP_TIME", 60 * 1000);      
    
    /* 0B VERIFY;  option byte 验证取消 */
    g_tProg.VerifyOptionByteDisalbe = lua_GetVarUint32("OB_VERIFY_DISABLE", 0);
    
    /*  SwdClockDelay;  SWD接口时序延迟 */
    g_tProg.SwdClockDelay = lua_GetVarUint32("SWD_CLOCK_DELAY", 0);
    
    /*  复位延迟 */
    g_tProg.SwdResetDelay = lua_GetVarUint32("RESET_DELAY", 20); 

    /* 读取复位类型： 软件还是硬件复位 */
    g_tProg.ResetMode = lua_GetVarUint32("RESET_MODE", 0); 

    /* 多路模式下，出错后立刻同时终止 */
    g_tProg.AbortOnError = lua_GetVarUint32("ABORT_ON_ERROR", 1);
        
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {    
        ;
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)  
    {
        h7swim_ReadLuaVar();        /* 读取LUA中的全局变量 */
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)  
    {
        //    uint32_t Capacity;          /* 芯片容量 */
        //    uint32_t SectorSize;        /* 扇区大小 */
        //    uint32_t EraseSectorCmd;    /* 扇区擦除指令 */
        //    uint32_t EraseSectorTimeout;    /* 擦除扇区超时 ms */
        //    uint32_t EraseChipTimeout;  /* 擦除整片超时 ms */
        //    uint32_t ProgPageTimeout;   /* 编程page超时 */
        //    uint8_t  AutoAddrInc;       /* AAI模式写 */
        
        g_tW25Q.Capacity = lua_GetVarUint32("FLASH_SIZE", 0);
        g_tW25Q.SectorSize = lua_GetVarUint32("SECTOR_SIZE", 4096);
        g_tW25Q.EraseSectorCmd = lua_GetVarUint32("ERASE_SECTOR_CMD", 0x20);
        g_tW25Q.EraseSectorTimeout = lua_GetVarUint32("ERASE_SECTOR_TIMEOUT", 2 * 1000);
        g_tW25Q.EraseChipTimeout = lua_GetVarUint32("ERASE_CHIP_TIMEOUT", 400 * 1000);
        g_tW25Q.ProgPageTimeout = 100;   /* 100ms */        
        g_tW25Q.AutoAddrInc = lua_GetVarUint32("PROG_AAI", 0);        
        g_tW25Q.ReadMode = lua_GetVarUint32("READ_MODE", 0);
        g_tW25Q.ReadIDCmd = lua_GetVarUint32("READ_ID_CMD", 0x9F); 
        g_tW25Q.EraseChipCmd = lua_GetVarUint32("ERASE_CHIP_CMD", 0xC7);
        g_tW25Q.UnlockCmd = lua_GetVarUint32("UNLOCK_CMD", 0x00);
        
    }    
}

/*
*********************************************************************************************************
*    函 数 名: PG_LuaFixData
*    功能说明: 在编程函数调用。用于动态生成UID,SN,USR数据
*    形    参:  _Path : 文件名
*               _FlashAddr : flash起始地址
*    返 回 值: 0 = ok, 其他表示错误
*********************************************************************************************************
*/
uint8_t PG_LuaFixData(void)
{   
    size_t len;
    int i;
    
    //lua FIX_TABLE结构
    //1, 0, 0x08000000, 05 00 00 00 
    //1, 1, 0x08000000, 02 
    //1, 2, 0x08000000, FF 
    //1, 3, 0x08000000, 01 
    //1, 1, 0x08000000, FB 32 C9 38 BE C8 06 F1 
    //1, 1, 0x08000000, 92 06 02 0E 90 7C C4 02 90 00 4A 0E 96 58 C4 02 
    
    //lua->stack，得到全局表，位置-1
    lua_getglobal(g_Lua, "FIX_TABLE");
    g_tFixData.Count = 0;
    for (i = 0; i < MAX_FIX_LINE; i++)
    {
        lua_pushinteger(g_Lua, i * 4 + 1);  //c->statck，设置key值，位置-1(上面的-1变为-2)
        lua_gettable(g_Lua, -2);//返回值为值的类型        
        if (lua_isinteger(g_Lua, -1))
        {
            g_tFixData.Lines[i].Enable = lua_tointeger(g_Lua, -1);
            lua_pop(g_Lua, 1);            
        }
        else
        {
            lua_pop(g_Lua, 1);
            break;
        }
        
        lua_pushinteger(g_Lua, i * 4 + 2);  //c->statck，设置key值，位置-1(上面的-1变为-2)
        lua_gettable(g_Lua, -2);//返回值为值的类型        
        if (lua_isinteger(g_Lua, -1))
        {
            g_tFixData.Lines[i].Chan = lua_tointeger(g_Lua, -1);
            lua_pop(g_Lua, 1);            
        }
        else
        {
            lua_pop(g_Lua, 1);
            break;
        }
        
        lua_pushinteger(g_Lua, i * 4 + 3);  //c->statck，设置key值，位置-1(上面的-1变为-2)
        lua_gettable(g_Lua, -2);//返回值为值的类型        
        if (lua_isinteger(g_Lua, -1))
        {
            g_tFixData.Lines[i].Addr = lua_tointeger(g_Lua, -1);
        }
        lua_pop(g_Lua, 1);

        lua_pushinteger(g_Lua, i * 4 + 4);  //c->statck，设置key值，位置-1(上面的-1变为-2)
        lua_gettable(g_Lua, -2);//返回值为值的类型        
        if (lua_isstring(g_Lua, -1))
        {
            g_tFixData.Lines[i].pData = luaL_checklstring(g_Lua, -1, &len); /* 1是参数的位置， len是string的长度 */  
            g_tFixData.Lines[i].Len = len;
        }
        lua_pop(g_Lua, 1);  

        g_tFixData.Count++;
    }
    
    lua_pop(g_Lua, 1);
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: PG_PrintText
*    功能说明: 烧录过程输出消息
*    形    参: _str : 字符串
*    返 回 值: 无
*********************************************************************************************************
*/
void PG_PrintText(char *_str)
{
    char str[128];
    
    /* 输出文本 */
    StrUTF8ToGBK(_str, str, sizeof(str));

    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
    {
        if (g_gMulSwd.Error[0] != 0)
        {
            strcat(str, " #1");
        }
        if (g_gMulSwd.Error[1] != 0)
        {
            strcat(str, " #2");
        }
        if (g_gMulSwd.Error[2] != 0)
        {
            strcat(str, " #3");
        }
        if (g_gMulSwd.Error[3] != 0)
        {
            strcat(str, " #4");
        }          
    }           
    
    if (g_MainStatus == MS_PROG_WORK)
    {
        DispProgProgress(str, -1, 0xFFFFFFFF);      /* -1表示不刷新进度 */
    }
    
    strcat(str, "\r\n"); 
    printf(str);    
    
    bsp_Idle();
}

/*
*********************************************************************************************************
*    函 数 名: PG_PrintPercent
*    功能说明: 烧录过程输出进度百分比
*    形    参: 百分比，浮点数
*    返 回 值: 无
*********************************************************************************************************
*/
extern void DispProgVoltCurrent(void);
void PG_PrintPercent(float _Percent, uint32_t _Addr)
{
    #if 1
        if (_Percent == 0)
        {
            printf("  %dms, %0.2f%%\r\n", bsp_CheckRunTime(g_tProg.Time), _Percent);
        }
        else if (_Percent == 100)
        {
            //printf("\r\n  %dms, %0.2f%%\r\n", bsp_CheckRunTime(g_tProg.Time), _Percent);
            //printf("\r\n");
            printf("  %dms, %0.2f%%\r\n", bsp_CheckRunTime(g_tProg.Time), _Percent);
        }
        else
        {
            printf(".");
        }
    #else
        printf("  %dms, %0.2f%%\r\n", bsp_CheckRunTime(g_tProg.Time), _Percent);
    #endif
    if (g_MainStatus == MS_PROG_WORK)
    {       
        g_tProg.Percent = _Percent;
        
        DispProgProgress(0, _Percent, _Addr);   /* 0表示不刷新文本 */
        
        DispProgVoltCurrent();                  /* 刷新TVCC电压和电流 */
    }
    
    bsp_Idle();
}

/*
*********************************************************************************************************
*    函 数 名: PG_FixFlashMem
*    功能说明: 编程过程中修改数据，实现滚码、加密等功能。暂时只支持单通道。
*    形    参:  _Path : 文件名
*               _FlashAddr : flash起始地址
*               _BuffSize : 数据大小
*               _chan : 通道号0，1-4
*    返 回 值: 0表示地址不在范围内。 1表示在范围内，并已修改好内存
*********************************************************************************************************
*/
uint8_t PG_FixFlashMem(uint32_t _FlashAddr, char *_Buff, uint32_t _BuffSize, uint32_t _FileIndex, uint8_t _chan)
{
    uint16_t m;
    uint16_t i;
    uint8_t change = 0;
    
    for (m = 0; m < g_tFixData.Count; m++)
    {
        if (g_tFixData.Lines[m].Enable == _FileIndex)
        {
            if (g_tFixData.Lines[m].Chan == 0 || _chan == 0 || g_tFixData.Lines[m].Chan == _chan) 
            {
                for (i = 0; i < g_tFixData.Lines[m].Len; i++)
                {
                    if (g_tFixData.Lines[m].Addr + i >= _FlashAddr && g_tFixData.Lines[m].Addr + i < _FlashAddr + _BuffSize)
                    {
                        _Buff[g_tFixData.Lines[m].Addr - _FlashAddr + i] = g_tFixData.Lines[m].pData[i];
                        change = 1;
                    }
                }
            }
        }
    }
    
    return change;
}

/*
*********************************************************************************************************
*    函 数 名: PG_CheckFlashFix
*    功能说明: 判断flash区域是否存在fix区数据
*    形    参:  _Path : 文件名
*               _FlashAddr : flash起始地址
*               _FileIndex : 文件编号（1-10)
*    返 回 值: 0表示地址不在范围内。 1表示在范围内
*********************************************************************************************************
*/
uint8_t PG_CheckFlashFix(uint32_t _FlashAddr, uint32_t _BuffSize, uint32_t _FileIndex)
{
    uint16_t m;
    uint8_t change = 0;
    
    /*
        add    0000000000     0000000000    0000000000      0000000000
        fix  11111             11111               11111     111111111111
    */
    if (_BuffSize == 0)
    {
        return 0;
    }
    
    for (m = 0; m < g_tFixData.Count; m++)
    {
        if (g_tFixData.Lines[m].Enable == _FileIndex
            && g_tFixData.Lines[m].Addr + g_tFixData.Lines[m].Len > _FlashAddr 
            && g_tFixData.Lines[m].Addr < _FlashAddr + _BuffSize)
        {
            change = 1;
            break;
        }
    }
    return change;
}

/*
*********************************************************************************************************
*    函 数 名: PG_CheckFixSplit
*    功能说明: 判断fix区数据是否需要顺序写入
*    形    参:  _Path : 文件名
*               _FlashAddr : flash起始地址
*               _FileIndex : 文件编号（1-10)
*    返 回 值: 0表示地址不在范围内。 1表示在范围内,每个通道数据不一致，需要顺序写入
*********************************************************************************************************
*/
uint8_t PG_CheckFixSplit(uint32_t _FlashAddr, uint32_t _BuffSize, uint32_t _FileIndex)
{
    uint16_t m;
    uint8_t change = 0;
    
    /*
        add    0000000000     0000000000    0000000000      0000000000
        fix  11111             11111               11111     111111111111
    */
    
    for (m = 0; m < g_tFixData.Count; m++)
    {
        if (g_tFixData.Lines[m].Enable == _FileIndex
            && g_tFixData.Lines[m].Addr + g_tFixData.Lines[m].Len > _FlashAddr 
            && g_tFixData.Lines[m].Addr < _FlashAddr + _BuffSize)
        {
            if (g_tFixData.Lines[m].Chan >= 2)
            {
                change = 1;
                break;
            }
        }
    }
    return change;
}

/*
*********************************************************************************************************
*    函 数 名: PG_FixDataIsDiff
*    功能说明: 判断FIX数据是否每个通道都一样
*    形    参: 无
*    返 回 值: 0表示相同，1表示不同（需要顺序编程）
*********************************************************************************************************
*/
uint8_t PG_FixDataIsDiff(void)
{
    uint16_t m;
    
    //    --返回C程序用的table，结构为 {
    //    --  0, 0x08000000, "1234567",   0表示全部通道数据一样， 1表示第1通，4表示4通道
    //    --  1, 0x08000200, "AAA",
    //    --  2, 0x08000300, "BBBB",
    //    -- }

    for (m = 0; m < g_tFixData.Count; m++)
    {        
        if (g_tFixData.Lines[m].Chan >= 2)
        {
            return 1;       /* 至少2个通道存在不同的数据 */
        }        
    }
    
    return 0;
}

/*
*********************************************************************************************************
*   下面代码，用于通用芯片编程. 被 pg_prog_file.c 文件调用
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*    函 数 名: PG_GetSectorSize
*    功能说明: 获得扇区大小
*    形    参: _Addr : 地址
*    返 回 值: 扇区大小
*********************************************************************************************************
*/
uint32_t PG_GetSectorSize(const char *_Algo, uint32_t _Addr)
{    
    uint32_t sz;
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        sz = 1024;
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        sz = lua_GetVarUint32("FLASH_BLOCK_SIZE", 64);
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        sz = 128;
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        sz = lua_GetVarUint32("SECTOR_SIZE", 4*1024);
    }     
    return sz;
}

/*
*********************************************************************************************************
*    函 数 名: PG_EraseSector
*    功能说明: 擦除扇区 通用芯片API
*    形    参: _Addr : 地址
*    返 回 值: 0表示失败 1表示OK
*********************************************************************************************************
*/
uint8_t PG_EraseSector(const char *_Algo, uint32_t _Addr)
{
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        /* ARM不用这个文件 */;
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        /* STM8都是整片擦除 */;
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        /* 选择整片擦除 */
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        W25Q_EraseSector(_Addr);
    }
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: PG_GetPageSize
*    功能说明: 获得编程page大小
*    形    参: 无
*    返 回 值: 页大小
*********************************************************************************************************
*/
uint32_t PG_GetPageSize(const char *_Algo)
{
    uint32_t sz;
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        sz = 1024;
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        sz = lua_GetVarUint32("FLASH_BLOCK_SIZE", 64);
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        sz = 32;
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        sz = lua_GetVarUint32("FLASH_PAGE_SIZE", 1024);;
    }     
    return sz;
}

/*
*********************************************************************************************************
*    函 数 名: PG_GetDeviceAddr
*    功能说明: 获得芯片起始地址
*    形    参: 无
*    返 回 值: 芯片起始地址
*********************************************************************************************************
*/
uint32_t PG_GetDeviceAddr(const char *_Algo)
{
    uint32_t addr;
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        addr = 0x08000000;
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (strcmp(_Algo, "FLASH") == 0)
        {
            addr = lua_GetVarUint32("FLASH_ADDRESS", 0);
        }
        else if (strcmp(_Algo, "EEPROM") == 0)
        {
            addr = lua_GetVarUint32("EEPROM_ADDRESS", 0);
        }
        else
        {
            addr = 0x008000;
        }        
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        if (strcmp(_Algo, "APROM") == 0)
        {
            addr = lua_GetVarUint32("APROM_ADDRESS", 0);
        }
        else if (strcmp(_Algo, "LDROM") == 0)
        {
            addr = lua_GetVarUint32("LDROM_ADDRESS", 0);
        }
        else if (strcmp(_Algo, "SPROM") == 0)
        {
            addr = lua_GetVarUint32("SPROM_ADDRESS", 0);
        }      
        else
        {
            addr = 0;
        }
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        addr = lua_GetVarUint32("FLASH_ADDRESS", 0);
    }
    return addr;
}

/*
*********************************************************************************************************
*    函 数 名: PG_GetDeviceSize
*    功能说明: 获得芯片容量大小
*    形    参: 无
*    返 回 值: 页大小
*********************************************************************************************************
*/
uint32_t PG_GetDeviceSize(const char *_Algo)
{
    uint32_t sz;
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        sz = 0x08000000;
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (strcmp(_Algo, "FLASH") == 0)
        {
            sz = lua_GetVarUint32("FLASH_SIZE", 0);
        }
        else if (strcmp(_Algo, "EEPROM") == 0)
        {
            sz = lua_GetVarUint32("EEPROM_SIZE", 0);
        }
        else
        {
            sz = 1024;
        }        
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        if (strcmp(_Algo, "APROM") == 0)
        {
            sz = lua_GetVarUint32("APROM_SIZE", 0);
        }
        else if (strcmp(_Algo, "LDROM") == 0)
        {
            sz = lua_GetVarUint32("LDROM_SIZE", 0);
        }
        else if (strcmp(_Algo, "SPROM") == 0)
        {
            sz = lua_GetVarUint32("SPROM_SIZE", 0);
        }      
        else
        {
            sz = 1024;
        }
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        sz = lua_GetVarUint32("FLASH_SIZE", 0);
    }
    return sz;
}

/*
*********************************************************************************************************
*    函 数 名: PG_EraseChip
*    功能说明: 擦除整片 通用芯片API
*    形    参: 无
*    返 回 值: 0表示失败 1表示OK
*********************************************************************************************************
*/
uint8_t PG_EraseChip(void)
{
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        ;
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        N76E_EraseChip();
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        W25Q_EraseChip();
    }  
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: PG_CheckBlank
*    功能说明: 检查空片
*    形    参: 无
*    返 回 值: 0表示不空 1表示空片
*********************************************************************************************************
*/
uint8_t PG_CheckBlank(const char *_Algo, uint32_t _Addr, uint32_t _Size)
{
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        ;
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
       ;
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
       ;
    }    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: PG_ProgramBuf
*    功能说明: Programs a memory block
*    形    参: _FlashAddr : 绝对地址。 
*              _Buff : Pointer to buffer containing source data.
*              _Size : 数据大小，可以大于1个block
*    返 回 值: 0 : 出错;  1 : 成功
*********************************************************************************************************
*/ 
uint8_t PG_ProgramBuf(const char *_Algo, uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size)
{
    uint8_t re = 0;
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        re = STM8_FLASH_ProgramBuf(_FlashAddr, _Buff, _Size);
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        re = N76E_FLASH_ProgramBuf(_FlashAddr, _Buff, _Size);
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        re = W25Q_FLASH_ProgramBuf(_FlashAddr, _Buff, _Size);
    }    
    return re;
}

/*
*********************************************************************************************************
*    函 数 名: PG_ReadBuf
*    功能说明: 读flash
*    形    参: _FlashAddr : 绝对地址。 
*              _Buff : Pointer to buffer containing source data.
*              _Size : 数据大小，可以大于1个block
*    返 回 值: 0 : 出错;  1 : 成功
*********************************************************************************************************
*/ 
uint8_t PG_ReadBuf(const char *_Algo, uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size)
{
    uint8_t re = 0;
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        re = STM8_FLASH_ReadBuf(_FlashAddr, _Buff, _Size);
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051)
    {
        re = N76E_ReadBuf(_FlashAddr, _Buff, _Size);
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH)
    {
        re = W25Q_ReadBuf(_FlashAddr, _Buff, _Size);
    }
    
    return re;   
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
