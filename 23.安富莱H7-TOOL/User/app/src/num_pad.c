/*
*********************************************************************************************************
*
*    模块名称 : 数字小键盘界面
*    文件名称 : main.c
*    版    本 : V1.0
*    说    明 : 数字小键盘界面主程序。弹出数字小键盘用于输入。
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2013-01-01  armfly  正式发布
*        V1.1    2014-04-25  armfly  (1) 调试整数输入分支， 增加字符串输入（应客户要求）
*        V1.2    2015-07-22  armfly  
*                    (1) 增加输入整数的函数，InputInt( ), 对 InputNumber() 函数重新封装
*                    (2) 增加字符输入功能, NUMPAD_STR
*                                    
*
*
*    Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "num_pad.h"

/* 输入数字最大长度 */
#define INPUT_MAX_LEN 32

/* 定义界面结构 */
typedef struct
{
    FONT_T FontBlack;    /* 黑色 */
    FONT_T FontBlue;     /* 蓝色 */
    FONT_T FontBtn;         /* 控制按钮F1-F4的字体 */
    FONT_T FontBtnNum; /* 数字按钮的字体 */
    FONT_T FontWin;         /* WIN的字体 */
    FONT_T FontEdit;     /* EDIT的字体 */

    EDIT_T Edit;
    WIN_T Win;
    char *pTitle; /* 窗口标题 */

    BUTTON_T Btn1;
    BUTTON_T Btn2;
    BUTTON_T Btn3;
    BUTTON_T Btn4;
    BUTTON_T Btn5;
    BUTTON_T Btn6;
    BUTTON_T Btn7;
    BUTTON_T Btn8;
    BUTTON_T Btn9;
    BUTTON_T Btn0;
    BUTTON_T BtnA;
    BUTTON_T BtnB;
    BUTTON_T BtnF1;
    BUTTON_T BtnF2;
    BUTTON_T BtnF3;
    BUTTON_T BtnF4;

    /* 编辑框内*/
    char *pEditText;
} FormPad_T;

/* 键盘显示模式模式 */
enum
{
    PAD_DIGIT = 0, /* 纯数字模式 */
    PAD_CHAR,             /* 特殊字符 */
    PAD_ABC,             /* T9，数字和字母 */
    PAD_ABC0,             /* T9，二级界面，选择0 之后*/
    PAD_ABC1,             /* T9，二级界面，选择1 之后*/
    PAD_ABC2,
    PAD_ABC3,
    PAD_ABC4,
    PAD_ABC5,
    PAD_ABC6,
    PAD_ABC7,
    PAD_ABC8,
    PAD_ABC9,
    PAD_ABCA,
    PAD_ABCB
};

/* 数字小键盘 窗口 */
enum
{
#if 1 /* fot 480*272 */
    /* 数字小键盘窗口位置 */
    WIN_NUM_X = 100,
    WIN_NUM_Y = 8,

    /* 数字按钮(0-9,.*)高度和宽度,全部一样 */
    BTN_H = 40,
    BTN_W = 70,

    /* 控制按钮（退格,CLR,RET,OK）高度和宽度,全部一样 */
    BUTTON_CTRL_H = 40,
    BUTTON_CTRL_W = 80,

    /* 数字小键盘窗口大小 */
    WIN_NUM_H = 249,
    WIN_NUM_W = (BTN_W * 3) + BUTTON_CTRL_W + 8 + 6,

    /* 输入数字编辑框 */
    EDIT_NUM_X = WIN_NUM_X + 4,
    EDIT_NUM_Y = WIN_NUM_Y + 34,
    EDIT_NUM_H = 40,
    EDIT_NUM_W = WIN_NUM_W - 8,

#else /* for 800 * 480 */
    /* 数字按钮(0-9,.*)高度和宽度,全部一样 */
    //BTN_H = 40,
    BTN_H = 60,
    BTN_W = 70,

    /* 控制按钮（退格,CLR,RET,OK）高度和宽度,全部一样 */
    //BUTTON_CTRL_H = 40,
    BTN_F_H = 60,
    BTN_F_W = 80,

    /* 数字小键盘窗口大小 */
    WIN_NUM_H = (BTN_H * 4 + 90),
    WIN_NUM_W = (BTN_W * 3) + BUTTON_CTRL_W + 8 + 6,

    /* 数字小键盘窗口位置 (按800 * 480的分辨率居中) */
    WIN_NUM_X = ((800 - WIN_NUM_W) / 2),
    WIN_NUM_Y = ((480 - WIN_NUM_H) / 2),

    /* 输入数字编辑框 */
    EDIT_NUM_X = WIN_NUM_X + 4,
    EDIT_NUM_Y = WIN_NUM_Y + 34,
    EDIT_NUM_H = 40,
    EDIT_NUM_W = WIN_NUM_W - 8,

#endif
    BUTTON_X_SPACE = BTN_W + 2,
    BUTTON_Y_SPACE = BTN_H + 2,

