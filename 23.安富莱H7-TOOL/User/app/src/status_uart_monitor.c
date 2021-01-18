/*
*********************************************************************************************************
*
*    模块名称 : 数据监视器
*    文件名称 : status_DataMonitor.c
*    版    本 : V1.0
*    说    明 : 数据监视器主程序
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2020-09-16  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"


/* 多行文本框 */
#define TITLE_X     0
#define TITLE_Y     0 
#define TITLE_H     20  
#define TITLE_W     (240 - 2 * TITLE_X)

/* 多行文本框 */
#define MEMO_X     0
#define MEMO_Y     20 
#define MEMO_H     (240 - MEMO_Y - 0)  
#define MEMO_W     (240 - 2 * MEMO_X)

extern char g_OutText[4 * 1024];
    
MEMO_T g_RecMemo = {0};

static uint8_t s_FrameHead1 = 1;
static uint8_t s_FrameHead2 = 1;

static uint8_t s_FrameTimeout1 = 1;
static uint8_t s_FrameTimeout2 = 1;

static uint64_t s_FrameHeadTime1;
static uint64_t s_FrameHeadTime2;

static uint64_t s_BeginTime;

static uint32_t s_RxCount1, s_RxCount2;

static uint8_t s_Pause = 0;

static void DispRxCounter(uint32_t _count1, uint32_t _count2);

static void UartSettingMenu(void);
static void DispClearTitle(void);

MENU_T g_tMenuUart;

/*
*********************************************************************************************************
*    函 数 名: Uart1ReciveNew
*    功能说明: RS485串口接收到新的字节的回调函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void UartReciveNew1(uint8_t _byte)
{
    char buf[32];
    static uint8_t LastChar = 0;
    
    if (s_Pause == 1)   /* 暂停 */
    {
        return;
    }
    
    if (s_FrameHead1 == 1)
    {
        s_FrameHead1 = 0;
        
        /* 记录下准确的首字符时刻 us */
        s_FrameHeadTime1 = bsp_GetRunTimeUs() - s_BeginTime;  
    
        {
            if (g_tParam.UartMonTimeStamp == 1)     /* 加上时间戳 */
            {
                if (g_tParam.UartMonHex == 0)   /* ASCII格式显示 */
                {
                    if (LastChar == 0x0D || LastChar == 0x0A)
                    {
                        sprintf(buf, "[%llu.%03llu] ", s_FrameHeadTime1 / 1000, s_FrameHeadTime1 % 1000);
                    }
                    else
                    {
                        sprintf(buf, "\r\n[%llu.%03llu] ", s_FrameHeadTime1 / 1000, s_FrameHeadTime1 % 1000);
                    }
                    LCD_MemoAddStr(&g_RecMemo, buf);
                }
                else        /* HEX */
                {
                    sprintf(buf, "\r\n[%llu.%03llu] ", s_FrameHeadTime1 / 1000, s_FrameHeadTime1 % 1000);
                    LCD_MemoAddStr(&g_RecMemo, buf);                      
                }
            }
            else 
            {
                /* 无时间戳，HEX格式，自动换行使能时，自动增加回车换行 */
                if (g_tParam.UartMonHex == 1 && g_tParam.UartMonWordWrap == 1)
                {
                    sprintf(buf, "\r\n");
                    LCD_MemoAddStr(&g_RecMemo, buf); 
                }
            }
        }
    }     
    
    if (g_tParam.UartMonHex == 0)   /* ASCII格式显示 */
    {
        LCD_MemoAddChar(&g_RecMemo, _byte);  
    }
    else    /* HEX格式显示 */
    {
        sprintf(buf, "%02X ", _byte);
        LCD_MemoAddStr(&g_RecMemo, buf); 
    }            

    LastChar = _byte;
    s_RxCount1++;     
}

