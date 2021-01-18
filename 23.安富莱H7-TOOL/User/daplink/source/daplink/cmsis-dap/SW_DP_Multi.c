/*
    由 SW_DP_Multi.c 文件修改得到，支持1-4路并发操作
    
    
    编译器优化3， 烧写1MB文件，STM32L476RGT6
    单路  16.77   16.51   16.51s
    多路  17.89   17.62   17.62s
    
    编译器优化0， 烧写1MB文件，STM32L476RGT6
    单路  16.82   16.56   16.56s
    多路  18.79   18.52   18.52s
*/
/*
*********************************************************************************************************
*
*    模块名称 : SWD一拖四驱动程序
*    文件名称 : SW_DP_Multi.c
*    版    本 : V1.0
*    说    明 : SWD接口底层驱动函数
*    修改记录 :
*        版本号  日期       作者    说明
*        V1.0    2020-03-13 armfly 
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/


/**
 * @file    SW_DP_Multi.c
 * @brief   1拖4 SWD driver
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ----------------------------------------------------------------------
 *
 * $Date:        20. May 2015
 * $Revision:    V1.10
 *
 * Project:      CMSIS-DAP Source
 * Title:        SW_DP.c CMSIS-DAP SW DP I/O
 *
 *---------------------------------------------------------------------------*/

#include "DAP_config.h"
#include "DAP.h"

#include "SW_DP_Multi.h"

/*
    BSP_SET_GPIO_1(SWCLK_TCK_PIN_PORT, SWCLK_TCK_PIN);  

    D0  PD14 PA15 PI0   - DIR PH8       RESET 共用
    D1  PD15 PA8 PH19   - DIR PG8       外部触发编程
    
    D2  PE6  PD0 PB7     - DIR PD9       SWCLK_4
    D3  PE5  PD1 PH11    - DIR PG10      SWDIO_4
    D4  PE4  PE7 PH12    - DR PG12          SWDIO_3    
    D5  PE2  PE8 PI5     - DIR PG7       ISWDIO_2
    D6  PE9  PD3 PA0     - DIR PD10         SWCLK_3
    D7  PE10 PI6         - DIR PI1      SWCLK_2    
    D8  PE11 PD4  PI3    - DIR PG9       SWDIO_1     
    D9  PE12 PD5         - DIR PI12      SWCLK_1
*/

#define MUL_GPIO_SWD        GPIOE
#define MUL_GPIO_DIR        GPIOG

#define MUL_PIN_SWCLK_1     GPIO_PIN_12 
#define MUL_PIN_SWDIO_1     GPIO_PIN_11 
#define MUL_PIN_DIR_1       GPIO_PIN_9 

#define MUL_PIN_SWCLK_2     GPIO_PIN_10 
#define MUL_PIN_SWDIO_2     GPIO_PIN_2 
#define MUL_PIN_DIR_2       GPIO_PIN_7 

#define MUL_PIN_SWCLK_3     GPIO_PIN_9 
#define MUL_PIN_SWDIO_3     GPIO_PIN_4 
#define MUL_PIN_DIR_3       GPIO_PIN_12

#define MUL_PIN_SWCLK_4     GPIO_PIN_6 
#define MUL_PIN_SWDIO_4     GPIO_PIN_5 
#define MUL_PIN_DIR_4       GPIO_PIN_10

/* pin bit */
#define MUL_PINB_SWCLK_1    12 
#define MUL_PINB_SWDIO_1    11 
#define MUL_PINB_DIR_1      9 

#define MUL_PINB_SWCLK_2    10 
#define MUL_PINB_SWDIO_2    2 
#define MUL_PINB_DIR_2      7 

#define MUL_PINB_SWCLK_3    9 
#define MUL_PINB_SWDIO_3    4 
#define MUL_PINB_DIR_3      12

#define MUL_PINB_SWCLK_4    6 
#define MUL_PINB_SWDIO_4    5
#define MUL_PINB_DIR_4      10

/* GPIO_MODER寄存器 */
#define MUL_DIO_MODE_MASK_1   ~(3u << (MUL_PINB_SWDIO_1 * 2)) 
#define MUL_DIO_MODE_MASK_2   ~(3u << (MUL_PINB_SWDIO_2 * 2)) 
#define MUL_DIO_MODE_MASK_3   ~(3u << (MUL_PINB_SWDIO_3 * 2)) 
#define MUL_DIO_MODE_MASK_4   ~(3u << (MUL_PINB_SWDIO_4 * 2)) 

#define MUL_DIO_MODE_OUT_1   (1u << (MUL_PINB_SWDIO_1 * 2)) 
#define MUL_DIO_MODE_OUT_2   (1u << (MUL_PINB_SWDIO_2 * 2)) 
#define MUL_DIO_MODE_OUT_3   (1u << (MUL_PINB_SWDIO_3 * 2)) 
#define MUL_DIO_MODE_OUT_4   (1u << (MUL_PINB_SWDIO_4 * 2)) 

