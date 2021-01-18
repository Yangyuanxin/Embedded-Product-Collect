/**
 * @file    IO_Config.h
 * @brief
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
 */

#ifndef __IO_CONFIG_H__
#define __IO_CONFIG_H__

#include "stm32h7xx_hal.h"
#include "compiler.h"
#include "daplink.h"

#define DAPLINK_HIC_ID    DAPLINK_HIC_ID_STM32F103XB

COMPILER_ASSERT(DAPLINK_HIC_ID == DAPLINK_HIC_ID_STM32F103XB);

//USB control pin
#if 0
    #define USB_CONNECT_PORT_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
    #define USB_CONNECT_PORT_DISABLE()   __HAL_RCC_GPIOA_CLK_DISABLE()
    #define USB_CONNECT_PORT             GPIOA
    #define USB_CONNECT_PIN              GPIO_PIN_15
    #define USB_CONNECT_ON()             (USB_CONNECT_PORT->BSRR = USB_CONNECT_PIN)
    #define USB_CONNECT_OFF()            (USB_CONNECT_PORT->BRR  = USB_CONNECT_PIN)
#else
    #define USB_CONNECT_ON() 
    #define USB_CONNECT_OFF()   
#endif

//Connected LED
#define CONNECTED_LED_PORT           GPIOB
#define CONNECTED_LED_PIN            GPIO_PIN_6
#define CONNECTED_LED_PIN_Bit        6

//When bootloader, disable the target port(not used)
#define POWER_EN_PIN_PORT            GPIOB
#define POWER_EN_PIN                 GPIO_PIN_15
#define POWER_EN_Bit                 15

// nRESET OUT Pin
#define nRESET_PIN_PORT              GPIOE
#define nRESET_PIN                   GPIO_PIN_4
#define nRESET_PIN_Bit               4

//SWD
#define SWCLK_TCK_PIN_PORT           GPIOD
#define SWCLK_TCK_PIN                GPIO_PIN_3
#define SWCLK_TCK_PIN_Bit            3

//#define SWDIO_OUT_PIN_PORT           GPIOI
//#define SWDIO_OUT_PIN                GPIO_PIN_3
//#define SWDIO_OUT_PIN_Bit            3

//#define SWDIO_IN_PIN_PORT            GPIOI
//#define SWDIO_IN_PIN                 GPIO_PIN_3
//#define SWDIO_IN_PIN_Bit             3

#define SWDIO_OUT_PIN_PORT           GPIOD
#define SWDIO_OUT_PIN                GPIO_PIN_4
#define SWDIO_OUT_PIN_Bit            4

#define SWDIO_IN_PIN_PORT            GPIOD
#define SWDIO_IN_PIN                 GPIO_PIN_4
#define SWDIO_IN_PIN_Bit             4

//LEDs
//USB status LED
#define RUNNING_LED_PORT             GPIOI
#define RUNNING_LED_PIN              GPIO_PIN_8
#define RUNNING_LED_Bit              8

#define PIN_HID_LED_PORT             GPIOI
#define PIN_HID_LED                  GPIO_PIN_8
#define PIN_HID_LED_Bit              8

#define PIN_CDC_LED_PORT             GPIOI
#define PIN_CDC_LED                  GPIO_PIN_8
#define PIN_CDC_LED_Bit              8

#define PIN_MSC_LED_PORT             GPIOI
#define PIN_MSC_LED                  GPIO_PIN_8
#define PIN_MSC_LED_Bit              8


#endif
