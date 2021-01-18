/*
*********************************************************************************************************
*
*    模块名称 : 修改参数模块
*    文件名称 : modify_param.c
*    版    本 : V1.0
*    说    明 : 修改参数的公共函数
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2020-09-20 armfly  发布
*
*    Copyright (C), 2020-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"

/* 多行文本框 */
#define MEMO_X     5
#define MEMO_Y     5 
#define MEMO_H     (240 - MEMO_Y - 0)  
#define MEMO_W     (240 - 2 * MEMO_X)

/* 菜单 */
#define MENU_ITEM_COUNT_MAX    20  /* 菜单项最大数量 */
char g_MenuBuf[20][32];

char *g_MenuParam_Text[MENU_ITEM_COUNT_MAX + 1];

MENU_T g_tMenuParam;

static void UartMonDispParam(void);
static void UartMonAdjustParam(uint8_t _index, int16_t _adj);

static const PARAM_LIST_T *s_pParamList;
static uint16_t s_ParamCount;

/****** 串口数据监控 - 参数设置 *************************************************************************/
#define UART_MON_PARAM_COUNT    7
const char *UartMonParam0[] = {"1200", "2400", "4700", "9600", "19200", "38400", "57600", "115200"};
const char *UartMonParam1[] = {"无校验", "奇校验", "偶校验"};
const char *UartMonParam2[] = {"关闭", "启用"};
const char *UartMonParam3[] = {"12点阵", "16点阵"};
const char *UartMonParam4[] = {"关闭", "启用"};
const char *UartMonParam5[] = {"关闭", "启用"};
const char *UartMonParam6[] = {"无协议", "Modbus RTU"};
const PARAM_LIST_T UartMonParamList[UART_MON_PARAM_COUNT] = 
{ 
    /*  数据类型,      名称          可选列表,      最小值, 最大值, 缺省值 */
    {   0,         "波特率:",     UartMonParam0,   0,      7,     7},
    {   0,         "奇偶校验:",   UartMonParam1,   0,      2,     0},
    {   0,         "自动换行:",   UartMonParam2,   0,      1,     1},
    {   0,         "字体:",       UartMonParam3,   0,      1,     1},
    {   0,         "HEX显示:",    UartMonParam4,   0,      1,     0},
    {   0,         "时间戳:",     UartMonParam5,   0,      1,     0},
    {   0,         "协议:",       UartMonParam6,   0,      1,     0},    
};          
            
/****** 系统设置-基本参数 ******************************************************************************/
#define SYS_BASE_PARAM_COUNT   4
const char *SysBaseParam0[] = {"关闭", "打开"};
const char *SysBaseParam1[] = {"1分钟", "5分钟", "15分钟", "1小时", "关闭"};
const char *SysBaseParam2[] = {"16点阵", "24点阵"};
const char *SysBaseParam3[] = {"缺省", "单路烧录", "多路烧录", "DAP-Link"};

const PARAM_LIST_T SysBaseParamList[SYS_BASE_PARAM_COUNT] = 
{ 
    /*  数据类型,      名称          可选列表,      最小值, 最大值, 缺省值 */
    {   0,         "按键音: ",       SysBaseParam0,   0,      1,     1},
    {   0,         "屏保超时: ",     SysBaseParam1,   0,      4,     0},
    {   0,         "列表字体: ",     SysBaseParam2,   0,      1,     0},
    {   0,         "开机启动: ",     SysBaseParam3,      0,      3,       0}, 
};

/****** 系统设置-IP参数 ******************************************************************************/
#define NET_PARAM_COUNT   13

const PARAM_LIST_T NetParamList[NET_PARAM_COUNT] = 
{ 
    /*  数据类型,      名称          可选列表,      最小值, 最大值, 缺省值 */
    {   0,         "RJ45本机IP0: ",   0,              0,      255,   192},
    {   0,         "RJ45本机IP1: ",   0,              0,      255,   168},
    {   0,         "RJ45本机IP2: ",   0,              0,      255,   1},
    {   0,         "RJ45本机IP3: ",   0,              0,      255,   211},
    {   0,         "RJ45网关地址0: ", 0,              0,      255,   192},
    {   0,         "RJ45网关地址1: ", 0,              0,      255,   168},
    {   0,         "RJ45网关地址2: ", 0,              0,      255,   1},
    {   0,         "RJ45网关地址3: ", 0,              0,      255,   1}, 
    {   0,         "RJ45子网掩码0: ", 0,              0,      255,   255},
    {   0,         "RJ45子网掩码1: ", 0,              0,      255,   255},
    {   0,         "RJ45子网掩码2: ", 0,              0,      255,   255},
    {   0,         "RJ45子网掩码3: ", 0,              0,      255,   0},
    {   0,         "端口号: ",        0,              1024, 65535,   30010},    
};

