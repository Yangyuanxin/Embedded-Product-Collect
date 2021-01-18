/*
*********************************************************************************************************
*
*    模块名称 : elf文件解码模块
*    文件名称 : elf_file.c
*    版    本 : V1.0
*    说    明 : 用于解码KEIL中FLM算法文件，提取加载到CPU RAM的代码。
*               FLM文件中的字符串表和符号表长度不能超过4096。
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-12-29  armfly   正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*    本文件为安富莱电子原创，仅供学习使用，禁止在同类产品中商用. 如需使用，需要得到我们的授权.
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "file_lib.h"
#include "elf_file.h"
#include "elf.h"
#include "SWD_flash.h"
#include "swd_host.h"
#include "lua_if.h"
#include "SW_DP_Multi.h"
#include "swd_host_multi.h"
#include "param.h"

/*
Flash Programming Functions (Called by FlashOS)
extern          int  Init        (unsigned long adr,   // Initialize Flash
                                  unsigned long clk,
                                  unsigned long fnc);
extern          int  UnInit      (unsigned long fnc);  // De-initialize Flash
extern          int  BlankCheck  (unsigned long adr,   // Blank Check
                                  unsigned long sz,
                                  unsigned char pat);
extern          int  EraseChip   (void);               // Erase complete Device
extern          int  EraseSector (unsigned long adr);  // Erase Sector Function
extern          int  ProgramPage (unsigned long adr,   // Program Page Function
                                  unsigned long sz,
                                  unsigned char *buf);
extern unsigned long Verify      (unsigned long adr,   // Verify Function
                                  unsigned long sz,
                                  unsigned char *buf);
*/

const char *strFuncName[FUNC_NUM] = {"FlashDevice", "Init", "UnInit", "BlankCheck",
        "EraseChip", "EraseSector", "ProgramPage", "Verify", "STM32_CRC32","ReadExtID"};

FLM_PARSE_T g_tFLM;

#define ALGO_RAM_ADDR   0x2000000
#define ALGO_RAM_SIZE   0x1000
        
program_target_t flash_algo;

ELF_RAM_T g_AlgoRam;               

static uint8_t ELF_FillToAlgo(char *_path, program_target_t *_algo);        

uint8_t SaveAlgoToCFile(char *_path);
uint8_t SaveAlgoToPrint(char *_path);
        