void UartReciveNew2(uint8_t _byte)
{
    char buf[32];
    static uint8_t LastChar = 0;    
    
    if (s_Pause == 1)   /* 暂停 */
    {
        return;
    }
    
    if (s_FrameHead2 == 1)
    {
        s_FrameHead2 = 0;
        
        /* 记录下准确的首字符时刻 us */
        s_FrameHeadTime2 = bsp_GetRunTimeUs() - s_BeginTime;  
    
        {
            if (g_tParam.UartMonTimeStamp == 1)     /* 加上时间戳 */
            {
                if (g_tParam.UartMonHex == 0)   /* ASCII格式显示 */
                {
                    if (LastChar == 0x0D || LastChar == 0x0A)
                    {
                        sprintf(buf, "<%llu.%03llu> ", s_FrameHeadTime2 / 1000, s_FrameHeadTime2 % 1000);
                    }
                    else
                    {
                        sprintf(buf, "\r\n<%llu.%03llu> ", s_FrameHeadTime2 / 1000, s_FrameHeadTime2 % 1000);
                    }
                    LCD_MemoAddStr(&g_RecMemo, buf);
                }
                else        /* HEX */
                {
                    sprintf(buf, "\r\n<%llu.%03llu> ", s_FrameHeadTime2 / 1000, s_FrameHeadTime2 % 1000);
                    LCD_MemoAddStr(&g_RecMemo, buf);                      
                }
            }
            else 
            {
                /* 无时间戳，HEX格式，自动换行使能时，自动增加回车换行 */
                if (g_tParam.UartMonHex == 1 && g_tParam.UartMonWordWrap == 1)
                {
                    sprintf(buf, "\r\n");
                    LCD_MemoAddStr(&g_RecMemo, buf); 
                }
            }            
        }
    }     
    
    if (g_tParam.UartMonHex == 0)   /* ASCII格式显示 */
    {
        if (_byte < ' ')
        {
            _byte = ' ';    /* 不可见字符转换为空格 */
        }
        LCD_MemoAddChar(&g_RecMemo, _byte);  
    }
    else    /* HEX格式显示 */
    {
        sprintf(buf, "%02X ", _byte);
        LCD_MemoAddStr(&g_RecMemo, buf); 
    }            

    LastChar = _byte;
    
    s_RxCount2++; 
}

