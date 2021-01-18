/*
*********************************************************************************************************
*
*    模块名称 : CAN数据监视器
*    文件名称 : status_can_monitor.c
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
#define MEMO_X     0
#define MEMO_Y     20 
#define MEMO_H     (240 - MEMO_Y - 0)  
#define MEMO_W     (240 - 2 * MEMO_X)

extern char g_OutText[4 * 1024];
    
extern MEMO_T g_RecMemo;

static uint8_t s_FrameHead1 = 1;

static uint64_t s_FrameHeadTime1;

static uint64_t s_BeginTime;

static uint32_t s_RxCount1;

static void DispRxCounter(uint32_t _count1, uint32_t _count2);

/*
*********************************************************************************************************
*    函 数 名: CanReciveNew
*    功能说明: CAN接收到新的字节的回调函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void CanReciveNew(uint8_t _byte)
{
    char buf[32];
    
    if (s_FrameHead1 == 1)
    {
        s_FrameHead1 = 2;
        
        /* 记录下准确的首字符时刻 us */
        s_FrameHeadTime1 = bsp_GetRunTimeUs() - s_BeginTime;  
    
        {
            if (g_tParam.UartMonTimeStamp == 1)     /* 加上时间戳 */
            {
                
                
                if (g_tParam.UartMonHex == 0)   /* ASCII格式显示 */
                {
                    uint8_t LastChar;
                    
                    if (g_RecMemo.Len > 0)
                    {
                        LastChar = g_RecMemo.Text[g_RecMemo.Len - 1];
                        if (LastChar == 0x0D || LastChar == 0x0A)
                        {
                            sprintf(buf, "[%d.%03d] ", (int32_t)s_FrameHeadTime1 / 1000, (int32_t)s_FrameHeadTime1 % 1000);
                        }
                        else
                        {
                            sprintf(buf, "\r\n[%d.%03d] ", (int32_t)s_FrameHeadTime1 / 1000, (int32_t)s_FrameHeadTime1 % 1000);
                        }
                    }
                    else
                    {
                        sprintf(buf, "\r\n[%d.%03d] ", (int32_t)s_FrameHeadTime1 / 1000, (int32_t)s_FrameHeadTime1 % 1000);
                    }
                    LCD_MemoAddStr(&g_RecMemo, buf);
                }
                else        /* HEX */
                {
                    sprintf(buf, "\r\n[%d.%03d] ", (int32_t)s_FrameHeadTime1 / 1000, (int32_t)s_FrameHeadTime1 % 1000);
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

    s_RxCount1++;     
}

/*
*********************************************************************************************************
*    函 数 名: status_MonitorCan
*    功能说明: 串口监视器
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_MonitorCan(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    uint8_t fInit;
    FONT_T tFontMemo;
    
    {
        tFontMemo.FontCode = FC_ST_16;          /* 字体代码 16点阵 */
        tFontMemo.FrontColor = CL_WHITE;        /* 字体颜色 */
        tFontMemo.BackColor = CL_MASK;          /* 文字背景颜色 */
        tFontMemo.Space = 0;                    /* 文字间距，单位 = 像素 */    
    }

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
    
    UartMonInitParam(MODIFY_PARAM_UART_MON);  
    UartMonCheckParam();    /* 检查参数 */
        
    fInit = 1;
    fRefresh = 1;
    while (g_MainStatus == MS_MONITOR_CAN)
    {
        bsp_Idle();
        
        if (fInit == 1)
        {
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
            
            
            LCD_MemoAddStr(&g_RecMemo, "CAN 功能预留"); 
            
            s_RxCount1 = 0;
        }
        
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            DispRxCounter(s_RxCount1, 0);
            
            LCD_DrawMemo(&g_RecMemo);
        }             
 

        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:          /* S键 上 */
                    LCD_MemoPageUp(&g_RecMemo, 1);
                    fRefresh = 1;
                    break;

                case KEY_LONG_DOWN_S:   /* S键 长按 */                    
                    LCD_SetEncode(ENCODE_UTF8);
                    ModifyParam(MODIFY_PARAM_UART_MON);       /* 参数修改界面，阻塞 */
                    LCD_SetEncode(ENCODE_GBK);
                    fRefresh = 1;
                    fInit = 1;
                    break;

                case KEY_UP_C:              /* C键 下 */
                    LCD_MemoPageDown(&g_RecMemo, 1);
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
}

/*
*********************************************************************************************************
*    函 数 名: DispRxCounter
*    功能说明: 显示接受到的总字符数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
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
    
    sprintf(buf, "%8d %8d", _count1, _count2);
    LCD_DispStr(x, y, buf, &tFont);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
