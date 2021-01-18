/**
 * @file    SW_DP.c
 * @brief   SWD driver
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

/* 
    SPI_BAUDRATEPRESCALER_4 = 50MHz SCK时钟，不能工作。
    SPI_BAUDRATEPRESCALER_8 = 25MHz SCK时钟，稳定读写。 写并读2048字节，耗时5ms，等效传输速度 800KB/s
    
    SPI_BAUDRATEPRESCALER_16 = 12.5MHz
    SPI_BAUDRATEPRESCALER_32 = 6.25MHz
    SPI_BAUDRATEPRESCALER_64 = 3.125MHz
    SPI_BAUDRATEPRESCALER_128 = 1.5625MHz
    
    实测ST-LINK下载640K程序，擦除需要9秒（这是CPU的FLASH擦除时间较慢导致），编程和校验需要13秒。
    按H7-TOOL的800KB/S的速度，SWD时序传输可以在1秒内完成，剩下的时间主要靠USB通信和CPU运算来提高效率。
    因此在SWD底层时序上优化已经没有多大的必要了，也许可以提高到1MB/S。但是对整体效率没有多大改善。
    
    
    SPI_BAUDRATEPRESCALER_32 : 稳定读写 STM32F030RCT6
    
    已经废弃SPI硬件模式，改用软件SPI模式
*/

/*
    延迟0: 写16.7M, 读9M
    
    延迟1:   写6.2M  读5.4M
    延迟2:   写5.7M  读5.0M
    延迟3:   写5.3M  读4.6M
    延迟4:   写4.2M  读4.3M
    延迟10:  写2.94M  读3.1M
    延迟20:  写2.1M  读2.0M
    延迟40:  写1.29M  读1.29M
    延迟100: 写 594K  读 594K 
    
    延迟200: 写 313K  读 313K
    延迟1000:写 65K  读 65K
    延迟2000:写 33.1K  读33.1K 
    延迟5000:写 13.2K  读13.2K 
*/

//#define PIN_SWCLK_SET()       PIN_SWCLK_TCK_SET();PIN_SWCLK_TCK_SET()
//#define PIN_SWCLK_CLR()       PIN_SWCLK_TCK_CLR();PIN_SWCLK_TCK_CLR()

#define PIN_SWCLK_SET()       PIN_SWCLK_TCK_SET()
#define PIN_SWCLK_CLR()       PIN_SWCLK_TCK_CLR()

/* SPI软件模式，低速配置 */
#define PIN_DELAY_S()           PIN_DELAY_SLOW(DAP_Data.clock_delay)
#define SW_CLOCK_CYCLE_SLOW()   PIN_SWCLK_CLR();  PIN_DELAY_S();  PIN_SWCLK_SET(); PIN_DELAY_S()
#define SW_WRITE_BIT_SLOW(bit)  PIN_SWDIO_OUT(bit); PIN_SWCLK_CLR(); PIN_DELAY_S(); PIN_SWCLK_SET(); PIN_DELAY_S()
#define SW_READ_BIT_SLOW(bit)   PIN_SWCLK_CLR();  PIN_DELAY_S(); bit = PIN_SWDIO_IN(); PIN_SWCLK_SET(); PIN_DELAY_S()

#define SW_CLOCK_CYCLE_FAST()   PIN_SWCLK_CLR();  PIN_SWCLK_SET();
#if 1   /* 优化GPIO操作 */
    #define SW_WRITE_BIT_FAST(bit)  \
            if (bit & 1)                \
            {                           \
                GPIOD->BSRR = GPIO_PIN_4 + (GPIO_PIN_3 << 16U); \
                PIN_SWCLK_SET();     \
            } \
            else \
            { \
                GPIOD->BSRR = (GPIO_PIN_4 + GPIO_PIN_3) << 16U; \
                PIN_SWCLK_SET(); \
            } 
#else
    #define SW_WRITE_BIT_FAST(bit)  PIN_SWDIO_OUT(bit); PIN_SWCLK_CLR(); PIN_SWCLK_SET();
#endif

#define SW_READ_BIT_FAST(bit)   PIN_SWCLK_CLR();  bit = PIN_SWDIO_IN(); PIN_SWCLK_SET();

#if (DAP_SWD != 0)

/* 优化 PD3 PD4 同步操作节省30ns */   

//#define SWCLK_TCK_PIN_PORT           GPIOD
//#define SWCLK_TCK_PIN                GPIO_PIN_3

//#define SWDIO_OUT_PIN_PORT           GPIOD
//#define SWDIO_OUT_PIN                GPIO_PIN_4

