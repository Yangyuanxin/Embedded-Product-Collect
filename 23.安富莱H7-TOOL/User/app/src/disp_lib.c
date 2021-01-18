/*
*********************************************************************************************************
*
*	模块名称 : 显示类函数集合
*	文件名称 : disp_lib.c
*	版    本 : V1.0
*	说    明 : H7-TOOL APP
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2020-1106-20  armfly      1.由APP部分整合而来。裸机程序框架
*
*	Copyright (C), 2020-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	

#include "includes.h"

extern uint16_t g_MainStatus;  /* 主状态字 */

/*
*********************************************************************************************************
*    函 数 名: DispHeader
*    功能说明: 显示界面标题（抬头第1行）
*    形    参: _str : 标题文字
*    返 回 值: 无
*********************************************************************************************************
*/
extern uint16_t GetStatusIndex(uint16_t _NowStatus);
void DispHeader(char *_str)
{    
    uint8_t idx;
    
    idx  = GetStatusIndex(g_MainStatus);
    DispHeader2(idx, _str);
}

/* 带序号参数 */
void DispHeader2(uint8_t _idx, char *_str)
{
    FONT_T tFont;
    char buf[48];

    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_24;          /* 字体代码 16点阵 */
        tFont.FrontColor = HEAD_TEXT_COLOR; /* 字体颜色 */
        tFont.BackColor = HEAD_BACK_COLOR;  /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    LCD_Fill_Rect(0, 0, HEAD_HEIGHT, HEAD_HEIGHT, HEAD_SN_COLOR);       /* 清屏标题序号区  */
    LCD_Fill_Rect(HEAD_HEIGHT, 0, HEAD_HEIGHT, 240 - HEAD_HEIGHT, HEAD_BACK_COLOR);   /* 清屏标题栏文本区  */
    
    sprintf(buf, "%02d", _idx);
    tFont.BackColor = HEAD_SN_COLOR;    /* 文字背景颜色 */
    LCD_DispStr(4, 4, buf, &tFont);
    
    /* 显示标题文字 */
    tFont.BackColor = HEAD_BACK_COLOR;  /* 文字背景颜色 */
    LCD_DispStrEx(HEAD_HEIGHT, 4, _str, &tFont, 240 - HEAD_HEIGHT, ALIGN_CENTER);

    LCD_DrawLine(0, HEAD_HEIGHT, 239, HEAD_HEIGHT, HEAD_BODER_COLOR);
    
    LCD_Fill_Rect(0, HEAD_HEIGHT + 1, 240 - HEAD_HEIGHT - 1, 240, FORM_BACK_COLOR); /* 清屏正文区  */
}

/* 不带数字标号，整行文本居中 */
void DispHeader3(char *_str)
{
    FONT_T tFont;

    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_24;          /* 字体代码 16点阵 */
        tFont.FrontColor = HEAD_TEXT_COLOR; /* 字体颜色 */
        tFont.BackColor = HEAD_BACK_COLOR;  /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    LCD_Fill_Rect(0, 0, HEAD_HEIGHT, 240 , HEAD_BACK_COLOR);   /* 清屏标题栏文本区  */
    
    /* 显示标题文字 */
    tFont.BackColor = HEAD_BACK_COLOR;  /* 文字背景颜色 */
    LCD_DispStrEx(0, 4, _str, &tFont, 240, ALIGN_CENTER);
    
    LCD_Fill_Rect(0, HEAD_HEIGHT + 0, 240 - HEAD_HEIGHT - 0, 240, FORM_BACK_COLOR); /* 清屏正文区  */
}

/* 只刷新标题文字部分，不清屏 */
void DispHeaderStr(char *_str)
{
    FONT_T tFont;

    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_24;          /* 字体代码 16点阵 */
        tFont.FrontColor = HEAD_TEXT_COLOR; /* 字体颜色 */
        tFont.BackColor = HEAD_BACK_COLOR;  /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    LCD_Fill_Rect(HEAD_HEIGHT, 0, HEAD_HEIGHT, 240 - HEAD_HEIGHT, HEAD_BACK_COLOR);   /* 清屏标题栏文本区  */
    
    /* 显示标题文字 */
    tFont.BackColor = HEAD_BACK_COLOR;  /* 文字背景颜色 */
    LCD_DispStrEx(HEAD_HEIGHT, 4, _str, &tFont, 240 - HEAD_HEIGHT, ALIGN_CENTER);
}

