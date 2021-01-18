/*
*********************************************************************************************************
*
*    模块名称 : lua接口API
*    文件名称 : lua_if.c
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

#include "lua_if.h"
#include "bsp.h"
#include "param.h"
#include "file_lib.h"
#include "main.h"
#include "prog_if.h"
#include "crc32_stm32.h"
#include "modbus_print.h"

/* 

lua.h 定义错误代码
#define LUA_ERRRUN    2
#define LUA_ERRSYNTAX    3
#define LUA_ERRMEM    4
#define LUA_ERRGCMM    5
#define LUA_ERRERR    6
*/

/*
    luaconf.h 文件对浮点和整数的处理。 缺省64位整数，双精度浮点
        default configuration for 64-bit Lua ('long long' and 'double')
*/

lua_State *g_Lua = 0;

char s_lua_prog_buf[LUA_PROG_LEN_MAX + 1];
uint32_t s_lua_prog_len;
uint32_t s_lua_func_init_idx;
uint32_t s_lua_func_write_idx;
uint32_t s_lua_func_read_idx;

ALIGN_32BYTES(uint8_t s_lua_read_buf[LUA_READ_LEN_MAX]);
uint32_t s_lua_read_len;

static uint8_t s_lua_quit = 0;


static int get_runtime(lua_State* L);
static int check_runtime(lua_State* L);

static void lua_RegisterFunc(void);

static void LuaYeildHook(lua_State *_L, lua_Debug *ar);

///* lua源码调用该函数，会告警 */
//void exit(int status)
//{
//    return;
//}

int system(const char *cmd)
{
    return 0;
}

/* time_t : date/time in unix secs past 1-Jan-70 */
time_t time(time_t *_t)
{
    /* 以下代码来自于： https://blog.csdn.net/qq_29350001/article/details/87637350 */
    #define xMINUTE (60)          /* 1分的秒数 */
    #define xHOUR   (60*xMINUTE)  /* 1小时的秒数 */
    #define xDAY    (24*xHOUR   ) /* 1天的秒数 */
    #define xYEAR   (365*xDAY   ) /* 1年的秒数 */

    /* 将localtime（UTC+8北京时间）转为UNIX TIME，以1970年1月1日为起点 */
    static unsigned int  month[12] =
    {
        /*01月*/xDAY*(0),
        /*02月*/xDAY*(31),
        /*03月*/xDAY*(31+28),
        /*04月*/xDAY*(31+28+31),
        /*05月*/xDAY*(31+28+31+30),
        /*06月*/xDAY*(31+28+31+30+31),
        /*07月*/xDAY*(31+28+31+30+31+30),
        /*08月*/xDAY*(31+28+31+30+31+30+31),
        /*09月*/xDAY*(31+28+31+30+31+30+31+31),
        /*10月*/xDAY*(31+28+31+30+31+30+31+31+30),
        /*11月*/xDAY*(31+28+31+30+31+30+31+31+30+31),
        /*12月*/xDAY*(31+28+31+30+31+30+31+31+30+31+30)
    };
    unsigned int  seconds = 0;
    unsigned int  year = 0;
  
    RTC_ReadClock();
    year = g_tRTC.Year - 1970;                        /* 不考虑2100年千年虫问题 */
    seconds = xYEAR * year + xDAY * ((year + 1) /4);  /* 前几年过去的秒数 */
    seconds += month[g_tRTC.Mon - 1];                  /* 加上今年本月过去的秒数 */
    if ((g_tRTC.Mon > 2) && (((year + 2) % 4) == 0))        /* 2008年为闰年 */
    seconds += xDAY;                                /* 闰年加1天秒数 */
    seconds += xDAY * (g_tRTC.Day-1);     /* 加上本天过去的秒数 */
    seconds += xHOUR * g_tRTC.Hour;        /* 加上本小时过去的秒数 */
    seconds += xMINUTE * g_tRTC.Min;        /* 加上本分钟过去的秒数 */
    seconds += g_tRTC.Sec;                /* 加上当前秒数<br>　seconds -= 8 * xHOUR; */
    *_t = seconds;
    return *_t;
}

