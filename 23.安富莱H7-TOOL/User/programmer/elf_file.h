/*
*********************************************************************************************************
*
*    模块名称 : elf文件解析模块
*    文件名称 : elf_file.h
*    说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __ELF_FILE_H_
#define __ELF_FILE_H_

#include "FlashOS.h"

#define FUNC_NUM    10               /* 全局符号个数（函数和一个常量数组） */
#define LOAD_NUM    4               /* 需要加载到RAM的段个数 */

#define SECTOR_INFO_NUM    32       /* 需要加载到RAM的段个数 */

enum
{
    IDX_FlashDevice = 0,
    IDX_Init,
    IDX_UnInit,
    IDX_BlankCheck,
    IDX_EraseChip,
    IDX_EraseSector,
    IDX_ProgramPage,
    IDX_Verify,
    IDX_CaculCRC32,
    IDX_ReadExtID,
};

typedef struct
{
    uint32_t Valid;
    uint32_t Offset;
    uint32_t Size;
}
ELF_FUNC_T;

typedef struct
{
    uint32_t Valid;
    uint32_t Offset;
    uint32_t Addr;
    uint32_t Size;    
}
ELF_LOAD_T;

typedef struct
{
    uint32_t Valid;
    uint32_t Addr;
    uint32_t Size;
}
ELF_RAM_T;

/* FLM文件分析结构 */
typedef struct
{    
    uint32_t FileOk;    
    
    ELF_LOAD_T Load[LOAD_NUM]; 
    
    ELF_FUNC_T Func[FUNC_NUM];  

    FlashDevice_T Device;
}FLM_PARSE_T;    

extern FLM_PARSE_T g_tFLM;
extern ELF_RAM_T g_AlgoRam;

uint8_t ELF_ParseFile(char *_path);
uint8_t LoadAlgoToTarget(void);

#endif