/****** 烧录参数   ******************************************************************************/
#define PROG_PARAM_COUNT   4
const char *ProgParam0[] = {"关闭", "1路", "1-2路", "1-3路", "1-4路"};

const PARAM_LIST_T ProgParamList[PROG_PARAM_COUNT] = 
{ 
    /*  数据类型,      名称          可选列表,      最小值, 最大值, 缺省值 */
    {   0,         "多路模式: ",     ProgParam0,    0,      4,       4},
    {   0,         "工厂代码: ",     0,             0,      999,     0},
    {   0,         "烧录器编号: ",   0,             0,      999,     0},
    {   0,         "开机启动: ",     SysBaseParam3,    0,      3,       0}, 
};

/****** DAP-Link参数   ******************************************************************************/
#define DAP_PARAM_COUNT   4
const char *DapParam1[] = {"关闭", "1.2V", "1.5V", "1.8V", "2.0V", "2.5V", "2.8V", "3.0V", 
    "3.3V", "3.6V", "4.2V", "4.5V", "4.8V", "5.0V"};
const char *DapParam2[] = {"关闭", "开启"};
const char *DapParam3[] = {"缺省", "单路烧录", "多路烧录", "DAP-Link"};

const PARAM_LIST_T DapParamList[DAP_PARAM_COUNT] = 
{ 
    /*  数据类型,      名称          可选列表,      最小值, 最大值, 缺省值 */
    {   0,          "TVCC电压: ",     DapParam1,     0,      13,    0},
    {   0,          "蜂鸣使能: ",     DapParam2,     0,      1,     1}, 
    {   0,          "开机启动: ",     DapParam3,    0,      3,     0},
    {   0,          "DAP 编号: ",      0,            1,      99,    1},    
};

