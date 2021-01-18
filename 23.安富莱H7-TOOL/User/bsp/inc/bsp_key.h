/*
*********************************************************************************************************
*
*    模块名称 : 按键驱动模块
*    文件名称 : bsp_key.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_KEY_H
#define __BSP_KEY_H


/* 借用KEYI FIFO做消息缓冲区 */
#define MSG_PC_RESET        0x80        /* PC机控制RESET硬件口线 */
#define MSG_DETECT_RESET    0x81

/* 根据应用程序的功能重命名按键宏 */
#define KEY_DOWN_S          KEY_1_DOWN
#define KEY_UP_S            KEY_1_UP
#define KEY_LONG_DOWN_S     KEY_1_LONG_DOWN
#define KEY_LONG_UP_S       KEY_1_LONG_UP
#define KEY_AUTO_S          KEY_1_AUTO_UP
#define KEY_DB_S            KEY_1_DB_UP

#define KEY_DOWN_C 		    KEY_2_DOWN
#define KEY_UP_C 		    KEY_2_UP
#define KEY_LONG_DOWN_C     KEY_2_LONG_DOWN
#define KEY_LONG_UP_C 	    KEY_2_LONG_UP
#define KEY_AUTO_C          KEY_2_AUTO_UP
#define KEY_DB_C            KEY_2_DB_UP

/* 按键ID, 主要用于bsp_KeyState()函数的入口参数 */
typedef enum
{
    KID_S = 0,
    KID_C,
} KEY_ID_E;

/*
    按键滤波时间50ms, 单位10ms。
    只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
    即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件
*/
#define KEY_FILTER_TIME         3
#define KEY_LONG_TIME           60  /* 单位10ms， 持续1秒，认为长按事件 */

#define KEY_DB_CLICK_TIME       25  /* 双击检测时长，单位ms */

/*
    每个按键对应1个全局的结构体变量。
*/
typedef struct
{
    /* 下面是一个函数指针，指向判断按键手否按下的函数 */
    uint8_t (*IsKeyDownFunc)(void);     /* 按键按下的判断函数,1表示按下 */

    uint8_t Count;          /* 滤波器计数器 */
    uint16_t LongCount;     /* 长按计数器 */
    uint16_t LongTime;      /* 按键按下持续时间, 0表示不检测长按 */
    uint8_t State;          /* 按键当前状态（按下还是弹起） */
    uint8_t RepeatSpeed;    /* 连续按键周期 */
    uint8_t RepeatCount;    /* 连续按键计数器 */
    
    uint16_t DelayCount;    /* 延迟计数器，用于双击检测 */
    uint8_t ClickCount;     /* 单击次数 */
    int32_t LastTime;       /* 上次按键时刻 */
} KEY_T;

/*
    定义键值代码, 必须按如下次序定时每个键的按下、弹起和长按事件

    推荐使用enum, 不用#define，原因：
    (1) 便于新增键值,方便调整顺序，使代码看起来舒服点
    (2) 编译器可帮我们避免键值重复。
*/
#define KEY_MSG_STEP    6   /* 按键消息代码步长(每个键有几个事件代码，顺序排列） */
typedef enum
{
    KEY_NONE = 0,       /* 0 表示按键事件 */

    KEY_1_DOWN,         /* 1键按下 */
    KEY_1_UP,           /* 1键弹起单击 */
    KEY_1_LONG_DOWN,    /* 1键长按 */
    KEY_1_LONG_UP,      /* 1键长按后的弹起 */  
    KEY_1_AUTO_UP,      /* 1键长按后自动发码 */
    KEY_1_DB_UP,        /* 1键双击 */    

    KEY_2_DOWN,         /* 2键按下 */
    KEY_2_UP,           /* 2键弹起 */
    KEY_2_LONG_DOWN,    /* 2键长按 */
    KEY_2_LONG_UP,      /* 2键长按后的弹起 */  
    KEY_2_AUTO_UP,      /* 2键长按后自动发码 */
    KEY_2_DB_UP,        /* 2键双击 */    
        
    KEY_LCD_SLEEP,      /* 虚拟一次按键，禁止LCD休眠 */
} KEY_ENUM;

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE 10
typedef struct
{
    uint8_t Buf[KEY_FIFO_SIZE]; /* 键值缓冲区 */
    uint8_t Read;               /* 缓冲区读指针1 */
    uint8_t Write;              /* 缓冲区写指针 */
    uint8_t Read2;              /* 缓冲区读指针2 */
    uint8_t Read3;					/* 缓冲区读指针3 */
} KEY_FIFO_T;

/* 供外部调用的函数声明 */
void bsp_InitKey(void);
void bsp_KeyScan10ms(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKey3(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t _RepeatSpeed);
void bsp_ClearKey(void);

void bsp_LcdSleepEnable(uint8_t _mode);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
