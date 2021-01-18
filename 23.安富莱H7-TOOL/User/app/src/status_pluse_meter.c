/*
*********************************************************************************************************
*
*    模块名称 : 频率计，脉冲计数
*    文件名称 : status_pulse_meter.c
*    版    本 : V1.0
*    说    明 : 电压表
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-10-06 armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

static void DispPulse(void);

/*
*********************************************************************************************************
*    函 数 名: status_VoltageMeter
*    功能说明: 电压表状态.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_PulseMeter(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    uint8_t ucIgnoreKey = 0;

    DispHeader("脉冲测量");
    DispHelpBar("D1输入信号 (0.1Hz-500KHz)",
            "长按S开始测量");
    
    fRefresh = 1;
    
    bsp_StartAutoTimer(0, 1000);
    while (g_MainStatus == MS_PULSE_METER)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            bsp_GetPulseParam();
            DispPulse();
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
                    if (ucIgnoreKey == 0)
                    {
                        g_MainStatus = LastStatus(g_MainStatus);
                    }        
                    ucIgnoreKey = 0;
                    break;

                case KEY_LONG_DOWN_S: /* S键长按 */
                    bsp_StartDetectPulse(); 
                    PlayKeyTone();
                    ucIgnoreKey = 1;
                    break;

                case KEY_DOWN_C: /* C键按下 */
                    break;

                case KEY_UP_C: /* C键释放 */
                    if (ucIgnoreKey == 0)
                    {
                        g_MainStatus = NextStatus(g_MainStatus);
                    }            
                    ucIgnoreKey = 0;
                    break;

                case KEY_LONG_DOWN_C: /* C键长按 */
                    bsp_StopDetectPulse(); 
                    PlayKeyTone();                
                    ucIgnoreKey = 1;
                    break;

                default:
                    break;
            }
        }
    }
    bsp_StopTimer(0);
    bsp_StopDetectPulse();     
}

/*
*********************************************************************************************************
*    函 数 名: DispPulse
*    功能说明: 显示脉冲参数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispPulse(void)
{
    char buf[32];
    
    sprintf(buf, "%7.3f", g_tPulse.Freq);
    DispMeasBar(0, "频  率:", buf, "Hz");
    
    sprintf(buf, "%7.3f", g_tPulse.Duty);
    DispMeasBar(1, "占空比:", buf, "%");

    sprintf(buf, "%7d", g_tPulse.Count);
    DispMeasBar(2, "个  数:", buf, "");        
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