/* 显示序号 */
void DispHeaderSn(uint8_t _idx)
{
    FONT_T tFont;
    char buf[48];

    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_24;          /* 字体代码 16点阵 */
        tFont.FrontColor = HEAD_TEXT_COLOR; /* 字体颜色 */
        tFont.BackColor = HEAD_BACK_COLOR;  /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    LCD_Fill_Rect(0, 0, HEAD_HEIGHT, HEAD_HEIGHT, HEAD_SN_COLOR);       /* 清屏标题序号区  */
    
    sprintf(buf, "%02d", _idx);
    tFont.BackColor = HEAD_SN_COLOR;    /* 文字背景颜色 */
    LCD_DispStr(4, 4, buf, &tFont);
}

/*
*********************************************************************************************************
*    函 数 名: DispBox
*    功能说明: 显示一个圆角矩形框，固定风格。
*    形    参: _usX : 坐标X
*              _usY : 坐标Y
*              _usHeight : 高度
*              _usWidth : 宽度
*              _usColor : 填充颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void DispBox(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{    
    /* 绘制边框 */
    LCD_DrawRoundRect(_usX, _usY, _usHeight, _usWidth, B0X_ARC + 2, MEAS_BODER_COLOR);
    
    /* 填充矩形 */
    LCD_FillRoundRect(_usX + 1, _usY + 1, _usHeight - 2, _usWidth - 2, B0X_ARC, _usColor);
}   

/*
*********************************************************************************************************
*    函 数 名: DispLabel
*    功能说明: 显示一个文本标签框, 矩形边角
*    形    参: _usX : 坐标X
*              _usY : 坐标Y
*              _usHeight : 高度
*              _usWidth : 宽度
*              _usColor : 填充颜色
*              _pStr : 显示内容
*              _pFont : 字体
*    返 回 值: 无
*********************************************************************************************************
*/
void DispLabel(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
    uint16_t _usColor, char *_pStr, FONT_T *_tFont)
{    
    uint16_t fh;
    uint16_t offset;
    
    fh = LCD_GetFontHeight(_tFont);
    if (_usHeight > fh)
    {
        offset = (_usHeight - fh) / 2;
    }
    else
    {
        offset = 0;
    }    
    
    /* 填充矩形 */
    LCD_Fill_Rect(_usX, _usY, _usHeight, _usWidth, _usColor);    
    LCD_DispStr(_usX + 2, _usY + offset, _pStr, _tFont);
}  

/*
*********************************************************************************************************
*    函 数 名: DispLabelRound
*    功能说明: 显示圆角标签，用于DAP-LINK
*    形    参: _usX : 坐标X
*              _usY : 坐标Y
*              _usHeight : 高度
*              _usWidth : 宽度
*              _usColor : 填充颜色
*              _pStr : 显示内容
*              _pFont : 字体
*    返 回 值: 无
*********************************************************************************************************
*/
void DispLabelRound(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
    uint16_t _usColor, char *_pStr, FONT_T *_tFont)
{
    uint16_t fh;
    uint16_t offset;
    
    /* 绘制圆角矩形 */
    DispBox(_usX, _usY, _usHeight, _usWidth, _usColor);

    fh = LCD_GetFontHeight(_tFont);
    if (_usHeight > fh)
    {
        offset = (_usHeight - fh) / 2;
    }
    else
    {
        offset = 0;
    }
    LCD_DispStrEx(_usX, _usY + offset, _pStr, _tFont, _usWidth, ALIGN_CENTER);
}

