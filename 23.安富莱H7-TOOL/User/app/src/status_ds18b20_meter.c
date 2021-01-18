/*
*********************************************************************************************************
*
*    模块名称 : DS18B20温度传感器监视
*    文件名称 : status_ds18b20_meter.c
*    版    本 : V1.0
*    说    明 : 显示8路DS18B20温度值
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2020-11-29 armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

static void DispDS18B20(void);

static float TemperatureMin[8];
static float TemperatureMax[8];

/*
*********************************************************************************************************
*    函 数 名: status_DS18B20Meter
*    功能说明: DS18B20温度表状态.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_DS18B20Meter(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;

    DispHeader("DS18B20温度表");
//    DispHelpBar("D0-D7可接8个DS18B20",
//                "");      

    fRefresh = 1;

    {
        uint8_t i;
        
        for (i = 0; i < 8; i++)
        {
            TemperatureMin[i] = 125;
            TemperatureMax[i] = -60;
        }
    }    
    
    bsp_StartAutoTimer(0, 300);
    while (g_MainStatus == MS_DS18B20_METER)
    {
        if (fRefresh)       /* 刷新整个界面 */
        {
            fRefresh = 0;

            DispDS18B20();
        }

        bsp_Idle();
        
        if (bsp_CheckTimer(0))
        {
            fRefresh = 1;
        }

        ucKeyCode = bsp_GetKey();   /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_DOWN_S:        /* S键按下 */
                    break;

                case KEY_UP_S:          /* S键释放 */
                    ;
                    break;

                case KEY_LONG_DOWN_S:    /* S键长按 */
                    break;

                case KEY_DOWN_C:        /* C键按下 */
                    break;

                case KEY_UP_C:           /* C键释放 */
                    ;
                    break;

                case KEY_LONG_DOWN_C:    /* C键长按 */
                    PlayKeyTone();
                    g_MainStatus = MS_EXTEND_MENU_REC;
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
*    函 数 名: DispDS18B20
*    功能说明: 显示8通道DS18B20数据
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
#define LABLE_X     5
static void DispDS18B20(void)
{
    /*        
        BackColor = RGB(255, 236, 245);
        BackColor = RGB(150, 240, 40);
        BackColor = RGB(255, 255, 136);   
        BackColor = RGB(239, 223, 255);    
        BackColor = RGB(119, 253, 213); 
    */    
    FONT_T tFont16, tFont24;
    char buf[32];
    uint16_t x, y, h, w;
    uint8_t i;
    uint8_t SensorOk[8];
    float  Temperature[8];
    
    for (i = 0; i < 8; i++)
    {
        if (DS18B20_ReadTemp(i, &Temperature[i]) == 1)
        {
            SensorOk[i] = 1;
            
            /* DS18B20第1次读出数据可能是 85.0 */
            if ((uint16_t)(Temperature[i] * 10) != 850)
            {
                if (Temperature[i] < TemperatureMin[i])
                {
                    TemperatureMin[i] = Temperature[i];
                }

                if (Temperature[i] > TemperatureMax[i])
                {
                    TemperatureMax[i] = Temperature[i];
                }
            }
        }
        else
        {
            SensorOk[i] = 0;
        }
    }
    
    /* 设置字体参数 */
    {
        tFont16.FontCode = FC_ST_16;            /* 字体代码 16点阵 */
        tFont16.FrontColor = RGB(120,120,120);     /* 字体颜色 */
        tFont16.BackColor = CL_MASK;            /* 文字背景 - 透明 */
        tFont16.Space = 0;                      /* 文字间距，单位 = 像素 */

        tFont24.FontCode = FC_ST_24;            /* 字体代码 16点阵 */
        tFont24.FrontColor = RGB(50,50,50);     /* 字体颜色 */
        tFont24.BackColor = CL_MASK;            /* 文字背景 - 透明 */
        tFont24.Space = 0;                      /* 文字间距，单位 = 像素 */         
    }     
    
    x = LABLE_X; 
    y = 36;
    h = 24 + 1;
    w = 200;   
    for (i = 0; i < 8; i++)
    {
        /* 显示数字编号 */
        sprintf(buf,  "%d", i);
        w = 24;  
        DispLabelRound(x, y, h, w, RGB(150, 240, 40), buf,  &tFont24);
        
        /* 显示实时温度值 */
        if (SensorOk[i] == 0)
        {
            sprintf(buf,  "----");
        }
        else
        {
            /* -40.0℃ */
            sprintf(buf, "%0.1f℃", Temperature[i]);
        }
        w = 100;
        DispLabelRound(x + 24 + 5, y, h, w, RGB(255, 236, 245), buf,  &tFont24);
        
        /* 显示温度最大最小值 */
        if (SensorOk[i] == 0)
        {
            sprintf(buf,  "----");
        }
        else
        {
            /* -40.0℃ */
            sprintf(buf, "%0.1f, %0.1f", TemperatureMin[i],TemperatureMax[i]);
        }
        w = 100;
        DispLabelRound(x + 24 + 8 + 100, y, h, w, RGB(255, 255, 136), buf,  &tFont16);
        
        y += h;
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
