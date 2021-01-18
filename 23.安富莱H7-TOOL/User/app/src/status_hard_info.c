/*
*********************************************************************************************************
*
*    模块名称 : 显示硬件信息状态
*    文件名称 : status_hard_info.c
*    版    本 : V1.0
*    说    明 : 显示CPU型号,外围硬件信息,版本号等
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
*    函 数 名: status_HardInfo
*    功能说明: 硬件信息测试
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr, uint32_t *pRegVal);
void status_HardInfo(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    char buf[128];
    uint8_t line = 0;

    DispHeader2(91, "硬件信息");

    /* 检测CPU ID */
    {
        uint32_t id[3];

        bsp_GetCpuID(id);

        sprintf(buf, "STM32H750IBK6  %dMHz", SystemCoreClock / 1000000);
        DispInfoBar16(line++, "CPU:", buf);

        sprintf(buf, "%08X %08X %08X", id[0], id[1], id[2]);
        DispInfoBar16(line++, "  ", buf);
    }

    /* 显示TFT控制器型号和屏幕分辨率 */
    {
        sprintf(buf, "ST7789 / 240x240");
        DispInfoBar16(line++, "LCD:", buf);
    }

    /* 测试eMMC */
    {
        BSP_MMC_CardInfo CardInfo;

        BSP_MMC_Init();
        BSP_MMC_GetCardInfo(&CardInfo);

        /* CardInfo.LogBlockSize = 512 */
        if (CardInfo.LogBlockSize == 512)
        {
            uint32_t kb;

            kb = CardInfo.LogBlockNbr / 2;
            sprintf(buf, "%d.%02dGB Ok", kb / (1024 * 1024), (kb % (1024 * 1024)) / 10000);
            DispInfoBar16(line++, "eMMC:", buf);
        }
        else
        {
            sprintf(buf, "Error, BlockSize %d", CardInfo.LogBlockSize);
            DispInfoBar16Ex(line++, "eMMC:", buf, CL_RED);
        }
    }

    /* 测试I2C设备 */
    {
        if (i2c_CheckDevice(EE_DEV_ADDR) == 0)
        {
            sprintf(buf, "24C16 Ok (0x%02X)", EE_DEV_ADDR);
            DispInfoBar16(line++, "EEPROM:", buf);
        }
        else
        {
            sprintf(buf, "24C16 Err (0x%02X)", EE_DEV_ADDR);
            DispInfoBar16Ex(line++, "EEPROM:", buf, CL_RED);
        }

        if (i2c_CheckDevice(MCP4018_SLAVE_ADDRESS) == 0)
        {
            sprintf(buf, "MCP4018 Ok (0x%02X)", MCP4018_SLAVE_ADDRESS);
            DispInfoBar16(line++, "POT:", buf);
        }
        else
        {
            sprintf(buf, "MCP4018 Err (0x%02X)", MCP4018_SLAVE_ADDRESS);
            DispInfoBar16Ex(line++, "POT:", buf, CL_RED);
        }
    }

    /* QSPI检测 */
    {
        uint32_t id;
        char name[32];

        bsp_InitQSPI_W25Q256();

        id = QSPI_ReadID();

        if (id == W25Q64_ID)
        {
            strcpy(name, "W25Q64");
        }
        else if (id == W25Q128_ID)
        {
            strcpy(name, "W25Q128");
        }
        else if (id == W25Q256_ID)
        {
            strcpy(name, "W25Q256");
        }
        else
        {
            strcpy(name, "UNKNOW");
        }

        /* 检测串行Flash OK */
        if (id == W25Q256_ID)
        {
            sprintf(buf, "W25Q256 Ok, %08X", id);
            DispInfoBar16(line++, "QSPI:", buf);
        }
        else
        {
            sprintf(buf, "QSPI : W25Q256 Err, %08X", id);
            DispInfoBar16Ex(line++, "QSPI:", buf, CL_RED);
        }
    }

    /* 以太网MAC */
    {
        uint32_t PhyID[2];

        /*
            PhyID[0].15:0  = OUI:3:18
            PhyID[1].15:10 = OUI.24:19
       + 
       、【开模 
            PhyID[1].9:4 = Model Number
            PhyID[1].3:0 = Revision Number
        */
        ETH_PHY_IO_ReadReg(0, 2, &PhyID[0]);
        ETH_PHY_IO_ReadReg(0, 3, &PhyID[1]);

        if (((PhyID[1] >> 4) & 0x2F) == 0x0F)
        {
            sprintf(buf, "LAN8720 Ok");
            DispInfoBar16(line++, "Eth Phy:", buf);
        }
        else
        {
            sprintf(buf, "LAN8720 Error");
            DispInfoBar16Ex(line++, "Eth Phy:", buf, CL_RED);
        }
    }

    /* WiFi MAC */
    {
        //uint16_t mac[6];

        //ESP32_GetMac(&mac);
    }
    
    /* 固件版本 */
    {
        sprintf(buf, "%d.%02X", BOOT_VERSION >> 8, BOOT_VERSION & 0xFF);
        DispInfoBar16(line++, "Boot Ver:", buf);  

        sprintf(buf, "%d.%02X", APP_VERSION >> 8, APP_VERSION & 0xFF);
        DispInfoBar16(line++, "App Ver:", buf);  
    }

    bsp_StartAutoTimer(0, 1000);
    while (g_MainStatus == MS_HARD_INFO)
    {
        bsp_Idle();

        /* 显示时钟 */
        if (bsp_CheckTimer(0))
        {
            FONT_T tFont;
            uint16_t x, y;

            tFont.FontCode = FC_ST_16;              /* 字体代码 16点阵 */
            tFont.FrontColor = CL_WHITE;            /* 字体颜色 */
            tFont.BackColor = HEAD_BACK_COLOR;      /* 文字背景颜色 */
            tFont.Space = 0;                        /* 文字间距，单位 = 像素 */

            RTC_ReadClock();    /* 读时钟，结果在 g_tRTC */

            x = 5;
            y = LCD_GetHeight() - 20;

            sprintf(buf, "%4d-%02d-%02d %02d:%02d:%02d",
                            g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day, g_tRTC.Hour, g_tRTC.Min, g_tRTC.Sec);
            LCD_DispStr(x, y, buf, &tFont);
        }

        ucKeyCode = bsp_GetKey();   /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_C:      /* C键 下 */
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
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