/*
*********************************************************************************************************
*    函 数 名: DispMeasBar
*    功能说明: 显示测量值
*    形    参: _ucLine : 行号 0 - 3 (最多4行)
*              _pName : 参数名称
*              _pValue : 测量值
*              _pUnit : 单位
*    返 回 值: 无
*********************************************************************************************************
*/
void DispMeasBar(uint8_t _ucLine, char *_pName, char *_pValue, char *_pUnit)
{
    DispMeasBarEx(_ucLine, _pName, _pValue, _pUnit, MEAS_BACK_COLOR);
}

/*
*********************************************************************************************************
*    函 数 名: DispMeasBarEx
*    功能说明: 显示测量值，带填充颜色参数
*    形    参: _ucLine : 行号 0 - 3 (最多4行)
*              _pName : 参数名称
*              _pValue : 测量值
*              _pUnit : 单位
*    返 回 值: 无
*********************************************************************************************************
*/
void DispMeasBarEx(uint8_t _ucLine, char *_pName, char *_pValue, char *_pUnit, uint16_t _usFillColor)
{
    FONT_T tFont;    
    uint16_t x;
    uint16_t y;
    
    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_24;          /* 字体代码 16点阵 */
        tFont.FrontColor = MEAS_NAME_COLOR; /* 字体颜色 */
        tFont.BackColor = CL_MASK;          /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    x = MEAS_WIN_LEFT;
    y = HEAD_HEIGHT + 9 + _ucLine * (MEAS_WIN_HEIGHT + 9);
    
    DispBox(x, y, MEAS_WIN_HEIGHT, MEAS_WIN_WIDTH, _usFillColor);
    
    /* 参数名字 */
    tFont.FrontColor = MEAS_NAME_COLOR;
    LCD_DispStr(MEAS_WIN_LEFT + 5, y + 4, _pName, &tFont);

    /* 测量值 */
    tFont.FrontColor = MEAS_VALUE_COLOR;
    LCD_DispStr(MEAS_WIN_LEFT + 80, y + 4, _pValue, &tFont);

    /* 单位 */
    tFont.FrontColor = MEAS_UNIT_COLOR;
    LCD_DispStr(MEAS_WIN_LEFT + 188, y + 4, _pUnit, &tFont);    
}

/*
*********************************************************************************************************
*    函 数 名: DispParamBar
*    功能说明: 显示参数设置 - 系统设置中使用
*    形    参: _ucLine : 行号 0 - 3 (最多4行)
*              _pName : 参数名称
*              _pValue : 测量值
*              _ucActive : 选中否
*    返 回 值: 无
*********************************************************************************************************
*/
void DispParamBar(uint8_t _ucLine, char *_pName, char *_pValue, uint8_t _ucActive)
{
    FONT_T tFont;    
    uint16_t x;
    uint16_t y;
    uint16_t NameWidth;
    
    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_24;          /* 字体代码 16点阵 */
        tFont.FrontColor = MEAS_NAME_COLOR; /* 字体颜色 */
        tFont.BackColor = CL_MASK;          /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    x = MEAS_WIN_LEFT;
    y = HEAD_HEIGHT + 9 + _ucLine * (MEAS_WIN_HEIGHT + 3);
    
    /* 绘制圆角矩形 */
    if (_ucActive == 1)    /* 选中是用黄色底 */
    {    
        DispBox(x, y, MEAS_WIN_HEIGHT, MEAS_WIN_WIDTH, CL_YELLOW);
    }
    else
    {
        DispBox(x, y, MEAS_WIN_HEIGHT, MEAS_WIN_WIDTH, MEAS_BACK_COLOR);
    }
    
    /* 参数名字 */
    tFont.FrontColor = MEAS_NAME_COLOR;
    LCD_DispStr(MEAS_WIN_LEFT + 5, y + 4, _pName, &tFont);

    NameWidth = LCD_GetStrWidth(_pName, &tFont);
    
    /* 测量值 */
    tFont.FrontColor = MEAS_VALUE_COLOR;
    LCD_DispStr(MEAS_WIN_LEFT + 5 + NameWidth + 5, y + 4, _pValue, &tFont);    
}

