/*
*********************************************************************************************************
*
*    模块名称 : STM32内部RTC模块
*    文件名称 : bsp_cpu_rtc.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2015-08-08 armfly  首版.安富莱电子原创
*
*    Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#define RTC_INIT_FLAG 0xA5A8

#define RTC_ASYNCH_PREDIV 0x7F
#define RTC_SYNCH_PREDIV 0x00FF

RTC_T g_tRTC;

/* 平年的每月天数表 */
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static void RTC_Config(void);

RTC_HandleTypeDef RtcHandle; /* 必须用全局变量 */

/*
*********************************************************************************************************
*    函 数 名: bsp_InitRTC
*    功能说明: 初始化CPU内部RTC
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitRTC(void)
{
    uint32_t back_reg;

    RtcHandle.Instance = RTC;

    /* 用于检测是否已经配置过RTC，如果配置过的话，会在配置结束时 
    设置RTC备份寄存器为0xA5A5 。如果检测RTC备份寄存器不是0xA5A5   那么表示没有配置过，需要配置RTC.   */
    back_reg = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR0);
    if (back_reg != RTC_INIT_FLAG)
    {
        RTC_Config(); /* RTC 配置 */

        RTC_WriteClock(2018, 9, 1, 0, 0, 0); /* 设置初始时间 */

        /* 配置备份寄存器，表示已经设置过RTC */
        HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR0, RTC_INIT_FLAG);
    }
    else
    {
        /* 检测上电复位标志是否设置 */
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != 0)
        {
            /* 发生上电复位 */
        }
        /* 检测引脚复位标志是否设置 */
        else if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != 0)
        {
            /* 发生引脚复位 */
        }

        RTC_Config(); /* RTC 配置 */
    }
}

