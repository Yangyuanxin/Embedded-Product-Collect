/*
*********************************************************************************************************
*
*    模块名称 : TFT液晶显示器驱动模块
*    文件名称 : bsp_tft_lcd.c
*    版    本 : V5.1
*    说    明 : H7-TOOL 1.3寸屏专用
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2011-08-21 armfly  ST固件库版本 V3.5.0版本。
*                    a) 取消访问寄存器的结构体，直接定义
*        V2.0    2011-10-16 armfly  增加R61509V驱动，实现图标显示函数
*        V2.1    2012-07-06 armfly  增加RA8875驱动，支持4.3寸屏
*        V2.2    2012-07-13 armfly  改进LCD_DispStr函数，支持12点阵字符;修改LCD_DrawRect,解决差一个像素问题
*        V2.3    2012-08-08 armfly  将底层芯片寄存器操作相关的函数放到单独的文件，支持RA8875
*       V3.0    2013-05-20 增加图标结构; 修改    LCD_DrawIconActive  修改DispStr函数支持文本透明
*        V3.1    2013-06-12 解决LCD_DispStr()函数BUG，如果内嵌字库中汉字个数多于256，则出现死循环。
*        V3.2    2013-06-28 完善Label控件, 当显示字符串比之前短时，自动清除多余的文字
*        V3.3    2013-06-29 FSMC初始化时，配置时序，写时序和读时序分开设置。 LCD_FSMCConfig 函数。
*        V3.4    2013-07-06 增加显示32位带Alpha图标的函数 LCD_DrawIcon32
*        V3.5    2013-07-24 增加显示32位带Alpha图片的函数 LCD_DrawBmp32
*        V3.6    2013-07-30 修改 DispEdit() 支持12点阵汉字对齐
*        V3.7    2014-09-06 修改 LCD_InitHard() 同时支持 RA8875-SPI接口和8080接口
*        V3.8    2014-09-15 增加若干函数:
*                    （1） LCD_DispStrEx() 可以自动对齐自动填白的显示字符串函数
*                    （2） LCD_GetStrWidth() 计算字符串的像素宽度
*        V3.9    2014-10-18
*                    (1) 增加 LCD_ButtonTouchDown() LCD_ButtonTouchRelease 判断触摸坐标并重绘按钮
*                    (2) 增加3.5寸LCD驱动
*                    (3) 增加 LCD_SetDirection() 函数，设置显示屏方向（横屏 竖屏动态切换）
*        V4.0   2015-04-04 
*                (1) 按钮、编辑框控件增加RA8875字体，内嵌字库和RA8875字库统一编码。字体代码增加 
*                    FC_RA8875_16, FC_RA8875_24,    FC_RA8875_32
*                (2) FONT_T结构体成员FontCode的类型由 uint16_t 修改为 FONT_CODE_E枚举，便于编译器查错;
*                (3) 修改 LCD_DispStrEx(), 将读点阵的语句独立到函数：_LCD_ReadAsciiDot(), _LCD_ReadHZDot()
*                (4) LCD_DispStr() 函数简化，直接调用 LCD_DispStrEx() 实现。
*                (5) LCD_DispStrEx() 函数支持 RA8875字体。
*                (6) LCD_ButtonTouchDown() 增加按键提示音
*        V4.1   2015-04-18 
*                (1) 添加RA885 ASCII字体的宽度表。LCD_DispStrEx() 函数可以支持RA8875 ASCII变长宽度计算。
*                (2) 添加 LCD_HardReset(）函数，支持LCD复位由GPIO控制的产品。STM32-V5 不需要GPIO控制。
*        V4.2   2015-07-23
*                (1) 添加函数LCD_InitButton()
*                (2) h文件中使能按键提示音 #define BUTTON_BEEP()    BEEP_KeyTone();
*        V4.8   2019-03-23
*                (1) 新增 LCD_DispStrEx0
*        V5.0   2019-10-27 H7-TOOL产品 增加UTF-8编码字符串支持。USE_UTF8 == 1启用。  
*                    - LCD_DispStrEx0() 函数更新。 增加形参 _UTF8
*                    - LCD_GetStrWidth() 函数更新。
*                    - 显示字符串函数限制超出宽度 LCD_DispStrEx0()
*                    - \t字符串中的\t划线指令更换为\v指令. 避免和Lua print的\t冲突
*        V5.1   2019-12-29 
*                    - 解决LCD_DrawMemo bug, Text==0时禁止显示
*
*    Copyright (C), 2015-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "fonts.h"

#define    USE_UTF8    1    /* 1表示启用字符串的UTF-8编码 */

#if USE_UTF8 == 1
#include "ff.h"
#endif

#define USE_RA8875

/* 下面3个变量，主要用于使程序同时支持不同的屏 */
uint16_t g_LcdHeight = 128;         /* 显示屏分辨率-高度 */
uint16_t g_LcdWidth = 128;          /* 显示屏分辨率-宽度 */
uint8_t s_ucBright;                 /* 背光亮度参数 */
uint8_t g_LcdDirection = 0;         /* 显示方向.0，1，2，3 */

uint8_t g_LcdSleepReq = 0;          /* LCD休眠请求，用于SPI硬件互斥 */

uint8_t g_Encode = ENCODE_UTF8;     /* 缺省编码方式 */

static void LCD_HardReset(void);
static void LCD_SetPwmBackLight(uint8_t _bright);
static void LCD_DispStrEx0(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
                    uint8_t _Align);

#if 1
#define LCDX_InitHard ST7789_InitHard
#define LCDX_GetChipDescribe ST7789_GetChipDescribe
#define LCDX_ClrScr ST7789_ClrScr
#define LCDX_PutPixel ST7789_PutPixel
#define LCDX_GetPixel ST7789_GetPixel
#define LCDX_DrawLine ST7789_DrawLine
#define LCDX_DrawRect ST7789_DrawRect
#define LCDX_FillRect ST7789_FillRect
#define LCDX_DrawCircle ST7789_DrawCircle
#define LCDX_DrawBMP ST7789_DrawBMP
#define LCDX_QuitWinMode ST7789_QuitWinMode
#define LCDX_SetDirection ST7789_SetDirection
#else
#define LCDX_InitHard ST7735_InitHard
#define LCDX_GetChipDescribe ST7735_GetChipDescribe
#define LCDX_ClrScr ST7735_ClrScr
#define LCDX_PutPixel ST7735_PutPixel
#define LCDX_GetPixel ST7735_GetPixel
#define LCDX_DrawLine ST7735_DrawLine
#define LCDX_DrawRect ST7735_DrawRect
#define LCDX_FillRect ST7735_FillRect
#define LCDX_DrawCircle ST7735_DrawCircle
#define LCDX_DrawBMP ST7735_DrawBMP
#define LCDX_QuitWinMode ST7735_QuitWinMode
#define LCDX_SetDirection ST7735_SetDirection
#endif

/*
*********************************************************************************************************
*    函 数 名: LCD_InitHard
*    功能说明: 初始化LCD
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitHard(void)
{
    LCD_HardReset();        /* 硬件复位  */

    LCDX_InitHard();

    LCD_SetDirection(0);

#if 0 /* 此处不开背光。等主界面初始化完毕后再开，避免开机显示黑屏颜色不均 */
    LCD_ClrScr(CL_BLACK);    /* 清屏，显示全黑 */

    LCD_SetBackLight(BRIGHT_DEFAULT);     /* 打开背光，设置为缺省亮度 */
#endif
}

/*
*********************************************************************************************************
*    函 数 名: LCD_SetEncode
*    功能说明: 设置文字编码方式
*    形    参: _code:  ENCODE_UTF8, ENCODE_GBK
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetEncode(uint8_t _code)
{
    g_Encode = _code;   // g_Encode == ENCODE_UTF8
}

uint8_t LCD_GetEncode(void)
{
    return g_Encode;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_Task
*    功能说明: 管理LCD刷屏和休眠的任务
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern uint32_t wTransferState;
void LCD_Task(void)
{
    /* 在DMA SPI传输完毕后才能设置LCD休眠 */
    if (wTransferState == 1)
    {
        if (g_LcdSleepReq == 1)         /* 进入休眠。按键中断服务程序中会设置1 */
        {
            g_LcdSleepReq = 0;
            LCD_DispOff();              /* 该函数会操作SPI，不可以在中断服务程序中执行 */
        }
        else if (g_LcdSleepReq == 2)    /* 退出休眠。按键中断服务程序中会设置2 */
        {
            g_LcdSleepReq = 0;
            LCD_DispOn();               /* 该函数会操作SPI，不可以在中断服务程序中执行 */
        }
    }
    
    ST7789_DrawScreen();            /* 硬件SPI+DMA+刷屏 */
}

/*
*********************************************************************************************************
*    函 数 名: LCD_SetPwmBackLight
*    功能说明: 初始化控制LCD背景光的GPIO,配置为PWM模式。
*            当关闭背光时，将CPU IO设置为浮动输入模式（推荐设置为推挽输出，并驱动到低电平)；将TIM3关闭 省电
*    形    参:  _bright 亮度，0是灭，255是最亮
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetPwmBackLight(uint8_t _bright)
{
    /* 背光有CPU输出PWM控制，PA0/TIM5_CH1/TIM2_CH1 */
    bsp_SetTIMOutPWM(GPIOH, GPIO_PIN_9, TIM12, 2, 20000, (_bright * 10000) / 255);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_SetBackLight