/*
*********************************************************************************************************
*    函 数 名: ELF_ParseFile
*    功能说明: 解析elf文件.  使用了 FsReadBuf[4096]全部变量做缓冲区。解析结果放到全局变量g_tFLM
*    形    参: _path : 文件路径
*    返 回 值: 0 = ok， 其他值表示错误
*********************************************************************************************************
*/
uint8_t ELF_ParseFile(char *_path)
{
    uint32_t bytes;
    uint32_t i,j;
    char *p;
    uint32_t StrFoud[FUNC_NUM] = {0};   
    uint32_t StrIdx[FUNC_NUM] = {0};  
    Elf_Ehdr Ehdr;
    Elf_Phdr *pPhdr;
    Elf_Shdr *pShdr;
    Elf_Sym *pSym;
    
    Elf_Shdr ShdrSym;   /* 符号表头 */
    Elf_Shdr ShdrStr;   /* 字符串表头 */

    /* 解析结果先全部清零 */
    for (i = 0; i < LOAD_NUM; i++)
    {
        g_tFLM.Load[i].Valid = 0;
        g_tFLM.Load[i].Offset = 0;
        g_tFLM.Load[i].Addr = 0;        
        g_tFLM.Load[i].Size = 0;
    }
    
    for (i = 0; i < FUNC_NUM; i++)
    {
        g_tFLM.Func[i].Valid = 0;
        g_tFLM.Func[i].Offset = 0;
        g_tFLM.Func[i].Size = 0;
    }    

    /* 解析文件头部 ELF Header */
    bytes = ReadFileToMem(_path, 0, FsReadBuf, 52);
    if (bytes != 52)
    {
        printf("Read file failed : %s\r\n", _path);
        goto err;   /* 读文件错误 */
    }
    memcpy((char *)&Ehdr, FsReadBuf, sizeof(Elf_Ehdr));
    if (IS_ELF(Ehdr) == 0)
    {
        goto err;   /* 不是ELF文件 */
    }
    
    if (Ehdr.e_type != ET_EXEC)
    {
        goto err;   /* 不是可执行的镜像文件 */
    }    
    
    /* 解析程序头部（Program Header） - 2段 */
    if (Ehdr.e_phnum > LOAD_NUM)
    {
        goto err;   /* Program Header 个数过大 */
    }
    bytes = ReadFileToMem(_path, Ehdr.e_phoff, FsReadBuf, Ehdr.e_phnum * 32);
    if (bytes != Ehdr.e_phnum * 32)
    {
        goto err;   /* 读文件错误 */
    }
    
    for (i = 0; i < Ehdr.e_phnum; i++)
    {
        pPhdr = (Elf_Phdr *)(FsReadBuf + i * 32);
        if (pPhdr->p_type == PT_LOAD)
        {
            g_tFLM.Load[i].Valid = 1;
            g_tFLM.Load[i].Offset = pPhdr->p_offset;
            g_tFLM.Load[i].Addr = pPhdr->p_vaddr;
            g_tFLM.Load[i].Size = pPhdr->p_filesz;
        }
    }
   
    /* 解析节区头部 （Sections Header） */
    if (Ehdr.e_shnum < 25)
    {
        uint8_t found = 0;
        
        bytes = ReadFileToMem(_path, Ehdr.e_shoff, FsReadBuf, 40 * Ehdr.e_shnum);
        
        /* 先查找符号表 */        
        for (i = 0; i < Ehdr.e_shnum; i++)
        {
            pShdr = (Elf_Shdr *)(FsReadBuf + 40 * i);
            if (pShdr->sh_type == SHT_SYMTAB)
            {                
                memcpy((char *)&ShdrSym, (char *)pShdr, sizeof(Elf_Shdr));
                found++;
            }
        }
        if (found == 0)
        {
            goto err;   /* 未找到符号表 */
        }
        
        /* 查找符号表对应的字符串表 （ELF文件中可能有多个字符串表）*/  
        if (ShdrSym.sh_link >= Ehdr.e_shnum)
        {
            goto err;   /* 未找到字符串表 */
        }
        
        pShdr = (Elf_Shdr *)(FsReadBuf + 40 * ShdrSym.sh_link);
        if (pShdr->sh_type == SHT_STRTAB)
        {                
            memcpy((char *)&ShdrStr, (char *)pShdr, sizeof(Elf_Shdr));
        }
        else
        {
            goto err;   /* 未找到字符串表 */
        }        
    }    
    else
    {
        goto err;   /* Sections个数过大 */;
    }    

    /* 字符串表 */
    if (ShdrStr.sh_size < sizeof(FsReadBuf))
    {
        bytes = ReadFileToMem(_path, ShdrStr.sh_offset, FsReadBuf, ShdrStr.sh_size);  
        if (bytes == ShdrStr.sh_size)
        {
            p = FsReadBuf;
            for (i = 0; i < bytes - 1; i++)
            {
                if (*p++ == 0)
                {
                    for (j = 0; j < FUNC_NUM; j++)
                    {
                        if (strcmp(p, strFuncName[j]) == 0)
                        {
                            StrFoud[j] = 1;
                            StrIdx[j] = i + 1;
                        }
                    }
                }
            }
        }
        else
        {
            goto err;   /* 读文件失败 */
        }
    }
    else
    {
        goto err;       /* 字符串表过大 */
    }
    
    /* 解析符号表 */
    if (ShdrSym.sh_size < sizeof(FsReadBuf)) 
    {
        bytes = ReadFileToMem(_path, ShdrSym.sh_offset, FsReadBuf, ShdrSym.sh_size);  
                      
        for (i = 0; i < bytes / sizeof(Elf_Sym); i++)
        {
            for (j = 0; j < FUNC_NUM; j++)
            {
                pSym = (Elf_Sym *)(FsReadBuf + 16 * i);
                if (pSym->st_name == StrIdx[j] && StrFoud[j] == 1)
                {
                    g_tFLM.Func[j].Valid = 1;
                    g_tFLM.Func[j].Offset = pSym->st_value;
                    g_tFLM.Func[j].Size = pSym->st_size;
                }
            }
        }
    }
    else
    {
        /* 符号表过大 */
    }
  
    /* 解析器件信息 */
    {
        
        #if 0   /* 2020-12-13，调试苏州国芯 CCM4208时修改 */
            bytes = ReadFileToMem(_path, g_tFLM.Func[IDX_FlashDevice].Offset + 52, FsReadBuf, sizeof(FlashDevice_T));
        #else
            bytes = ReadFileToMem(_path, g_tFLM.Load[1].Offset, FsReadBuf, sizeof(FlashDevice_T));
        #endif
        
        memcpy((char *)&g_tFLM.Device, FsReadBuf, sizeof(FlashDevice_T));     
    }
    
    if (ELF_FillToAlgo(_path, &flash_algo) == 0)
    {
        return 0;   /* 解析成功 */
    }
err:  
    return 1;   /* 解析失败 */   
}

        
/*
*********************************************************************************************************
*    函 数 名: ELF_FillToAlgo
*    功能说明: 解析elf文件.  使用了 FsReadBuf[4096]全部变量做缓冲区。解析结果放到全局变量g_tFLM
*    形    参: _path : 文件路径
*    返 回 值: 0 = ok， 其他值表示错误
*********************************************************************************************************
*/
const uint32_t BLOB_HEADER[] = {0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2};
static uint8_t ELF_FillToAlgo(char *_path, program_target_t *_algo)
{    
    //typedef struct {
    //    uint32_t breakpoint;
    //    uint32_t static_base;
    //    uint32_t stack_pointer;
    //} program_syscall_t;

    //typedef struct {
    //    const uint32_t  init;
    //    const uint32_t  uninit;
    //    const uint32_t  erase_chip;
    //    const uint32_t  erase_sector;
    //    const uint32_t  program_page;
    //    const program_syscall_t sys_call_s;
    //    const uint32_t  program_buffer;
    //    const uint32_t  algo_start;
    //    const uint32_t  algo_size;
    //    const uint32_t *algo_blob;
    //    const uint32_t  program_buffer_size;
    //} program_target_t;

    //typedef struct {
    //    const uint32_t start;
    //    const uint32_t size;
    //} sector_info_t;

    //    IDX_FlashDevice = 0,
    //    IDX_Init,
    //    IDX_UnInit,
    //    IDX_BlankCheck,
    //    IDX_EraseChip,
    //    IDX_EraseSector,
    //    IDX_ProgramPage,
    //    IDX_Verify,
    
    //static const uint32_t flash_code[] = {
    //    0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
    //    0x4603B510, 0x4C442000, 0x48446020, 0x48446060, 0x46206060, 0xF01069C0, 0xD1080F04, 0x5055F245,
    //    0x60204C40, 0x60602006, 0x70FFF640, 0x200060A0, 0x4601BD10, 0x69004838, 0x0080F040, 0x61104A36,
    //    0x47702000, 0x69004834, 0x0004F040, 0x61084932, 0x69004608, 0x0040F040, 0xE0036108, 0x20AAF64A,
    //    0x60084930, 0x68C0482C, 0x0F01F010, 0x482AD1F6, 0xF0206900, 0x49280004, 0x20006108, 0x46014770,
    //    0x69004825, 0x0002F040, 0x61104A23, 0x61414610, 0xF0406900, 0x61100040, 0xF64AE003, 0x4A2120AA,
    //    0x481D6010, 0xF01068C0, 0xD1F60F01, 0x6900481A, 0x0002F020, 0x61104A18, 0x47702000, 0x4603B510,
    //    0xF0201C48, 0xE0220101, 0x69004813, 0x0001F040, 0x61204C11, 0x80188810, 0x480FBF00, 0xF01068C0,
    //    0xD1FA0F01, 0x6900480C, 0x0001F020, 0x61204C0A, 0x68C04620, 0x0F14F010, 0x4620D006, 0xF04068C0,
    //    0x60E00014, 0xBD102001, 0x1C921C9B, 0x29001E89, 0x2000D1DA, 0x0000E7F7, 0x40022000, 0x45670123,
    //    0xCDEF89AB, 0x40003000, 0x00000000
    //};

    //const program_target_t flash_algo = {
    //    0x20000021,  // Init
    //    0x20000053,  // UnInit
    //    0x20000065,  // EraseChip
    //    0x2000009F,  // EraseSector
    //    0x200000DD,  // ProgramPage

    //    // BKPT : start of blob + 1
    //    // RSB  : address to access global/static data
    //    // RSP  : stack pointer
    //    {
    //        0x20000001,
    //        0x20000C00,
    //        0x20001000
    //    },

    //    0x20000400,  // mem buffer location
    //    0x20000000,  // location to write prog_blob in target RAM
    //    sizeof(flash_code),  // prog_blob size
    //    flash_code,  // address of prog_blob
    //    0x00000400,  // ram_to_flash_bytes_to_be_written
    //};
//    uint32_t bytes;
    
//    memcpy(FsReadBuf, (char *)BLOB_HEADER, 4 * 8);    
//    
//    if (g_tFLM.Load[0].Size + 32 > sizeof(FsReadBuf))
//    {
//        goto err;
//    }
//    bytes = ReadFileToMem(_path, g_tFLM.Load[0].Offset, FsReadBuf + 32, g_tFLM.Load[0].Size);          
//    if (bytes != g_tFLM.Load[0].Size)
//    {
//        goto err;
//    }
    
    _algo->init = g_tFLM.Func[IDX_Init].Offset + g_AlgoRam.Addr + 32;
    _algo->uninit = g_tFLM.Func[IDX_UnInit].Offset + g_AlgoRam.Addr + 32;
    
    _algo->erase_chip = 0;
    if (g_tFLM.Func[IDX_EraseChip].Offset > 0)
    {
        _algo->erase_chip = g_tFLM.Func[IDX_EraseChip].Offset + g_AlgoRam.Addr + 32;
    }
    
    _algo->erase_sector = 0; 
    if (g_tFLM.Func[IDX_EraseSector].Offset > 0)
    {
        _algo->erase_sector = g_tFLM.Func[IDX_EraseSector].Offset + g_AlgoRam.Addr + 32;
    }
    
    _algo->program_page = g_tFLM.Func[IDX_ProgramPage].Offset + g_AlgoRam.Addr + 32;
    
    _algo->verify = 0;
    if (g_tFLM.Func[IDX_Verify].Offset > 0)
    {
        _algo->verify = g_tFLM.Func[IDX_Verify].Offset + g_AlgoRam.Addr + 32;
    }
    
    _algo->check_blank = 0;
    if (g_tFLM.Func[IDX_BlankCheck].Offset > 0)
    {    
        _algo->check_blank = g_tFLM.Func[IDX_BlankCheck].Offset + g_AlgoRam.Addr + 32;    
    }
    
    _algo->cacul_crc32 = 0;
    if (g_tFLM.Func[IDX_CaculCRC32].Offset > 0)
    {    
        _algo->cacul_crc32 = g_tFLM.Func[IDX_CaculCRC32].Offset + g_AlgoRam.Addr + 32;    
    }  

    _algo->read_extid = 0;
    if (g_tFLM.Func[IDX_ReadExtID].Offset > 0)
    {    
        _algo->read_extid = g_tFLM.Func[IDX_ReadExtID].Offset + g_AlgoRam.Addr + 32;    
    } 
    
    _algo->algo_start = g_AlgoRam.Addr + g_tFLM.Load[0].Addr;
    _algo->algo_size = g_tFLM.Load[0].Size + 32;
    
    strncpy(_algo->algo_file_name, _path, sizeof(_algo->algo_file_name));
    
    _algo->program_buffer_size = g_tFLM.Device.szPage;
    _algo->program_buffer = g_AlgoRam.Addr + _algo->algo_size + 0;

    _algo->sys_call_s.breakpoint = g_AlgoRam.Addr  + 1;
    _algo->sys_call_s.static_base = _algo->program_buffer + _algo->program_buffer_size + 0;     /* 还有待研究，未明白用途 */
    
    {
        uint32_t RamSize;
        
        lua_getglobal(g_Lua, "AlgoRamSize");  
        if (lua_isinteger(g_Lua, -1)) 
        {
            RamSize = lua_tointeger(g_Lua, -1);
        }
        else
        {
            RamSize = 0x1000;
        }
        lua_pop(g_Lua, 1);        
        _algo->sys_call_s.stack_pointer = g_AlgoRam.Addr + RamSize;    /* 设置栈顶指针 */ 

        /* 2020-05-20 修正FLM中缺省的芯片容量数据 */
        {
            uint32_t FlashAddr;
            uint32_t FlashSize;
            
            lua_getglobal(g_Lua, "FLASH_ADDRESS");  
            if (lua_isinteger(g_Lua, -1)) 
            {
                FlashAddr = lua_tointeger(g_Lua, -1);
                
                if (g_tFLM.Device.DevAdr == FlashAddr)
                {
                    lua_getglobal(g_Lua, "FLASH_SIZE");
                    if (lua_isinteger(g_Lua, -1)) 
                    {
                        FlashSize = lua_tointeger(g_Lua, -1);
                        
                        g_tFLM.Device.szDev = FlashSize;
                    }
                    lua_pop(g_Lua, 1);
                }
            }
            lua_pop(g_Lua, 1);
        }
        
        /* 打印FLM算法文件期间信息和占用内存 */
        {
            printf("FLM memory Infomation :\r\n");
            printf("  algo file : %s\r\n", _algo->algo_file_name);
            
            /* Device信息 */
            printf("  ----Device Info------------------------\r\n");
            printf("  Version & Architecture : 0x%04X\r\n", g_tFLM.Device.Vers);
            printf("  Device Name : %s\r\n", g_tFLM.Device.DevName);
            printf("  Device Type : %d\r\n", g_tFLM.Device.DevType);
            printf("  Device Addr : 0x%08X\r\n", g_tFLM.Device.DevAdr);
            printf("  Device Size : %dKB (0x%X)\r\n", g_tFLM.Device.szDev / 1024, g_tFLM.Device.szDev);
            printf("  Page Size   : %dB\r\n", g_tFLM.Device.szPage);
            printf("  Reserved    : 0x%08X\r\n", g_tFLM.Device.Res);
            printf("  Erased Content : 0x%02X\r\n", g_tFLM.Device.valEmpty);
            printf("  Program Page TimeOut   : %d\r\n", g_tFLM.Device.toProg);
            printf("  Erase Sector TimeOut   : %d\r\n", g_tFLM.Device.toErase);            
            printf("  Erase Sector Size      : %08X, %dKB (0x%X)\r\n", 
                g_tFLM.Device.sectors[0].AddrSector, g_tFLM.Device.sectors[0].szSector / 1024, g_tFLM.Device.sectors[0].szSector);
            
            /* 算法函数偏移地址 */
            printf("  ----Algo function offset ---------------\r\n");            
            printf("  Load Offset : 0x%08X\r\n", g_tFLM.Load[0].Offset);
            printf("  Load Size   : 0x%08X\r\n", g_tFLM.Load[0].Size);
            printf("  FlashDevice : 0x%08X\r\n", g_tFLM.Func[IDX_FlashDevice].Offset + 52);
            printf("  Init        : 0x%08X\r\n", g_tFLM.Func[IDX_Init].Offset);
            printf("  UnInit      : 0x%08X\r\n", g_tFLM.Func[IDX_UnInit].Offset);
            printf("  BlankCheck  : 0x%08X\r\n", g_tFLM.Func[IDX_BlankCheck].Offset);
            printf("  EraseChip   : 0x%08X\r\n", g_tFLM.Func[IDX_EraseChip].Offset);
            printf("  EraseSector : 0x%08X\r\n", g_tFLM.Func[IDX_EraseSector].Offset);
            printf("  ProgramPage : 0x%08X\r\n", g_tFLM.Func[IDX_ProgramPage].Offset);
            printf("  Verify      : 0x%08X\r\n", g_tFLM.Func[IDX_Verify].Offset);
            printf("  CaculCRC32  : 0x%08X\r\n", g_tFLM.Func[IDX_CaculCRC32].Offset);
            printf("  ReadExtID   : 0x%08X\r\n", g_tFLM.Func[IDX_ReadExtID].Offset);
            
            /* 算法占用地址 */
            #if 0
                printf("  ----Algo RAM Info---------------------------\r\n");
                printf("  algo ram address   : 0x%08X\r\n", _algo->algo_start);
                printf("  algo size          : 0x%08X\r\n", _algo->algo_size);
                printf("  buffer address     : 0x%08X\r\n", _algo->program_buffer);
                printf("  buffer size        : 0x%08X\r\n", _algo->program_buffer_size);
                printf("  breakpoint addres  : 0x%08X\r\n", _algo->sys_call_s.breakpoint);
                printf("  static base adress : 0x%08X\r\n", _algo->sys_call_s.static_base);
                printf("  stack pointer      : 0x%08X\r\n", _algo->sys_call_s.stack_pointer); 
            #endif
        }
        
        /* 预留256字节全局变量空间和堆栈空间 */
        if (_algo->sys_call_s.static_base + 0x100 > g_AlgoRam.Addr + RamSize)
        {
            printf("AlgoRamSize too small, out of memory\r\n");
            return 1;   /* 出错 */
        }
    }
    
    /* 
        pg_load_algo_file("0:/H7-TOOL/Programmer/TestFLM/STM32_CRC_G4.FLM", 0, 2*1024, 1)
    */    
    if (g_tVar.SaveAlgoToCFile == 2)       /* 保存为c数组文件 (debug) */
    {
        SaveAlgoToCFile(_path);
    }
    else if (g_tVar.SaveAlgoToCFile == 1)
    {
        SaveAlgoToPrint(_path);
    }
    return 0;   /* 解析成功 */
}

