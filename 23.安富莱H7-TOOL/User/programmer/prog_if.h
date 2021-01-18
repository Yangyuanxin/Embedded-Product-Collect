/*
*********************************************************************************************************
*
*    模块名称 : 编程器接口
*    文件名称 : prog_if.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __PROG_IF_H_
#define __PROG_IF_H_

#include "lua_if.h"

/* */
typedef enum
{
    CHIP_SWD_ARM     = 0,
    CHIP_SWIM_STM8   = 1,
    CHIP_SPI_FLASH   = 2,
    CHIP_I2C_EEPROM  = 3,
    CHIP_NUVOTON_8051  = 4,   /* 新唐51系,如 N76E类型 */
}CHIP_TYPE_E;

/* 校验模式 */
typedef enum
{
    VERIFY_AUTO = 0,            /* 自动选择. FLM中有CRC就执行, 没有就采用ReadBack  */
    VERIFY_READ_BACK  = 1,      /* 读回校验 */
    VERIFY_SOFT_CRC   = 2,      /* 软件CRC */
    VERIFY_STM32_CRC  = 3,      /* STM32 CRC硬件算法 */
}VERIFY_MODE_E;

typedef struct 
{
    char FilePath[128];         /* lua文件路径 */    
    
    CHIP_TYPE_E ChipType;       /* 芯片类型 */
    
    uint32_t Time;
    
    float Percent;              /* 烧录进度 */
    
    uint8_t Err;
    
    uint32_t FLMFuncTimeout;    /* 执行算法函数超时时间。擦除整片时可能耗时20秒 */
    uint8_t FLMFuncDispProgress;/* 临时处理，正在擦除全片， 需要显示进度 */
    uint32_t FLMFuncDispAddr;   /* 进度条中的地址栏 */
    
    uint8_t AutoStart;      /* 检测到芯片后自动开始编程 */
    
    int32_t NowProgCount;   /* 编程次数。掉电清零 */
    
    uint32_t VerifyMode;    /* 校验模式 */
    uint32_t EraseChipTime; /* 全片擦除时间 */
    
    uint8_t VerifyOptionByteDisalbe;   /* 1 表示芯片设置读保护时,不校验 (STM32F207VC设置读保护后会即可生效导致无法校验) */
    
    uint32_t SwdClockDelay;     /* SWD时钟延迟，0表示最快，值越大速度越慢 */
    
    uint32_t SwdResetDelay;     /* 硬件复位后的延迟时间，ms */
    uint32_t ResetMode;         /* 复位模式 */
    
    uint8_t AbortOnError;      /* 多路模式，0表示出错时继续烧录OK的芯片  1表示出错后立刻同时终止 */
    
    uint8_t ChNum;              /* 通道个数 */
    
}OFFLINE_PROG_T;

/* lua脚本fix_data_begin()返回一个table，用于通知C程序哪些内存地址需要填充数据 */
typedef struct
{
	uint8_t Enable;			/* 使能控制，1-10表示写入哪个文件区 */
    uint8_t Chan;           /* 通道0-4, 0表示全部通道，1-4表示对应的某一个通道 */
    uint32_t Addr;          /* 内存地址 */
    const char *pData;      /* 数据指针 */
    uint32_t Len;           /* 数据长度 */
}FIX_CELL_T;

#define MAX_FIX_LINE    128
typedef struct
{
    uint32_t Count;          /* 有效数据个数 */
    FIX_CELL_T Lines[MAX_FIX_LINE];
}FIX_DATA_T;

extern OFFLINE_PROG_T g_tProg;

extern FIX_DATA_T g_tFixData;

void PG_ReloadLuaVar(void);

uint8_t WaitChipInsert(void);
uint8_t WaitChipRemove(void);

uint8_t ProgCancelKey(void);
void PG_PrintText(char *_str);
void PG_PrintPercent(float _Percent, uint32_t _Addr);
uint8_t PG_CheckFlashFix(uint32_t _FlashAddr, uint32_t _BuffSize, uint32_t _FileIndex);
uint8_t PG_CheckFixSplit(uint32_t _FlashAddr, uint32_t _BuffSize, uint32_t _FileIndex);
uint8_t PG_FixFlashMem(uint32_t _FlashAddr, char *_Buff, uint32_t _BuffSize, uint32_t _FileIndex, uint8_t _chan);
    
uint16_t PG_SWD_ProgFile(char *_Path, uint32_t _FlashAddr, uint32_t _EndAddr, uint32_t _CtrlByte, uint32_t _FileIndex);

uint16_t PG_SWD_ProgBuf(uint32_t _FlashAddr, uint8_t *_DataBuf, uint32_t _BufLen, uint8_t _Mode);

uint16_t PG_SWD_ProgBuf_OB(uint32_t _FlashAddr, uint8_t *_DataBuf, uint32_t _BufLen);

uint16_t PG_SWD_EraseChip(uint32_t _FlashAddr);
uint16_t PG_SWD_EraseSector(uint32_t _FlashAddr);

void DispProgProgress(char *_str, float _progress, uint32_t _addr);
uint32_t GetChipTypeFromLua(lua_State *L);

uint32_t PG_GetPageSize(const char *_Algo);
uint32_t PG_GetDeviceAddr(const char *_Algo);
uint32_t PG_GetDeviceSize(const char *_Algo);
uint32_t PG_GetSectorSize(const char *_Algo, uint32_t _Addr);
uint8_t PG_CheckBlank(const char *_Algo, uint32_t _Addr, uint32_t _Size);
uint8_t PG_EraseSector(const char *_Algo, uint32_t _Addr);
uint8_t PG_EraseChip(void);
uint8_t PG_ProgramBuf(const char *_Algo, uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size);
uint8_t PG_ReadBuf(const char *_Algo, uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size);


extern uint8_t flash_buff[16*1024];   /* flash_buff[sizeof(FsReadBuf)]; */

#endif
