/*
*********************************************************************************************************
*
*    模块名称 : 循环执行模块
*    文件名称 : bsp_period_ctrl.c
*    版    本 : V1.0
*    说    明 : 周期性执行某个事物. 比如闪烁指示灯， 蜂鸣器鸣叫, 控制风扇占空比
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2015-12-27 armfly  正式发布
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

//PERIOD_CTRL_T g_tWiFiLed;    /* WiFi 指示灯 */
PERIOD_CTRL_T g_tRunLed; /* 运行 指示灯 */
static void PERIOD_ScanDev(PERIOD_CTRL_T *_ptPer);

void RunLedOn(void) { bsp_LedOn(1); }
void RunLedOff(void) { bsp_LedOff(1); }

/*
*********************************************************************************************************
*    函 数 名: PERIOD_Start
*    功能说明: 
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
void PERIOD_InitVar(void)
{
    g_tRunLed.ucEnalbe = 0;
    g_tRunLed.OnFunc = RunLedOn;
    g_tRunLed.OffFunc = RunLedOff;

    //    g_tWiFiLed.ucEnalbe = 0;
    //    g_tWiFiLed.OnFunc = bsp_WiFiLedOn;
    //    g_tWiFiLed.OffFunc = bsp_WiFiLedOff;
}

/*
*********************************************************************************************************
*    函 数 名: PERIOD_Scan
*    功能说明: 插入10ms中断执行
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void PERIOD_Scan(void)
{
    //    PERIOD_ScanDev(&g_tWiFiLed);
    PERIOD_ScanDev(&g_tRunLed);
}

/*
*********************************************************************************************************
*    函 数 名: PERIOD_Start
*    功能说明: 启动蜂鸣音。
*    形    参: _usOnTime : 蜂鸣时间，单位1ms; 0 表示不鸣叫
*              _usOffTime : 停止时间，单位1ms; 0 表示持续鸣叫
*             _usCycle : 鸣叫次数， 0 表示持续鸣叫
*    返 回 值: 无
*********************************************************************************************************
*/
void PERIOD_Start(PERIOD_CTRL_T *_ptPer, uint16_t _usOnTime, uint16_t _usOffTime, uint16_t _usCycle)
{
    if (_usOnTime == 0)
    {
        return;
    }

    _ptPer->usOnTime = (_usOnTime + 9) / 10;
    _ptPer->usOffTime = (_usOffTime + 9) / 10;
    _ptPer->usCycle = _usCycle;
    _ptPer->usCount = 0;
    _ptPer->usCycleCount = 0;
    _ptPer->ucState = 0;
    _ptPer->ucEnalbe = 1; /* 设置完全局参数后再使能发声标志 */

    _ptPer->OnFunc(); /* 执行ON时的函数 */
}

/*
*********************************************************************************************************
*    函 数 名: PERIOD_Stop
*    功能说明: 停止蜂鸣音。
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
void PERIOD_Stop(PERIOD_CTRL_T *_ptPer)
{
    _ptPer->ucEnalbe = 0;

    /* 必须在清控制标志后再停止发声，避免停止后在中断中又开启 */
    _ptPer->OffFunc(); /* 执行ON时的函数 */
    ;
}

/*
*********************************************************************************************************
*    函 数 名: PERIOD_ScanDev
*    功能说明: 每隔10ms调用1次该函数，用于控制需要定时执行的事件。该函数在 bsp_timer.c 中被调用。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void PERIOD_ScanDev(PERIOD_CTRL_T *_ptPer)
{
    if ((_ptPer->ucEnalbe == 0) || (_ptPer->usOffTime == 0))
    {
        return;
    }

    if (_ptPer->ucState == 0)
    {
        if (_ptPer->usOffTime > 0) /* 间断 */
        {
            if (++_ptPer->usCount >= _ptPer->usOnTime)
            {
                _ptPer->OffFunc(); /* 关闭动作 */
                _ptPer->usCount = 0;
                _ptPer->ucState = 1;
            }
        }
        else
        {
            ; /* 不做任何处理，连续发声 */
        }
    }
    else if (_ptPer->ucState == 1)
    {
        if (++_ptPer->usCount >= _ptPer->usOffTime)
        {
            /* 连续发声时，直到调用stop停止为止 */
            if (_ptPer->usCycle > 0)
            {
                if (++_ptPer->usCycleCount >= _ptPer->usCycle)
                {
                    /* 循环次数到，停止发声 */
                    _ptPer->ucEnalbe = 0;
                }

                if (_ptPer->ucEnalbe == 0)
                {
                    _ptPer->usOffTime = 0;
                    return;
                }
            }

            _ptPer->usCount = 0;
            _ptPer->ucState = 0;

            _ptPer->OnFunc(); /* 周期性执行On函数 */
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