/*
*********************************************************************************************************
*    函 数 名: RTC_Config
*    功能说明: 1. 选择不同的RTC时钟源LSI或者LSE。
*             2. 配置RTC时钟。
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
static void RTC_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /* To enable access on RTC registers */
    HAL_PWR_EnableBkUpAccess();

    /* Configure LSE/LSI as RTC clock source ###############################*/
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* Configures the External Low Speed oscillator (LSE) drive capability */
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_HIGH);

    /* Enable RTC Clock */
    __HAL_RCC_RTC_ENABLE();

    /* Configure RTC prescaler and RTC data registers */
    {

        RtcHandle.Instance = RTC;

        /* RTC configured as follows:
          - Hour Format    = Format 24
          - Asynch Prediv  = Value according to source clock
          - Synch Prediv   = Value according to source clock
          - OutPut         = Output Disable
          - OutPutPolarity = High Polarity
          - OutPutType     = Open Drain */
        RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
        RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
        RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
        RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
        RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
        RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
        if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
        {
            /* Initialization Error */
            Error_Handler(__FILE__, __LINE__);
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: RTC_WriteClock
*    功能说明: 设置RTC时钟
*    形    参：无
*    返 回 值: 1表示成功 0表示错误
*********************************************************************************************************
*/
uint8_t RTC_WriteClock(uint16_t _year, uint8_t _mon, uint8_t _day, uint8_t _hour, uint8_t _min, uint8_t _sec)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    RtcHandle.Instance = RTC;

    /* 设置年月日和星期 */
    date.Year = _year - 2000;
    date.Month = _mon;
    date.Date = _day;
    date.WeekDay = RTC_CalcWeek(_year, _mon, _day); /* 周5=5， 周日=7 */
    if (HAL_RTC_SetDate(&RtcHandle, &date, FORMAT_BIN) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 设置时分秒，以及显示格式 */
    time.Hours = _hour;
    time.Minutes = _min;
    time.Seconds = _sec;
    time.TimeFormat = RTC_HOURFORMAT12_AM;
    time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    time.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&RtcHandle, &time, FORMAT_BIN) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: RTC_ReadClock
*    功能说明: 得到当前时钟。结果存放在 g_tRTC。
*    形    参：无
*    返 回 值: 无
*********************************************************************************************************
*/
void RTC_ReadClock(void)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    RtcHandle.Instance = RTC;

    /* CPU BUG: 必须先读取时间，再读取日期 */
    if (HAL_RTC_GetTime(&RtcHandle, &time, FORMAT_BIN) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    if (HAL_RTC_GetDate(&RtcHandle, &date, FORMAT_BIN) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    g_tRTC.Year = date.Year + 2000;
    g_tRTC.Mon = date.Month;
    g_tRTC.Day = date.Date;

    g_tRTC.Hour = time.Hours;    /* 小时 */
    g_tRTC.Min = time.Minutes; /* 分钟 */
    g_tRTC.Sec = time.Seconds; /* 秒 */

    g_tRTC.Week = RTC_CalcWeek(g_tRTC.Year, g_tRTC.Mon, g_tRTC.Day); /* 计算星期 */
}

/*
*********************************************************************************************************
*    函 数 名: bsp_CalcWeek
*    功能说明: 根据日期计算星期几
*    形    参: _year _mon _day  年月日  (年是2字节整数，月和日是字节整数）
*    返 回 值: 周几 （1-7） 7表示周日
*********************************************************************************************************
*/
uint8_t RTC_CalcWeek(uint16_t _year, uint8_t _mon, uint8_t _day)
{
    /*
    蔡勒（Zeller）公式
        历史上的某一天是星期几？未来的某一天是星期几？关于这个问题，有很多计算公式（两个通用计算公式和
    一些分段计算公式），其中最著名的是蔡勒（Zeller）公式。
        即w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

        公式中的符号含义如下，
         w：星期；
         c：年的高2位，即世纪-1
         y：年（两位数）；
         m：月（m大于等于3，小于等于14，即在蔡勒公式中，某年的1、2月要看作上一年的13、14月来计算，
              比如2003年1月1日要看作2002年的13月1日来计算）；
         d：日；
         [ ]代表取整，即只要整数部分。

        算出来的W除以7，余数是几就是星期几。如果余数是0，则为星期日。
        如果结果是负数，负数求余数则需要特殊处理：
            负数不能按习惯的余数的概念求余数，只能按数论中的余数的定义求余。为了方便
        计算，我们可以给它加上一个7的整数倍，使它变为一个正数，然后再求余数

        以2049年10月1日（100周年国庆）为例，用蔡勒（Zeller）公式进行计算，过程如下：
        蔡勒（Zeller）公式：w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
        =49+[49/4]+[20/4]-2×20+[26× (10+1)/10]+1-1
        =49+[12.25]+5-40+[28.6]
        =49+12+5-40+28
        =54 (除以7余5)
        即2049年10月1日（100周年国庆）是星期5。
    */
    uint8_t y, c, m, d;
    int16_t w;

    if (_mon >= 3)
    {
        m = _mon;
        y = _year % 100;
        c = _year / 100;
        d = _day;
    }
    else /* 某年的1、2月要看作上一年的13、14月来计算 */
    {
        m = _mon + 12;
        y = (_year - 1) % 100;
        c = (_year - 1) / 100;
        d = _day;
    }

    w = y + y / 4 + c / 4 - 2 * c + ((uint16_t)26 * (m + 1)) / 10 + d - 1;
    if (w == 0)
    {
        w = 7; /* 表示周日 */
    }
    else if (w < 0) /* 如果w是负数，则计算余数方式不同 */
    {
        w = 7 - (-w) % 7;
    }
    else
    {
        w = w % 7;
    }

    /* 2018-10-20 else的情况里面，会有情况把w计算成0的 */
    if (w == 0)
    {
        w = 7; /* 表示周日 */
    }

    return w;
}

/*
*********************************************************************************************************
*    函 数 名: RTC_ReadBkup32
*    功能说明: 读时钟备份寄存器
*    形    参: _addr ： 0-31
*    返 回 值: 数据
*********************************************************************************************************
*/
uint32_t RTC_ReadBkup32(uint32_t _addr)
{    
    return  HAL_RTCEx_BKUPRead(&RtcHandle, _addr);
}

/*
*********************************************************************************************************
*    函 数 名: RTC_WriteBkup32
*    功能说明: 写时钟备份寄存器
*    形    参: _addr ： 0-31
*    返 回 值: 数据
*********************************************************************************************************
*/
void RTC_WriteBkup32(uint32_t _addr, uint32_t _value)
{    
    HAL_RTCEx_BKUPWrite(&RtcHandle, _addr, _value);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
