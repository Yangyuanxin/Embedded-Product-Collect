/*
*********************************************************************************************************
*
*    模块名称 : 文件系统模块
*    文件名称 : file_lib.c
*    版    本 : V1.0
*    说    明 : 提供文件系统相关的API函数。文件系统采用FatFS。
*
*    修改记录 :
*        版本号   日期         作者        说明
*        V1.0    2019-12-12    armfly    正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"
#include "md5.h"
#include "crc32_stm32.h"

/*
    1、V7开发板的SD卡接口是用的SDMMC1，而这个接口仅支持AXI SRAM区访问，其它SRAM和TCP均不支持。
    2、AXI SRAM的主频是200MHz，需要Cache配合提速，所以此例子主RAM直接使用性能最高的DTCM。仅DMA
       操作SDIO的地方使用AXI SRAM，下面这几个变量是都需要使用AXI SRAM空间的。
    3、详情在此贴进行了说明：http://forum.armfly.com/forum.php?mod=viewthread&tid=91531
*/
#if defined(__ICCARM__)
#pragma location = 0x24000000
FATFS fs;
#pragma location = 0x24001000
FIL file;
#pragma location = 0x24002000
char FsReadBuf[16*1024];
#pragma location = 0x24003000
char FsWriteBuf[8*1024] = {"FatFS Write Demo \r\n www.armfly.com \r\n"};
#pragma location = 0x24004000
uint8_t g_TestBuf[BUF_SIZE];
#elif defined(__CC_ARM)
__attribute__((section(".RAM_D1"))) FATFS fs;
__attribute__((section(".RAM_D1"))) FIL g_file;
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) char FsReadBuf[16*1024]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) char FsWriteBuf[1024]) = {"FatFS Write Demo \r\n www.armfly.com \r\n"};
#endif

DIR DirInf;
FILINFO FileInf;
char DiskPath[4];   /* SD卡逻辑驱动路径，比盘符0，就是"0:/" */

/* FatFs API的返回值 */
#if 0
static const char *FR_Table[] =
{
    "FR_OK：成功",                                                /* (0) Succeeded */
    "FR_DISK_ERR：底层硬件错误",                                  /* (1) A hard error occurred in the low level disk I/O layer */
    "FR_INT_ERR：断言失败",                                       /* (2) Assertion failed */
    "FR_NOT_READY：物理驱动没有工作",                             /* (3) The physical drive cannot work */
    "FR_NO_FILE：文件不存在",                                     /* (4) Could not find the file */
    "FR_NO_PATH：路径不存在",                                     /* (5) Could not find the path */
    "FR_INVALID_NAME：无效文件名",                                /* (6) The path name format is invalid */
    "FR_DENIED：由于禁止访问或者目录已满访问被拒绝",              /* (7) Access denied due to prohibited access or directory full */
    "FR_EXIST：文件已经存在",                                     /* (8) Access denied due to prohibited access */
    "FR_INVALID_OBJECT：文件或者目录对象无效",                    /* (9) The file/directory object is invalid */
    "FR_WRITE_PROTECTED：物理驱动被写保护",                       /* (10) The physical drive is write protected */
    "FR_INVALID_DRIVE：逻辑驱动号无效",                           /* (11) The logical drive number is invalid */
    "FR_NOT_ENABLED：卷中无工作区",                               /* (12) The volume has no work area */
    "FR_NO_FILESYSTEM：没有有效的FAT卷",                          /* (13) There is no valid FAT volume */
    "FR_MKFS_ABORTED：由于参数错误f_mkfs()被终止",                /* (14) The f_mkfs() aborted due to any parameter error */
    "FR_TIMEOUT：在规定的时间内无法获得访问卷的许可",             /* (15) Could not get a grant to access the volume within defined period */
    "FR_LOCKED：由于文件共享策略操作被拒绝",                      /* (16) The operation is rejected according to the file sharing policy */
    "FR_NOT_ENOUGH_CORE：无法分配长文件名工作区",                 /* (17) LFN working buffer could not be allocated */
    "FR_TOO_MANY_OPEN_FILES：当前打开的文件数大于_FS_SHARE",      /* (18) Number of open files > _FS_SHARE */
    "FR_INVALID_PARAMETER：参数无效"                              /* (19) Given parameter is invalid */
};
#endif

FILE_LIST_T g_tFileList;

uint8_t *g_MenuLua_Text[FILE_MAX_NUM + 1] =
{
    /* 结束符号, 用于菜单函数自动识别菜单项个数 */
    "&"
};

MENU_T g_tMenuLua;

/*
*********************************************************************************************************
*    函 数 名: FileSystemLoad
*    功能说明: 文件系统加载
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void FileSystemLoad(void)
{
    FRESULT result;
    
    /* 注册SD卡驱动 */
    FATFS_LinkDriver(&SD_Driver, DiskPath);

    /* 挂载文件系统 */
    result = f_mount(&fs, DiskPath, 0); /* Mount a logical drive */
    if (result != FR_OK)
    {
        //printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
    }  
}

/*
*********************************************************************************************************
*    函 数 名: FileSystemUnLoad
*    功能说明: 文件系统卸载
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void FileSystemUnLoad(void)
{
    /* 卸载文件系统 */
    f_mount(NULL, DiskPath, 0);
}

/*
*********************************************************************************************************
*    函 数 名: CreateNewFile
*    功能说明: 创建一个新文件，并关闭.
*    形    参: 文件名 _FileName, 必须带全路径
*    返 回 值: 0成功
*********************************************************************************************************
*/
uint8_t CreateNewFile(char *_FileName)
{
    FRESULT result;

    /* 打开文件 */
    result = f_open(&g_file, _FileName, FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK)
    {
        return result;
    }

    /* 关闭文件*/
    f_close(&g_file);
    
    return result;
}

/*
*********************************************************************************************************
*    函 数 名: CloseFile
*    功能说明: 关闭文件
*    形    参: 文件名 _FileName, 必须带全路径
*    返 回 值: 0成功
*********************************************************************************************************
*/
uint8_t CloseFile(FIL *_file)
{
    /* 关闭文件*/
    return f_close(_file);
}

/*
*********************************************************************************************************
*    函 数 名: DeleteFile
*    功能说明: 删除目录或文件
*    形    参: 文件名 _FileName, 必须带全路径
*    返 回 值: 0成功
*********************************************************************************************************
*/
uint8_t DeleteFile(char *_Path)
{
    FRESULT result;
    
    result = f_unlink(_Path);
    if (result == FR_OK)
    {
        //printf("删除子目录/Dir1/Dir1_1成功\r\n");       
    }
    else if ((result == FR_NO_FILE) || (result == FR_NO_PATH))
    {
        //printf("没有发现文件或目录 :%s\r\n", "/Dir1/Dir1_1");
    }
    else
    {
        //printf("删除子目录/Dir1/Dir1_1失败(错误代码 = %d) 文件只读或目录非空\r\n", result);
    }
    
    return result;
}