/* CLK_0_DIO_0 */
#define CLK_0_DIO_0_1    (((uint32_t)MUL_PIN_SWCLK_1 +  MUL_PIN_SWDIO_1) << 16)
#define CLK_0_DIO_0_2    (((uint32_t)MUL_PIN_SWCLK_2 +  MUL_PIN_SWDIO_2) << 16)
#define CLK_0_DIO_0_3    (((uint32_t)MUL_PIN_SWCLK_3 +  MUL_PIN_SWDIO_3) << 16)
#define CLK_0_DIO_0_4    (((uint32_t)MUL_PIN_SWCLK_4 +  MUL_PIN_SWDIO_4) << 16)

/* CLK_0_DIO_1 */
#define CLK_0_DIO_1_1    (((uint32_t)MUL_PIN_SWCLK_1 << 16) | MUL_PIN_SWDIO_1)
#define CLK_0_DIO_1_2    (((uint32_t)MUL_PIN_SWCLK_2 << 16) | MUL_PIN_SWDIO_2)
#define CLK_0_DIO_1_3    (((uint32_t)MUL_PIN_SWCLK_3 << 16) | MUL_PIN_SWDIO_3)
#define CLK_0_DIO_1_4    (((uint32_t)MUL_PIN_SWCLK_4 << 16) | MUL_PIN_SWDIO_4)

MUL_SWD_T g_gMulSwd = {0};

/* 4个SWDIO配置为输出 */
static __forceinline void MUL_PIN_SWDIO_OUT_ENABLE(void)
{    
    BSP_SET_GPIO_1(MUL_GPIO_DIR, g_gMulSwd.DIR_Pins);    
    
    MUL_GPIO_SWD->MODER = (MUL_GPIO_SWD->MODER & g_gMulSwd.MODER_Mask) | g_gMulSwd.MODER_Out;     /* 输出 */
    
    BSP_SET_GPIO_0(MUL_GPIO_SWD, g_gMulSwd.SWDIO_Pins);
}

/* 4个SWDIO配置为输入 */
static __forceinline void MUL_PIN_SWDIO_OUT_DISABLE(void)
{
    BSP_SET_GPIO_0(MUL_GPIO_DIR, g_gMulSwd.DIR_Pins);    
    
    MUL_GPIO_SWD->MODER = (MUL_GPIO_SWD->MODER & g_gMulSwd.MODER_Mask);  /* 输入 */
    
    BSP_SET_GPIO_0(MUL_GPIO_SWD, g_gMulSwd.SWDIO_Pins);
}

/* 读取4个SWDIO引脚状态 */
static __forceinline uint32_t MUL_PIN_SWDIO_IN(void)
{
    uint32_t input;
    uint32_t ret = 0;
    
    input = MUL_GPIO_SWD->IDR;
    
    if (input & MUL_PIN_SWDIO_1)
    {
        ret += 0x00000001;
    }
    
    if (input & MUL_PIN_SWDIO_2)
    {
        ret += 0x00000100;
    }

    if (input & MUL_PIN_SWDIO_3)
    {
        ret += 0x00010000;
    }

    if (input & MUL_PIN_SWDIO_4)
    {
        ret += 0x01000000;
    }  

    return ret;
}

/* 设置4个SWDIO引脚状态 */
static __forceinline void MUL_PIN_SWDIO_OUT(uint32_t bit)
{
    if (bit & 1)
    {
        BSP_SET_GPIO_1(MUL_GPIO_SWD, g_gMulSwd.SWDIO_Pins);
    }
    else
    {
        BSP_SET_GPIO_0(MUL_GPIO_SWD, g_gMulSwd.SWDIO_Pins);
    }
}

/* */
static __forceinline void MUL_PIN_SWCLK_SET(void)
{
    BSP_SET_GPIO_1(MUL_GPIO_SWD, g_gMulSwd.SWCLK_Pins);
}

/* 高速 */
static __forceinline void MUL_PIN_SWCLK_CLR(void)
{
    BSP_SET_GPIO_0(MUL_GPIO_SWD, g_gMulSwd.SWCLK_Pins);
}

/* SPI软件模式，低速配置 */
#define PIN_DELAY_S()               PIN_DELAY_SLOW(DAP_Data.clock_delay)
#define MUL_SW_CLOCK_CYCLE_SLOW()   MUL_PIN_SWCLK_CLR();  PIN_DELAY_S();  MUL_PIN_SWCLK_SET(); PIN_DELAY_S()
#define MUL_SW_WRITE_BIT_SLOW(bit)  MUL_PIN_SWDIO_OUT(bit); MUL_PIN_SWCLK_CLR(); PIN_DELAY_S(); MUL_PIN_SWCLK_SET(); PIN_DELAY_S()
#define MUL_SW_READ_BIT_SLOW(bit)   MUL_PIN_SWCLK_CLR();  PIN_DELAY_S(); bit = MUL_PIN_SWDIO_IN(); MUL_PIN_SWCLK_SET(); PIN_DELAY_S()

