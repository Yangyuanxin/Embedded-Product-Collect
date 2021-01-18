/*
*********************************************************************************************************
*
*    模块名称 : 独立按键驱动模块 (外部输入IO)
*    文件名称 : bsp_key.c
*    版    本 : V2.0
*    说    明 : 扫描独立按键，具有软件滤波机制，具有按键FIFO。可以检测如下事件：
*                (1) 按键按下
*                (2) 按键弹起
*                (3) 长按键按下， 长按键弹起
*                (4) 长按时自动连发
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2013-02-01 armfly  正式发布
*        V1.1    2013-06-29 armfly  增加1个读指针，用于bsp_Idle() 函数读取系统控制组合键（截屏）
*                                   增加 K1 K2 组合键 和 K2 K3 组合键，用于系统控制
*        V1.2    2016-01-25 armfly  针对P02工控板更改. 调整gpio定义方式，更加简洁
*        V1.3    2018-11-26 armfly  s_tBtn结构赋初值0
*        V2.0    2019-12-07 armfly  增加长按弹起事件和自动连发事件，增加双击事件。
*                                   增加按键超时熄屏功能。
*
*    Copyright (C), 2016-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"

/*
    一次双击动作触发如下事件
    按下    
    弹起 -- 丢弃
    按下
    双击弹起
    弹起   
*/

#define DOUBLE_CLICK_ENABLE     0   /* 1表示启用双击检测， 会影响单击体验变慢 */

#define HARD_KEY_NUM            2                       /* 实体按键个数 */
#define KEY_COUNT               (HARD_KEY_NUM + 0)      /* 2个独立建 + 0个组合按键 */

/* 使能GPIO时钟 */
#define ALL_KEY_GPIO_CLK_ENABLE() \
    {                               \
        __HAL_RCC_GPIOF_CLK_ENABLE(); \
        __HAL_RCC_GPIOG_CLK_ENABLE(); \
    };

/* 依次定义GPIO */
typedef struct
{
    GPIO_TypeDef *gpio;
    uint16_t pin;
    uint8_t ActiveLevel; /* 激活电平 */
} X_GPIO_T;

/* GPIO和PIN定义 */
static const X_GPIO_T s_gpio_list[HARD_KEY_NUM] = {
        {GPIOF, GPIO_PIN_2, 0}, /* S键 */
        {GPIOG, GPIO_PIN_0, 0}, /* C键 */
};

/* 定义一个宏函数简化后续代码 
    判断GPIO引脚是否有效按下
*/
static KEY_T s_tBtn[KEY_COUNT] = {0};
static KEY_FIFO_T s_tKey; /* 按键FIFO变量,结构体 */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);

/* 用于按键超时进入屏保 */
static int32_t s_KeyTimeOutCount = 0;
static uint8_t s_LcdOn = 1;

/*
*********************************************************************************************************
*    函 数 名: KeyPinActive
*    功能说明: 判断按键是否按下
*    形    参: 无
*    返 回 值: 返回值1 表示按下(导通），0表示未按下（释放）
*********************************************************************************************************
*/
static uint8_t KeyPinActive(uint8_t _id)
{
    uint8_t level;
    
    if ((s_gpio_list[_id].gpio->IDR & s_gpio_list[_id].pin) == 0)
    {
        level = 0;
    }
    else
    {
        level = 1;
    }

    if (level == s_gpio_list[_id].ActiveLevel)
    {
        level = 1;
    }
    else
    {
        level = 0;
    }
    
    /* V1.45 2020-12-25 增加PC机模拟按键功能 */
    if (g_tVar.PC_KeyEnabled == 1)
    {
        g_tVar.PC_CKeyState = 0;
        if (_id == KID_S)
        {
            level = level | g_tVar.PC_SKeyState;
            
            if (level == 0)
            {
                g_tVar.PC_KeyEnabled = 0;
            }
        }
    }
    else if (g_tVar.PC_KeyEnabled == 2)
    {
        g_tVar.PC_SKeyState = 0;
        if (_id == KID_C)
        {
            level = level | g_tVar.PC_CKeyState;
            if (level == 0)
            {
                g_tVar.PC_KeyEnabled = 0;
            }            
        }
    }
    
    return level;
}

