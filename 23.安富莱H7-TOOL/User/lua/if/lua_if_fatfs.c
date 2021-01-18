#include "lauxlib.h"
#include "lualib.h"
#include "time.h"
#include "lua_if.h"
#include "bsp.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio_dma.h"


FATFS   g_lua_fs;
FIL     g_lua_file1;

char luaDiskPath[4] = "0:/"; /* 保存FatFS 磁盘路径 */

static int lua_f_mount(lua_State* L);
static int lua_f_dir(lua_State* L);
void ViewDir(char *_path);

void lua_fatfs_RegisterFun(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "f_init", lua_f_mount);    
    lua_register(g_Lua, "f_dir", lua_f_dir);
}

/*
*********************************************************************************************************
*    函 数 名: lua_udp_SendBytes
*    功能说明: 向UDP发送一包数据。UDP目标IP由最后一次接收到的UDP包中提取，也就是只发给最后一次通信的主机
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_f_mount(lua_State* L)
{
//    FATFS_LinkDriver(&SD_Driver, luaDiskPath);
    
    /* 挂载文件系统 */
    if (f_mount(&g_lua_fs, luaDiskPath, 0) != FR_OK)
    {
        printf("f_mount文件系统失败");
    }
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_f_dir
*    功能说明: 
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_f_dir(lua_State* L)
{
    const char *data;
    uint32_t len;
    
    if (lua_type(L, 1) == LUA_TSTRING)     /* 判断第1个参数 */
    {        
        data = luaL_checklstring(L, 1, &len); /* 1是参数的位置， len是string的长度 */        
    }
    
    ViewDir((char *)data);
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ViewDir
*    功能说明: 显示根目录下的文件名
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ViewDir(char *_path)
{
    /* 本函数使用的局部变量占用较多，请修改启动文件，保证堆栈空间够用 */
    FRESULT result;
    DIR DirInf;
    FILINFO FileInf;
    uint32_t cnt = 0;

    /* 打开根文件夹 - 用完后需要 f_closedir  */
    result = f_opendir(&DirInf, _path); /* 1: 表示盘符 */
    if (result != FR_OK)
    {
        printf("打开根目录失败 (%d)\r\n", result);
        return;
    }

    printf("\r\n当前目录：%s\r\n", _path);

    /* 读取当前文件夹下的文件和目录 */
    printf("属性        |  文件大小 | 短文件名 | 长文件名\r\n");
    for (cnt = 0; ;cnt++)
    {
        result = f_readdir(&DirInf,&FileInf);         /* 读取目录项，索引会自动下移 */
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

        /* 打印文件大小, 最大4G */
        printf(" %10d", FileInf.fsize);

        printf("  %s |", FileInf.altname);    /* 短文件名 */

        printf("  %s\r\n", (char *)FileInf.fname);    /* 长文件名 */
    }
    
    f_closedir(&DirInf);    /*　关闭打开的目录 */
}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