//#define BSP_SET_GPIO_1(gpio, pin)   gpio->BSRR = pin
//#define BSP_SET_GPIO_0(gpio, pin)   gpio->BSRR = (uint32_t)pin << 16U

#define SEND_32BIT_ONCE_FAST()           \
        if (val & 1)                \
        {                           \
            GPIOD->BSRR = GPIO_PIN_4 + (GPIO_PIN_3 << 16U); \
            val >>= 1;  \
            PIN_SWCLK_SET();     \
        } \
        else \
        { \
            GPIOD->BSRR = (GPIO_PIN_4 + GPIO_PIN_3) << 16U; \
            val >>= 1;  \
            PIN_SWCLK_SET(); \
        } 

#define SEND_32BIT_ONCE_SLOW()           \
        if (val & 1)                \
        {                           \
            GPIOD->BSRR = GPIO_PIN_4 + (GPIO_PIN_3 << 16U); \
            val >>= 1;  PIN_DELAY_S(); \
            PIN_SWCLK_SET(); PIN_DELAY_S();   \
        } \
        else \
        { \
            GPIOD->BSRR = (GPIO_PIN_4 + GPIO_PIN_3) << 16U; \
            val >>= 1;  PIN_DELAY_S(); \
            PIN_SWCLK_SET(); PIN_DELAY_S();  \
        } 
        
static const uint8_t ParityTable256[256] = 
{
#   define P2(n) n, n^1, n^1, n
#   define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
#   define P6(n) P4(n), P4(n^1), P4(n^1), P4(n)
    P6(0), P6(1), P6(1), P6(0)
};

uint8_t GetParity(uint32_t data)
{
    uint8_t parity;
    
    data ^= data >> 16;
    data ^= data >> 8;
    
    parity = ParityTable256[data & 0xff];
    
    return parity;
}