*    功能说明: 初始化控制LCD背景光的GPIO,配置为PWM模式。
*            当关闭背光时，将CPU IO设置为浮动输入模式（推荐设置为推挽输出，并驱动到低电平)；将TIM3关闭 省电
*    形    参: _bright 亮度，0是灭，255是最亮
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetBackLight(uint8_t _bright)
{
    s_ucBright = _bright; /* 保存背光值 */

    LCD_SetPwmBackLight(_bright);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_GetBackLight
*    功能说明: 获得背光亮度参数
*    形    参: 无
*    返 回 值: 背光亮度参数
*********************************************************************************************************
*/
uint8_t LCD_GetBackLight(void)
{
    return s_ucBright;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_HardReset
*    功能说明: 硬件复位. 针对复位口线由GPIO控制的产品。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_HardReset(void)
{
#if 0    
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* 配置背光GPIO为推挽输出模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_PIN_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB, GPIO_PIN_1);
    bsp_DelayMS(20);
    GPIO_SetBits(GPIOB, GPIO_PIN_1);
#endif
}

/*
*********************************************************************************************************
*    函 数 名: LCD_SetDirection
*    功能说明: 设置显示屏显示方向（横屏 竖屏）
*    形    参: 显示方向代码 0 横屏正常, 1=横屏180度翻转, 2=竖屏, 3=竖屏180度翻转
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_SetDirection(uint8_t _dir)
{
    g_LcdDirection = _dir; /* 保存在全局变量 */

    LCDX_SetDirection(_dir);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_GetChipDescribe
*    功能说明: 读取LCD驱动芯片的描述符号，用于显示
*    形    参: char *_str : 描述符字符串填入此缓冲区
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_GetChipDescribe(char *_str)
{
    LCDX_GetChipDescribe(_str);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_GetHeight
*    功能说明: 读取LCD分辨率之高度
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD_GetHeight(void)
{
    return g_LcdHeight;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_GetWidth
*    功能说明: 读取LCD分辨率之宽度
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD_GetWidth(void)
{
    return g_LcdWidth;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DispOn
*    功能说明: 打开显示
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispOn(void)
{
    ST7789_DispOn();
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DispOff
*    功能说明: 关闭显示
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispOff(void)
{
    ST7789_DispOff();
}

/*
*********************************************************************************************************
*    函 数 名: LCD_ClrScr
*    功能说明: 根据输入的颜色值清屏
*    形    参: _usColor : 背景色
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_ClrScr(uint16_t _usColor)
{
    LCDX_ClrScr(_usColor);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DispStr
*    功能说明: 在LCD指定坐标（左上角）显示一个字符串
*    形    参:
*        _usX : X坐标
*        _usY : Y坐标
*        _ptr  : 字符串指针
*        _tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
{
    LCD_DispStrEx(_usX, _usY, _ptr,  _tFont, 0, 0);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DispStrEx, LCD_DispStrEx_GBK
*    功能说明: 在LCD指定坐标（左上角）显示一个字符串。 增强型函数。支持左\中\右对齐，支持定长清屏。 支持换行
*    形    参:
*        _usX : X坐标
*        _usY : Y坐标
*        _ptr  : 字符串指针
*        _tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数。可以指定RA8875字库显示汉字
*        _Width : 字符串显示区域的宽度. 0 表示不处理留白区域，此时_Align无效
*        _Align :字符串在显示区域的对齐方式，
*                ALIGN_LEFT = 0,
*                ALIGN_CENTER = 1,
*                ALIGN_RIGHT = 2
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispStrEx(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
                        uint8_t _Align)
{
    uint16_t i = 0;
    char str_buf[128] = {0};
    uint16_t len;
    uint16_t x, y;
    uint8_t ch;

    len = 0;
    x = _usX;
    y = _usY;
    str_buf[0] = 0;

    for (i = 0; i < 1024; i++)
    {
        ch = _ptr[i];

        if (ch == 0)
        {
            LCD_DispStrEx0(x, y, str_buf, _tFont, _Width, _Align);
            break;
        }
        else if (ch == '\r') /* 换行指令，后面2个字符表示垂直间距（单位像素） 支持重叠 */
        {
            uint8_t cap;

            LCD_DispStrEx0(x, y, str_buf, _tFont, _Width, _Align);

            len = 0;

            x = _usX;

            cap = (_ptr[i + 1] - '0') * 10 + _ptr[i + 2] - '0'; /* 间距 */
            y += cap;
            i += 2;
        }
        else if (ch == '\v') /* 2019-12-25 由t改为v 划线指令，后面8个字符表示 X1, Y2, X2,  Y2 00 99 02 02 */
        {
            uint16_t x1, x2, y1, y2;

            x1 = _usX + (_ptr[i + 1] - '0') * 10 + (_ptr[i + 2] - '0');
            y1 = _usY + (_ptr[i + 3] - '0') * 10 + (_ptr[i + 4] - '0');
            x2 = _usX + (_ptr[i + 5] - '0') * 10 + (_ptr[i + 6] - '0');
            y2 = _usY + (_ptr[i + 7] - '0') * 10 + (_ptr[i + 8] - '0');
            LCD_DrawLine(x1, y1, x2, y2, _tFont->FrontColor);

            i += 8;
        }
        else if (ch == '\n') /* 定位偏移，后面8个字符表示 X1, Y2 00 99 */
        {
            LCD_DispStrEx0(x, y, str_buf, _tFont, _Width, _Align);
            len = 0;
            
            x = _usX + (_ptr[i + 1] - '0') * 10 + (_ptr[i + 2] - '0');
            y = _usY + (_ptr[i + 3] - '0') * 10 + (_ptr[i + 4] - '0');            
            i += 4;
        }        
        else
        {
            if (len < sizeof(str_buf) - 1)
            {
                if (ch == '\t')     /* 支持tab，8字符定位 */
                {
                    uint8_t m;
                    
                    for (m = 0; m < 8; m++)
                    {
                        if ((len % 8) == 0)
                        {
                            break;
                        }
                        str_buf[len++] = ' ';
                    }
                }
                else
                {
                    str_buf[len++] = ch;
                }
                str_buf[len] = 0;
            }
        }
    }

}

/*
*********************************************************************************************************
*    函 数 名: LCD_GetFontWidth
*    功能说明: 读取字体的宽度（像素单位)
*    形    参:
*        _tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*    返 回 值: 字体的宽度（像素单位)
*********************************************************************************************************
*/
uint16_t LCD_GetFontWidth(FONT_T *_tFont)
{
    uint16_t font_width = 16;

    switch (_tFont->FontCode)
    {
        case FC_ST_12:
            font_width = 12;
            break;

        case FC_ST_16:
        case FC_RA8875_16:
            font_width = 16;
            break;

        case FC_RA8875_24:
        case FC_ST_24:
            font_width = 24;
            break;

        case FC_ST_32:
        case FC_RA8875_32:
            font_width = 32;
            break;

        case FC_ST_62X40:
            font_width = 40;
            break;

        case FC_ST_96X40:
            font_width = 40;
            break;
    }
    return font_width;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_GetFontHeight
*    功能说明: 读取字体的高度（像素单位)
*    形    参:
*        _tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*    返 回 值: 字体的宽度（像素单位)
*********************************************************************************************************
*/
uint16_t LCD_GetFontHeight(FONT_T *_tFont)
{
    uint16_t height = 16;

    switch (_tFont->FontCode)
    {
        case FC_ST_12:
            height = 12;
            break;

        case FC_ST_16:
        case FC_RA8875_16:
            height = 16;
            break;

        case FC_RA8875_24:
        case FC_ST_24:
            height = 24;
            break;

        case FC_ST_32:
        case FC_RA8875_32:
            height = 32;
            break;

        case FC_ST_62X40:
            height = 62;
            break;

        case FC_ST_96X40:
            height = 96;
            break;
    }
    return height;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_GetStrWidth, LCD_GetStrWidth_GBK
*    功能说明: 计算字符串宽度(像素单位)
*    形    参:
*        _ptr  : 字符串指针
*        _tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*    返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD_GetStrWidth(char *_ptr, FONT_T *_tFont)
{
    char *p = _ptr;
    uint16_t width = 0;
    uint8_t code1, code2;
    uint16_t font_width;
    uint16_t m;
    uint16_t address;
    uint8_t a_flag = 0;

    font_width = LCD_GetFontWidth(_tFont);

    while (*p != 0)
    {
        code1 = *p;                /* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
        if (code1 < 0x80) /* ASCII */
        {
            if (code1 == '\a')
            {
                a_flag = 1;
                p++;
                code1 = *p;
            }
            else
            {
                a_flag = 0;
            }

            switch (_tFont->FontCode)
            {
                case FC_RA8875_16:
                    font_width = g_RA8875_Ascii16_width[code1 - 0x20];
                    break;

                case FC_RA8875_24:
                    if (a_flag == 0)
                    {
                        font_width = g_RA8875_Ascii24_width[code1 - 0x20];
                    }
                    else
                    {
                        {
                            m = 0;
                            while (1)
                            {
                                address = m * (72 + 2);
                                m++;
                                if (code1 == g_Ascii24_VarWidth[address + 0])
                                {
                                    font_width = g_Ascii24_VarWidth[address + 1];
                                    break;
                                }
                                else if ((g_Ascii24_VarWidth[address + 0] == 0xFF) && (g_Ascii24_VarWidth[address + 1] == 0xFF))
                                {
                                    //                              /* 字库搜索完毕，未找到，则填充全FF */
                                    //                              memset(g_Ascii32_VarWidth, 0xFF, 128);
                                    break;
                                }
                            }
                        }
                    }
                    break;

                case FC_RA8875_32:
                    if (a_flag == 0)
                    {
                        font_width = g_RA8875_Ascii32_width[code1 - 0x20];
                    }
                    else
                    {
                        {
                            m = 0;
                            while (1)
                            {
                                address = m * (128 + 2);
                                m++;
                                if (code1 == g_Ascii32_VarWidth[address + 0])
                                {
                                    font_width = g_Ascii32_VarWidth[address + 1];
                                    break;
                                }
                                else if ((g_Ascii32_VarWidth[address + 0] == 0xFF) && (g_Ascii32_VarWidth[address + 1] == 0xFF))
                                {
                                    //                              /* 字库搜索完毕，未找到，则填充全FF */
                                    //                              memset(g_Ascii32_VarWidth, 0xFF, 128);
                                    break;
                                }
                            }
                        }
                    }
                    break;

                case FC_ST_12:
                    font_width = 6;
                    break;

                case FC_ST_16:
                    font_width = 8;
                    break;

                case FC_ST_24:
                    font_width = 12;
                    break;

                case FC_ST_32:
                    font_width = 16;
                    break;

                case FC_ST_62X40:
                    //对秒进行特殊处理
                    if (code1 == 0x5E)
                    {
                        font_width = 28;
                    }
                    else
                    {
                        font_width = 40;
                    }
                    break;

                case FC_ST_96X40:
                    //对秒进行特殊处理
                    if (code1 == 0x5E)
                    {
                        font_width = 28;
                    }
                    else
                    {
                        font_width = 40;
                    }
                    break;

                default:
                    font_width = 8;
                    break;
            }
        }
        else /* 汉字 */
        {
            if (g_Encode == ENCODE_UTF8)     /* UTF-8 */
            {
                /* 解读 UTF-8 编码非常简单。
                    如果一个字节的第一位是0，则这个字节单独就是一个字符；如果第一位是1，则连续有多少个1，就表示当前字符占用多少个字节。
                    UNICODE 最后一个二进制位开始，依次从后向前填入格式中的x，多出的位补0
            
                    110XXXXX  10XXXXXX           -- 支持
                    1110XXXX  10XXXXXX 10XXXXXX  -- 支持
                    11110XXX  10XXXXXX 10XXXXXX 10XXXXXX  -- 本转换程序不支持
                */
                {            
                    uint8_t code3;
                    
                    if ((code1 & 0xE0) == 0xC0)    /* 2字节 */
                    {
                        code2 = *++p;
                        if (code2 == 0)
                        {
                            break;
                        }                                                    
                    }
                    else if ((code1 & 0xF0) == 0xE0)    /* 3字节 */
                    {
                        code2 = *++p;
                        code3 = *++p;
                        if (code2 == 0 || code3 == 0)
                        {
                            break;
                        }
                    }
                    else if ((code1 & 0xF8) == 0xF0)    /* 4字节 */
                    {
                        code2 = *++p;
                        if (code2 == 0)
                        {
                            break;
                        }                            
                    }    
                    else
                    {
                        code2 = *++p;
                        if (code2 == 0)
                        {
                            break;
                        }                            
                    }
                }
            }
            else     /* GBK */       
            {
                code2 = *++p;
                if (code2 == 0)
                {
                    break;
                }
            }
            font_width = LCD_GetFontWidth(_tFont);
        }
        width += (font_width + _tFont->Space);
        p++;
    }

    return width;
}

/*
*********************************************************************************************************
*    函 数 名: _LCD_ReadSmallDot
*    功能说明: 读取1个小语种字符的点阵数据
*    形    参:
*        _code : ASCII字符的编码，1字节。1-128
*        _fontcode ：字体代码
*        _pBuf : 存放读出的字符点阵数据
*    返 回 值: 文字宽度
*********************************************************************************************************
*/
static void _LCD_ReadSmallDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT /* 使用CPU 内部Flash 小字库 */
    const uint8_t *pAscDot;
    uint32_t font_bytes = 0;
    uint16_t m;
    uint16_t address;
    uint8_t fAllHz = 0; /* 1表示程序中内嵌全部的ASCII字符集 */

    pAscDot = 0;
    switch (_fontcode)
    {
        case FC_ST_12: /* 12点阵 */
            font_bytes = 24 / 2;
            pAscDot = g_Ascii12;
            fAllHz = 1;
            break;

        case FC_ST_16:
            /* 缺省是16点阵 */
            font_bytes = 32 / 2;
            pAscDot = g_Ascii16;
            fAllHz = 1;
            break;

        case FC_ST_24:
            font_bytes = 48;
            pAscDot = g_Ascii24;
            break;

        case FC_ST_32:
            font_bytes = 64;
            pAscDot = g_Ascii32;
            break;

        case FC_ST_62X40:
            font_bytes = 310;
            pAscDot = g_Ascii62x40;
            break;

        case FC_ST_96X40:
            font_bytes = 480;
            pAscDot = g_Ascii96x40;
            break;

        case FC_RA8875_24:
            font_bytes = 72;
            pAscDot = g_Ascii24_VarWidth;
            fAllHz = 2;
            break;

        case FC_RA8875_32:
            font_bytes = 128;
            pAscDot = g_Ascii32_VarWidth;
            fAllHz = 2;
            break;

        default:
            return;
    }

    if (fAllHz == 1) /* 内嵌全部ASCII字符点阵 */
    {
        /* 将CPU内部Flash中的ascii字符点阵复制到buf */
        memcpy(_pBuf, &pAscDot[_code * (font_bytes)], (font_bytes));
    }
    else if (fAllHz == 2)
    {
        m = 0;
        while (1)
        {
            address = m * (font_bytes + 2);
            m++;
            if (_code == pAscDot[address + 0])
            {
                address += 2;
                memcpy(_pBuf, &pAscDot[address], font_bytes);
                break;
            }
            else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
            {
                /* 字库搜索完毕，未找到，则填充全FF */
                memset(_pBuf, 0xFF, font_bytes);
                break;
            }
        }
    }
    else /* 内嵌部分字符，字模数组首字节是ASCII码 */
    {
        m = 0;
        while (1)
        {
            address = m * (font_bytes + 1);
            m++;
            if (_code == pAscDot[address + 0])
            {
                address += 1;
                memcpy(_pBuf, &pAscDot[address], font_bytes);
                break;
            }
            else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
            {
                /* 字库搜索完毕，未找到，则填充全FF */
                memset(_pBuf, 0xFF, font_bytes);
                break;
            }
        }
    }
#else /* 用全字库 */
    uint32_t pAscDot;
    uint8_t font_bytes = 0;

    pAscDot = 0;
    switch (_fontcode)
    {
        case FC_ST_12: /* 12点阵 */
            font_bytes = 12;
    #if 0
            pAscDot = ASC12_ADDR;    /* 字库芯片的16点阵字符不好看,笔画细了，而且是非等宽字体 */
    #else
            pAscDot = (uint32_t)&g_Ascii12[' ' * 12]; /* 使用CPU内嵌的16点阵字符 */
    #endif
            break;

        case FC_ST_16:
            font_bytes = 16;
    #if 0
                    pAscDot = ASC16_ADDR;    /* 字库芯片的16点阵字符不好看,笔画细了，而且是非等宽字体 */
    #else
            pAscDot = (uint32_t)&g_Ascii16[' ' * 16]; /* 使用CPU内嵌的16点阵字符 */
    #endif
            break;

        case FC_ST_24:
            font_bytes = 48;
            pAscDot = ASC24_ADDR;
            break;

        case FC_ST_32:
            font_bytes = 64;
            pAscDot = ASC32_ADDR;
            break;

        default:
            return;
    }
    if (_code >= 0x20 && _code <= 0x7E)
    {
        pAscDot = ((uint32_t)_code - 0x20) * font_bytes + pAscDot;
    }

#ifdef USE_NOR_FONT /* NOR Flash全字库 */
    /* 将CPU内部Flash中的ascii字符点阵复制到buf */
    memcpy(_pBuf, (char *)pAscDot, font_bytes);
#endif

#ifdef USE_SPI_FONT /* 串行 Flash全字库 */
    if (_fontcode == FC_ST_12 || _fontcode == FC_ST_16)
    {
        memcpy(_pBuf, (char *)pAscDot, font_bytes);
    }
    else
    {
        /* 字库芯片的12点阵和16点阵字符不好看,笔画细了，而且是非等宽字体 */
        sf_ReadBuffer(_pBuf, pAscDot, font_bytes);
    }
#endif

#endif
}

/*
*********************************************************************************************************
*    函 数 名: _LCD_ReadAsciiDot
*    功能说明: 读取1个ASCII字符的点阵数据
*    形    参:
*        _code : ASCII字符的编码，1字节。1-128
*        _fontcode ：字体代码
*        _pBuf : 存放读出的字符点阵数据
*    返 回 值: 文字宽度
*********************************************************************************************************
*/
static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT /* 使用CPU 内部Flash 小字库 */
    const uint8_t *pAscDot;
    uint32_t font_bytes = 0;
    uint16_t m;
    uint16_t address;
    uint8_t fAllHz = 0; /* 1表示程序中内嵌全部的ASCII字符集 */

    pAscDot = 0;
    switch (_fontcode)
    {
        case FC_ST_12: /* 12点阵 */
            font_bytes = 24 / 2;
            pAscDot = g_Ascii12;
            fAllHz = 1;
            break;

        case FC_ST_16:
            /* 缺省是16点阵 */
            font_bytes = 32 / 2;
            pAscDot = g_Ascii16;
            fAllHz = 1;
            break;

        case FC_ST_24:
            font_bytes = 48;
            pAscDot = g_Ascii24;
            break;

        case FC_ST_32:
            font_bytes = 64;
            pAscDot = g_Ascii32;
            break;

        case FC_ST_62X40:
            font_bytes = 310;
            pAscDot = g_Ascii62x40;
            break;

        case FC_ST_96X40:
            font_bytes = 480;
            pAscDot = g_Ascii96x40;
            break;

        default:
            return;
    }

    if (fAllHz == 1) /* 内嵌全部ASCII字符点阵 */
    {
        /* 将CPU内部Flash中的ascii字符点阵复制到buf */
        memcpy(_pBuf, &pAscDot[_code * (font_bytes)], (font_bytes));
    }
    else /* 内嵌部分字符，字模数组首字节是ASCII码 */
    {
        m = 0;
        while (1)
        {
            address = m * (font_bytes + 1);
            m++;
            if (_code == pAscDot[address + 0])
            {
                address += 1;
                memcpy(_pBuf, &pAscDot[address], font_bytes);
                break;
            }
            else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
            {
                /* 字库搜索完毕，未找到，则填充全FF */
                memset(_pBuf, 0xFF, font_bytes);
                break;
            }
        }
    }
#else /* 用全字库 */
    uint32_t pAscDot;
    uint8_t font_bytes = 0;

    pAscDot = 0;
    switch (_fontcode)
    {
        case FC_ST_12: /* 12点阵 */
            font_bytes = 12;
    #if 0
                    pAscDot = ASC12_ADDR;    /* 字库芯片的16点阵字符不好看,笔画细了，而且是非等宽字体 */
    #else
            pAscDot = (uint32_t)&g_Ascii12[' ' * 12]; /* 使用CPU内嵌的16点阵字符 */
    #endif
            break;

        case FC_ST_16:
            font_bytes = 16;
    #if 0
                    pAscDot = ASC16_ADDR;    /* 字库芯片的16点阵字符不好看,笔画细了，而且是非等宽字体 */
    #else
            pAscDot = (uint32_t)&g_Ascii16[' ' * 16]; /* 使用CPU内嵌的16点阵字符 */
    #endif
            break;

        case FC_ST_24:
            font_bytes = 48;
            pAscDot = ASC24_ADDR;
            break;

        case FC_ST_32:
            font_bytes = 64;
            pAscDot = ASC32_ADDR;
            break;

        default:
            return;
    }
    if (_code >= 0x20 && _code <= 0x7E)
    {
        pAscDot = ((uint32_t)_code - 0x20) * font_bytes + pAscDot;
    }

#ifdef USE_NOR_FONT /* NOR Flash全字库 */
    /* 将CPU内部Flash中的ascii字符点阵复制到buf */
    memcpy(_pBuf, (char *)pAscDot, font_bytes);
#endif

#ifdef USE_SPI_FONT /* 串行 Flash全字库 */
    if (_fontcode == FC_ST_12 || _fontcode == FC_ST_16)
    {
        memcpy(_pBuf, (char *)pAscDot, font_bytes);
    }
    else
    {
        /* 字库芯片的12点阵和16点阵字符不好看,笔画细了，而且是非等宽字体 */
        sf_ReadBuffer(_pBuf, pAscDot, font_bytes);
    }
#endif

#endif
}

/*
*********************************************************************************************************
*    函 数 名: _LCD_ReadHZDotQSPI
*    功能说明: 读取1个汉字的点阵数据， 在QSPI Flash字库中读取
*    形    参:
*        _code1, _cod2 : 汉字内码. GB2312编码
*        _fontcode ：字体代码
*        _pBuf : 存放读出的字符点阵数据
*    返 回 值: 无
*********************************************************************************************************
*/
static void _LCD_ReadHZDotQSPI(uint8_t _code1, uint8_t _code2, uint8_t _fontcode, uint8_t *_pBuf)
{
    uint32_t offset = 0;
    uint8_t font_bytes = 0;

    switch (_fontcode)
    {
        case FC_ST_12: /* 12点阵 */
            font_bytes = 24;
            offset = HZK12_ADDR;
            break;

        case FC_ST_16:
            font_bytes = 32;
            offset = HZK16_ADDR;
            break;

        case FC_ST_24:
            font_bytes = 72;
            offset = HZK24_ADDR;
            break;

        case FC_ST_32:
            font_bytes = 128;
            offset = HZK32_ADDR;
            break;

        default:
            return;
    }

    /* 此处需要根据字库文件存放位置进行修改 
        GB2312范围： 0xA1A1 - 0xFEFE
        其中汉字范围 : 0xB0A1 - 0xF7FE
    
        GBK 范围： 0x8140 - 0xFEFE 
    
        安富莱自定义汉字编码错开GBK和GB2312编码空间： 0x8000 - 0x813F （319个）        
    */
    if (_code1 >= 0xA1 && _code1 <= 0xA9 && _code2 >= 0xA1)
    {
        offset += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
    }
    else if (_code1 >= 0xB0 && _code1 <= 0xF7 && _code2 >= 0xA1)
    {
        offset += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
    }
    else /* 2018-03-13 增加自定义汉字编码，用于实现特殊图标符号 */
    {
        uint16_t code16;
        uint8_t *pDot;
        uint32_t address;
        uint16_t m;

        code16 = _code1 * 256 + _code2;
        if (code16 >= 0x8000 && code16 <= 0x813F) /* 自定义汉字点阵，固定使用CPU片内部小字库 */
        {
            pDot = 0; /* 仅仅用于避免告警 */
            switch (_fontcode)
            {
                case FC_ST_12: /* 12点阵 */
                    font_bytes = 24;
                    pDot = (uint8_t *)g_Hz12;
                    break;

                case FC_ST_16:
                    font_bytes = 32;
                    pDot = (uint8_t *)g_Hz16;
                    break;

                case FC_ST_24:
                    font_bytes = 72;
                    pDot = (uint8_t *)g_Hz24;
                    break;

                case FC_ST_32:
                    font_bytes = 128;
                    pDot = (uint8_t *)g_Hz32;
                    break;

                default:
                    break;
            }

            m = 0;
            while (1)
            {
                address = m * (font_bytes + 2);
                m++;
                if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
                {
                    address += 2;
                    memcpy(_pBuf, &pDot[address], font_bytes);
                    break;
                }
                else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
                {
                    /* 字库搜索完毕，未找到，则填充全FF */
                    memset(_pBuf, 0xFF, font_bytes);
                    break;
                }
            }
            return;
        }
    }

    /* 将CPU内部Flash中的ascii字符点阵复制到buf */
    //memcpy(_pBuf, (char *)offset, font_bytes); 内存映射模式未调通，数据错乱
    QSPI_ReadBuffer(_pBuf, offset, font_bytes);
}

/*
*********************************************************************************************************
*    函 数 名: _LCD_ReadHZDot
*    功能说明: 读取1个汉字的点阵数据
*    形    参:
*        _code1, _cod2 : 汉字内码. GB2312编码
*        _fontcode ：字体代码
*        _pBuf : 存放读出的字符点阵数据
*    返 回 值: 无
*********************************************************************************************************
*/
static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT /* 使用CPU 内部Flash 小字库 */
    uint8_t *pDot;
    uint8_t font_bytes = 0;
    uint32_t address;
    uint16_t m;

    pDot = 0; /* 仅仅用于避免告警 */
    switch (_fontcode)
    {
        case FC_ST_12: /* 12点阵 */
            font_bytes = 24;
            pDot = (uint8_t *)g_Hz12;
            break;

        case FC_ST_16:
            font_bytes = 32;
            pDot = (uint8_t *)g_Hz16;
            break;

        case FC_ST_24:
            font_bytes = 72;
            pDot = (uint8_t *)g_Hz24;
            break;

        case FC_ST_32:
            font_bytes = 128;
            pDot = (uint8_t *)g_Hz32;
            break;

        default:
            return;
    }

    m = 0;
    while (1)
    {
        address = m * (font_bytes + 2);
        m++;
        if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
        {
            address += 2;
            memcpy(_pBuf, &pDot[address], font_bytes);
            break;
        }
        else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
        {
            #if 1    /* 2019-12-24，H7-TOOL，未找到则去QSPI字库寻找 */
                _LCD_ReadHZDotQSPI(_code1, _code2, _fontcode, _pBuf);
            #else            
                /* 字库搜索完毕，未找到，则填充全FF */
                memset(_pBuf, 0xFF, font_bytes);    
            #endif
            break;
        }
    }
#else /* 用全字库 */
    uint32_t offset = 0;
    uint8_t font_bytes = 0;

    switch (_fontcode)
    {
        case FC_ST_12: /* 12点阵 */
            font_bytes = 24;
            offset = HZK12_ADDR;
            break;

        case FC_ST_16:
            font_bytes = 32;
            offset = HZK16_ADDR;
            break;

        case FC_ST_24:
            font_bytes = 72;
            offset = HZK24_ADDR;
            break;

        case FC_ST_32:
            font_bytes = 128;
            offset = HZK32_ADDR;
            break;

        default:
            return;
    }

    /* 此处需要根据字库文件存放位置进行修改 
        GB2312范围： 0xA1A1 - 0xFEFE
        其中汉字范围 : 0xB0A1 - 0xF7FE
    
        GBK 范围： 0x8140 - 0xFEFE 
    
        安富莱自定义汉字编码错开GBK和GB2312编码空间： 0x8000 - 0x813F （319个）        
    */
    if (_code1 >= 0xA1 && _code1 <= 0xA9 && _code2 >= 0xA1)
    {
        offset += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
    }
    else if (_code1 >= 0xB0 && _code1 <= 0xF7 && _code2 >= 0xA1)
    {
        offset += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
    }
    else /* 2018-03-13 增加自定义汉字编码，用于实现特殊图标符号 */
    {
        uint16_t code16;
        uint8_t *pDot;
        uint32_t address;
        uint16_t m;

        code16 = _code1 * 256 + _code2;
        if (code16 >= 0x8000 && code16 <= 0x813F) /* 自定义汉字点阵，固定使用CPU片内部小字库 */
        {
            pDot = 0; /* 仅仅用于避免告警 */
            switch (_fontcode)
            {
                case FC_ST_12: /* 12点阵 */
                    font_bytes = 24;
                    pDot = (uint8_t *)g_Hz12;
                    break;

                case FC_ST_16:
                    font_bytes = 32;
                    pDot = (uint8_t *)g_Hz16;
                    break;

                case FC_ST_24:
                    font_bytes = 72;
                    pDot = (uint8_t *)g_Hz24;
                    break;

                case FC_ST_32:
                    font_bytes = 128;
                    pDot = (uint8_t *)g_Hz32;
                    break;

                default:
                    break;
            }

            m = 0;
            while (1)
            {
                address = m * (font_bytes + 2);
                m++;
                if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
                {
                    address += 2;
                    memcpy(_pBuf, &pDot[address], font_bytes);
                    break;
                }
                else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
                {
                    /* 字库搜索完毕，未找到，则填充全FF */
                    memset(_pBuf, 0xFF, font_bytes);
                    break;
                }
            }
            return;
        }
    }

#ifdef USE_NOR_FONT /* NOR Flash全字库 */
    /* 将CPU内部Flash中的ascii字符点阵复制到buf */
    memcpy(_pBuf, (char *)offset, font_bytes);
#endif

#ifdef USE_SPI_FONT /* NOR Flash全字库 */
    sf_ReadBuffer(_pBuf, offset, font_bytes);
#endif

#endif
}

