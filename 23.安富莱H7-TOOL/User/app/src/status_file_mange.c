/*
*********************************************************************************************************
*
*    模块名称 : 数据维护状态
*    文件名称 : status_file_mange.c
*    版    本 : V1.0
*    说    明 : 写字库，升级boot
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2018-12-06  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

ALIGN_32BYTES(uint8_t qspi_file_buf[1024]);
ALIGN_32BYTES(uint8_t qspi_read_buf[1024]);

static void FM_WirteFont(void);
static void FM_WirteBoot(void);
static void FM_WirteQspiApp(void);

/*
*********************************************************************************************************
*    函 数 名: status_FileManage
*    功能说明: 文件管理，维护。写字库，刷boot。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
/* 按钮坐标定义 */
#define BTN1_X     5
#define BTN1_Y     50
#define BTN1_H     28 
#define BTN1_W     100 
#define BTN1_TEXT  "写QSPI字库"

#define BTN2_X     BTN1_X
#define BTN2_Y     BTN1_Y + BTN1_H + 5 
#define BTN2_H     BTN1_H 
#define BTN2_W     BTN1_W 
#define BTN2_TEXT  "刷新boot"

#define BTN3_X     BTN1_X
#define BTN3_Y     BTN1_Y + 2 * (BTN1_H + 5)
#define BTN3_H     BTN1_H 
#define BTN3_W     BTN1_W 
#define BTN3_TEXT  "写QSPI App"

#define BTN4_X     (5 + 120)
#define BTN4_Y     BTN1_Y
#define BTN4_H     BTN1_H 
#define BTN4_W     BTN1_W 
#define BTN4_TEXT  "----"

#define BTN5_X     BTN4_X
#define BTN5_Y     BTN1_Y + BTN1_H + 5 
#define BTN5_H     BTN1_H 
#define BTN5_W     BTN1_W 
#define BTN5_TEXT  "----"

#define BTN6_X     BTN4_X
#define BTN6_Y     BTN1_Y + 2 * (BTN1_H + 5)
#define BTN6_H     BTN1_H 
#define BTN6_W     BTN1_W 
#define BTN6_TEXT  "----"

#define BTN_NUM     6

