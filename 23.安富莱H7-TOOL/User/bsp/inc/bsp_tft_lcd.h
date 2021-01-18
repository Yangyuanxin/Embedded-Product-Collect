/*
*********************************************************************************************************
*
*    模块名称 : TFT液晶显示器驱动模块
*    文件名称 : LCD_tft_lcd.h
*    版    本 : V2.0
*    说    明 : 头文件
*
*    Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_TFT_LCD_H
#define _BSP_TFT_LCD_H

#define ENCODE_UTF8    0
#define ENCODE_GBK     1

#define BUTTON_BEEP() BEEP_KeyTone(); /* 按键提示音 */
//#define BUTTON_BEEP()    /* 无按键提示音 */

/* 定义LCD显示区域的分辨率 */
#define LCD_30_HEIGHT 240 /* 3.0寸宽屏 高度，单位：像素 */
#define LCD_30_WIDTH 400    /* 3.0寸宽屏 宽度，单位：像素 */

#define LCD_43_HEIGHT 272 /* 4.3寸宽屏 高度，单位：像素 */
#define LCD_43_WIDTH 480    /* 4.3寸宽屏 宽度，单位：像素 */

#define LCD_70_HEIGHT 480 /* 7.0寸宽屏 高度，单位：像素 */
#define LCD_70_WIDTH 800    /* 7.0寸宽屏 宽度，单位：像素 */

/* 支持的驱动芯片ID */
enum
{
    IC_5420 = 0x5420,
    IC_4001 = 0x4001,
    IC_61509 = 0xB509,
    IC_8875 = 0x0075,
    IC_9488 = 0x9488
};

#define CHIP_STR_5420 "SPFD5420A"
#define CHIP_STR_4001 "OTM4001A"
#define CHIP_STR_61509 "R61509V"
#define CHIP_STR_8875 "RA8875"
#define CHIP_STR_9488 "ILI9488"

#define CHIP_STR_F429 "STM32F429"

/*
    LCD 颜色代码，CL_是Color的简写
    16Bit由高位至低位， RRRR RGGG GGGB BBBB

    下面的RGB 宏将24位的RGB值转换为16位格式。
    启动windows的画笔程序，点击编辑颜色，选择自定义颜色，可以获得的RGB值。

    推荐使用迷你取色器软件获得你看到的界面颜色。
*/
#define RGB(R, G, B) (((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3)) /* 将8位R,G,B转化为 16位RGB565格式 */

/* 解码出 R=8bit G=8bit B=8bit */
#define RGB565_R(x) ((x >> 8) & 0xF8)
#define RGB565_G(x) ((x >> 3) & 0xFC)
#define RGB565_B(x) ((x << 3) & 0xF8)

/* 解码出 R=5bit G=6bit B=5bit */
#define RGB565_R2(x) ((x >> 11) & 0x1F)
#define RGB565_G2(x) ((x >> 5) & 0x3F)
#define RGB565_B2(x) ((x >> 0) & 0x1F)

enum
{
    CL_WHITE = RGB(255, 255, 255), /* 白色 */
    CL_BLACK = RGB(0, 0, 0),             /* 黑色 */
    CL_RED = RGB(255, 0, 0),             /* 红色 */
    CL_GREEN = RGB(0, 255, 0),         /* 绿色 */
    CL_BLUE = RGB(0, 0, 255),             /* 蓝色 */
    CL_YELLOW = RGB(255, 255, 0),    /* 黄色 */

    CL_GREY = RGB(98, 98, 98),         /* 深灰色 */
    CL_GREY1 = RGB(150, 150, 150), /* 浅灰色 */
    CL_GREY2 = RGB(180, 180, 180), /* 浅灰色 */
    CL_GREY3 = RGB(200, 200, 200), /* 最浅灰色 */
    CL_GREY4 = RGB(230, 230, 230), /* 最浅灰色 */

    CL_BUTTON_GREY = RGB(220, 220, 220), /* WINDOWS 按钮表面灰色 */

    CL_MAGENTA = 0xF81F, /* 红紫色，洋红色 */
    CL_CYAN = 0x7FFF,         /* 蓝绿色，青色 */