/*
*********************************************************************************************************
*    函 数 名: MakeDir
*    功能说明: 创建目录. 自动创建每级目录
*    形    参: _Path, 路径全名
*    返 回 值: FR_EXIST  FR_OK
*********************************************************************************************************
*/
uint8_t MakeDir(char *_Path)
{   
    FRESULT re;
    char buf[256];
    uint16_t i;
    
    /* 0:/H7-TOOL/Programmer/Device */
    for (i = 0; i < 256; i++)
    {
        buf[i] = _Path[i];
        
        if (buf[i] == '/')
        {
            buf[i] = 0;
            
            if (i > 2)
            {
                re = f_mkdir(buf);
            }
            
             buf[i] = '/';
        }
        else if (buf[i] == 0)
        {
            if (i > 2)
            {
                re = f_mkdir(buf);
            }            
            break;
        }
    }
    
//    if (re == FR_EXIST || re == FR_OK)
//    {
//        return 0;
//    }
    
    return re;
}

/*
*********************************************************************************************************
*    函 数 名: ReadFileToMem
*    功能说明: 读取完整的文件到内存
*    形    参: _Path : 文件路径+文件名
*              _offset : 文件偏移地址
*              _Buff : 目标缓冲区
*              _MaxLen : 最大长度
*    返 回 值: 读到的数据长度。 0 表示失败。
*********************************************************************************************************
*/
uint32_t ReadFileToMem(char *_Path, uint32_t _offset, char *_Buff, uint32_t _MaxLen)
{
    FRESULT re;
    uint32_t br;
       
    re = f_open(&g_file, _Path, FA_OPEN_EXISTING | FA_READ);
    if (re != FR_OK)
    {
        return 0;
    }
    if (_offset > 0)
    {
        f_lseek(&g_file, _offset);
    }  
    
    re = f_read(&g_file, _Buff, _MaxLen,  &br);
    f_close(&g_file);
    
    SCB_InvalidateDCache_by_Addr((uint32_t *)_Buff,  _MaxLen);
    
    return br;
}

/*
*********************************************************************************************************
*    函 数 名: WriteFile
*    功能说明: 写文件
*    形    参: _Path : 文件路径+文件名 .lua文件，后缀改为.ini作为计数文件
*              _offset : 偏移
*              _Len : 长度
*    返 回 值: -1表示错误  0表示OK
*********************************************************************************************************
*/
int32_t WriteFile(char *_Path, uint32_t _offset, char *_Buff, uint32_t _Len)
{
    FRESULT re;
    uint32_t FileSize;
    
    if (_offset == 0)
    {
        re = f_open(&g_file, _Path, FA_WRITE | FA_CREATE_ALWAYS);
        if (re != FR_OK)
        {
            goto err_quit;
        }
    }
    else
    {
        re = f_open(&g_file, _Path, FA_WRITE | FA_READ);
        if (re != FR_OK)
        {
            goto err_quit;
        }
        f_lseek(&g_file, _offset);
    }    
    
    re = f_write(&g_file, _Buff, _Len,  &FileSize);
    if (re != FR_OK)
    {
        goto err_quit;
    }    
       
    f_close(&g_file);   
    return 0;

err_quit:
    f_close(&g_file);   
    return -1;
}

/*
*********************************************************************************************************
*    函 数 名: GetFileSize
*    功能说明: 得到文件长度
*    形    参: _Path : 文件路径+文件名*             
*    返 回 值: 文件长度。0表示失败。
*********************************************************************************************************
*/
uint32_t GetFileSize(char *_Path)
{
    FRESULT re;
    uint32_t size;
       
    re = f_open(&g_file, _Path, FA_OPEN_EXISTING | FA_READ);
    if (re != FR_OK)
    {
        return 0;
    }
 
    size =  f_size(&g_file);
    
    f_close(&g_file);
    
    return size;
}