/*
*********************************************************************************************************
*    函 数 名: SeachStr_a
*    功能说明: 搜索一个字符串是否有‘\a’
*    形    参:
*        _ptr  : 字符串指针
*    返 回 值: 无
*********************************************************************************************************
*/
//static uint8_t SeachStr_a(char *_ptr)
//{
//    uint16_t j = 0;
//    uint8_t a_flag = 0;
//
//    while(_ptr[j] != 0)
//    {
//        if (_ptr[j] == '\a')
//        {
//            a_flag = 1;
//            break;
//        }
//        j++;
//    }
//
//    return a_flag;
//}

/*
*********************************************************************************************************
*    函 数 名: LCD_DispStrEx0
*    功能说明: 在LCD指定坐标（左上角）显示一个字符串。 增强型函数。支持左\中\右对齐，支持定长清屏。
*    形    参:
*        _usX : X坐标
*        _usY : Y坐标
*        _ptr  : 字符串指针
*        _tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数。可以指定RA8875字库显示汉字
*        _Width : 字符串显示区域的宽度. 0 表示不处理留白区域，此时_Align无效
*        _Align :字符串在显示区域的对齐方式，
*                ALIGN_LEFT = 0,
*                ALIGN_CENTER = 1,
*                ALIGN_RIGHT = 2
*    返 回 值: 无
*********************************************************************************************************
*/
ALIGN_32BYTES(uint8_t g_DotBuf[96 * 40 / 8]); /* 最大支持96x40点阵字符 */