/*
*********************************************************************************************************
*    函 数 名: DispProgressBar
*    功能说明: 显示进度条
*    形    参:  _usX : 坐标
*               _usX : 坐标
*               _usHeight : 高度
*               _usWidth : 宽度
*               _str : 显示文字
*               _Percent : 百分比, 浮点数
*               tFont : 字体
*    返 回 值: 无
*********************************************************************************************************
*/
extern uint8_t s_DispRefresh;
static uint16_t s_ProgressBarColor1 = PROGRESS_BACK_COLOR1;
void ProgressBarSetColor(uint16_t _Color)
{
    s_ProgressBarColor1 = _Color;
}

void DispProgressBar(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, 
    char *_str1, float _Percent, char *_str2, FONT_T *_tFont)    
{   
    uint16_t width;
    char buf[16];
    uint16_t StrWidth;
    uint16_t StrHeight;
    uint16_t x, y;
    
    if (_Percent > 100)
    {
        _Percent = 100;
    }
    
    width = ((_usWidth - 4) * _Percent) / 100;
    
    /* 填充矩形 */
    LCD_DrawRect(_usX,          _usY,     _usHeight,     _usWidth,  PROGRESS_BODER_COLOR);
    
    LCD_Fill_Rect(_usX + 2,     _usY + 2, _usHeight - 4, width,     s_ProgressBarColor1);
    
    if (_Percent < 100)
    {
        LCD_Fill_Rect(_usX + width + 2, _usY + 2, _usHeight - 4, _usWidth - width - 4, PROGRESS_BACK_COLOR2); 
    }

    StrHeight = LCD_GetFontHeight(_tFont);
    y = _usY + (_usHeight - StrHeight) / 2;
    if (_str1[0] == 0)   /* 只显示显示百分比文字 */
    {              
        sprintf(buf, "%0.0f%%", _Percent);
        StrWidth = LCD_GetStrWidth(buf, _tFont);
        x = _usX + (_usWidth - StrWidth) / 2;
        LCD_DispStr(x, y, buf, _tFont);  
    }
    else
    {
        /* 显示左侧文本 */
        x = _usX + 4;
        LCD_DispStr(x, y, _str1, _tFont);  
        
        /* 显示百分比 */
        sprintf(buf, "%0.0f%%", _Percent);
        StrWidth = LCD_GetStrWidth(buf, _tFont);
        x = _usX + (_usWidth - StrWidth) / 2;
        LCD_DispStr(x, y, buf, _tFont);

        /* 显示右侧文本 */
        x = x + 36;
        LCD_DispStr(x, y, _str2, _tFont);          
    }     
}