    CL_BLUE1 = RGB(0, 0, 240),     /* 深蓝色 */
    CL_BLUE2 = RGB(0, 0, 128),     /* 深蓝色 */
    CL_BLUE3 = RGB(68, 68, 255), /* 浅蓝色1 */
    CL_BLUE4 = RGB(0, 64, 128),    /* 浅蓝色1 */

    /* UI 界面 Windows控件常用色 */
    CL_BTN_FACE = RGB(236, 233, 216), /* 按钮表面颜色(灰) */

    CL_BTN_FONT = CL_BLACK, /* 按钮字体颜色（黑） */

    CL_BOX_BORDER1 = RGB(172, 168, 153), /* 分组框主线颜色 */
    CL_BOX_BORDER2 = RGB(255, 255, 255), /* 分组框阴影线颜色 */

    CL_MASK = 0x9999 /* 颜色掩码，用于文字背景透明 */
};

/* 文字对齐方式 */
enum
{
    ALIGN_LEFT = 0,
    ALIGN_CENTER = 1,
    ALIGN_RIGHT = 2
};

/* 编辑框风格 */
enum
{
    EDIT_BORDER_COLOR = CL_BLUE2,           /* 编辑框四个边的颜色，未选中时 */
    EDIT_BORDER_COLOR2 = CL_BLUE3,          /* 编辑框四个边的颜色,选中时 */
    EDIT_BACK_COLOR = RGB(237, 125, 49),    /* 编辑框背景，未选中时 */
    EDIT_BACK_COLOR2 = RGB(255, 192, 0),    /* 编辑框背景颜色，选中时 */
};

/* 按钮风格 */
enum
{
    /* 未按下时 */
    BTN_BORDER_COLOR1 = RGB(172, 168, 153), /* 按钮边框的颜色 */
    BTN_BODY_COLOR1 = RGB(200, 210, 210),        /* 按钮底色 */
    BTN_SHADOW_COLOR1 = RGB(220, 230, 230), /* 按钮上半部阴影色 */

    /* 按下时 */
    BTN_BORDER_COLOR2 = RGB(172, 200, 153), /* 按下时按钮边框的颜色 */
    BTN_BODY_COLOR2 = RGB(191, 238, 255),        /* 按下时按钮底色 */
    BTN_SHADOW_COLOR2 = RGB(200, 255, 255), /* 按下时按钮上半部阴影色 */
};

/* 窗口风格 */
enum
{
    WIN_BORDER_COLOR = CL_BLUE4,    /* 窗口边框 */
    WIN_TITLE_COLOR = CL_BLUE3,        /* 窗口标题蓝背景颜色 */
    WIN_CAPTION_COLOR = CL_WHITE, /* 窗口标题栏文字颜色 */
    WIN_BODY_COLOR = CL_GREY2,        /* 窗体颜色 */
};

/* 检查框风格 */
enum
{
    CHECK_BOX_BORDER_COLOR = CL_BLUE2, /* 检查框四个边的颜色 */
    CHECK_BOX_BACK_COLOR = CL_GREY3,     /* 检查框背景 */
    CHECK_BOX_CHECKED_COLOR = CL_RED,    /* 检查框打勾的颜色 */

    CHECK_BOX_H = 24, /* 检查框高度 */
    CHECK_BOX_W = 24, /* 检查框高度 */
};

/* 字体代码 */
typedef enum
{
    FC_ST_12 = 0, /* 宋体12x12点阵 （宽x高） */
    FC_ST_16,            /* 宋体15x16点阵 （宽x高） */
    FC_ST_24,            /* 宋体24x24点阵 （宽x高） -- 暂时未支持 */
    FC_ST_32,            /* 宋体32x32点阵 （宽x高） -- 暂时未支持 */

    FC_ST_62X40, /* 64x40点阵(高62，宽40)，ASCII字符 */
    FC_ST_96X40, /* 96x40点阵(高96，宽40)，ASCII字符 */

    FC_RA8875_16, /* RA8875 内置字体 16点阵 */
    FC_RA8875_24, /* RA8875 内置字体 24点阵 */
    FC_RA8875_32    /* RA8875 内置字体 32点阵 */
} FONT_CODE_E;