void status_FileManage(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    FONT_T tFont16;
    uint8_t cursor = 0;
    BUTTON_T btn1, btn2, btn3, btn4, btn5, btn6;
    
    DispHeader2(94, "数据维护");
//    DispHelpBar("S、C键短按切换焦点",
//                "S键长按执行");
    
    /* 设置字体参数 */
    {
        tFont16.FontCode = FC_ST_16;          /* 字体代码 16点阵 */
        tFont16.FrontColor = INFO_NAME_COLOR; /* 字体颜色 */
        tFont16.BackColor = CL_MASK;          /* 文字背景颜色 */
        tFont16.Space = 0;                    /* 文字间距，单位 = 像素 */
    } 
    
    fRefresh = 1;
    while (g_MainStatus == MS_FILE_MANAGE)
    {
        /* PC机控制刷新boot */
        if (g_tVar.ReqWriteBoot == 1)
        {
            g_tVar.ReqWriteBoot = 0;
            
            cursor = 1;
            fRefresh = 1;                        
            
            //BEEP_Start(5, 5, 1);
            bsp_PutKey(KEY_DB_S);           /* 任意发一个本状态无用的按键消息，重开背光 */
            bsp_PutKey(KEY_LONG_DOWN_S);    /* 触发写boot */
        }        
        
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

			{
				btn1.Left = BTN1_X;
				btn1.Top = BTN1_Y;
				btn1.Height = BTN1_H;
				btn1.Width = BTN1_W;
				btn1.pCaption = BTN1_TEXT;
				btn1.Font =  &tFont16;
				btn1.Focus = 0;

				btn2.Left = BTN2_X;
				btn2.Top = BTN2_Y;
				btn2.Height = BTN2_H;
				btn2.Width = BTN2_W;
				btn2.pCaption = BTN2_TEXT;
				btn2.Font =  &tFont16;
				btn2.Focus = 0;

				btn3.Left = BTN3_X;
				btn3.Top = BTN3_Y;
				btn3.Height = BTN3_H;
				btn3.Width = BTN3_W;
				btn3.pCaption = BTN3_TEXT;
				btn3.Font =  &tFont16;
				btn3.Focus = 0;

				btn4.Left = BTN4_X;
				btn4.Top = BTN4_Y;
				btn4.Height = BTN4_H;
				btn4.Width = BTN4_W;
				btn4.pCaption = BTN4_TEXT;
				btn4.Font =  &tFont16;
				btn4.Focus = 0;
                
				btn5.Left = BTN5_X;
				btn5.Top = BTN5_Y;
				btn5.Height = BTN5_H;
				btn5.Width = BTN5_W;
				btn5.pCaption = BTN5_TEXT;
				btn5.Font =  &tFont16;
				btn5.Focus = 0;

				btn6.Left = BTN6_X;
				btn6.Top = BTN6_Y;
				btn6.Height = BTN6_H;
				btn6.Width = BTN6_W;
				btn6.pCaption = BTN6_TEXT;
				btn6.Font =  &tFont16;
				btn6.Focus = 0;
				
				if (cursor == 0) btn1.Focus = 1;
				else if (cursor == 1) btn2.Focus = 1;
				else if (cursor == 2) btn3.Focus = 1;
                else if (cursor == 3) btn4.Focus = 1;
                else if (cursor == 4) btn5.Focus = 1;
                else if (cursor == 5) btn6.Focus = 1;
				
				LCD_DrawButton(&btn1);
				LCD_DrawButton(&btn2);
				LCD_DrawButton(&btn3);
                LCD_DrawButton(&btn4);
                LCD_DrawButton(&btn5);
                LCD_DrawButton(&btn6);
			} 			
        }

        bsp_Idle();
        

        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:      /* S键释放 - 移动按钮焦点*/    
                    if (++cursor == BTN_NUM)
                    {
                        cursor = 0;
                    }
                    fRefresh = 1;
                    break;

               case KEY_UP_C:      /* C键释放 - 确认执行按钮功能 */ 		
                    if (cursor > 0)
                    {
                        cursor--;
                    }             
                    else
                    {
                        cursor = BTN_NUM - 1;
                    }
                    fRefresh = 1;
                    break;

                case KEY_LONG_DOWN_S:       /* S键长按 - 确认后，闪烁，修改参数 */      
                    if (cursor == 0)
                    {
                        FM_WirteFont();     /* 写字库 */
                    }
                    else if (cursor == 1)
                    {
                        FM_WirteBoot();     /* 写BOOT */
                    }
                    else if (cursor == 2)
                    {
                        FM_WirteQspiApp();  /* 写QSPI App */
                    }	                
                    break;
                
                case KEY_LONG_DOWN_C:    /* C键长按 */
                    g_MainStatus = MS_SYSTEM_SET;
                    break;

                default:
                    break;
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: FM_DispFileInfo
*    功能说明: 显示文件操作信息
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void FM_DispFileInfo(char *_str1)
{
    FONT_T tFont;   /* 定义字体结构体变量 */

    tFont.FontCode = FC_ST_16;              /* 字体代码 16点阵 */
    tFont.FrontColor = HELP_TEXT_COLOR;     /* 字体颜色 */
    tFont.BackColor = HELP_BACK_COLOR;      /* 文字背景颜色 */
    tFont.Space = 0;                        /* 文字间距，单位 = 像素 */
    
    LCD_DispStrEx(5, 180, _str1, &tFont, 230, 0);
    ST7789_DrawScreen();    /* 硬件SPI+DMA+刷屏 */    
}

/*
*********************************************************************************************************
*    函 数 名: FM_DispProgress
*    功能说明: 显示文件操作进度和时间
*    形    参: _percent 百分比
*              _time1 时间ms单位
*    返 回 值: 无
*********************************************************************************************************
*/
void FM_DispProgress(uint8_t _percent, uint32_t _time1)
{
    FONT_T tFont;   /* 定义字体结构体变量 */
    static uint8_t s_percent = 200;
    static uint32_t s_ms = 0;    
    char buf[10];    

    tFont.FontCode = FC_ST_16;              /* 字体代码 16点阵 */
    tFont.FrontColor = HELP_TEXT_COLOR;     /* 字体颜色 */
    tFont.BackColor = CL_MASK;      		/* 文字背景颜色 */
    tFont.Space = 0;                        /* 文字间距，单位 = 像素 */
	
    if (s_percent == 0 || s_percent != _percent || s_ms != _time1 / 100)
    {
        /* 100.2 秒 */
        sprintf(buf, "%d.%d 秒", _time1 / 1000, (_time1 % 1000) / 100);
        
        DispProgressBar(5, 200, 24, 240 - 2 * 5, "", _percent, buf, &tFont); 

//        LCD_DispStr(160, 200 + 4, buf, &tFont);
        ST7789_DrawScreen();    /* 硬件SPI+DMA+刷屏 */ 

        s_percent = _percent;
        s_ms = _time1 / 100;
    }    
}

/*
*********************************************************************************************************
*    函 数 名: FM_WirteQSPIFont
*    功能说明: 写字库
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
#define QSPI_WRITE_PAGE_SIZE	256
#define QSPI_ERASE_SECTOR_SIZE	(4 * 1024)
#define QSPI_FONT_ADDRESS		((32 - 2) * 1024 * 1024)
#define QSPI_FILE_SIZE			(2 * 1024 * 1024)
#define QSPI_WRITE_PAGE_SIZE    256
#define QSPI_ERASE_SECTOR_SIZE  (4 * 1024)
#define QSPI_BEGIN_ADDRESS      ((32 - 2) * 1024 * 1024)
#define QSPI_FILE_SIZE          (2 * 1024 * 1024)
static void FM_WirteFont(void)
{
    uint32_t i;
    FRESULT result;
    uint32_t bw;
    char *path = "0:/H7-TOOL/Fonts/GB2312ZK.bin";
    uint32_t FileSize;
    uint32_t OffsetAddr = 0;
    uint8_t percent;
    int32_t time1;

    FM_DispProgress(0, 0); 
    /* 改程序仅在CPU 内部运行时才能执行 */
    if ((uint32_t)FM_WirteFont < 0x08000000 || (uint32_t)FM_WirteFont > 0x08100000)
    {
        FM_DispFileInfo("只能在CPU内部Flash执行");
        return;
    }
        
    result = f_open(&g_file, path, FA_OPEN_EXISTING | FA_READ);
    if (result !=  FR_OK)
    {
        FM_DispFileInfo("没有找到文件GB2312ZK.bin");
        goto quit1;
    }

    /* 获得文件大小 */
    {
        FILINFO fno;

        f_stat(path, &fno);
        FileSize = fno.fsize;

        /* 文件大小 */
        if (FileSize  != 2 * 1024 * 1024)
        {
            FM_DispFileInfo("字库文件长度必须是2MB");
            goto quit1;
        }
    }

    time1 = bsp_GetRunTime();
    /* 擦除 */
    FM_DispFileInfo("正在擦除QSPI Flash字库...");
    OffsetAddr = QSPI_FONT_ADDRESS;
    for (i = 0; i < QSPI_FILE_SIZE / QSPI_ERASE_SECTOR_SIZE; i++)
    {
        QSPI_EraseSector(OffsetAddr);
        OffsetAddr += QSPI_ERASE_SECTOR_SIZE;
        percent = ((i + 1) * 100) / (QSPI_FILE_SIZE / QSPI_ERASE_SECTOR_SIZE);
        FM_DispProgress(percent, bsp_CheckRunTime(time1));        
    }

    /* 写入，每次256字节 */
    FM_DispFileInfo("正在写入...");
    percent = 0;
    FM_DispProgress(percent, bsp_CheckRunTime(time1));    
    OffsetAddr = QSPI_FONT_ADDRESS;
    for (i = 0; i < QSPI_FILE_SIZE / 1024; i++)
    {
        percent = ((i + 1) * 100) / (QSPI_FILE_SIZE / 1024);

        result = f_read(&g_file, &qspi_file_buf, 1024, &bw);
        if (result != FR_OK)
        {
            FM_DispFileInfo("文件读取失败");
            goto quit1;
        }
        
        SCB_InvalidateDCache_by_Addr((uint32_t *)qspi_file_buf,  sizeof(qspi_file_buf));
        
        QSPI_WriteBuffer(qspi_file_buf, OffsetAddr, 256);
        QSPI_WriteBuffer(qspi_file_buf + 256, OffsetAddr + 256, 256);
        QSPI_WriteBuffer(qspi_file_buf + 2 * 256, OffsetAddr + 2 * 256, 256);
        QSPI_WriteBuffer(qspi_file_buf + 3 * 256, OffsetAddr + 3 * 256, 256);

        QSPI_ReadBuffer(qspi_read_buf, OffsetAddr, 1024);
        
        SCB_InvalidateDCache_by_Addr((uint32_t *)qspi_read_buf,  sizeof(qspi_read_buf));
        if (memcmp(qspi_file_buf, qspi_read_buf, 1024) != 0)
        {
            FM_DispFileInfo("写QSPI Flash出错!");
            goto quit1;
        }

        OffsetAddr += 1024;
        percent = ((i + 1) * 100) / (QSPI_FILE_SIZE / 1024);
        FM_DispProgress(percent, bsp_CheckRunTime(time1));
    }

    FM_DispFileInfo("QSPI Flash字库成功!");
    
quit1:
    /* 关闭文件*/
    f_close(&g_file);
}

