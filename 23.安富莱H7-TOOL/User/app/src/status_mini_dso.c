/*
*********************************************************************************************************
*
*    模块名称 : 迷你示波器
*    文件名称 : status_mini_dso.c
*    版    本 : V1.0
*    说    明 : 实现迷你示波器
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-12-14  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

/*
*********************************************************************************************************
*    函 数 名: status_MiniDSO
*    功能说明: 迷你示波器
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_MiniDSO(void)
{
    uint8_t ucKeyCode;  /* 按键代码 */
    uint8_t fRefresh;
    
    DispHeader("迷你示波器");
    DispHelpBar("迷你示波器功能预留",
                "还未实现"); 
    
    fRefresh = 1;
    while (g_MainStatus == MS_MINI_DSO)
    {
        if (fRefresh)   /* 刷新整个界面 */
        {
            fRefresh = 0;
        }

        bsp_Idle();
        
        ucKeyCode = bsp_GetKey();   /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_DOWN_S:        /* S键按下 */
                    break;

                case KEY_UP_S:          /* S键释放 */
                    g_MainStatus = LastStatus(g_MainStatus);
                    break;

                case KEY_LONG_DOWN_S:    /* S键长按 */
                    ;
                    break;

                case KEY_DOWN_C:        /* C键按下 */
                    break;

                case KEY_UP_C:          /* C键释放 */
                    g_MainStatus = NextStatus(g_MainStatus);
                    break;

                case KEY_LONG_DOWN_C:   /* C键长按 - 快捷键 */
                    ;
                    break;

                default:
                    break;
            }
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
