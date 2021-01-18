/*
*********************************************************************************************************
*
*    模块名称 : LCD液晶菜单(键盘控制）
*    文件名称 : lcd_menu.c
*    版    本 : V1.0
*    说    明 : 。
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2015-04-25 armfly  ST固件库版本 V2.1.0
*
*    Copyright (C), 2014-2015, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*    函 数 名: LCD_InitMenu
*    功能说明: 按照16点阵汉字初始化菜单结构
*    形    参: _pMenu : 菜单结构体指针; 由主程序申请全局菜单结构体变量。
*             _Text : 菜单文本的数组指针。 由主程序分配空间保存数组
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitMenu(MENU_T *_pMenu, char **_Text)
{
    uint8_t i;

    _pMenu->Text = (uint8_t **)_Text; /* 菜单文本 */

    for (i = 0; i < 255; i++)
    {
        if (_pMenu->Text[i][0] == '&')
        {
            _pMenu->Count = i;
            break;
        }
    }

    _pMenu->Cursor = 0; /* 当前屏幕第1行对应的索引 */
    _pMenu->Offset = 0; /* 选中行的索引 */
}

/*
*********************************************************************************************************
*    函 数 名: LCD_ClearMenu
*    功能说明: 显示菜单 清屏
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_ClearMenu(MENU_T *_pMenu)
{
    LCD_Fill_Rect(_pMenu->Left, _pMenu->Top, _pMenu->Height, _pMenu->Width, CL_MENU_BACK1);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DispMenu
*    功能说明: 显示菜单 显示光标行
*    形    参: _pMenu : 菜单文字数组
*              _Count : 菜单项个数
*              _Cursor : 光标行
*              _FocusLine :  焦点行(0-3)
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispMenu(MENU_T *_pMenu)
{
    uint8_t i;
    uint8_t FontHeight;
    uint16_t y;
    uint8_t line_cap1, line_cap2;

    FontHeight = LCD_GetFontWidth(&_pMenu->Font);

    line_cap1 = _pMenu->LineCap / 2;                 /* 菜单文本前的高度 */
    line_cap2 = _pMenu->LineCap - line_cap1; /* 菜单文本后的高度 */
    for (i = 0; i < _pMenu->ViewLine; i++)
    {
        if (i >= _pMenu->Count)
        {
            break;
        }

        if (i + _pMenu->Offset == _pMenu->Cursor)   /* 选钟行 */
        {            
            /* 设置为反白 */
            
            _pMenu->Font.BackColor = CL_MASK;

            y = _pMenu->Top + i * (FontHeight + _pMenu->LineCap);

            /* 清全行背景 */
            if (_pMenu->ActiveBackColor == 0)
            {
                _pMenu->Font.FrontColor = CL_MENU_TEXT2;
                LCD_Fill_Rect(_pMenu->Left, y, _pMenu->LineCap + FontHeight, _pMenu->Width, CL_MENU_BACK2);
            }
            else
            {
                _pMenu->Font.FrontColor = CL_MENU_TEXT3;
                LCD_Fill_Rect(_pMenu->Left, y, _pMenu->LineCap + FontHeight, _pMenu->Width, CL_MENU_BACK3);
            }
                
            /* 刷新文本 */
            if (_pMenu->GBK == 1)
            {               
                LCD_SetEncode(ENCODE_GBK);
            }
            
            LCD_DispStrEx(_pMenu->Left, y + line_cap1, (char *)_pMenu->Text[_pMenu->Offset + i], &_pMenu->Font,
                                    _pMenu->Width, ALIGN_LEFT);             

            LCD_SetEncode(ENCODE_UTF8);
            
            LCD_DrawRoundRect(_pMenu->Left, y, _pMenu->LineCap + FontHeight, _pMenu->Width, 5, CL_MENU_ACTIVE_BODER);
        }
        else
        {
            /* 恢复正常底色 */
            _pMenu->Font.FrontColor = CL_MENU_TEXT1;
            _pMenu->Font.BackColor = CL_MENU_BACK1;
            
            y = _pMenu->Top + i * (FontHeight + _pMenu->LineCap);

            /* 清段前背景 */
            LCD_Fill_Rect(_pMenu->Left, y, line_cap1, _pMenu->Width, _pMenu->Font.BackColor);

            /* 刷新文本 */
            if (_pMenu->GBK == 1)
            {
                LCD_SetEncode(ENCODE_GBK);
            }
            LCD_DispStrEx(_pMenu->Left, y + line_cap1, (char *)_pMenu->Text[_pMenu->Offset + i], &_pMenu->Font,
                                        _pMenu->Width, ALIGN_LEFT);               
            LCD_SetEncode(ENCODE_UTF8);
            
            /* 清段后背景 */
            LCD_Fill_Rect(_pMenu->Left, y + line_cap1 + FontHeight, line_cap2, _pMenu->Width, _pMenu->Font.BackColor);            
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DispMenu2
*    功能说明: 显示菜单 不显示光标行
*    形    参: _pMenu : 菜单文字数组
*              _Count : 菜单项个数
*              _Cursor : 光标行
*              _FocusLine :  焦点行(0-3)
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispMenu2(MENU_T *_pMenu)
{
    uint8_t i;
    uint8_t FontHeight;
    uint16_t y;
    uint8_t line_cap1, line_cap2;

    FontHeight = LCD_GetFontWidth(&_pMenu->Font);

    line_cap1 = _pMenu->LineCap / 2;                 /* 菜单文本前的高度 */
    line_cap2 = _pMenu->LineCap - line_cap1; /* 菜单文本后的高度 */
    for (i = 0; i < _pMenu->ViewLine; i++)
    {
        if (i >= _pMenu->Count)
        {
            break;
        }

        if (i + _pMenu->Offset == _pMenu->Cursor)   /* 选钟行 */
        {            
            /* 设置为反白 */
            _pMenu->Font.FrontColor = CL_MENU_TEXT1;
            _pMenu->Font.BackColor = CL_MASK;

            y = _pMenu->Top + i * (FontHeight + _pMenu->LineCap);

            /* 清全行背景 */
            LCD_Fill_Rect(_pMenu->Left, y, _pMenu->LineCap + FontHeight, _pMenu->Width, CL_MENU_BACK1);

            /* 刷新文本 */
            if (_pMenu->GBK == 1)
            {
                LCD_SetEncode(ENCODE_GBK);
            }
            
            LCD_DispStrEx(_pMenu->Left, y + line_cap1, (char *)_pMenu->Text[_pMenu->Offset + i], &_pMenu->Font,
                                            _pMenu->Width, ALIGN_LEFT);               

            LCD_SetEncode(ENCODE_UTF8);
            
            LCD_DrawRoundRect(_pMenu->Left, y, _pMenu->LineCap + FontHeight, _pMenu->Width, 5, CL_MENU_ACTIVE_BODER);
        }
        else
        {
            /* 恢复正常底色 */
            _pMenu->Font.FrontColor = CL_MENU_TEXT1;
            _pMenu->Font.BackColor = CL_MENU_BACK1;
            
            y = _pMenu->Top + i * (FontHeight + _pMenu->LineCap);

            /* 清段前背景 */
            LCD_Fill_Rect(_pMenu->Left, y, line_cap1, _pMenu->Width, _pMenu->Font.BackColor);

            /* 刷新文本 */
            if (_pMenu->GBK == 1)
            {
                LCD_SetEncode(ENCODE_GBK);
            }
            
            LCD_DispStrEx(_pMenu->Left, y + line_cap1, (char *)_pMenu->Text[_pMenu->Offset + i], &_pMenu->Font,
                                        _pMenu->Width, ALIGN_LEFT);
            
            LCD_SetEncode(ENCODE_UTF8);

            /* 清段后背景 */
            LCD_Fill_Rect(_pMenu->Left, y + line_cap1 + FontHeight, line_cap2, _pMenu->Width, _pMenu->Font.BackColor);            
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_MoveDownMenu
*    功能说明: 向下移动选中的菜单行, 并刷新显示.
*    形    参: _pMenu : 菜单结构体指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_MoveDownMenu(MENU_T *_pMenu)
{
    if (_pMenu->Cursor < _pMenu->Count - 1)
    {
        _pMenu->Cursor++;

        if (_pMenu->Cursor - _pMenu->Offset >= _pMenu->ViewLine)
        {
            _pMenu->Offset++;
        }

        LCD_DispMenu(_pMenu); /* 刷新显示 */
    }
    else
    {
        if (_pMenu->RollBackEn == 1)    /* 回滚 */
        {        
            _pMenu->Cursor = 0;
            _pMenu->Offset = 0;
            LCD_DispMenu(_pMenu); /* 刷新显示 */
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_MoveUpMenu
*    功能说明: 向上移动选中的菜单行, 并刷新显示.
*    形    参: _pMenu : 菜单结构体指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_MoveUpMenu(MENU_T *_pMenu)
{
    if (_pMenu->Cursor > 0)
    {
        _pMenu->Cursor--;

        if (_pMenu->Cursor < _pMenu->Offset)
        {
            _pMenu->Offset--;
        }

        LCD_DispMenu(_pMenu); /* 刷新显示 */
    }
    else
    {
        if (_pMenu->RollBackEn == 1)    /* 回滚 */
        {
            _pMenu->Cursor = _pMenu->Count - 1;
            if (_pMenu->Count > _pMenu->ViewLine)
            {
                _pMenu->Offset = _pMenu->Count - _pMenu->ViewLine;
            }
            else
            {
                _pMenu->Offset = 0;
            }
            LCD_DispMenu(_pMenu); /* 刷新显示 */
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
