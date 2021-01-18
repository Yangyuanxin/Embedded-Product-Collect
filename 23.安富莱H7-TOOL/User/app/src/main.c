/*
*********************************************************************************************************
*
*    模块名称 : H7-TOOL App主程序
*    文件名称 : main.c
*    版    本 : V1.1
*    说    明 : 
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-10-01 armfly  正式发布
*        V1.1    2019-11-02 armfly  整理格式。增加功能。
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"

#include "status_link_mode.h"
#include "status_voltage_meter.h"
#include "status_current_meter.h"
#include "status_resistor_meter.h"
#include "status_temp_meter.h"
#include "status_programmer.h"
#include "status_system_set.h"
#include "status_tvcc_power.h"
#include "status_pulse_meter.h"
#include "status_extend_menu.h"
#include "status_mini_dso.h"
#include "status_lua.h"
#include "status_can_monitor.h"
#include "status_uart_monitor.h"
#include "wifi_if.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "emmc_diskio_dma.h"

#include "lwip_user.h"
#include "lua_if.h"

#include "target_reset.h"
#include "target_config.h"
#include "swd_host.h"
#include "SW_DP_Multi.h"

//#include "usbd_user.h"
#include "usb_if.h"
#include "file_lib.h"
#include "elf_file.h"

#include "w25q_flash.h"


extern void status_DS18B20Meter(void);

uint16_t GetStatusIndex(uint16_t _NowStatus);

uint16_t g_MainStatus;  /* 主状态字 */

/* 主状态切换顺序 */
static const uint16_t StatusOrder[] =
{
    MS_LINK_MODE,        /* 联机状态 */
    MS_VOLTAGE_METER,    /* 电压表 */
    MS_RESISTOR_METER,   /* 电阻表 */
    MS_CURRENT_METER,    /* 高侧电流表 */
    MS_TEMP_METER,       /* 温度表 */
    MS_TVCC_POWER,       /* 微型数控电源 */   
    MS_PULSE_METER,      /* 脉冲计 */
    //MS_MINI_DSO,         /* 迷你示波器 */    
};

