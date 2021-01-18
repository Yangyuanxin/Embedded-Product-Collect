/*
*********************************************************************************************************
*
*    模块名称 : USB虚拟磁盘状态
*    文件名称 : status_usb_emmc.c
*    版    本 : V1.0
*    说    明 : 关闭USB串口，进入USB磁盘模式.
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
*    函 数 名: status_UsbEMMC
*    功能说明: USB虚拟磁盘，eMMC磁盘
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_UsbEMMC(void)
{
    uint8_t ucKeyCode;          /* 按键代码 */
    
    DispHeader2(93, "USB eMMC磁盘");
    DispHelpBar("请在电脑操作eMMC文件",
                "");
    
    usbd_CloseCDC();    
    usbd_OpenMassStorage();

    DispInfoBar16(1, "USB模式:", "虚拟磁盘");      

    while (g_MainStatus == MS_USB_EMMC)
    {
        bsp_Idle();

        if (g_tVar.UsbEmmcRemoved == 1)
        {
            g_tVar.UsbEmmcRemoved = 0;
            
            g_MainStatus = MS_LINK_MODE;
        }
        
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:      /* S键 弹起 */
                    break;

                case KEY_UP_C:      /* C键 下 */
                    break;

                case KEY_LONG_DOWN_S:    /* S键 上 */
                    break;

                case KEY_LONG_DOWN_C:    /* C键长按 */
                    PlayKeyTone();
                    g_MainStatus = MS_SYSTEM_SET;
                    break;

                default:
                    break;
            }
        }
    }

	usbd_CloseMassStorage();

    usbd_OpenCDC2(COM_USB_PC);   /* 映射到串口8. 和PC软件联机 */
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
