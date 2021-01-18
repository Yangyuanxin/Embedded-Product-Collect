/*
*********************************************************************************************************
*
*   模块名称 : 高侧电压电流表
*   文件名称 : status_current_meter.c
*   版    本 : V1.1
*   说    明 : 
*   修改记录 :
*       版本号  日期        作者     说明
*       V1.1    2019-10-19  armfly  正式发布
*       V1.2    2019-11-03  armfly  完善功能。增加电池容量计算。
*
*   Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

static void DispCurrentVolt(void);
static void AutoCurrentRange(void);

/*
*********************************************************************************************************
*   函 数 名: status_CurrentMeter
*   功能说明: 高侧电压电流表状态.
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void status_CurrentMeter(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;

    DispHeader("高侧电流表");
    DispHelpBar("长按S开始测量电池放电容量",
                "长按C停止测量");  

    fRefresh = 1;

    bsp_StartAutoTimer(0, 300);     
    bsp_StartAutoTimer(1, 1000);
    while (g_MainStatus == MS_CURRENT_METER)
    {
        if (fRefresh)       /* 刷新整个界面 */
        {
            fRefresh = 0;

            DispCurrentVolt();
            AutoCurrentRange();
        }

        bsp_Idle();

        if (bsp_CheckTimer(0))
        {
            fRefresh = 1;
        }
        
        /* 每秒统计一次 */
        if (bsp_CheckTimer(1))
        {
            if (g_tVar.StartBatCap == 1)
            {
                /* 1mAh = 0.001安培*3600秒 = 3.6安培秒 = 3.6库仑 */                
                g_tVar.BatteryCapacity += g_tVar.HighSideCurr / 3600;
            }
        }        

        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_DOWN_S:    /* S键按下 */
                    break;

                case KEY_UP_S:      /* S键释放 */
                    if (g_tVar.StartBatCap == 0)
                    {
                        g_MainStatus = LastStatus(g_MainStatus);
                    }
                    
                    break;

                case KEY_LONG_DOWN_S:    /* S键长按 */
                    g_tVar.StartBatCap = 1;
                    g_tVar.BatteryCapacity = 0;
                    PlayKeyTone();
                    break;

                case KEY_DOWN_C:    /* C键按下 */
                    break;

                case KEY_UP_C:      /* C键释放 */
                    if (g_tVar.StartBatCap == 0)
                    {                
                        g_MainStatus = NextStatus(g_MainStatus);
                    }
                    break;

                case KEY_LONG_DOWN_C:    /* C键长按 */
                    g_tVar.StartBatCap = 0;
                    PlayKeyTone();
                    break;

                default:
                    break;
            }
        }
    }
    bsp_StopTimer(0);
    bsp_StopTimer(1);    
}

/*
*********************************************************************************************************
*   函 数 名: AutoCurrentRange
*   功能说明: 自动调节量程. 
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
static void AutoCurrentRange(void)
{	
    if (g_tVar.HighSideCurr < 100)
    {
        WriteRegValue_06H(0x0211, 0);   /* 0表示120mA, 1表示1.2A量程 */
    }
    else if (g_tVar.HighSideCurr > 105)
    {
        WriteRegValue_06H(0x0211, 1);   /* 0表示120mA, 1表示1.2A量程 */
    }
}
        
/*
*********************************************************************************************************
*   函 数 名: DispCurrentVolt
*   功能说明: 显示电压电流功率
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
static void DispCurrentVolt(void)
{
    char buf[32];

    sprintf(buf, "%8.3f", g_tVar.HighSideVolt);
    DispMeasBar(0, "电 压:", buf, "V");
    
    sprintf(buf, "%8.3f", g_tVar.HighSideCurr);
    DispMeasBar(1, "电 流:", buf, "mA");

    sprintf(buf, "%8.3f", g_tVar.HighSideVolt * g_tVar.HighSideCurr / 1000);
    DispMeasBar(2, "功 率:", buf, "W");    
    
    sprintf(buf, "%8.3f", g_tVar.BatteryCapacity);
    DispMeasBar(3, "容 量:", buf, "mAh");       
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
