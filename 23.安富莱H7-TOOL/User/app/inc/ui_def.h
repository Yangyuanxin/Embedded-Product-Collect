/*
*********************************************************************************************************
*
*    模块名称 : GUI配色模块
*    文件名称 : ui_def.h
*    版    本 : V1.0
*
*    Copyright (C), 2018-2025, 安富莱电子 www.armfly.com
*
********************************************************************************************************
*/

#ifndef __UI_DEF_H_
#define __UI_DEF_H_

/*
    RGB(32, 98, 230)    橙色
    
    RGB(5, 54, 131)     深蓝
    
    RGB(0, 186, 38)     粉绿
    
    RGB(0, 179, 254)    淡蓝
    
    RGB(215, 235, 118)  淡草绿
*/

#define UI_STYLE_NUM            3                   /* 界面风格个数 */

#define FORM_BACK_COLOR         RGB(240, 240, 240)  /* 背景颜色 */

#define HEAD_HEIGHT             32                  /* 标题栏高度 */
#define HEAD_SN_COLOR           RGB(57, 117, 206)   /* 标题序号背景颜色 */
#define HEAD_TEXT_COLOR         RGB(255, 255, 255)  /* 标题文字颜色 */
#define HEAD_BACK_COLOR         RGB(80, 145, 239)   /* 标题背景颜色 */
#define HEAD_BODER_COLOR        RGB(215, 233, 248)  /* 标题栏下边沿颜色 */

#define PROGRESS_TEXT_COLOR     MEAS_NAME_COLOR     /* 进度条文字颜色 */
#define PROGRESS_BACK_COLOR1    RGB(77, 255, 77)    /* 进度条背景颜色1 */
#define PROGRESS_BACK_COLOR2    RGB(255, 255, 255)  /* 进度条背景颜色2 */
#define PROGRESS_BODER_COLOR    MEAS_BODER_COLOR    /* 进度条边沿颜色 */

#define B0X_ARC                 8                   /* 圆角矩形的四角的弧半径 */

#define MEAS_WIN_WIDTH          230                  /* 测量栏坐标 */
#define MEAS_WIN_HEIGHT         32                   /* 测量栏坐标 */
#define MEAS_WIN_LEFT           ((240 - MEAS_WIN_WIDTH) / 2)

#define MEAS_BACK_COLOR         RGB(255, 255, 255)  /* 测量栏背景颜色 */
#define MEAS_BODER_COLOR        RGB(230, 230, 230)  /* 测量栏边框颜色 */
#define MEAS_NAME_COLOR         RGB(140, 140, 140)  /* 测量栏名称颜色 RGB(189, 189, 189) */
#define MEAS_VALUE_COLOR        RGB(102, 156, 247)  /* 测量栏正文颜色 */
#define MEAS_UNIT_COLOR         RGB(140, 140, 140)  /* 测量栏单位颜色 RGB(189, 189, 189) */

#define HELP_TEXT_COLOR         RGB(140, 140, 140)  /* 帮助文字颜色  RGB (200 200 200) */
#define HELP_BACK_COLOR         FORM_BACK_COLOR     /* 帮助文字背景颜色 */

#define INFO_BACK_COLOR         FORM_BACK_COLOR     /* 联机模式正文文字背景颜色 */
#define INFO_NAME_COLOR         RGB(140, 140, 140)  /* 联机模式正文文字颜色 */
#define INFO_VALUE_COLOR        RGB(190, 190, 190)  /* 联机模式正文文字颜色 */
#define INFO_HEIGHT             18                  /* 高度 */

#define CL_MENU_TEXT1           RGB(140, 140, 140)  /* 菜单文字颜色 RGB(189, 189, 189) */
//#define CL_MENU_BACK1           RGB(5, 54, 131)   /* 菜单文字背景颜色 */
#define CL_MENU_BACK1           CL_WHITE            /* 菜单文字背景颜色 */
#define CL_MENU_TEXT2           RGB(140, 140, 140)  /* 光标选中文字颜色 RGB(160, 160, 160) */
#define CL_MENU_BACK2           CL_YELLOW           /* 光标选中文字背景颜色 */
#define CL_MENU_ACTIVE_BODER    MEAS_BODER_COLOR    /* 光标选中文字背景颜色 */
#define CL_MENU_BACK3           RGB(255, 236, 222)  /* 光标选中文字背景颜色 - 用于修改参数 */
#define CL_MENU_TEXT3           RGB(50, 50, 50)     /* 光标选中文字颜色 */

#define LIST_COLOR1             RGB(255, 255, 255) 
#define LIST_COLOR2             RGB(240, 240, 240) 
#define LIST_COLOR_ACTIVE       CL_YELLOW
#define LIST_ITEM_COLOR         RGB(189, 189, 189)

#define SETTING_TIMEOUT         30                  /* 菜单按键超时 */
#define LCD_WAKE_UP_TIMEOUT     180                 /* LCD背光唤醒超时 3分钟 */

void FillTopBar(uint16_t _color);
void FillBottomBar(uint16_t _color);
void FillMidRect(uint16_t _color);
void DispStrInTopBar(char *_str);
void DispStrInBottomBar(char *_str);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