/*
*********************************************************************************************************
*    函 数 名: LoadAlgoToTarget
*    功能说明: 将算法加载到目标CPU内存
*    形    参: 
*    返 回 值: 0 = ok， 其他值表示错误
*********************************************************************************************************
*/
uint8_t LoadAlgoToTarget(void)
{
    const uint32_t BLOB_HEADER[] = {0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2};    
    uint32_t bytes;
    
    if (g_tFLM.Load[0].Size + 32 < sizeof(FsReadBuf))   /* 小于16KB */
    {
        memcpy(FsReadBuf, (char *)BLOB_HEADER, 4 * 8);  
        bytes = ReadFileToMem(flash_algo.algo_file_name, g_tFLM.Load[0].Offset, FsReadBuf + 32, g_tFLM.Load[0].Size);          
        if (bytes != g_tFLM.Load[0].Size)
        {
            goto err;
        }
        
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            if (0 == MUL_swd_write_memory(flash_algo.algo_start, (uint8_t *)FsReadBuf, flash_algo.algo_size)) {
                return ERROR_ALGO_DL;
            }

            /* V1.11增加目标内存清零 */
//            bytes = flash_algo.sys_call_s.stack_pointer - flash_algo.sys_call_s.static_base;
//            if (bytes > sizeof(FsReadBuf))
//            {
//                bytes = sizeof(FsReadBuf);
//            }
//            else
//            {
//                memset(FsReadBuf, 0, bytes);
//            }
//            if (0 == MUL_swd_write_memory(flash_algo.sys_call_s.static_base, (uint8_t *)FsReadBuf, bytes)) {
//                return ERROR_ALGO_DL;
//            }            
        }
        else
        {
            if (0 == swd_write_memory(flash_algo.algo_start, (uint8_t *)FsReadBuf, flash_algo.algo_size)) {
                return ERROR_ALGO_DL;
            }             
        }
        return 0;        
    }
    else    /* 大于16KB */
    {
        uint32_t FileOffset;
        uint32_t MemAddr;  
        uint32_t size;
        
        size = flash_algo.algo_size;
        
        FileOffset = g_tFLM.Load[0].Offset;
        MemAddr = flash_algo.algo_start;
        
        /* 写第1个16KB */
        memcpy(FsReadBuf, (char *)BLOB_HEADER, 4 * 8);
        bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf + 32, sizeof(FsReadBuf) - 32);
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
            {
                return ERROR_ALGO_DL;
            }  
        }
        else
        {
            if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
            {
                return ERROR_ALGO_DL;
            }             
        }
    
        MemAddr += sizeof(FsReadBuf);
        FileOffset += sizeof(FsReadBuf) - 32;
        size -= sizeof(FsReadBuf);
        
        while (size)
        {
            if (size > sizeof(FsReadBuf))
            {
                bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf, sizeof(FsReadBuf));  
                if (bytes != sizeof(FsReadBuf))
                {
                    goto err;
                }                 
                
                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                {
                    if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
                    {
                        return ERROR_ALGO_DL;
                    } 
                }
                else    /* 单路模式 */
                {
                    if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
                    {
                        return ERROR_ALGO_DL;
                    }                     
                }
                MemAddr += sizeof(FsReadBuf);
                FileOffset += sizeof(FsReadBuf);
                size -= sizeof(FsReadBuf);                
            }
            else
            {
                bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf, size);  
                if (bytes != size)
                {
                    goto err;
                }                 
                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                {                
                    if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, size)) 
                    {
                        return ERROR_ALGO_DL;
                    } 
                }
                else
                {
                    if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, size)) 
                    {
                        return ERROR_ALGO_DL;
                    }                     
                }
                MemAddr += size;
                FileOffset += size;
                size -= size;                 
            }
        }
    }
    return 0;