static void LCD_DispStrEx0(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
                    uint8_t _Align)
{
    uint32_t i;
    uint8_t code1;
    uint8_t code2;
    uint8_t width;
    uint16_t m;
    uint8_t font_width = 0;
    uint8_t font_height = 0;
    uint16_t x, y;
    uint16_t offset;
    uint16_t str_width; /* 字符串实际宽度  */
                                            //    uint8_t ra8875_use = 0;
                                            //    uint8_t ra8875_font_code = 0;
    uint16_t address;
    uint8_t a_flag = 0;
    uint8_t RA8875_flag = 0;

    uint8_t line_bytes;
    uint8_t asc_bytes = 0;
    uint8_t hz_bytes = 0;
    
    uint16_t xMax;

    if (_Width > 0)
    {
        xMax = _usX + _Width - 1;
    }
    else
    {
        xMax = 1920;
    }
    
    switch (_tFont->FontCode)
    {
        case FC_ST_12: /* 12点阵 */
            font_height = 12;
            font_width = 12;
            asc_bytes = 1;
            hz_bytes = 2;
            break;

        case FC_ST_16:
            font_height = 16;
            font_width = 16;
            asc_bytes = 1;
            hz_bytes = 2;
            break;

        case FC_ST_24:
            font_height = 24;
            font_width = 24;
            asc_bytes = 2;
            hz_bytes = 3;
            break;

        case FC_ST_32:
            font_height = 32;
            font_width = 32;
            asc_bytes = 2;
            hz_bytes = 4;
            break;

        case FC_ST_62X40:
            font_height = 62;
            font_width = 80;
            asc_bytes = 5;
            hz_bytes = 10;
            break;

        case FC_ST_96X40:
            font_height = 96;
            font_width = 80;
            asc_bytes = 5;
            hz_bytes = 10;
            break;
        
        default:
            font_height = 16;
            font_width = 16;
            asc_bytes = 1;
            hz_bytes = 2;
            break;        
    }

    str_width = LCD_GetStrWidth(_ptr, _tFont); /* 计算字符串实际宽度(RA8875内部ASCII点阵宽度为变长 */
    offset = 0;
    if (_Width > str_width)
    {
        if (_Align == ALIGN_RIGHT) /* 右对齐 */
        {
            offset = _Width - str_width;
        }
        else if (_Align == ALIGN_CENTER) /* 左对齐 */
        {
            offset = (_Width - str_width) / 2;
        }
        else /* 左对齐 ALIGN_LEFT */
        {
            ;
        }
    }

    /* 左侧填背景色, 中间对齐和右边对齐  */
    if (offset > 0)
    {
        if (_tFont->BackColor != CL_MASK) /* 透明色 */
        {
            LCD_Fill_Rect(_usX, _usY, LCD_GetFontHeight(_tFont), offset, _tFont->BackColor);
        }
        _usX += offset;
    }

    /* 右侧填背景色 */
    if (_Width > str_width)
    {
        if (_tFont->BackColor != CL_MASK) /* 透明色 */
        {
            LCD_Fill_Rect(_usX + str_width, _usY, LCD_GetFontHeight(_tFont), _Width - str_width - offset, _tFont->BackColor);
        }
    }

    /* 使用CPU内部字库. 点阵信息由CPU读取 */
    {
        /* 开始循环处理字符 */
        while (*_ptr != 0)
        {
            code1 = *_ptr; /* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
            if (code1 < 0x80)
            {
                if (a_flag == 0)
                {
                    RA8875_flag = 0;
                    /* 将ascii字符点阵复制到buf */
                    _LCD_ReadAsciiDot(code1, _tFont->FontCode, g_DotBuf); /* 读取ASCII字符点阵 */

                    //对秒进行特殊处理,避免宽度过大
                    if (_tFont->FontCode == FC_ST_62X40 || _tFont->FontCode == FC_ST_96X40)
                    {
                        if (code1 == 0x5E)
                        {
                            width = 28;
                        }
                        else
                        {
                            width = font_width / 2;
                        }
                    }
                    else
                    {
                        width = font_width / 2;
                    }

                    line_bytes = asc_bytes;
                }
                else
                {
                    if (code1 == '\a')
                    {
                        RA8875_flag = 0;
                        _ptr++;
                        code1 = *_ptr;
                        if (_tFont->FontCode == FC_RA8875_32)
                        {
                            m = 0;
                            while (1)
                            {
                                address = m * (128 + 2);
                                m++;
                                if (code1 == g_Ascii32_VarWidth[address + 0])
                                {
                                    font_width = g_Ascii32_VarWidth[address + 1];
                                    break;
                                }
                                else if ((g_Ascii32_VarWidth[address + 0] == 0xFF) && (g_Ascii32_VarWidth[address + 1] == 0xFF))
                                {
                                    //                              /* 字库搜索完毕，未找到，则填充全FF */
                                    //                              memset(g_Ascii32_VarWidth, 0xFF, 128);
                                    break;
                                }
                            }
                        }
                        else if (_tFont->FontCode == FC_RA8875_24)
                        {
                            m = 0;
                            while (1)
                            {
                                address = m * (72 + 2);
                                m++;
                                if (code1 == g_Ascii24_VarWidth[address + 0])
                                {
                                    font_width = g_Ascii24_VarWidth[address + 1];
                                    break;
                                }
                                else if ((g_Ascii24_VarWidth[address + 0] == 0xFF) && (g_Ascii24_VarWidth[address + 1] == 0xFF))
                                {
                                    //                              /* 字库搜索完毕，未找到，则填充全FF */
                                    //                              memset(g_Ascii32_VarWidth, 0xFF, 128);
                                    break;
                                }
                            }
                        }
                        _LCD_ReadSmallDot(code1, _tFont->FontCode, g_DotBuf);                                            

                        width = font_width;

                        line_bytes = asc_bytes;
                    }
                    //                    else
                    //                    {
                    //                        RA8875_flag = 1;
                    //                        if (_tFont->FontCode == FC_RA8875_32)
                    //                        {
                    //                            font_width = g_RA8875_Ascii32_width[code1 - 0x20];
                    //                        }
                    //                        else if (_tFont->FontCode == FC_RA8875_24)
                    //                        {
                    //                            font_width = g_RA8875_Ascii24_width[code1 - 0x20];
                    //                        }
                    //                        width = font_width;
                    //                        line_bytes = asc_bytes;
                    //                    }
                }
            }
            else
            {
                RA8875_flag = 0;
                
                if (g_Encode == ENCODE_UTF8)     /* UTF-8 */
                {
                    /* 解读 UTF-8 编码非常简单。
                        如果一个字节的第一位是0，则这个字节单独就是一个字符；如果第一位是1，则连续有多少个1，就表示当前字符占用多少个字节。
                        UNICODE 最后一个二进制位开始，依次从后向前填入格式中的x，多出的位补0
                
                        110XXXXX  10XXXXXX           -- 支持
                        1110XXXX  10XXXXXX 10XXXXXX  -- 支持
                        11110XXX  10XXXXXX 10XXXXXX 10XXXXXX  -- 本转换程序不支持
                    */
                    {            
                        uint8_t code3;
                        uint32_t unicode1;
                        uint16_t gb;
                        
                        if ((code1 & 0xE0) == 0xC0)    /* 2字节 */
                        {
                            code2 = *++_ptr;
                            if (code2 == 0)
                            {
                                break;
                            }                            
                            unicode1 = ((uint32_t)(code1 & 0x1F) << 6) + (code2 & 0x3F);                            
                        }
                        else if ((code1 & 0xF0) == 0xE0)    /* 3字节 */
                        {
                            code2 = *++_ptr;
                            code3 = *++_ptr;
                            if (code2 == 0 || code3 == 0)
                            {
                                break;
                            }
                            unicode1 = ((uint32_t)(code1 & 0x0F) << 12) + ((uint32_t)(code2 & 0x3F) << 6) + (code3 & 0x3F);
                        }
                        else if ((code1 & 0xF8) == 0xF0)    /* 4字节 */
                        {
                            code2 = *++_ptr;
                            if (code2 == 0)
                            {
                                break;
                            }                            
                        }    
                        else
                        {
                            code2 = *++_ptr;
                            if (code2 == 0)
                            {
                                break;
                            }                            
                        }
                        
                        /* 将UNICODE码转换为GB2312 */
                        if (unicode1 > 0xFFFF)
                        {
                            break;
                        }
                        gb = ff_convert(unicode1, 0);    /* Unicode -> OEM */
                        
                        code1 = gb >> 8;
                        code2 = gb;
                    }
                }
                else    /* GBK */
                {
                    code2 = *++_ptr;                
                    if (code2 == 0)
                    {
                        break;
                    }
                }
                
                /* 读1个汉字的点阵 */
                _LCD_ReadHZDot(code1, code2, _tFont->FontCode, g_DotBuf);
                    
                SCB_InvalidateDCache_by_Addr((uint32_t *)g_DotBuf,  sizeof(g_DotBuf));
                    
                width = font_width;
                line_bytes = hz_bytes;
            }

            y = _usY;
            if (RA8875_flag == 0)
            {
                /* 开始刷LCD */
                for (m = 0; m < font_height; m++) /* 字符高度 */
                {
                    x = _usX;
                    for (i = 0; i < width; i++) /* 字符宽度 */
                    {
                        if (x < xMax)
                        {                        
                            if ((g_DotBuf[m * line_bytes + i / 8] & (0x80 >> (i % 8))) != 0x00)
                            {
                                LCD_PutPixel(x, y, _tFont->FrontColor); /* 设置像素颜色为文字色 */
                            }
                            else
                            {
                                if (_tFont->BackColor != CL_MASK) /* 透明色 */
                                {
                                    LCD_PutPixel(x, y, _tFont->BackColor); /* 设置像素颜色为文字背景色 */
                                }
                            }
                        }
                        x++;
                    }

                    for (i = 0; i < _tFont->Space; i++) /* 字符宽度 */
                    {
                        if (x < xMax)
                        {                        
                            if (_tFont->BackColor != CL_MASK) /* 透明色 */
                            {
                                /* 如果文字底色按_tFont->usBackColor，并且字间距大于点阵的宽度，那么需要在文字之间填充(暂时未实现) */
                                LCD_PutPixel(x + i, y, _tFont->BackColor); /* 设置像素颜色为文字背景色 */
                            }
                        }
                    }
                    y++;
                }
            }
            //            else
            //            {
            //                if (_tFont->BackColor == CL_MASK)    /* 透明色 */
            //                {
            //                    RA8875_SetTextTransp(1);
            //                }
            //                RA8875_SetFrontColor(_tFont->FrontColor);            /* 设置字体前景色 */
            //                RA8875_SetBackColor(_tFont->BackColor);                /* 设置字体背景色 */
            //                RA8875_SetFont(ra8875_font_code, 0, _tFont->Space);    /* 字体代码，行间距，字间距 */
            //                RA8875_DispStr(_usX, _usY, (char *)&code1);
            //                if (_tFont->BackColor == CL_MASK)    /* 透明色 */
            //                {
            //                    RA8875_SetTextTransp(0);
            //                }
            //            }
            _usX += width + _tFont->Space; /* 列地址递增 */
            _ptr++;                                                 /* 指向下一个字符 */
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_PutPixel
*    功能说明: 画1个像素
*    形    参:
*            _usX,_usY : 像素坐标
*            _usColor  : 像素颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
    LCDX_PutPixel(_usX, _usY, _usColor);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_GetPixel
*    功能说明: 读取1个像素
*    形    参:
*            _usX,_usY : 像素坐标
*            _usColor  : 像素颜色
*    返 回 值: RGB颜色值
*********************************************************************************************************
*/
uint16_t LCD_GetPixel(uint16_t _usX, uint16_t _usY)
{
    uint16_t usRGB;

    usRGB = LCDX_GetPixel(_usX, _usY);
    return usRGB;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawLine
*    功能说明: 采用 Bresenham 算法，在2点间画一条直线。
*    形    参:
*            _usX1, _usY1 : 起始点坐标
*            _usX2, _usY2 : 终止点Y坐标
*            _usColor     : 颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawLine(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor)
{
    LCDX_DrawLine(_usX1, _usY1, _usX2, _usY2, _usColor);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawPoints
*    功能说明: 采用 Bresenham 算法，绘制一组点，并将这些点连接起来。可用于波形显示。
*    形    参:
*            x, y     : 坐标数组
*            _usColor : 颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor)
{
    uint16_t i;

    for (i = 0; i < _usSize - 1; i++)
    {
        LCD_DrawLine(x[i], y[i], x[i + 1], y[i + 1], _usColor);
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawRect
*    功能说明: 绘制水平放置的矩形。
*    形    参:
*            _usX,_usY: 矩形左上角的坐标
*            _usHeight : 矩形的高度
*            _usWidth  : 矩形的宽度
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
    LCDX_DrawRect(_usX, _usY, _usHeight, _usWidth, _usColor);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_Fill_Rect
*    功能说明: 用一个颜色值填充一个矩形。【emWin 中有同名函数 LCD_FillRect，因此加了下划线区分】
*    形    参:
*            _usX,_usY: 矩形左上角的坐标
*            _usHeight : 矩形的高度
*            _usWidth  : 矩形的宽度
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
    LCDX_FillRect(_usX, _usY, _usHeight, _usWidth, _usColor);  
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawCircle
*    功能说明: 绘制一个圆，笔宽为1个像素
*    形    参:
*            _usX,_usY  : 圆心的坐标
*            _usRadius  : 圆的半径
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
    LCDX_DrawCircle(_usX, _usY, _usRadius, _usColor);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawBMP
*    功能说明: 在LCD上显示一个BMP位图，位图点阵扫描次序: 从左到右，从上到下
*    形    参:
*            _usX, _usY : 图片的坐标
*            _usHeight  : 图片高度
*            _usWidth   : 图片宽度
*            _ptr       : 图片点阵指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
    LCDX_DrawBMP(_usX, _usY, _usHeight, _usWidth, _ptr);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawWin
*    功能说明: 在LCD上绘制一个窗口
*    形    参: 结构体指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawWin(WIN_T *_pWin)
{
    uint16_t TitleHegiht;

    TitleHegiht = 20;

    /* 绘制窗口外框 */
    LCD_DrawRect(_pWin->Left, _pWin->Top, _pWin->Height, _pWin->Width, WIN_BORDER_COLOR);
    LCD_DrawRect(_pWin->Left + 1, _pWin->Top + 1, _pWin->Height - 2, _pWin->Width - 2, WIN_BORDER_COLOR);

    /* 窗口标题栏 */
    LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + 2, TitleHegiht, _pWin->Width - 4, WIN_TITLE_COLOR);

    /* 窗体填充 */
    LCD_Fill_Rect(_pWin->Left + 2, _pWin->Top + TitleHegiht + 2, _pWin->Height - 4 - TitleHegiht,
                                _pWin->Width - 4, WIN_BODY_COLOR);

    LCD_DispStr(_pWin->Left + 3, _pWin->Top + 2, _pWin->pCaption, _pWin->Font);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawIcon
*    功能说明: 在LCD上绘制一个图标，四角自动切为弧脚
*    形    参: _pIcon : 图标结构
*              _tFont : 字体属性
*              _ucFocusMode : 焦点模式。0 表示正常图标  1表示选中的图标
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
    const uint16_t *p;
    uint16_t usNewRGB;
    uint16_t x, y; /* 用于记录窗口内的相对坐标 */

    p = _tIcon->pBmp;
    for (y = 0; y < _tIcon->Height; y++)
    {
        for (x = 0; x < _tIcon->Width; x++)
        {
            usNewRGB = *p++; /* 读取图标的颜色值后指针加1 */
            /* 将图标的4个直角切割为弧角，弧角外是背景图标 */
            if ((y == 0 && (x < 6 || x > _tIcon->Width - 7)) ||
                    (y == 1 && (x < 4 || x > _tIcon->Width - 5)) ||
                    (y == 2 && (x < 3 || x > _tIcon->Width - 4)) ||
                    (y == 3 && (x < 2 || x > _tIcon->Width - 3)) ||
                    (y == 4 && (x < 1 || x > _tIcon->Width - 2)) ||
                    (y == 5 && (x < 1 || x > _tIcon->Width - 2)) ||

                    (y == _tIcon->Height - 1 && (x < 6 || x > _tIcon->Width - 7)) ||
                    (y == _tIcon->Height - 2 && (x < 4 || x > _tIcon->Width - 5)) ||
                    (y == _tIcon->Height - 3 && (x < 3 || x > _tIcon->Width - 4)) ||
                    (y == _tIcon->Height - 4 && (x < 2 || x > _tIcon->Width - 3)) ||
                    (y == _tIcon->Height - 5 && (x < 1 || x > _tIcon->Width - 2)) ||
                    (y == _tIcon->Height - 6 && (x < 1 || x > _tIcon->Width - 2)))
            {
                ;
            }
            else
            {
                if (_ucFocusMode != 0) /* 1表示选中的图标 */
                {
                    /* 降低原始像素的亮度，实现图标被激活选中的效果 */
                    uint16_t R, G, B;
                    uint16_t bright = 15;

                    /* rrrr rggg gggb bbbb */
                    R = (usNewRGB & 0xF800) >> 11;
                    G = (usNewRGB & 0x07E0) >> 5;
                    B = usNewRGB & 0x001F;
                    if (R > bright)
                    {
                        R -= bright;
                    }
                    else
                    {
                        R = 0;
                    }
                    if (G > 2 * bright)
                    {
                        G -= 2 * bright;
                    }
                    else
                    {
                        G = 0;
                    }
                    if (B > bright)
                    {
                        B -= bright;
                    }
                    else
                    {
                        B = 0;
                    }
                    usNewRGB = (R << 11) + (G << 5) + B;
                }

                LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
            }
        }
    }

    /* 绘制图标下的文字 */
    {
        uint16_t len;
        uint16_t width;

        len = strlen(_tIcon->Text);

        if (len == 0)
        {
            return; /* 如果图标文本长度为0，则不显示 */
        }

        /* 计算文本的总宽度 */
        if (_tFont->FontCode == FC_ST_12) /* 12点阵 */
        {
            width = 6 * (len + _tFont->Space);
        }
        else /* FC_ST_16 */
        {
            width = 8 * (len + _tFont->Space);
        }

        /* 水平居中 */
        x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
        y = _tIcon->Top + _tIcon->Height + 2;
        LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_Blend565
*    功能说明: 对像素透明化 颜色混合
*    形    参: src : 原始像素
*              dst : 混合的颜色
*              alpha : 透明度 0-32
*    返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD_Blend565(uint16_t src, uint16_t dst, uint8_t alpha)
{
    uint32_t src2;
    uint32_t dst2;

    src2 = ((src << 16) | src) & 0x07E0F81F;
    dst2 = ((dst << 16) | dst) & 0x07E0F81F;
    dst2 = ((((dst2 - src2) * alpha) >> 5) + src2) & 0x07E0F81F;
    return (dst2 >> 16) | dst2;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawIcon32
*    功能说明: 在LCD上绘制一个图标, 带有透明信息的位图(32位， RGBA). 图标下带文字
*    形    参: _pIcon : 图标结构
*              _tFont : 字体属性
*              _ucFocusMode : 焦点模式。0 表示正常图标  1表示选中的图标
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawIcon32(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode)
{
    const uint8_t *p;
    uint16_t usOldRGB, usNewRGB;
    int16_t x, y;                     /* 用于记录窗口内的相对坐标 */
    uint8_t R1, G1, B1, A; /* 新像素色彩分量 */
    uint8_t R0, G0, B0;         /* 旧像素色彩分量 */

    p = (const uint8_t *)_tIcon->pBmp;
    p += 54; /* 直接指向图像数据区 */

    /* 按照BMP位图次序，从左至右，从上至下扫描 */
    for (y = _tIcon->Height - 1; y >= 0; y--)
    {
        for (x = 0; x < _tIcon->Width; x++)
        {
            B1 = *p++;
            G1 = *p++;
            R1 = *p++;
            A = *p++; /* Alpha 值(透明度)，0-255, 0表示透明，1表示不透明, 中间值表示透明度 */

            if (A == 0x00) /* 需要透明,显示背景 */
            {
                ; /* 不用刷新背景 */
            }
            else if (A == 0xFF) /* 完全不透明， 显示新像素 */
            {
                usNewRGB = RGB(R1, G1, B1);
                if (_ucFocusMode == 1)
                {
                    usNewRGB = LCD_Blend565(usNewRGB, CL_YELLOW, 10);
                }
                LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
            }
            else /* 半透明 */
            {
                /* 计算公式： 实际显示颜色 = 前景颜色 * Alpha / 255 + 背景颜色 * (255-Alpha) / 255 */
                usOldRGB = LCD_GetPixel(x + _tIcon->Left, y + _tIcon->Top);

                //usOldRGB = 0xFFFF;
                R0 = RGB565_R(usOldRGB);
                G0 = RGB565_G(usOldRGB);
                B0 = RGB565_B(usOldRGB);

                R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
                G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
                B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
                usNewRGB = RGB(R1, G1, B1);
                if (_ucFocusMode == 1)
                {
                    usNewRGB = LCD_Blend565(usNewRGB, CL_YELLOW, 10);
                }
                LCD_PutPixel(x + _tIcon->Left, y + _tIcon->Top, usNewRGB);
            }
        }
    }

    /* 绘制图标下的文字 */
    {
        uint16_t len;
        uint16_t width;

        len = strlen(_tIcon->Text);

        if (len == 0)
        {
            return; /* 如果图标文本长度为0，则不显示 */
        }

        /* 计算文本的总宽度 */
        if (_tFont->FontCode == FC_ST_12) /* 12点阵 */
        {
            width = 6 * (len + _tFont->Space);
        }
        else /* FC_ST_16 */
        {
            width = 8 * (len + _tFont->Space);
        }

        /* 水平居中 */
        x = (_tIcon->Left + _tIcon->Width / 2) - width / 2;
        y = _tIcon->Top + _tIcon->Height + 2;
        LCD_DispStr(x, y, (char *)_tIcon->Text, _tFont);
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawBmp32
*    功能说明: 在LCD上绘制一个32位的BMP图, 带有透明信息的位图(32位， RGBA)
*    形    参: _usX, _usY : 显示坐标
*              _usHeight, _usWidth : 图片高度和宽度
*              _pBmp : 图片数据（带BMP文件头）
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawBmp32(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t *_pBmp)
{
    const uint8_t *p;
    uint16_t usOldRGB, usNewRGB;
    int16_t x, y;                     /* 用于记录窗口内的相对坐标 */
    uint8_t R1, G1, B1, A; /* 新像素色彩分量 */
    uint8_t R0, G0, B0;         /* 旧像素色彩分量 */

    p = (const uint8_t *)_pBmp;
    p += 54; /* 直接指向图像数据区 */

    /* 按照BMP位图次序，从左至右，从上至下扫描 */
    for (y = _usHeight - 1; y >= 0; y--)
    {
        for (x = 0; x < _usWidth; x++)
        {
            B1 = *p++;
            G1 = *p++;
            R1 = *p++;
            A = *p++; /* Alpha 值(透明度)，0-255, 0表示透明，1表示不透明, 中间值表示透明度 */

            if (A == 0x00) /* 需要透明,显示背景 */
            {
                ; /* 不用刷新背景 */
            }
            else if (A == 0xFF) /* 完全不透明， 显示新像素 */
            {
                usNewRGB = RGB(R1, G1, B1);
                //if (_ucFocusMode == 1)
                //{
                //    usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
                //}
                LCD_PutPixel(x + _usX, y + _usY, usNewRGB);
            }
            else /* 半透明 */
            {
                /* 计算公式： 实际显示颜色 = 前景颜色 * Alpha / 255 + 背景颜色 * (255-Alpha) / 255 */
                usOldRGB = LCD_GetPixel(x + _usX, y + _usY);
                R0 = RGB565_R(usOldRGB);
                G0 = RGB565_G(usOldRGB);
                B0 = RGB565_B(usOldRGB);

                R1 = (R1 * A) / 255 + R0 * (255 - A) / 255;
                G1 = (G1 * A) / 255 + G0 * (255 - A) / 255;
                B1 = (B1 * A) / 255 + B0 * (255 - A) / 255;
                usNewRGB = RGB(R1, G1, B1);
                //if (_ucFocusMode == 1)
                //{
                //    usNewRGB = Blend565(usNewRGB, CL_YELLOW, 10);
                //}
                LCD_PutPixel(x + _usX, y + _usY, usNewRGB);
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawLabel
*    功能说明: 绘制一个文本标签
*    形    参: _pLabel : Label结构体指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitLabel(LABEL_T *_pLabel, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w,
                                     char *_Text, FONT_T *_tFont)
{
    _pLabel->Left = _x;
    _pLabel->Top = _y;
    _pLabel->Height = _h;
    _pLabel->Width = _w;
    _pLabel->pCaption = _Text;
    _pLabel->Font = _tFont;

    _pLabel->MaxLen = 0;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawLabel
*    功能说明: 绘制一个文本标签
*    形    参: 结构体指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawLabel(LABEL_T *_pLabel)
{
    char dispbuf[256];
    uint16_t i;
    uint16_t NewLen;

    NewLen = strlen(_pLabel->pCaption);

    if (NewLen > _pLabel->MaxLen)
    {
        LCD_DispStr(_pLabel->Left, _pLabel->Top, _pLabel->pCaption, _pLabel->Font);
        _pLabel->MaxLen = NewLen;
    }
    else
    {
        for (i = 0; i < NewLen; i++)
        {
            dispbuf[i] = _pLabel->pCaption[i];
        }
        for (; i < _pLabel->MaxLen; i++)
        {
            dispbuf[i] = ' '; /* 末尾填充空格 */
        }
        dispbuf[i] = 0;
        LCD_DispStr(_pLabel->Left, _pLabel->Top, dispbuf, _pLabel->Font);
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawCheckBox
*    功能说明: 绘制一个检查框
*    形    参: 结构体指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawCheckBox(CHECK_T *_pCheckBox)
{
    uint16_t x, y;

    /* 目前只做了16点阵汉字的大小 */

    /* 绘制外框 */
    x = _pCheckBox->Left;
    LCD_DrawRect(x, _pCheckBox->Top, CHECK_BOX_H, CHECK_BOX_W, CHECK_BOX_BORDER_COLOR);
    LCD_DrawRect(x + 1, _pCheckBox->Top + 1, CHECK_BOX_H - 2, CHECK_BOX_W - 2, CHECK_BOX_BORDER_COLOR);
    LCD_Fill_Rect(x + 2, _pCheckBox->Top + 2, CHECK_BOX_H - 4, CHECK_BOX_W - 4, CHECK_BOX_BACK_COLOR);

    /* 绘制文本标签 */
    x = _pCheckBox->Left + CHECK_BOX_W + 2;
    y = _pCheckBox->Top + CHECK_BOX_H / 2 - 8;
    LCD_DispStr(x, y, _pCheckBox->pCaption, _pCheckBox->Font);

    if (_pCheckBox->Checked)
    {
        FONT_T font;

        font.FontCode = FC_ST_16;
        font.BackColor = CL_MASK;
        font.FrontColor = CHECK_BOX_CHECKED_COLOR; /* 钩的颜色 */
        font.Space = 0;
        x = _pCheckBox->Left;
        LCD_DispStr(x + 3, _pCheckBox->Top + 3, "√", &font);
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawEdit
*    功能说明: 在LCD上绘制一个编辑框
*    形    参: _pEdit 编辑框结构体指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawEdit(EDIT_T *_pEdit)
{
    uint16_t len, x, y;

    /* 仿XP风格，平面编辑框 */
    if (_pEdit->Focus == 0)
    {
        LCD_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR);
        LCD_Fill_Rect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2, EDIT_BACK_COLOR);
    }
    else
    {
        LCD_DrawRect(_pEdit->Left, _pEdit->Top, _pEdit->Height, _pEdit->Width, EDIT_BORDER_COLOR2);
        LCD_Fill_Rect(_pEdit->Left + 1, _pEdit->Top + 1, _pEdit->Height - 2, _pEdit->Width - 2, EDIT_BACK_COLOR2);
    }

    if (_pEdit->pCaption > 0)
    {
        for (len = 0; len < 32; len++)
        {
            _pEdit->Text[len] = _pEdit->pCaption[len];

            if (_pEdit->pCaption[len] == 0)
            {
                break;
            }
        }
        _pEdit->Text[32] = 0;

        //_pEdit->pCaption = 0;
    }

    /* 文字居中 */
    len = LCD_GetStrWidth(_pEdit->Text, _pEdit->Font);
    x = _pEdit->Left + (_pEdit->Width - len) / 2;
    y = _pEdit->Top + (_pEdit->Height - LCD_GetFontHeight(_pEdit->Font)) / 2;

    LCD_DispStr(x, y, _pEdit->Text, _pEdit->Font);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_EditTouchDown
*    功能说明: 判断按钮是否被按下. 检查触摸坐标是否在按钮的范围之内。并重绘按钮。
*    形    参:  _edit : 编辑框对象
*              _usX, _usY: 触摸坐标
*    返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
uint8_t LCD_EditTouchDown(EDIT_T *_Edit, uint16_t _usX, uint16_t _usY)
{
    if ((_usX > _Edit->Left) && (_usX < _Edit->Left + _Edit->Width) && (_usY > _Edit->Top) && (_usY < _Edit->Top + _Edit->Height))
    {
        BUTTON_BEEP(); /* 按键提示音 bsp_tft_lcd.h 文件开头可以使能和关闭 */
        _Edit->Focus = 1;
        LCD_DrawEdit(_Edit);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_EditTouchRelease
*    功能说明: 编辑框退出编辑状态，重绘
*    形    参:  _Edit : 编辑框对象
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_EditRefresh(EDIT_T *_Edit)
{
    _Edit->Focus = 0;
    LCD_DrawEdit(_Edit);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_InitGroupBox
*    功能说明: 初始化分组框参数
*    形    参: _pBox 分组框
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitGroupBox(GROUP_T *_pBox, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w,
                                            char *pCaption, FONT_T *Font)
{
    _pBox->Left = _x;
    _pBox->Top = _y;
    _pBox->Height = _h;
    _pBox->Width = _w;
    _pBox->pCaption = pCaption;
    _pBox->Font = Font;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawGroupBox
*    功能说明: 在LCD上绘制一个分组框
*    形    参: _pBox 分组框
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawGroupBox(GROUP_T *_pBox)
{
    uint16_t x, y;
    uint16_t x1, y1; /* 矩形左上角 */
    uint16_t x2, y2; /* 矩形右下角 */
    uint16_t len;

    len = LCD_GetStrWidth(_pBox->pCaption, _pBox->Font); /* 字符串的总宽度 */

    /* 画阴影线 */
    //LCD_DrawRect(_pBox->Left + 1, _pBox->Top + 5, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER2);
    x1 = _pBox->Left + 1;
    y1 = _pBox->Top + 5;
    x2 = _pBox->Left + 1 + _pBox->Width - 2;
    y2 = _pBox->Top + 5 + _pBox->Height - 1;

    LCD_DrawLine(x1, y1, x1 + 6, y1, CL_BOX_BORDER2);                        /* 顶1 */
    LCD_DrawLine(x1 + 8 + len + 1, y1, x2, y1, CL_BOX_BORDER2); /* 顶2 */
    LCD_DrawLine(x1, y2, x2, y2, CL_BOX_BORDER2);                                /* 底 */
    LCD_DrawLine(x1, y1, x1, y2, CL_BOX_BORDER2);                                /* 左 */
    LCD_DrawLine(x2, y1, x2, y2, CL_BOX_BORDER2);                                /* 右 */

    /* 画主框线 */
    //LCD_DrawRect(_pBox->Left, _pBox->Top + 4, _pBox->Height, _pBox->Width - 1, CL_BOX_BORDER1);
    x1 = _pBox->Left;
    y1 = _pBox->Top + 4;
    x2 = _pBox->Left + _pBox->Width - 2;
    y2 = _pBox->Top + 4 + _pBox->Height - 1;

    LCD_DrawLine(x1, y1, x1 + 6, y1, CL_BOX_BORDER1);                        /* 顶1 */
    LCD_DrawLine(x1 + 9 + len + 1, y1, x2, y1, CL_BOX_BORDER1); /* 顶2 */
    LCD_DrawLine(x1, y2, x2, y2, CL_BOX_BORDER1);                                /* 底 */
    LCD_DrawLine(x1, y1, x1, y2, CL_BOX_BORDER1);                                /* 左 */
    LCD_DrawLine(x2, y1, x2, y2, CL_BOX_BORDER1);                                /* 右 */

    /* 显示分组框标题（文字在左上角） */
    x = _pBox->Left + 9;
    y = _pBox->Top;
    LCD_DispStr(x, y, _pBox->pCaption, _pBox->Font);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DispControl
*    功能说明: 绘制控件
*    形    参: _pControl 控件指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispControl(void *_pControl)
{
    uint8_t id;

    id = *(uint8_t *)_pControl; /* 读取ID */

    switch (id)
    {
        case ID_ICON:
            //void LCD_DrawIcon(const ICON_T *_tIcon, FONT_T *_tFont, uint8_t _ucFocusMode);
            break;

        case ID_WIN:
            LCD_DrawWin((WIN_T *)_pControl);
            break;

        case ID_LABEL:
            LCD_DrawLabel((LABEL_T *)_pControl);
            break;

        case ID_BUTTON:
            LCD_DrawButton((BUTTON_T *)_pControl);
            break;

        case ID_CHECK:
            LCD_DrawCheckBox((CHECK_T *)_pControl);
            break;

        case ID_EDIT:
            LCD_DrawEdit((EDIT_T *)_pControl);
            break;

        case ID_GROUP:
            LCD_DrawGroupBox((GROUP_T *)_pControl);
            break;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_InitButton
*    功能说明: 初始化按钮结构体成员。
*    形    参:  _x, _y : 坐标
*              _h, _w : 高度和宽度
*              _pCaption : 按钮文字
*              _pFont : 按钮字体
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitButton(BUTTON_T *_btn, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w, char *_pCaption, FONT_T *_pFont)
{
    _btn->Left = _x;
    _btn->Top = _y;
    _btn->Height = _h;
    _btn->Width = _w;
    _btn->pCaption = _pCaption;
    _btn->Font = _pFont;
    _btn->Focus = 0;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawButton
*    功能说明: 在LCD上绘制一个按钮，类似emwin按钮
*    形    参:
*            _usX, _usY : 图片的坐标
*            _usHeight  : 图片高度
*            _usWidth   : 图片宽度
*            _ptr       : 图片点阵指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawButton(BUTTON_T *_pBtn)
{
    uint16_t x, y;
    uint8_t muti_line = 0;

    {
        uint16_t i;

        for (i = 0; i < 1024; i++)
        {
            if (_pBtn->pCaption[i] == '\r' || _pBtn->pCaption[i] == '\t')
            {
                muti_line = 1;
                break;
            }
            if (_pBtn->pCaption[i] == 0)
            {
                break;
            }
        }
    }
    x = _pBtn->Left;
    if (muti_line == 0)
    {
        y = _pBtn->Top + (_pBtn->Height - LCD_GetFontHeight(_pBtn->Font)) / 2; /* 单行文本垂直居中 */
    }
    else
    {
        y = _pBtn->Top; /* 多行文本,垂直坐标从顶部开始 */
    }

    //    if (g_ChipID == IC_8875)
    //    {
    //        uint8_t Arc = 5;
    //
    //        if (_pBtn->Focus == 0)
    //        {
    //            RA8875_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR1);
    //            RA8875_FillRoundRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, Arc,  BTN_BODY_COLOR1);
    //            LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR1);    /* 画阴影对比色 */
    //        }
    //        else
    //        {
    //            RA8875_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR2);
    //            RA8875_FillRoundRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, Arc, BTN_BODY_COLOR2);
    //            LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR2);    /* 画阴影对比色 */
    //        }

    //        RA8875_SetTextTransp(1);
    //        LCD_DispStrEx(x, y, _pBtn->pCaption, _pBtn->Font, _pBtn->Width, ALIGN_CENTER);
    //        RA8875_SetTextTransp(0);
    //    }
    //    else if (g_ChipID == IC_8876)
    //    {
    //        uint8_t Arc = 5;
    //
    //        if (_pBtn->Focus == 0)
    //        {
    //            RA8876_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR1);
    //            RA8876_FillRoundRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, Arc,  BTN_BODY_COLOR1);
    //            LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR1);    /* 画阴影对比色 */
    //        }
    //        else
    //        {
    //            RA8876_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc,  BTN_BORDER_COLOR2);
    //            RA8876_FillRoundRect(_pBtn->Left + 1, _pBtn->Top + 1, _pBtn->Height - 2, _pBtn->Width - 2, Arc, BTN_BODY_COLOR2);
    //            LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR2);    /* 画阴影对比色 */
    //        }
    //
    //        RA8876_SetTextTransp(1);
    //        LCD_DispStrEx(x, y, _pBtn->pCaption, _pBtn->Font, _pBtn->Width, ALIGN_CENTER);
    //        RA8876_SetTextTransp(0);
    //    }
    //    else
    {
        uint8_t Arc = 5;

        if (_pBtn->Focus == 0)
        {
            LCD_FillRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc, BTN_BODY_COLOR1);
            LCD_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc, BTN_BORDER_COLOR1);
            LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR1); /* 画阴影对比色 */
        }
        else
        {
            LCD_FillRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc, BTN_BODY_COLOR2);
            LCD_DrawRoundRect(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, Arc, BTN_BORDER_COLOR2);
            LCD_Fill_Rect(_pBtn->Left + Arc, _pBtn->Top + 1, _pBtn->Height / 2, _pBtn->Width - 2 * Arc, BTN_SHADOW_COLOR2); /* 画阴影对比色 */
        }

        LCD_DispStrEx(x, y, _pBtn->pCaption, _pBtn->Font, _pBtn->Width, ALIGN_CENTER);
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_ButtonTouchDown
*    功能说明: 判断按钮是否被按下. 检查触摸坐标是否在按钮的范围之内。并重绘按钮。
*    形    参:  _btn : 按钮对象
*              _usX, _usY: 触摸坐标
*    返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
uint8_t LCD_ButtonTouchDown(BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
    if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width) && (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
    {
        BUTTON_BEEP(); /* 按键提示音 bsp_tft_lcd.h 文件开头可以使能和关闭 */
        _btn->Focus = 1;
        LCD_DrawButton(_btn);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_ButtonTouchRelease
*    功能说明: 判断按钮是否被触摸释放. 并重绘按钮。在触摸释放事件中被调用。
*    形    参:  _btn : 按钮对象
*              _usX, _usY: 触摸坐标
*    返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
uint8_t LCD_ButtonTouchRelease(BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
    /* 2016-04-24 避免闪屏 */
    if (_btn->Focus != 0)
    {
        _btn->Focus = 0;
        LCD_DrawButton(_btn);
    }

    if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width) && (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
    {

        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawBmpButton
*    功能说明: 在LCD上绘制一个图片按钮
*    形    参:
*            _usX, _usY : 图片的坐标
*            _usHeight  : 图片高度
*            _usWidth   : 图片宽度
*            _ptr       : 图片点阵指针
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawBmpButton(BMP_BUTTON_T *_pBtn)
{
    //    if (_pBtn->Focus == 1)
    //    {
    //        RA8875_DispBmpInFlash(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, _pBtn->Pic2);
    //    }
    //    else
    //    {
    //        RA8875_DispBmpInFlash(_pBtn->Left, _pBtn->Top, _pBtn->Height, _pBtn->Width, _pBtn->Pic1);
    //    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_BmpButtonTouchDown
*    功能说明: 判断图片按钮按钮是否被按下. 检查触摸坐标是否在按钮的范围之内。并重绘按钮。
*    形    参:  _btn : 按钮对象
*              _usX, _usY: 触摸坐标
*    返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
uint8_t LCD_BmpButtonTouchDown(BMP_BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
    if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width) && (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
    {
        BUTTON_BEEP(); /* 按键提示音 bsp_tft_lcd.h 文件开头可以使能和关闭 */
        _btn->Focus = 1;
        LCD_DrawBmpButton(_btn);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_BmpButtonTouchRelease
*    功能说明: 判断图片按钮是否被触摸释放. 并重绘按钮。在触摸释放事件中被调用。
*    形    参:  _btn : 按钮对象
*              _usX, _usY: 触摸坐标
*    返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
uint8_t LCD_BmpButtonTouchRelease(BMP_BUTTON_T *_btn, uint16_t _usX, uint16_t _usY)
{
    _btn->Focus = 0;
    LCD_DrawBmpButton(_btn);

    if ((_usX > _btn->Left) && (_usX < _btn->Left + _btn->Width) && (_usY > _btn->Top) && (_usY < _btn->Top + _btn->Height))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_SelectTouchDown
*    功能说明: 选中要校准的参数
*    形    参:  _slt : 选中对象
*              _usX, _usY: 触摸坐标
*    返 回 值: 1 表示在范围内
*********************************************************************************************************
*/
uint8_t LCD_SelectTouchDown(SELECT_T *_slt, uint16_t _usX, uint16_t _usY)
{
    if ((_usX > _slt->Left) && (_usX < _slt->Left + _slt->Width) && (_usY > _slt->Top) && (_usY < _slt->Top + _slt->Height))
    {
        BUTTON_BEEP();
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_InitPannel
*    功能说明: 初始化面板结构体成员。
*    形    参: _panl : pannel 对象
*              _x, _y : 坐标
*              _h, _w : 高度和宽度
*              _pCaption : 按钮文字
*              _pFont : 按钮字体
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitPannel(PANNEL_T *_pnl, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w, uint16_t _arc, uint16_t _color)
{
    _pnl->Left = _x;
    _pnl->Top = _y;
    _pnl->Height = _h;
    _pnl->Width = _w;
    _pnl->Arc = _arc;
    _pnl->Color = _color;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawPannel
*    功能说明: 在LCD上绘制一个面板
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawPannel(PANNEL_T *_pnl)
{
    //    if (g_ChipID == IC_8875)
    //    {
    //        /* 绘制一个圆角矩形，填充底色 */
    //        RA8875_FillRoundRect(_pnl->Left, _pnl->Top, _pnl->Height, _pnl->Width, _pnl->Arc, _pnl->Color);
    //    }
    //    else
    //    {
    //        ;
    //    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_PannelClick
*    功能说明: 判断Pannel是否被点击. 检查触摸坐标是否在按钮的范围之内。
*    形    参:  _obj : PANNEL对象
*              _usX, _usY: 触摸坐标
*    返 回 值: 1 表示在范围内 0表示不在
*********************************************************************************************************
*/
uint8_t LCD_PannelClick(PANNEL_T *_obj, uint16_t _usX, uint16_t _usY)
{
    if ((_usX > _obj->Left) && (_usX < _obj->Left + _obj->Width) && (_usY > _obj->Top) && (_usY < _obj->Top + _obj->Height))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_LabelClick
*    功能说明: 判断Label是否被点击. 检查触摸坐标是否在按钮的范围之内。
*    形    参:  _obj : PANNEL对象
*              _usX, _usY: 触摸坐标
*    返 回 值: 1 表示在范围内 0表示不在
*********************************************************************************************************
*/
uint8_t LCD_LabelClick(LABEL_T *_obj, uint16_t _usX, uint16_t _usY)
{
    if ((_usX > _obj->Left) && (_usX < _obj->Left + _obj->Width) && (_usY > _obj->Top) && (_usY < _obj->Top + _obj->Height))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawArc
*    功能说明: 绘制一个圆弧，笔宽为1个像素
*    形    参:
*            _usX,_usY  ：圆心的坐标
*            _usRadius  ：圆的半径
*            _StartAng  : 起始角度
*            _EndAng       : 终止角度
*            _usColor   : 圆弧颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawArc(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, float _StartRnd, float _EndRnd, uint16_t _usColor)
{
    float CurX, CurY, rnd;

    rnd = _StartRnd;
    while (rnd <= _EndRnd)
    {
        CurX = _usRadius * cos(rnd);
        CurY = _usRadius * sin(rnd);
        LCD_PutPixel(_usX + CurX, _usY - CurY, _usColor);
        rnd = rnd + 0.01f;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawQuterCircle
*    功能说明: 绘制一个1/4圆，笔宽为1个像素, 使用软件算法绘制
*    形    参:
*            _usX,_usY  : 圆心的坐标
*            _usRadius  : 圆的半径
*            _ucMode    : 0 表示左上角1/4圆 1表示右上角  2表示右下角 3表示左下角
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode)
{
    int32_t D;         /* Decision Variable */
    uint32_t CurX; /* 当前 X 值 */
    uint32_t CurY; /* 当前 Y 值 */

    D = 3 - (_usRadius << 1);

    CurX = 0;
    CurY = _usRadius;

    while (CurX <= CurY)
    {
        if (_ucMode == 0)
        {
            LCD_PutPixel(_usX - CurY, _usY - CurX, _usColor); // 左 -> 上
            LCD_PutPixel(_usX - CurX, _usY - CurY, _usColor); // 上 -> 左
        }
        else if (_ucMode == 1)
        {
            LCD_PutPixel(_usX + CurX, _usY - CurY, _usColor); // 上 -> 右
            LCD_PutPixel(_usX + CurY, _usY - CurX, _usColor); // 右 -> 上
        }
        else if (_ucMode == 2)
        {
            LCD_PutPixel(_usX + CurX, _usY + CurY, _usColor); // 下 -> 右
            LCD_PutPixel(_usX + CurY, _usY + CurX, _usColor); // 右 -> 下
        }
        else if (_ucMode == 3)
        {
            LCD_PutPixel(_usX - CurX, _usY + CurY, _usColor); // 下 -> 左
            LCD_PutPixel(_usX - CurY, _usY + CurX, _usColor); // 左 -> 下
        }

        if (D < 0)
        {
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_FillQuterCircle
*    功能说明: 填充一个1/4圆，软件算法实现。
*    形    参:
*            _usX,_usY  : 圆心的坐标
*            _usRadius  : 圆的半径
*            _usColor   : 填充的颜色
*            _ucMode    : 0 表示左上角1/4圆 1表示右上角  2表示左下角 3表示右下角
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_FillQuterCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor, uint8_t _ucMode)
{
    int32_t D;
    uint32_t CurX; /* 当前 X 值 */
    uint32_t CurY; /* 当前 Y 值 */

    D = 3 - (_usRadius << 1);
    CurX = 0;
    CurY = _usRadius;

    while (CurX <= CurY)
    {
        if (_ucMode == 0)
        {
            LCD_DrawLine(_usX - CurY, _usY - CurX, _usX, _usY - CurX, _usColor); // 左 -> 上
            LCD_DrawLine(_usX - CurX, _usY - CurY, _usX, _usY - CurY, _usColor); // 上 -> 左
        }
        else if (_ucMode == 1)
        {
            LCD_DrawLine(_usX + CurX, _usY - CurY, _usX, _usY - CurY, _usColor); // 上 -> 右
            LCD_DrawLine(_usX + CurY, _usY - CurX, _usX, _usY - CurX, _usColor); // 右 -> 上
        }
        else if (_ucMode == 2)
        {
            LCD_DrawLine(_usX + CurX, _usY + CurY, _usX, _usY + CurY, _usColor); // 下 -> 右
            LCD_DrawLine(_usX + CurY, _usY + CurX, _usX, _usY + CurX, _usColor); // 右 -> 下
        }
        else if (_ucMode == 3)
        {
            LCD_DrawLine(_usX - CurX, _usY + CurY, _usX, _usY + CurY, _usColor); // 下 -> 左
            LCD_DrawLine(_usX - CurY, _usY + CurX, _usX, _usY + CurX, _usColor); // 左 -> 下
        }

        if (D < 0)
        {
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawRoundRect
*    功能说明: 绘制圆角矩形轮廓，笔宽度1像素
*    形    参:
*            _usX,_usY:矩形左上角的坐标
*            _usHeight :矩形的高度
*            _usWidth  :矩形的宽度
*            _usArc    :圆角的弧半径
*            _usColor  :颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_DrawRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth,
                                             uint16_t _usRadius, uint16_t _usColor)
{
    if (_usHeight < 2 * _usRadius)
    {
        _usHeight = 2 * _usRadius;
    }

    if (_usWidth < 2 * _usRadius)
    {
        _usWidth = 2 * _usRadius;
    }

    LCD_DrawQuterCircle(_usX + _usRadius, _usY + _usRadius, _usRadius, _usColor, 0); /* 左上角的弧 */
    LCD_DrawLine(_usX + _usRadius, _usY, _usX + _usWidth - _usRadius - 1, _usY, _usColor);

    LCD_DrawQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usRadius, _usRadius, _usColor, 1); /* 右上角的弧 */
    LCD_DrawLine(_usX + _usWidth - 1, _usY + _usRadius, _usX + _usWidth - 1, _usY + _usHeight - _usRadius - 1, _usColor);

    LCD_DrawQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 2); /* 右下角的弧 */
    LCD_DrawLine(_usX + _usRadius, _usY + _usHeight - 1, _usX + _usWidth - _usRadius - 1, _usY + _usHeight - 1, _usColor);

    LCD_DrawQuterCircle(_usX + _usRadius, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 3); /* 左下角的弧 */
    LCD_DrawLine(_usX, _usY + _usRadius, _usX, _usY + _usHeight - _usRadius - 1, _usColor);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_FillRoundRect
*    功能说明: 填充圆角矩形
*    形    参:
*            _usX,_usY:矩形左上角的坐标
*            _usHeight :矩形的高度
*            _usWidth  :矩形的宽度
*            _usArc    :圆角的弧半径
*            _usColor  :颜色
*    返 回 值: 无
*********************************************************************************************************
*/
extern uint8_t s_DispRefresh;
void LCD_FillRoundRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth,
                                             uint16_t _usRadius, uint16_t _usColor)
{        
    if (_usHeight < 2 * _usRadius)
    {
        _usHeight = 2 * _usRadius;
    }

    if (_usWidth < 2 * _usRadius)
    {
        _usWidth = 2 * _usRadius;
    }
        
    LCD_FillQuterCircle(_usX + _usRadius, _usY + _usRadius, _usRadius, _usColor, 0); /* 左上角的弧 */
    
    LCD_Fill_Rect(_usX + _usRadius, _usY, _usRadius + 1, _usWidth - 2 * _usRadius, _usColor);
    
    LCD_FillQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usRadius, _usRadius, _usColor, 1); /* 右上角的弧 */
    
    LCD_Fill_Rect(_usX, _usY + _usRadius, _usHeight - 2 * _usRadius, _usWidth, _usColor);

    LCD_FillQuterCircle(_usX + _usWidth - _usRadius - 1, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 2); /* 右下角的弧 */
    
    LCD_Fill_Rect(_usX + _usRadius, _usY + _usHeight - _usRadius - 1, _usRadius + 1, _usWidth - 2 * _usRadius, _usColor);

    LCD_FillQuterCircle(_usX + _usRadius, _usY + _usHeight - _usRadius - 1, _usRadius, _usColor, 3); /* 左下角的弧 */
}

/*
*********************************************************************************************************
*    函 数 名: __MemoAddStr
*    功能说明: 添加1行文本0结束. 0x0D 0A结束
*    形    参:  _pMemo : 文本框对象
*               _str : 字符串
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_MemoAddStr(MEMO_T *_pMemo, char *_str)
{
    uint32_t i = 0;
    uint32_t InputLen;

    InputLen = strlen(_str);
            
    /* 追加新字符串填入 */
    for (i = 0; i < InputLen; i++)
    {
        _pMemo->Text[_pMemo->FifoWrite] = _str[i];
        if (++_pMemo->FifoWrite >= _pMemo->MaxLen)
        {
            _pMemo->FifoWrite = 0;            
        }

        if (_pMemo->Len < _pMemo->MaxLen)
        {
            _pMemo->Len++;        
        }
    }       

    if (_pMemo->Len < _pMemo->MaxLen)
    {
        _pMemo->FifoRead = 0;     
    }
    else
    {
        _pMemo->FifoRead = _pMemo->FifoWrite + 1024;    /* 预留1K缓冲带 */
        
        if (_pMemo->FifoRead >= _pMemo->MaxLen)
        {
            _pMemo->FifoRead = 0;
        }
    }
        
    _pMemo->Refresh = 1;    /* 内容变化，需要刷新显示 */    
}

/*
*********************************************************************************************************
*    函 数 名: LCD_MemoAddChar
*    功能说明: 添加1个字符，补0
*    形    参:  _pMemo : 文本框对象
*               _ch : 字符
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_MemoAddChar(MEMO_T *_pMemo, char _ch)
{
    _pMemo->Text[_pMemo->FifoWrite] = _ch;
    if (++_pMemo->FifoWrite >= _pMemo->MaxLen)
    {
        _pMemo->FifoWrite = 0;
        
        _pMemo->FifoRead = 1;
    }
    
    _pMemo->Text[_pMemo->FifoWrite] = 0;  
    
    if (_pMemo->Len < _pMemo->MaxLen)
    {
        _pMemo->FifoRead = 0;
        _pMemo->Len++;        
    }
    else
    {
        _pMemo->FifoRead = _pMemo->FifoWrite + 1024;    /* 预留1K缓冲带 */
        
        if (_pMemo->FifoRead >= _pMemo->MaxLen)
        {
            _pMemo->FifoRead = 0;
        }
    }
  
    _pMemo->Refresh = 1;    /* 内容变化，需要刷新显示 */
}

/*
*********************************************************************************************************
*    函 数 名: LCD_MemoPageDown
*    功能说明: 向后翻行
*    形    参:  _pMemo : 文本框对象
*               _LineNum : 函数
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_MemoPageDown(MEMO_T *_pMemo, uint16_t  _LineNum)
{    
    if (_pMemo->LineOffset < _pMemo->LineCount)
    {
        _pMemo->LineOffset += _LineNum;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_MemoPageUp
*    功能说明: 向前翻行
*    形    参:  _pMemo : 文本框对象
*               _LineNum : 函数
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_MemoPageUp(MEMO_T *_pMemo, uint16_t  _LineNum)
{
    if (_pMemo->LineOffset > -_pMemo->LineCount)
    {
        _pMemo->LineOffset -= _LineNum;
    }
}

/*
*********************************************************************************************************
*    函 数 名: LCD_MemoClear
*    功能说明: 清除文本
*    形    参:  _pMemo : 文本框对象
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_MemoClear(MEMO_T *_pMemo)
{
    _pMemo->Len = 0;
    _pMemo->Text[0] = 0;
    _pMemo->FifoWrite = 0;
    _pMemo->FifoRead = 0;
    
    LCD_Fill_Rect(_pMemo->Left + 1, _pMemo->Top + 1, _pMemo->Height - 2, _pMemo->Width - 2, EDIT_BACK_COLOR);
}

/*
*********************************************************************************************************
*    函 数 名: LCD_InitMemo
*    功能说明: 初始化多行文本框
*    形    参: _pMemo 对象
*    返 回 值: 无
*********************************************************************************************************
*/
void LCD_InitMemo(MEMO_T *_pMemo)
{
    _pMemo->id = 0;
    _pMemo->Len = 0;
    _pMemo->Text[0] = 0; 
    _pMemo->FifoWrite = 0;
    _pMemo->FifoRead = 0;    
    _pMemo->Refresh = 0;
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawMemo
*    功能说明: 在LCD上绘制一个多行文本框
*    形    参:
*            _usX, _usY : 图片的坐标
*            _usHeight  : 图片高度
*            _usWidth   : 图片宽度
*            _ptr       : 图片点阵指针
*    返 回 值: 无
*********************************************************************************************************
*/
#define LINE_CAP    2       /* 文字行间距 */
void LCD_DrawMemo(MEMO_T *_pMemo)
{
    uint16_t x, y;
    uint16_t TextLineNum;
    uint16_t line;
    uint16_t CanDispLineNum;    /* 可以显示的行数 */
    int16_t BeginLine;
    uint32_t i;
    uint8_t FontHeight;
    char buf[128];          /* 每行最大128字符 */
    uint16_t CharNum;       /* 自动换行时，每行字符个数 */
    uint16_t LenPerLine;    /* 每行最大字符数 */
    uint16_t FifoRead;
    uint16_t FifoWrite;
    uint16_t FifoLen;       
    
    _pMemo->Refresh = 0;
    
    if (_pMemo->Text == 0)
    {
        return;
    }
    
    DISABLE_INT();
    FifoRead = _pMemo->FifoRead;    /*  确定首字符位置 */
    FifoWrite = _pMemo->FifoWrite;
    ENABLE_INT();
    
    if (FifoRead == FifoWrite)
    {
        return;
    }
    else if (FifoWrite  > FifoRead)
    {
        FifoLen = FifoWrite - FifoRead;
    }
    else
    {
        FifoLen = _pMemo->MaxLen - FifoRead + FifoWrite;
    }
    
    /* 绘制边框，填充窗口 */
    LCD_DrawRect(_pMemo->Left, _pMemo->Top, _pMemo->Height, _pMemo->Width, EDIT_BORDER_COLOR);
    LCD_Fill_Rect(_pMemo->Left + 1, _pMemo->Top + 1, _pMemo->Height - 2, _pMemo->Width - 2, EDIT_BACK_COLOR);
    
    if (_pMemo->WordWrap == 0)    /* 不自动换行 */
    {
        /* 解析文本，计算行数 */
        TextLineNum = 0;
        for (i = 0; i < FifoLen; i++)
        {
            uint8_t ch;
            uint16_t idx;
            
            idx = FifoRead + i;
            if (idx >= _pMemo->MaxLen)
            {
                idx = idx - _pMemo->MaxLen;
            }
            ch = _pMemo->Text[idx];
            
            if (ch == 0)
            {
                break;
            }
            if (ch == 0x0D || ch == 0x0A)
            {
                TextLineNum++;
                
                idx++;
                if (idx >= _pMemo->MaxLen)
                {
                    idx = idx - _pMemo->MaxLen;
                }
                ch = _pMemo->Text[idx];
            
                if (ch == 0x0A)
                {
                    i++;
                }
            }
        }
        _pMemo->LineCount = TextLineNum;
        
        /* 计算可以显示的行数 */
        FontHeight = LCD_GetFontHeight(_pMemo->Font);
        CanDispLineNum = (_pMemo->Height - 2) / (FontHeight + LINE_CAP) - 1;
        
        /* 计算第1行位置 */
        if (TextLineNum <= CanDispLineNum)
        {
            BeginLine = 0;
        }
        else
        {
            BeginLine = TextLineNum - CanDispLineNum;
        }
        
        /* 翻页功能 */
        BeginLine += _pMemo->LineOffset;
        if (BeginLine >= TextLineNum)
        {
            BeginLine = TextLineNum - 1;
            //_pMemo->LineOffset = 0;
        }
        if (BeginLine < 0)
        {
            BeginLine = 0;
            //_pMemo->LineOffset = 0;
        }        
        
        x = _pMemo->Left + 2;
        y = _pMemo->Top + 2;
        line = 0;
        CharNum = 0;
        for (i = 0; i < FifoLen; i++)
        {
            uint8_t ch;
            uint16_t idx;
            
            idx = FifoRead + i;
            if (idx >= _pMemo->MaxLen)
            {
                idx = idx - _pMemo->MaxLen;
            }
            ch = _pMemo->Text[idx];
            
            if (ch == 0)
            {
                if (CharNum > 0)
                {
                    buf[CharNum] = 0;
                    LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                }
                break;
            }         
            
            if (CharNum < sizeof(buf))
            {
                buf[CharNum++] = ch;
            }
            
            if (ch == 0x0D || ch == 0x0A)
            {
                if (line++ >= BeginLine)
                {         
                    buf[CharNum] = 0;
                    LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                    
                    y += FontHeight + LINE_CAP;
                }
                CharNum = 0;

                if (++idx >= _pMemo->MaxLen)
                {
                    idx = idx - _pMemo->MaxLen;
                }
                
                ch = _pMemo->Text[idx];                
                if (ch == 0x0A)
                {
                    i++;
                }
            }
        }
    }
    else    /* 自动换行 */
    {
        if (_pMemo->Font->FontCode == FC_ST_12)
        {
            LenPerLine = (_pMemo->Width - 4) / 6;
        }
        else if (_pMemo->Font->FontCode == FC_ST_16)
        {
            LenPerLine = (_pMemo->Width - 4) / 8;
        }
        else if (_pMemo->Font->FontCode == FC_ST_24)
        {
            LenPerLine = (_pMemo->Width - 4) / 12;
        }
        else
        {
            LenPerLine = (_pMemo->Width - 4) / 8;
        }
        
        /* 解析文本，计算行数 */
        TextLineNum = 0;
        CharNum = 0;
        for (i = 0; i < FifoLen; i++)
        {           
            uint8_t ch;
            uint16_t idx;
            
            idx = FifoRead + i;
            if (idx >= _pMemo->MaxLen)
            {
                idx = idx - _pMemo->MaxLen;
            }
            ch = _pMemo->Text[idx];
            
            if (ch == 0x0D || ch == 0x0A)
            {
                TextLineNum++;
                CharNum = 0;
                
                if (++idx >= _pMemo->MaxLen)
                {
                    idx = idx - _pMemo->MaxLen;
                }
                ch = _pMemo->Text[idx];                
                if (ch == 0x0A)
                {
                    i++;
                }                
            }
            else
            {
                if (ch == 0)
                {
                    if (CharNum > 0)
                    {
                        TextLineNum++;
                    }
                    break;
                } 
                
                CharNum++;
                if (ch > 0x80)     /* 简单处理半个汉字问题 */
                {
                    CharNum++;
                    i++;
                }
                
                if (CharNum > LenPerLine - 1)
                {              
                    TextLineNum++;
                    CharNum = 0;
                }

            }
        }
        _pMemo->LineCount = TextLineNum;
        
        /* 计算可以显示的行数 */
        FontHeight = LCD_GetFontHeight(_pMemo->Font);
        CanDispLineNum = (_pMemo->Height - 2) / (FontHeight + LINE_CAP) - 1;
        
        /* 计算第1行位置 */
        if (TextLineNum <= CanDispLineNum)
        {
            BeginLine = 0;
        }
        else
        {
            BeginLine = TextLineNum - CanDispLineNum;
        }

        /* 翻页功能 */
        BeginLine += _pMemo->LineOffset;
        if (BeginLine >= TextLineNum)
        {
            BeginLine = TextLineNum - 1;
            _pMemo->LineOffset--;
        }
        if (BeginLine < 0)
        {
            BeginLine = 0;
            _pMemo->LineOffset++;
        }         
        
        /* 开始显示 */
        x = _pMemo->Left + 2;
        y = _pMemo->Top + 2;
        line = 0;
        CharNum = 0;
        for (i = 0; i < FifoLen; i++)
        {
            uint8_t ch;
            uint16_t idx;
            
            idx = FifoRead + i;
            if (idx >= _pMemo->MaxLen)
            {
                idx = idx - _pMemo->MaxLen;
            }
            ch = _pMemo->Text[idx];
            
            if (ch == 0x0D || ch == 0x0A)
            {
                buf[CharNum] = 0;
                CharNum = 0;
                if (line >= BeginLine && line < BeginLine + CanDispLineNum)
                {
                    LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                    
                    y += FontHeight + LINE_CAP;   
                }
                line++;
                
                if (++idx >= _pMemo->MaxLen)
                {
                    idx = idx - _pMemo->MaxLen;
                }
                ch = _pMemo->Text[idx];                
                if (ch == 0x0A)
                {
                    i++;
                }                
            }
            else
            {
                if (ch == 0)
                {
                    if (CharNum > 0)
                    {
                        buf[CharNum] = 0;
                        CharNum = 0;
                        if (line >= BeginLine && line < BeginLine + CanDispLineNum)
                        {
                            LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                    
                            y += FontHeight + LINE_CAP;
                        }
                        line++;
                    }
                    break;
                } 
                
                buf[CharNum] = ch;
                CharNum++;
                if (ch > 0x80)     /* 简单处理半个汉字问题 */
                {
                    if (++idx >= _pMemo->MaxLen)
                    {
                        idx = idx - _pMemo->MaxLen;
                    }
                    ch = _pMemo->Text[idx];                      
                    buf[CharNum] = ch;
                    CharNum++;
                    i++;
                }
                
                if (CharNum > LenPerLine - 1)
                {             
                    if (line >= BeginLine && line < BeginLine + CanDispLineNum)
                    {
                        buf[CharNum] = 0;
                           
                        LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                        
                        y += FontHeight + LINE_CAP;
                    }
                    line++;                    
                    CharNum = 0;                    
                }                           
            }
        }         
    }

    if (CharNum > 0)
    {             
        buf[CharNum] = 0;
               
        LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
            
        y += FontHeight + LINE_CAP;                    
        CharNum = 0;                    
    }     
}

/*
*********************************************************************************************************
*    函 数 名: LCD_DrawMemoEx
*    功能说明: 在LCD上绘制一个多行文本框(增强版），支持文本着色（未完成）
*    形    参:
*            _usX, _usY : 图片的坐标
*            _usHeight  : 图片高度
*            _usWidth   : 图片宽度
*            _ptr       : 图片点阵指针
*    返 回 值: 无
*********************************************************************************************************
*/
#define LINE_CAP    2       /* 文字行间距 */
void LCD_DrawMemoEx(MEMO_T *_pMemo)
{
    uint16_t x, y;
    uint16_t TextLineNum;
    uint16_t line;
    uint16_t CanDispLineNum;    /* 可以显示的行数 */
    int16_t BeginLine;
    uint32_t i;
    uint8_t FontHeight;
    char buf[128];          /* 每行最大128字符 */
    uint16_t CharNum;       /* 自动换行时，每行字符个数 */
    uint16_t LenPerLine;    /* 每行最大字符数 */
    uint16_t FifoRead;
    uint16_t FifoWrite;
    uint16_t FifoLen;       
    
    _pMemo->Refresh = 0;
    
    if (_pMemo->Text == 0)
    {
        return;
    }
    
    DISABLE_INT();
    FifoRead = _pMemo->FifoRead;    /*  确定首字符位置 */
    FifoWrite = _pMemo->FifoWrite;
    ENABLE_INT();
    
    if (FifoRead == FifoWrite)
    {
        return;
    }
    else if (FifoWrite  > FifoRead)
    {
        FifoLen = FifoWrite - FifoRead;
    }
    else
    {
        FifoLen = _pMemo->MaxLen - FifoRead + FifoWrite;
    }
    
    /* 绘制边框，填充窗口 */
    LCD_DrawRect(_pMemo->Left, _pMemo->Top, _pMemo->Height, _pMemo->Width, EDIT_BORDER_COLOR);
    LCD_Fill_Rect(_pMemo->Left + 1, _pMemo->Top + 1, _pMemo->Height - 2, _pMemo->Width - 2, EDIT_BACK_COLOR);
    
    if (_pMemo->WordWrap == 0)    /* 不自动换行 */
    {
        /* 解析文本，计算行数 */
        TextLineNum = 0;
        for (i = 0; i < FifoLen; i++)
        {
            uint8_t ch;
            uint16_t idx;
            
            idx = FifoRead + i;
            if (idx >= _pMemo->MaxLen)
            {
                idx = idx - _pMemo->MaxLen;
            }
            ch = _pMemo->Text[idx];
            
            if (ch == 0)
            {
                break;
            }
            if (ch == 0x0D || ch == 0x0A)
            {
                TextLineNum++;
                
                idx++;
                if (idx >= _pMemo->MaxLen)
                {
                    idx = idx - _pMemo->MaxLen;
                }
                ch = _pMemo->Text[idx];
            
                if (ch == 0x0A)
                {
                    i++;
                }
            }
        }
        _pMemo->LineCount = TextLineNum;
        
        /* 计算可以显示的行数 */
        FontHeight = LCD_GetFontHeight(_pMemo->Font);
        CanDispLineNum = (_pMemo->Height - 2) / (FontHeight + LINE_CAP) - 1;
        
        /* 计算第1行位置 */
        if (TextLineNum <= CanDispLineNum)
        {
            BeginLine = 0;
        }
        else
        {
            BeginLine = TextLineNum - CanDispLineNum;
        }
        
        /* 翻页功能 */
        BeginLine += _pMemo->LineOffset;
        if (BeginLine >= TextLineNum)
        {
            BeginLine = TextLineNum - 1;
            //_pMemo->LineOffset = 0;
        }
        if (BeginLine < 0)
        {
            BeginLine = 0;
            //_pMemo->LineOffset = 0;
        }        
        
        x = _pMemo->Left + 2;
        y = _pMemo->Top + 2;
        line = 0;
        CharNum = 0;
        for (i = 0; i < FifoLen; i++)
        {
            uint8_t ch;
            uint16_t idx;
            
            idx = FifoRead + i;
            if (idx >= _pMemo->MaxLen)
            {
                idx = idx - _pMemo->MaxLen;
            }
            ch = _pMemo->Text[idx];
            
            if (ch == 0)
            {
                if (CharNum > 0)
                {
                    buf[CharNum] = 0;
                    LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                }
                break;
            }         
            
            if (CharNum < sizeof(buf))
            {
                buf[CharNum++] = ch;
            }
            
            if (ch == 0x0D || ch == 0x0A)
            {
                if (line++ >= BeginLine)
                {         
                    buf[CharNum] = 0;
                    LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                    
                    y += FontHeight + LINE_CAP;
                }
                CharNum = 0;

                if (++idx >= _pMemo->MaxLen)
                {
                    idx = idx - _pMemo->MaxLen;
                }
                
                ch = _pMemo->Text[idx];                
                if (ch == 0x0A)
                {
                    i++;
                }
            }
        }
    }
    else    /* 自动换行 */
    {
        if (_pMemo->Font->FontCode == FC_ST_12)
        {
            LenPerLine = (_pMemo->Width - 4) / 6;
        }
        else if (_pMemo->Font->FontCode == FC_ST_16)
        {
            LenPerLine = (_pMemo->Width - 4) / 8;
        }
        else if (_pMemo->Font->FontCode == FC_ST_24)
        {
            LenPerLine = (_pMemo->Width - 4) / 12;
        }
        else
        {
            LenPerLine = (_pMemo->Width - 4) / 8;
        }
        
        /* 解析文本，计算行数 */
        TextLineNum = 0;
        CharNum = 0;
        for (i = 0; i < FifoLen; i++)
        {           
            uint8_t ch;
            uint16_t idx;
            
            idx = FifoRead + i;
            if (idx >= _pMemo->MaxLen)
            {
                idx = idx - _pMemo->MaxLen;
            }
            ch = _pMemo->Text[idx];
            
            if (ch == 0x0D || ch == 0x0A)
            {
                TextLineNum++;
                CharNum = 0;
                
                if (++idx >= _pMemo->MaxLen)
                {
                    idx = idx - _pMemo->MaxLen;
                }
                ch = _pMemo->Text[idx];                
                if (ch == 0x0A)
                {
                    i++;
                }                
            }
            else
            {
                if (ch == 0)
                {
                    if (CharNum > 0)
                    {
                        TextLineNum++;
                    }
                    break;
                } 
                
                CharNum++;
                if (ch > 0x80)     /* 简单处理半个汉字问题 */
                {
                    CharNum++;
                    i++;
                }
                
                if (CharNum > LenPerLine - 1)
                {              
                    TextLineNum++;
                    CharNum = 0;
                }

            }
        }
        _pMemo->LineCount = TextLineNum;
        
        /* 计算可以显示的行数 */
        FontHeight = LCD_GetFontHeight(_pMemo->Font);
        CanDispLineNum = (_pMemo->Height - 2) / (FontHeight + LINE_CAP) - 1;
        
        /* 计算第1行位置 */
        if (TextLineNum <= CanDispLineNum)
        {
            BeginLine = 0;
        }
        else
        {
            BeginLine = TextLineNum - CanDispLineNum;
        }

        /* 翻页功能 */
        BeginLine += _pMemo->LineOffset;
        if (BeginLine >= TextLineNum)
        {
            BeginLine = TextLineNum - 1;
            _pMemo->LineOffset--;
        }
        if (BeginLine < 0)
        {
            BeginLine = 0;
            _pMemo->LineOffset++;
        }         
        
        /* 开始显示 */
        x = _pMemo->Left + 2;
        y = _pMemo->Top + 2;
        line = 0;
        CharNum = 0;
        for (i = 0; i < FifoLen; i++)
        {
            uint8_t ch;
            uint16_t idx;
            
            idx = FifoRead + i;
            if (idx >= _pMemo->MaxLen)
            {
                idx = idx - _pMemo->MaxLen;
            }
            ch = _pMemo->Text[idx];
            
            if (ch == 0x0D || ch == 0x0A)
            {
                buf[CharNum] = 0;
                CharNum = 0;
                if (line >= BeginLine && line < BeginLine + CanDispLineNum)
                {
                    LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                    
                    y += FontHeight + LINE_CAP;   
                }
                line++;
                
                if (++idx >= _pMemo->MaxLen)
                {
                    idx = idx - _pMemo->MaxLen;
                }
                ch = _pMemo->Text[idx];                
                if (ch == 0x0A)
                {
                    i++;
                }                
            }
            else
            {
                if (ch == 0)
                {
                    if (CharNum > 0)
                    {
                        buf[CharNum] = 0;
                        CharNum = 0;
                        if (line >= BeginLine && line < BeginLine + CanDispLineNum)
                        {
                            LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                    
                            y += FontHeight + LINE_CAP;
                        }
                        line++;
                    }
                    break;
                } 
                
                buf[CharNum] = ch;
                CharNum++;
                if (ch > 0x80)     /* 简单处理半个汉字问题 */
                {
                    if (++idx >= _pMemo->MaxLen)
                    {
                        idx = idx - _pMemo->MaxLen;
                    }
                    ch = _pMemo->Text[idx];                      
                    buf[CharNum] = ch;
                    CharNum++;
                    i++;
                }
                
                if (CharNum > LenPerLine - 1)
                {             
                    if (line >= BeginLine && line < BeginLine + CanDispLineNum)
                    {
                        buf[CharNum] = 0;
                           
                        LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
                        
                        y += FontHeight + LINE_CAP;
                    }
                    line++;                    
                    CharNum = 0;                    
                }                           
            }
        }         
    }

    if (CharNum > 0)
    {             
        buf[CharNum] = 0;
               
        LCD_DispStrEx(x, y, buf, _pMemo->Font, _pMemo->Width - 4, ALIGN_LEFT);
            
        y += FontHeight + LINE_CAP;                    
        CharNum = 0;                    
    }     
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
