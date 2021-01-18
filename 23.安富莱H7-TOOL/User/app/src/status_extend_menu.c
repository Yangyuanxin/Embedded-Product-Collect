/*
*********************************************************************************************************
*
*    模块名称 : 扩展功能菜单
*    文件名称 : status_extend_menu.c
*    版    本 : V1.0
*    说    明 : 扩展功能菜单。 比如烧录器、LUA小程序、串口记录仪
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-12-14  armfly  正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "lcd_menu.h"
#include "SW_DP_Multi.h"
#include "modify_param.h"
#include "usb_if.h"

const uint8_t *g_Menu1_Text[] =
{
    " 1 CMSIS-DAP仿真器",     
    " 2 脱机烧录器(单路)",
    " 3 脱机烧录器(多路)",
    " 4 LUA小程序",
    " 5 数据监视器",
    " 6 USB虚拟串口", 
    " 7 系统设置", 
    
    /* 结束符号, 用于菜单函数自动识别菜单项个数 */
    "&"    
};

MENU_T g_tMenu1;

const uint8_t *g_MenuRec_Text[] = 
{
    "1 串口(TTL 485 232)",
    "2 DS18B20温度表",

    /* 结束符号, 用于菜单函数自动识别菜单项个数 */
    "&"
};

MENU_T g_tMenuRec;

void JumpToDAPLink(void);

