/*
*********************************************************************************************************
*
*    模块名称 : 脱机编程器
*    文件名称 : status_programmer.c
*    版    本 : V1.0
*    说    明 : 脱机编程器
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-10-06 armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"

#include "SW_DP_Multi.h"

/* 三个按钮 */
#define BTN1_X     (240 - BTN1_W - 5)
#define BTN1_Y     80
#define BTN1_H     28 
#define BTN1_W     80 

#define BTN2_X     BTN1_X
#define BTN2_Y     BTN1_Y + BTN1_H + 8 
#define BTN2_H     BTN1_H 
#define BTN2_W     BTN1_W 

#define BTN3_X     BTN1_X
#define BTN3_Y     BTN1_Y + 2 * (BTN1_H + 8)
#define BTN3_H     BTN1_H 
#define BTN3_W     BTN1_W 

#define TEXT_WIDTH  144

/* 文件大小 */
#define TEXT1_X    5
#define TEXT1_Y    BTN1_Y

/* 本次次数  */
#define TEXT2_X    TEXT1_X
#define TEXT2_Y    TEXT1_Y + 18

/* 累积次数+限制次数 */
#define TEXT3_X    TEXT1_X
#define TEXT3_Y    TEXT2_Y + 24

/* CRC32 */
#define TEXT4_X    TEXT1_X
#define TEXT4_Y    TEXT3_Y + 20

/* 供电电压*/
#define TEXT_VOLT_X     5
#define TEXT_VOLT_Y     TEXT4_Y + 25
#define TEXT_VOLT_W     62

/* 供电电流 */
#define TEXT_CURR_X     TEXT_VOLT_X + TEXT_VOLT_W + 8
#define TEXT_CURR_Y     TEXT_VOLT_Y
#define TEXT_CURR_W     62

/* 烧录时间 */
#define PROG_TIME_X     160
#define PROG_TIME_Y     240 - 50

/* 烧录步骤提示 */
#define PROG_STEP_X     5
#define PROG_STEP_Y     240 - 50

/* 烧录进度条 */
#define PROGRESS_X      5
#define PROGRESS_Y      240 - 30


const uint8_t *g_MenuProg1_Text[] =
{
    " 1 修改编程参数",    
    " 2 清零本次计数",
    " 3 清零累计计数",
    " 4 输入产品序号",
    /* 结束符号, 用于菜单函数自动识别菜单项个数 */
    "&"
};

MENU_T g_tMenuProg1;

void DispProgVoltCurrent(void);

static void DispProgCounter(void);
static uint32_t GetDataFileSum(void);

extern void sysTickInit(void);
extern uint8_t swd_init_debug(void);
extern uint8_t swd_read_idcode(uint32_t *id);
extern uint8_t swd_init(void);


static uint32_t s_DataFileSum = 0;

/*
*********************************************************************************************************
*    函 数 名: status_ProgSelectFile
*    功能说明: 脱机编程器界面，选择文件。 - 文件浏览器
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_ProgSelectFile(void)
{
    uint8_t re;
    
    re = SelectFile(PROG_USER_DIR, MS_PROG_SELECT_FILE, MS_PROG_WORK, "*.lua");
    
    if (re == 1)
    {
        /* 保存缺省lua文件路径到autorun.ini文件 */
        SaveProgAutorunFile(g_tFileList.Path);
    }
} 