/* SPI软件模式，高速配置 */
#define MUL_SW_CLOCK_CYCLE_FAST()   MUL_PIN_SWCLK_CLR();  MUL_PIN_SWCLK_SET();
#define MUL_SW_WRITE_BIT_FAST(bit)  MUL_PIN_SWDIO_OUT(bit); MUL_PIN_SWCLK_CLR(); MUL_PIN_SWCLK_SET();

#define MUL_SW_READ_BIT_FAST(bit)   MUL_PIN_SWCLK_CLR();  bit = MUL_PIN_SWDIO_IN(); MUL_PIN_SWCLK_SET();
                    

/* 高速模式*/
#define MUL_SEND_32BIT_ONCE_FAST()  \
    if (val & 1) {      \
        MUL_GPIO_SWD->BSRR = CLK_0_DIO_1;   \
        val >>= 1;      \
        MUL_PIN_SWCLK_SET();    \
    }   \
    else {      \
        MUL_GPIO_SWD->BSRR = CLK_0_DIO_0; \
        val >>= 1;  \
        MUL_PIN_SWCLK_SET();    \
    }
static __forceinline void MUL_SEND_32BIT_FAST(uint32_t val)
{    
    uint32_t CLK_0_DIO_1 = g_gMulSwd.CLK_0_DIO_1;
    uint32_t CLK_0_DIO_0 = g_gMulSwd.CLK_0_DIO_0;
    
    MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();
    MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();
    MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();
    MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();
    MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();
    MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();
    MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();
    MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();MUL_SEND_32BIT_ONCE_FAST();
}

/* 低速模式 */
#define MUL_SEND_32BIT_ONCE_SLOW()  \
    if (val & 1) {      \
        MUL_GPIO_SWD->BSRR = CLK_0_DIO_1;   PIN_DELAY_S();\
        val >>= 1;      \
        MUL_PIN_SWCLK_SET();    PIN_DELAY_S();\
    }   \
    else {      \
        MUL_GPIO_SWD->BSRR = CLK_0_DIO_0;  PIN_DELAY_S();\
        val >>= 1;  \
        MUL_PIN_SWCLK_SET();    PIN_DELAY_S();\
    }
static __forceinline void MUL_SEND_32BIT_SLOW(uint32_t val)
{    
    uint32_t CLK_0_DIO_1 = g_gMulSwd.CLK_0_DIO_1;
    uint32_t CLK_0_DIO_0 = g_gMulSwd.CLK_0_DIO_0;
    
    MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();
    MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();
    MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();
    MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();
    MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();
    MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();
    MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();
    MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();MUL_SEND_32BIT_ONCE_SLOW();
}

extern uint8_t GetParity(uint32_t data);

/*
*********************************************************************************************************
*    函 数 名: MUL_SWD_GPIOConfig
*    功能说明: GPIO配置
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void MUL_SWD_GPIOConfig(void)
{
    EIO_D0_Config(ES_GPIO_OUT);         /* reset */
    EIO_D1_Config(ES_GPIO_IN);          /* 输入 */
  
    if (g_gMulSwd.Active[0] == 1)
    {
        EIO_D8_Config(ES_GPIO_SWD_OUT);     /* 用FMC口线做GPIO。因此FMC功能失效 */
        EIO_D9_Config(ES_GPIO_SWD_OUT);     /* 用FMC口线做GPIO。因此FMC功能失效 */
    }
    if (g_gMulSwd.Active[1] == 1)
    {
        EIO_D5_Config(ES_GPIO_OUT);
        EIO_D7_Config(ES_GPIO_SWD_OUT);     /* 用FMC口线做GPIO。因此FMC功能失效 */
    }
    if (g_gMulSwd.Active[2] == 1)
    {
        EIO_D4_Config(ES_GPIO_OUT);    
        EIO_D6_Config(ES_GPIO_SWD_OUT);     /* 用FMC口线做GPIO。因此FMC功能失效 */
    }
    if (g_gMulSwd.Active[3] == 1)
    {
        EIO_D2_Config(ES_GPIO_OUT);
        EIO_D3_Config(ES_GPIO_OUT);
    }    
}

/*
*********************************************************************************************************
*    函 数 名: MUL_PORT_SWD_SETUP
*    功能说明: MUL_swd_init()调用该函数。初始化4路SWD硬件GPIO，并设置初始状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void MUL_PORT_SWD_SETUP(void)
{
    //    Set SWCLK HIGH
    //    Set SWDIO HIGH
    //    Set RESET LOW  转接板有反相器三极管    
    MUL_SWD_GPIOConfig();
    MUL_RefreshGpioParam();
    
    MUL_PIN_SWDIO_OUT_ENABLE();
    MUL_PIN_SWDIO_OUT(1);
    MUL_PIN_SWCLK_SET();
    
    //EIO_SetOutLevel(0, 0);    /* D0输出0V, 转接板RESET输出高 */
}