    /* 数字1 按钮 */
    BTN1_X = WIN_NUM_X + 4,
    BTN1_Y = EDIT_NUM_Y + 43,
    /* 数字2 按钮 */
    BTN2_X = BTN1_X + BUTTON_X_SPACE,
    BTN2_Y = BTN1_Y,
    /* 数字3 按钮 */
    BTN3_X = BTN1_X + 2 * BUTTON_X_SPACE,
    BTN3_Y = BTN1_Y,
    /* F1 按钮 */
    BTNF1_X = BTN1_X + 3 * BUTTON_X_SPACE,
    BTNF1_Y = BTN1_Y,

    /* 数字4 按钮 */
    BTN4_X = BTN1_X,
    BTN4_Y = BTN1_Y + BUTTON_Y_SPACE,
    /* 数5 按钮 */
    BTN5_X = BTN1_X + BUTTON_X_SPACE,
    BTN5_Y = BTN1_Y + BUTTON_Y_SPACE,
    /* 数字6 按钮 */
    BTN6_X = BTN1_X + 2 * BUTTON_X_SPACE,
    BTN6_Y = BTN1_Y + BUTTON_Y_SPACE,
    /* F2 按钮 */
    BTNF2_X = BTN1_X + 3 * BUTTON_X_SPACE,
    BTNF2_Y = BTN1_Y + BUTTON_Y_SPACE,

    /* 数字7 按钮 */
    BTN7_X = BTN1_X,
    BTN7_Y = BTN1_Y + 2 * BUTTON_Y_SPACE,
    /* 数字8 按钮 */
    BTN8_X = BTN1_X + BUTTON_X_SPACE,
    BTN8_Y = BTN1_Y + 2 * BUTTON_Y_SPACE,
    /* 数字9 按钮 */
    BTN9_X = BTN1_X + 2 * BUTTON_X_SPACE,
    BTN9_Y = BTN1_Y + 2 * BUTTON_Y_SPACE,
    /* F3 按钮 */
    BTNF3_X = BTN1_X + 3 * BUTTON_X_SPACE,
    BTNF3_Y = BTN1_Y + 2 * BUTTON_Y_SPACE,

    /* 数字A 按钮 */
    BTNA_X = BTN1_X,
    BTNA_Y = BTN1_Y + 3 * BUTTON_Y_SPACE,
    /* 数字0 按钮 */
    BTN0_X = BTN1_X + BUTTON_X_SPACE,
    BTN0_Y = BTN1_Y + 3 * BUTTON_Y_SPACE,
    /* B 按钮 */
    BTNB_X = BTN1_X + 2 * BUTTON_X_SPACE,
    BTNB_Y = BTN1_Y + 3 * BUTTON_Y_SPACE,
    /* F4按钮 */
    BTNF4_X = BTN1_X + 3 * BUTTON_X_SPACE,
    BTNF4_Y = BTN1_Y + 3 * BUTTON_Y_SPACE,
};

/*　输入无效窗口　*/
enum
{
    WIN_INVALID_X = WIN_NUM_X + 15,
    WIN_INVALID_Y = WIN_NUM_Y + 90,
    WIN_INVALID_H = 100,
    WIN_INVALID_W = WIN_NUM_W - 40,

    LABLE_INVALID_X = WIN_INVALID_X + 5,
    LABLE_INVALID_Y = WIN_INVALID_Y + 38,
};

#define F1_NAME "退格"
#define F2_NAME "清除"
#define F3_NAME "返回"
#define F4_NAME "确认"

