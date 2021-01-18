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

// teeny usb platform header for STM32F1/STM32F0 device
#ifndef __STM32_FS_PLATFORM_H__
#define __STM32_FS_PLATFORM_H__

#include "tusb_def.h"

#if defined(STM32F0) || defined(STM32F302xE) || defined(STM32F303xE) || defined(STM32F302x8)
// Define PMA buffer layout for STM32F0xx
typedef struct _pma_data{
  uint16_t data;
}pma_data;

typedef struct _pma_record{
  uint16_t    addr;       // TX/RX address
  uint16_t    cnt:10;     // TX/RX data count
  uint16_t    block:6;    // RX buffer size
}pma_record;

#elif defined(STM32F1) || defined(STM32F302xC) || defined(STM32F303xC) || defined(STM32F373xC)
// Define PMA buffer layout for STM32F1xx
typedef struct _pma_data{
  uint16_t data;
  uint16_t padding;
}pma_data;

typedef struct _pma_record{
  uint16_t    addr;       // TX/RX address
  uint16_t padding1;
  uint16_t    cnt:10;     // TX/RX data count
  uint16_t    block:6;    // RX buffer size
  uint16_t padding2;
}pma_record;
#endif

// There is only one USB device in STM32F0/STM32F1, just return the USB directly
#define  GetUSB(dev)           (USB)


#ifndef NEED_MAX_PACKET
#define NEED_MAX_PACKET
#endif
#define  GetInMaxPacket(dev, EPn)   ((dev)->tx_max_size[(EPn)])
#define  GetOutMaxPacket(dev, EPn)  ((dev)->rx_max_size[(EPn)])


#define  PMATable(dev)     ( (pma_data*) ((uint32_t)GetUSB(dev) + 0x400U) )
#define  GetPMAAddr(dev, offset)    \
  (PMATable(dev) + ( (offset) / sizeof(((pma_data*)0)->data)) )

#ifndef BTABLE_ADDRESS
#define BTABLE_ADDRESS (0)
#endif
// BTABLE address will not change in this kind device, so just return the fixed value
#define GetBTABLE(dev)  (BTABLE_ADDRESS)

typedef union _pma_ep_desc {
  struct _normal{
    pma_record     tx;
    pma_record     rx;
  }normal;
  struct _dbl_tx{
    pma_record     tx0;
    pma_record     tx1;
  }dbl_tx;
  struct _dlb_rx{
    pma_record     rx0;
    pma_record     rx1;
  }dbl_rx;
}pma_ep_desc;

// easy way to aceess the field
#define  pma_tx        normal.tx
#define  pma_rx        normal.rx
#define  pma_tx0       dbl_tx.tx0
#define  pma_tx1       dbl_tx.tx1
#define  pma_rx0       dbl_rx.rx0
#define  pma_rx1       dbl_rx.rx1

#define  tx_addr   normal.tx.addr
#define  tx_cnt    normal.tx.cnt
#define  rx_addr   normal.rx.addr
#define  rx_cnt    normal.rx.cnt
#define  rx_block  normal.rx.block

#define  tx0_addr   dbl_tx.tx0.addr
#define  tx0_cnt    dbl_tx.tx0.cnt
#define  tx1_addr   dbl_tx.tx1.addr
#define  tx1_cnt    dbl_tx.tx1.cnt

#define  rx0_addr   dbl_rx.rx0.addr
#define  rx0_cnt    dbl_rx.rx0.cnt
#define  rx0_block  dbl_rx.rx0.block

#define  rx1_addr   dbl_rx.rx1.addr
#define  rx1_cnt    dbl_rx.rx1.cnt
#define  rx1_block  dbl_rx.rx1.block


#define PMA_DESC(dev)    ((pma_ep_desc*) (GetBTABLE(dev) + PMATable(dev)) )
#define EPT(dev, bEpNum)  (PMA_DESC(dev)[bEpNum])


#define  PMA_TX(dev, bEpNum)   (&(PMA_DESC(dev)[bEpNum].pma_tx))
#define  PMA_RX(dev, bEpNum)   (&(PMA_DESC(dev)[bEpNum].pma_rx))
#define  PMA_TX0(dev, bEpNum)  (&(PMA_DESC(dev)[bEpNum].pma_tx0))
#define  PMA_TX1(dev, bEpNum)  (&(PMA_DESC(dev)[bEpNum].pma_tx1))
#define  PMA_RX0(dev, bEpNum)  (&(PMA_DESC(dev)[bEpNum].pma_rx0))
#define  PMA_RX1(dev, bEpNum)  (&(PMA_DESC(dev)[bEpNum].pma_rx1))

#define  SET_TX_ADDR(dev, bEpNum, addr)  do { EPT(dev, bEpNum).tx_addr = (addr); }while(0)

#define  SET_RX_ADDR(dev, bEpNum, addr)  do { EPT(dev, bEpNum).rx_addr = (addr); }while(0)

#define  SET_DOUBLE_ADDR(dev, bEpNum, addr0, addr1) \
  do{\
    EPT(dev, bEpNum).tx0_addr = addr0;\
    EPT(dev, bEpNum).tx1_addr = addr1;\
  }while(0)

#define  SET_RX_CNT(dev, bEpNum, cnt)\
  do{\
      uint16_t block = (cnt)>62 ? ((((cnt)+31)/32) | (1<<5)): (((cnt)+1)/2);\
      EPT(dev, bEpNum).rx_cnt = 0;\
      EPT(dev, bEpNum).rx_block = block;\
  }while(0)
#define SET_TX_CNT(dev, bEpNum, cnt) \
  do{  EPT(dev, bEpNum).tx_cnt = (cnt); }while(0)
  