/*
*********************************************************************************************************
*    函 数 名: ModifyParam
*    功能说明: 系统设置状态. 菜单选择
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ModifyParam(uint16_t _MainStatus)
{
    uint8_t ucKeyCode;
    uint8_t fRefresh;
    static uint8_t s_enter_sub_menu = 0;
    uint8_t fQuit = 0;
    uint8_t ucModifyStatus = 0;
    uint8_t ucLastAdj = 0;
    int32_t iLastTime = 0;
    uint8_t fSaveParam = 0;

    DispHeader2(90, "设置参数");
    
    /* 动态菜单项初始化指针 */
    {
        uint8_t i;
        
        for (i = 0; i < MENU_ITEM_COUNT_MAX; i++)
        {
            g_MenuParam_Text[i] = g_MenuBuf[i];
        }
    }

    UartMonInitParam(_MainStatus);
    UartMonDispParam();
    
    if (s_enter_sub_menu == 0)
    {
        g_tMenuParam.Left = MENU_LEFT;
        g_tMenuParam.Top = MENU_TOP;
        g_tMenuParam.Height = MENU_HEIGHT;
        g_tMenuParam.Width = MENU_WIDTH;
        g_tMenuParam.LineCap = MENU_CAP;
        g_tMenuParam.ViewLine = 7;
        g_tMenuParam.Font.FontCode = FC_ST_24;
//        g_tMenuParam.Font.FrontColor = CL_BLACK;        /* 字体颜色 */
//        g_tMenuParam.Font.BackColor = FORM_BACK_COLOR;    /* 文字背景颜色 */
        g_tMenuParam.Font.Space = 0;
        g_tMenuParam.RollBackEn = 1;  /* 允许回滚 */        
        g_tMenuParam.GBK = 0;
        g_tMenuParam.ActiveBackColor = 0;   /* 选中行背景色ID */        
        LCD_InitMenu(&g_tMenuParam, (char **)g_MenuParam_Text); /* 初始化菜单结构 */
    }

    fRefresh = 1;
    while (fQuit == 0)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            UartMonDispParam();
            LCD_DispMenu(&g_tMenuParam);
        }
        
        bsp_Idle();
        
        if (ucModifyStatus == 0)
        {        
            ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
            if (ucKeyCode != KEY_NONE)
            {
                /* 有键按下 */
                switch (ucKeyCode)
                {
                    case KEY_UP_S:          /* S键 上 */                   
                        LCD_MoveUpMenu(&g_tMenuParam);
                        break;

                    case KEY_LONG_DOWN_S:   /* S键长按，进入参数修改状态 */
                        PlayKeyTone();
                        g_tMenuParam.ActiveBackColor = 1;   /* 选中行背景色ID */   
                        fRefresh = 1;
                        break;

                    case KEY_LONG_UP_S:     /* 长按弹起 */
                        if (g_tMenuParam.ActiveBackColor == 1)
                        {
                            ucModifyStatus = 1;
                            ucLastAdj = 0;
                            bsp_SetKeyParam(KID_S, KEY_LONG_TIME, 3);   /* 600ms 算长按，间隔30ms发码1次（每秒20个） */
                        }
                        break;
                        
                    case KEY_UP_C:          /* C键 下 */
                        LCD_MoveDownMenu(&g_tMenuParam);
                        break;

                    case KEY_LONG_DOWN_C:   /* C键长按 */
                        PlayKeyTone();
                        s_enter_sub_menu = 0;
                        fQuit = 1;
                        break;

                    default:
                        break;
                }
            }
        }
        else    /* 修改状态 */
        { 
            ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
            if (ucKeyCode != KEY_NONE)
            {
                /* 有键按下 */
                switch (ucKeyCode)
                {
                    case KEY_UP_S:          /* S键短按 */                   
                        UartMonAdjustParam(g_tMenuParam.Cursor, -1);
                        ucLastAdj = 0;
                        fRefresh = 1;
                        fSaveParam = 1;
                        break;

                    case KEY_LONG_DOWN_S:   /* S键长按 */
                        iLastTime = bsp_GetRunTime();
                        break;

                    case KEY_LONG_UP_S:     /* 长按弹起 */                    
                        break;
                    
                    case KEY_AUTO_S:        /* 长安S自动发码 */
                        {
                            int32_t step;
                            
                            step = s_pParamList[g_tMenuParam.Cursor].MaxValue - s_pParamList[g_tMenuParam.Cursor].MinValue;
                            if (step < 500)
                            {
                                step = 1;
                            }
                            else
                            {
                                if (bsp_CheckRunTime(iLastTime) < 5000)
                                {
                                    step = 1;
                                }
                                else if (bsp_CheckRunTime(iLastTime) < 10000)    
                                {
                                    step = 10;
                                }
                                else
                                {
                                    step = 100;
                                }
                            }
                            if (ucLastAdj == 0) /* 上次短按递减，这次继续自动递减 */
                            {                                
                                step = -step;
                                UartMonAdjustParam(g_tMenuParam.Cursor, step);
                            }
                            else    /* 上次短按递增，这次继续自动递增 */
                            {
                                step = step;
                                UartMonAdjustParam(g_tMenuParam.Cursor, step);
                            }
                            fSaveParam = 1;
                        }
                        fRefresh = 1;
                        break;
                        
                    case KEY_UP_C:          /* C键 下 */
                        UartMonAdjustParam(g_tMenuParam.Cursor, 1);
                        fRefresh = 1;
                        ucLastAdj = 1;
                        fSaveParam = 1;
                        break;

                    case KEY_LONG_DOWN_C:   /* C键长按 */
                        SaveParam();
                        g_tMenuParam.ActiveBackColor = 0;   /* 选中行背景色ID */  
                        ucModifyStatus = 0;
                        bsp_SetKeyParam(KID_S, KEY_LONG_TIME, 0);   /* 600ms 算长按，取消自动发码 */
                        fRefresh = 1;
                        break;

                    default:
                        break;
                }
            }           
        }
    }
    
    if (fSaveParam == 1)
    {
        SaveParam();    /* 保存参数 */
    }
}

/*
*********************************************************************************************************
*    函 数 名: UartMonInitParam
*    功能说明: 初始化参数列表
*    形    参: _ParamID : 参数ID
*    返 回 值: 无
*********************************************************************************************************
*/
void UartMonInitParam(uint16_t _ParamID)
{
    if (_ParamID == MODIFY_PARAM_UART_MON)
    {
        s_pParamList = UartMonParamList;
        s_ParamCount = UART_MON_PARAM_COUNT;
    }
    else if (_ParamID == MODIFY_PARAM_SYSTEM)
    {
        s_pParamList = SysBaseParamList;
        s_ParamCount = SYS_BASE_PARAM_COUNT;        
    }    
    else if (_ParamID == MODIFY_PARAM_PROG)
    {
        s_pParamList = ProgParamList;
        s_ParamCount = PROG_PARAM_COUNT;         
    }
    else if (_ParamID == MODIFY_PARAM_NET)
    {
        s_pParamList = NetParamList;
        s_ParamCount = NET_PARAM_COUNT;         
    }
    else if (_ParamID == MODIFY_PARAM_DAPLINK)
    {
        s_pParamList = DapParamList;
        s_ParamCount = DAP_PARAM_COUNT;         
    }    	    
}

