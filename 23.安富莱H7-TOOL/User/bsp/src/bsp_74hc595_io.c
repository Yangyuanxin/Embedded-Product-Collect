/*
*********************************************************************************************************
*
*    模块名称 : 示波器模块74HC595扩展输出口驱动模块
*    文件名称 : bsp_74hc595_io.c
*    版    本 : V1.0
*    说    明 : 使用74HC595扩展8个输出口
*    修改记录 :
*        版本号  日期       作者    说明
*        v1.0    2019-08-13 armfly  
*
*    Copyright (C), 2016-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

static uint8_t s_HC595;

/*
    PI4/595_LOCK
    PI9/595_SCLK
    PG5/595_SDI
    
*/
#define ALL_HC595_GPIO_CLK_ENABLE() \
    __HAL_RCC_GPIOI_CLK_ENABLE();     \
    __HAL_RCC_GPIOG_CLK_ENABLE();

#define HC595_LCK_GPIO      GPIOI
#define HC595_LCK_PIN       GPIO_PIN_4
#define HC595_LCK_0()       BSP_SET_GPIO_0(HC595_LCK_GPIO, HC595_LCK_PIN)
#define HC595_LCK_1()       BSP_SET_GPIO_1(HC595_LCK_GPIO, HC595_LCK_PIN)

#define HC595_SCLK_GPIO     GPIOI
#define HC595_SCLK_PIN      GPIO_PIN_9
#define HC595_SCLK_0()      BSP_SET_GPIO_0(HC595_SCLK_GPIO, HC595_SCLK_PIN)
#define HC595_SCLK_1()      BSP_SET_GPIO_1(HC595_SCLK_GPIO, HC595_SCLK_PIN)

#define HC595_SDI_GPIO      GPIOG
#define HC595_SDI_PIN       GPIO_PIN_5
#define HC595_SDI_0()       BSP_SET_GPIO_0(HC595_SDI_GPIO, HC595_SDI_PIN)
#define HC595_SDI_1()       BSP_SET_GPIO_1(HC595_SDI_GPIO, HC595_SDI_PIN)

static void HC595_Delay(void);

/*
*********************************************************************************************************
*    函 数 名: HC595_InitHard
*    功能说明: 初始化硬件
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void HC595_InitHard(void)
{
    GPIO_InitTypeDef gpio_init;

    /* 打开GPIO时钟 */
    ALL_HC595_GPIO_CLK_ENABLE();

    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        /* 设置推挽输出 */
    gpio_init.Pull = GPIO_NOPULL;                        /* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度等级 */

    gpio_init.Pin = HC595_LCK_PIN;
    HAL_GPIO_Init(HC595_LCK_GPIO, &gpio_init);

    gpio_init.Pin = HC595_SCLK_PIN;
    HAL_GPIO_Init(HC595_SCLK_GPIO, &gpio_init);

    gpio_init.Pin = HC595_SDI_PIN;
    HAL_GPIO_Init(HC595_SDI_GPIO, &gpio_init);

    s_HC595 = 0;
    HC595_LockData();
}

/*
*********************************************************************************************************
*    函 数 名: HC595_Delay
*    功能说明: 延迟
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void HC595_Delay(void)
{
    uint8_t i;

    for (i = 0; i < 5; i++)
        ;
}

/*
*********************************************************************************************************
*    函 数 名: STM8_WriteGPIO
*    功能说明: 设置GPIO输出状态
*    形    参: _pin : 0-7
*              _value : 0,1
*    返 回 值: 无
*********************************************************************************************************
*/
void HC595_WriteGPIO(uint8_t _pin, uint8_t _value)
{
    if (_value == 1)
    {
        s_HC595 |= (1 << _pin);
    }
    else
    {
        s_HC595 &= ~(1 << _pin);
    }

    //HC595_LockData();
}

/*
*********************************************************************************************************
*    函 数 名: HC595_LockData
*    功能说明: 锁存数据
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void HC595_LockData(void)
{
    uint8_t i;
    uint8_t dd;

    dd = s_HC595;
    for (i = 0; i < 8; i++)
    {
        if (dd & 0x80)
        {
            HC595_SDI_1();
        }
        else
        {
            HC595_SDI_0();
        }

        HC595_Delay();
        HC595_SCLK_0();
        ;
        HC595_Delay();
        HC595_SCLK_1();
        dd <<= 1;
    }

    HC595_Delay();
    HC595_LCK_1();
    ;
    HC595_Delay();
    HC595_Delay();
    HC595_Delay();
    HC595_LCK_0();
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