/* 字符输入模式下,2级菜单按键字符 */
const char *g_key_tab[12 * 12] =
        {
#if 1
                "0",
                " ",
                "!",
                "@",
                "#",
                "$",
                "%",
                "=",
                "*",
                "(",
                ")",
                "|",
                "1",
                "-",
                "[",
                "]",
                "\\",
                "/",
                ":",
                ";",
                "\042",
                " ",
                " ",
                " ",
                "2",
                "a",
                "b",
                "c",
                "A",
                "B",
                "C",
                " ",
                " ",
                " ",
                " ",
                " ",
                "3",
                "d",
                "e",
                "f",
                "D",
                "E",
                "F",
                " ",
                " ",
                " ",
                " ",
                " ",
                "4",
                "g",
                "h",
                "i",
                "G",
                "H",
                "I",
                " ",
                " ",
                " ",
                " ",
                " ",
                "5",
                "j",
                "k",
                "l",
                "J",
                "K",
                "L",
                " ",
                " ",
                " ",
                " ",
                " ",
                "6",
                "m",
                "n",
                "o",
                "M",
                "N",
                "O",
                " ",
                " ",
                " ",
                " ",
                " ",
                "7",
                "p",
                "q",
                "r",
                "s",
                "P",
                "Q",
                "R",
                "S",
                " ",
                " ",
                " ",
                "8",
                "t",
                "u",
                "v",
                "T",
                "U",
                "V",
                " ",
                " ",
                " ",
                " ",
                " ",
                "9",
                "w",
                "x",
                "y",
                "z",
                "W",
                "X",
                "Y",
                "Z",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
#else
                " ",
                "!",
                "@",
                "#",
                "$",
                "%",
                "=",
                "*",
                "(",
                "0",
                ")",
                "|",
                "-",
                "[",
                "]",
                "\\",
                "/",
                ":",
                ";",
                "\042",
                " ",
                "1",
                " ",
                "a",
                "b",
                "c",
                "A",
                "B",
                "C",
                " ",
                " ",
                " ",
                "2",
                " ",
                " ",
                "d",
                "e",
                "f",
                "D",
                "E",
                "F",
                " ",
                " ",
                " ",
                "3",
                " ",
                " ",
                "g",
                "h",
                "i",
                "G",
                "H",
                "I",
                " ",
                " ",
                " ",
                "4",
                " ",
                " ",
                "j",
                "k",
                "l",
                "J",
                "K",
                "L",
                " ",
                " ",
                " ",
                "5",
                " ",
                " ",
                "m",
                "n",
                "o",
                "M",
                "N",
                "O",
                " ",
                " ",
                " ",
                "6",
                " ",
                " ",
                "p",
                "q",
                "r",
                "s",
                "P",
                "Q",
                "R",
                "S",
                " ",
                "7",
                " ",
                " ",
                "t",
                "u",
                "v",
                "T",
                "U",
                "V",
                " ",
                " ",
                " ",
                "8",
                " ",
                " ",
                "w",
                "x",
                "y",
                "z",
                "W",
                "X",
                "Y",
                "Z",
                " ",
                "9",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
                " ",
#endif
};

static void InitFormPad(void);
static void DispFormPad(void);
static void DispAllBtn(void);
static void SetBtnText(uint8_t _mode);
static uint8_t GetCharPad(uint8_t _key1, uint8_t _key2);

FormPad_T *FormPad;

