/*
*********************************************************************************************************
*
*    模块名称 : 系统设置主程序
*    文件名称 : status_system_set.c
*    版    本 : V1.0
*    说    明 : 提供一个菜单选择子功能.
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2018-12-06  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

const uint8_t *g_MenuSys_Text[] =
{
    " 1 基本参数",
    " 2 网络参数",
    " 3 ESP32固件升级",
    " 4 USB eMMC磁盘",
    " 5 数据维护",
    " 6 硬件信息",    
    " 7 重启",
    /* 结束符号, 用于菜单函数自动识别菜单项个数 */
    "&"
};

MENU_T g_tMenuSys;

/*
*********************************************************************************************************
*    函 数 名: status_SystemSetMain
*    功能说明: 系统设置状态. 菜单选择
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_SystemSetMain(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    static uint8_t s_enter_sub_menu = 0;
    uint8_t ResetReq = 0;

    DispHeader2(90, "系统设置");

    if (s_enter_sub_menu == 0)
    {
        g_tMenuSys.Left = MENU_LEFT;
        g_tMenuSys.Top = MENU_TOP;
        g_tMenuSys.Height = MENU_HEIGHT;
        g_tMenuSys.Width = MENU_WIDTH;
        g_tMenuSys.LineCap = MENU_CAP;
        g_tMenuSys.ViewLine = 8;
        g_tMenuSys.Font.FontCode = FC_ST_24;
//        g_tMenuSys.Font.FrontColor = CL_BLACK;        /* 字体颜色 */
//        g_tMenuSys.Font.BackColor = FORM_BACK_COLOR;    /* 文字背景颜色 */
        g_tMenuSys.Font.Space = 0;
        g_tMenuSys.RollBackEn = 1;  /* 允许回滚 */        
        g_tMenuSys.GBK = 0;
        g_tMenuSys.ActiveBackColor = 0;   /* 选中行背景色ID */
        LCD_InitMenu(&g_tMenuSys, (char **)g_MenuSys_Text); /* 初始化菜单结构 */
    }
    LCD_DispMenu(&g_tMenuSys);

    fRefresh = 1;
    while (g_MainStatus == MS_SYSTEM_SET)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            if (g_tMenuSys.Cursor == 0)
            {
                ;
            }
        }
        
        bsp_Idle();
        
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S: /* S键 上 */                   
                    LCD_MoveUpMenu(&g_tMenuSys);
                    break;

                case KEY_LONG_DOWN_S: /* S键 上 */
                    PlayKeyTone();
                    s_enter_sub_menu = 1;
                
                    if (g_tMenuSys.Cursor == 0)
                    {
                        ModifyParam(MODIFY_PARAM_SYSTEM);
                        DispHeader2(90, "系统设置");    /* 需要清屏 */
                        LCD_DispMenu(&g_tMenuSys);
                    }
                    else if (g_tMenuSys.Cursor == 1)
                    {
                        ModifyParam(MODIFY_PARAM_NET);
                        DispHeader2(90, "系统设置");    /* 需要清屏 */
                        LCD_DispMenu(&g_tMenuSys);
                    }
                    else if (g_tMenuSys.Cursor == 2)
                    {
                        g_MainStatus = MS_ESP32_TEST;
                    }
                    else if (g_tMenuSys.Cursor == 3)
                    {
                        g_MainStatus = MS_USB_EMMC;
                    }    
                    else if (g_tMenuSys.Cursor == 4)
                    {
                        g_MainStatus = MS_FILE_MANAGE;
                    }
                    else if (g_tMenuSys.Cursor == 5)
                    {
                        g_MainStatus = MS_HARD_INFO;
                    }                    
                    else if (g_tMenuSys.Cursor == 6)    /* 重启 */
                    {
                        ResetReq = 1;   
                    }                     
                    break;

                case KEY_LONG_UP_S:     /* 长按弹起 */
                    if (ResetReq == 1)
                    {
                        /* 复位进入APP */
                        *(uint32_t *)0x20000000 = 0;
                        NVIC_SystemReset(); /* 复位CPU */
                    }                     
                    break;
                    
                case KEY_UP_C: /* C键 下 */
                    LCD_MoveDownMenu(&g_tMenuSys);
                    break;

                case KEY_LONG_DOWN_C: /* C键长按 */
                    PlayKeyTone();
                    s_enter_sub_menu = 0;
                                
                    g_MainStatus = MS_EXTEND_MENU1;
                    break;

                default:
                    break;
            }
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