/*
*********************************************************************************************************
*    函 数 名: MUL_RefreshGpioParam
*    功能说明: 根据通道状态刷新GPIO配置
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
const uint32_t TablePinDIR[4]   = {MUL_PIN_DIR_1, MUL_PIN_DIR_2, MUL_PIN_DIR_3, MUL_PIN_DIR_4};
const uint32_t TablePinSWClK[4] = {MUL_PIN_SWCLK_1, MUL_PIN_SWCLK_2, MUL_PIN_SWCLK_3, MUL_PIN_SWCLK_4};
const uint32_t TablePinSWDIO[4] = {MUL_PIN_SWDIO_1, MUL_PIN_SWDIO_2, MUL_PIN_SWDIO_3, MUL_PIN_SWDIO_4};

const uint32_t TableModeMask[4] = {MUL_DIO_MODE_MASK_1, MUL_DIO_MODE_MASK_2, MUL_DIO_MODE_MASK_3, MUL_DIO_MODE_MASK_4};
const uint32_t TableModeOut[4]  = {MUL_DIO_MODE_OUT_1, MUL_DIO_MODE_OUT_2, MUL_DIO_MODE_OUT_3, MUL_DIO_MODE_OUT_4};

const uint32_t TableClk0Dio0[4] = {CLK_0_DIO_0_1, CLK_0_DIO_0_2, CLK_0_DIO_0_3, CLK_0_DIO_0_4};
const uint32_t TableClk0Dio1[4] = {CLK_0_DIO_1_1, CLK_0_DIO_1_2, CLK_0_DIO_1_3, CLK_0_DIO_1_4};

void MUL_RefreshGpioParam(void)
{    
    uint32_t DIR_Pins = 0;
    uint32_t SWDIO_Pins = 0;
    uint32_t SWCLK_Pins = 0;
    uint32_t MODER_Mask = 0xFFFFFFFF;
    uint32_t MODER_Out = 0;
    uint32_t CLK_0_DIO_0 = 0;
    uint32_t CLK_0_DIO_1 = 0;
    uint8_t i;
    
    for (i = 0; i < 4; i++)
    {
        if (g_gMulSwd.Active[i] == 1 && g_gMulSwd.Ignore[i] == 0 && g_gMulSwd.TempIgnore[i] == 0)
        {
            DIR_Pins |= TablePinDIR[i];
            SWCLK_Pins  |= TablePinSWClK[i];
            SWDIO_Pins  |= TablePinSWDIO[i];
            MODER_Mask  &= TableModeMask[i];
            MODER_Out   |= TableModeOut[i];
            CLK_0_DIO_0 |= TableClk0Dio0[i];
            CLK_0_DIO_1 |= TableClk0Dio1[i];
        }
    }
    g_gMulSwd.DIR_Pins    = DIR_Pins;  
    g_gMulSwd.SWCLK_Pins  = SWCLK_Pins;
    g_gMulSwd.SWDIO_Pins  = SWDIO_Pins;
    g_gMulSwd.MODER_Mask  = MODER_Mask;
    g_gMulSwd.MODER_Out   = MODER_Out;
    g_gMulSwd.CLK_0_DIO_0 = CLK_0_DIO_0;
    g_gMulSwd.CLK_0_DIO_1 = CLK_0_DIO_1;   
}

/*
*********************************************************************************************************
*    函 数 名: MUL_SWD_TransferFast
*    功能说明: SWD Transfer I/O.  读和写一体
*    形    参: request: A[3:2] RnW APnDP
*              data:    DATA[31:0] 缓冲区。4个通道不同.
*    返 回 值: uint8_t指针，目标存放4个通道的应答值。s_ack[4]
*********************************************************************************************************
*/                   
uint8_t* MUL_SWD_TransferFast(uint32_t request, uint32_t *data) 
{ 
    static uint8_t s_ack[4]; 
    uint32_t ack;
    uint8_t *ack_buf;    
    uint32_t bit;  
    uint8_t *bit_buf = (uint8_t *)&bit;    
    uint32_t val;   
    uint32_t val_buf[4];  
    uint32_t pb_buf[4];
    uint32_t pb;
    uint32_t n;    
    uint8_t i;
    uint8_t route = 0;
    
    for (i = 0; i < 4; i++)
    {
        g_gMulSwd.TempIgnore[i] = 0;
    }
    
    MUL_RefreshGpioParam();     /* 刷新GPIO寄存器变量 */
    
    MUL_PIN_SWDIO_OUT_ENABLE();  
    
    /* Packet Request */                                                       
    pb = 0U;                                                               
    MUL_SW_WRITE_BIT_FAST(1U);                     /* Start Bit */                      
    bit = request >> 0;                                                        
    MUL_SW_WRITE_BIT_FAST(bit);                    /* APnDP Bit */                      
    pb += bit;                                                             
    bit = request >> 1;                                                        
    MUL_SW_WRITE_BIT_FAST(bit);                    /* RnW Bit */                        
    pb += bit;                                                             
    bit = request >> 2;                                                        
    MUL_SW_WRITE_BIT_FAST(bit);                    /* A2 Bit */                         
    pb += bit;                                                             
    bit = request >> 3;                                                        
    MUL_SW_WRITE_BIT_FAST(bit);                    /* A3 Bit */                         
    pb += bit;                                                             
    MUL_SW_WRITE_BIT_FAST(pb);                     /* Parity Bit */                     
    MUL_SW_WRITE_BIT_FAST(0U);                     /* Stop Bit */                       
    MUL_SW_WRITE_BIT_FAST(1U);                     /* Park Bit */                       
                                                                             
    /* Turnaround */                                                           
    MUL_PIN_SWDIO_OUT_DISABLE();                                                   
    for (n = DAP_Data.swd_conf.turnaround; n; n--) 
    {                           
        MUL_SW_CLOCK_CYCLE_FAST();                                                        
    }                                                                          
                                                                             
    /* Acknowledge response */                                                 
    MUL_SW_READ_BIT_FAST(bit);                                                          
    ack  = bit << 0;                                                           
    MUL_SW_READ_BIT_FAST(bit);                                                          
    ack |= bit << 1;                                                           
    MUL_SW_READ_BIT_FAST(bit);                                                          
    ack |= bit << 2;                                                           
    
    ack_buf = (uint8_t *)&ack;      /* 4个芯片同时应答 */

    route = 0;
    for (i = 0; i < 4; i++)
    {
        if (g_gMulSwd.Ignore[i] == 0)
        {
            if (ack_buf[i] == DAP_TRANSFER_OK)
            {
                route = DAP_TRANSFER_OK;        /* 4个芯片有1个OK，后面就优先处理OK流程 */
            }
            else
            {
                g_gMulSwd.TempIgnore[i] = 1;    /* 收到其他应答 */
            }
        }
    }
    
    if (route == DAP_TRANSFER_OK)           /* OK response */  
    {                  
        MUL_RefreshGpioParam();     /* 刷新GPIO寄存器变量 */
        
        if (request & DAP_TRANSFER_RnW)     /* 读数据 */
        {                                        
            /* Read data */                                                        
            /* armfly ： 优化奇偶校验算法 */  
            
            for (i = 0; i < 4; i++)
            {
                val_buf[i] = 0;
            }
            for (n = 32U; n; n--) 
            {                                                
                MUL_SW_READ_BIT_FAST(bit);               /* Read RDATA[0:31] */ 
                
                for (i = 0; i < 4; i++)
                {
                    val_buf[i] >>= 1;                                                           
                    val_buf[i]  |= bit_buf[i] << 31;  
                }
            }    
            
            for (i = 0; i < 4; i++)
            {
                pb_buf[i] = GetParity(val_buf[i]);
            }
            
            MUL_SW_READ_BIT_FAST(bit);                 /* Read Parity */                    
            
            for (i = 0; i < 4; i++)
            {
                if ((pb_buf[i] ^ bit_buf[i]) & 1U) 
                {                                             
                    ack_buf[i] = DAP_TRANSFER_ERROR;   
                    if (data > 0)
                    {
                        data[i] = 0;
                    }                    
                }
                else
                {
                    if (data > 0)
                    {
                        data[i] = val_buf[i];
                    }
                }
            }  
            
            /* Turnaround */                                                       
            for (n = DAP_Data.swd_conf.turnaround; n; n--) 
            {                       
                MUL_SW_CLOCK_CYCLE_FAST();                                                    
            }                                                                      
            MUL_PIN_SWDIO_OUT_ENABLE();                                                
        } 
        else    /* 写数据 */
        {                                                                 
            /* Turnaround */                                                       
            for (n = DAP_Data.swd_conf.turnaround; n; n--) 
            {                       
                MUL_SW_CLOCK_CYCLE_FAST();                                                    
            }                                                                      
            MUL_PIN_SWDIO_OUT_ENABLE();  
            
            /* Write data */                                                       
            val = *data;     

            /* armfly ： 优化奇偶校验算法 */
            pb = GetParity(val);
            MUL_SEND_32BIT_FAST(val);      

            MUL_SW_WRITE_BIT_FAST(pb);             /* Write Parity Bit */               
        }                                                                        
        /* Idle cycles */                                                        
        n = DAP_Data.transfer.idle_cycles;                                       
        if (n) 
        {                                                                 
            MUL_PIN_SWDIO_OUT(0U);                                                     
            for (; n; n--) 
            {                                                       
                MUL_SW_CLOCK_CYCLE_FAST();                                                    
            }                                                                      
        }                                                                        
        MUL_PIN_SWDIO_OUT(1U);                                                                                                      
    }  
    
    /* 处理异常 */
    for (i = 0; i < 4; i++)
    {
        g_gMulSwd.TempIgnore[i] = 0;
    }    
    route = 0;
    for (i = 0; i < 4; i++)
    {
        if (g_gMulSwd.Ignore[i] == 0)
        {
            if (ack_buf[i] == DAP_TRANSFER_WAIT || ack_buf[i] == DAP_TRANSFER_FAULT)
            {
                route = ack_buf[i];
            }
            else
            {
                g_gMulSwd.TempIgnore[i] = 1;    /* 收到其他应答 */
            }
        }
    }  
    
    if ((route == DAP_TRANSFER_WAIT) || (route == DAP_TRANSFER_FAULT)) 
    {           
        MUL_RefreshGpioParam();     /* 刷新GPIO寄存器变量 */
        
        /* WAIT or FAULT response */                                             
        if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) != 0U)) 
        { 
            for (n = 32U+1U; n; n--) 
            {                                             
                MUL_SW_CLOCK_CYCLE_FAST();               /* Dummy Read RDATA[0:31] + Parity */
            }                                                                      
        }                                                                        
        /* Turnaround */                                                         
        for (n = DAP_Data.swd_conf.turnaround; n; n--) 
        {                         
            MUL_SW_CLOCK_CYCLE_FAST();                                                      
        }                                                                        
        MUL_PIN_SWDIO_OUT_ENABLE();                                                  
        if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) == 0U)) 
        {
            MUL_PIN_SWDIO_OUT(0U);                                                     
            for (n = 32U+1U; n; n--) 
            {                                             
                MUL_SW_CLOCK_CYCLE_FAST();               /* Dummy Write WDATA[0:31] + Parity */
            }                                                                      
        }                                                                        
        MUL_PIN_SWDIO_OUT(1U);                                                                                                        
    }                                                                          
   
    /* 处理异常 */
    for (i = 0; i < 4; i++)
    {
        g_gMulSwd.TempIgnore[i] = 0;
    }
    route = 0;
    for (i = 0; i < 4; i++)
    {        
        if (g_gMulSwd.Ignore[i] == 0)
        {
            if (ack_buf[i] == DAP_TRANSFER_WAIT || ack_buf[i] == DAP_TRANSFER_FAULT 
                || ack_buf[i] == DAP_TRANSFER_OK)
            {
                g_gMulSwd.TempIgnore[i] = 1;
            }
            else
            {
                g_gMulSwd.TempIgnore[i] = 0;    /* 收到其他应答 */
                route = DAP_TRANSFER_ERROR;
            }
        }
    }
    if (route == DAP_TRANSFER_ERROR)
    {
        MUL_RefreshGpioParam();     /* 刷新GPIO寄存器变量 */
        
        /* Protocol error */                                                       
        for (n = DAP_Data.swd_conf.turnaround + 32U + 1U; n; n--) 
        {                
            MUL_SW_CLOCK_CYCLE_FAST();                   /* Back off data phase */            
        }                                                                          
        MUL_PIN_SWDIO_OUT_ENABLE();                                                    
        MUL_PIN_SWDIO_OUT(1U);                                                         
    }
    
    for (i = 0; i < 4; i++)
    {
        g_gMulSwd.TempIgnore[i] = 0;
        
        s_ack[i] = ack_buf[i];
    }
    return s_ack;
}

