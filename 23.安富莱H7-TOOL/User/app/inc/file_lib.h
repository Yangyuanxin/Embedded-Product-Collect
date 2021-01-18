/*
*********************************************************************************************************
*                                      
*    模块名称 : 文件操作接口函数
*    文件名称 : file_lib.h
*
*********************************************************************************************************
*/

#ifndef __FILE_LIB_H
#define __FILE_LIB_H

#include "ff.h" /* FatFS文件系统模块*/
#include "ff_gen_drv.h"
#include "sd_diskio_dma.h"

/* Lua程序根目录 */
#define LUA_ROOT_DIR    "0:/H7-TOOL/Lua"

/* 脱机编程器用户文件目录 */
#define PROG_USER_DIR    "0:/H7-TOOL/Programmer/User"

/* 脱机编程器算法文件目录 */
#define PROG_FLM_DIR    "0:/H7-TOOL/Programmer/FLM"

/* 脱机编程器启动配置文件 */
#define PROG_AUTORUN_FILE    "0:/H7-TOOL/Programmer/User/autorun.ini"

/* 静态分配，单个目录下最多100个文件 */
#define FILE_MAX_NUM        100
#define FILE_NAME_MAX_LEN   32      /* 32字符, 只显示前面16字符 */

typedef struct
{
    /* 文件类型（目录或文件）列表数组，静态分配 */
    uint8_t Type[FILE_MAX_NUM];
    char Name[FILE_MAX_NUM][FILE_NAME_MAX_LEN + 1];
    uint8_t Count;   
    char Path[200];     
}FILE_LIST_T;

/* prog ini文件结构 */
typedef struct
{
    uint32_t Locked;            /* 程序锁死标志，1表示锁死，需要人工解除 */

    uint32_t ProgramLimit;      /* 烧录次数限制 0表示不限制 */
    uint32_t ProgrammedCount;
    
    uint32_t ProductSN;         /* 产品序号（整数部分） */

}PROG_INI_T;

extern PROG_INI_T g_tProgIni;
extern FILE_LIST_T g_tFileList;

extern FATFS fs;
extern FIL g_file;
extern char FsReadBuf[16*1024];
extern char FsWriteBuf[1024];

extern DIR DirInf;
extern FILINFO FileInf;
extern char DiskPath[4];   /* SD卡逻辑驱动路径，比盘符0，就是"0:/" */


void FileSystemLoad(void);
void FileSystemUnLoad(void);
uint8_t DeleteFile(char *_Path);
void ListDir(char *_Path, char *_Filter);
void ListFileToMem(char *_Path, char *_OutBuf, uint32_t _BufSize);
uint8_t MakeDir(char *_Path);
uint32_t ReadFileToMem(char *_Path, uint32_t _offset, char *_Buff, uint32_t _MaxLen);
int32_t WriteFile(char *_Path, uint32_t _offset, char *_Buff, uint32_t _Len);

uint8_t SelectFile(char *_InitPath, uint16_t _MainStatus, uint16_t _RetStatus, char *_Filter);

uint32_t GetFileSize(char *_Path);
uint8_t CheckFileNamePostfix(char *_Path, char *_Filter);
void GetDirOfFileName(char *_Path, char *_Dir);
void FixFileName(char *_Path);


void ini_ReadString(const char *_IniBuf, const char *_ParamName, char *_OutBuff, int32_t _BuffSize);
int32_t ini_ReadInteger(const char *_IniBuf, const char *_ParamName);
void ini_WriteString(const char *_IniBuf, const char *_ParamName, const char *_NewStr, uint32_t _IniBufSize);
void ini_WriteInteger(const char *_IniBuf, const char *_ParamName, int _IntValue, uint32_t _IniBufSize);
int32_t ReadProgIniFile(char *_LuaPath, PROG_INI_T *pIni);
int32_t WriteProgIniFile(char *_LuaPath, PROG_INI_T *_pIni);
void LoadProgAutorunFile(char *_OutBuff, uint32_t _BuffSize);
void SaveProgAutorunFile(const char *_NewStr);
uint32_t GetFileMD5(char *_Path, char *_OutMD5);
uint32_t GetFileSum32(const char *_Path, uint32_t *_Out);
uint32_t GetFileCRC32(const char *_Path, uint32_t *_Out);

#endif
