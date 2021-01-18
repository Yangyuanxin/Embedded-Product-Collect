/*
*********************************************************************************************************
*
*    模块名称 : BSP模块(For STM32H7)
*    文件名称 : bsp.h
*    版    本 : V1.0
*    说    明 : 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h" 来包含所有的外设驱动模块。
*               bsp = Borad surport packet 板级支持包
*    修改记录 :
*        版本号  日期         作者       说明
*        V1.0    2018-07-29  Eric2013   正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_H
#define _BSP_H

/* 定义 BSP 版本号 */
#define __STM32H7_BSP_VERSION "1.2"

#define  USE_RTX    0

/* CPU空闲时执行的函数 */
//#define CPU_IDLE()        bsp_Idle()

/* 开关全局中断的宏 */
#define ENABLE_INT()    __set_PRIMASK(0)    /* 使能全局中断 */
#define DISABLE_INT()   __set_PRIMASK(1)    /* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
//#define BSP_Printf printf
#define BSP_Printf(...)

#define ERROR_HANDLER()     Error_Handler(__FILE__, __LINE__)

#define BSP_SET_GPIO_1(gpio, pin)   gpio->BSRR = pin
#define BSP_SET_GPIO_0(gpio, pin)   gpio->BSRR = (uint32_t)(pin) << 16U

/* 默认是关闭状态 */
#define Enable_EventRecorder 0

#if Enable_EventRecorder == 1
#include "EventRecorder.h"
#endif

#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* 定义优先级分组 */
#define NVIC_PREEMPT_PRIORITY 4

/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
#include "bsp_msg.h"
#include "bsp_user_lib.h"
#include "bsp_timer.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_msg.h"

#include "bsp_cpu_adc.h"
#include "bsp_cpu_dac.h"
#include "bsp_cpu_flash.h"
#include "bsp_uart_fifo.h"
#include "bsp_cpu_rtc.h"
#include "bsp_esp32_at.h"

#include "bsp_spi_bus.h"
#include "bsp_qspi_w25q256.h"

#include "bsp_fmc_io.h"

#include "bsp_i2c_gpio.h"
#include "bsp_i2c_gpio_ext.h"
#include "bsp_i2c_eeprom_24xx.h"
#include "bsp_i2c_mcp4018.h"
#include "bsp_i2c_mcp4725.h"
#include "bsp_power_tvcc.h"

#include "bsp_tft_st7789.h"
#include "bsp_tft_lcd.h"

#include "bsp_beep.h"
#include "bsp_tim_pwm.h"
#include "bsp_fmc_io.h"

#include "bsp_period_ctrl.h"

#include "bsp_tim_dma.h"
#include "bsp_tim_capture.h"

#include "bsp_74hc595_io.h"

#include "bsp_emmc.h"
#include "bsp_ntc.h"
#include "bsp_ext_io.h"

#include "bsp_rng.h"
#include "bsp_ds18b20.h"



#define HARD_MODEL              0x0750
#define BOOT_VERSION            *(uint16_t *)(0x08000000 + 28)
#define APP_VERSION             *(uint16_t *)(0x08020000 + 28)

/* 提供给其他C文件调用的函数 */
void bsp_Init(void);
void bsp_DeInit(void);
void bsp_Idle(void);

void bsp_GetCpuID(uint32_t *_id);
void Error_Handler(char *file, uint32_t line);
void System_Init(void);
/* 用于调试测试时间 D2 和 D0 */
#define DEBUG_D2_TRIG()                     \
    if (s_D2State == 0)                     \
    {                                       \
        BSP_SET_GPIO_1(GPIOE, GPIO_PIN_6);  \
        s_D2State = 1;                      \
    }                                       \
    else if (s_D2State == 1)                \
    {                                       \
        BSP_SET_GPIO_0(GPIOE, GPIO_PIN_6);  \
        s_D2State = 0;                      \
    }                                       \
    else                                    \
    {                                       \
        EIO_D2_Config(ES_GPIO_OUT);         \
        BSP_SET_GPIO_1(GPIOE, GPIO_PIN_6);  \
        s_D2State = 1;                      \
    }                                       
extern uint8_t s_D2State;

#define DEBUG_D0_TRIG()                     \
    if (s_D0State == 0)                     \
    {                                       \
        BSP_SET_GPIO_1(GPIOI, GPIO_PIN_0);  \
        s_D0State = 1;                      \
    }                                       \
    else if (s_D0State == 1)                \
    {                                       \
        BSP_SET_GPIO_0(GPIOI, GPIO_PIN_0);  \
        s_D0State = 0;                      \
    }                                       \
    else                                    \
    {                                       \
        EIO_D0_Config(ES_GPIO_OUT);         \
        BSP_SET_GPIO_1(GPIOI, GPIO_PIN_0);  \
        s_D0State = 1;                      \
    }                                       
extern uint8_t s_D0State;
    
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
