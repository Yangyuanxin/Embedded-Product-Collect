/*
*********************************************************************************************************
*
*    模块名称 : 电阻表
*    文件名称 : status_resitor_meter.c
*    版    本 : V1.0
*    说    明 : 电阻表
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2018-12-06 armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

static void DispResistor(void);

/*
*********************************************************************************************************
*    函 数 名: status_ResistorMeter
*    功能说明: 电阻表状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_ResistorMeter(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    uint8_t fSound = 0;

    DispHeader("电阻、二极管测量");
    DispHelpBar("电阻小于20欧姆时蜂鸣",
                "");      

    fRefresh = 1;
    bsp_StartAutoTimer(0, 300);
    while (g_MainStatus == MS_RESISTOR_METER)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;
            DispResistor();
        }

        bsp_Idle();
        
        /* 短路蜂鸣 20欧 */
        if (g_tVar.NTCRes < (float)0.02)
        {
            BEEP_Start(10, 10, 0);
            fSound = 1;
        }
        else
        {
            if (fSound == 1)
            {
                fSound = 0;
                
                BEEP_Stop();
            }
        }

        if (bsp_CheckTimer(0))
        {
            fRefresh = 1;
        }

        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:          /* S键释放 */
                    g_MainStatus = LastStatus(g_MainStatus);
                    break;

                case KEY_LONG_DOWN_S:   /* S键长按 */
                    break;

                case KEY_DOWN_C:        /* C键按下 */
                    break;

                case KEY_UP_C:          /* C键释放 */
                    g_MainStatus = NextStatus(g_MainStatus);
                    break;

                case KEY_LONG_DOWN_C:   /* C键长按 */
                    break;

                default:
                    break;
            }
        }
    }
    bsp_StopTimer(0);

    if (fSound == 1)
    {
        BEEP_Stop();
    }    
}

/*
*********************************************************************************************************
*    函 数 名: DispResistor
*    功能说明: 显示电阻值
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispResistor(void)
{
    char buf[32];
    float volt;
    float curr;

    if (g_tVar.NTCRes < 1.0f)
    {
        sprintf(buf, "  %0.1f", g_tVar.NTCRes * 1000);
    }
    else if (g_tVar.NTCRes < 1000)
    {
        sprintf(buf, "  %0.3fK", g_tVar.NTCRes);
    }
    else
    {
        sprintf(buf, "  > 1M");
    }
    DispMeasBar(0, "电 阻:", buf, "Ω");
  
    /* 大致计算，不是很精确 */
    volt = 2.5f * g_tVar.NTCRes / (g_tVar.NTCRes + 5.1f);
    curr = volt / g_tVar.NTCRes;
    if (volt > 2.4f)
    {
        sprintf(buf, "  > 2.4");
        DispMeasBar(1, "压 降:", buf, "V");

        sprintf(buf, "  %0.3f", curr);
        DispMeasBar(2, "电 流:", buf, "mA");
    }
    else
    {
        sprintf(buf, "  %0.3f", volt);
        DispMeasBar(1, "压 降:", buf, "V");
        
        sprintf(buf, "  %0.3f", curr);
        DispMeasBar(2, "电 流:", buf, "mA");     
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
