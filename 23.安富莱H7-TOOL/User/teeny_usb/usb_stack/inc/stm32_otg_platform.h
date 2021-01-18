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

// teeny usb platform header for STM32F7 device
#ifndef __STM32_OTG_PLATFORM_H__
#define __STM32_OTG_PLATFORM_H__

#include "tusb_def.h"
#define PCD_ENDP0                                                     ((uint8_t)0)
#define PCD_ENDP1                                                     ((uint8_t)1)
#define PCD_ENDP2                                                     ((uint8_t)2)
#define PCD_ENDP3                                                     ((uint8_t)3)
#define PCD_ENDP4                                                     ((uint8_t)4)
#define PCD_ENDP5                                                     ((uint8_t)5)
#define PCD_ENDP6                                                     ((uint8_t)6)
#define PCD_ENDP7                                                     ((uint8_t)7)
#define PCD_ENDP8                                                     ((uint8_t)8)
#define PCD_ENDP9                                                     ((uint8_t)9)

#define USB_EP_CONTROL                                                USBD_EP_TYPE_CTRL
#define USB_EP_ISOCHRONOUS                                            USBD_EP_TYPE_ISOC
#define USB_EP_BULK                                                   USBD_EP_TYPE_BULK
#define USB_EP_INTERRUPT                                              USBD_EP_TYPE_INTR

#if defined(STM32F1)
    #define USB_OTG_FS_MAX_EP_NUM   4
    #define USB_OTG_FS_MAX_CH_NUM   8
    #define USB_OTG_HS_MAX_EP_NUM   0
    #define USB_OTG_HS_MAX_CH_NUM   0
    #define MAX_HC_NUM              8



#elif defined(STM32F4) || defined(STM32F2)
    #define USB_OTG_FS_MAX_EP_NUM   4
    #define USB_OTG_HS_MAX_EP_NUM   6
    #define USB_OTG_FS_MAX_CH_NUM   8
    #define USB_OTG_HS_MAX_CH_NUM   12
    #define MAX_HC_NUM              12


#elif defined(STM32F7)
    #define USB_OTG_FS_MAX_EP_NUM   6
    #define USB_OTG_HS_MAX_EP_NUM   9
    #define USB_OTG_FS_MAX_CH_NUM   12
    #define USB_OTG_HS_MAX_CH_NUM   16
    #define MAX_HC_NUM              16

#elif defined(STM32H7)
    #define USB_OTG_FS_MAX_EP_NUM   9
    #define USB_OTG_HS_MAX_EP_NUM   9
    #define USB_OTG_FS_MAX_CH_NUM   16
    #define USB_OTG_HS_MAX_CH_NUM   16
    #define MAX_HC_NUM              16

    #define  GPIO_AF10_OTG_FS  GPIO_AF10_OTG1_FS
    #define  GPIO_AF10_OTG_HS  GPIO_AF10_OTG2_HS

#else
    #error unsupport chip
#endif



#if defined(USB_OTG_FS) && defined(USB_OTG_HS)
    #define  USB_CORE_HANDLE_TYPE        PCD_TypeDef*
    #define  GetUSB(dev)           ((dev)->handle)
    #define  SetUSB(dev, usbx)     do{ (dev)->handle = usbx;  }while(0)

#else
    #if defined(USB_OTG_FS)
        #define  GetUSB(dev)           USB_OTG_FS
        #define  SetUSB(dev, usbx)

    #elif defined(USB_OTG_HS)
        #define  GetUSB(dev)           USB_OTG_HS
        #define  SetUSB(dev, usbx)

    #else
        #error neither otg_hs or otg_fs
    #endif
#endif


#define  USBx_BASE   ((uint32_t)USBx)

#if defined (NEED_MAX_PACKET)
    #undef NEED_MAX_PACKET
#endif

#define  GetInMaxPacket(dev, EPn)  get_max_in_packet_size(GetUSB(dev), EPn)
#define  GetOutMaxPacket(dev, EPn) get_max_out_packet_size(GetUSB(dev), EPn)

void flush_rx(USB_OTG_GlobalTypeDef *USBx);
void flush_tx(USB_OTG_GlobalTypeDef *USBx, uint32_t num);

#define init_ep_tx(dev, EPn, type, mps)                                                    \
do{                                                                                        \
  PCD_TypeDef *USBx = GetUSB(dev);                                                         \
  uint32_t maxpacket = mps;                                                                \
  USB_OTG_INEndpointTypeDef* INEp = USBx_INEP(EPn);                                        \
  if(USBx == USB_OTG_FS && EPn == 0){                                                      \
    maxpacket = __CLZ(maxpacket) - 25;                                                     \
  }                                                                                        \
  USBx_DEVICE->DAINTMSK |=  ( (USB_OTG_DAINTMSK_IEPM) & ( (1 << (EPn))) );                 \
  if (((INEp->DIEPCTL) & USB_OTG_DIEPCTL_USBAEP) == 0U)                                    \
  INEp->DIEPCTL |= ((maxpacket & USB_OTG_DIEPCTL_MPSIZ ) | (type << 18 ) |                 \
    ((EPn) << 22 ) | (USB_OTG_DIEPCTL_SD0PID_SEVNFRM) | (USB_OTG_DIEPCTL_USBAEP));         \
}while(0)

#define init_ep_rx(dev, EPn, type, mps)                                                    \
do{                                                                                        \
  PCD_TypeDef *USBx = GetUSB(dev);                                                         \
  uint32_t maxpacket = mps;                                                                \
  if(EPn == 0){                                                                            \
    maxpacket = __CLZ(maxpacket) - 25;                                                     \
  }                                                                                        \
  USBx_DEVICE->DAINTMSK |=  ( (USB_OTG_DAINTMSK_OEPM) & ( (1 << (EPn))<<16 ) );            \
  if (((USBx_OUTEP(EPn)->DOEPCTL) & USB_OTG_DOEPCTL_USBAEP) == 0U)                         \
  USBx_OUTEP(EPn)->DOEPCTL |= ((maxpacket & USB_OTG_DOEPCTL_MPSIZ ) | (type << 18 ) |      \
    (USB_OTG_DOEPCTL_SD0PID_SEVNFRM) | (USB_OTG_DIEPCTL_USBAEP));                          \
}while(0)

#define INIT_EP_Tx(dev, bEpNum, type, maxpacket)  \
do{\
  init_ep_tx(dev, bEpNum, type, maxpacket);\
}while(0)

#define INIT_EP_Rx(dev, bEpNum, type, maxpacket) \
do{\
  init_ep_rx(dev, bEpNum, type, maxpacket);\
}while(0)

// In OTG core, a fifo is used to handle the rx/tx data, all rx ep share a same fifo, each tx ep has a fifo

#define  SET_TX_FIFO(dev, bEpNum, addr, size) \
do{\
  if(bEpNum == 0){\
    GetUSB(dev)->DIEPTXF0_HNPTXFSIZ = (uint32_t)(((uint32_t)( (size)/4) << 16) | ( (addr)/4));\
  }else{\
                       /*avoid the compiler warning */\
    GetUSB(dev)->DIEPTXF[bEpNum==0?0: bEpNum- 1] = (uint32_t)(((uint32_t)( (size)/4) << 16) | ( (addr)/4));\
  }\
}while(0)

#define  SET_RX_FIFO(dev, addr, size) \
  do{\
    GetUSB(dev)->GRXFSIZ = (size/4);\
  }while(0)

#endif