/*
*********************************************************************************************************
*    函 数 名: MonDispReadParam
*    功能说明: 读参数
*    形    参: _index : 参数索引
*    返 回 值: 无
*********************************************************************************************************
*/
static int32_t MonDispReadParam(uint8_t _index)
{
    int32_t value = 0;
    
    if (s_pParamList == UartMonParamList)
    {    
        if (_index == 0) value = g_tParam.UartMonBaud;
        else if (_index == 1) value = g_tParam.UartMonParit;
        else if (_index == 2) value = g_tParam.UartMonWordWrap;
        else if (_index == 3) value = g_tParam.UartMonFont;
        else if (_index == 4) value = g_tParam.UartMonHex;
        else if (_index == 5) value = g_tParam.UartMonTimeStamp;
        else if (_index == 6) value = g_tParam.UartMonProxy;
    }    
    else if (s_pParamList == SysBaseParamList)
    {
        if (_index == 0) value = g_tParam.KeyToneEnable;
        else if (_index == 1) value = g_tParam.LcdSleepTime;
        else if (_index == 2) value = g_tParam.FileListFont24;
        else if (_index == 3) value = g_tParam.StartRun;       
    }
    else if (s_pParamList == NetParamList)
    {
        if (_index == 0) value = g_tParam.LocalIPAddr[0];
        else if (_index == 1) value = g_tParam.LocalIPAddr[1];
        else if (_index == 2) value = g_tParam.LocalIPAddr[2];
        else if (_index == 3) value = g_tParam.LocalIPAddr[3];
        else if (_index == 4) value = g_tParam.Gateway[0];
        else if (_index == 5) value = g_tParam.Gateway[1];
        else if (_index == 6) value = g_tParam.Gateway[2];
        else if (_index == 7) value = g_tParam.Gateway[3];
        else if (_index == 8) value = g_tParam.NetMask[0];
        else if (_index == 9) value = g_tParam.NetMask[1];
        else if (_index == 10) value = g_tParam.NetMask[2];
        else if (_index == 11) value = g_tParam.NetMask[3];
        else if (_index == 12) value = g_tParam.LocalTCPPort;         
    }    
    else if (s_pParamList == ProgParamList)
    {
        if (_index == 0) value = g_tParam.MultiProgMode;
        else if (_index == 1) value = g_tParam.FactoryId;
        else if (_index == 2) value = g_tParam.ToolSn;
        else if (_index == 3) value = g_tParam.StartRun;        
    }   
    else if (s_pParamList == DapParamList)
    {
        if (_index == 0) value = g_tParam.DAP_TVCCVolt;
        else if (_index == 1) value = g_tParam.DAP_BeepEn;
        else if (_index == 2) value = g_tParam.StartRun;
        else if (_index == 3) value = g_tParam.DAP_Sn;      
    }     
    return value;
}