/*
*********************************************************************************************************
*    函 数 名: status_ProgWork
*    功能说明: 脱机编程器烧录状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void PG_PrintText(char *_str);
void status_ProgWork(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    BUTTON_T btn1, btn2, btn3;
    FONT_T tFontNote;
    FONT_T tFontText;
    FONT_T tFontBtn;   
    uint8_t cursor = 0;  
    uint8_t fRunOnce = 0; 
    uint8_t ucAutoState = 0;
    uint8_t ucDetectCount = 0;    /* 连续多少次没有检测到芯片则认为目标板已拔出 */
    int32_t iDetectTime;
        
    DSO_StartMode2();   /* 示波器启动模式2-低速多通道扫描 */
    
    /* 读autorun.ini文件，获得缺省lua文件路径 */
    LoadProgAutorunFile(g_tProg.FilePath, sizeof(g_tProg.FilePath));
    
    /* 设置字体参数 */
    {
        tFontNote.FontCode = FC_ST_16;              /* 字体代码 16点阵 */
        tFontNote.FrontColor = INFO_VALUE_COLOR;    /* 字体颜色 */
        tFontNote.BackColor = CL_WHITE;             /* 文字背景颜色 */      
        tFontNote.Space = 0;                        /* 文字间距，单位 = 像素 */

        tFontText.FontCode = FC_ST_16;              /* 字体代码 16点阵 */
        tFontText.FrontColor = INFO_NAME_COLOR;     /* 字体颜色 */
        tFontText.BackColor = FORM_BACK_COLOR;      /* 文字背景颜色 */      
        tFontText.Space = 0;                        /* 文字间距，单位 = 像素 */      

        tFontBtn.FontCode = FC_ST_16;               /* 字体代码 16点阵 */
        tFontBtn.FrontColor = INFO_NAME_COLOR;      /* 字体颜色 */
        tFontBtn.BackColor = CL_MASK;               /* 文字背景颜色 */      
        tFontBtn.Space = 0;                         /* 文字间距，单位 = 像素 */          
    }    

    LCD_ClrScr(FORM_BACK_COLOR); 
    
    {                
        s_lua_read_len = ReadFileToMem(g_tProg.FilePath, 0, s_lua_prog_buf, LUA_PROG_LEN_MAX);  /* 读lua文件 */
        
        {
            /* 绘制边框 */
            LCD_DrawRoundRect(4, 4, 64, 240 - 8, 3, MEAS_BODER_COLOR);
            
            /* 填充矩形 */
            LCD_FillRoundRect(4 + 1, 4 + 1, 64 - 2, 240 - 10, 2, CL_WHITE);                 
        }      
        
        if (s_lua_read_len > 0)
        {              
            const char *pNote1;
            char *p;
            char buf[64];   

            uint8_t i,j;
            uint16_t len;
            uint8_t line = 0;
            
            lua_DownLoadFile(g_tProg.FilePath);  /* 重新初始化lua环境，并装载lua文件到内存，不执行 */  
            lua_RunLuaProg();   /* 执行lua */
            
            /* 从lua文件中获得注释字符串Note01 */
            lua_getglobal(g_Lua, "Note01");    
            if (lua_isstring(g_Lua, -1))
            {
                pNote1 = lua_tostring(g_Lua, -1); 
            }
            else
            {
                pNote1 = "";
            }
            lua_pop(g_Lua, 1);            
            
            LCD_SetEncode(ENCODE_GBK);            
            
            /* 显示lua文件名，两行最多56字符。 需要处理过长的文件名，每行只能显示28字符 */
            p = &g_tProg.FilePath[strlen(PROG_USER_DIR) + 1];
            if (strlen(p) > 28)
            {
                line = 2;
            }
            else
            {
                line = 1;
            }
            
            for (j = 0; j < line; j++)
            {
                len = 0;
                for (i = 0; i < 28; i++)
                {
                    buf[len] = *p++;
                    if (buf[len] == 0)
                    {
                        break;
                    }
                    len++;
                }
                buf[len] = 0;
                LCD_DispStr(5 + 3,      5 + 3 + 20 * j,      buf, &tFontNote);                 
            }          

            tFontNote.FrontColor = MEAS_VALUE_COLOR; 
            LCD_DispStr(5 + 3,      5 + 3 + 40, (char *)pNote1, &tFontNote);  /* 显示lua中的注释 */        
            
            LCD_SetEncode(ENCODE_UTF8);
            
            s_DataFileSum = GetDataFileSum();   /* 读数据文件，计算文件校验和 */
            
            /* 读ini配置文件 */
            ReadProgIniFile(g_tProg.FilePath, &g_tProgIni);
            DispProgCounter();     
        }
//        else
//        {
//            LCD_DispStr(5 + 3,      5 + 3,      "未选择文件", &tFontText);   
//            LCD_DispStr(5 + 3,      5 + 3 + 30, "", &tFontText);                
//        }
        
        /* 当前烧录模式 */
        if (g_gMulSwd.MultiMode == 0) PG_PrintText("单路模式"); 
        else if (g_gMulSwd.MultiMode == 1) PG_PrintText("多路模式:1路");
        else if (g_gMulSwd.MultiMode == 2) PG_PrintText("多路模式:1-2路");
        else if (g_gMulSwd.MultiMode == 3) PG_PrintText("多路模式:1-3路"); 
        else if (g_gMulSwd.MultiMode == 4) PG_PrintText("多路模式:1-4路");         
    }     
         
    /* V1.36 解决第一次上电第1次烧录失败问题 */
    {
        if (g_gMulSwd.MultiMode == 0)
        {            
            swd_init();
        }
        else
        {
            MUL_PORT_SWD_SETUP();
        }
    }
    
    g_tProg.AutoStart = 0;
    fRefresh = 1;
    while (g_MainStatus == MS_PROG_WORK)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

			{
				btn1.Left = BTN1_X;
				btn1.Top = BTN1_Y;
				btn1.Height = BTN1_H;
				btn1.Width = BTN1_W;
				btn1.pCaption = "开始烧录";
				btn1.Font =  &tFontBtn;
				btn1.Focus = 0;

				btn2.Left = BTN2_X;
				btn2.Top = BTN2_Y;
				btn2.Height = BTN2_H;
				btn2.Width = BTN2_W;
				btn2.pCaption = "选择文件";
				btn2.Font =  &tFontBtn;
				btn2.Focus = 0;

				btn3.Left = BTN3_X;
				btn3.Top = BTN3_Y;
				btn3.Height = BTN3_H;
				btn3.Width = BTN3_W;
				btn3.pCaption = "烧录设置";
				btn3.Font =  &tFontBtn;
				btn3.Focus = 0;
				
				if (cursor == 0) btn1.Focus = 1;
				else if (cursor == 1) btn2.Focus = 1;
				else if (cursor == 2) btn3.Focus = 1;
				
				LCD_DrawButton(&btn1);
				LCD_DrawButton(&btn2);
				LCD_DrawButton(&btn3);
			} 			
        }

        DispProgVoltCurrent();

        bsp_Idle();                        
        
        if (fRunOnce == 1)
        {
            fRunOnce = 0;          
            
            PERIOD_Start(&g_tRunLed, 100, 100, 0);    /* 烧录过程LED快速闪烁 */
            
//            DEBUG_D2_TRIG();    /* 测试执行时间 - 编程开始 */
            
            /* 读ini配置文件 */
            ReadProgIniFile(g_tProg.FilePath, &g_tProgIni);
            DispProgCounter();  

            /* 判断剩余次数 */
            {
                uint8_t want;
                
                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                {
                    want = g_tParam.MultiProgMode;
                }
                else
                {
                    want = 1;
                }
                    
                if (g_tProgIni.Locked == 1)
                {
                    BEEP_Start(5, 5, 3);    /* 错误提示音 */
                    PG_PrintText("烧录功能已锁死");
                    continue;
                }
                
                if (g_tProgIni.ProgramLimit > 0 && g_tProgIni.ProgrammedCount + want > g_tProgIni.ProgramLimit)
                {           
                    BEEP_Start(5, 5, 3);    /* 错误提示音 */
                    PG_PrintText("剩余次数不足");
                    continue;
                }
            }               

            g_tProg.Time = bsp_GetRunTime();    /* 记录开始时间, 在start_prog()脚本中会重置开始时间 */
            g_tProg.Err = 100;
            if (g_Lua > 0)
            {
                const char *ret_str;
                /* 
                    执行lua中的函数start_prog()。会阻塞只到编程完毕。  
                    lua_if.c 中的钩子函数LuaYeildHook()实现长按S键终止lua执行。
                    界面的绘制由编程函数内部负责刷新
                */
                PG_PrintText("开始烧录...");
                
                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                {
                    g_gMulSwd.MultiMode = g_tParam.MultiProgMode;

                    if (g_tParam.MultiProgMode == 0)
                    {
                        g_gMulSwd.Active[0] = 0;
                        g_gMulSwd.Active[1] = 0;
                        g_gMulSwd.Active[2] = 0;
                        g_gMulSwd.Active[3] = 0;
                    }
                    else if (g_tParam.MultiProgMode == 1)
                    {
                        g_gMulSwd.Active[0] = 1;
                        g_gMulSwd.Active[1] = 0;
                        g_gMulSwd.Active[2] = 0;
                        g_gMulSwd.Active[3] = 0;
                    }
                    else if (g_tParam.MultiProgMode == 2)
                    {
                        g_gMulSwd.Active[0] = 1;
                        g_gMulSwd.Active[1] = 1;
                        g_gMulSwd.Active[2] = 0;
                        g_gMulSwd.Active[3] = 0;
                    }
                    else if (g_tParam.MultiProgMode == 3)
                    {
                        g_gMulSwd.Active[0] = 1;
                        g_gMulSwd.Active[1] = 1;
                        g_gMulSwd.Active[2] = 1;
                        g_gMulSwd.Active[3] = 0;
                    }
                    else if (g_tParam.MultiProgMode == 4)
                    {
                        g_gMulSwd.Active[0] = 1;
                        g_gMulSwd.Active[1] = 1;
                        g_gMulSwd.Active[2] = 1;
                        g_gMulSwd.Active[3] = 1;
                    }                                
                }               
            
                bsp_LcdSleepEnable(0);      /* 临时屏蔽LCD背光控制，应对烧录时间大于1分钟的情况，避免中途关闭背光 */    
                
                lua_do("ret_str = start_prog()");   /* 执行编程，阻塞只到编程完毕 */         

                bsp_LcdSleepEnable(1);      /* 恢复LCD背光控制 */                    

                {                    
                    lua_getglobal(g_Lua, "ret_str"); 
                    if (lua_isstring(g_Lua, -1))
                    {
                        ret_str = lua_tostring(g_Lua, -1); 
                    }
                    else
                    {
                        ret_str = "";
                    }
                    lua_pop(g_Lua, 1);
                    
                    if (strcmp(ret_str, "OK") == 0)
                    {
                        g_tProg.Err = 0;
                    }
                    else
                    {
                        g_tProg.Err = 1;                        
                    }
                }              
                
                /* 编程完毕 */                
                if (g_tProg.Err == 0)
                {
                    PERIOD_Start(&g_tRunLed, 1000, 0, 0);   /* 烧录成功 LED常亮 */                
                    
                    /* 累加计数器并写入ini文件 */
                    if (g_gMulSwd.MultiMode > 0)
                    {
                        uint8_t i;
                        
                        for (i = 0; i < 4; i++)
                        {
                            if (g_gMulSwd.Active[i] == 1)
                            {
                                g_tProg.NowProgCount++;                 /* 当前次数加1，掉电会清零 */
                                g_tProgIni.ProgrammedCount++;           /* 已编程计数器加1 */
                            }
                        }
                    }
                    else
                    {
                        g_tProg.NowProgCount++;                 /* 当前次数加1，掉电会清零 */
                        g_tProgIni.ProgrammedCount++;           /* 已编程计数器加1 */                        
                    }
                        
                    WriteProgIniFile(g_tProg.FilePath, &g_tProgIni);    /* 写入ini文件 */                    
                    
                    /* 读ini配置文件 */
                    ReadProgIniFile(g_tProg.FilePath, &g_tProgIni);
                    DispProgCounter();      /* 显示烧录计数器 */

                    /* 连续自动烧录 */
                    if (g_tProg.AutoStart == 1)
                    {
                        ucAutoState = 1;
                        
                        iDetectTime = bsp_GetRunTime();
                        
                        PG_PrintText("烧录成功,等待移除");
                    }
                    else
                    {
                        ucAutoState = 0;
                    }    
                }  
                else    /* 烧录失败 */
                {
                    PERIOD_Stop(&g_tRunLed);        /* 烧录失败，LED熄灭 */                   
                    
                    /* 连续自动烧录 */
                    if (g_tProg.AutoStart == 1)
                    {
                        ucAutoState = 1;
                        
                        iDetectTime = bsp_GetRunTime();
                        
                        PG_PrintText("烧录失败,等待移除");
                    }
                    
                    /* 出错时，进度条变为红色 */
                    ProgressBarSetColor(CL_RED);                /* 红色进度 */
                    if (g_tProg.Percent < 10)
                    {
                        g_tProg.Percent = 10;
                    }
                    DispProgProgress(0, g_tProg.Percent, 0xFFFFFFFE);   /* 0xFFFF FFFE 表示维持之前的地址 */
                    ProgressBarSetColor(PROGRESS_BACK_COLOR1);  /* 恢复缺省颜色 */
                }
            }
            else
            {
                BEEP_Start(5, 5, 3);    /* 错误提示音 */
  
                LCD_DispStrEx(PROG_STEP_X, PROG_STEP_Y,  "请选择文件", &tFontText, 120, 0);
            }
        }
        
        if (g_tProg.AutoStart == 1)
        {
            if (ucAutoState == 1)           /* 连续烧录状态，等待移除 */
            {                
                if (bsp_CheckRunTime(iDetectTime) > 30)
                {
                    iDetectTime = bsp_GetRunTime();
                    if (WaitChipRemove())           /* 等待芯片移除 */
                    {
                        if (++ucDetectCount > 3)    /* 连续3次读取失败 */
                        {
                            ucDetectCount = 0;
                            
                            ucAutoState = 2;
                            
                            if (g_tProg.Err == 0)
                            {
                                PG_PrintText("等待插入");
                            }
                            else
                            {
                                PG_PrintText("上次失败,等待插入");
                            }                       
                            
                            bsp_PutKey(KEY_DB_S);   /* 任意发一个本状态无用的按键消息，避免背光超时关闭 */
                        }
                    }
                    else
                    {
                        ucDetectCount = 0;
                    }
                }
            }
            else if (ucAutoState == 2)              /* 连续烧录状态，等待插入 */
            {
                if (bsp_CheckRunTime(iDetectTime) > 30)
                {
                    iDetectTime = bsp_GetRunTime();
                                   
                    if (WaitChipInsert())           /* 等待芯片插入 */
                    {
                        if (++ucDetectCount > 3)    /* 连续3次 300ms */
                        {
                            ucAutoState = 0;
                            fRunOnce = 1;
                            bsp_PutKey(KEY_DB_S);   /* 任意发一个本状态无用的按键消息，重开背光 */
                        }
                    }
                    else
                    {
//                        if (g_tProg.Err == 0)
//                        {
//                            PG_PrintText("等待插入");
//                        }
//                        else
//                        {
//                            PG_PrintText("上次失败,等待插入");
//                        } 
                            
                        ucDetectCount = 0;
                    }
                }
            }
            else
            {
                ;
            }
        }
        else
        {
            ;
        }
        
        /* 处理消息. 和PC机或lua程序传递信息 */
        {
            static int32_t s_time = 0;
            
            MSG_T msg;            
            
            if (bsp_GetMsg(&msg))
            {
                switch (msg.MsgCode)
                {
                    case MSG_PG_START:
                        fRunOnce = 1;
                        bsp_PutKey(KEY_DB_S);   /* 任意发一个本状态无用的按键消息，重开背光 */
                        break;
                    
                    case MSG_PG_ABORT:
                        break;
                }
            }
            
            if (bsp_CheckRunTime(s_time) > 20)
            {
                s_time = bsp_GetRunTime();
                lua_do("if mi_idle ~= nil then mi_idle() end");   /* 烧录空闲时刻，检测启动键，通过lua实现 */
            }
        }
        
          
 
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:      /* S键释放 - 移动按钮焦点*/    
                    if (++cursor == 3)
                    {
                        cursor = 0;
                    }
                    if (g_tProg.AutoStart == 1)
                    {
                        g_tProg.AutoStart = 0;
                        PG_PrintText("退出连续烧录");
                    }
                    fRefresh = 1;
                    break;

                case KEY_UP_C:      /* C键释放 - 确认执行按钮功能 */ 
                    if (cursor == 0)
                    {
                        /* 烧录一次 */
                        g_tProg.AutoStart = 0;
                        fRunOnce = 1;
                    }			
                    break;

                case KEY_LONG_DOWN_S:     /* S键长按 - 确认后，闪烁，修改参数 */      
                    if (cursor == 0)
                    {
                        g_tProg.AutoStart = 1;  /* 连续烧录 */
                        fRunOnce = 1;
                    }
                    else if (cursor == 1)
                    {
                        PlayKeyTone();
                        g_MainStatus = MS_PROG_SELECT_FILE;
                    }
                    else if (cursor == 2)
                    {
                        PlayKeyTone();
                        g_MainStatus = MS_PROG_SETTING;
                    }	                
                    break;
                
                case KEY_LONG_DOWN_C:    /* C键长按 */                    
                    if (g_tParam.StartRun == 1)
                    {
                        g_MainStatus = MS_PROG_WORK;
                    }
                    else if (g_tParam.StartRun == 2)
                    {
                        g_MainStatus = MS_PROG_WORK;
                    }
                    else
                    {
                        g_MainStatus = MS_EXTEND_MENU1;
                    }
                    break;

                default:
                    break;
            }
        }
    }
    
    PERIOD_Start(&g_tRunLed, 1000, 1000, 0);    /* LED一直闪烁, 每2秒闪1次 */
}