/*
*********************************************************************************************************
*    函 数 名: FM_WirteQspiApp
*    功能说明: 写QSPI APP
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
#define QSPI_APP_ADDRESS   0
static void FM_WirteQspiApp(void)
{
    uint32_t i;
    FRESULT result;
    uint32_t bw;
    char *path = "0:/H7-TOOL/Firmware/H7-QSPI_APP.bin";
    uint32_t FileSize;
    uint32_t OffsetAddr = 0;
    uint8_t percent;
    int32_t time1;

    FM_DispProgress(0, 0); 
    
    /* 改程序仅在CPU 内部运行时才能执行 */
    if ((uint32_t)FM_WirteFont < 0x08000000 || (uint32_t)FM_WirteFont > 0x08100000)
    {
        FM_DispFileInfo("只能在CPU内部Flash执行");
        return;
    }
        
    result = f_open(&g_file, path, FA_OPEN_EXISTING | FA_READ);
    if (result !=  FR_OK)
    {
        FM_DispFileInfo("没有找到文件H7-QSPI_APP.bin");
        goto quit1;
    }
    
    /* 获得文件大小 */
    {
        FILINFO fno;

        f_stat(path, &fno);
        FileSize = fno.fsize;

        /* 文件大小 */
        if (FileSize  > 30 * 1024 * 1024)
        {
            FM_DispFileInfo("文件长度必须小于30MB");
            goto quit1;
        }
    }

    time1 = bsp_GetRunTime();
    /* 擦除 */
    FM_DispFileInfo("正在擦除QSPI Flash...");
    OffsetAddr = QSPI_APP_ADDRESS;
    for (i = 0; i < (FileSize + QSPI_ERASE_SECTOR_SIZE - 1) / QSPI_ERASE_SECTOR_SIZE; i++)
    {
        QSPI_EraseSector(OffsetAddr);
        OffsetAddr += QSPI_ERASE_SECTOR_SIZE;
        percent = ((i + 1) * 100) / ((FileSize + QSPI_ERASE_SECTOR_SIZE - 1) / QSPI_ERASE_SECTOR_SIZE);
        FM_DispProgress(percent, bsp_CheckRunTime(time1));        
    }

    /* 写入，每次256字节 */
    FM_DispFileInfo("正在写入...");
    percent = 0;
    FM_DispProgress(percent, bsp_CheckRunTime(time1));    
    OffsetAddr = QSPI_APP_ADDRESS;
    for (i = 0; i < (FileSize + 1024 - 1) / 1024; i++)
    {
        percent = ((i + 1) * 100) / ((FileSize + 1024 - 1) / 1024);

        memset(qspi_file_buf, 0, 1024);   /* 末尾填0 */
        result = f_read(&g_file, &qspi_file_buf, 1024, &bw);
        if (result != FR_OK)
        {
            FM_DispFileInfo("文件读取失败");
            goto quit1;
        }
        
        
        SCB_InvalidateDCache_by_Addr((uint32_t *)qspi_file_buf,  sizeof(qspi_file_buf));
        
        QSPI_WriteBuffer(qspi_file_buf, OffsetAddr, 256);
        QSPI_WriteBuffer(qspi_file_buf + 256, OffsetAddr + 256, 256);
        QSPI_WriteBuffer(qspi_file_buf + 2 * 256, OffsetAddr + 2 * 256, 256);
        QSPI_WriteBuffer(qspi_file_buf + 3 * 256, OffsetAddr + 3 * 256, 256);

        QSPI_ReadBuffer(qspi_read_buf, OffsetAddr, 1024);
        
        SCB_InvalidateDCache_by_Addr((uint32_t *)qspi_read_buf,  sizeof(qspi_read_buf));
        if (memcmp(qspi_file_buf, qspi_read_buf, 1024) != 0)
        {
            FM_DispFileInfo("写QSPI Flash出错!");
            goto quit1;
        }

        OffsetAddr += 1024;
        percent = ((i + 1) * 100) / ((FileSize + 1024 - 1) / 1024);
        FM_DispProgress(percent, bsp_CheckRunTime(time1));
    }

    FM_DispFileInfo("写QSPI Flash成功!");
    