/* 字体属性结构, 用于LCD_DispStr() */
typedef struct
{
    FONT_CODE_E FontCode; /* 字体代码 FONT_CODE_E  */
    uint16_t FrontColor;    /* 字体颜色 */
    uint16_t BackColor;        /* 文字背景颜色，透明 */
    uint16_t Space;                /* 文字间距，单位 = 像素 */
} FONT_T;

/* 控件ID */
typedef enum
{
    ID_ICON = 1,
    ID_WIN = 2,
    ID_LABEL = 3,
    ID_BUTTON = 4,
    ID_CHECK = 5,
    ID_EDIT = 6,
    ID_GROUP = 7,
} CONTROL_ID_T;

/* 图标结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;     /* 左上角X坐标 */
    uint16_t Top;         /* 左上角Y坐标 */
    uint16_t Height; /* 图标高度 */
    uint16_t Width;    /* 图标宽度 */
    uint16_t *pBmp;    /* 指向图标图片数据 */
    char Text[16];     /* 图标文本, 最多显示5个汉字16点阵 */
} ICON_T;

/* 窗体结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;
    uint16_t Top;
    uint16_t Height;
    uint16_t Width;
    uint16_t Color;
    FONT_T *Font;
    char *pCaption;
} WIN_T;

/* 文本标签结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;     /* 左上角X坐标 */
    uint16_t Top;         /* 左上角Y坐标 */
    uint16_t Height; /* 高度 */
    uint16_t Width;    /* 宽度 */
    uint16_t MaxLen; /* 字符串长度 */
    FONT_T *Font;         /* 字体 */
    char *pCaption;
} LABEL_T;

/* 按钮结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;
    uint16_t Top;
    uint16_t Height;
    uint16_t Width;
    /* 按钮的颜色，由底层自动管理 */
    FONT_T *Font; /* 字体 */
    char *pCaption;
    uint8_t Focus; /* 焦点 */
} BUTTON_T;

/* 按钮的坐标和文本参数结构 */
typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t h;
    uint16_t w;
    char *text; /* 按钮的文字 */
} BTN_PARAM_T;

/* 图片按钮结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;
    uint16_t Top;
    uint16_t Height;
    uint16_t Width;
    uint32_t Pic1;
    uint32_t Pic2;
    uint8_t Focus; /* 焦点 */
} BMP_BUTTON_T;

/* 选中结构 */
typedef struct
{
    uint16_t Left;
    uint16_t Top;
    uint16_t Height;
    uint16_t Width;

} SELECT_T;

/* 编辑框结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;
    uint16_t Top;
    uint16_t Height;
    uint16_t Width;
    uint16_t Color;
    FONT_T *Font; /* 字体 */
    char *pCaption;
    uint8_t Focus;
    char Text[32 + 1]; /* 保存编辑框内的值 */
} EDIT_T;

/* 检查框 CHECK BOX 结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;     /* 左上角X坐标 */
    uint16_t Top;         /* 左上角Y坐标 */
    uint16_t Height; /* 高度 */
    uint16_t Width;    /* 宽度 */
    uint16_t Color;    /* 颜色 */
    FONT_T *Font;         /* 字体 */
    char *pCaption;
    uint8_t Checked; /* 1表示打勾 */
} CHECK_T;

/* 分组框GROUP BOX 结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;     /* 左上角X坐标 */
    uint16_t Top;         /* 左上角Y坐标 */
    uint16_t Height; /* 高度 */
    uint16_t Width;    /* 宽度 */
    FONT_T *Font;         /* 字体 */
    char *pCaption;
} GROUP_T;

/* Pannel面板结构 */
typedef struct
{
    uint8_t id;
    uint16_t Left;      /* 左上角X坐标 */
    uint16_t Top;       /* 左上角Y坐标 */
    uint16_t Height;    /* 高度 */
    uint16_t Width;     /* 宽度 */
    uint16_t Arc;       /* 圆角弧半径 */
    uint16_t Color;     /* 填充颜色 */
} PANNEL_T;

