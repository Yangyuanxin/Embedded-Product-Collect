/*
*********************************************************************************************************
*
*   模块名称 : 微型数控电源
*   文件名称 : status_TVccPower.c
*   版    本 : V1.0
*   说    明 : 通过TVCC输出可调电压。电压范围1.2-5.0V，电流限制400mA
*   修改记录 :
*       版本号  日期        作者     说明
*       V1.0    2019-11-04  armfly  正式发布1
*
*   Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

static void DispTVccVoltCurr(void);
static void DispTVccSetting(uint16_t _volt, uint8_t _mode);

/*
*********************************************************************************************************
*   函 数 名: status_TVCCPower
*   功能说明: 微型电源状态。TVCC引脚输出电压可调，电流可以监视。400mA限流。
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
void status_TVCCPower(void)
{
    uint8_t ucKeyCode;      /* 按键代码 */
    uint8_t fRefresh;
    uint8_t ucAdjustMode = 0;
    uint16_t NowVolt;       /* 当前设置电压 mV */

    DispHeader("微型数控电源");
    DispHelpBar("TVCC输出1.2-5.0V,限流400mA",
                "长按S进入调节状态");  
    
    fRefresh = 1;
    
    NowVolt = 3300;
    DispTVccSetting(NowVolt, 0);
    bsp_StartAutoTimer(0, 300);     
    while (g_MainStatus == MS_TVCC_POWER)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            bsp_SetTVCC(NowVolt);
            DispTVccVoltCurr();
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
                case KEY_DOWN_S:    /* S键按下 */
                    break;

                case KEY_UP_S:      /* S键释放 */
                    if (ucAdjustMode == 0)      /* 测量模式 */
                    {
                        g_MainStatus = LastStatus(MS_TVCC_POWER);
                    }
                    else                        /* 设置电压的模式 */
                    {
                        if (NowVolt < 5000)
                        {
                            NowVolt += 100;
                            DispTVccSetting(NowVolt, 1);
                        }
                        else
                        {
                            if (g_tParam.KeyToneEnable != 0)
                            {
                                BEEP_Start(5, 5, 3);    /* 叫50ms，停50ms，循环3次 */
                            }
                        }
                    }
                    break;

                case KEY_LONG_DOWN_S:    /* S键长按 */
                    if(ucAdjustMode == 0)
                    {
                        ucAdjustMode = 1;
                        if (g_tParam.KeyToneEnable != 0)
                        {
                            PlayKeyTone();
                        }
                        DispTVccSetting(NowVolt, 1);  
                    }
                    break;

                case KEY_DOWN_C:    /* C键按下 */
                    break;

                case KEY_UP_C:      /* C键释放 */
                    if (ucAdjustMode == 0)
                    {      
                        g_MainStatus = NextStatus(MS_TVCC_POWER);       
                    }
                    else 
                    {
                        if (NowVolt > 1200)
                        {
                            NowVolt -= 100; 
                            DispTVccSetting(NowVolt, 1);                              
                        }
                        else
                        {
                            if (g_tParam.KeyToneEnable != 0)
                            {                            
                                BEEP_Start(5,5,3);      /* 叫50ms，停50ms，循环3次 */
                            }
                        }                       
                    }                
                    break;

                case KEY_LONG_DOWN_C:    /* C键长按 */
                    if(ucAdjustMode == 1)
                    {
                        ucAdjustMode = 0;
                        if (g_tParam.KeyToneEnable != 0)
                        {
                            PlayKeyTone();
                        }
                        DispTVccSetting(NowVolt, 0);   
                    }
                    break;

                default:
                    break;
            }
        }
    }
    bsp_StopTimer(0);    
    
    bsp_SetTVCC(3300);      /* 退出时还原为3.3V */
}
        
/*
*********************************************************************************************************
*   函 数 名: DispTVccVoltCurr
*   功能说明: 显示电压电流
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
static void DispTVccVoltCurr(void)
{
    char buf[32];

    sprintf(buf, "%8.3f", g_tVar.TVCCVolt );
    DispMeasBar(0, "电 压:", buf, "V");

    sprintf(buf, "%7.2f", g_tVar.TVCCCurr);
    DispMeasBar(1, "电 流:", buf, "mA");

    sprintf(buf, "%8.3f", g_tVar.TVCCVolt * g_tVar.TVCCCurr / 1000);
    DispMeasBar(2, "功 率:", buf, "W");   
}

/*
*********************************************************************************************************
*   函 数 名: DispTVccSetting
*   功能说明: 显示设置电压
*   形    参: _volt : mV (1200 - 5000)
*            _mode : 1表示设置模式  0 表示测量模式。 两个模式就是底色不同
*   返 回 值: 无
*********************************************************************************************************
*/
static void DispTVccSetting(uint16_t _volt, uint8_t _mode)
{
    char buf[64];
    uint16_t color;

    sprintf(buf, "   %d.%dV", _volt / 1000, (_volt % 1000) / 100);
    
    if (_mode == 1)
    {
        color = CL_YELLOW;
    }
    else
    {
        color = MEAS_BACK_COLOR;
    }
        
    DispMeasBarEx(3, "设 置:", buf, "", color); 
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
