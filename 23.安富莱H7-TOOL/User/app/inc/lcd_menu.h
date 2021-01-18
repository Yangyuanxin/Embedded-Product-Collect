/*
*********************************************************************************************************
*
*    模块名称 : LCD液晶显示器 菜单组件
*    文件名称 : lcd_menu.h
*    版    本 : V1.0
*    说    明 : 头文件
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2015-04-25 armfly  ST固件库版本 V2.1.0
*
*    Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _LCD_MENU_H
#define _LCD_MENU_H

//#define CL_MENU_TEXT1   CL_WHITE
//#define CL_MENU_BACK1   MAIN_BACK_COLOR

///* 光标选中 */
//#define CL_MENU_TEXT2   CL_BLACK
//#define CL_MENU_BACK2   CL_YELLOW

///* 菜单选中，保留另外一个颜色 */
//#define CL_MENU_TEXT3   CL_BLACK
//#define CL_MENU_BACK3   RGB(142, 209, 158)

#define MENU_LEFT       (0 + 5)
#define MENU_TOP        (0 + 38)
#define MENU_WIDTH      (240 - 10)
#define MENU_HEIGHT     (240 - 50)
#define MENU_CAP        3

/* 菜单结构 */
typedef struct
{
    uint8_t **Text;         /* 菜单文本 */
    uint8_t ViewLine;       /* 可视行数 */
    uint8_t Count;          /* 菜单项个数 */
    uint8_t Offset;         /* 当前屏幕第1行对应的索引 */
    uint8_t Cursor;         /* 选中行的索引 */

    uint16_t Left;          /* X 坐标 */
    uint16_t Top;           /* Y 坐标 */
    uint16_t Height;        /* 高度 */
    uint16_t Width;         /* 宽度 */
    uint16_t LineCap;       /* 行间距 */

    FONT_T Font;            /* 字体 */
    
    uint8_t RollBackEn;     /* 回滚使能 */
    
    uint8_t GBK;            /* 文字编码 0=UTF8, 1=GBK */
    
    uint8_t ActiveBackColor;    /* 选中行背景色,0表示黄色 1表示参数修改状态 */
} MENU_T;

/* 菜单显示类 */
void LCD_InitMenu(MENU_T *_pMenu, char **_Text);
void LCD_DispMenu(MENU_T *_pMenu);
void LCD_DispMenu2(MENU_T *_pMenu);
void LCD_MoveDownMenu(MENU_T *_pMenu);
void LCD_MoveUpMenu(MENU_T *_pMenu);
void LCD_ClearMenu(MENU_T *_pMenu);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