/* 多行文本框 */
typedef struct
{
    uint8_t id;
    uint16_t Left;      /* 左上角X坐标 */
    uint16_t Top;       /* 左上角Y坐标 */
    uint16_t Height;    /* 高度 */
    uint16_t Width;     /* 宽度 */
    uint16_t Arc;       /* 圆角弧半径 */
    uint16_t Color;     /* 填充颜色 */
    char *Text;         /* 执行文本区 */
    
    uint32_t MaxLen;    /* 文本最大长度 */
    uint16_t LineCount; /* 行数 */
    uint32_t Len;       /* 字符串长度 */
    uint32_t FifoRead;  /* FIFO读位置 */
    uint32_t FifoWrite; /* FIFO写位置 */
    FONT_T *Font;       /* 字体 */
    uint16_t Cursor;    /* 行光标, 保留未用 */
    uint8_t Refresh;    /* 1表示内容变化，需要显示刷新 */
    uint8_t WordWrap;   /* 1表示自动换行 */
    int32_t LineOffset; /* 查询时向前翻行的行数 */
}MEMO_T;

/* 背景光控制 */
#define BRIGHT_MAX 255
#define BRIGHT_MIN 0
#define BRIGHT_DEFAULT 255
#define BRIGHT_STEP 5

/* 可供外部模块调用的函数 */
void LCD_InitHard(void);
void LCD_GetChipDescribe(char *_str);
uint16_t LCD_GetHeight(void);
uint16_t LCD_GetWidth(void);
void LCD_DispOn(void);
void LCD_DispOff(void);
void LCD_ClrScr(uint16_t _usColor);
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont);
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY);
void LCD_DrawLine(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor);
void LCD_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor);
void LCD_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr);
void LCD_SetBackLight(uint8_t _bright);
uint8_t LCD_GetBackLight(void);

void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);

void LCD_DrawWin(WIN_T *_pWin);
void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode);
void LCD_DrawEdit(EDIT_T *_pEdit);
void LCD_DrawButton(BUTTON_T *_pBtn);
void LCD_DrawLabel(LABEL_T *_pLabel);
void LCD_DrawCheckBox(CHECK_T *_pCheckBox);
void LCD_DrawGroupBox(GROUP_T *_pBox);

void LCD_DispControl(void *_pControl);

void LCD_DrawIcon32(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode);
void LCD_DrawBmp32(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t *_pBmp);

uint16_t LCD_GetFontWidth(FONT_T *_tFont);
uint16_t LCD_GetFontHeight(FONT_T *_tFont);
uint16_t LCD_GetStrWidth(char *_ptr, FONT_T *_tFont);
void LCD_DispStrEx(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
                                     uint8_t _Align);

void LCD_SetDirection(uint8_t _dir);

uint8_t LCD_ButtonTouchDown(BUTTON_T *_btn, uint16_t _usX, uint16_t _usY);
uint8_t LCD_ButtonTouchRelease(BUTTON_T *_btn, uint16_t _usX, uint16_t _usY);
void LCD_InitButton(BUTTON_T *_btn, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w, char *_pCaption, FONT_T *_pFont);

void LCD_FillRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth,
                                             uint16_t _usRadius, uint16_t _usColor);
void LCD_DrawRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth,
                                             uint16_t _usRadius, uint16_t _usColor);

void LCD_InitMemo(MEMO_T *_pMemo);
void LCD_DrawMemo(MEMO_T *_pMemo);
void LCD_MemoAddStr(MEMO_T *_pMemo, char *_str);
void LCD_MemoAddChar(MEMO_T *_pMemo, char _ch);
void LCD_MemoClear(MEMO_T *_pMemo);
void LCD_MemoPageDown(MEMO_T *_pMemo, uint16_t  _LineNum);
void LCD_MemoPageUp(MEMO_T *_pMemo, uint16_t  _LineNum);

void LCD_SetEncode(uint8_t _code);
uint8_t LCD_GetEncode(void);

/* 下面3个变量，主要用于使程序同时支持不同的屏 */
extern uint16_t g_LcdHeight;     	/* 显示屏分辨率-高度 */
extern uint16_t g_LcdWidth;			/* 显示屏分辨率-宽度 */
extern uint8_t g_LcdDirection; 		/* 显示方向.0，1，2，3 */
extern uint8_t g_LcdSleepReq;		/* LCD休眠请求，用于硬件互斥 */

#endif