/*
*********************************************************************************************************
*    函 数 名: InputInt
*    功能说明: 调出数字小键盘，输入整数。内部会判断值域.
*    形    参：
*             _ucMode : 工作模式， 0 表示IP地址； 1表示输入普通整数
*            _Caption : 数字小键盘界面的窗口标题
*            _pInParam : 输入参数
*                    _ucMode = NUMPAD_STR  时，表示输入字符串的长度，uint8_t 型
*                    _ucMode = NUMPAD_INT  带小数点整数输入模式，(NUMPAD_INT_T *) 
*                            typedef struct
*                            {
*                                int32_t Min;    // 最小值
*                                int32_t Max;    // 最大值
*                                int32_t Return;    // 存放输入结果
*                                uint8_t DotNum;    // 小数点位数
*                            }NUMPAD_INT_T;
*                    _ucMode = NUMPAD_IP    ，此参数可以填0
*                    _ucMode = NUMPAD_TEL ，此参数可以填0
*            _pOutParam  : 输出参数
*    返 回 值: 1表示输入有效  0 表示输入无效
*********************************************************************************************************
*/
uint8_t InputInt(char *_Caption, int32_t _min, int32_t _max, int32_t *_value)
{
    char buf[13];
    NUMPAD_INT_T Param;

    Param.Min = _min; /* 最小值 */
    Param.Max = _max; /* 最大值 */
    Param.DotNum = 0; /* 小数点位数 */

    if (InputNumber(NUMPAD_INT, _Caption, &Param, (void *)buf))
    {
        *_value = str_to_int(buf);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: InputNumber
*    功能说明: 数字小键盘
*    形    参：
*             _ucMode : 工作模式， 0 表示IP地址； 1表示输入普通整数
*            _Caption : 数字小键盘界面的窗口标题
*            _pInParam : 输入参数
*                    _ucMode = NUMPAD_STR  时，表示输入字符串的长度，uint8_t 型
*                    _ucMode = NUMPAD_INT  带小数点整数输入模式，(NUMPAD_INT_T *) 
*                            typedef struct
*                            {
*                                int32_t Min;    // 最小值
*                                int32_t Max;    // 最大值
*                                int32_t Return;    // 存放输入结果
*                                uint8_t DotNum;    // 小数点位数
*                            }NUMPAD_INT_T;
*                    _ucMode = NUMPAD_IP    ，此参数可以填0
*                    _ucMode = NUMPAD_TEL ，此参数可以填0
*            _pOutParam  : 输出参数
*    返 回 值: 1表示输入有效  0 表示输入无效
*********************************************************************************************************
*/
uint8_t InputNumber(NUMPAD_MODE_E _Mode, char *_Caption, void *_pInParam, void *_pOutParam)
{
    uint8_t ucTouch;    /* 触摸事件 */
    uint8_t fRefresh; /* 刷屏请求标志,1表示需要刷新 */
    int16_t tpX, tpY;
    uint8_t ucCursor; /* 光标 */
    uint8_t ucKeyValue = 0xFF;
    FormPad_T form;
    char cEditBuf[INPUT_MAX_LEN + 2]; /* 编辑框内数值 */
    uint8_t pad_layer = 0;
    uint8_t pad_key = 0;

    FormPad = &form;

    FormPad->pTitle = _Caption;
    FormPad->pEditText = cEditBuf;

    InitFormPad();

    /* IP地址 */ /* 电话号码 */ /* 整数（带范围判断） */ /* 任意字母数字 小数点 */
    if (_Mode == NUMPAD_IP || _Mode == NUMPAD_TEL || _Mode == NUMPAD_INT)
    {
        SetBtnText(PAD_DIGIT); /* 纯数字键盘 */
    }
    else
    {
        SetBtnText(PAD_ABC); /* T9，数字和字母 */
    }

    DispFormPad();

    ucCursor = 0;
    cEditBuf[0] = 0;
    fRefresh = 1; /* 1表示需要刷新LCD */
    while (1)
    {
        bsp_Idle();

        if (fRefresh)
        {
            fRefresh = 0;

            LCD_DrawEdit(&FormPad->Edit);
        }

        ucTouch = TOUCH_GetKey(&tpX, &tpY); /* 读取触摸事件 */
        if (ucTouch != TOUCH_NONE)
        {
            switch (ucTouch)
            {
            case TOUCH_DOWN:                                                                            /* 触笔按下事件 */
                if (LCD_ButtonTouchDown(&FormPad->BtnF1, tpX, tpY)) /* 退格 */
                {
                    if (ucCursor > 0)
                    {
                        ucCursor--;
                    }
                    cEditBuf[ucCursor] = 0;
                    fRefresh = 1;
                }
                else if (LCD_ButtonTouchDown(&FormPad->BtnF2, tpX, tpY))
                {
                    ucCursor = 0; /* "清除"; */
                    cEditBuf[ucCursor] = 0;
                    fRefresh = 1;
                }
                else if (LCD_ButtonTouchDown(&FormPad->BtnF3, tpX, tpY))
                {
                    if (pad_layer == 1)
                    {
                        fRefresh = 1;
                        FormPad->FontBtnNum.FrontColor = CL_BLACK; /* 数字按钮用黑色字体 */
                        SetBtnText(PAD_ABC);
                        DispAllBtn(); /* 只刷新按钮 */
                        pad_layer = 0;
                    }
                    else
                    {
                        return 0; /* "返回"; */
                    }
                }
                else if (LCD_ButtonTouchDown(&FormPad->BtnF4, tpX, tpY))
                {
                    ; /* "确认 ，等按钮释放时再确认修改 */
                }
                else
                {
                    ucKeyValue = 0xFF;
                    if (LCD_ButtonTouchDown(&FormPad->Btn0, tpX, tpY))
                    {
                        ucKeyValue = '0';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn1, tpX, tpY))
                    {
                        ucKeyValue = '1';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn2, tpX, tpY))
                    {
                        ucKeyValue = '2';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn3, tpX, tpY))
                    {
                        ucKeyValue = '3';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn4, tpX, tpY))
                    {
                        ucKeyValue = '4';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn5, tpX, tpY))
                    {
                        ucKeyValue = '5';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn6, tpX, tpY))
                    {
                        ucKeyValue = '6';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn7, tpX, tpY))
                    {
                        ucKeyValue = '7';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn8, tpX, tpY))
                    {
                        ucKeyValue = '8';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->Btn9, tpX, tpY))
                    {
                        ucKeyValue = '9';
                    }
                    else if (LCD_ButtonTouchDown(&FormPad->BtnA, tpX, tpY))
                    {
                        ucKeyValue = 'A';
                    }
                    /* 星号键修改为减号，便于数码管显示 ----- */
                    else if (LCD_ButtonTouchDown(&FormPad->BtnB, tpX, tpY))
                    {
                        ucKeyValue = 'B';
                    }

                    if (_Mode == NUMPAD_STR)
                    {
                        if (ucKeyValue != 0xFF)
                        {
                            if (pad_layer == 0)
                            {
                                pad_layer = 1; /* 进入2级键盘 */
                                pad_key = ucKeyValue;

                                if (ucKeyValue == 'A')
                                {
                                    SetBtnText(PAD_ABCA);
                                }
                                else if (ucKeyValue == 'B')
                                {
                                    SetBtnText(PAD_ABCB);
                                }
                                else if (ucKeyValue >= '0' && ucKeyValue <= '9')
                                {
                                    SetBtnText(pad_key - '0' + PAD_ABC0); /* 纯数字键盘 */
                                }
                                else
                                {
                                    continue;
                                }
                                FormPad->FontBtnNum.FrontColor = CL_BLUE; /* 数字按钮用蓝色字体 */
                                DispAllBtn();                                                            /* 只刷新按钮 */
                                continue;
                            }
                            else /* 在二级菜单 */
                            {
                                ucKeyValue = GetCharPad(pad_key, ucKeyValue); /* 得到具体的字符 */
                                pad_layer = 2;

                                LCD_DrawEdit(&FormPad->Edit);
                            }
                        }
                    }
                    else
                    {
                        if (ucKeyValue == 'A')
                        {
                            ucKeyValue = '.';
                        }
                        else if (ucKeyValue == 'B')
                        {
                            ucKeyValue = '-';
                        }
                    }

                    if (ucKeyValue != 0xFF)
                    {
                        if (ucCursor < INPUT_MAX_LEN)
                        {
                            cEditBuf[ucCursor++] = ucKeyValue;
                        }
                        else
                        {
                            cEditBuf[ucCursor - 1] = ucKeyValue; /* 光标不移动，更新最后1个数字 */
                        }
                        cEditBuf[ucCursor] = 0;
                        fRefresh = 1;
                    }
                }
                break;

            case TOUCH_MOVE: /* 触笔移动事件 */
                break;

            case TOUCH_RELEASE: /* 触笔释放事件 */

                if (_Mode == NUMPAD_STR) /* 输入字符的分支 */
                {
                    if (pad_layer == 2)
                    {
                        pad_layer = 0; /* 进入2级键盘 */

                        FormPad->FontBtnNum.FrontColor = CL_BLACK; /* 数字按钮用黑色字体 */
                        SetBtnText(PAD_ABC);
                        DispAllBtn(); /* 刷新按钮 */
                    }
                }

                LCD_ButtonTouchRelease(&FormPad->Btn0, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn1, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn2, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn3, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn4, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn5, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn6, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn7, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn8, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->Btn9, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->BtnA, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->BtnB, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->BtnF1, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->BtnF2, tpX, tpY);
                LCD_ButtonTouchRelease(&FormPad->BtnF3, tpX, tpY);

                if (LCD_ButtonTouchRelease(&FormPad->BtnF4, tpX, tpY)) /* F4键 */
                {
                    /* 点击了 ok 按钮 */
                    if (_Mode == NUMPAD_IP) /* IP地址输入模式. 将IP地址的4个字段解码为4个整数 */
                    {
                        int32_t iIP;
                        uint8_t ucDotNum; /* 小数点个数，用于判断数据合法 */
                        int16_t i;
                        NUMPAD_IP_T *pIP = (NUMPAD_IP_T *)_pOutParam; /* IP地址输入模式 */

                        ucDotNum = 0;
                        for (i = 0; i < ucCursor; i++)
                        {
                            if (cEditBuf[i] == '.')
                            {
                                ucDotNum++;
                            }
                        }
                        if (ucDotNum != 3) /* 必须包括3个小数点 */
                        {
                            DispInvlidInput(); /* 显示输入无效 */

                            fRefresh = 1;
                        }
                        else
                        {
                            /* 有3个小数点，开始解码IP地址 */
                            ucDotNum = 0;
                            iIP = 0;
                            for (i = 0; i < ucCursor; i++)
                            {
                                if ((cEditBuf[i] == '.') || (i == ucCursor - 1))
                                {
                                    if (i == ucCursor - 1)
                                    {
                                        iIP = iIP * 10 + cEditBuf[i] - '0';
                                    }
                                    if (iIP <= 255)
                                    {
                                        pIP->ip_buf[ucDotNum] = iIP;
                                        ucDotNum++;

                                        if (ucDotNum == 4)
                                        {
                                            return 1; /* 返回正确结果 */
                                        }
                                        iIP = 0;
                                    }
                                    else
                                    {
                                        DispInvlidInput(); /* 显示输入无效 */
                                        fRefresh = 1;
                                        break;
                                    }
                                }
                                else
                                {
                                    iIP = iIP * 10 + cEditBuf[i] - '0';
                                }
                            }
                        }
                    }
                    else if (_Mode == NUMPAD_TEL) /* 电话号码输入模式。 直接复制结果（小数点已提前禁止） */
                    {
                        if (ucCursor < 3)
                        {
                            DispInvlidInput(); /* 显示输入无效 */
                            fRefresh = 1;
                            break;
                        }

                        strcpy((char *)_pOutParam, cEditBuf);
                        return 1; /* 返回正确结果 */
                    }
                    else if (_Mode == NUMPAD_INT) /* NUMPAD_INT - 带小数点整数输入模式 【该分支还未调试】*/
                    {
                        int32_t iRet;
                        uint8_t ucDotNum; /* 小数点个数，用于判断数据合法 */
                        int16_t i;
                        NUMPAD_INT_T *pInt = (NUMPAD_INT_T *)_pInParam; /* 带小数点整数输入模式 */

                        ucDotNum = 0;
                        for (i = 0; i < ucCursor; i++)
                        {
                            if (cEditBuf[i] == '.')
                            {
                                ucDotNum++;
                            }
                        }
                        if (ucDotNum > 1)
                        {
                            /* 小数点个数大于1 */
                            iRet = -1;
                        }
                        else if (ucDotNum == 1) /* 1个小数点 */
                        {
                            int32_t iDotValue;
                            int32_t iMult;

                            /* 先处理小数点前面的数字 */
                            iRet = 0;
                            for (i = 0; i < ucCursor; i++)
                            {
                                if (cEditBuf[i] == '.')
                                {
                                    break;
                                }
                                iRet *= 10;
                                iRet += cEditBuf[i] - '0';
                            }
                            iRet *= 1000;

                            /* 再处理小数点后面的数字 */
                            iDotValue = 0;
                            iMult = 100;
                            i++;
                            for (; i < ucCursor; i++)
                            {
                                iDotValue += iMult * (cEditBuf[i] - '0');
                                iMult /= 10;
                            }

                            iRet += iDotValue;
                        }
                        else
                        {
                            /* 全部整数 */
                            iRet = 0;
                            for (i = 0; i < ucCursor; i++)
                            {
                                iRet *= 10;
                                iRet += cEditBuf[i] - '0';
                            }
                        }

                        if ((ucCursor == 0) || (iRet < pInt->Min) || (iRet > pInt->Max))
                        {
                            DispInvlidInput(); /* 显示输入无效 */

                            DispFormPad(); /* 重刷界面 */
                            fRefresh = 1;
                        }
                        else
                        {
                            strcpy((char *)_pOutParam, cEditBuf);
                            return 1; /* 返回有效输入 */
                        }
                    }
                    else if (_Mode == NUMPAD_STR) /* 输入字符的分支 */
                    {
                        uint8_t len = *(uint8_t *)_pInParam;

                        /* 对字符串长度进行限制 */
                        if (ucCursor > len)
                        {
                            DispInvlidInput(); /* 显示输入无效 */
                            DispFormPad();         /* 重刷界面 */
                            fRefresh = 1;
                            break;
                        }
                        strcpy((char *)_pOutParam, cEditBuf);
                        return 1; /* 返回正确结果 */
                    }
                }
                break;
            }
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名：DispInvlidInput
*    功能说明：显示输入无效小窗口，延迟2秒
*    形    参：无
*    返 回 值：无
*********************************************************************************************************
*/
void DispInvlidInput(void)
{
    WIN_T tWin;
    LABEL_T tLable;
    FONT_T tFont;

    tFont.FontCode = FC_ST_16; /* 字体代码 16点阵 */
    tFont.FrontColor = CL_RED; /* 字体颜色 */
    tFont.BackColor = CL_MASK; /* 文字背景颜色 */
    tFont.Space = 0;                     /* 文字间距，单位 = 像素 */

    /* 绘制主窗体 */
    tWin.Font = &tFont;

    tWin.Left = WIN_INVALID_X;
    tWin.Top = WIN_INVALID_Y;
    tWin.Height = WIN_INVALID_H;
    tWin.Width = WIN_INVALID_W;
    tWin.pCaption = "Warning";
    LCD_DrawWin(&tWin);

    tLable.Font = &tFont;
    tLable.Left = LABLE_INVALID_X;
    tLable.Top = LABLE_INVALID_Y;
    tLable.MaxLen = 0;
    tLable.pCaption = "输入无效!";
    LCD_DrawLabel(&tLable);

    bsp_DelayMS(2000);
}