/*
*********************************************************************************************************
*    函 数 名: lua_Init
*    功能说明: 初始化lua虚拟机
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void lua_Init(void)
{    
    g_Lua = luaL_newstate();         /* 建立Lua运行环境 */
    luaL_openlibs(g_Lua);
    luaopen_base(g_Lua);
    
    lua_RegisterFunc();        /* 注册c函数，供lua调用 */

	//错误处理函数
	lua_sethook(g_Lua, LuaYeildHook, LUA_MASKLINE, 0);    
}

/* 关闭释放Lua */
void lua_DeInit(void)
{    
    if (g_Lua > 0)
    {
        lua_close(g_Lua);                /* 释放内存 */
        g_Lua = 0;
    }
}

// 每行执行的钩子函数，用于终止lua程序
extern MEMO_T g_LuaMemo;
extern void PG_PrintText(char *_str);
extern uint8_t g_LuaSubStatus;
void LuaYeildHook(lua_State *_L, lua_Debug *ar)
{
	if (s_lua_quit == 1)
	{
		s_lua_quit = 0;

		lua_yield(_L, 0);
	}
    
    if (g_MainStatus == MS_LUA_EXEC_FILE)
    {
        uint8_t ucKeyCode;
        
        /* 显示Lua程序print的字符串. 内容在bsp_uart_fif文件 fputc 函数填充的 */
        if (g_LuaSubStatus == 0)    /* 非GUI界面，才执行如下语句 */
        {
            if (g_LuaMemo.Refresh == 1)     
            {
                LCD_SetEncode(ENCODE_GBK);
                LCD_DrawMemo(&g_LuaMemo);            
                LCD_SetEncode(ENCODE_UTF8);
            }
        }
        
        ucKeyCode = bsp_GetKey2(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {            
                case KEY_LONG_DOWN_C:       /* C键长按 - 终止Lua */
                    while(bsp_GetKey());    /* 清空lua运行期间的按键消息 */
                    lua_yield(_L, 0);   
                    break;

                default:
                    break;
            }
        }        
    }
    else  
    //if (g_MainStatus == MS_PROG_WORK)  在其他状态按C键退出
    {
        uint8_t ucKeyCode;

        ucKeyCode = bsp_GetKey2(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {            
                case KEY_LONG_DOWN_C:   /* C键长按 - 终止Lua */
                    PG_PrintText("用户终止运行");
                    g_tProg.AutoStart = 0;
                
                    //while(bsp_GetKey());    /* 清空lua运行期间的按键消息 */
                    lua_yield(_L, 0); 
                    break;

                case KEY_UP_S:
                case KEY_UP_C:
                    if (g_tProg.AutoStart == 1)
                    {
                        ;
                    }
                    break;
                
                default:
                    break;
            }
        }        
    }
}

// 终止lua
void lua_abort(void)
{
	s_lua_quit = 1;
}

/*
判断lua中变量是否存在
basic types
#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define LUA_NUMTAGS		9
*/
int lua_CheckGlobal(const char *name)
{
    int re;
    
    re = lua_getglobal(g_Lua, name);
    lua_pop(g_Lua, 1);
    
    return re;
}

// H7-TOOL上电时，启动lua
void lua_PowerOnLua(void)
{
    lua_DeInit();   // 先释放
    
    lua_Init();     // 重新分配内存
    
    //lua_do("beep()"); 
}
                
// 装载文件并初始化lua全局对象
void lua_DownLoadFile(char *_path)
{
    lua_DeInit();   // 先释放
    
    lua_Init();
    
    // 读文件到内存
    s_lua_prog_len = ReadFileToMem(_path, 0, s_lua_prog_buf, LUA_PROG_LEN_MAX);
    
    if (s_lua_prog_len > 0)
    {
        s_lua_prog_buf[s_lua_prog_len] = 0; /* 文件末尾补0 */    
    }    
}

void lua_RunLuaProg(void)
{
    lua_do(s_lua_prog_buf);
}