err:
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: SaveAlgoToCFile
*    功能说明: 将算法文件中的算法函数提取到C数组文件
*    形    参: 
*    返 回 值: 0 = ok， 其他值表示错误
*********************************************************************************************************
*/
uint8_t SaveAlgoToCFile(char *_path)
{
    const uint32_t BLOB_HEADER[] = {0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2};    
    uint32_t bytes;
    uint32_t wr;
    
    if (g_tFLM.Load[0].Size + 32 < sizeof(FsReadBuf))   /* 小于16KB */
    {
        memcpy(FsReadBuf, (char *)BLOB_HEADER, 4 * 8);  
        bytes = ReadFileToMem(flash_algo.algo_file_name, g_tFLM.Load[0].Offset, FsReadBuf + 32, g_tFLM.Load[0].Size);          
        if (bytes != g_tFLM.Load[0].Size)
        {
            goto err;
        }
        
        /* 写文件 ，格式如下
            static const uint32_t flash_code[] = {
                0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
                0x4603B510, 0x4C442000, 0x48446020, 0x48446060, 0x46206060, 0xF01069C0, 0xD1080F04, 0x5055F245,
                0x60204C40, 0x60602006, 0x70FFF640, 0x200060A0, 0x4601BD10, 0x69004838, 0x0080F040, 0x61104A36,
                0x47702000, 0x69004834, 0x0004F040, 0x61084932, 0x69004608, 0x0040F040, 0xE0036108, 0x20AAF64A,
                0x60084930, 0x68C0482C, 0x0F01F010, 0x482AD1F6, 0xF0206900, 0x49280004, 0x20006108, 0x46014770,
                0x69004825, 0x0002F040, 0x61104A23, 0x61414610, 0xF0406900, 0x61100040, 0xF64AE003, 0x4A2120AA,
                0x481D6010, 0xF01068C0, 0xD1F60F01, 0x6900481A, 0x0002F020, 0x61104A18, 0x47702000, 0x4603B510,
                0xF0201C48, 0xE0220101, 0x69004813, 0x0001F040, 0x61204C11, 0x80188810, 0x480FBF00, 0xF01068C0,
                0xD1FA0F01, 0x6900480C, 0x0001F020, 0x61204C0A, 0x68C04620, 0x0F14F010, 0x4620D006, 0xF04068C0,
                0x60E00014, 0xBD102001, 0x1C921C9B, 0x29001E89, 0x2000D1DA, 0x0000E7F7, 0x40022000, 0x45670123,
                0xCDEF89AB, 0x40003000, 0x00000000
            };

            //const program_target_t flash_algo = {
            //    0x20000021,  // Init
            //    0x20000053,  // UnInit
            //    0x20000065,  // EraseChip
            //    0x2000009F,  // EraseSector
            //    0x200000DD,  // ProgramPage

            //    // BKPT : start of blob + 1
            //    // RSB  : address to access global/static data
            //    // RSP  : stack pointer
            //    {
            //        0x20000001,
            //        0x20000C00,
            //        0x20001000
            //    },

            //    0x20000400,  // mem buffer location
            //    0x20000000,  // location to write prog_blob in target RAM
            //    sizeof(flash_code),  // prog_blob size
            //    flash_code,  // address of prog_blob
            //    0x00000400,  // ram_to_flash_bytes_to_be_written
            //};        
        */
        {
            char buf[256];
            uint32_t i;
            
            strcpy(buf, _path);
            strcat(buf, ".c");
            f_open(&g_file, buf, FA_WRITE | FA_CREATE_ALWAYS);
            
            /* flash_code */
            strcpy(buf, "static const uint32_t flash_code[] = {\r\n");            
            f_write(&g_file, buf, strlen(buf),  &wr);            
            for (i = 0; i < (g_tFLM.Load[0].Size + 32) / 4; i++)
            {
                if (i == (g_tFLM.Load[0].Size + 32) / 4 - 1)
                {
                    sprintf(buf, "0x%08X", *(uint32_t *)&FsReadBuf[4 * i]);
                }
                else
                {
                    sprintf(buf, "0x%08X, ", *(uint32_t *)&FsReadBuf[4 * i]);
                }
                f_write(&g_file, buf, strlen(buf),  &wr);
                
                if ((i % 8) == 7)
                {
                    f_write(&g_file, "\r\n", 2,  &wr);
                }
            }
            
            f_write(&g_file, "};\r\n", 4,  &wr);
            
            /* flash_algo */
//typedef struct {   
//    uint32_t  init;
//    uint32_t  uninit;
//    uint32_t  erase_chip;
//    uint32_t  erase_sector;
//    uint32_t  program_page;
//    
//    uint32_t  verify;
//    uint32_t  check_blank;
//    uint32_t  cacul_crc32;
//    uint32_t  read_extid;
//    
//    program_syscall_t sys_call_s;
//    uint32_t  program_buffer;
//    uint32_t  algo_start;
//    uint32_t  algo_size;
//    uint32_t *algo_blob;
//    char algo_file_name[256];
//    uint32_t  program_buffer_size;
//} program_target_t;

    //typedef struct {
    //    uint32_t breakpoint;
    //    uint32_t static_base;
    //    uint32_t stack_pointer;
    //} program_syscall_t;
            
            strcpy(buf, "static const program_target_temp_t flash_algo = {\r\n");
            f_write(&g_file, buf, strlen(buf),  &wr);  

            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_Init].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr); 
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_UnInit].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr);  
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_EraseChip].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr);  
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_EraseSector].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr);  
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_ProgramPage].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr);  
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_Verify].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr);  
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_BlankCheck].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr);            
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_CaculCRC32].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr);              
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_ReadExtID].Offset);
            f_write(&g_file, buf, strlen(buf),  &wr);    

            sprintf(buf, "{0x%08X,0x%08X,0x%08X},\r\n", 
                flash_algo.sys_call_s.breakpoint,flash_algo.sys_call_s.static_base,flash_algo.sys_call_s.stack_pointer);
            f_write(&g_file, buf, strlen(buf),  &wr); 
            
            sprintf(buf, "0x%08X,\r\n", flash_algo.program_buffer);
            f_write(&g_file, buf, strlen(buf),  &wr); 
            sprintf(buf, "0x%08X,\r\n", flash_algo.program_buffer_size);
            f_write(&g_file, buf, strlen(buf),  &wr);             
            sprintf(buf, "0x%08X,\r\n", flash_algo.algo_start);
            f_write(&g_file, buf, strlen(buf),  &wr);
            sprintf(buf, "0x%08X,\r\n", flash_algo.algo_size);
            f_write(&g_file, buf, strlen(buf),  &wr);             

            f_write(&g_file, "};\r\n", 4,  &wr);
            
            
            /* 关闭文件 */
            f_close(&g_file);            
        }
        
        return 0;        
    }
    else    /* 大于16KB */
    {
        uint32_t FileOffset;
        uint32_t MemAddr;  
        uint32_t size;
        
        size = flash_algo.algo_size;
        
        FileOffset = g_tFLM.Load[0].Offset;
        MemAddr = flash_algo.algo_start;
        
        /* 写第1个16KB */
        memcpy(FsReadBuf, (char *)BLOB_HEADER, 4 * 8);
        bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf + 32, sizeof(FsReadBuf) - 32);
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
            {
                return ERROR_ALGO_DL;
            }  
        }
        else
        {
            if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
            {
                return ERROR_ALGO_DL;
            }             
        }
    
        MemAddr += sizeof(FsReadBuf);
        FileOffset += sizeof(FsReadBuf) - 32;
        size -= sizeof(FsReadBuf);
        
        while (size)
        {
            if (size > sizeof(FsReadBuf))
            {
                bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf, sizeof(FsReadBuf));  
                if (bytes != sizeof(FsReadBuf))
                {
                    goto err;
                }                 
                
                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                {
                    if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
                    {
                        return ERROR_ALGO_DL;
                    } 
                }
                else    /* 单路模式 */
                {
                    if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
                    {
                        return ERROR_ALGO_DL;
                    }                     
                }
                MemAddr += sizeof(FsReadBuf);
                FileOffset += sizeof(FsReadBuf);
                size -= sizeof(FsReadBuf);                
            }
            else
            {
                bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf, size);  
                if (bytes != size)
                {
                    goto err;
                }                 
                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                {                
                    if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, size)) 
                    {
                        return ERROR_ALGO_DL;
                    } 
                }
                else
                {
                    if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, size)) 
                    {
                        return ERROR_ALGO_DL;
                    }                     
                }
                MemAddr += size;
                FileOffset += size;
                size -= size;                 
            }
        }
    }
    return 0;