uint8_t* MUL_SWD_TransferSlow(uint32_t request, uint32_t *data) 
{ 
    static uint8_t s_ack[4]; 
    uint32_t ack;
    uint8_t *ack_buf;    
    uint32_t bit;  
    uint8_t *bit_buf = (uint8_t *)&bit;    
    uint32_t val;   
    uint32_t val_buf[4];  
    uint32_t pb_buf[4];
    uint32_t pb;
    uint32_t n;    
    uint8_t i;
    uint8_t route = 0;
    
    for (i = 0; i < 4; i++)
    {
        g_gMulSwd.TempIgnore[i] = 0;
    }
    
    MUL_RefreshGpioParam();     /* 刷新GPIO寄存器变量 */
    
    MUL_PIN_SWDIO_OUT_ENABLE();  
    
    /* Packet Request */                                                       
    pb = 0U;                                                               
    MUL_SW_WRITE_BIT_SLOW(1U);                     /* Start Bit */                      
    bit = request >> 0;                                                        
    MUL_SW_WRITE_BIT_SLOW(bit);                    /* APnDP Bit */                      
    pb += bit;                                                             
    bit = request >> 1;                                                        
    MUL_SW_WRITE_BIT_SLOW(bit);                    /* RnW Bit */                        
    pb += bit;                                                             
    bit = request >> 2;                                                        
    MUL_SW_WRITE_BIT_SLOW(bit);                    /* A2 Bit */                         
    pb += bit;                                                             
    bit = request >> 3;                                                        
    MUL_SW_WRITE_BIT_SLOW(bit);                    /* A3 Bit */                         
    pb += bit;                                                             
    MUL_SW_WRITE_BIT_SLOW(pb);                     /* Parity Bit */                     
    MUL_SW_WRITE_BIT_SLOW(0U);                     /* Stop Bit */                       
    MUL_SW_WRITE_BIT_SLOW(1U);                     /* Park Bit */                       
                                                                             
    /* Turnaround */                                                           
    MUL_PIN_SWDIO_OUT_DISABLE();                                                   
    for (n = DAP_Data.swd_conf.turnaround; n; n--) 
    {                           
        MUL_SW_CLOCK_CYCLE_SLOW();                                                        
    }                                                                          
                                                                             
    /* Acknowledge response */                                                 
    MUL_SW_READ_BIT_SLOW(bit);                                                          
    ack  = bit << 0;                                                           
    MUL_SW_READ_BIT_SLOW(bit);                                                          
    ack |= bit << 1;                                                           
    MUL_SW_READ_BIT_SLOW(bit);                                                          
    ack |= bit << 2;                                                           
    
    ack_buf = (uint8_t *)&ack;      /* 4个芯片同时应答 */

    route = 0;
    for (i = 0; i < 4; i++)
    {
        if (g_gMulSwd.Ignore[i] == 0)
        {
            if (ack_buf[i] == DAP_TRANSFER_OK)
            {
                route = DAP_TRANSFER_OK;        /* 4个芯片有1个OK，后面就优先处理OK流程 */
            }
            else
            {
                g_gMulSwd.TempIgnore[i] = 1;    /* 收到其他应答 */
            }
        }
    }
    
    if (route == DAP_TRANSFER_OK)           /* OK response */  
    {                  
        MUL_RefreshGpioParam();     /* 刷新GPIO寄存器变量 */
        
        if (request & DAP_TRANSFER_RnW)     /* 读数据 */
        {                                        
            /* Read data */                                                        
            /* armfly ： 优化奇偶校验算法 */  
            
            for (i = 0; i < 4; i++)
            {
                val_buf[i] = 0;
            }
            for (n = 32U; n; n--) 
            {                                                
                MUL_SW_READ_BIT_SLOW(bit);               /* Read RDATA[0:31] */ 
                
                for (i = 0; i < 4; i++)
                {
                    val_buf[i] >>= 1;                                                           
                    val_buf[i]  |= bit_buf[i] << 31;  
                }
            }    
            
            for (i = 0; i < 4; i++)
            {
                pb_buf[i] = GetParity(val_buf[i]);
            }
            
            MUL_SW_READ_BIT_SLOW(bit);                 /* Read Parity */                    
            
            for (i = 0; i < 4; i++)
            {
                if ((pb_buf[i] ^ bit_buf[i]) & 1U) 
                {                                             
                    ack_buf[i] = DAP_TRANSFER_ERROR;   
                    if (data > 0)
                    {
                        data[i] = 0;
                    }                    
                }
                else
                {
                    if (data > 0)
                    {
                        data[i] = val_buf[i];
                    }
                }
            }  
            
            /* Turnaround */                                                       
            for (n = DAP_Data.swd_conf.turnaround; n; n--) 
            {                       
                MUL_SW_CLOCK_CYCLE_SLOW();                                                    
            }                                                                      
            MUL_PIN_SWDIO_OUT_ENABLE();                                                
        } 
        else    /* 写数据 */
        {                                                                 
            /* Turnaround */                                                       
            for (n = DAP_Data.swd_conf.turnaround; n; n--) 
            {                       
                MUL_SW_CLOCK_CYCLE_SLOW();                                                    
            }                                                                      
            MUL_PIN_SWDIO_OUT_ENABLE();  
            
            /* Write data */                                                       
            val = *data;     

            /* armfly ： 优化奇偶校验算法 */
            pb = GetParity(val);
            MUL_SEND_32BIT_SLOW(val);      

            MUL_SW_WRITE_BIT_SLOW(pb);             /* Write Parity Bit */               
        }                                                                        
        /* Idle cycles */                                                        
        n = DAP_Data.transfer.idle_cycles;                                       
        if (n) 
        {                                                                 
            MUL_PIN_SWDIO_OUT(0U);                                                     
            for (; n; n--) 
            {                                                       
                MUL_SW_CLOCK_CYCLE_SLOW();                                                    
            }                                                                      
        }                                                                        
        MUL_PIN_SWDIO_OUT(1U);                                                                                                      
    }  
    
    /* 处理异常 */
    for (i = 0; i < 4; i++)
    {
        g_gMulSwd.TempIgnore[i] = 0;
    }    
    route = 0;
    for (i = 0; i < 4; i++)
    {
        if (g_gMulSwd.Ignore[i] == 0)
        {
            if (ack_buf[i] == DAP_TRANSFER_WAIT || ack_buf[i] == DAP_TRANSFER_FAULT)
            {
                route = ack_buf[i];
            }
            else
            {
                g_gMulSwd.TempIgnore[i] = 1;    /* 收到其他应答 */
            }
        }
    }  
    
    if ((route == DAP_TRANSFER_WAIT) || (route == DAP_TRANSFER_FAULT)) 
    {           
        MUL_RefreshGpioParam();     /* 刷新GPIO寄存器变量 */
        
        /* WAIT or FAULT response */                                             
        if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) != 0U)) 
        { 
            for (n = 32U+1U; n; n--) 
            {                                             
                MUL_SW_CLOCK_CYCLE_SLOW();               /* Dummy Read RDATA[0:31] + Parity */
            }                                                                      
        }                                                                        
        /* Turnaround */                                                         
        for (n = DAP_Data.swd_conf.turnaround; n; n--) 
        {                         
            MUL_SW_CLOCK_CYCLE_SLOW();                                                      
        }                                                                        
        MUL_PIN_SWDIO_OUT_ENABLE();                                                  
        if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) == 0U)) 
        {
            MUL_PIN_SWDIO_OUT(0U);                                                     
            for (n = 32U+1U; n; n--) 
            {                                             
                MUL_SW_CLOCK_CYCLE_SLOW();               /* Dummy Write WDATA[0:31] + Parity */
            }                                                                      
        }                                                                        
        MUL_PIN_SWDIO_OUT(1U);                                                                                                        
    }                                                                          
   
    /* 处理异常 */
    for (i = 0; i < 4; i++)
    {
        g_gMulSwd.TempIgnore[i] = 0;
    }
    route = 0;
    for (i = 0; i < 4; i++)
    {        
        if (g_gMulSwd.Ignore[i] == 0)
        {
            if (ack_buf[i] == DAP_TRANSFER_WAIT || ack_buf[i] == DAP_TRANSFER_FAULT 
                || ack_buf[i] == DAP_TRANSFER_OK)
            {
                g_gMulSwd.TempIgnore[i] = 1;
            }
            else
            {
                g_gMulSwd.TempIgnore[i] = 0;    /* 收到其他应答 */
                route = DAP_TRANSFER_ERROR;
            }
        }
    }
    if (route == DAP_TRANSFER_ERROR)
    {
        MUL_RefreshGpioParam();     /* 刷新GPIO寄存器变量 */
        
        /* Protocol error */                                                       
        for (n = DAP_Data.swd_conf.turnaround + 32U + 1U; n; n--) 
        {                
            MUL_SW_CLOCK_CYCLE_SLOW();                   /* Back off data phase */            
        }                                                                          
        MUL_PIN_SWDIO_OUT_ENABLE();                                                    
        MUL_PIN_SWDIO_OUT(1U);                                                         
    }
    
    for (i = 0; i < 4; i++)
    {
        g_gMulSwd.TempIgnore[i] = 0;
        
        s_ack[i] = ack_buf[i];
    }
    return s_ack;                                               
}