/*
*********************************************************************************************************
*    函 数 名: MonDispWriteParam
*    功能说明: 修改参数
*    形    参: _index : 参数索引
*              _value : 参数值
*    返 回 值: 无
*********************************************************************************************************
*/
static void MonDispWriteParam(uint8_t _index, int32_t _value)
{   
    if (s_pParamList == UartMonParamList)
    {
        if (_index == 0)  g_tParam.UartMonBaud = _value;
        else if (_index == 1) g_tParam.UartMonParit = _value;
        else if (_index == 2) g_tParam.UartMonWordWrap = _value;
        else if (_index == 3) g_tParam.UartMonFont = _value;
        else if (_index == 4) g_tParam.UartMonHex = _value;
        else if (_index == 5) g_tParam.UartMonTimeStamp = _value;
        else if (_index == 6) g_tParam.UartMonProxy = _value;
    }
    else if (s_pParamList == SysBaseParamList)
    {
        if (_index == 0) g_tParam.KeyToneEnable = _value;
        else if (_index == 1) g_tParam.LcdSleepTime = _value;
        else if (_index == 2) g_tParam.FileListFont24 = _value;
        else if (_index == 3) g_tParam.StartRun = _value;
    }
    else if (s_pParamList == NetParamList)
    {     
        if (_index == 0) g_tParam.LocalIPAddr[0] = _value;
        else if (_index == 1) g_tParam.LocalIPAddr[1] = _value;
        else if (_index == 2) g_tParam.LocalIPAddr[2] = _value;
        else if (_index == 3) g_tParam.LocalIPAddr[3] = _value;
        else if (_index == 4) g_tParam.Gateway[0] = _value;
        else if (_index == 5) g_tParam.Gateway[1] = _value;
        else if (_index == 6) g_tParam.Gateway[2] = _value;
        else if (_index == 7) g_tParam.Gateway[3] = _value;
        else if (_index == 8) g_tParam.NetMask[0] = _value;
        else if (_index == 9) g_tParam.NetMask[1] = _value;
        else if (_index == 10) g_tParam.NetMask[2] = _value;
        else if (_index == 11) g_tParam.NetMask[3] = _value;
        else if (_index == 12) g_tParam.LocalTCPPort = _value;   
    }    
    else if (s_pParamList == ProgParamList)
    {
        if (_index == 0)  g_tParam.MultiProgMode = _value;
        else if (_index == 1)  g_tParam.FactoryId = _value;
        else if (_index == 2)  g_tParam.ToolSn = _value;
        else if (_index == 3)  g_tParam.StartRun = _value;       
    }
    else if (s_pParamList == DapParamList)
    {
        if (_index == 0) g_tParam.DAP_TVCCVolt = _value;
        else if (_index == 1) g_tParam.DAP_BeepEn = _value;
        else if (_index == 2) g_tParam.StartRun = _value;
        else if (_index == 3) g_tParam.DAP_Sn = _value;      
    }     
}

/*
*********************************************************************************************************
*    函 数 名: UartMonDispParam
*    功能说明: 显示参数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void UartMonDispParam(void)
{
    uint8_t i;
    int32_t min, max, def;
    int32_t now;    
    
    for (i = 0; i < s_ParamCount; i++)
    {
        min = s_pParamList[i].MinValue;
        max = s_pParamList[i].MaxValue;
        def = s_pParamList[i].DefaultValue;
        
        now = MonDispReadParam(i);
        
        if (now < min || now > max)
        {
            now = def;
        }

        if (s_pParamList[i].ParamItems == 0)
        {
            sprintf(g_MenuBuf[i], "%s%d", s_pParamList[i].ParamName, now);;
        }
        else
        {
            sprintf(g_MenuBuf[i], "%s%s", s_pParamList[i].ParamName, s_pParamList[i].ParamItems[now]);
        }
    }

    sprintf(g_MenuBuf[i], "&");     /* 结束符 */
}

/*
*********************************************************************************************************
*    函 数 名: UartMonAdjustParam
*    功能说明: 修改参数值
*    形    参: _index : 参数序号
*              _adj : 调节大小
*    返 回 值: 无
*********************************************************************************************************
*/
static void UartMonAdjustParam(uint8_t _index, int16_t _adj)
{
    int32_t min, max, def;
    int32_t OldValue, NewValue;
    
    if (_index >= s_ParamCount)
    {
        return;
    }
    
    min = s_pParamList[_index].MinValue;
    max = s_pParamList[_index].MaxValue;
    def = s_pParamList[_index].DefaultValue;
    
    OldValue = MonDispReadParam(_index);
    
    if (OldValue < min || OldValue > max)
    {
        NewValue = def;
    }
    else
    {
        NewValue = OldValue + _adj;
        
        if (NewValue > max)
        {
            NewValue = min;
        }
        if (NewValue < min)
        {
            NewValue = max;
        }
    }
   
    MonDispWriteParam(_index, NewValue);
}

/*
*********************************************************************************************************
*    函 数 名: UartMonCheckParam
*    功能说明: 检查参数合法性并修正
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void UartMonCheckParam(void)
{
    uint8_t i;
    int32_t min, max, def;
    int32_t now;    
    
    for (i = 0; i < s_ParamCount; i++)
    {
        min = s_pParamList[i].MinValue;
        max = s_pParamList[i].MaxValue;
        def = s_pParamList[i].DefaultValue;
        
        now = MonDispReadParam(i);
        
        if (now < min || now > max)
        {
            now = def;
        }

        MonDispWriteParam(i, now);
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