err:
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: SaveAlgoToPrint
*    功能说明: 将算法文件中的算法函数提取到C数组文件,仅打印出来
*    形    参: 
*    返 回 值: 0 = ok， 其他值表示错误
*********************************************************************************************************
*/
uint8_t SaveAlgoToPrint(char *_path)
{
    const uint32_t BLOB_HEADER[] = {0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2};    
    uint32_t bytes;
//    uint32_t wr;
    
    if (g_tFLM.Load[0].Size + 32 < sizeof(FsReadBuf))   /* 小于16KB */
    {
        memcpy(FsReadBuf, (char *)BLOB_HEADER, 4 * 8);  
        bytes = ReadFileToMem(flash_algo.algo_file_name, g_tFLM.Load[0].Offset, FsReadBuf + 32, g_tFLM.Load[0].Size);          
        if (bytes != g_tFLM.Load[0].Size)
        {
            goto err;
        }
        
        /* 写文件 ，格式如下
            static const uint32_t flash_code[] = {
                0xE00ABE00, 0x062D780D, 0x24084068, 0xD3000040, 0x1E644058, 0x1C49D1FA, 0x2A001E52, 0x4770D1F2,
                0x4603B510, 0x4C442000, 0x48446020, 0x48446060, 0x46206060, 0xF01069C0, 0xD1080F04, 0x5055F245,
                0x60204C40, 0x60602006, 0x70FFF640, 0x200060A0, 0x4601BD10, 0x69004838, 0x0080F040, 0x61104A36,
                0x47702000, 0x69004834, 0x0004F040, 0x61084932, 0x69004608, 0x0040F040, 0xE0036108, 0x20AAF64A,
                0x60084930, 0x68C0482C, 0x0F01F010, 0x482AD1F6, 0xF0206900, 0x49280004, 0x20006108, 0x46014770,
                0x69004825, 0x0002F040, 0x61104A23, 0x61414610, 0xF0406900, 0x61100040, 0xF64AE003, 0x4A2120AA,
                0x481D6010, 0xF01068C0, 0xD1F60F01, 0x6900481A, 0x0002F020, 0x61104A18, 0x47702000, 0x4603B510,
                0xF0201C48, 0xE0220101, 0x69004813, 0x0001F040, 0x61204C11, 0x80188810, 0x480FBF00, 0xF01068C0,
                0xD1FA0F01, 0x6900480C, 0x0001F020, 0x61204C0A, 0x68C04620, 0x0F14F010, 0x4620D006, 0xF04068C0,
                0x60E00014, 0xBD102001, 0x1C921C9B, 0x29001E89, 0x2000D1DA, 0x0000E7F7, 0x40022000, 0x45670123,
                0xCDEF89AB, 0x40003000, 0x00000000
            };

            //const program_target_t flash_algo = {
            //    0x20000021,  // Init
            //    0x20000053,  // UnInit
            //    0x20000065,  // EraseChip
            //    0x2000009F,  // EraseSector
            //    0x200000DD,  // ProgramPage

            //    // BKPT : start of blob + 1
            //    // RSB  : address to access global/static data
            //    // RSP  : stack pointer
            //    {
            //        0x20000001,
            //        0x20000C00,
            //        0x20001000
            //    },

            //    0x20000400,  // mem buffer location
            //    0x20000000,  // location to write prog_blob in target RAM
            //    sizeof(flash_code),  // prog_blob size
            //    flash_code,  // address of prog_blob
            //    0x00000400,  // ram_to_flash_bytes_to_be_written
            //};        
        */
        {
            char buf[256];
            uint32_t i;
            
            strcpy(buf, _path);
            strcat(buf, ".c");
            f_open(&g_file, buf, FA_WRITE | FA_CREATE_ALWAYS);
            
            /* flash_code */
            strcpy(buf, "static const uint32_t flash_code[] = {\r\n");            
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);            
            for (i = 0; i < (g_tFLM.Load[0].Size + 32) / 4; i++)
            {
                if (i == (g_tFLM.Load[0].Size + 32) / 4 - 1)
                {
                    sprintf(buf, "0x%08X", *(uint32_t *)&FsReadBuf[4 * i]);
                }
                else
                {
                    sprintf(buf, "0x%08X, ", *(uint32_t *)&FsReadBuf[4 * i]);
                }
                //f_write(&g_file, buf, strlen(buf),  &wr);
                printf(buf);  
                if ((i % 8) == 7)
                {
                    //f_write(&g_file, "\r\n", 2,  &wr);
                    printf("\r\n");
                }
            }
            
            //f_write(&g_file, "};\r\n", 4,  &wr);
            printf("};\r\n");
            
            /* flash_algo */
//typedef struct {   
//    uint32_t  init;
//    uint32_t  uninit;
//    uint32_t  erase_chip;
//    uint32_t  erase_sector;
//    uint32_t  program_page;
//    
//    uint32_t  verify;
//    uint32_t  check_blank;
//    uint32_t  cacul_crc32;
//    uint32_t  read_extid;
//    
//    program_syscall_t sys_call_s;
//    uint32_t  program_buffer;
//    uint32_t  algo_start;
//    uint32_t  algo_size;
//    uint32_t *algo_blob;
//    char algo_file_name[256];
//    uint32_t  program_buffer_size;
//} program_target_t;

    //typedef struct {
    //    uint32_t breakpoint;
    //    uint32_t static_base;
    //    uint32_t stack_pointer;
    //} program_syscall_t;
            
            strcpy(buf, "static const program_target_temp_t flash_algo = {\r\n");
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);

            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_Init].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_UnInit].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_EraseChip].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr); 
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_EraseSector].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);    
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_ProgramPage].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_Verify].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_BlankCheck].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_CaculCRC32].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", g_tFLM.Func[IDX_ReadExtID].Offset);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);            

            sprintf(buf, "{0x%08X,0x%08X,0x%08X},\r\n", 
                flash_algo.sys_call_s.breakpoint,flash_algo.sys_call_s.static_base,flash_algo.sys_call_s.stack_pointer);
            //f_write(&g_file, buf, strlen(buf),  &wr); 
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", flash_algo.program_buffer);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", flash_algo.program_buffer_size);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);    
            sprintf(buf, "0x%08X,\r\n", flash_algo.algo_start);
            //f_write(&g_file, buf, strlen(buf),  &wr);
            printf(buf);
            sprintf(buf, "0x%08X,\r\n", flash_algo.algo_size);
            //f_write(&g_file, buf, strlen(buf),  &wr);             
            printf(buf);
            //f_write(&g_file, "};\r\n", 4,  &wr);
            printf("};\r\n");
            
            /* 关闭文件 */
            //f_close(&g_file);            
        }
        
        return 0;        
    }
    else    /* 大于16KB */
    {
        uint32_t FileOffset;
        uint32_t MemAddr;  
        uint32_t size;
        
        size = flash_algo.algo_size;
        
        FileOffset = g_tFLM.Load[0].Offset;
        MemAddr = flash_algo.algo_start;
        
        /* 写第1个16KB */
        memcpy(FsReadBuf, (char *)BLOB_HEADER, 4 * 8);
        bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf + 32, sizeof(FsReadBuf) - 32);
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
            {
                return ERROR_ALGO_DL;
            }  
        }
        else
        {
            if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
            {
                return ERROR_ALGO_DL;
            }             
        }
    
        MemAddr += sizeof(FsReadBuf);
        FileOffset += sizeof(FsReadBuf) - 32;
        size -= sizeof(FsReadBuf);
        
        while (size)
        {
            if (size > sizeof(FsReadBuf))
            {
                bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf, sizeof(FsReadBuf));  
                if (bytes != sizeof(FsReadBuf))
                {
                    goto err;
                }                 
                
                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                {
                    if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
                    {
                        return ERROR_ALGO_DL;
                    } 
                }
                else    /* 单路模式 */
                {
                    if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, sizeof(FsReadBuf))) 
                    {
                        return ERROR_ALGO_DL;
                    }                     
                }
                MemAddr += sizeof(FsReadBuf);
                FileOffset += sizeof(FsReadBuf);
                size -= sizeof(FsReadBuf);                
            }
            else
            {
                bytes = ReadFileToMem(flash_algo.algo_file_name, FileOffset, FsReadBuf, size);  
                if (bytes != size)
                {
                    goto err;
                }                 
                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                {                
                    if (0 == MUL_swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, size)) 
                    {
                        return ERROR_ALGO_DL;
                    } 
                }
                else
                {
                    if (0 == swd_write_memory(MemAddr, (uint8_t *)FsReadBuf, size)) 
                    {
                        return ERROR_ALGO_DL;
                    }                     
                }
                MemAddr += size;
                FileOffset += size;
                size -= size;                 
            }
        }
    }
    return 0;

err:
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