// 重新封装下执行函数，支持打印调试信息
void lua_do(char *buf)
{
    int re;

    static const char *str;    

    while(bsp_GetKey2());   /* 读空按键FIFO, */
    
    re = luaL_dostring(g_Lua, buf);
	if (re != LUA_OK)
	{        
        str = lua_tostring(g_Lua, -1);
        if (strstr(str, "attempt to yield from outside a coroutine"))   /* 用户终止了程序 */
        {
            /* 程序被用户终止\r\n 这是UTF8编码,需要GBK编码 */
            printf("\xB3\xCC\xD0\xF2\xB1\xBB\xD3\xC3\xBB\xA7\xD6\xD5\xD6\xB9\r\n");    
        }
        else    /* 程序语法或执行错误 */
        {
            printf(str);
            printf("\r\n");
        }  
		lua_pop(g_Lua, 1); //将错误信息出栈        
	}

    /* 显示Lua程序print的字符串. 内容在bsp_uart_fifo文件 fputc 函数填充的 */
    if (g_LuaMemo.Refresh == 1)     
    {
        LCD_SetEncode(ENCODE_GBK);
        LCD_DrawMemo(&g_LuaMemo);            
        LCD_SetEncode(ENCODE_UTF8);
    } 
}

// 通信程序用的函数，下载文件到lua程序缓冲区，不执行。
void lua_DownLoad(uint32_t _addr, uint8_t *_buf, uint32_t _len, uint32_t _total_len)
{
    uint32_t i;
    
    for (i = 0; i < _len; i++)
    {
        if (_addr + i < LUA_PROG_LEN_MAX)
        {
            s_lua_prog_buf[_addr + i] = _buf[i];
        }
    }
    
    if (_total_len < LUA_PROG_LEN_MAX)
    {
        s_lua_prog_len = _total_len;
        s_lua_prog_buf[s_lua_prog_len] = 0;
    }
    else
    {
        s_lua_prog_len = 0;
    }
    
//    if (g_Lua > 0)
//    {
//        lua_DeInit();
//    }
//    lua_Init();
}

void lua_Poll(void)
{
    if (g_tVar.LuaRunOnce == 1)
    {
        g_tVar.LuaRunOnce = 0;
        //luaL_dostring(g_Lua, s_lua_prog_buf);
        lua_do(s_lua_prog_buf);
    }
}

void lua_DoInit(void)
{
    luaL_dostring(g_Lua, "init()");
}

void lua_StackDump(lua_State *L) 
{
    int i;
    int top = lua_gettop(L);

    printf("\r\nlua stack top = %d\r\n", top);   
    for (i = 1; i <= top; i++) 
    {
        int t = lua_type(L, i);
        switch (t) 
        {
            case LUA_TSTRING:
                printf("%d, %s\r\n", i, lua_tostring(L, i));
                break;

            case LUA_TBOOLEAN:
                printf("%d, %s\r\n", i, lua_toboolean(L, i) ? "true":"false");
                break;

            case LUA_TNUMBER:
                printf("%d, %f\r\n", i, lua_tonumber(L, i));
                break;

            default:
                printf("%d, %s\r\n", i, lua_typename(L, t));
                break;

        }
    }
}