#define SET_DBL_TX_CNT(dev, bEpNum, cnt)\
  do{\
    EPT(dev, bEpNum).tx0_cnt = (cnt);\
    EPT(dev, bEpNum).tx1_cnt = (cnt);\
  }while(0)

#define SET_DBL_RX_CNT(dev, bEpNum, cnt)\
  do{\
    uint16_t block = (cnt)>62 ? ((((cnt)+31)/32) | (1<<5)): (((cnt)+1)/2);\
    EPT(dev, bEpNum).rx0_cnt = 0;\
    EPT(dev, bEpNum).rx0_block = block;\
    EPT(dev, bEpNum).rx1_cnt = 0;\
    EPT(dev, bEpNum).rx1_block = block;\
  }while(0)

  
// Macro used to build ep setting, Tx/Rx CTR not changed, Tx/Rx TOG bits forced to 0
#define  BUILD_EP_SETTING(dev, bEpNum, type, rx_state, tx_state, kind)\
  /* Get end point current value */  \
 ((( PCD_GET_ENDPOINT(GetUSB(dev), bEpNum)      \
  /* TOG bits is write 1 to toggle, mask them with current value */\
  /* then write them back will set the TOG bits to 0*/\
   & (USB_EP_DTOG_RX | USB_EP_DTOG_TX | USB_EPTX_STAT | USB_EPRX_STAT)) \
  /* tx/rx state is wirte 1 to toggle, xor them with current value */ \
  /* then write them back, will set them to desired value */ \
   ^ ((tx_state) | (rx_state)) ) \
  /* Write 1 to CTR has no effect, ored them with 1 makes no change */ \
  | (USB_EP_CTR_RX|USB_EP_CTR_TX)     \
  /* Write ep num, tpye and kind to the register */ \
  | (bEpNum) | (type) | (kind) )

// Init ep in tx/tx mode, and set tx to NAK, set rx to VALID
#define  INIT_EP_BiDirection(dev, bEpNum, type) \
  PCD_SET_ENDPOINT(GetUSB(dev), bEpNum,  BUILD_EP_SETTING(dev, bEpNum, type, USB_EP_RX_VALID, USB_EP_TX_NAK, 0) )

// Init ep in tx mode, and set tx to NAK state
#define  INIT_EP_TxOnly(dev, bEpNum, type) \
  PCD_SET_ENDPOINT(GetUSB(dev), bEpNum,  BUILD_EP_SETTING(dev, bEpNum, type, USB_EP_RX_DIS, USB_EP_TX_NAK, 0) )

// Init ep in rx mode, and set rx to VALID state
#define  INIT_EP_RxOnly(dev, bEpNum, type) \
  PCD_SET_ENDPOINT(GetUSB(dev), bEpNum,  BUILD_EP_SETTING(dev, bEpNum, type, USB_EP_RX_VALID, USB_EP_TX_DIS, 0) )

// Init ep in tx double buffer mode, and set tx to NAK state
#define  INIT_EP_TxDouble(dev, bEpNum, type) \
  /* Keep to Rx/Tx TOG bit to 0, the double buffer tx ep is NAK even tx state is VALID */ \
  PCD_SET_ENDPOINT(GetUSB(dev), bEpNum,  BUILD_EP_SETTING(dev, bEpNum, type, USB_EP_RX_DIS, USB_EP_TX_VALID, USB_EP_KIND) )

// Init ep in rx double buffer mode, and set rx to VALID state
#define  INIT_EP_RxDouble(dev, bEpNum, type) \
  PCD_SET_ENDPOINT(GetUSB(dev), bEpNum,  BUILD_EP_SETTING(dev, bEpNum, type, USB_EP_RX_VALID, USB_EP_TX_DIS, USB_EP_KIND) \
  /* Toggle the SW_BUF bits, to make sure the rx state is real valid */ \
  ^ USB_EP_DTOG_TX ) 


// Teeny USB style tog, set, clear macros, they all required old ep value
#define  TUSB_RX_DTOG(USBx, EPn, EPOld) \
  PCD_SET_ENDPOINT(USBx, EPn, (EPOld & USB_EPREG_MASK) | (USB_EP_CTR_RX|USB_EP_CTR_TX|USB_EP_DTOG_RX) )
  
#define  TUSB_TX_DTOG(USBx, EPn, EPOld) \
  PCD_SET_ENDPOINT(USBx, EPn, (EPOld & USB_EPREG_MASK) | (USB_EP_CTR_RX|USB_EP_CTR_TX|USB_EP_DTOG_TX) )

#define   TUSB_SET_TX_STATUS(USBx, EPn, EPOld, Status)  \
  PCD_SET_ENDPOINT( USBx, EPn,  \
    ((EPOld & USB_EPTX_DTOGMASK) ^ Status) | USB_EP_CTR_RX|USB_EP_CTR_TX)

#define   TUSB_SET_RX_STATUS(USBx, EPn, EPOld, Status)  \
  PCD_SET_ENDPOINT( USBx, EPn,  \
    ((EPOld & USB_EPRX_DTOGMASK) ^ Status) | USB_EP_CTR_RX|USB_EP_CTR_TX)

#define   TUSB_CLEAR_TX_CTR(USBx, EPn, EPOld) \
  PCD_SET_ENDPOINT((USBx), (EPn), \
    EPOld & 0xFF7FU & USB_EPREG_MASK)

#define   TUSB_CLEAR_RX_CTR(USBx, EPn, EPOld) \
  PCD_SET_ENDPOINT((USBx), (EPn), \
    EPOld & 0x7FFFU & USB_EPREG_MASK)

#endif