/*
*********************************************************************************************************
*   函 数 名: DispHelpBar
*   功能说明: 显示操作提示
*   形    参: 无
*   返 回 值: 无
*********************************************************************************************************
*/
const unsigned char g_ImageHelp[512] = { /* 0X10,0X10,0X00,0X10,0X00,0X10,0X01,0X1B, */
    0XF7,0X9E,0XF7,0X9E,0XF7,0X9E,0XFF,0XBE,0XFF,0XBE,0XEF,0X9E,0XD7,0X5E,0XCF,0X3F,
    0XCF,0X1F,0XD7,0X5F,0XE7,0X9F,0XFF,0XDF,0XFF,0XDE,0XF7,0X9E,0XF7,0X9E,0XF7,0X9E,
    0XF7,0X9E,0XF7,0X9E,0XF7,0XBE,0XEF,0X7E,0XC6,0XFE,0XAE,0XBE,0XA6,0X7E,0X9E,0X7E,
    0X9E,0X7E,0X9E,0X7E,0XA6,0X7E,0XBE,0XDE,0XE7,0X7E,0XFF,0XBE,0XF7,0X9E,0XF7,0X9E,
    0XF7,0X9E,0XF7,0XBE,0XE7,0X5E,0XB6,0XBE,0XA6,0X7E,0X9E,0X5E,0X96,0X5E,0X8E,0X1E,
    0X8E,0X1E,0X96,0X3E,0X96,0X5E,0X9E,0X5E,0XAE,0X9E,0XDF,0X5E,0XFF,0XBE,0XF7,0X9E,
    0XF7,0XBE,0XEF,0X7E,0XB6,0XBD,0X9E,0X7E,0X96,0X3E,0X96,0X3E,0X8E,0X1E,0XA6,0X7E,
    0XA6,0X7E,0X8E,0X1D,0X8E,0X1E,0X8E,0X3D,0X96,0X3D,0XA6,0X7D,0XE7,0X7E,0XFF,0XDE,
    0XFF,0XBE,0XBE,0XDD,0X9E,0X5D,0X96,0X3D,0X8E,0X1D,0X8E,0X1D,0X85,0XFD,0XDF,0X7E,
    0XE7,0X9F,0X85,0XFD,0X86,0X1D,0X86,0X1D,0X8E,0X1D,0X8E,0X1D,0XB6,0XBD,0XFF,0XDF,
    0XE7,0X7E,0X96,0X3D,0X8E,0X1D,0X86,0X1D,0X85,0XFD,0X86,0X1D,0X7D,0XFD,0X96,0X3D,
    0X96,0X3D,0X85,0XFD,0X85,0XFD,0X85,0XFD,0X85,0XFD,0X85,0XFD,0X8E,0X1C,0XE7,0X7E,
    0XCF,0X1E,0X7D,0XFC,0X85,0XFD,0X85,0XFD,0X7D,0XFD,0X7D,0XFD,0X8E,0X3D,0X96,0X3D,
    0X8E,0X1D,0X7D,0XFD,0X7D,0XFD,0X7D,0XFD,0X7D,0XDC,0X7D,0XFC,0X75,0XDC,0XC6,0XFE,
    0XB6,0XBD,0X75,0XBC,0X7D,0XDC,0X7D,0XDC,0X7D,0XFD,0X7D,0XDD,0X8E,0X3D,0XE7,0X7F,
    0XD7,0X5E,0X75,0XDC,0X7D,0XDC,0X7D,0XDC,0X7D,0XDC,0X7D,0XDC,0X75,0X9C,0XAE,0X9D,
    0XAE,0X9D,0X75,0X9C,0X7D,0XDC,0X7D,0XDC,0X7D,0XDC,0X7D,0XDC,0X6D,0X9C,0XBE,0XDE,
    0XDF,0X5E,0X75,0XBC,0X7D,0XDC,0X7D,0XDC,0X75,0XBC,0X75,0XBC,0X6D,0X7B,0XA6,0X7D,
    0XC6,0XFD,0X75,0X9C,0X7D,0XBC,0X75,0XBC,0X75,0XBC,0X75,0XBC,0X6D,0X9C,0XBE,0XDD,
    0XD7,0X5E,0X6D,0X9C,0X75,0XBC,0X75,0XBC,0X75,0X9C,0X75,0X9C,0X65,0X7B,0XBE,0XDD,
    0XE7,0X7E,0X85,0XDC,0X7D,0XBC,0X75,0X9C,0X75,0X9B,0X75,0X9C,0X65,0X7B,0XB6,0XBD,
    0XD7,0X3E,0X65,0X7B,0X6D,0X9C,0X6D,0X7B,0X6D,0X7B,0X75,0X9B,0X75,0X9B,0XDF,0X5E,
    0XFF,0XDE,0XAE,0X7C,0X85,0XDB,0X75,0X9B,0X6D,0X7B,0X6D,0X7B,0X5D,0X3B,0XB6,0XBD,
    0XD7,0X5E,0X5D,0X3B,0X6D,0X7B,0X6D,0X7B,0X6D,0X7B,0X75,0X7B,0X9E,0X3C,0XFF,0XDF,
    0XFF,0XBE,0XE7,0X5E,0X96,0X1C,0X7D,0XBB,0X75,0X7B,0X65,0X5B,0X75,0X9B,0XCF,0X1D,
    0XDF,0X7E,0X85,0XDB,0X5D,0X3A,0X6D,0X5B,0X75,0X7B,0X85,0XDB,0XDF,0X5D,0XFF,0XDE,
    0XF7,0X9E,0XFF,0XBE,0XD7,0X3D,0X95,0XFB,0X75,0X9B,0X6D,0X7B,0X6D,0X5B,0X6D,0X5A,
    0X65,0X5A,0X6D,0X5A,0X65,0X5A,0X6D,0X5A,0X7D,0XBB,0XCE,0XFD,0XFF,0XDE,0XF7,0X9E,
    0XF7,0X9E,0XF7,0X9E,0XFF,0XBE,0XDF,0X3D,0X9E,0X1B,0X6D,0X7A,0X5D,0X1A,0X4C,0XFA,
    0X4C,0XDA,0X4C,0XFA,0X5D,0X3A,0X8D,0XFB,0XDF,0X3D,0XFF,0XBE,0XF7,0X9E,0XF7,0X9E,
    0XF7,0X9E,0XF7,0X9E,0XF7,0X9E,0XF7,0X9E,0XEF,0X5D,0XCE,0XBB,0XA6,0X1A,0X85,0XBA,
    0X85,0XBA,0X9E,0X1A,0XC6,0XBC,0XEF,0X7D,0XF7,0XBE,0XF7,0X9E,0XF7,0X9E,0XF7,0X9E,
};
void DispHelpBar(char *_str1, char *_str2)
{
    FONT_T tFont;   /* 定义字体结构体变量 */

    tFont.FontCode = FC_ST_16;              /* 字体代码 16点阵 */
    tFont.FrontColor = HELP_TEXT_COLOR;     /* 字体颜色 */
    tFont.BackColor = HELP_BACK_COLOR;      /* 文字背景颜色 */
    tFont.Space = 0;                        /* 文字间距，单位 = 像素 */

    ST7789_DrawBMP8(1, 240 - 36, 16, 16, (uint8_t *)g_ImageHelp);
    
    LCD_DispStr(20, 240 - 36, _str1, &tFont);
    LCD_DispStr(20, 240 - 18, _str2, &tFont);  
}