/*
*********************************************************************************************************
*    函 数 名: ProgFinishedCallBack
*    功能说明: 显示编程进度
*    形    参:  _ErrCode 编程结果
*    返 回 值: 无
*********************************************************************************************************
*/
void ProgFinishedCallBack(uint8_t _ErrCode)
{
    g_tProg.Err = _ErrCode;
}

/*
*********************************************************************************************************
*    函 数 名: DispProgProgress
*    功能说明: 显示编程进度
*    形    参:  _str :  文本提示. 0表示不刷新显示
*               _progress : 进度百分比， -1表示不刷新
*               _addr :  内存地址，0xFFFFFFFF表示不显示
*    返 回 值: 无
*********************************************************************************************************
*/
void DispProgProgress(char *_str, float _progress, uint32_t _addr)
{    
    FONT_T tFont16;
    static uint32_t s_LastAddress = 0;
    
    /* 设置字体参数 */
    {
        tFont16.FontCode = FC_ST_16;          /* 字体代码 16点阵 */
        tFont16.FrontColor = INFO_NAME_COLOR; /* 字体颜色 */
        tFont16.BackColor = FORM_BACK_COLOR;  /* 文字背景颜色 */
        tFont16.Space = 0;                    /* 文字间距，单位 = 像素 */ 
    } 
          
    {
        char buf1[32];
        char buf2[32];
        uint32_t t1;     
        
        if (g_tProg.Time > 0)  
        {
            t1 = bsp_CheckRunTime(g_tProg.Time);
            
            /* 烧录时间 */
            sprintf(buf2, "%3d.%02d 秒", t1 / 1000, (t1 % 1000) / 10);
        }
        else
        {
            buf2[0] = 0;
        }
        
        /* 烧录步骤提示 */
        if (_str > 0)
        {
            LCD_SetEncode(ENCODE_GBK);
            LCD_DispStrEx(PROG_STEP_X, PROG_STEP_Y,  _str, &tFont16, 240 - 10, 0);
            LCD_SetEncode(ENCODE_UTF8);
        }
        
        /* 烧录进度条 */
        if (_progress > 0)
        {
            if (_addr == 0xFFFFFFFF)        /* 不显示地址 */
            {
                buf1[0] = 0;
            }
            else if (_addr == 0xFFFFFFFE)   /* 用于显示出错的地址 */
            {
                sprintf(buf1, "0x%08X", s_LastAddress);
            }
            else    /* 显示地址 */
            {
                sprintf(buf1, "0x%08X", _addr);
                s_LastAddress = _addr;
            }
            
            tFont16.BackColor = CL_MASK;        
            DispProgressBar(PROGRESS_X, PROGRESS_Y, 24, 230, buf1, _progress, buf2, &tFont16);             
            tFont16.BackColor = FORM_BACK_COLOR;  /* 文字背景颜色 */
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: status_ProgSetting
*    功能说明: 选择烧录器参数配置
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_ProgSetting(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh;
    static uint8_t s_MenuInit = 0;
    
    DispHeader("烧录器参数");
//    DispHelpBar("",
//                ""); 
    
    if (s_MenuInit == 0)
    {
        s_MenuInit = 1;
        
        g_tMenuProg1.Left = MENU_LEFT;
        g_tMenuProg1.Top = MENU_TOP;
        g_tMenuProg1.Height = MENU_HEIGHT;
        g_tMenuProg1.Width = MENU_WIDTH;
        g_tMenuProg1.LineCap = MENU_CAP;
        g_tMenuProg1.ViewLine = 8;
        g_tMenuProg1.Font.FontCode = FC_ST_24;
        g_tMenuProg1.Font.Space = 0;
        g_tMenuProg1.RollBackEn = 1;  /* 允许回滚 */
        g_tMenuProg1.GBK = 0;
        g_tMenuProg1.ActiveBackColor = 0;   /* 选中行背景色ID */
        LCD_InitMenu(&g_tMenuProg1, (char **)g_MenuProg1_Text); /* 初始化菜单结构 */
    }    
    LCD_DispMenu(&g_tMenuProg1);

    fRefresh = 1;
    while (g_MainStatus == MS_PROG_SETTING)
    {
        if (fRefresh) /* 刷新整个界面 */
        {
            fRefresh = 0;

            if (g_tMenuProg1.Cursor == 0)
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
                case KEY_UP_S: /* S键 上 */
                    LCD_MoveUpMenu(&g_tMenuProg1);
                    break;

                case KEY_LONG_DOWN_S: /* S键 上 */
                    if (g_tMenuProg1.Cursor == 0)      /* 修改编程参数 */
                    {
                        //g_MainStatus = MS_PROG_MODIFY_PARAM);;
                        
                        ModifyParam(MODIFY_PARAM_PROG);
                        LCD_DispMenu(&g_tMenuProg1);
                        /* 通知lua程序，多路编程参数变化 */
                        lua_do("MULTI_MODE = pg_read_c_var(\"MultiProgMode\")");
                    }                    
                    else if (g_tMenuProg1.Cursor == 1)      /* 本次计数清零 */
                    {
                        g_tProg.NowProgCount = 0;
                        g_MainStatus = MS_PROG_WORK;
                    }
                    else if (g_tMenuProg1.Cursor == 2)      /* 累积次数清零 */
                    {
                        /* 累积次数，无限制时才允许手动清零 */
                        if (g_tProgIni.ProgramLimit == 0)
                        {
                            g_tProgIni.ProgrammedCount = 0;                     /* 已编程计数器加1 */                    
                            WriteProgIniFile(g_tProg.FilePath, &g_tProgIni);    /* 写入ini文件 */ 
                        }
                        else
                        {
                            /* 弹窗显示信息 */
                            DispMsgBox(20, 180, 20, 200, "请通过U盘模式更改");
                            bsp_DelayMS(2000);
                            LCD_DispMenu(&g_tMenuProg1);                            
                        }
                        g_MainStatus = MS_PROG_WORK;
                    }   
                    else if (g_tMenuProg1.Cursor == 3)      /* 输入产品序号 */
                    {
                        //g_MainStatus = MS_SYSTEM_SET;
                    }                     
                    break;

                case KEY_UP_C: /* C键 下 */
                    LCD_MoveDownMenu(&g_tMenuProg1);
                    break;

                case KEY_LONG_DOWN_C: /* C键长按 */
                    PlayKeyTone();
                
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
                    else
                    {
                        g_MainStatus = MS_PROG_WORK;
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: status_ProgModifyParam
*    功能说明: 修改复位类型等参数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
#define PARAM_NUM  4
void status_ProgModifyParam(void)
{
    uint8_t ucKeyCode; /* 按键代码 */
    uint8_t fRefresh = 1;
    uint8_t fSaveParam = 0;
    uint8_t cursor = 0;
    char buf[48];
    uint8_t active;

    DispHeader2(93, "烧录参数");
    DispHelpBar("长按S键选择参数",
                "短按S、C键修改参数值");
    
    while (g_MainStatus == MS_PROG_MODIFY_PARAM)
    {
        bsp_Idle();

        if (fRefresh == 1)
        {
            fRefresh = 0;
            
           /* 第1个参数 - 多路模式  */
            {
                if (cursor == 0)
                {
                    active = 1;       
                }
                else 
                {
                    active = 0;
                }
                
                if (g_tParam.MultiProgMode == 1)
                {
                    DispParamBar(0, "多路模式:", "1路", active);
                }
                else if (g_tParam.MultiProgMode == 2)
                {
                    DispParamBar(0, "多路模式:", "1-2路", active);
                }
                else if (g_tParam.MultiProgMode == 3)
                {
                    DispParamBar(0, "多路模式:", "1-3路", active);
                } 
                else if (g_tParam.MultiProgMode == 4)
                {
                    DispParamBar(0, "多路模式:", "1-4路", active);
                }
                else
                {
                    DispParamBar(0, "多路模式:", "关闭", active);
                }                               
            }   
            
            /* 第2个参数 - 保留 */
            {
                if (cursor == 1)
                {
                    active = 1;       
                }
                else 
                {
                    active = 0;
                }
                
                sprintf(buf, "%d", g_tParam.FactoryId);
                DispParamBar(1, "工厂代码:", buf, active);             
            }           

             /* 第3个参数 - 编程参数3  */
            {
                if (cursor == 2)
                {
                    active = 1;       
                }
                else 
                {
                    active = 0;
                }
                
                sprintf(buf, "%d", g_tParam.ToolSn);
                DispParamBar(2, "烧录器编号:", buf, active);
            }      
 
            /* 第4个参数 - 开机启动  */
            {
                if (cursor == 3)
                {
                    active = 1;       
                }
                else 
                {
                    active = 0;
                }
                
                if (g_tParam.StartRun == 1)
                {
                    DispParamBar(3, "开机启动:", "单路烧录", active);
                }
                else if (g_tParam.StartRun == 2)
                {
                    DispParamBar(3, "开机启动:", "多路烧录", active);
                }
                else
                {
                    DispParamBar(3, "开机启动:", "缺省", active);
                }                 
            }            
        }
        
        ucKeyCode = bsp_GetKey(); /* 读取键值, 无键按下时返回 KEY_NONE = 0 */
        if (ucKeyCode != KEY_NONE)
        {
            /* 有键按下 */
            switch (ucKeyCode)
            {
                case KEY_UP_S:      /* S键 弹起 */
                    if (cursor == 0)
                    {
                        if (g_tParam.MultiProgMode < 4)
                        {
                            g_tParam.MultiProgMode++;
                        }
                        else
                        {
                            g_tParam.MultiProgMode = 0;
                        }                         
                    }
                    else if (cursor == 1)
                    {
                        if (g_tParam.FactoryId < 1 || g_tParam.FactoryId > 99)
                        {
                            g_tParam.FactoryId = 1;
                        }
                        else
                        {
                            if (g_tParam.FactoryId < 99)
                            {
                                g_tParam.FactoryId++;
                            }
                            else
                            {
                                g_tParam.FactoryId = 1;
                            }
                        }                     
                    }     
                    else if (cursor == 2)
                    {
                        if (g_tParam.ToolSn < 1 || g_tParam.ToolSn > 99)
                        {
                            g_tParam.ToolSn = 1;
                        }
                        else
                        {
                            if (g_tParam.ToolSn < 99)
                            {
                                g_tParam.ToolSn++;
                            }
                            else
                            {
                                g_tParam.ToolSn = 1;
                            }
                        }
                    }
                    else if (cursor == 3)
                    {
                        if (++g_tParam.StartRun >= 3)
                        {
                            g_tParam.StartRun = 0;
                        }
                    }                    
                    fRefresh = 1;
                    fSaveParam = 1;
                    break;

                case KEY_UP_C:      /* C键 下 */
                    if (cursor == 0)
                    {        
                        if (g_tParam.MultiProgMode > 0 && g_tParam.MultiProgMode < 5)
                        {
                            g_tParam.MultiProgMode--;
                        }
                        else
                        {
                            g_tParam.MultiProgMode = 4;
                        }
                    }
                    else if (cursor == 1)
                    {
                        if (g_tParam.FactoryId < 1 || g_tParam.FactoryId > 99)
                        {
                            g_tParam.FactoryId = 1;
                        }
                        else
                        {
                            if (g_tParam.FactoryId > 1)
                            {
                                g_tParam.FactoryId--;
                            }
                            else
                            {
                                g_tParam.ToolSn = 99;
                            }
                        }
                    } 
                    else if (cursor == 2)
                    {
                        if (g_tParam.ToolSn < 1 || g_tParam.ToolSn > 99)
                        {
                            g_tParam.ToolSn = 1;
                        }
                        else
                        {
                            if (g_tParam.ToolSn > 1)
                            {
                                g_tParam.ToolSn--;
                            }
                            else
                            {
                                g_tParam.ToolSn = 99;
                            }
                        }
                    } 
                    else if (cursor == 3)
                    {
                        if (g_tParam.StartRun == 0)
                        {
                            g_tParam.StartRun = 2;
                        }
                        else if (g_tParam.StartRun > 2)
                        {
                            g_tParam.StartRun = 1;
                        }
                        else
                        {
                            g_tParam.StartRun--;
                        }                         
                    }                     
                    fRefresh = 1;
                    fSaveParam = 1;
                    break;

                case KEY_LONG_DOWN_S:        /* S键长按 - 选择参数 */
                    if (++cursor >= PARAM_NUM)
                    {
                        cursor = 0;
                    }
                    fRefresh = 1;
                    break;

                case KEY_LONG_DOWN_C:        /* C键长按 - 返回 */
                    PlayKeyTone();
                    g_MainStatus = MS_PROG_SETTING;
                    break;

                default:
                    break;
            }
        }
    }
    
    if (fSaveParam == 1)
    {
        SaveParam();    /* 保存参数 */
        
        /* 通知lua程序，多路编程参数变化 */
        lua_do("MULTI_MODE = pg_read_c_var(\"MultiProgMode\")");
    }
}

/*
*********************************************************************************************************
*    函 数 名: DispProgCounter
*    功能说明: 显示烧录次数等信息
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispProgCounter(void)
{
    char buf[32];
    FONT_T tFontText;
    FONT_T tFont2;

    tFontText.FontCode = FC_ST_16;              /* 字体代码 16点阵 */
    tFontText.FrontColor = INFO_NAME_COLOR;     /* 字体颜色 */
    tFontText.BackColor = FORM_BACK_COLOR;      /* 文字背景颜色 */      
    tFontText.Space = 0;                        /* 文字间距，单位 = 像素 */       

    tFont2.FontCode = FC_ST_32;    /* 字体代码 16点阵 */
    tFont2.FrontColor = CL_WHITE;  /* 字体颜色 */
    tFont2.BackColor = HEAD_SN_COLOR;    /* 文字背景颜色 */
    tFont2.Space = 0;              /* 文字间距，单位 = 像素 */ 
    
    /* 显示本次计数 */
    LCD_DispStrEx(TEXT1_X,    TEXT1_Y, "本次", &tFontText,  TEXT_WIDTH, ALIGN_LEFT);
    LCD_DispStrEx(TEXT2_X,    TEXT2_Y, "计数", &tFontText,  TEXT_WIDTH, ALIGN_LEFT);    
    sprintf(buf, "%05d", g_tProg.NowProgCount);    /* 最大 99999 */
    LCD_DispStrEx(TEXT1_X + 38,    TEXT1_Y + 2, buf, &tFont2,  16 * 6, ALIGN_CENTER);
    
    /* 
        显示剩余次数+累积次数
        累计 20000 / 60000
    
    */
//    if (g_tProgIni.Locked == 1)
//    {
//        sprintf(buf, "已锁死");
//    }
//    else 
//    {
//        count = g_tProgIni.ProgramLimit - g_tProgIni.ProgrammedCount;
//    }
    if (g_tProgIni.ProgramLimit == 0)
    {
        sprintf(buf, "累计: %d", g_tProgIni.ProgrammedCount);
    }
    else
    {
        sprintf(buf, "累计: %d/%d", g_tProgIni.ProgrammedCount, g_tProgIni.ProgramLimit);
    }         
    LCD_DispStrEx(TEXT3_X,    TEXT3_Y, buf, &tFontText,  TEXT_WIDTH, ALIGN_LEFT); 
    
    sprintf(buf, "校验: 0x%08X", s_DataFileSum);
         
    LCD_DispStrEx(TEXT4_X,    TEXT4_Y, buf, &tFontText,  TEXT_WIDTH, ALIGN_LEFT);     
}

/*
*********************************************************************************************************
*    函 数 名: DispProgVoltCurrent
*    功能说明: 显示TVCC电压电流。 每隔300ms刷新一次。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DispProgVoltCurrent(void)
{
    FONT_T tFont16;
    char buf[32];
    static int32_t s_last_time = 0;
    
    if (bsp_CheckRunTime(s_last_time) < 300)
    {
        return;
    }
    
    s_last_time = bsp_GetRunTime();
    
    /* 设置字体参数 */
    {
        tFont16.FontCode = FC_ST_16;            /* 字体代码 16点阵 */
        tFont16.FrontColor = RGB(50,50,50);     /* 字体颜色 */
        tFont16.BackColor = RGB(254, 120, 128); /* 文字背景颜色 */
        tFont16.Space = 0;                      /* 文字间距，单位 = 像素 */ 
    } 
    
    tFont16.BackColor = RGB(255, 220, 41);      /* 文字背景颜色 */    
    sprintf(buf, "%0.3fV", g_tVar.TVCCVolt);
    LCD_DispStrEx(TEXT_VOLT_X, TEXT_VOLT_Y, buf, &tFont16, TEXT_VOLT_W, 1);  

    tFont16.BackColor = RGB(150, 240, 40);      /* 文字背景颜色 */
    sprintf(buf, "%0.0fmA", g_tVar.TVCCCurr);
    LCD_DispStrEx(TEXT_CURR_X, TEXT_CURR_Y, buf, &tFont16, TEXT_CURR_W, 1);         
}    
/*
*********************************************************************************************************
*    函 数 名: GetDataFileSum
*    功能说明: 获取数据文件校验和. 通过STM32硬件CRC32单元计算. 多个文件时，每个文件单独计算CRC32,然后累加。
*    形    参: 无
*    返 回 值: CRC32
*********************************************************************************************************
*/
#define MAX_FILE_SUM 128        /* 最大文件个数 */
static uint32_t GetDataFileSum(void)
{
#if 1   /* 由PC机计算 */
    uint32_t crc_sum; 
    
    crc_sum = lua_GetVarUint32("DATA_FILE_CHECK_SUMS", 0x00000000);
    
    return crc_sum;
#else   /* 由H7-TOOL计算，数据文件2MB时，0.5秒。如果更大文件，则载入界面太慢，废弃 */
    size_t FileLen;
    uint32_t i;
    const char *pFileName;
    uint32_t crc_sum;   /* 多个数据文件，分别计算CRC32，然后求和 */
    uint32_t crc32;
    char path[256];

    /*
      TaskList = {
        AlgoFile_FLASH, --算法文件
        "2K.bin",       --数据文件
        0x00000000,     --目标地址
        1,  --0表示按扇区擦除, 1表示整片擦除
      }
    */
    
    crc_sum = 0;
    //lua->stack，得到全局表，位置-1
    lua_getglobal(g_Lua, "TaskList");
    for (i = 0; i < MAX_FILE_SUM; i++)
    {
        lua_pushinteger(g_Lua, i * 4 + 2);  //c->statck，设置key值，位置-1(上面的-1变为-2)
        lua_gettable(g_Lua, -2);//返回值为值的类型        
        if (lua_isstring(g_Lua, -1))
        {
            pFileName = luaL_checklstring(g_Lua, -1, &FileLen); /* 1是参数的位置， len是string的长度 */              
            lua_pop(g_Lua, 1);            
 
            if (pFileName[0] == '0' && pFileName[1] == ':')         /* 是绝对路径 */
            {
                strncpy(path, pFileName, sizeof(pFileName));
            }
            else    /* 是相对路路径 */ 
            {
                GetDirOfFileName(g_tProg.FilePath, path);   /* 从lua文件名、中获取目录 */
                strcat(path, "/");
                strcat(path, pFileName);

                FixFileName(path);  /* 去掉路径中的..上级目录 */
            }
    
            GetFileCRC32(path, &crc32);
            crc_sum += crc32;
        }
        else
        {
            lua_pop(g_Lua, 1);
            break;            
        }
    }
    
    lua_pop(g_Lua, 1);
    return crc_sum;
#endif

}   
    
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