// SWD Transfer I/O
//   request: A[3:2] RnW APnDP
//   data:    DATA[31:0]
//   return:  ACK[2:0]
uint8_t SWD_TransferFast(uint32_t request, uint32_t *data) 
{ 
  uint32_t ack;                                                              
  uint32_t bit;                                                              
  uint32_t val;                                                              
  uint32_t parity;                                                           
                                                                             
  uint32_t n;                                                                
                                                                             
  /* Packet Request */                                                       
  parity = 0U;                                                               
  SW_WRITE_BIT_FAST(1U);                     /* Start Bit */                      
  bit = request >> 0;                                                        
  SW_WRITE_BIT_FAST(bit);                    /* APnDP Bit */                      
  parity += bit;                                                             
  bit = request >> 1;                                                        
  SW_WRITE_BIT_FAST(bit);                    /* RnW Bit */                        
  parity += bit;                                                             
  bit = request >> 2;                                                        
  SW_WRITE_BIT_FAST(bit);                    /* A2 Bit */                         
  parity += bit;                                                             
  bit = request >> 3;                                                        
  SW_WRITE_BIT_FAST(bit);                    /* A3 Bit */                         
  parity += bit;                                                             
  SW_WRITE_BIT_FAST(parity);                 /* Parity Bit */                     
  SW_WRITE_BIT_FAST(0U);                     /* Stop Bit */                       
  SW_WRITE_BIT_FAST(1U);                     /* Park Bit */                       
                                                                             
  /* Turnaround */                                                           
  PIN_SWDIO_OUT_DISABLE();                                                   
  for (n = DAP_Data.swd_conf.turnaround; n; n--) {                           
    SW_CLOCK_CYCLE_FAST();                                                        
  }                                                                          
                                                                             
  /* Acknowledge response */                                                 
  SW_READ_BIT_FAST(bit);                                                          
  ack  = bit << 0;                                                           
  SW_READ_BIT_FAST(bit);                                                          
  ack |= bit << 1;                                                           
  SW_READ_BIT_FAST(bit);                                                          
  ack |= bit << 2;                                                           
                                                                             
  if (ack == DAP_TRANSFER_OK) {         /* OK response */                    
    /* Data transfer */                                                      
    if (request & DAP_TRANSFER_RnW) {                                        
      /* Read data */                                                        
        /* armfly ： 优化奇偶校验算法 */  
      val = 0U;  
      #if 0
      {
        uint32_t buf[32];

        // PD3 = CLK  PD4 = DIO
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[0] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[1] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[2] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[3] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[4] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[5] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[6] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[7] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[8] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[9] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[10] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[11] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[12] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[13] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[14] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[15] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[16] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[17] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[18] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[19] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[20] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[21] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[22] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[23] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[24] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[25] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[26] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[27] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[28] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[29] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[30] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;
        GPIOD->BSRR = (GPIO_PIN_3 << 16U); buf[31] = GPIOD->IDR; GPIOD->BSRR = GPIO_PIN_3;

        parity = 0;
        for (n = 0; n < 32; n++)
        {
            val >>= 1;
            if (buf[n] & GPIO_PIN_4)
            {
                val |= 0x80000000;
                parity++;
            }
        }
        //parity = GetParity(val);
      }
      #else
      for (n = 32U; n; n--) {                                                
        SW_READ_BIT_FAST(bit);               /* Read RDATA[0:31] */                                                                    
        val >>= 1;                                                           
        val  |= bit << 31;                                                   
      } 
      parity = GetParity(val);  
      #endif     

      

          
      SW_READ_BIT_FAST(bit);                 /* Read Parity */                    
      if ((parity ^ bit) & 1U) {                                             
        ack = DAP_TRANSFER_ERROR;                                            
      }                                                                      
      if (data) { *data = val; }                                             
      /* Turnaround */                                                       
      for (n = DAP_Data.swd_conf.turnaround; n; n--) {                       
        SW_CLOCK_CYCLE_FAST();                                                    
      }                                                                      
      PIN_SWDIO_OUT_ENABLE();                                                
    } else {                                                                 
      /* Turnaround */                                                       
      for (n = DAP_Data.swd_conf.turnaround; n; n--) {                       
        SW_CLOCK_CYCLE_FAST();                                                    
      }                                                                      
      PIN_SWDIO_OUT_ENABLE();                                                
      /* Write data */                                                       
      val = *data;     

        /* armfly ： 优化奇偶校验算法 */
        parity = GetParity(val);
        SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();
        SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();
        SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();
        SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();  
        SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();
        SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();
        SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();
        SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();SEND_32BIT_ONCE_FAST();       

      SW_WRITE_BIT_FAST(parity);             /* Write Parity Bit */               
    }                                                                        
    /* Idle cycles */                                                        
    n = DAP_Data.transfer.idle_cycles;                                       
    if (n) {                                                                 
      PIN_SWDIO_OUT(0U);                                                     
      for (; n; n--) {                                                       
        SW_CLOCK_CYCLE_FAST();                                                    
      }                                                                      
    }                                                                        
    PIN_SWDIO_OUT(1U);                                                       
    return ((uint8_t)ack);                                                   
  }                                                                          
                                                                             
  if ((ack == DAP_TRANSFER_WAIT) || (ack == DAP_TRANSFER_FAULT)) {           
    /* WAIT or FAULT response */                                             
    if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) != 0U)) { 
      for (n = 32U+1U; n; n--) {                                             
        SW_CLOCK_CYCLE_FAST();               /* Dummy Read RDATA[0:31] + Parity */
      }                                                                      
    }                                                                        
    /* Turnaround */                                                         
    for (n = DAP_Data.swd_conf.turnaround; n; n--) {                         
      SW_CLOCK_CYCLE_FAST();                                                      
    }                                                                        
    PIN_SWDIO_OUT_ENABLE();                                                  
    if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) == 0U)) {
      PIN_SWDIO_OUT(0U);                                                     
      for (n = 32U+1U; n; n--) {                                             
        SW_CLOCK_CYCLE_FAST();               /* Dummy Write WDATA[0:31] + Parity */
      }                                                                      
    }                                                                        
    PIN_SWDIO_OUT(1U);                                                       
    return ((uint8_t)ack);                                                   
  }                                                                          
                                                                             
  /* Protocol error */                                                       
  for (n = DAP_Data.swd_conf.turnaround + 32U + 1U; n; n--) {                
    SW_CLOCK_CYCLE_FAST();                   /* Back off data phase */            
  }                                                                          
  PIN_SWDIO_OUT_ENABLE();                                                    
  PIN_SWDIO_OUT(1U);                                                         
  return ((uint8_t)ack);                                                       
}

