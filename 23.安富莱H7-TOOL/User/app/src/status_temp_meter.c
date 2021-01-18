/*
*********************************************************************************************************
*
*    模块名称 : 温度表
*    文件名称 : status_temp_meter.c
*    版    本 : V1.0
*    说    明 : 温度表
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-10-06 armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

static void DispTemp(void);

/*
*********************************************************************************************************
*    函 数 名: status_TempMeter
*    功能说明: 温度表状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_TempMeter(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;

    DispHeader("温度测量");
    DispHelpBar("温度传感器类型",
                "NTC 10K B3950");     

    fRefresh = 1;
    bsp_StartAutoTimer(0, 200);
    while (g_MainStatus == MS_TEMP_METER)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;
            DispTemp();
        }
        
        bsp_Idle();
        
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
                case KEY_DOWN_S: /* S键按下 */
                    break;

                case KEY_UP_S: /* S键释放 */
                    g_MainStatus = LastStatus(g_MainStatus);
                    break;

                case KEY_LONG_DOWN_S: /* S键长按 */
                    break;

                case KEY_DOWN_C: /* C键按下 */
                    break;

                case KEY_UP_C: /* C键释放 */
                    g_MainStatus = NextStatus(g_MainStatus);
                    break;

                case KEY_LONG_DOWN_C: /* C键长按 */
                    break;

                default:
                    break;
            }
        }
    }

    bsp_StopTimer(0);
}

/*
*********************************************************************************************************
*    函 数 名: DispTemp
*    功能说明: 显示电阻和温度值
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispTemp(void)
{
    char buf[64];

    if (g_tVar.NTCRes > 1000)
    {
        sprintf(buf, "   ----");
        DispMeasBar(0, "电 阻:", buf, "KΩ");

        sprintf(buf, "   ----");
        DispMeasBar(1, "温 度:", buf, "℃");
    }
    else
    {
        sprintf(buf, "%0.3f", g_tVar.NTCRes);
        DispMeasBar(0, "电 阻:", buf, "KΩ");

        sprintf(buf,  "%0.2f", g_tVar.NTCTemp);
        DispMeasBar(1, "温 度:", buf, "℃");
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
