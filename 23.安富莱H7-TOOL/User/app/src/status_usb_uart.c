/*
*********************************************************************************************************
*
*    模块名称 : 联机界面
*    文件名称 : status_link_mode.c
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
#include "status_usb_uart.h"

USB_UART_T g_tUasUart;

static void DispUsbUartStatus(void);

/*
*********************************************************************************************************
*    函 数 名: status_UsbUart
*    功能说明: USB虚拟串口. 
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_UsbUart(void)
{
    uint8_t ucKeyCode;          /* 按键代码 */
    uint8_t fRefresh;
    
    DispHeader2(93, "USB 虚拟串口");
    
    usbd_CloseCDC();
    usbd_OpenCDC(COM1); /* 启用USB虚拟串口1， 用于虚拟串口，RS232 RS485 TTL-UART */

    g_tUasUart.Baud = 0;
    g_tUasUart.DataBit = 8;
    g_tUasUart.Parity = 0;
    g_tUasUart.Connected = 0; 
    g_tUasUart.PcTxCount = 0; 
    g_tUasUart.DevTxCount = 0; 
    g_tUasUart.Changed = 0;
    
    fRefresh = 1;
    while (g_MainStatus == MS_USB_UART)
    {
        bsp_Idle();

        if (fRefresh == 1)
        {
            fRefresh = 0;
            
            DispUsbUartStatus();
        }
        
        if (g_tUasUart.Changed == 1)
        {
            g_tUasUart.Changed = 0;
            fRefresh = 1;
        }
        
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:      /* S键 弹起 */
                    break;

                case KEY_UP_C:      /* C键 下 */
                    break;

                case KEY_LONG_DOWN_S:    /* S键 上 */
                    break;

                case KEY_LONG_DOWN_C:    /* C键长按 */
                    g_MainStatus = MS_EXTEND_MENU1;
                    break;

                default:
                    break;
            }
        }
    }

    usbd_CloseCDC();
    usbd_OpenCDC(COM_USB1); /* 启用USB虚拟串口8， 用于和PC软件USB通信 */
}

/*
*********************************************************************************************************
*    函 数 名: DispUsbUartStatus
*    功能说明: 显示USB虚拟串口连接状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispUsbUartStatus(void)
{
    FONT_T tFont;
    char buf[48];
    uint16_t x,y,h,w;
    const char *ParityName[3] = {"无校验", "奇校验", "偶校验"};

    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_16;              /* 字体代码 16点阵 */
        tFont.FrontColor = RGB(100,100,100);    /* 字体颜色 */
        tFont.BackColor = CL_MASK;              /* 文字背景颜色 */
        tFont.Space = 0;                        /* 文字间距，单位 = 像素 */
    }       
    
    x = 5;
    y = 35;
    h = 20;
    w = 230;    
    if (g_tUasUart.Connected == 0)
    {
        // 波特率 115200，无校验
        sprintf(buf, "------未连接------");
        DispLabel(x, y, h, w, RGB(168,255,168), buf, &tFont);
        y += 20;
        sprintf(buf, "------------------");
        DispLabel(x, y, h, w, RGB(168,255,168), buf, &tFont); 
    }
    else
    {
        // 波特率 115200，无校验
        sprintf(buf, "已连接 波特率:%d %s", g_tUasUart.Baud, ParityName[g_tUasUart.Parity]);
        DispLabel(x, y, h, w, RGB(168,255,168), buf, &tFont);
        y += 25;
        sprintf(buf, "PC:%10u DEV:%10u", g_tUasUart.PcTxCount, g_tUasUart.DevTxCount);
        DispLabel(x, y, h, w, RGB(168,255,168), buf, &tFont);         
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
