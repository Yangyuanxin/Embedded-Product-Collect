/*
*********************************************************************************************************
*
*    模块名称 : SD卡Fat文件系统演示模块。
*    文件名称 : demo_sd_fatfs.c
*    版    本 : V1.0
*    说    明 : 该例程移植FatFS文件系统（版本 R0.12c），演示如何创建文件、读取文件、创建目录和删除文件
*               并测试了文件读写速度.支持以下6个功能，用户通过电脑端串口软件发送数字给开发板即可:
*              1 - 显示根目录下的文件列表
*              2 - 创建一个新文件armfly.txt
*              3 - 读armfly.txt文件的内容
*              4 - 创建目录
*              5 - 删除文件和目录
*              6 - 读写文件速度测试
*
*    修改记录 :
*        版本号   日期         作者        说明
*        V1.0    2018-12-12   Eric2013    正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "ff.h" /* FatFS文件系统模块*/
#include "ff_gen_drv.h"
#include "sd_diskio_dma.h"

/* 用于测试读写速度 */
#define TEST_FILE_LEN (16 * 1024 * 1024) /* 用于测试的文件长度 */
#define BUF_SIZE (2 * 1024)                             /* 每次读写SD卡的最大数据长度 */

/* 仅允许本文件内调用的函数声明 */
static void DispMenu(void);
static void ViewRootDir(void);
static void CreateNewFile(void);
static void ReadFileData(void);
static void CreateDir(void);
static void DeleteDirFile(void);
static void WriteFileTest(void);

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
char FsReadBuf[1024];
#pragma location = 0x24003000
char FsWriteBuf[1024] = {"FatFS Write Demo \r\n www.armfly.com \r\n"};
#pragma location = 0x24004000
uint8_t g_TestBuf[BUF_SIZE];
#elif defined(__CC_ARM)
__attribute__((section(".RAM_D1"))) FATFS fs;
__attribute__((section(".RAM_D1"))) FIL file;
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) char FsReadBuf[1024]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) char FsWriteBuf[1024]) = {"FatFS Write Demo \r\n www.armfly.com \r\n"};
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) uint8_t g_TestBuf[BUF_SIZE]);
#endif

DIR DirInf;
FILINFO FileInf;
char DiskPath[4]; /* SD卡逻辑驱动路径，比盘符0，就是"0:/" */

/* FatFs API的返回值 */
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

/*
*********************************************************************************************************
*    函 数 名: DemoFatFS
*    功能说明: FatFS文件系统演示主程序
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DemoFatFS(void)
{
    uint8_t cmd;

    bsp_DelayMS(6000);

    /* 打印命令列表，用户可以通过串口操作指令 */
    DispMenu();

    /* 注册SD卡驱动 */
    FATFS_LinkDriver(&SD_Driver, DiskPath);

    bsp_StartAutoTimer(0, 500); /* 启动1个500ms的自动重装的定时器 */

    while (1)
    {

        /* 判断定时器超时时间 */
        if (bsp_CheckTimer(0))
        {
            /* 每隔500ms 进来一次 */
            bsp_LedToggle(2);
        }

        if (comGetChar(COM_USB1, &cmd)) /* 从串口读入一个字符(非阻塞方式) */
        {
            printf("\r\n");
            switch (cmd)
            {
            case '1':
                printf("【1 - ViewRootDir】\r\n");
                ViewRootDir(); /* 显示SD卡根目录下的文件名 */
                break;

            case '2':
                printf("【2 - CreateNewFile】\r\n");
                CreateNewFile(); /* 创建一个新文件,写入一个字符串 */
                break;

            case '3':
                printf("【3 - ReadFileData】\r\n");
                ReadFileData(); /* 读取根目录下armfly.txt的内容 */
                break;

            case '4':
                printf("【4 - CreateDir】\r\n");
                CreateDir(); /* 创建目录 */
                break;

            case '5':
                printf("【5 - DeleteDirFile】\r\n");
                DeleteDirFile(); /* 删除目录和文件 */
                break;

            case '6':
                printf("【6 - TestSpeed】\r\n");
                WriteFileTest(); /* 速度测试 */
                break;

            default:
                DispMenu();
                break;
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: DispMenu
*    功能说明: 显示操作提示菜单
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispMenu(void)
{
    printf("\r\n------------------------------------------------\r\n");
    printf("请选择操作命令:\r\n");
    printf("1 - 显示根目录下的文件列表\r\n");
    printf("2 - 创建一个新文件armfly.txt\r\n");
    printf("3 - 读armfly.txt文件的内容\r\n");
    printf("4 - 创建目录\r\n");
    printf("5 - 删除文件和目录\r\n");
    printf("6 - 读写文件速度测试\r\n");
}

/*
*********************************************************************************************************
*    函 数 名: ViewRootDir
*    功能说明: 显示SD卡根目录下的文件名
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/

extern SD_HandleTypeDef uSdHandle;
static void ViewRootDir(void)
{
    FRESULT result;
    uint32_t cnt = 0;
    FILINFO fno;

    /* 挂载文件系统 */
    result = f_mount(&fs, DiskPath, 0); /* Mount a logical drive */
    if (result != FR_OK)
    {
        printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
    }

    /* 打开根文件夹 */
    result = f_opendir(&DirInf, DiskPath); /* 如果不带参数，则从当前目录开始 */
    if (result != FR_OK)
    {
        printf("打开根目录失败  (%s)\r\n", FR_Table[result]);
        return;
    }

    printf("属性        |  文件大小 | 短文件名 | 长文件名\r\n");
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
            printf("(0x%02d)目录  ", FileInf.fattrib);
        }
        else
        {
            printf("(0x%02d)文件  ", FileInf.fattrib);
        }

        f_stat(FileInf.fname, &fno);

        /* 打印文件大小, 最大4G */
        printf(" %10d", (int)fno.fsize);

        printf("  %s\r\n", (char *)FileInf.fname); /* 长文件名 */
    }

    /* 打印卡速度信息 */
    if (uSdHandle.SdCard.CardSpeed == CARD_NORMAL_SPEED)
    {
        printf("Normal Speed Card <12.5MB/S, MAX Clock < 25MHz, Spec Version 1.01\r\n");
    }
    else if (uSdHandle.SdCard.CardSpeed == CARD_HIGH_SPEED)
    {
        printf("High Speed Card <25MB/s, MAX Clock < 50MHz, Spec Version 2.00\r\n");
    }
    else if (uSdHandle.SdCard.CardSpeed == CARD_ULTRA_HIGH_SPEED)
    {
        printf("UHS-I SD Card <50MB/S for SDR50, DDR50 Cards, MAX Clock < 50MHz OR 100MHz\r\n");
        printf("UHS-I SD Card <104MB/S for SDR104, MAX Clock < 108MHz, Spec version 3.01\r\n");
    }

    /* 卸载文件系统 */
    f_mount(NULL, DiskPath, 0);
}

