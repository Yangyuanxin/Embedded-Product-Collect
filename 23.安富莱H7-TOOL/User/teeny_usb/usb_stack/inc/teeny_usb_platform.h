/*       
 *         _______                    _    _  _____ ____  
 *        |__   __|                  | |  | |/ ____|  _ \ 
 *           | | ___  ___ _ __  _   _| |  | | (___ | |_) |
 *           | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ < 
 *           | |  __/  __/ | | | |_| | |__| |____) | |_) |
 *           |_|\___|\___|_| |_|\__, |\____/|_____/|____/ 
 *                               __/ |                    
 *                              |___/                     
 *
 * TeenyUSB - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2019 XToolBox  - admin@xtoolbox.org
 *                         www.tusb.org
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// tiny usb platform header for STM32F1/STM32F0 device
#ifndef __TEENY_USB_PLATFORM_H__
#define __TEENY_USB_PLATFORM_H__

#if defined(STM32F042x6) || defined(STM32F048xx) || defined(STM32F072xB) || defined(STM32F078xx) || defined(STM32F070xB)|| defined(STM32F070x6)
// HAL header for STM32F0xx devices
#include "stm32f0xx_hal.h"
#include "stm32_fs_platform.h"

#elif defined(STM32F102x6) || defined(STM32F102xB) || \
    defined(STM32F103x6) || defined(STM32F103xB) || \
    defined(STM32F103xE) || defined(STM32F103xG)
// HAL header for STM32F1xx devices
#include "stm32f1xx_hal.h"
#include "stm32_fs_platform.h"

#elif defined(STM32F105xC) || defined(STM32F107xC)
// HAL header for STM32F1xx connective line devices
#include "stm32f1xx_hal.h"
#include "stm32_otg_platform.h"

#elif defined (STM32F205xx) || defined (STM32F215xx) || defined (STM32F207xx) || defined (STM32F217xx)
// HAL header for STM32F2xx devices
#include "stm32f2xx_hal.h"
#include "stm32_otg_platform.h"


#elif defined(STM32F302xE) || defined(STM32F303xE) || defined(STM32F302x8) || \
       defined(STM32F302xC) || defined(STM32F303xC) || defined(STM32F373xC)
// HAL header for STM32F3xx devices
#include "stm32f3xx_hal.h"
#include "stm32_fs_platform.h"


#elif defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) || \
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) || \
    defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE) || defined(STM32F446xx) || \
    defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) || defined(STM32F412Vx) || \
    defined(STM32F412Rx) || defined(STM32F412Cx) || defined(STM32F413xx) || defined(STM32F423xx)
// HAL header for STM32F4xx devices
#include "stm32f4xx_hal.h"
#include "stm32_otg_platform.h"

#elif defined (STM32F756xx) || defined (STM32F746xx) || defined (STM32F745xx) || defined (STM32F767xx) || \
      defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx) || defined (STM32F722xx) || \
      defined (STM32F723xx) || defined (STM32F732xx) || defined (STM32F733xx)
// HAL header for STM32F7xx devices
#include "stm32f7xx_hal.h"
#include "stm32_otg_platform.h"

#elif  defined (STM32H743xx) || defined (STM32H753xx) || defined (STM32H750xx)
// HAL header for STM32H7xx devices
#include "stm32h7xx_hal.h"
#include "stm32_otg_platform.h"

#else

// Device unknown
#error  Device not supported
#endif

#include "board_config.h"


#endif

