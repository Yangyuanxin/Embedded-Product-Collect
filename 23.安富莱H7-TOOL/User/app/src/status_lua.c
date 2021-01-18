/*
*********************************************************************************************************
*
*    模块名称 : lua小程序
*    文件名称 : status_lua.c
*    版    本 : V1.0
*    说    明 : lua小程序执行状态
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
#include "file_lib.h"
#include "lua_if.h"
#include "prog_if.h"
/*
*********************************************************************************************************
*    函 数 名: status_LuaSelectFile
*    功能说明: lua界面，选择文件。 - 文件浏览器
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_LuaSelectFile(void)
{
    SelectFile(LUA_ROOT_DIR, MS_LUA_SELECT_FILE, MS_EXTEND_MENU1, "*.lua");
}

/*
*********************************************************************************************************
*    函 数 名: status_LuaRun
*    功能说明: lua界面，执行文件。 g_tFileList.Path
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
/* 按钮坐标定义 */
#define BTN1_X     5
#define BTN1_Y     40
#define BTN1_H     28 
#define BTN1_W     110 
#define BTN1_TEXT  "F1"

#define BTN2_X     BTN1_X
#define BTN2_Y     BTN1_Y + BTN1_H + 5 
#define BTN2_H     BTN1_H 
#define BTN2_W     BTN1_W 
#define BTN2_TEXT  "F2"

#define BTN3_X     BTN1_X
#define BTN3_Y     BTN1_Y + 2 * (BTN1_H + 5)
#define BTN3_H     BTN1_H 
#define BTN3_W     BTN1_W 
#define BTN3_TEXT  "F3"

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

#define BTN_NUM         6
#define BTN_TEXT_LEN   16

/* 多行文本框 */
#define MEMO_X     5
#define MEMO_Y     140 
#define MEMO_H     (240 - MEMO_Y - 5)  
#define MEMO_W     (240 - 2 * MEMO_X)
typedef struct
{
    char Note[32];
    char Cmd[32];
}CMD_LIST_T;

CMD_LIST_T tBtnList[16];

char g_OutText[4 * 1024] = {0}; /* 最多支持4K字节文本缓存 */
MEMO_T g_LuaMemo = {0};

uint8_t g_LuaSubStatus = 0;     /* 0表示命令行界面  1表示GUI界面 */

/* 
char s_lua_prog_buf[LUA_PROG_LEN_MAX + 1];
uint32_t s_lua_prog_len;

根据LUA文件查找按钮名字和执行指令
*/
void FindBtnNoteCmd(void)
{
	/* 解析文件，填充按钮提示栏 */
	{
        uint16_t i,j,k;
		char *pText;
		char *p;
		char head[12];
		char strNote[128];
		char strCmd[128];

		pText = s_lua_prog_buf;
		for (i = 0; i < 16; i++)
		{
			sprintf(head, "--F%02d=", i + 1);

			p = strstr(pText, head);
			if (p > 0)
			{
				p += 6;
				for (j = 0; j < 128; j++)
				{
					strNote[j] = p[j];
					if (p[j] == ',')
					{
						strNote[j] = 0;
						break;
					}
				}
				strNote[j] = 0;

				j++;
				for (k = 0; j < 128; j++,k++)
				{
					strCmd[k] = p[j];
					if (p[j] == 0 || p[j] == 0x0D || p[j] == 0x0A)
					{
						strCmd[k] = 0;
						break;
					}
				}
				strCmd[j] = 0;

				strcpy(tBtnList[i].Note, strNote);
				strcpy(tBtnList[i].Cmd, strCmd);
			}
			else
			{
				strcpy(tBtnList[i].Note, "");
				strcpy(tBtnList[i].Cmd, "");
			}
		}
	}
}