/*
*********************************************************************************************************
*    函 数 名: UartIdleLine1
*    功能说明: RS485串口线路空闲中断回调函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void UartIdleLine1(void)
{
    s_FrameHead1 = 1;
    s_FrameTimeout1 = 1;
}

void UartIdleLine2(void)
{
    s_FrameHead2 = 1;
    s_FrameTimeout2 = 1;
}

/*
*********************************************************************************************************
*    函 数 名: status_MonitorUart
*    功能说明: 串口监视器
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern const char *UartMonParam1[];
extern const char *UartMonParam6[];
void status_MonitorUart(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    uint8_t fInit;
    FONT_T tFontMemo;
    char buf[128];

    {
        tFontMemo.FontCode = FC_ST_16;          /* 字体代码 16点阵 */
        tFontMemo.FrontColor = CL_WHITE;        /* 字体颜色 */
        tFontMemo.BackColor = CL_MASK;          /* 文字背景颜色 */
        tFontMemo.Space = 0;                    /* 文字间距，单位 = 像素 */    
    }

    DispClearTitle();
    
    {
        g_RecMemo.Left = MEMO_X;
        g_RecMemo.Top = MEMO_Y;
        g_RecMemo.Height = MEMO_H;
        g_RecMemo.Width = MEMO_W;
        g_RecMemo.Font = &tFontMemo;
        //g_LuaMemo.Color = CL_WHITE;
        g_RecMemo.Text = g_OutText;
        g_RecMemo.MaxLen = sizeof(g_OutText);
        g_RecMemo.LineCount = 0;
        g_RecMemo.WordWrap = 1;        
        g_RecMemo.LineOffset = 0;
        
        LCD_InitMemo(&g_RecMemo);
        LCD_DrawMemo(&g_RecMemo);
    }
        
    LCD_SetEncode(ENCODE_GBK);
    
    /* V1.35, 检查串口参数 1.41 修正升级后串口缺省参数bug */
    UartMonInitParam(MODIFY_PARAM_UART_MON);  
    UartMonCheckParam();    /* 检查参数 */
    
    s_Pause = 0;    
    fInit = 1;
    fRefresh = 1;
    s_FrameHead1 = 1;
    s_FrameHead2 = 1;
    while (g_MainStatus == MS_MONITOR_UART)
    {
        bsp_Idle();
        
        if (fInit == 1)
        {
            uint32_t BaudTab[] = {1200, 2400, 4700, 9600, 19200, 38400, 57600, 115200};
            uint32_t parity[] = {UART_PARITY_NONE, UART_PARITY_ODD, UART_PARITY_EVEN};
            char *ParTable[] = {"None", "Odd", "Even"};
            
            fInit = 0;
            
            /* 字体 */
            if (g_tParam.UartMonFont == 0)
            {
                tFontMemo.FontCode = FC_ST_12;
            }
            else if (g_tParam.UartMonFont == 1)
            {
                tFontMemo.FontCode = FC_ST_16;
            }
            else if (g_tParam.UartMonFont == 2)
            {
                tFontMemo.FontCode = FC_ST_24;
            }   
            else
            {
                tFontMemo.FontCode = FC_ST_16;
            }
            
            s_BeginTime = bsp_GetRunTimeUs();
           
            
            /* 设置第1个串口 9600 无校验 */
            bsp_SetUartParam(COM_RS485, BaudTab[g_tParam.UartMonBaud], 
                parity[g_tParam.UartMonParit], UART_WORDLENGTH_8B, UART_STOPBITS_1);                                      
            
            /* 配置第1个串口中断回调函数 */
            comSetCallBackReciveNew(COM_RS485, UartReciveNew1);
            comSetCallBackIdleLine(COM_RS485, UartIdleLine1);            
            comSetReciverTimeout(COM_RS485, 20);   /* 接收器超时时间 */
            
            /* 设置第2个串口 9600 无校验 */
            EIO_ConfigPort(EIO_D1, ES_GPIO_UART);     /* UART7_RX */
            
            bsp_SetUartParam(COM_UART7, BaudTab[g_tParam.UartMonBaud], 
                parity[g_tParam.UartMonParit], UART_WORDLENGTH_8B, UART_STOPBITS_1);   
            
            /* 配置第2个串口中断回调函数 */
            comSetCallBackReciveNew(COM_UART7, UartReciveNew2);
            comSetCallBackIdleLine(COM_UART7, UartIdleLine2);            
            comSetReciverTimeout(COM_UART7, 20);   /* 接收器超时时间 */            
            
            sprintf(buf, "Uart : %d, %s", BaudTab[g_tParam.UartMonBaud], ParTable[g_tParam.UartMonParit]);
            if (g_tParam.UartMonHex == 0) strcat(buf, ", ASCII\r\n"); else strcat(buf, ", HEX\r\n");
            
            LCD_MemoAddStr(&g_RecMemo, buf); 
            
            s_RxCount1 = 0;
            s_RxCount2 = 0;
        }
        
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            DispRxCounter(s_RxCount1, s_RxCount2);
            
            LCD_DrawMemo(&g_RecMemo);
        }             
        
        if (s_FrameTimeout1 == 1)
        {
            s_FrameTimeout1 = 0;
            
            fRefresh = 1;
        }

        if (s_FrameTimeout2 == 1)
        {
            s_FrameTimeout2 = 0;
            
            fRefresh = 1;
        }        

        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:          /* S键 上 */
                    LCD_MemoPageUp(&g_RecMemo, 5);
                    fRefresh = 1;
                    break;

                case KEY_LONG_DOWN_S:   /* S键 长按 */
                    UartSettingMenu();              /* 参数设置，阻塞 */
                    if (g_tMenuUart.Cursor == 0)    /* 返回 */
                    {
                        ;
                    }
                    else if (g_tMenuUart.Cursor == 1)    /* 暂停/恢复 */
                    {
                        if (s_Pause == 0)
                        {
                            s_Pause = 1;
                            LCD_MemoAddStr(&g_RecMemo, "\r\npause\r\n");     
                        }
                        else
                        {
                            s_Pause = 0;
                            LCD_MemoAddStr(&g_RecMemo, "\r\nresume\r\n"); 
                        }
                    }
                    else if (g_tMenuUart.Cursor == 2)    /* 清屏 */
                    {
                        LCD_MemoClear(&g_RecMemo);
                        s_RxCount1 = 0; 
                        s_RxCount2 = 0;                        
                    }
                    else if (g_tMenuUart.Cursor == 3)    /* 设置串口参数 */
                    {
                        LCD_SetEncode(ENCODE_UTF8);
                        ModifyParam(MODIFY_PARAM_UART_MON);       /* 参数修改界面，阻塞 */
                        LCD_SetEncode(ENCODE_GBK);       
                        fInit = 1;                        
                    }
                    DispClearTitle();
                    fRefresh = 1;                    
                    break;

                case KEY_UP_C:              /* C键 下 */
                    LCD_MemoPageDown(&g_RecMemo, 5);
                    fRefresh = 1;
                    break;

                case KEY_LONG_DOWN_C:   /* C键长按 */
                    g_MainStatus = MS_EXTEND_MENU_REC;
                    break;

                default:
                    break;
            }
        }
    }
    
    LCD_SetEncode(ENCODE_UTF8);
    
    /* 取消所有的回调函数 */
    comSetCallBackReciveNew(COM_RS485, 0);
    comSetCallBackIdleLine(COM_RS485, 0);

    comSetCallBackReciveNew(COM_UART7, 0);
    comSetCallBackIdleLine(COM_UART7, 0);
    EIO_ConfigPort(EIO_D1, ES_GPIO_IN);     /* UART7_RX */
}