/*
*********************************************************************************************************
*    函 数 名: ClearWinNumPad
*    功能说明：清除数字键盘窗口（用于主界面重绘)
*    形    参：_usColor : 底色
*    返 回 值：无
*********************************************************************************************************
*/
void ClearWinNumPad(uint16_t _usColor)
{
    LCD_Fill_Rect(WIN_NUM_X, WIN_NUM_Y, WIN_NUM_H, WIN_NUM_W, _usColor);
}

/*
*********************************************************************************************************
*    函 数 名: InitFormPad
*    功能说明: 初始化控件属性, 坐标、字体
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
static void InitFormPad(void)
{
    /* 字体1  */
    FormPad->FontBlack.FontCode = FC_ST_16;
    FormPad->FontBlack.BackColor = CL_MASK; /* 透明色 */
    FormPad->FontBlack.FrontColor = CL_BLACK;
    FormPad->FontBlack.Space = 0;

    /* 字体 蓝色 */
    FormPad->FontBlue.FontCode = FC_ST_16;
    FormPad->FontBlue.BackColor = CL_BTN_FACE;
    FormPad->FontBlue.FrontColor = CL_BLUE;
    FormPad->FontBlue.Space = 0;

    /* 按钮字体 */
    FormPad->FontBtn.FontCode = FC_ST_16;
    FormPad->FontBtn.BackColor = CL_MASK; /* 透明背景 */
    FormPad->FontBtn.FrontColor = CL_BLACK;
    FormPad->FontBtn.Space = 0;

    FormPad->FontBtnNum.FontCode = FC_ST_16;
    FormPad->FontBtnNum.BackColor = CL_MASK; /* 透明背景 */
    FormPad->FontBtnNum.FrontColor = CL_BLACK;
    FormPad->FontBtnNum.Space = 0;

    /* WIN字体 */
    FormPad->FontWin.FontCode = FC_ST_16;
    FormPad->FontWin.BackColor = CL_MASK; /* 透明色 */
    FormPad->FontWin.FrontColor = CL_WHITE;
    FormPad->FontWin.Space = 0;

    /* Edit字体 */
    FormPad->FontEdit.FontCode = FC_ST_16;
    FormPad->FontEdit.BackColor = CL_MASK; /* 透明色 */
    FormPad->FontEdit.FrontColor = CL_RED;
    FormPad->FontEdit.Space = 0;

    /* win */
    FormPad->Win.Font = &FormPad->FontWin;
    ;
    FormPad->Win.Left = WIN_NUM_X;
    FormPad->Win.Top = WIN_NUM_Y;
    FormPad->Win.Height = WIN_NUM_H;
    FormPad->Win.Width = WIN_NUM_W;
    FormPad->Win.pCaption = FormPad->pTitle; /* 形参，全局的 */

    /* Edit */
    FormPad->Edit.Font = &FormPad->FontEdit;
    FormPad->Edit.Left = EDIT_NUM_X;
    FormPad->Edit.Top = EDIT_NUM_Y;
    FormPad->Edit.Height = EDIT_NUM_H;
    FormPad->Edit.Width = EDIT_NUM_W;
    FormPad->Edit.pCaption = FormPad->pEditText;

    /* 按钮 */
    // LCD_InitButton(BUTTON_T *_btn, uint16_t _x, uint16_t _y, uint16_t _h, uint16_t _w, char *_pCaption, FONT_T *_pFont);
    LCD_InitButton(&FormPad->Btn0, BTN0_X, BTN0_Y, BTN_H, BTN_W, "0", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn1, BTN1_X, BTN1_Y, BTN_H, BTN_W, "1", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn2, BTN2_X, BTN2_Y, BTN_H, BTN_W, "2", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn3, BTN3_X, BTN3_Y, BTN_H, BTN_W, "3", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn4, BTN4_X, BTN4_Y, BTN_H, BTN_W, "4", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn5, BTN5_X, BTN5_Y, BTN_H, BTN_W, "5", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn6, BTN6_X, BTN6_Y, BTN_H, BTN_W, "6", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn7, BTN7_X, BTN7_Y, BTN_H, BTN_W, "7", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn8, BTN8_X, BTN8_Y, BTN_H, BTN_W, "8", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->Btn9, BTN9_X, BTN9_Y, BTN_H, BTN_W, "9", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->BtnA, BTNA_X, BTNA_Y, BTN_H, BTN_W, ".", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->BtnB, BTNB_X, BTNB_Y, BTN_H, BTN_W, "-", &FormPad->FontBtnNum);
    LCD_InitButton(&FormPad->BtnF1, BTNF1_X, BTNF1_Y, BTN_H, BTN_W, "退格", &FormPad->FontBtn);
    LCD_InitButton(&FormPad->BtnF2, BTNF2_X, BTNF2_Y, BTN_H, BTN_W, "清除", &FormPad->FontBtn);
    LCD_InitButton(&FormPad->BtnF3, BTNF3_X, BTNF3_Y, BTN_H, BTN_W, "返回", &FormPad->FontBtn);
    LCD_InitButton(&FormPad->BtnF4, BTNF4_X, BTNF4_Y, BTN_H, BTN_W, "确认", &FormPad->FontBtn);
}