/*
*********************************************************************************************************
*    函 数 名: IsKeyDownFunc
*    功能说明: 判断按键是否按下。单键和组合键区分。单键事件不允许有其他键按下。
*    形    参: 无
*    返 回 值: 返回值1 表示按下(导通），0表示未按下（释放）
*********************************************************************************************************
*/
static uint8_t IsKeyDownFunc(uint8_t _id)
{
    
    /* 实体单键 */
    if (_id < HARD_KEY_NUM)
    {
        uint8_t i;
        uint8_t count = 0;
        uint8_t save = 255;

        /* 判断有几个键按下 */
        for (i = 0; i < HARD_KEY_NUM; i++)
        {
            if (KeyPinActive(i))
            {
                count++;
                save = i;
            }
        }

        if (count == 1 && save == _id)
        {
            return 1; /* 只有1个键按下时才有效 */
        }

        return 0;
    }

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitKey
*    功能说明: 初始化按键. 该函数被 bsp_Init() 调用。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
    bsp_InitKeyVar();   /* 初始化按键变量 */
    bsp_InitKeyHard();  /* 初始化按键硬件 */
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitKeyHard
*    功能说明: 配置按键对应的GPIO
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyHard(void)
{
    GPIO_InitTypeDef gpio_init;
    uint8_t i;

    /* 第1步：打开GPIO时钟 */
    ALL_KEY_GPIO_CLK_ENABLE();

    /* 第2步：配置所有的按键GPIO为浮动输入模式(实际上CPU复位后就是输入状态) */
    gpio_init.Mode = GPIO_MODE_INPUT;               /* 设置输入 */
    gpio_init.Pull = GPIO_PULLUP;                   /* 使能上拉电阻 */
    gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;    /* GPIO速度等级 */

    for (i = 0; i < HARD_KEY_NUM; i++)
    {
        gpio_init.Pin = s_gpio_list[i].pin;
        HAL_GPIO_Init(s_gpio_list[i].gpio, &gpio_init);
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitKeyVar
*    功能说明: 初始化按键变量
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
extern uint16_t GetSleepTimeMinute(void);
static void bsp_InitKeyVar(void)
{
    uint8_t i;

    /* 对按键FIFO读写指针清零 */
    s_tKey.Read = 0;
    s_tKey.Write = 0;
    s_tKey.Read2 = 0;

    /* 给每个按键结构体成员变量赋一组缺省值 */
    for (i = 0; i < KEY_COUNT; i++)
    {
        s_tBtn[i].LongTime = KEY_LONG_TIME;         /* 长按时间 0 表示不检测长按键事件 */
        s_tBtn[i].Count = KEY_FILTER_TIME / 2;      /* 计数器设置为滤波时间的一半 */
        s_tBtn[i].State = 0;                        /* 按键缺省状态，0为未按下 */
        s_tBtn[i].RepeatSpeed = 0;                  /* 按键连发的速度，0表示不支持连发 */
        s_tBtn[i].RepeatCount = 0;                  /* 连发计数器 */
        s_tBtn[i].DelayCount = 0;
        s_tBtn[i].ClickCount = 0;  
        s_tBtn[i].LastTime = 0;
    }

    /* 如果需要单独更改某个按键的参数，可以在此单独重新赋值 */

    /* 摇杆上下左右，支持长按1秒后，自动连发 */
    bsp_SetKeyParam(KID_S, KEY_LONG_TIME, 0);
    bsp_SetKeyParam(KID_C, KEY_LONG_TIME, 0);
    
    s_KeyTimeOutCount = GetSleepTimeMinute() * 60 * 100u;  /* 10ms单位 */
}

/*
*********************************************************************************************************
*    函 数 名: bsp_PutKey
*    功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*    形    参:  _KeyCode : 按键代码
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
    s_KeyTimeOutCount = GetSleepTimeMinute() * 60 * 100u;  /* 10ms单位 */
    
    /* 屏幕熄灭阶段，丢弃唤醒键 */
    if (s_LcdOn == 0)
    {        
        if (_KeyCode == KEY_1_UP || _KeyCode == KEY_1_LONG_UP 
            || _KeyCode == KEY_2_UP || _KeyCode == KEY_2_LONG_UP)
        {
            s_LcdOn = 1;            
            g_LcdSleepReq = 2;    /* 控制LCD唤醒, 在bsp_Idle执行. 不可以在此调用 LCD_DispOff() */
            LCD_SetBackLight(BRIGHT_DEFAULT);   /* 打开背光 */                
        }                
        return;
    }
    
    /* 启动后 100ms内检测到按键按下全部忽略 - 例如从 DAP返回，C键一直按着. 需要忽略这个按键事件 */
    {
        static uint8_t s_JumpFlag = 0;
        
        if (s_JumpFlag == 0)
        {
            if (bsp_CheckRunTime(0) < 100)
            {
                s_JumpFlag = 1;
                return;
            }
            else
            {
                s_JumpFlag = 2;
            }
        }
        else if (s_JumpFlag == 1)
        {
            if (_KeyCode == KEY_1_UP || _KeyCode == KEY_1_LONG_UP 
                || _KeyCode == KEY_2_UP || _KeyCode == KEY_2_LONG_UP)
            {
                s_JumpFlag = 2;
                return;
            }
        }
    }
    
    s_tKey.Buf[s_tKey.Write] = _KeyCode;

    if (++s_tKey.Write >= KEY_FIFO_SIZE)
    {
        s_tKey.Write = 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_GetKey
*    功能说明: 从按键FIFO缓冲区读取一个键值。
*    形    参: 无
*    返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
    uint8_t ret;

    if (s_tKey.Read == s_tKey.Write)
    {
        return KEY_NONE;
    }
    else
    {
        ret = s_tKey.Buf[s_tKey.Read];

        if (++s_tKey.Read >= KEY_FIFO_SIZE)
        {
            s_tKey.Read = 0;
        }
        return ret;
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_GetKey2
*    功能说明: 从按键FIFO缓冲区读取一个键值。独立的读指针。
*    形    参:  无
*    返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey2(void)
{
    uint8_t ret;

    if (s_tKey.Read2 == s_tKey.Write)
    {
        return KEY_NONE;
    }
    else
    {
        ret = s_tKey.Buf[s_tKey.Read2];

        if (++s_tKey.Read2 >= KEY_FIFO_SIZE)
        {
            s_tKey.Read2 = 0;
        }
        return ret;
    }
}


/*
*********************************************************************************************************
*    函 数 名: bsp_GetKey3
*    功能说明: 从按键FIFO缓冲区读取一个键值。独立的读指针。
*    形    参:  无
*    返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey3(void)
{
    uint8_t ret;

    if (s_tKey.Read3 == s_tKey.Write)
    {
        return KEY_NONE;
    }
    else
    {
        ret = s_tKey.Buf[s_tKey.Read3];

        if (++s_tKey.Read3 >= KEY_FIFO_SIZE)
        {
            s_tKey.Read3 = 0;
        }
        return ret;
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_GetKeyState
*    功能说明: 读取按键的状态
*    形    参:  _ucKeyID : 按键ID，从0开始
*    返 回 值: 0 表示未按下 1 表示按下， 2 表示长按持续状态
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
    return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SetKeyParam
*    功能说明: 设置按键参数
*    形    参：_ucKeyID : 按键ID，从0开始
*            _LongTime : 长按事件时间  10ms单位
*             _RepeatSpeed : 连发速度(间隔时间)  10ms单位
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t _RepeatSpeed)
{
    s_tBtn[_ucKeyID].LongTime = _LongTime;              /* 长按时间 0 表示不检测长按键事件 */
    s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;        /* 按键连发的速度，0表示不支持连发 */
    s_tBtn[_ucKeyID].RepeatCount = 0;                   /* 连发计数器 */
}

/*
*********************************************************************************************************
*    函 数 名: bsp_ClearKey
*    功能说明: 清空按键FIFO缓冲区
*    形    参：无
*    返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
    s_tKey.Write = 0;
    s_tKey.Read = 0;
    s_tKey.Read2 = 0;
    s_tKey.Read3 = 0;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_DetectKey
*    功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*    形    参: IO的id， 从0开始编码
*    返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
    KEY_T *pBtn;

    pBtn = &s_tBtn[i];
    if (IsKeyDownFunc(i))
    {
        if (pBtn->Count < KEY_FILTER_TIME)
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if (pBtn->Count < 2 * KEY_FILTER_TIME)
        {
            pBtn->Count++;
        }
        else
        {
            if (pBtn->State == 0)
            {
                pBtn->State = 1;

                /* 发送按钮按下的消息 */
                bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_DOWN));
            }

            if (pBtn->LongTime > 0)
            {
                if (pBtn->LongCount < pBtn->LongTime)
                {
                    /* 发送长按消息 */
                    if (++pBtn->LongCount == pBtn->LongTime)
                    {
                        pBtn->State = 2;
                        
                        /* 键值放入按键FIFO */
                        bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_LONG_DOWN));                        
                    }
                }
                else
                {
                    if (pBtn->RepeatSpeed > 0)
                    {
                        if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
                        {
                            pBtn->RepeatCount = 0;
                            /* 常按键后，每隔10ms发送1个按键弹起事件 */
                            //bsp_PutKey((uint8_t)(4 * i + 1));  这是发按键按下事件
                            bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_AUTO_UP));
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (pBtn->Count > KEY_FILTER_TIME)
        {
            pBtn->Count = KEY_FILTER_TIME;
        }
        else if (pBtn->Count != 0)
        {
            pBtn->Count--;
        }
        else
        {
            if (pBtn->State != 0)
            {
                /* 2019-12-05 H7-TOOL增加，第4个事件, 长按后的弹起 */
                if (pBtn->LongTime == 0)
                {
                    /* 发送短按弹起的消息 */
                    bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_UP));
                }
                else
                {
                    if (pBtn->State == 2)
                    {
                        /* 发送长按弹起的消息 */
                        bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_LONG_UP));
                        
                        #if DOUBLE_CLICK_ENABLE == 1
                            s_tBtn[i].LastTime = bsp_GetRunTime();  /* 记录按键弹起时刻 */
                        #endif
                    }
                    else
                    {                       
                        #if DOUBLE_CLICK_ENABLE == 1
                        /* 发送短按弹起的消息 */
                            //if (bsp_CheckRunTime(s_tBtn[i].LastTime) < 500)
                            if (pBtn->DelayCount > 0)
                            {                            
                                if (pBtn->ClickCount == 1)
                                {
                                    bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_DB_UP));  /* 双击事件 */
                                    pBtn->ClickCount = 0;
                                }
                                else
                                {
                                    bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_UP));     /* 单击弹起事件 */
                                    pBtn->ClickCount = 0;
                                }
                                pBtn->DelayCount = 80; 
                            }
                            else                       
                            {
                                pBtn->ClickCount++;                        
                                pBtn->DelayCount = KEY_DB_CLICK_TIME;  
                            }
                            s_tBtn[i].LastTime = bsp_GetRunTime();  /* 记录按键弹起时刻 */   
                        #else
                            bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_UP));     /* 单击弹起事件 */
                        #endif
                    }
                }
                pBtn->State = 0;                
            }
        }

        pBtn->LongCount = 0;
        pBtn->RepeatCount = 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_KeyScan10ms