/*
*********************************************************************************************************
*    函 数 名: CreateNewFile
*    功能说明: 在SD卡创建一个新文件，文件内容填写“www.armfly.com”
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
static void CreateNewFile(void)
{
    FRESULT result;
    uint32_t bw;
    char path[32];

    /* 挂载文件系统 */
    result = f_mount(&fs, DiskPath, 0); /* Mount a logical drive */
    if (result != FR_OK)
    {
        printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
    }

    /* 打开文件 */
    sprintf(path, "%sarmfly.txt", DiskPath);
    result = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE);
    if (result == FR_OK)
    {
        printf("armfly.txt 文件打开成功\r\n");
    }
    else
    {
        printf("armfly.txt 文件打开失败  (%s)\r\n", FR_Table[result]);
    }

    /* 写一串数据 */
    result = f_write(&file, FsWriteBuf, strlen(FsWriteBuf), &bw);
    if (result == FR_OK)
    {
        printf("armfly.txt 文件写入成功\r\n");
    }
    else
    {
        printf("armfly.txt 文件写入失败  (%s)\r\n", FR_Table[result]);
    }

    /* 关闭文件*/
    f_close(&file);

    /* 卸载文件系统 */
    f_mount(NULL, DiskPath, 0);
}

/*
*********************************************************************************************************
*    函 数 名: ReadFileData
*    功能说明: 读取文件armfly.txt前128个字符，并打印到串口
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
static void ReadFileData(void)
{
    FRESULT result;
    uint32_t bw;
    char path[64];

    /* 挂载文件系统 */
    result = f_mount(&fs, DiskPath, 0); /* Mount a logical drive */
    if (result != FR_OK)
    {
        printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
    }

    /* 打开文件 */
    sprintf(path, "%sarmfly.txt", DiskPath);
    result = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK)
    {
        printf("Don't Find File : armfly.txt\r\n");
        return;
    }

    /* 读取文件 */
    result = f_read(&file, FsReadBuf, sizeof(FsReadBuf), &bw);
    if (bw > 0)
    {
        FsReadBuf[bw] = 0;
        printf("\r\narmfly.txt 文件内容 : \r\n%s\r\n", FsReadBuf);
    }
    else
    {
        printf("\r\narmfly.txt 文件内容 : \r\n");
    }

    /* 关闭文件*/
    f_close(&file);

    /* 卸载文件系统 */
    f_mount(NULL, DiskPath, 0);
}