uint8_t SWD_TransferSlow(uint32_t request, uint32_t *data) 
{
  uint32_t ack;
  uint32_t bit;
  uint32_t val;
  uint32_t parity;

  uint32_t n;

  /* Packet Request */
  parity = 0U;
  SW_WRITE_BIT_SLOW(1U);                     /* Start Bit */
  bit = request >> 0;
  SW_WRITE_BIT_SLOW(bit);                    /* APnDP Bit */
  parity += bit;
  bit = request >> 1;
  SW_WRITE_BIT_SLOW(bit);                    /* RnW Bit */
  parity += bit;
  bit = request >> 2;
  SW_WRITE_BIT_SLOW(bit);                    /* A2 Bit */
  parity += bit;
  bit = request >> 3;
  SW_WRITE_BIT_SLOW(bit);                    /* A3 Bit */
  parity += bit;
  SW_WRITE_BIT_SLOW(parity);                 /* Parity Bit */
  SW_WRITE_BIT_SLOW(0U);                     /* Stop Bit */
  SW_WRITE_BIT_SLOW(1U);                     /* Park Bit */

  /* Turnaround */
  PIN_SWDIO_OUT_DISABLE();
  for (n = DAP_Data.swd_conf.turnaround; n; n--) {
    SW_CLOCK_CYCLE_SLOW();
  }

  /* Acknowledge response */
  SW_READ_BIT_SLOW(bit);
  ack  = bit << 0;
  SW_READ_BIT_SLOW(bit);
  ack |= bit << 1;
  SW_READ_BIT_SLOW(bit);
  ack |= bit << 2;

  if (ack == DAP_TRANSFER_OK) {         /* OK response */
    /* Data transfer */
    if (request & DAP_TRANSFER_RnW) {
      /* Read data */
        /* armfly ： 优化奇偶校验算法 */
      val = 0U;
      for (n = 32U; n; n--) {
        SW_READ_BIT_SLOW(bit);               /* Read RDATA[0:31] */
        val >>= 1;
        val  |= bit << 31;
      }

      parity = GetParity(val);


      SW_READ_BIT_SLOW(bit);                 /* Read Parity */
      if ((parity ^ bit) & 1U) {
        ack = DAP_TRANSFER_ERROR;
      }
      if (data) { *data = val; }
      /* Turnaround */
      for (n = DAP_Data.swd_conf.turnaround; n; n--) {
        SW_CLOCK_CYCLE_SLOW();
      }
      PIN_SWDIO_OUT_ENABLE();
    } else {
      /* Turnaround */
      for (n = DAP_Data.swd_conf.turnaround; n; n--) {
        SW_CLOCK_CYCLE_SLOW();
      }
      PIN_SWDIO_OUT_ENABLE();
      /* Write data */
      val = *data;

        /* armfly ： 优化奇偶校验算法 */
        parity = GetParity(val);
        SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();
        SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();
        SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();
        SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();
        SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();
        SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();
        SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();
        SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();SEND_32BIT_ONCE_SLOW();

      SW_WRITE_BIT_SLOW(parity);             /* Write Parity Bit */
    }
    /* Idle cycles */
    n = DAP_Data.transfer.idle_cycles;
    if (n) {
      PIN_SWDIO_OUT(0U);
      for (; n; n--) {
        SW_CLOCK_CYCLE_SLOW();
      }
    }
    PIN_SWDIO_OUT(1U);
    return ((uint8_t)ack);
  }

  if ((ack == DAP_TRANSFER_WAIT) || (ack == DAP_TRANSFER_FAULT)) {
    /* WAIT or FAULT response */
    if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) != 0U)) {
      for (n = 32U+1U; n; n--) {
        SW_CLOCK_CYCLE_SLOW();               /* Dummy Read RDATA[0:31] + Parity */
      }
    }
    /* Turnaround */
    for (n = DAP_Data.swd_conf.turnaround; n; n--) {
      SW_CLOCK_CYCLE_SLOW();
    }
    PIN_SWDIO_OUT_ENABLE();
    if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) == 0U)) {
      PIN_SWDIO_OUT(0U);
      for (n = 32U+1U; n; n--) {
        SW_CLOCK_CYCLE_SLOW();               /* Dummy Write WDATA[0:31] + Parity */
      }
    }
    PIN_SWDIO_OUT(1U);
    return ((uint8_t)ack);
  }

  /* Protocol error */
  for (n = DAP_Data.swd_conf.turnaround + 32U + 1U; n; n--) {
    SW_CLOCK_CYCLE_SLOW();                   /* Back off data phase */
  }
  PIN_SWDIO_OUT_ENABLE();
  PIN_SWDIO_OUT(1U);
  return ((uint8_t)ack);
}

// SWD Transfer I/O
//   request: A[3:2] RnW APnDP
//   data:    DATA[31:0]
//   return:  ACK[2:0]
uint8_t  SWD_Transfer(uint32_t request, uint32_t *data) 
{
    if (DAP_Data.fast_clock) 
    {
        return SWD_TransferFast(request, data);        
    } 
    else 
    {
        return SWD_TransferSlow(request, data);
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
void SWJ_Sequence (uint32_t count, const uint8_t *data) {
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
      PIN_SWDIO_TMS_SET();
    } else {
      PIN_SWDIO_TMS_CLR();
    }
    SW_CLOCK_CYCLE_SLOW();
    val >>= 1;
    n--;
  }
}
#endif

#endif  /* (DAP_SWD != 0) */