/*
*********************************************************************************************************
*    函 数 名: DispInfoBar16
*    功能说明: 显示信息，16点阵.
*    形    参: _line   : 行号
*              _str1 : 参数名
*              _str2 : 参数值
*    返 回 值: 无
*********************************************************************************************************
*/
void DispInfoBar16(uint8_t _ucLine, char *_pName, char *_pValue)
{
    DispInfoBar16Ex(_ucLine, _pName, _pValue, INFO_VALUE_COLOR);
}

/* 带颜色参数 - 用于显示红色超标参数 */
void DispInfoBar16Ex(uint8_t _ucLine, char *_pName, char *_pValue, uint16_t _ucColor)
{
    FONT_T tFont;    
    uint16_t x;
    uint16_t y;
    
    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_16;          /* 字体代码 16点阵 */
        tFont.FrontColor = INFO_NAME_COLOR; /* 字体颜色 */
        tFont.BackColor = INFO_BACK_COLOR;  /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    x = 5;
    y = HEAD_HEIGHT + 5 + _ucLine * INFO_HEIGHT;
    
    /* 参数名字 */
    tFont.FrontColor = INFO_NAME_COLOR;
    LCD_DispStr(x, y, _pName, &tFont);

    x += LCD_GetStrWidth(_pName, &tFont) + 5;
    
    /* 参数值 */
    tFont.FrontColor = _ucColor;
    LCD_DispStr(x, y, _pValue, &tFont);  
}

/*
*********************************************************************************************************
*    函 数 名: DispMsgBox
*    功能说明: 显示一个消息框. 字体16点阵
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void DispMsgBox(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, char *_str)    
{   
    FONT_T tFont;
    
    /* 设置字体参数 */
    {
        tFont.FontCode = FC_ST_16;          /* 字体代码 16点阵 */
        tFont.FrontColor = MEAS_NAME_COLOR; /* 字体颜色 */
        tFont.BackColor = CL_MASK;          /* 文字背景颜色 */
        tFont.Space = 0;                    /* 文字间距，单位 = 像素 */
    }
    
    DispBox(_usX, _usY, _usHeight, _usWidth, MEAS_BACK_COLOR);

    /* 参数名字 */
    tFont.FrontColor = MEAS_NAME_COLOR;
    LCD_DispStr(_usX + 5, _usY + 4, _str, &tFont);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