/*
*********************************************************************************************************
*    函 数 名: ini_ReadString
*    功能说明: 读ini文件中的字符串变量。简化版，不支持[]分组，需要保证每个ini参数名唯一.  字符串内容不支持引号
*    形    参: _IniFileBuf : 文件内容缓冲区
*              _ParamName : 参数名(区分大小写）
*              _pOutString : 输出结果
*    返 回 值: 无
*********************************************************************************************************
*/
void ini_ReadString(const char *_IniBuf, const char *_ParamName, char *_OutBuff, int32_t _BuffSize)
{
    /*
    Count = -1
    ProductSN = 0
    Path = "1234"
    */    
    char *p;
    
    p = strstr(_IniBuf, _ParamName);
    if (p == 0)
    {
        _OutBuff[0] = 0;
        return;
    }
    
    p = strstr(p, "=");
    if (p == 0)
    {
        _OutBuff[0] = 0;
        return;
    }

    p = strstr( p++, "\"");
    if (p == 0)
    {
        _OutBuff[0] = 0;
        return;
    }
    p++;
    
    while (1)
    {
        if (*p == 0 || *p == '"')
        {
            *_OutBuff = 0;
            break;
        }
        else
        {
            *_OutBuff++ = *p++;
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: ini_WriteString
*    功能说明: 写ini文件中的字符串变量。简化版，不支持[]分组，需要保证每个ini参数名唯一.  字符串内容不支持引号
*    形    参: _IniFileBuf : 文件内容缓冲区
*              _ParamName : 参数名(区分大小写）
*              _OutBuff : 输出结果
*              _BuffSize : 输出缓冲区大小
*    返 回 值: 无
*********************************************************************************************************
*/
void ini_WriteString(const char *_IniBuf, const char *_ParamName, const char *_NewStr, uint32_t _IniBufSize)
{
    /*
    Count = -1
    ProductSN = 0
    Path = "1234"
    */    
    char *p;
    char *p1, *p2, *p3, *p4;
    int len, len2;
    int i;
    
    p = strstr(_IniBuf, _ParamName);
    if (p == 0)
    {
        return;
    }
    
    p = strstr(p, "=");
    if (p == 0)
    {
        return;
    }    
    p1 = p + 1;

    p2 = strstr(p, "\r");
    if (p2 == 0)
    {
        return;
    }
    
    len = strlen(_NewStr);    
    if (p2 - p1 > len + 2)  /* 将后面的字符串前移 */
    {
        memcpy(p1, " \"", 2); p1 += 2;
        memcpy(p1, _NewStr, len); p1 += len;
        memcpy(p1, "\"", 1); p1 += 1;
        
        while(1)
        {
            *p1++ = *p2++;
            
            if (*p2 == 0)
            {
                break;
            }
        }
    }
    else    /* 将后面的字符串后移，挪出空间存放新字符串  p1   p2   尾部 p3 --- p4 */
    {
        len2 = strlen(p2);
        p3 = len2 + p2;
        p4 = (p1 + len + 2) + len2;
        
        if (p4 - _IniBuf > _IniBufSize)
        {
            return;
        }
        
        for (i = 0; i < len2; i++)
        {
            *p4-- = *p3--; 
        }     

        memcpy(p1, " \"", 2); p1 += 2;
        memcpy(p1, _NewStr, len); p1 += len;
        memcpy(p1, "\"", 1); p1 += 1;        
    }
}

/*
*********************************************************************************************************
*    函 数 名: ini_ReadString
*    功能说明: 读ini文件中的整数变量。简化版，不支持[]分组，需要保证每个ini参数名唯一
*    形    参: _IniFileBuf : 文件内容缓冲区
*              _ParamName : 参数名(区分大小写）
*    返 回 值: 读到的整数值。如果没找到则返回0
*********************************************************************************************************
*/
int32_t ini_ReadInteger(const char *_IniBuf, const char *_ParamName)
{
    /*
    Count = -1
    ProductSN = 0
    Path = "1234"
    */    
    char *p;
    
    p = strstr(_IniBuf, _ParamName);
    if (p == 0)
    {
        return 0;
    }
    
    p = strstr(p, "=");
    if (p == 0)
    {
        return 0;
    }
    
    return str_to_int3(p + 1);
}

/*
*********************************************************************************************************
*    函 数 名: ini_WriteInteger
*    功能说明: 写ini文件中的整数变量。简化版，不支持[]分组，需要保证每个ini参数名唯一.  字符串内容不支持引号
*    形    参: _IniFileBuf : 文件内容缓冲区
*              _ParamName : 参数名(区分大小写）
*              _OutBuff : 输出结果
*              _BuffSize : 输出缓冲区大小
*    返 回 值: 无
*********************************************************************************************************
*/
void ini_WriteInteger(const char *_IniBuf, const char *_ParamName, int _IntValue, uint32_t _IniBufSize)
{
    /*
    Count = -1
    ProductSN = 0
    Path = "1234"
    */    
    char *p;
    char *p1, *p2, *p3, *p4;
    int len, len2;
    int i;
    char NewStr[32];
    
    p = strstr(_IniBuf, _ParamName);
    if (p == 0)
    {
        return;
    }
    
    p = strstr(p, "=");
    if (p == 0)
    {
        return;
    }    
    p1 = p + 1;

    p2 = strstr(p, "\r");
    if (p2 == 0)
    {
        return;
    }
    
    sprintf(NewStr, " %d", _IntValue);
    
    len = strlen(NewStr);    
    if (p2 - p1 > len)  /* 将后面的字符串前移 */
    {
        memcpy(p1, NewStr, len); p1 += len;
        
        while(1)
        {
            *p1++ = *p2++;
            
            if (*p2 == 0)
            {
                break;
            }
        }
    }
    else    /* 将后面的字符串后移，挪出空间存放新字符串  p1   p2   尾部 p3 --- p4 */
    {
        len2 = strlen(p2);
        p3 = len2 + p2;
        p4 = (p1 + len + 0) + len2;
        
        for (i = 0; i < len2; i++)
        {
            *p4-- = *p3--; 
        }     

        memcpy(p1, NewStr, len); p1 += len;       
    }
}

/*
*********************************************************************************************************
*    函 数 名: ReadProgIniFile
*    功能说明: 读ini文件.  使用了全局变量 FsReadBuf[]
*    形    参: _Path : 文件路径+文件名 .lua文件，后缀改为.ini作为计数文件            
*    返 回 值: -1表示错误
*********************************************************************************************************
*/
int32_t ReadProgIniFile(char *_LuaPath, PROG_INI_T *pIni)
{
    FRESULT re;
    uint32_t FileSize;

    char path[256];
    uint16_t len;            
                
    len = strlen(_LuaPath);
    
    if (len > sizeof(path))
    {
        /* 文件名过长 */
        goto err_quit;
    }
    
    /* 将 *.lua 文件修改为 *.txt */
    memcpy(path, _LuaPath, len);
    path[len - 3] = 'i';
    path[len - 2] = 'n';
    path[len - 1] = 'i';    
    path[len] = 0;
    
    /*　打开ini文件 */
    re = f_open(&g_file, path, FA_OPEN_EXISTING | FA_READ);
    if (re != FR_OK)
    {
        goto err_quit;
    }
 
    /* 将ini文件全部内容读到内存 16KB */
    re = f_read(&g_file, FsReadBuf, sizeof(FsReadBuf),  &FileSize);
    if (re != FR_OK || FileSize == 0)
    {
        f_close(&g_file);
        goto err_quit;
    }   
    FsReadBuf[FileSize] = 0;
    
    f_close(&g_file);
    
    /* 解析 Locked */
    pIni->Locked = ini_ReadInteger(FsReadBuf, "Locked");
    
    /* 解析 ProgramLimit */
    pIni->ProgramLimit = ini_ReadInteger(FsReadBuf, "ProgramLimit");

    /* 解析 ProgrammedCount */
    pIni->ProgrammedCount = ini_ReadInteger(FsReadBuf, "ProgrammedCount");    
    
    /* 解析 ProductSN */
    pIni->ProductSN = ini_ReadInteger(FsReadBuf, "ProductSN");  

    return 1;

err_quit:
    pIni->Locked = 0;
    pIni->ProductSN = 0;
    pIni->ProgramLimit = 0;
    pIni->ProgrammedCount = 0;
    return -1;
}

/*
*********************************************************************************************************
*    函 数 名: WriteProgIniFile
*    功能说明: 写ini文件.  使用了全局变量 FsReadBuf[]
*    形    参: _Path : 文件路径+文件名 .lua文件，后缀改为.ini作为计数文件
*              _pIni : ini文件变量
*    返 回 值: -1表示错误  1表示OK
*********************************************************************************************************
*/
int32_t WriteProgIniFile(char *_LuaPath, PROG_INI_T *_pIni)
{
    FRESULT re;
    uint32_t FileSize;

    char path[256];
    uint16_t len;            
                
    len = strlen(_LuaPath);
    
    if (len > sizeof(path))
    {
        /* 文件名过长 */
        goto err_quit;
    }
    
    /* 将 *.lua 文件修改为 *.txt */
    memcpy(path, _LuaPath, len);
    path[len - 3] = 'i';
    path[len - 2] = 'n';
    path[len - 1] = 'i';    
    path[len] = 0;
    
    sprintf(FsReadBuf, "Locked = %d\r\n", _pIni->Locked);
    sprintf(&FsReadBuf[strlen(FsReadBuf)], "ProgramLimit = %d\r\n", _pIni->ProgramLimit);
    sprintf(&FsReadBuf[strlen(FsReadBuf)], "ProgrammedCount = %d\r\n", _pIni->ProgrammedCount);
    sprintf(&FsReadBuf[strlen(FsReadBuf)], "ProductSN = %d\r\n", _pIni->ProductSN);    
    
    /*　打开ini文件准备写 */
    re = f_open(&g_file, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (re != FR_OK)
    {
        goto err_quit;
    }

    re = f_write(&g_file, FsReadBuf, strlen(FsReadBuf),  &FileSize);
    if (re != FR_OK)
    {
        goto err_quit;
    }    
       
    f_close(&g_file);   
    return 0;

err_quit:
    f_close(&g_file);   
    return -1;
}

/*
*********************************************************************************************************
*    函 数 名: LoadProgAutorunFile
*    功能说明: 读Autorun.ini文件.  使用了全局变量 FsReadBuf[]
*    形    参: _OutPath : 读到的结果保存在这个缓冲区         
*    返 回 值: -1表示错误
*********************************************************************************************************
*/
void LoadProgAutorunFile(char *_OutBuff, uint32_t _BuffSize)
{
    FRESULT re;
    uint32_t FileSize;

    /*　打开ini文件 */
    re = f_open(&g_file, PROG_AUTORUN_FILE, FA_OPEN_EXISTING | FA_READ);
    if (re != FR_OK)
    {
        goto err_quit;
    }
 
    /* 将ini文件全部内容读到内存 16KB */
    re = f_read(&g_file, FsReadBuf, sizeof(FsReadBuf),  &FileSize);
    if (re != FR_OK || FileSize == 0)
    {
        goto err_quit;
    }   
    FsReadBuf[FileSize] = 0;
    
    f_close(&g_file);

    ini_ReadString(FsReadBuf, "DefaultLuaFile", _OutBuff, _BuffSize);
    return;
 
err_quit:    
    f_close(&g_file); 
    _OutBuff[0] = 0;
    return;
}

/*
*********************************************************************************************************
*    函 数 名: SaveProgAutorunFile
*    功能说明: 更新修改Autorun.ini文件.  使用了全局变量 FsReadBuf[]
*    形    参: _OutPath : 读到的结果保存在这个缓冲区         
*    返 回 值: -1表示错误
*********************************************************************************************************
*/
void SaveProgAutorunFile(const char *_NewStr)
{
    FRESULT re;
    uint32_t FileSize;

//    /*　打开ini文件 */
//    re = f_open(&g_file, PROG_AUTORUN_FILE, FA_OPEN_EXISTING | FA_READ);
//    if (re != FR_OK)
//    {
//        goto err_quit;
//    }
// 
//    /* 将ini文件全部内容读到内存 16KB */
//    re = f_read(&g_file, FsReadBuf, sizeof(FsReadBuf),  &FileSize);
//    if (re != FR_OK || FileSize == 0)
//    {
//        f_close(&g_file);
//        goto err_quit;
//    }   
//    FsReadBuf[FileSize] = 0;
//    
//    f_close(&g_file);

//    ini_WriteString(FsReadBuf, "DefaultLuaFile", _NewStr, sizeof(FsReadBuf));


    sprintf(FsReadBuf, "DefaultLuaFile = \"%s\"", _NewStr);

    /*　打开ini文件准备写 */
    re = f_open(&g_file, PROG_AUTORUN_FILE, FA_WRITE | FA_CREATE_ALWAYS);
    if (re != FR_OK)
    {
        goto err_quit;
    }

    re = f_write(&g_file, FsReadBuf, strlen(FsReadBuf),  &FileSize);
    if (re != FR_OK)
    {

        goto err_quit;
    }     
        
err_quit:
    f_close(&g_file);
    return;
}

/*
*********************************************************************************************************
*    函 数 名: GetFileNamePostfix
*    功能说明: 获得文件名的后缀，不包括小数点。  1234.lua 则返回 "lua". 
*    形    参: _Path : 完整的路径带文件名
*              _Dir : 去掉文件名后的路径，不带斜杠
*    返 回 值: 无
*********************************************************************************************************
*/
void GetDirOfFileName(char *_Path, char *_Dir)
{
    uint16_t len;
    uint8_t i;
    
    len = strlen(_Path);
    
    for (i = len; i > 0; i--)
    {
        if (_Path[i] == '/')
        {
            len = i;
            break;
        }
    }
    
    for (i = 0; i < len; i++)
    {
        _Dir[i] = _Path[i];
    }
    _Dir[i] = 0;
}


/*
*********************************************************************************************************
*    函 数 名: GetFileNamePostfix
*    功能说明: 获得文件名的后缀，不包括小数点。  1234.lua 则返回 "lua". 
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
char *GetFileNamePostfix(char *_Path)
{
    uint16_t len;
    uint8_t i;
    
    len = strlen(_Path);
    
    for (i = len; i > 0; i--)
    {
        if (_Path[i] == '.')
        {
            break;
        }
    }
    if (i == 0)
    {
        return &_Path[len];
    }
    return &_Path[i];
}

/*
*********************************************************************************************************
*    函 数 名: CheckFileNamePostfix
*    功能说明: 判断文件名后缀是否在过滤器内
*    形    参: _Path 文件名
*              _Filter 过滤器 *.lua|*.txt  必须全部小写
*    返 回 值: 1表示在过滤器内， 0表示不在
*********************************************************************************************************
*/
uint8_t CheckFileNamePostfix(char *_Path, char *_Filter)
{
    char FixName[10];
    char *p;
    
    if (_Filter[0] == 0)
    {
        return 1;
    }
    
    p = GetFileNamePostfix(_Path);          /* 获得后缀名，不包括小数点 */
    strncpy(FixName, p, sizeof(FixName) - 1);   
    
    strlwr(FixName);    /* 全部转换为小写字母 */
    
    p = strstr(_Filter, FixName);
    if (p > 0)
    {
        return 1;
    }
    
    return 0;    
}

/*
*********************************************************************************************************
*    函 数 名: FixFileName
*    功能说明: 修正文件名中..上级目录. 0:H7-TOOL/User/../Dir1 转换为 0:H7-TOOL/Dir1
*    形    参: _Path 文件名
*              _Filter 过滤器 *.lua|*.txt  必须全部小写
*    返 回 值: 无
*********************************************************************************************************
*/
void FixFileName(char *_Path)
{
    int32_t i,j;
    int32_t len;
    
    len = strlen(_Path);
    
    for (i = 0; i < len - 3; i++)
    {
        if (_Path[i] == '/' &&_Path[i + 1] == '.' && _Path[i + 2] == '.')
        {
            for (j = i - 1; j > 0; j--)
            {
                if (_Path[j] == '/')
                {
                    strcpy(&_Path[j], &_Path[i + 3]);
                    
                    len = strlen(_Path);                    
                    i = 0;  /* 重新开始处理 */
                    break;
                }
            }
        }
    }
    
}

/*
*********************************************************************************************************
*    函 数 名: ListDir
*    功能说明: lua界面，列出文件. 保存到 g_FileNameList
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ListDir(char *_Path, char *_Filter)
{
    FRESULT result;
    uint32_t cnt = 0;
    uint32_t i;
    const char strBack[] = {0xA1,0xFB,0xB7,0xB5,0xBB,0xD8,0xC9,0xCF,0xBC,0xB6,0};  /* "←返回上级" */
    
    strcpy(g_tFileList.Name[0], strBack);     /* 此处中文用GBK编码 返回上级  */
    g_tFileList.Count = 1;
    
    /* 打开根文件夹 */
    result = f_opendir(&DirInf, _Path); /* 如果不带参数，则从当前目录开始 */
    if (result != FR_OK)
    {
        //printf("打开根目录失败  (%s)\r\n", FR_Table[result]);
        return;
    }

    for (cnt = 0;; cnt++)
    {
        result = f_readdir(&DirInf, &FileInf); /* 读取目录项，索引会自动下移 */
        if (result != FR_OK || FileInf.fname[0] == 0)
        {
            break;
        }

        if (FileInf.fname[0] == '.')
        {       
            continue;
        }

        /* 判断是文件还是子目录 */
        if (FileInf.fattrib & AM_DIR)
        {       
            g_tFileList.Type[g_tFileList.Count] = 0;    /* 目录 */
        }
        else
        {
            g_tFileList.Type[g_tFileList.Count] = 1;    /* 文件 */
            
            /* 过滤掉不符合后缀要求的文件名 */
            if (CheckFileNamePostfix(FileInf.fname, _Filter) == 0)                
            {
                continue;
            }
        }
        
        for (i = 0; i < FILE_NAME_MAX_LEN; i++)
        {
            if (FileInf.fname[i] == 0)
            {
                break;
            }
            g_tFileList.Name[g_tFileList.Count][i] = FileInf.fname[i];
        }
        g_tFileList.Name[g_tFileList.Count][i] = 0;
        g_tFileList.Count++;
    }
    
    f_closedir(&DirInf);
}

/*
*********************************************************************************************************
*    函 数 名: FileDataTimeToStr
*    功能说明: 文件的日期时间转换为HEX格式
*    形    参:  _data : FatFS格式的日期
*               _time : FatFS格式的时间
*    返 回 值: 日期ASCII码, 2020-12-38 01:01:59  (19字节)
*********************************************************************************************************
*/
void FileDataTimeToStr(uint16_t _date, uint16_t _time, char *_out)
{
    unsigned short year;
    unsigned short month;
    unsigned short day;
    unsigned short hour;
    unsigned short minute;
    unsigned short second;
    
    year = _date & 0xFE00;      // 0b1111 1110 0000 0000;
    year = year >> 9;
    year += 1980;
    
    month = _date & 0x01E0;     // 0b0000 0001 1110 0000;
    month = month >> 5;
    day = _date & 0x001F;       // 0b0000 0000 0001 1111;
    
    hour = _time & 0xF800;      // 0b1111 1000 0000 0000;
    hour = hour >> 11;

    minute = _time & 0x07E0;    // 0b0000 0111 1110 0000;
    minute = minute >> 5;
    
    second = _time & 0x001F;    // 0b0000 0000 0001 1111;
    second = (second * 2) % 60;
    
    printf("%02u-%02u", hour, minute);
    
    sprintf(_out, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
}

/*
*********************************************************************************************************
*    函 数 名: GetFileMD5
*    功能说明: 得到文件的MD5码
*    形    参: _Path : 文件名
*              _OutMD5 : 输出结果 16字节
*             _FileSize : 文件大小
*    返 回 值: 文件大小. 0表示打开文件失败
*********************************************************************************************************
*/
uint32_t GetFileMD5(char *_Path, char *_OutMD5)
{
    uint32_t bytes;  
    uint32_t offset = 0;
    MD5_CTX md5;    
    FRESULT re;
    uint32_t size;
       
    memset(_OutMD5, 0, 16);    
    
    re = f_open(&g_file, _Path, FA_OPEN_EXISTING | FA_READ);
    if (re != FR_OK)
    {
        return 0;
    }
    size = f_size(&g_file);
    
    f_close(&g_file);  

	MD5Init(&md5);

    while (1)
    {
        bytes = ReadFileToMem(_Path, offset, FsReadBuf, sizeof(FsReadBuf)); 
        offset += bytes;
        if (bytes > 0)
        {
            MD5Update(&md5, (uint8_t *)FsReadBuf, bytes);
        }
        else
        {
            break;
        }
    }
	MD5Final(&md5, (uint8_t *)_OutMD5);
    return size;
}

/*
*********************************************************************************************************
*    函 数 名: GetFileSum32
*    功能说明: 得到文件的累加校验和，32bit
*    形    参: _Path : 文件名
*              _Out : 输出结果 4字节
*    返 回 值: 文件大小. 0表示打开文件失败
*********************************************************************************************************
*/
uint32_t GetFileSum32(const char *_Path, uint32_t *_Out)
{
    uint32_t bytes;  
    uint32_t offset = 0;   
    FRESULT re;
    uint32_t size;
    uint32_t sum32;
    uint32_t i;
       
    *_Out = 0;  
        
    re = f_open(&g_file, _Path, FA_OPEN_EXISTING | FA_READ);
    if (re != FR_OK)
    {
        return 0;
    }
    size = f_size(&g_file);
    
    f_close(&g_file);  

    sum32 = 0;
    while (1)
    {
        bytes = ReadFileToMem((char *)_Path, offset, FsReadBuf, sizeof(FsReadBuf)); 
        offset += bytes;
        if (bytes > 0)
        {
            for (i = 0; i < bytes; i++)
            {
                sum32 += FsReadBuf[i];
            }
        }
        else
        {
            break;
        }
    }

    *_Out = sum32;    
    return size;
}

/*
*********************************************************************************************************
*    函 数 名: GetFileCRC32
*    功能说明: 得到文件的CRC32校验和，32bit
*    形    参: _Path : 文件名
*              _Out : 输出结果 4字节
*    返 回 值: 文件大小. 0表示打开文件失败
*********************************************************************************************************
*/
uint32_t GetFileCRC32(const char *_Path, uint32_t *_Out)
{
    uint32_t bytes;  
    uint32_t offset = 0;   
    FRESULT re;
    uint32_t size;
       
    *_Out = 0;  
        
    re = f_open(&g_file, _Path, FA_OPEN_EXISTING | FA_READ);
    if (re != FR_OK)
    {
        return 0;
    }
    size = f_size(&g_file);
    
    f_close(&g_file);  

    CRC32_Init();   /* 硬件CRC32计算 */
    
    while (1)
    {
        bytes = ReadFileToMem((char *)_Path, offset, FsReadBuf, sizeof(FsReadBuf)); 
        offset += bytes;
        if (bytes > 0)
        {
            CRC32_Update(FsReadBuf, bytes);
        }
        else
        {
            break;
        }
    }

    *_Out = CRC32_Final();    
    return size;
}

/*
*********************************************************************************************************
*    函 数 名: ListFileToMem
*    功能说明: 列出文件. 保存指定内存.  一条记录一样，tab间隔,回车换行。
*    形    参:  _Path : 目录
*               _OutBuf : 输出缓冲区
*               _BufSize : 缓冲区大小
*    返 回 值: 无
*********************************************************************************************************
*/
void ListFileToMem(char *_Path, char *_OutBuf, uint32_t _BufSize)
{
    FRESULT result;
    uint32_t cnt = 0;
    uint32_t i;
    
    _OutBuf[0] = 0;
    
    /* 打开根文件夹 */
    result = f_opendir(&DirInf, _Path); /* 如果不带参数，则从当前目录开始 */
    if (result != FR_OK)
    {
        //printf("打开根目录失败  (%s)\r\n", FR_Table[result]);
        return;
    }

    while (1)
    {
        result = f_readdir(&DirInf, &FileInf); /* 读取目录项，索引会自动下移 */
        if (result != FR_OK || FileInf.fname[0] == 0)
        {
            break;
        }

        if (FileInf.fname[0] == '.')
        {       
            continue;
        }

        /* 判断是文件还是子目录 */
        if (FileInf.fattrib & AM_DIR)
        {       
            _OutBuf[cnt++] = 'D';    /* 目录 */
        }
        else
        {
            _OutBuf[cnt++] = 'F';    /* 目录 */;    /* 文件 */            
        }        
        sprintf(&_OutBuf[cnt], "%08X",  FileInf.fsize);
        cnt += 8;        
        _OutBuf[cnt++] = '|';

        FileDataTimeToStr(FileInf.fdate, FileInf.ftime, &_OutBuf[cnt]);
        cnt += 19;
        _OutBuf[cnt++] = '|';     
        
        for (i = 0; i < 256; i++)
        {
            if (FileInf.fname[i] == 0)
            {
                break;
            }
            
            if (cnt >= _BufSize - 1)
            {
                break;
            }
            
            _OutBuf[cnt++] = FileInf.fname[i];
        }       
        
        _OutBuf[cnt++] = '\r';
        _OutBuf[cnt++] = '\n';
    }
    
    _OutBuf[cnt++] = 0;
    
    f_closedir(&DirInf);
}

/*
*********************************************************************************************************
*    函 数 名: SelectFile
*    功能说明: l文件浏览器。 用于Lua小程序和脱机编程器
*    形    参:  _InitPath :  初始路径
*               _MainStatus : 运行时的状态字
*               _RetStatus : 返回时的状态字
*               _Filter : 过滤器，*.lua
*    返 回 值: 0 表示返回， 1表示用户按了确认
*********************************************************************************************************
*/
uint8_t SelectFile(char *_InitPath, uint16_t _MainStatus, uint16_t _RetStatus, char *_Filter)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    uint8_t fListDir;
    uint32_t i;
    uint8_t RetValue = 0;
    
    DispHeader("文件浏览器");
    
    LCD_DispMenu(&g_tMenuLua);

    strcpy(g_tFileList.Path, _InitPath);
    
    fRefresh = 0;
    fListDir = 1;
    while (g_MainStatus == _MainStatus)
    {
        if (fListDir == 1)
        {
            fListDir = 0;
            
            ListDir(g_tFileList.Path, _Filter);
            
            g_tMenuLua.Left = MENU_LEFT;
            g_tMenuLua.Top = MENU_TOP;
            g_tMenuLua.Height = MENU_HEIGHT;
            g_tMenuLua.Width = MENU_WIDTH;
            if (g_tParam.FileListFont24 == 1)   /* 24点阵显示文件列表 */
            {
                g_tMenuLua.LineCap = MENU_CAP;            
                g_tMenuLua.ViewLine = 7;
                g_tMenuLua.Font.FontCode = FC_ST_24;
            }
            else    /* 16点阵显示文件列表 */
            {
                g_tMenuLua.LineCap = MENU_CAP;            
                g_tMenuLua.ViewLine = 10;
                g_tMenuLua.Font.FontCode = FC_ST_16;               
            }
            g_tMenuLua.Font.Space = 0;            
            g_tMenuLua.RollBackEn = 1;
            g_tMenuLua.GBK = 1;
            g_tMenuLua.ActiveBackColor = 0;   /* 选中行背景色ID */              
            for (i = 0; i < g_tFileList.Count; i++)
            {
                g_MenuLua_Text[i] = (uint8_t *)g_tFileList.Name[i];
            }
            g_MenuLua_Text[i] = "&";
                            
            LCD_InitMenu(&g_tMenuLua, (char **)g_MenuLua_Text); /* 初始化菜单结构 */
            
            if (g_tFileList.Count >= 2)
            {
                g_tMenuLua.Cursor = 1;
            }
            else
            {
                g_tMenuLua.Cursor  = 0;     /* 光标初始位置设置为返回上级 */
            }
            
            fRefresh = 1;
        }

        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;
            
            LCD_ClearMenu(&g_tMenuLua);     /* 清屏菜单区 */
            LCD_DispMenu(&g_tMenuLua);
        }        
        
        bsp_Idle();
        
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:          /* S键 上 */
                    LCD_MoveUpMenu(&g_tMenuLua);
                    break;

                case KEY_LONG_DOWN_S:   /* S键 长按 */      
                    if (g_tMenuLua.Cursor == 0)     /* 返回上级 */
                    {
                        uint8_t len;
                        
                        /* 0:/H7-TOOL/Lua */
                        len = strlen(g_tFileList.Path);
                        if (strcmp(g_tFileList.Path, _InitPath) == 0)   /* 已经在根目录了，退出 */
                        {
                            g_MainStatus = _RetStatus;
                        }
                        else    /* 还在子目录 */
                        {                        
                            for (i = len - 1; i > 2; i--)
                            {
                                if (g_tFileList.Path[i] == '/')
                                {
                                    g_tFileList.Path[i] = 0;
                                    fListDir = 1;
                                    break;
                                }
                            }
                        }
                    }
                    else 
                    {
                        if (g_tFileList.Type[g_tMenuLua.Cursor] == 0)   /* 目录 */
                        {
                            /* 路径后面追加目录 */
                            strcat(g_tFileList.Path, "/");
                            strcat(g_tFileList.Path, g_tFileList.Name[g_tMenuLua.Cursor]);
                            fListDir = 1;
                        }
                        else    /* 文件 */
                        {
                            uint16_t len;
                            char *p;
                            char ext_name[5];   /* 扩展名 */
                            
                            /* 支持 1.lua，  prog.lua */       
                            p = g_tFileList.Name[g_tMenuLua.Cursor];
                            len = strlen(p);
                            if (len >= 5)
                            {
                                memcpy(ext_name, &p[len - 4], 4);
                                ext_name[4] = 0;
                                strlwr(ext_name);   /* 转换为小写 */
                                if (strcmp(ext_name, ".lua") == 0)
                                {
                                    if (_RetStatus == MS_PROG_WORK)
                                    {                   
                                        g_MainStatus = MS_PROG_WORK;
                                        RetValue = 1;
                                    }
                                    else
                                    {
                                        g_MainStatus = MS_LUA_EXEC_FILE;
                                        RetValue = 1;
                                    }
                                    strcat(g_tFileList.Path, "/");
                                    strcat(g_tFileList.Path, g_tFileList.Name[g_tMenuLua.Cursor]);                                    
                                }
                            }
                        }
                    }
                    break;

                case KEY_UP_C:          /* C键 下 */
                    LCD_MoveDownMenu(&g_tMenuLua);
                    break;

                case KEY_LONG_DOWN_C:   /* C键长按 */
                    g_MainStatus = _RetStatus;
                    break;

                default:
                    break;
            }
        }
    } 
    return RetValue;    
}

extern _ARMABI int remove(const char * /*filename*/) __attribute__((__nonnull__(1)));
   /*
    * causes the file whose name is the string pointed to by filename to be
    * removed. Subsequent attempts to open the file will fail, unless it is
    * created anew. If the file is open, the behaviour of the remove function
    * is implementation-defined.
    * Returns: zero if the operation succeeds, nonzero if it fails.
    */
extern _ARMABI int rename(const char * /*old*/, const char * /*new*/) __attribute__((__nonnull__(1,2)));
   /*
    * causes the file whose name is the string pointed to by old to be
    * henceforth known by the name given by the string pointed to by new. The
    * file named old is effectively removed. If a file named by the string
    * pointed to by new exists prior to the call of the rename function, the
    * behaviour is implementation-defined.
    * Returns: zero if the operation succeeds, nonzero if it fails, in which
    *          case if the file existed previously it is still known by its
    *          original name.
    */
extern _ARMABI FILE *tmpfile(void);
   /*
    * creates a temporary binary file that will be automatically removed when
    * it is closed or at program termination. The file is opened for update.
    * Returns: a pointer to the stream of the file that it created. If the file
    *          cannot be created, a null pointer is returned.
    */
extern _ARMABI char *tmpnam(char * /*s*/);
   /*
    * generates a string that is not the same as the name of an existing file.
    * The tmpnam function generates a different string each time it is called,
    * up to TMP_MAX times. If it is called more than TMP_MAX times, the
    * behaviour is implementation-defined.
    * Returns: If the argument is a null pointer, the tmpnam function leaves
    *          its result in an internal static object and returns a pointer to
    *          that object. Subsequent calls to the tmpnam function may modify
    *          the same object. if the argument is not a null pointer, it is
    *          assumed to point to an array of at least L_tmpnam characters;
    *          the tmpnam function writes its result in that array and returns
    *          the argument as its value.
    */

//extern _ARMABI int fclose(FILE * /*stream*/) __attribute__((__nonnull__(1)));
_ARMABI int fclose(FILE * stream)
   /*
    * causes the stream pointed to by stream to be flushed and the associated
    * file to be closed. Any unwritten buffered data for the stream are
    * delivered to the host environment to be written to the file; any unread
    * buffered data are discarded. The stream is disassociated from the file.
    * If the associated buffer was automatically allocated, it is deallocated.
    * Returns: zero if the stream was succesfully closed, or nonzero if any
    *          errors were detected or if the stream was already closed.
    */
{
    FIL *fp;
    FRESULT result;
    
    fp = (FIL *)stream;
    result = f_close(fp);
    
    if (result != FR_OK)
    {
        return result;
    }
    return 0;
}

extern _ARMABI int fflush(FILE * /*stream*/);
   /*
    * If the stream points to an output or update stream in which the most
    * recent operation was output, the fflush function causes any unwritten
    * data for that stream to be delivered to the host environment to be
    * written to the file. If the stream points to an input or update stream,
    * the fflush function undoes the effect of any preceding ungetc operation
    * on the stream.
    * Returns: nonzero if a write error occurs.
    */

_ARMABI FILE *fopen(const char * filename /*filename*/,
                           const char * mode /*mode*/)
   /*
    * opens the file whose name is the string pointed to by filename, and
    * associates a stream with it.
    * The argument mode points to a string beginning with one of the following
    * sequences:
    * "r"         open text file for reading
    * "w"         create text file for writing, or truncate to zero length
    * "a"         append; open text file or create for writing at eof
    * "rb"        open binary file for reading
    * "wb"        create binary file for writing, or truncate to zero length
    * "ab"        append; open binary file or create for writing at eof
    * "r+"        open text file for update (reading and writing)
    * "w+"        create text file for update, or truncate to zero length
    * "a+"        append; open text file or create for update, writing at eof
    * "r+b"/"rb+" open binary file for update (reading and writing)
    * "w+b"/"wb+" create binary file for update, or truncate to zero length
    * "a+b"/"ab+" append; open binary file or create for update, writing at eof
    *
    * Opening a file with read mode ('r' as the first character in the mode
    * argument) fails if the file does not exist or cannot be read.
    * Opening a file with append mode ('a' as the first character in the mode
    * argument) causes all subsequent writes to be forced to the current end of
    * file, regardless of intervening calls to the fseek function. In some
    * implementations, opening a binary file with append mode ('b' as the
    * second or third character in the mode argument) may initially position
    * the file position indicator beyond the last data written, because of the
    * NUL padding.
    * When a file is opened with update mode ('+' as the second or third
    * character in the mode argument), both input and output may be performed
    * on the associated stream. However, output may not be directly followed
    * by input without an intervening call to the fflush fuction or to a file
    * positioning function (fseek, fsetpos, or rewind), and input be not be
    * directly followed by output without an intervening call to the fflush
    * fuction or to a file positioning function, unless the input operation
    * encounters end-of-file. Opening a file with update mode may open or
    * create a binary stream in some implementations. When opened, a stream
    * is fully buffered if and only if it does not refer to an interactive
    * device. The error and end-of-file indicators for the stream are
    * cleared.
    * Returns: a pointer to the object controlling the stream. If the open
    *          operation fails, fopen returns a null pointer.
    */
{
    /*
        fatfs官网有兼容方式定义格式
        http://elm-chan.org/fsw/ff/doc/open.html
    
        POSIX	FatFs
        "r"	    FA_READ
        "r+"	FA_READ | FA_WRITE
        "w"	    FA_CREATE_ALWAYS | FA_WRITE
        "w+"	FA_CREATE_ALWAYS | FA_WRITE | FA_READ
        "a"     FA_OPEN_APPEND | FA_WRITE
        "a+"    FA_OPEN_APPEND | FA_WRITE | FA_READ
        "wx"    FA_CREATE_NEW | FA_WRITE
        "w+x"   FA_CREATE_NEW | FA_WRITE | FA_READ
    */
    FRESULT result;
    BYTE ff_mode;

    if (strcmp(mode, "r") == 0)         ff_mode = FA_READ;
    else if (strcmp(mode, "r+") == 0)   ff_mode = FA_READ | FA_WRITE;
    else if (strcmp(mode, "w") == 0)    ff_mode = FA_CREATE_ALWAYS | FA_WRITE;
    else if (strcmp(mode, "w+") == 0)   ff_mode = FA_CREATE_ALWAYS | FA_WRITE | FA_READ;
    else if (strcmp(mode, "a") == 0)    ff_mode = FA_OPEN_APPEND | FA_WRITE;
    else if (strcmp(mode, "a+") == 0)   ff_mode = FA_OPEN_APPEND | FA_WRITE | FA_READ;
    else if (strcmp(mode, "wx") == 0)   ff_mode = FA_CREATE_NEW | FA_WRITE;
    else if (strcmp(mode, "w+x") == 0)  ff_mode = FA_CREATE_NEW | FA_WRITE | FA_READ;
    
    /* 打开文件 */
    f_close(&g_file);
    result = f_open(&g_file, filename, ff_mode);
    if (result == FR_OK)
    {
        return (FILE *)&g_file;
    }
    
    return 0;
}

_ARMABI FILE *freopen(const char * filename /*filename*/,
                    const char * mode /*mode*/,
                    FILE * stream /*stream*/)
   /*
    * opens the file whose name is the string pointed to by filename and
    * associates the stream pointed to by stream with it. The mode argument is
    * used just as in the fopen function.
    * The freopen function first attempts to close any file that is associated
    * with the specified stream. Failure to close the file successfully is
    * ignored. The error and end-of-file indicators for the stream are cleared.
    * Returns: a null pointer if the operation fails. Otherwise, freopen
    *          returns the value of the stream.
    */  
{  
    return 0;
}
                    
extern _ARMABI void setbuf(FILE * __restrict /*stream*/,
                    char * __restrict /*buf*/) __attribute__((__nonnull__(1)));
   /*
    * Except that it returns no value, the setbuf function is equivalent to the
    * setvbuf function invoked with the values _IOFBF for mode and BUFSIZ for
    * size, or (if buf is a null pointer), with the value _IONBF for mode.
    * Returns: no value.
    */
extern _ARMABI int setvbuf(FILE * __restrict /*stream*/,
                   char * __restrict /*buf*/,
                   int /*mode*/, size_t /*size*/) __attribute__((__nonnull__(1)));
   /*
    * may be used after the stream pointed to by stream has been associated
    * with an open file but before it is read or written. The argument mode
    * determines how stream will be buffered, as follows: _IOFBF causes
    * input/output to be fully buffered; _IOLBF causes output to be line
    * buffered (the buffer will be flushed when a new-line character is
    * written, when the buffer is full, or when input is requested); _IONBF
    * causes input/output to be completely unbuffered. If buf is not the null
    * pointer, the array it points to may be used instead of an automatically
    * allocated buffer (the buffer must have a lifetime at least as great as
    * the open stream, so the stream should be closed before a buffer that has
    * automatic storage duration is deallocated upon block exit). The argument
    * size specifies the size of the array. The contents of the array at any
    * time are indeterminate.
    * Returns: zero on success, or nonzero if an invalid value is given for
    *          mode or size, or if the request cannot be honoured.
    */
                   
extern _ARMABI size_t fread(void * ptr /*ptr*/,
                    size_t size/*size*/, size_t nmemb/*nmemb*/, FILE * stream /*stream*/)
   /*
    * reads into the array pointed to by ptr, up to nmemb members whose size is
    * specified by size, from the stream pointed to by stream. The file
    * position indicator (if defined) is advanced by the number of characters
    * successfully read. If an error occurs, the resulting value of the file
    * position indicator is indeterminate. If a partial member is read, its
    * value is indeterminate. The ferror or feof function shall be used to
    * distinguish between a read error and end-of-file.
    * Returns: the number of members successfully read, which may be less than
    *          nmemb if a read error or end-of-file is encountered. If size or
    *          nmemb is zero, fread returns zero and the contents of the array
    *          and the state of the stream remain unchanged.
    */
{
    FRESULT result;
    uint32_t br = 0;
    FIL *fp;
    
    fp = (FIL *)stream;    
    
    result = f_read(fp, ptr, nmemb,  &br);
    if (result == FR_OK)
    {
        return br;
    }
    
    return 0;
}

extern _ARMABI int setvbuf(FILE * __restrict /*stream*/,
                   char * __restrict /*buf*/,
                   int /*mode*/, size_t /*size*/) __attribute__((__nonnull__(1)));
   /*
    * may be used after the stream pointed to by stream has been associated
    * with an open file but before it is read or written. The argument mode
    * determines how stream will be buffered, as follows: _IOFBF causes
    * input/output to be fully buffered; _IOLBF causes output to be line
    * buffered (the buffer will be flushed when a new-line character is
    * written, when the buffer is full, or when input is requested); _IONBF
    * causes input/output to be completely unbuffered. If buf is not the null
    * pointer, the array it points to may be used instead of an automatically
    * allocated buffer (the buffer must have a lifetime at least as great as
    * the open stream, so the stream should be closed before a buffer that has
    * automatic storage duration is deallocated upon block exit). The argument
    * size specifies the size of the array. The contents of the array at any
    * time are indeterminate.
    * Returns: zero on success, or nonzero if an invalid value is given for
    *          mode or size, or if the request cannot be honoured.
    */

extern _ARMABI size_t fwrite(const void * __restrict /*ptr*/,
                    size_t /*size*/, size_t /*nmemb*/, FILE * __restrict /*stream*/) __attribute__((__nonnull__(1,4)));
   /*
    * writes, from the array pointed to by ptr up to nmemb members whose size
    * is specified by size, to the stream pointed to by stream. The file
    * position indicator (if defined) is advanced by the number of characters
    * successfully written. If an error occurs, the resulting value of the file
    * position indicator is indeterminate.
    * Returns: the number of members successfully written, which will be less
    *          than nmemb only if a write error is encountered.
    */

extern _ARMABI int fseek(FILE * /*stream*/, long int /*offset*/, int /*whence*/) __attribute__((__nonnull__(1)));
   /*
    * sets the file position indicator for the stream pointed to by stream.
    * For a binary stream, the new position is at the signed number of
    * characters specified by offset away from the point specified by whence.
    * The specified point is the beginning of the file for SEEK_SET, the
    * current position in the file for SEEK_CUR, or end-of-file for SEEK_END.
    * A binary stream need not meaningfully support fseek calls with a whence
    * value of SEEK_END.
    * For a text stream, either offset shall be zero, or offset shall be a
    * value returned by an earlier call to the ftell function on the same
    * stream and whence shall be SEEK_SET.
    * The fseek function clears the end-of-file indicator and undoes any
    * effects of the ungetc function on the same stream. After an fseek call,
    * the next operation on an update stream may be either input or output.
    * Returns: nonzero only for a request that cannot be satisfied.
    */ 
_ARMABI int fseek(FILE *stream, long int offset, int whence)
{
    return 0;
}
 
extern _ARMABI long int ftell(FILE * /*stream*/) __attribute__((__nonnull__(1)));
   /*
    * obtains the current value of the file position indicator for the stream
    * pointed to by stream. For a binary stream, the value is the number of
    * characters from the beginning of the file. For a text stream, the file
    * position indicator contains unspecified information, usable by the fseek
    * function for returning the file position indicator to its position at the
    * time of the ftell call; the difference between two such return values is
    * not necessarily a meaningful measure of the number of characters written
    * or read.
    * Returns: if successful, the current value of the file position indicator.
    *          On failure, the ftell function returns -1L and sets the integer
    *          expression errno to an implementation-defined nonzero value.
    */
_ARMABI long int ftell(FILE *stream)  
{
    FIL *fp;
    
    fp = (FIL *)stream;    
   
    return fp->fptr;
}

extern _ARMABI void clearerr(FILE * /*stream*/) __attribute__((__nonnull__(1)));
   /*
    * clears the end-of-file and error indicators for the stream pointed to by
    * stream. These indicators are cleared only when the file is opened or by
    * an explicit call to the clearerr function or to the rewind function.
    * Returns: no value.
    */
void clearerr(FILE *stream)
{
    FIL *fp;
    
    fp = (FIL *)stream;
    
    fp->err = 0;
}


extern _ARMABI int feof(FILE * /*stream*/) __attribute__((__nonnull__(1)));
   /*
    * tests the end-of-file indicator for the stream pointed to by stream.
    * Returns: nonzero iff the end-of-file indicator is set for stream.
    */
_ARMABI int feof(FILE *stream)
{
    FIL *fp;
    
    fp = (FIL *)stream;
    if (fp->fptr >= fp->obj.objsize)
    {
        return 1;
    }
    return 0;   
}

extern _ARMABI int ferror(FILE * /*stream*/) __attribute__((__nonnull__(1)));
   /*
    * tests the error indicator for the stream pointed to by stream.
    * Returns: nonzero iff the error indicator is set for stream.
    */
_ARMABI int ferror(FILE *stream)
{
    FIL *fp;
    
    fp = (FIL *)stream;
    
    return fp->err;
}


/*
*********************************************************************************************************
*    函 数 名: fgetc
*    功能说明: 重定义getc函数，这样可以使用getchar函数从串口1输入数据
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
int fgetc(FILE *f)
{
    if (f == &__stdin)
    {     
        return 0;   /* sacan 标准输入，还未做 */
    }
    else    /* 文件操作， lua dofile()会执行这个分支 */
    {
        FRESULT result;
        uint32_t br = 0;
        FIL *fp;
        char buf[2];
        
        fp = (FIL *)f;    
        
        result = f_read(fp, buf, 1,  &br);
        if (result == FR_OK)
        {
            return buf[0];
        } 
        
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: fputc
*    功能说明: 重定义putc函数，这样可以使用printf函数从串口1打印输出
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern MEMO_T g_LuaMemo;
extern uint16_t g_MainStatus;
extern void MODH_PrintByte(char _ch);
int fputc(int ch, FILE *f)
{
    if (f == &__stdout)
    {
        #if 0
            {
                uint8_t buf[1];
                
                buf[0] = ch;
                comSendBuf(COM_RS485,buf, 1);
            }
        #else
            if (g_MainStatus == MS_LUA_EXEC_FILE)
            {
                LCD_MemoAddChar(&g_LuaMemo, ch);
            }
            
            MODH_PrintByte(ch);
                   
        #endif        
            return ch; 
    }
    else
    {
        /* FatFS文件操作，还未做 */
        return 0;
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