/*
*********************************************************************************************************
*    函 数 名: SetBtnText
*    功能说明: 切换输入按钮的文字。 0-9 共12个按钮。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void SetBtnText(uint8_t _mode)
{
    if (_mode == PAD_DIGIT) /* 纯数字 */
    {
        FormPad->Btn0.pCaption = "0";
        FormPad->Btn1.pCaption = "1";
        FormPad->Btn2.pCaption = "2";
        FormPad->Btn3.pCaption = "3";
        FormPad->Btn4.pCaption = "4";
        FormPad->Btn5.pCaption = "5";
        FormPad->Btn6.pCaption = "6";
        FormPad->Btn7.pCaption = "7";
        FormPad->Btn8.pCaption = "8";
        FormPad->Btn9.pCaption = "9";
        FormPad->BtnA.pCaption = ".";
        FormPad->BtnB.pCaption = "-";
    }
    else if (_mode == PAD_ABC) /* 数字和字符混合 */
    {
        FormPad->Btn0.pCaption = "0 符号";
        FormPad->Btn1.pCaption = "1 数字";
        FormPad->Btn2.pCaption = "2 abc";
        FormPad->Btn3.pCaption = "3 def";
        FormPad->Btn4.pCaption = "4 ghi";
        FormPad->Btn5.pCaption = "5 jkl";
        FormPad->Btn6.pCaption = "6 mno";
        FormPad->Btn7.pCaption = "7 pqrs";
        FormPad->Btn8.pCaption = "8 tuv";
        FormPad->Btn9.pCaption = "9 wxyz";
        FormPad->BtnA.pCaption = ".";
        FormPad->BtnB.pCaption = "-";
    }
    else if (_mode >= PAD_ABC0 && _mode <= PAD_ABCB) /* 数字和字符混合 0 按下后的二级键盘 */
    {
        uint8_t index1;

        index1 = _mode - PAD_ABC0;

        FormPad->Btn0.pCaption = (char *)g_key_tab[index1 * 12];
        FormPad->Btn1.pCaption = (char *)g_key_tab[index1 * 12 + 1];
        FormPad->Btn2.pCaption = (char *)g_key_tab[index1 * 12 + 2];
        FormPad->Btn3.pCaption = (char *)g_key_tab[index1 * 12 + 3];
        FormPad->Btn4.pCaption = (char *)g_key_tab[index1 * 12 + 4];
        FormPad->Btn5.pCaption = (char *)g_key_tab[index1 * 12 + 5];
        FormPad->Btn6.pCaption = (char *)g_key_tab[index1 * 12 + 6];
        FormPad->Btn7.pCaption = (char *)g_key_tab[index1 * 12 + 7];
        FormPad->Btn8.pCaption = (char *)g_key_tab[index1 * 12 + 8];
        FormPad->Btn9.pCaption = (char *)g_key_tab[index1 * 12 + 9];
        FormPad->BtnA.pCaption = (char *)g_key_tab[index1 * 12 + 10];
        FormPad->BtnB.pCaption = (char *)g_key_tab[index1 * 12 + 11];

        /* 只显示数字部分 */
        LCD_DrawButton(&FormPad->Btn0);
        LCD_DrawButton(&FormPad->Btn1);
        LCD_DrawButton(&FormPad->Btn2);
        LCD_DrawButton(&FormPad->Btn3);
        LCD_DrawButton(&FormPad->Btn4);
        LCD_DrawButton(&FormPad->Btn5);
        LCD_DrawButton(&FormPad->Btn6);
        LCD_DrawButton(&FormPad->Btn5);
        LCD_DrawButton(&FormPad->Btn6);
        LCD_DrawButton(&FormPad->Btn7);
        LCD_DrawButton(&FormPad->Btn8);
        LCD_DrawButton(&FormPad->Btn9);
        LCD_DrawButton(&FormPad->BtnA);
        LCD_DrawButton(&FormPad->BtnB);
    }
}