quit1:
    /* 关闭文件*/
    f_close(&g_file);
}

/*
*********************************************************************************************************
*    函 数 名: FM_WirteBoot
*    功能说明: 写BOOT
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
#define BOOT_BEGIN_ADDRESS  0x08000000
static void FM_WirteBoot(void)
{
    uint32_t i;
    FRESULT result;
    uint32_t bw;
    char *path = "0:/H7-TOOL/Firmware/H7-BOOT.bin";
    uint32_t FileSize;
    uint32_t OffsetAddr = 0;
    uint8_t percent;
    int32_t time1;
    
    FM_DispProgress(0, 0); 
    result = f_open(&g_file, path, FA_OPEN_EXISTING | FA_READ);
    if (result !=  FR_OK)
    {
        FM_DispFileInfo("没有找到文件H7-BOOT.bin");
        goto quit1;
    }

    /* 获得文件大小 */
    {
        FILINFO fno;

        f_stat(path, &fno);
        FileSize = fno.fsize;

        /* 文件大小 */
        if (FileSize  > 128 * 1024)
        {
            FM_DispFileInfo("文件长度大于128KB");
            goto quit1;
        }
    }
    
    /* 简单判断下文件是否boot固件 */
    {
        result = f_read(&g_file, &qspi_file_buf, 512, &bw);
        if (result != FR_OK)
        {
            FM_DispFileInfo("文件读取失败");
            goto quit1;
        }
        
        SCB_InvalidateDCache_by_Addr((uint32_t *)qspi_file_buf, 512);
        // 0x2408EDF8 0x080002AD
        // 0x20020000 0x080002AD
        if ((qspi_file_buf[3] == 0x24 || qspi_file_buf[3] == 0x20) 
            && (qspi_file_buf[7] == 0x08) && (qspi_file_buf[6] == 0x00))
        {
            /* 合法固件 */;
        }
        else
        {
            FM_DispFileInfo("不是固件文件");
            goto quit1;
        }     

        /* 还原文件指针到开头 */
        f_lseek(&g_file, 0);
    }
    
    time1 = bsp_GetRunTime();
    
    /* 擦除 */
    FM_DispFileInfo("正在擦除Flash...");
    percent = 0;
    FM_DispProgress(percent, bsp_CheckRunTime(time1)); 
    OffsetAddr = BOOT_BEGIN_ADDRESS;
    if (bsp_EraseCpuFlash(BOOT_BEGIN_ADDRESS) != HAL_OK)
    {
        percent = 100;
        FM_DispProgress(percent, bsp_CheckRunTime(time1)); 
    }
        
    /* 写入，每次1024字节 */
    FM_DispFileInfo("正在写入...");
    percent = 0;
    FM_DispProgress(percent, bsp_CheckRunTime(time1));    
    OffsetAddr = BOOT_BEGIN_ADDRESS;
    for (i = 0; i < (FileSize + 1023) / 1024; i++)
    {
        percent = ((i + 1) * 100) / ((FileSize + 1023) / 1024);
        
        memset(qspi_file_buf, 0, 1024);   /* 末尾填0 */
        result = f_read(&g_file, &qspi_file_buf, 1024, &bw);
        if (result != FR_OK)
        {
            FM_DispFileInfo("文件读取失败");
            goto quit1;
        }
        
        SCB_InvalidateDCache_by_Addr((uint32_t *)qspi_file_buf,  sizeof(qspi_file_buf));
        
        if (bsp_WriteCpuFlash(OffsetAddr, qspi_file_buf, 1024) == 0)		/* 每次写入1024个字节 */
        {
            if (memcmp((uint8_t *)OffsetAddr, qspi_file_buf, 1024) != 0)
            {
                FM_DispFileInfo("写CPU Flash出错!");
                goto quit1;
            }            
        }
        else
        {
            FM_DispFileInfo("写CPU Flash出错!");
            goto quit1;           
        }
        
        OffsetAddr += 1024;
        percent = ((i + 1) * 100) / ((FileSize + 1023) / 1024);
        FM_DispProgress(percent, bsp_CheckRunTime(time1));
    }

    FM_DispFileInfo("Boot写入成功!");
    
quit1:
    /* 关闭文件*/
    f_close(&g_file);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