/*
*********************************************************************************************************
*    函 数 名: DispClearTitle
*    功能说明: 清标题栏背景
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispClearTitle(void)
{
    LCD_Fill_Rect(TITLE_X, TITLE_Y, TITLE_H, TITLE_W, CL_WHITE);
}

static void DispRxCounter(uint32_t _count1, uint32_t _count2)
{
    FONT_T tFont;    
    uint16_t x;
    uint16_t y;
    char buf[128];
    
    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_16;          /* 字体代码 16点阵 */
        tFont.FrontColor = INFO_NAME_COLOR; /* 字体颜色 */
        tFont.BackColor = INFO_BACK_COLOR;  /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    x = 2;
    y = 2;
    
    sprintf(buf, "[RX1:%8d] <RX2:%8d>", _count1, _count2);
    LCD_DispStr(x, y, buf, &tFont);
}

const uint8_t *g_MenuUart_Text[] = 
{
    "0 返回",
    "1 暂停/恢复",
    "2 清屏",    
    "3 设置串口参数",

    /* 结束符号, 用于菜单函数自动识别菜单项个数 */
    "&"
};


/*
*********************************************************************************************************
*    函 数 名: UartMenuPro
*    功能说明: 串口监视器设置菜单
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void UartSettingMenu(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    static uint8_t s_MenuInit = 0;
    uint8_t fQuit = 0;

    if (s_MenuInit == 0)
    {
        s_MenuInit = 1;
        
        g_tMenuUart.Left = MENU_LEFT;
        g_tMenuUart.Top = MENU_TOP;
        g_tMenuUart.Height = MENU_HEIGHT;
        g_tMenuUart.Width = MENU_WIDTH;
        g_tMenuUart.LineCap = MENU_CAP;
        g_tMenuUart.ViewLine = 8;
        g_tMenuUart.Font.FontCode = FC_ST_24;
        g_tMenuUart.Font.Space = 0;
        g_tMenuUart.RollBackEn = 1;  /* 允许回滚 */   
        g_tMenuUart.GBK = 0; 
        g_tMenuUart.ActiveBackColor = 0;   /* 选中行背景色ID */      
        LCD_InitMenu(&g_tMenuUart, (char **)g_MenuUart_Text); /* 初始化菜单结构 */
    }
    LCD_DispMenu(&g_tMenuUart);
    
    DispHeader("请选择");
    
    LCD_DispMenu(&g_tMenuUart);

    while (fQuit == 0)
    {
        bsp_Idle();
        
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:          /* S键 上 */
                    LCD_MoveUpMenu(&g_tMenuUart);
                    break;

                case KEY_LONG_DOWN_S:   /* S键长按 */
                    fQuit = 1;
                    break;

                case KEY_UP_C:              /* C键 下 */
                    LCD_MoveDownMenu(&g_tMenuUart);
                    break;

                case KEY_LONG_DOWN_C:       /* C键长按 */
                    fQuit = 1;
                    break;

            default:
                break;
            }
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
