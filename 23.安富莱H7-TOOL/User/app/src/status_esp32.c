/*
*********************************************************************************************************
*
*    模块名称 : ESP32 WIFI模块固件升级状态
*    文件名称 : status_system_set.c
*    版    本 : V1.0
*    说    明 : 实现USB虚拟串口，PC串口可以直连到ESP32 WiFi模块，用于固件升级
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2018-12-06  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

/*
*********************************************************************************************************
*    函 数 名: status_ESP32Test
*    功能说明: esp32测试
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_ESP32Test(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    uint8_t isp_flag = 0;

    DispHeader2(92, "ESP32固件升级");
    DispHelpBar("S键切换模式",
                "");
    
    usbd_CloseCDC();
    usbd_OpenCDC(COM4);

    //bsp_InitESP32();

    ESP32_EnterISP();
    isp_flag = 1;

    wifi_state = WIFI_STOP;

    fRefresh = 1;
    while (g_MainStatus == MS_ESP32_TEST)
    {
        bsp_Idle();

        if (fRefresh == 1) /* 刷新整个界面 */
        {
            fRefresh = 0;

            if (isp_flag == 0)
            {
                DispInfoBar16(1, "当前模式:", "AT指令 ");  
            }
            else
            {
                DispInfoBar16(1, "当前模式:", "ISP升级");  
            }
        }

        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S: /* S键 弹起 */
                    PlayKeyTone();

                    if (isp_flag == 0)
                    {
                        isp_flag = 1;

                        ESP32_EnterISP();
                    }
                    else
                    {
                        isp_flag = 0;

                        ESP32_EnterAT();
                    }
                    fRefresh = 1;
                    break;

                case KEY_UP_C: /* C键 下 */
                    fRefresh = 1;
                    break;

                case KEY_LONG_DOWN_S: /* S键 上 */
                    break;

                case KEY_LONG_DOWN_C: /* C键长按 */
                    PlayKeyTone();
                    g_MainStatus = MS_SYSTEM_SET;
                    break;

                default:
                    break;
            }
        }
    }
    usbd_CloseCDC();
    usbd_OpenCDC(COM_USB_PC); /* 启用USB虚拟串口 */
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