/*
*********************************************************************************************************
*    函 数 名: GetCharPad
*    功能说明: 根据菜单ID获得具体的字符
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static uint8_t GetCharPad(uint8_t _key1, uint8_t _key2)
{
    uint8_t index1, index2;
    char *p;

    if (_key1 >= '0' && _key1 <= '9')
    {
        index1 = _key1 - '0';
    }
    else if (_key1 == 'A')
    {
        index1 = 10;
    }
    else if (_key1 == 'B')
    {
        index1 = 11;
    }
    else
    {
        index1 = 0;
    }

    if (_key2 >= '0' && _key2 <= '9')
    {
        index2 = _key2 - '0';
    }
    else if (_key2 == 'A')
    {
        index2 = 10;
    }
    else if (_key2 == 'B')
    {
        index2 = 11;
    }
    else
    {
        index2 = 0;
    }

    p = (char *)g_key_tab[index1 * 12 + index2];

    return p[0];
}

/*
*********************************************************************************************************
*    函 数 名: DispAllBtn
*    功能说明: 显示所有的按钮控件
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispAllBtn(void)
{
    /* 按钮 */
    LCD_DrawButton(&FormPad->Btn0);
    LCD_DrawButton(&FormPad->Btn1);
    LCD_DrawButton(&FormPad->Btn2);
    LCD_DrawButton(&FormPad->Btn3);
    LCD_DrawButton(&FormPad->Btn4);
    LCD_DrawButton(&FormPad->Btn5);
    LCD_DrawButton(&FormPad->Btn6);
    LCD_DrawButton(&FormPad->Btn5);
    LCD_DrawButton(&FormPad->Btn6);
    LCD_DrawButton(&FormPad->Btn7);
    LCD_DrawButton(&FormPad->Btn8);
    LCD_DrawButton(&FormPad->Btn9);
    LCD_DrawButton(&FormPad->BtnA);
    LCD_DrawButton(&FormPad->BtnB);
    LCD_DrawButton(&FormPad->BtnF1);
    LCD_DrawButton(&FormPad->BtnF2);
    LCD_DrawButton(&FormPad->BtnF3);
    LCD_DrawButton(&FormPad->BtnF4);
}

/*
*********************************************************************************************************
*    函 数 名: DispFormPad
*    功能说明: 显示所有的静态控件
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void DispFormPad(void)
{
    LCD_DrawWin(&FormPad->Win);
    DispAllBtn();
    LCD_DrawEdit(&FormPad->Edit);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