/*
*********************************************************************************************************
*    函 数 名: CreateDir
*    功能说明: 在SD卡根目录创建Dir1和Dir2目录，在Dir1目录下创建子目录Dir1_1
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
static void CreateDir(void)
{
    FRESULT result;
    char path[64];

    /* 挂载文件系统 */
    result = f_mount(&fs, DiskPath, 0); /* Mount a logical drive */
    if (result != FR_OK)
    {
        printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
    }

    /* 创建目录/Dir1 */
    sprintf(path, "%sDir1", DiskPath);
    result = f_mkdir(path);
    if (result == FR_OK)
    {
        printf("f_mkdir Dir1 Ok\r\n");
    }
    else if (result == FR_EXIST)
    {
        printf("Dir1 目录已经存在(%d)\r\n", result);
    }
    else
    {
        printf("f_mkdir Dir1 失败 (%s)\r\n", FR_Table[result]);
        return;
    }

    /* 创建目录/Dir2 */
    sprintf(path, "%sDir2", DiskPath);
    result = f_mkdir(path);
    if (result == FR_OK)
    {
        printf("f_mkdir Dir2 Ok\r\n");
    }
    else if (result == FR_EXIST)
    {
        printf("Dir2 目录已经存在(%d)\r\n", result);
    }
    else
    {
        printf("f_mkdir Dir2 失败 (%s)\r\n", FR_Table[result]);
        return;
    }

    /* 创建子目录 /Dir1/Dir1_1       注意：创建子目录Dir1_1时，必须先创建好Dir1 */
    sprintf(path, "%sDir1/Dir1_1", DiskPath);
    result = f_mkdir(path); /* */
    if (result == FR_OK)
    {
        printf("f_mkdir Dir1_1 成功\r\n");
    }
    else if (result == FR_EXIST)
    {
        printf("Dir1_1 目录已经存在 (%d)\r\n", result);
    }
    else
    {
        printf("f_mkdir Dir1_1 失败 (%s)\r\n", FR_Table[result]);
        return;
    }

    /* 卸载文件系统 */
    f_mount(NULL, DiskPath, 0);
}

/*
*********************************************************************************************************
*    函 数 名: DeleteDirFile
*    功能说明: 删除SD卡根目录下的 armfly.txt 文件和 Dir1，Dir2 目录
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DeleteDirFile(void)
{
    FRESULT result;
    uint8_t i;
    char path[64];

    /* 挂载文件系统 */
    result = f_mount(&fs, DiskPath, 0); /* Mount a logical drive */
    if (result != FR_OK)
    {
        printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
    }

    /* 删除目录/Dir1 【因为还存在目录非空（存在子目录)，所以这次删除会失败】*/
    sprintf(path, "%sDir1", DiskPath);
    result = f_unlink(path);
    if (result == FR_OK)
    {
        printf("删除目录Dir1成功\r\n");
    }
    else if (result == FR_NO_FILE)
    {
        printf("没有发现文件或目录 :%s\r\n", "/Dir1");
    }
    else
    {
        printf("删除Dir1失败(错误代码 = %d) 文件只读或目录非空\r\n", result);
    }

    /* 先删除目录/Dir1/Dir1_1 */
    sprintf(path, "%sDir1/Dir1_1", DiskPath);
    result = f_unlink(path);
    if (result == FR_OK)
    {
        printf("删除子目录/Dir1/Dir1_1成功\r\n");
    }
    else if ((result == FR_NO_FILE) || (result == FR_NO_PATH))
    {
        printf("没有发现文件或目录 :%s\r\n", "/Dir1/Dir1_1");
    }
    else
    {
        printf("删除子目录/Dir1/Dir1_1失败(错误代码 = %d) 文件只读或目录非空\r\n", result);
    }

    /* 先删除目录/Dir1 */
    sprintf(path, "%sDir1", DiskPath);
    result = f_unlink(path);
    if (result == FR_OK)
    {
        printf("删除目录Dir1成功\r\n");
    }
    else if (result == FR_NO_FILE)
    {
        printf("没有发现文件或目录 :%s\r\n", "/Dir1");
    }
    else
    {
        printf("删除Dir1失败(错误代码 = %d) 文件只读或目录非空\r\n", result);
    }

    /* 删除目录/Dir2 */
    sprintf(path, "%sDir2", DiskPath);
    result = f_unlink(path);
    if (result == FR_OK)
    {
        printf("删除目录 Dir2 成功\r\n");
    }
    else if (result == FR_NO_FILE)
    {
        printf("没有发现文件或目录 :%s\r\n", "/Dir2");
    }
    else
    {
        printf("删除Dir2 失败(错误代码 = %d) 文件只读或目录非空\r\n", result);
    }

    /* 删除文件 armfly.txt */
    sprintf(path, "%sarmfly.txt", DiskPath);
    result = f_unlink(path);
    if (result == FR_OK)
    {
        printf("删除文件 armfly.txt 成功\r\n");
    }
    else if (result == FR_NO_FILE)
    {
        printf("没有发现文件或目录 :%s\r\n", "armfly.txt");
    }
    else
    {
        printf("删除armfly.txt失败(错误代码 = %d) 文件只读或目录非空\r\n", result);
    }

    /* 删除文件 speed1.txt */
    for (i = 0; i < 20; i++)
    {
        sprintf(path, "%sSpeed%02d.txt", DiskPath, i); /* 每写1次，序号递增 */
        result = f_unlink(path);
        if (result == FR_OK)
        {
            printf("删除文件%s成功\r\n", path);
        }
        else if (result == FR_NO_FILE)
        {
            printf("没有发现文件:%s\r\n", path);
        }
        else
        {
            printf("删除%s文件失败(错误代码 = %d) 文件只读或目录非空\r\n", path, result);
        }
    }

    /* 卸载文件系统 */
    f_mount(NULL, DiskPath, 0);
}