// SWD Transfer I/O
//   request: A[3:2] RnW APnDP
//   data:    DATA[31:0]
//   return:  ACK[2:0]
uint8_t*  MUL_SWD_Transfer(uint32_t request, uint32_t *data) 
{
    if (DAP_Data.fast_clock) 
    {
        return MUL_SWD_TransferFast(request, data);        
    } 
    else 
    {
        return MUL_SWD_TransferSlow(request, data);
    }
}

/********************************* armfly 优化时序速度 ***************************/
// SWD Transfer I/O
//   request: A[3:2] RnW APnDP
//   data:    DATA[31:0]
//   return:  ACK[2:0]                              
#undef  PIN_DELAY
#define PIN_DELAY() PIN_DELAY_FAST()


#undef  PIN_DELAY
#define PIN_DELAY() PIN_DELAY_SLOW(DAP_Data.clock_delay)
// Generate SWJ Sequence
//   count:  sequence bit count
//   data:   pointer to sequence bit data
//   return: none
#if ((DAP_SWD != 0) || (DAP_JTAG != 0))
void MUL_SWJ_Sequence (uint32_t count, const uint8_t *data) {
  uint32_t val;
  uint32_t n;

  val = 0U;
  n = 0U;
  while (count--) {
    if (n == 0U) {
      val = *data++;
      n = 8U;
    }
    if (val & 1U) {
      MUL_PIN_SWDIO_OUT(1);
    } else {
      MUL_PIN_SWDIO_OUT(0);
    }
    MUL_SW_CLOCK_CYCLE_SLOW();
    val >>= 1;
    n--;
  }
}
#endif