/*
*********************************************************************************************************
*    函 数 名: status_ExtendMenu1
*    功能说明: 扩展功能菜单- 第1级
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_ExtendMenu1(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    static uint8_t s_MenuInit = 0;
    

    DispHeader("扩展功能");
//    DispHelpBar("",
//                ""); 
    
    if (s_MenuInit == 0)
    {
        s_MenuInit = 1;
        
        g_tMenu1.Left = MENU_LEFT;
        g_tMenu1.Top = MENU_TOP;
        g_tMenu1.Height = MENU_HEIGHT;
        g_tMenu1.Width = MENU_WIDTH;
        g_tMenu1.LineCap = MENU_CAP;
        g_tMenu1.ViewLine = 8;
        g_tMenu1.Font.FontCode = FC_ST_24;
        g_tMenu1.Font.Space = 0;
        g_tMenu1.RollBackEn = 1;  /* 允许回滚 */
        g_tMenu1.GBK = 0;
        g_tMenu1.ActiveBackColor = 0;   /* 选中行背景色ID */        
        LCD_InitMenu(&g_tMenu1, (char **)g_Menu1_Text); /* 初始化菜单结构 */
    }    
    LCD_DispMenu(&g_tMenu1);

    fRefresh = 1;
    while (g_MainStatus == MS_EXTEND_MENU1)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            if (g_tMenu1.Cursor == 0)
            {
                ;
            }
        }

        bsp_Idle();
        
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:          /* S键 上 */
                    LCD_MoveUpMenu(&g_tMenu1);
                    break;

                case KEY_LONG_DOWN_S:   /* S键 上 */
                    if (g_tMenu1.Cursor == 0)
                    {
                        /* 跳转到DAP固件 */
                        JumpToDAPLink();
                    }                     
                    else if (g_tMenu1.Cursor == 1)
                    {
                        g_gMulSwd.MultiMode = 0;
                        g_MainStatus = MS_PROG_WORK;
                    }
                    else if (g_tMenu1.Cursor == 2)
                    {
                        g_gMulSwd.MultiMode = g_tParam.MultiProgMode;        
                        g_MainStatus = MS_PROG_WORK;
                    }                    
                    else if (g_tMenu1.Cursor == 3)
                    {
                        g_MainStatus = MS_LUA_SELECT_FILE;
                    }
                    else if (g_tMenu1.Cursor == 4)
                    {
                        g_MainStatus = MS_EXTEND_MENU_REC;
                    }   
                    else if (g_tMenu1.Cursor == 5)
                    {
                        g_MainStatus = MS_USB_UART;                        
                    }
                    else if (g_tMenu1.Cursor == 6)
                    {
                        g_MainStatus = MS_SYSTEM_SET;                        
                    }                     
                    break;

                case KEY_UP_C:          /* C键 下 */
                    LCD_MoveDownMenu(&g_tMenu1);
                    break;

                case KEY_LONG_DOWN_C:   /* C键长按 */
                    PlayKeyTone();
                    g_MainStatus = MS_LINK_MODE;
                    break;

                default:
                    break;
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: status_ExtendMenuRec
*    功能说明: 扩展功能菜单 选择记录仪
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_ExtendMenuRec(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    static uint8_t s_MenuInit = 0;

    if (s_MenuInit == 0)
    {
        s_MenuInit = 1;
        
        g_tMenuRec.Left = MENU_LEFT;
        g_tMenuRec.Top = MENU_TOP;
        g_tMenuRec.Height = MENU_HEIGHT;
        g_tMenuRec.Width = MENU_WIDTH;
        g_tMenuRec.LineCap = MENU_CAP;
        g_tMenuRec.ViewLine = 8;
        g_tMenuRec.Font.FontCode = FC_ST_24;
        g_tMenuRec.Font.Space = 0;
        g_tMenuRec.RollBackEn = 1;  /* 允许回滚 */   
        g_tMenuRec.GBK = 0; 
        g_tMenuRec.ActiveBackColor = 0;   /* 选中行背景色ID */      
        LCD_InitMenu(&g_tMenuRec, (char **)g_MenuRec_Text); /* 初始化菜单结构 */
    }
    LCD_DispMenu(&g_tMenuRec);
    
    DispHeader("请选择");
//    DispHelpBar("",
//                ""); 
    
    LCD_DispMenu(&g_tMenuRec);

    fRefresh = 1;
    while (g_MainStatus == MS_EXTEND_MENU_REC)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            if (g_tMenuRec.Cursor == 0)
            {
                ;
            }
        }

        bsp_Idle();
        
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:          /* S键 上 */
                    PlayKeyTone();
                    LCD_MoveUpMenu(&g_tMenuRec);
                    break;

                case KEY_LONG_DOWN_S:   /* S键 上 */
                    PlayKeyTone();

                    if (g_tMenuRec.Cursor == 0)
                    {
                        g_MainStatus = MS_MONITOR_UART;
                    }
                    else if (g_tMenuRec.Cursor == 1)
                    {
                        g_MainStatus = MS_DS18B20_METER;
                    }
                    else if (g_tMenuRec.Cursor == 2)
                    {
                        g_MainStatus = MS_MONITOR_GPIO;
                    }
                    else if (g_tMenuRec.Cursor == 3)
                    {
                        g_MainStatus = MS_MONITOR_ANALOG;
                    }                                
                    break;

            case KEY_UP_C:              /* C键 下 */
                PlayKeyTone();
                LCD_MoveDownMenu(&g_tMenuRec);
                break;

            case KEY_LONG_DOWN_C:       /* C键长按 */
                PlayKeyTone();
                g_MainStatus = MS_EXTEND_MENU1;
                break;

            default:
                break;
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: JumpToDAPLink
*    功能说明: 跳转固件，去执行daplink固件
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void JumpToDAPLink(void)
{
    uint8_t i;
    __IO uint32_t AppAddr = 0x08100000;  /* APP 地址 */        
    void (*AppJump)(void);         /* 声明一个函数指针 */
    
    usbd_UnInit();
    
    /* 关闭全局中断 */
    DISABLE_INT(); 
    
    /* 关闭所有的外设 */
    bsp_DeInit();
    
    /* 设置所有时钟到默认状态，使用HSI时钟 */
    HAL_RCC_DeInit();
    
    /* 关闭UART外设 */
    {                                    
        USART1->CR1 &= ~USART_CR1_UE;
        USART2->CR1 &= ~USART_CR1_UE;
        USART3->CR1 &= ~USART_CR1_UE;
        UART4->CR1 &= ~USART_CR1_UE;
        UART5->CR1 &= ~USART_CR1_UE;
        USART6->CR1 &= ~USART_CR1_UE;
        UART7->CR1 &= ~USART_CR1_UE;
        UART8->CR1 &= ~USART_CR1_UE;
    }
    
    /* 关闭滴答定时器，复位到默认值 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    /* 关闭所有中断，清除所有中断挂起标志 */
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i]=0xFFFFFFFF;
        NVIC->ICPR[i]=0xFFFFFFFF;
    }	

    /* 使能全局中断 */
    ENABLE_INT();

    /* 跳转到应用程序，首地址是MSP，地址+4是复位中断服务程序地址 */
    AppJump = (void (*)(void)) (*((uint32_t *) (AppAddr + 4)));

    /* 设置主堆栈指针 */
    __set_MSP(*(uint32_t *)AppAddr);
    
    /* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
    __set_CONTROL(0);

    /* 跳转到系统BootLoader */
    AppJump(); 

    /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
    while (1)
    {

    }
}
        
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