*    功能说明: 扫描所有按键。非阻塞，被systick中断周期性的调用，10ms一次
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_KeyScan10ms(void)
{
    uint8_t i;

    for (i = 0; i < KEY_COUNT; i++)
    {   
        #if DOUBLE_CLICK_ENABLE == 1
            /* 超时判断 */
            if (s_tBtn[i].DelayCount > 0)
            {
                if (--s_tBtn[i].DelayCount == 0)
                {
                    if (s_tBtn[i].ClickCount == 1)
                    {
                        bsp_PutKey((uint8_t)(KEY_MSG_STEP * i + KEY_1_UP));   /* 单击弹起 */
                    }              
                    s_tBtn[i].ClickCount = 0;
                }
            }
        #endif
            
        /* 检测按键 */
        bsp_DetectKey(i);        
    }
    
    if (s_KeyTimeOutCount > 0)
    {
        if (--s_KeyTimeOutCount == 0)
        {
            LCD_SetBackLight(0);        /* 关闭背光 */                
            g_LcdSleepReq = 1;          /* 控制LCD休眠, 在bsp_Idle执行. 不可以在此调用 LCD_DispOff() */
            s_LcdOn = 0;                /* 屏幕关闭 */
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_LcdSleepEnable
*    功能说明: 背光关闭功能使能控制。应用: 脱机烧录如果超过1分钟，中途会关闭背光。
*    形    参: _mode : 0表示临时屏蔽背光控制  1表示恢复背光控制
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_LcdSleepEnable(uint8_t _mode)
{
    if (_mode == 0)
    {
        s_KeyTimeOutCount = 0;
    }
    else if (_mode == 1)
    {
        s_KeyTimeOutCount = GetSleepTimeMinute() * 60 * 100u;  /* 10ms单位 */
    }    
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