/*
*********************************************************************************************************
*    函 数 名: WriteFileTest
*    功能说明: 测试文件读写速度
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
static void WriteFileTest(void)
{
    FRESULT result;
    char path[64];
    uint32_t bw;
    uint32_t i, k;
    uint32_t runtime1, runtime2, timelen;
    uint8_t err = 0;
    static uint8_t s_ucTestSn = 0;

    for (i = 0; i < sizeof(g_TestBuf); i++)
    {
        g_TestBuf[i] = (i / 512) + '0';
    }

    /* 挂载文件系统 */
    result = f_mount(&fs, DiskPath, 0); /* Mount a logical drive */
    if (result != FR_OK)
    {
        printf("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
    }

    /* 打开文件 */
    sprintf(path, "%sSpeed%02d.txt", DiskPath, s_ucTestSn++); /* 每写1次，序号递增 */
    result = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE);

    /* 写一串数据 */
    printf("开始写文件%s %dKB ...\r\n", path, TEST_FILE_LEN / 1024);

    runtime1 = bsp_GetRunTime(); /* 读取系统运行时间 */
    for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
    {
        result = f_write(&file, g_TestBuf, sizeof(g_TestBuf), &bw);
        if (result == FR_OK)
        {
            if (((i + 1) % 8) == 0)
            {
                printf(".");
            }
        }
        else
        {
            err = 1;
            printf("%s文件写失败\r\n", path);
            break;
        }
    }
    runtime2 = bsp_GetRunTime(); /* 读取系统运行时间 */

    if (err == 0)
    {
        timelen = (runtime2 - runtime1);
        printf("\r\n  写耗时 : %dms   平均写速度 :%dKB/S\r\n",
                     timelen,

                     ((TEST_FILE_LEN / 1024) * 1000) / timelen);
    }

    f_close(&file); /* 关闭文件*/

    /* 开始读文件测试 */
    result = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK)
    {
        printf("没有找到文件: %s\r\n", path);
        return;
    }

    printf("开始读文件 %dKB ...\r\n", TEST_FILE_LEN / 1024);

    runtime1 = bsp_GetRunTime(); /* 读取系统运行时间 */
    for (i = 0; i < TEST_FILE_LEN / BUF_SIZE; i++)
    {
        result = f_read(&file, g_TestBuf, sizeof(g_TestBuf), &bw);
        if (result == FR_OK)
        {
            if (((i + 1) % 8) == 0)
            {
                printf(".");
            }

            /* 比较写入的数据是否正确，此语句会导致读卡速度结果降低到 3.5MBytes/S */
            for (k = 0; k < sizeof(g_TestBuf); k++)
            {
                if (g_TestBuf[k] != (k / 512) + '0')
                {
                    err = 1;
                    printf("Speed1.txt 文件读成功，但是数据出错\r\n");
                    break;
                }
            }
            if (err == 1)
            {
                break;
            }
        }
        else
        {
            err = 1;
            printf("Speed1.txt 文件读失败\r\n");
            break;
        }
    }

    runtime2 = bsp_GetRunTime(); /* 读取系统运行时间 */

    if (err == 0)
    {
        timelen = (runtime2 - runtime1);
        printf("\r\n  读耗时 : %dms   平均读速度 : %dKB/S\r\n", timelen,
                     ((TEST_FILE_LEN / 1024) * 1000) / timelen);
    }

    /* 关闭文件*/
    f_close(&file);

    /* 卸载文件系统 */
    f_mount(NULL, DiskPath, 0);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