/*
*********************************************************************************************************
*    函 数 名: beep
*    功能说明: 蜂鸣函数 lua调用
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int beep(lua_State* L)
{
    uint16_t usBeepTime;
    uint16_t usStopTime;
    uint16_t usCycle;
    
    if (lua_type(L, 1) == LUA_TNUMBER)  /* 判断第1个参数. */
    {        
        usBeepTime = luaL_checknumber(L, 1);
        
        if (lua_type(L, 2) == LUA_TNUMBER)  /* 判断第2个参数. */
        {
            usStopTime = luaL_checknumber(L, 2);
        }
        else
        {
            return 0;
        }

        if (lua_type(L, 3) == LUA_TNUMBER)  /* 判断第3个参数. */
        {
            usCycle = luaL_checknumber(L, 3);
        } 
        else
        {
            return 0;
        }        
            
        BEEP_Start(usBeepTime, usStopTime, usCycle); /* 鸣叫50ms，停10ms， 1次 */
    }
    else
    {
        BEEP_Start(5, 1, 1); /* 鸣叫50ms，停10ms， 1次 */
    }
    
    //返回值用于提示该C函数的返回值数量，即压入栈中的返回值数量。
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: delayus
*    功能说明: 微秒延迟 lua函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int delayus(lua_State* L)
{
    uint32_t n = luaL_checknumber(L,1);

    bsp_DelayUS(n);
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: delayms
*    功能说明: 毫秒延迟 lua函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int delayms(lua_State* L)
{
    uint32_t n = luaL_checknumber(L,1);

    bsp_DelayMS(n);
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: HexToTable
*    功能说明: HEX格式的string转换为table返回。用于lua api  "0800 0123" --> {0x0800, 0x0123}
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int HexToTable(lua_State* L)
{
    const char *data;
    size_t len;
    int i;
    uint32_t temp;
    uint32_t start;
    uint16_t cnt = 0;
    
    if (lua_type(L, 1) == LUA_TSTRING)          /* 判断第1个参数 */
    {        
        data = luaL_checklstring(L, 1, &len);   /* 1是参数的位置， len是string的长度 */     
    }    
    else
    {
        return 0;
    }
    
    temp = 0;
    for (i = 0; i < len; i++)
    {
        if (data[i] == ' ')
        {
            if (start == 1)
            {
                cnt++;
                lua_pushnumber(L, temp);    /* 添加返回值 */
                
                start = 0;
                temp = 0;
            }
        }
        else
        {
            start = 1;
            
            temp = temp * 16;
            temp += CharToInt(data[i]);            
        }
    }

    if (start == 1)
    {
        cnt++;
        lua_pushnumber(L, temp);    /* 添加返回值 */
    }
    
    return cnt;
}

/*
*********************************************************************************************************
*    函 数 名: HexToLuaString
*    功能说明: HEX格式的string转换为二进制string返回。用于lua api  "0800 0123" --> 0x08 0x00, 0x01 0x23
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int HexToBinString(lua_State* L)
{
    const char *data;
    size_t len;
    int i;
    uint32_t temp;
    uint32_t start;
    uint16_t cnt = 0;
    
    if (lua_type(L, 1) == LUA_TSTRING)          /* 判断第1个参数 */
    {        
        data = luaL_checklstring(L, 1, &len);   /* 1是参数的位置， len是string的长度 */     
    }
    else
    {
        return 0;
    }
    
    temp = 0;
    start = 0;
    for (i = 0; i < len; i++)
    {
        if ( (data[i] >= '0' && data[i] <= '9') || (data[i] >= 'A' && data[i] <= 'F') 
            || (data[i] >= 'a' && data[i] <= 'f') )
        {
            if (start == 0)
            {
                temp = CharToInt(data[i]) * 16;
                start = 1;
            }
            else
            {
                start = 0;
                temp += CharToInt(data[i]);
                
                if (cnt < LUA_READ_LEN_MAX)
                {
                    s_lua_read_buf[cnt++] = temp;
                }
            }       
        }
    }
    
    lua_pushlstring(L, (char *)s_lua_read_buf, cnt); 
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: print_hex
*    功能说明: 打印二进制数据    print_hex("123", 16)  print_hex("123", 16, 0x08000000)
*    形    参: 第1个是二进制数据，第2个是换行位置(缺省16)
*    返 回 值: 无
*********************************************************************************************************
*/
extern void MODH_PrintByte(char _ch);
static int print_hex(lua_State* L)
{
    const char *data;
    size_t len;
    size_t line_bytes;
    int i;
    int j;
    char buf[128];
    uint16_t pos = 0;
    uint8_t ch;
    size_t intvalue;
    uint8_t DispAddr = 0;
    uint32_t addr = 0;
    
    if (lua_type(L, 1) == LUA_TSTRING)          /* 判断第1个参数 */
    {        
        data = luaL_checklstring(L, 1, &len);   /* 1是参数的位置， len是string的长度 */     
    }
    else
    {
        buf[0] = 0;
        for (i = 1; i <= 8; i++)
        {
            if (lua_type(L, i) == LUA_TNUMBER)
            {
                intvalue = luaL_checknumber(L, i);
                
                sprintf(&buf[strlen(buf)], "%08X ", intvalue);
            }
            else
            {
                break;
            }            
        }
        
        if (i > 1)
        {
            strcpy(&buf[strlen(buf)], "\r\n");
            
            MODH_PrintBuf((uint8_t *)buf, strlen(buf));            
        }
        return 0;
    }

    if (lua_type(L, 2) == LUA_TNUMBER)          /* 判断第2个参数 */
    {
        line_bytes = luaL_checknumber(L, 2);    /* 每行几个字节 */
    }
    else
    {
        line_bytes = 16;
    }    
    if (line_bytes == 0)
    {
        line_bytes = 16;
    }
    else if (line_bytes > 32)
    {
        line_bytes = 32;
    }
    
    if (lua_type(L, 3) == LUA_TNUMBER)      /* 判断第3个参数 */
    {
        addr = luaL_checknumber(L, 3);      /* 地址 */
        DispAddr = 1;     
    }
    else
    {
        DispAddr = 0;
    }

    /* 整行打印 */
    for (i = 0; i < len / line_bytes; i++)
    {               
        pos = 0;
        if (DispAddr != 0)
        {
            sprintf(&buf[pos], "%08X : ", addr);
            pos = 11;
        }        
        for (j = 0; j < line_bytes; j++)
        {
            ch = *data++;  
            addr++;
            
  
            buf[pos++] = BcdToChar(ch >> 4);
            buf[pos++] = BcdToChar(ch & 0x0F);
            buf[pos++] = ' ';
        }
        buf[pos++] = '\r'; 
        buf[pos++] = '\n'; 
        MODH_PrintBuf((uint8_t *)buf, pos);   
        bsp_Idle();
    }
    
    /* 打印不足一行的数据 */
    if (len % line_bytes)
    {
        pos = 0;
        for (j = 0; j < (len % line_bytes); j++)
        {
            ch = *data++;
            buf[pos++] = BcdToChar(ch >> 4);
            buf[pos++] = BcdToChar(ch & 0x0F);
            buf[pos++] = ' ';
        }
        buf[pos++] = '\r'; 
        buf[pos++] = '\n';   
        MODH_PrintBuf((uint8_t *)buf, pos); 
    }
    
    return 1;
}


/*
*********************************************************************************************************
*    函 数 名: write_clock
*    功能说明: 写时钟
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int write_clock(lua_State* L)
{
    uint16_t year;
    uint8_t mon, day, hour, min, sec;
    
    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        year = luaL_checknumber(L, 1);
    }
    if (lua_type(L, 2) == LUA_TNUMBER)
    {
        mon = luaL_checknumber(L, 2);
    }
    if (lua_type(L, 3) == LUA_TNUMBER)
    {
        day = luaL_checknumber(L, 3);
    }
    if (lua_type(L, 4) == LUA_TNUMBER)
    {
        hour = luaL_checknumber(L, 4);
    }
    if (lua_type(L, 5) == LUA_TNUMBER)
    {
        min = luaL_checknumber(L, 5);
    }
    if (lua_type(L, 6) == LUA_TNUMBER)
    {
        sec = luaL_checknumber(L, 6);
    }    
    
    RTC_WriteClock(year, mon, day, hour, min, sec);
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: read_clock
*    功能说明: 写时钟
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int read_clock(lua_State* L)
{
    static uint8_t rtc_buf[8];
    
    RTC_ReadClock();

    rtc_buf[0] = g_tRTC.Year >> 8;
    rtc_buf[1] = g_tRTC.Year;
    rtc_buf[2] = g_tRTC.Mon;
    rtc_buf[3] = g_tRTC.Day;
    rtc_buf[4] = g_tRTC.Hour;
    rtc_buf[5] = g_tRTC.Min;
    rtc_buf[6] = g_tRTC.Sec;
    rtc_buf[7] = g_tRTC.Week;
    
    lua_pushlstring(L, (const char *)rtc_buf, 8); 
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: get_runtime
*    功能说明: 获得CPU运行时间。ms单位
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int get_runtime(lua_State* L)
{
    int32_t runtime;
    
    runtime = bsp_GetRunTime();
    lua_pushnumber(L, runtime); 
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: check_runtime
*    功能说明: 判断时间长度
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int check_runtime(lua_State* L)
{
    int32_t lasttime;
    int32_t re;
    
    if (lua_type(L, 1) == LUA_TNUMBER) /* 判断第1个参数 */
    {
        lasttime = luaL_checknumber(L, 1);
    }

    re = bsp_CheckRunTime(lasttime);
    
    lua_pushnumber(L,     re); 
    return 1;
}

int l_my_print(lua_State* L) 
{
    int nargs = lua_gettop(L);

    for (int i=1; i <= nargs; i++) 
    {
        if (lua_isstring(L, i)) 
        {
            /* Pop the next arg using lua_tostring(L, i) and do your print */
            
            //lua_tostring(L, i)
        }
        else 
        {
            /* Do something with non-strings if you like */
        }
    }

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: get_key
*    功能说明: 读取键值
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int get_key(lua_State* L)
{
    uint8_t key;
    
    key = bsp_GetKey3();
    lua_pushnumber(L, key); 
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: put_key
*    功能说明: 模拟一个键值
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int put_key(lua_State* L)
{
    uint8_t key;
    
    if (lua_type(L, 1) == LUA_TNUMBER)  /* 判断第1个参数 */
    {
        key = luaL_checknumber(L, 1);
        bsp_PutKey(key);
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: clear_key
*    功能说明: 清除按键缓冲区
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int clear_key(lua_State* L)
{   
    bsp_ClearKey();
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: put_key
*    功能说明: 模拟一个键值
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int put_msg(lua_State* L)
{
    uint32_t msg_code;
    uint32_t msg_param;
    
    if (lua_type(L, 1) == LUA_TNUMBER)  /* 判断第1个参数 */
    {
        msg_code = luaL_checknumber(L, 1);
    }
    else
    {
        return 0;
    }

    if (lua_type(L, 2) == LUA_TNUMBER)  /* 判断第2个参数 */
    {
        msg_param = luaL_checknumber(L, 2);
    }
    else
    {
        msg_param = 0;
    }
    
    bsp_PutMsg(msg_code, msg_param);
    
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: load_file
*    功能说明: 加载lua文件
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int load_file(lua_State* L)
{   
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: get_rng
*    功能说明: 获得随机数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int get_rng(lua_State* L)
{
    uint16_t len;

    if (lua_type(L, 1) == LUA_TNUMBER)  /* 判断第1个参数 : 随机数长度 */
    {
        len = luaL_checknumber(L, 1);
    }

    if (len > 1024)
    {
        len = 1024;
    }
    
    bsp_GenRNG(s_lua_read_buf, len);    

    lua_pushlstring(L, (char *)s_lua_read_buf, len); 
    return 1;
}



/*
*********************************************************************************************************
*    函 数 名: crc32_stm32
*    功能说明: 计算CRC32，用STM32硬件CRC单元.  
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int crc32_stm32(lua_State* L)
{
    const char *data;
    size_t len;    
    uint32_t temp;
    uint32_t crc;
    
    if (lua_type(L, 1) == LUA_TSTRING)          /* 判断第1个参数 */
    {        
        data = luaL_checklstring(L, 1, &len);   /* 1是参数的位置， len是string的长度 */     
    }

    if (lua_type(L, 2) == LUA_TNUMBER)          /* 判断第2个参数 */
    {        
        temp = luaL_checknumber(L, 2);  
    }    
    
    crc = STM32_CRC32_LE((uint8_t *)data, len, temp);
    
    lua_pushnumber(L, crc); 
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: lua_GetVarUint32
*    功能说明: 读取lua全局变量，返回UINT32型
*    形    参: _VarName : 变量名    _Default : 缺省值
*    返 回 值: 变量值
*********************************************************************************************************
*/
uint32_t lua_GetVarUint32(const char *_VarName, uint32_t _Default)
{
    uint32_t data;
    
    /* 读取复位类型： 软件还是硬件复位 */
    lua_getglobal(g_Lua, _VarName);  
    if (lua_isinteger(g_Lua, -1)) 
    {    
        data = lua_tointeger(g_Lua, -1);
    }
    else
    {
        /* 没有定义则用缺省值 */
        data = _Default;
    }
    lua_pop(g_Lua, 1);

    return data;        
}

/*
*********************************************************************************************************
*    函 数 名: lua_ReadDev
*    功能说明: 通用外设读。 ok, temp = read("DS18B20", 1)
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_ReadDev(lua_State* L)
{
    const char *device;
    size_t len;    
    
    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {        
        device = luaL_checklstring(L, 1, &len);     /* 1是参数的位置， len是string的长度 */     
    }
    
    if (strcmp(device, "DS18B20") == 0)  /* re,temp = read("DS18B20", 0) */
    {
        float ftemp;
        uint8_t ch;
        
        if (lua_type(L, 2) == LUA_TNUMBER)          /* 判断第2个参数, 通道 */
        {
            ch = luaL_checknumber(L, 2);
        }
        else
        {
            ch = 0;
        }
    
        if (DS18B20_ReadTemp(ch, &ftemp) == 1)  /* 正常返回温度值 */
        {
            lua_pushnumber(L, 1);
            lua_pushnumber(L, ftemp);            
            return 2;
        }
        else
        {
            lua_pushnumber(L, 0); 
            return 1;
        }
    }  
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_WriteDev
*    功能说明: 通用外设写 
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static int lua_WriteDev(lua_State* L)
{
    const char *device;
    size_t len;    
    
    if (lua_type(L, 1) == LUA_TSTRING)              /* 判断第1个参数 */
    {        
        device = luaL_checklstring(L, 1, &len);     /* 1是参数的位置， len是string的长度 */     
    }
    
    if (strcmp(device, "DS18B20") == 0)  /* re,temp = read("DS18B20", 0) */
    {
        ;
    }  
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: lua_RegisterFunc
*    功能说明: 注册lua可调用的c函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void lua_lcd_RegisterFun(void);
static void lua_RegisterFunc(void)
{
    //将指定的函数注册为Lua的全局函数变量，其中第一个字符串参数为Lua代码
    //在调用C函数时使用的全局函数名，第二个参数为实际C函数的指针。
    lua_register(g_Lua, "beep", beep);
    lua_register(g_Lua, "delayus", delayus);
    lua_register(g_Lua, "delayms", delayms);
    lua_register(g_Lua, "print_hex", print_hex);
    lua_register(g_Lua, "hex_to_table", HexToTable);
    lua_register(g_Lua, "hex_to_bin", HexToBinString);    
        
    lua_register(g_Lua, "write_clock", write_clock);
    lua_register(g_Lua, "read_clock", read_clock);
    lua_register(g_Lua, "get_runtime", get_runtime);
    lua_register(g_Lua, "check_runtime", check_runtime);
    lua_register(g_Lua, "get_key", get_key);
    lua_register(g_Lua, "put_key", put_key);
    lua_register(g_Lua, "clear_key", clear_key);
    lua_register(g_Lua, "put_msg", put_msg);    
    lua_register(g_Lua, "load_file", load_file);
    
    lua_register(g_Lua, "get_rng", get_rng);
    lua_register(g_Lua, "crc32_stm32", crc32_stm32);

    /* 读写2个通用函数 */
    lua_register(g_Lua, "read", lua_ReadDev);
    lua_register(g_Lua, "write", lua_WriteDev);
    
    /* 注册接口函数 */
    lua_gpio_RegisterFun();    
    lua_i2c_RegisterFun();
    lua_spi_RegisterFun();
    lua_tim_RegisterFun();
    lua_tcp_RegisterFun();
    lua_qspi_RegisterFun();
    lua_fatfs_RegisterFun();
    lua_swd_RegisterFun();   
    lua_swim_RegisterFun();  
    lua_adc_RegisterFun();
    lua_dac_RegisterFun();
    lua_reg_RegisterFun();
    lua_extio_RegisterFun();
    lua_uart_RegisterFun();
    lua_lcd_RegisterFun();
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