/*
*********************************************************************************************************
*    函 数 名: main
*    功能说明: c程序入口
*    形    参: 无
*    返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
void JumpToDAPLink(void);
extern MENU_T g_tMenu1;
extern void status_UsbUart(void);
int main(void)
{    
    bsp_Init();
    
    LoadParam(); /* 读取应用程序参数, 该函数在param.c */
    
    bsp_InitTVCC();     /* TVCC控制引脚 -- 放到后面读完参数后设置 */
    bsp_SetTVCC(3300);    
    
    ST7789_SetDirection(g_tParam.DispDir);

    /* 主程序采用状态机实现程序功能切换 */
    g_MainStatus = MS_LINK_MODE; /* 初始状态 = 联机界面 */
    if (g_tParam.StartRun == 1)
    {
        g_gMulSwd.MultiMode = 0;        /* 单路烧录 */
        g_MainStatus = MS_PROG_WORK;
    }
    else if (g_tParam.StartRun == 2)
    {
        g_gMulSwd.MultiMode = g_tParam.MultiProgMode; 
        g_MainStatus = MS_PROG_WORK;    /* 多路烧录 */
    } 
    else if (g_tParam.StartRun == 3)
    {
        uint32_t RegJump;
        /*
            备份寄存器，32bit。 
            RTC_BKP_DR0 ： 用于RTC初始化标志
            RTC_BKP_DR1 ： 用于状态之间跳转
                - boot = 1, app = 2, dap = 3
                - 0x0102 : 表示从boot来，要去app
                - 0x0301 : 表示从dap来，要去app
                - 0x0103 : 表示从boot来，要去dap
        */
        RegJump = RTC_ReadBkup32(RTC_BKP_DR1);
        
        if (RegJump == 0x0000)
        {
            JumpToDAPLink();    /* 进入DAP */
        }
        
        /* 从DAP返回，直接到菜单选择状态 */
        if (RegJump == 0x0301)
        {
            g_tMenu1.Cursor = 0;
            g_MainStatus = MS_EXTEND_MENU1;
        }        
        
        /* 进入00 待机界面 */
        RTC_WriteBkup32(RTC_BKP_DR1, 0x0000);        
    }
    else    /* 0 - 缺省*/
    {
        uint32_t RegJump;
        RegJump = RTC_ReadBkup32(RTC_BKP_DR1);
        
        /* 从DAP返回，直接到菜单选择状态 */
        if (RegJump == 0x0301)
        {
            g_tMenu1.Cursor = 0;
            g_MainStatus = MS_EXTEND_MENU1;
        }
        
        RTC_WriteBkup32(RTC_BKP_DR1, 0x0000);
    }
    
    /* 执行较快的初始化代码部分 */
    {
        //PERIOD_Start(&g_tRunLed, 50, 50, 0); /* LED一直闪烁, 非阻塞 */
        PERIOD_Start(&g_tRunLed, 1000, 1000, 0);    /* LED一直闪烁, 每2秒闪1次 */
        
        bsp_InitESP32();   
        
        DSO_InitHard();
        DSO_SetDC(1, 1);
        DSO_SetDC(2, 1);
        DSO_SetGain(1, 3);
        DSO_SetGain(2, 3);

        /* 脱机烧录用的全局变量 */
        {
            g_gMulSwd.MultiMode = 0;   
            g_gMulSwd.Active[0] = 1;
            g_gMulSwd.Active[1] = 1;
            g_gMulSwd.Active[2] = 1;
            g_gMulSwd.Active[3] = 1;
        }

        usbd_Init();        /* 初始化USB协议栈 - 实际是空的 */
        
        FileSystemLoad();   /* 挂载文件系统 */
            
        lua_PowerOnLua();         /* 启动lua */        
    }
    
    #if 0
        usbd_CloseCDC();
        usbd_OpenCDC(COM_USB1); /* 启用USB虚拟串口8， 用于和PC软件USB通信 */
    #else
        g_tVar.DelayOpenUSB = 1;    /* 延迟2秒启动USB串口，WIN7系统。 */
    #endif
    
    //wifi_state = WIFI_INIT;   
    
    while (1)
    {
        switch (g_MainStatus)
        {
            case MS_LINK_MODE:      /* 联机状态 */
                status_LinkMode();
                break;

            case MS_SYSTEM_SET:     /* 系统设置 */
                status_SystemSetMain();
                break;

            case MS_HARD_INFO:      /* 硬件信息 */
                status_HardInfo();
                break;

            case MS_MODIFY_PARAM:   /* 修改参数 */
                //status_ModifyParam();
                break;   
            
            case MS_ESP32_TEST:     /* ESP32模块固件升级 */
                status_ESP32Test();
                break;

            case MS_USB_EMMC:       /* USB虚拟磁盘，电脑操作emcc文件 */
                status_UsbEMMC();
                break;

            case MS_FILE_MANAGE:    /* 文件管理，烧写字库 */
                status_FileManage();
                break;

            case MS_PROG_SELECT_FILE:    /* 脱机下载器 - 选择文件 */
                status_ProgSelectFile();
                break;   
            
            case MS_PROG_WORK:          /* 脱机下载器 - 工作阶段 */
                status_ProgWork();
                break;            

            case MS_PROG_SETTING:       /* 脱机下载器 - 参数设置 */
                status_ProgSetting();
                break;

            case MS_PROG_MODIFY_PARAM:  /* 脱机下载器 - 修改复位类型 */
                status_ProgModifyParam();
                break;             
                    
            case MS_VOLTAGE_METER:  /* 电压表 */
                status_VoltageMeter();
                break;

            case MS_CURRENT_METER:  /* 高侧电流表 */
                status_CurrentMeter();
                break;

            case MS_TEMP_METER:     /* 温度表 */
                status_TempMeter();
                break;

            case MS_RESISTOR_METER: /* 电阻表 */
                status_ResistorMeter();
                break;

            case MS_TVCC_POWER:     /* 微型电源 */
                status_TVCCPower();
                break;        
            
            case MS_PULSE_METER:    /* 脉冲测量 */
                status_PulseMeter();
                break;  
            
            case MS_MINI_DSO:    /* 扩展菜单，显示 */
                status_MiniDSO();
                break;

            case MS_EXTEND_MENU1:    /* 扩展菜单，第1级 */
                status_ExtendMenu1();
                break;

            case MS_EXTEND_MENU_REC: /* 扩展菜单，第2级-数据记录仪 */
                status_ExtendMenuRec();
                break;
            
            case MS_LUA_SELECT_FILE: /* lua文件浏览 */
                status_LuaSelectFile();
                break;
            
            case MS_LUA_EXEC_FILE:  /* lua执行状态 */    
                status_LuaRun();
                break;
            
            case MS_MONITOR_UART:   /* 串口监视 */
                status_MonitorUart();
                break;

            case MS_MONITOR_CAN:    /* CAN监视 */
                status_MonitorCan();
                break;
            
            case MS_MONITOR_GPIO:   /* 串口视器 */
                status_MonitorUart();
                break;

            case MS_DS18B20_METER:  /* DS18B20温度表 */              
                status_DS18B20Meter();
                break;              
            
            case MS_USB_UART:       /* USB虚拟串口 */
                status_UsbUart();
                break;
            
            default:
                g_MainStatus = MS_LINK_MODE;
                break;
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: DelayOpenUSBTask
*    功能说明: 延迟启动USB
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DelayOpenUSBTask(void)
{
    if (g_tVar.DelayOpenUSB == 1)        
    {
        /* 延迟2秒启动USB串口，解决WIN7系统USB异常。 */
        if (bsp_CheckRunTime(0) > 2000)
        {
            g_tVar.DelayOpenUSB = 0;
                    
            usbd_CloseCDC();
            usbd_OpenCDC(COM_USB1); /* 启用USB虚拟串口8， 用于和PC软件USB通信 */
        }
    } 
}

/*
*********************************************************************************************************
*    函 数 名: NextStatus
*    功能说明: 状态切换, 向后翻
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint16_t NextStatus(uint16_t _NowStatus)
{
    uint16_t next;
    uint16_t i;
    uint16_t count;

    count = sizeof(StatusOrder) / 2;

    for (i = 0; i < count; i++)
    {
        if (_NowStatus == StatusOrder[i])
        {
            next = i;
            break;
        }
    }

    if (++next >= count)
    {
        next = 0;
    }

    PlayKeyTone();
    return StatusOrder[next];
}

/*
*********************************************************************************************************
*    函 数 名: LastStatus
*    功能说明: 状态切换, 向前翻
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint16_t LastStatus(uint16_t _NowStatus)
{
    uint16_t next;
    uint16_t i;
    uint16_t count;

    count = sizeof(StatusOrder) / 2;

    for (i = 0; i < count; i++)
    {
        if (_NowStatus == StatusOrder[i])
        {
            next = i;
            break;
        }
    }

    if (next > 0)
    {
        next--;
    }
    else
    {
        next = count - 1;
    }
    PlayKeyTone();
    return StatusOrder[next];
}

/*
*********************************************************************************************************
*    函 数 名: GetStatusIndex
*    功能说明: 根据状态字获取菜单序号
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint16_t GetStatusIndex(uint16_t _NowStatus)
{
    uint16_t idx;
    uint16_t i;
    uint16_t count;

    count = sizeof(StatusOrder) / 2;

    for (i = 0; i < count; i++)
    {
        if (_NowStatus == StatusOrder[i])
        {
            idx = i;
            break;
        }
    }

    return idx;
}

/*
*********************************************************************************************************
*    函 数 名: DSO_StartMode2
*    功能说明: 启动示波器欧式2，多通道低速扫描模式
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DSO_StartMode2(void)
{
    WriteRegValue_06H(0x01FF, 2);    /* 多通道低速测量 */
    WriteRegValue_06H(0x0200, 1);    /* CH1选DC耦合 */
    WriteRegValue_06H(0x0201, 1);    /* CH2选DC耦合 */
    WriteRegValue_06H(0x0202, 0);    /* CH1通道增益0档，不放大 */
    WriteRegValue_06H(0x0203, 0);    /* CH2通道增益0档，不放大 */
    WriteRegValue_06H(0x0204, 0);    /* CH1通道直流偏值，未用 */
    WriteRegValue_06H(0x0205, 0);    /* CH2通道直流偏值，未用 */
    WriteRegValue_06H(0x0206, 12);   /* 采样频率1M */
    WriteRegValue_06H(0x0207, 0);    /* 采样深度1K */
    WriteRegValue_06H(0x0208, 0);    /* 触发电平 */
    WriteRegValue_06H(0x0209, 50);   /* 触发位置 */
    WriteRegValue_06H(0x020A, 0);    /* 触发模式 0=自动 */
    WriteRegValue_06H(0x020B, 0);    /* 触发通道CH1 */
    WriteRegValue_06H(0x020C, 0);    /* 触发边沿 */
    WriteRegValue_06H(0x020D, 2);    /* 通道使能 */
    WriteRegValue_06H(0x020E, 1);    /* 开始采集 */
}

/*
*********************************************************************************************************
*    函 数 名: PlayKeyTone
*    功能说明: 播放按键音。通过参数可以关闭
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void PlayKeyTone(void)
{
    if (g_tParam.KeyToneEnable != 0)
    {
        BEEP_KeyTone();
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