void status_LuaRun(void)
{
    lua_DownLoadFile(g_tFileList.Path);  /* 重新初始化lua环境，并装载lua文件到内存，不执行 */  
    
    /* --GUIMODE=1   --用于识别GUI程序还是print程序 */
    if (memcmp(s_lua_prog_buf, "--GUIMODE=1", 11) == 0)
    {
        g_LuaSubStatus = 1;
    }
    else
    {
        g_LuaSubStatus = 0;
    }
    
    if (g_LuaSubStatus == 1)    /* GUI界面 --需要是死循环 */
    {
        lua_RunLuaProg();   /* 执行lua -阻塞 */
        
        while(bsp_GetKey());
        g_MainStatus = MS_LUA_SELECT_FILE;               
    }
    else    /* 命令行界面 -必须是非阻塞的 */
    {
        uint8_t ucKeyCode; /* 按键代码 */
        uint8_t fRefresh;
        FONT_T tFontBtn, tFontMemo;
        uint8_t cursor = 0;
        BUTTON_T btn1, btn2, btn3, btn4, btn5, btn6;
                    
        DispHeader("Lua小程序");
        
        {
            g_LuaMemo.Left = MEMO_X;
            g_LuaMemo.Top = MEMO_Y;
            g_LuaMemo.Height = MEMO_H;
            g_LuaMemo.Width = MEMO_W;
            g_LuaMemo.Font = &tFontMemo;
            //g_LuaMemo.Color = CL_WHITE;
            g_LuaMemo.Text = g_OutText;
            g_LuaMemo.MaxLen = sizeof(g_OutText);
            g_LuaMemo.LineCount = 0;        
            g_LuaMemo.WordWrap = 0;
            g_LuaMemo.LineOffset = 0;
            
            LCD_InitMemo(&g_LuaMemo);
            LCD_DrawMemo(&g_LuaMemo);
        }
        
        bsp_Idle();

        /* 设置字体参数 */
        {
            tFontBtn.FontCode = FC_ST_16;           /* 字体代码 16点阵 */
            tFontBtn.FrontColor = INFO_NAME_COLOR;  /* 字体颜色 */
            tFontBtn.BackColor = CL_MASK;           /* 文字背景颜色 */
            tFontBtn.Space = 0;                     /* 文字间距，单位 = 像素 */
            
            tFontMemo.FontCode = FC_ST_16;          /* 字体代码 16点阵 */
            tFontMemo.FrontColor = CL_WHITE;        /* 字体颜色 */
            tFontMemo.BackColor = CL_MASK;          /* 文字背景颜色 */
            tFontMemo.Space = 0;                    /* 文字间距，单位 = 像素 */                
        }     
        
        /* 从lua文件中解析按钮名字和程序语句 */
        FindBtnNoteCmd();
        
        lua_RunLuaProg();   /* 执行lua */    
        GetChipTypeFromLua(g_Lua);                  /* 从lua中解析芯片类型 */        
        
        fRefresh = 1;
        while (g_MainStatus == MS_LUA_EXEC_FILE)
        {
            if (fRefresh) /* 刷新整个界面 */
            {
                fRefresh = 0;

                {
                    btn1.Left = BTN1_X;
                    btn1.Top = BTN1_Y;
                    btn1.Height = BTN1_H;
                    btn1.Width = BTN1_W;
                    btn1.pCaption = tBtnList[0].Note;
                    btn1.Font =  &tFontBtn;
                    btn1.Focus = 0;

                    btn2.Left = BTN2_X;
                    btn2.Top = BTN2_Y;
                    btn2.Height = BTN2_H;
                    btn2.Width = BTN2_W;
                    btn2.pCaption = tBtnList[1].Note;
                    btn2.Font =  &tFontBtn;
                    btn2.Focus = 0;

                    btn3.Left = BTN3_X;
                    btn3.Top = BTN3_Y;
                    btn3.Height = BTN3_H;
                    btn3.Width = BTN3_W;
                    btn3.pCaption = tBtnList[2].Note;
                    btn3.Font =  &tFontBtn;
                    btn3.Focus = 0;

                    btn4.Left = BTN4_X;
                    btn4.Top = BTN4_Y;
                    btn4.Height = BTN4_H;
                    btn4.Width = BTN4_W;
                    btn4.pCaption = tBtnList[3].Note;
                    btn4.Font =  &tFontBtn;
                    btn4.Focus = 0;
                    
                    btn5.Left = BTN5_X;
                    btn5.Top = BTN5_Y;
                    btn5.Height = BTN5_H;
                    btn5.Width = BTN5_W;
                    btn5.pCaption = tBtnList[4].Note;
                    btn5.Font =  &tFontBtn;
                    btn5.Focus = 0;

                    btn6.Left = BTN6_X;
                    btn6.Top = BTN6_Y;
                    btn6.Height = BTN6_H;
                    btn6.Width = BTN6_W;
                    btn6.pCaption = tBtnList[5].Note;
                    btn6.Font =  &tFontBtn;
                    btn6.Focus = 0;
                    
                    if (cursor == 0) btn1.Focus = 1;
                    else if (cursor == 1) btn2.Focus = 1;
                    else if (cursor == 2) btn3.Focus = 1;
                    else if (cursor == 3) btn4.Focus = 1;
                    else if (cursor == 4) btn5.Focus = 1;
                    else if (cursor == 5) btn6.Focus = 1;
                    
                    LCD_SetEncode(ENCODE_GBK);  /* 按钮文字是GBK编码 */
                    
                    LCD_DrawButton(&btn1);
                    LCD_DrawButton(&btn2);
                    LCD_DrawButton(&btn3);
                    LCD_DrawButton(&btn4);
                    LCD_DrawButton(&btn5);
                    LCD_DrawButton(&btn6);
                    
                    LCD_SetEncode(ENCODE_UTF8); /* 还原UTF8编码 */
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
                        if (++cursor == BTN_NUM)
                        {
                            cursor = 0;
                        }                                      
                        fRefresh = 1;
                        break;

                    case KEY_LONG_DOWN_S:   /* S键 长按 */ 
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

                    case KEY_UP_C:              /* C键 下 */
                        if (cursor < 6)
                        {
                            if (strlen(tBtnList[cursor].Cmd) > 0)
                            {                            
                                LCD_MemoClear(&g_LuaMemo);
                                
                                DispHeaderStr("Lua正在运行...");
                                
                                lua_do(tBtnList[cursor].Cmd);
                                
                                DispHeaderStr("Lua小程序");
                            }
                        }	
                        break;

                    case KEY_LONG_DOWN_C:   /* C键长按 */
                        g_MainStatus = MS_LUA_SELECT_FILE;
                        break;

                    default:
                        break;
                }
            }
        }         
    }    
}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
