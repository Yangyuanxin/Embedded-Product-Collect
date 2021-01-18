/////////////////////////////////////////////////////////////
//// Auto generate by TeenyDT, http://dt.tusb.org
/////////////////////////////////////////////////////////////
#ifndef __COMP_TEENY_USB_INIT_H__
#define __COMP_TEENY_USB_INIT_H__
// forward declare the tusb_descriptors struct
typedef struct _tusb_descriptors tusb_descriptors;

#define COMP_VID                                            0x0d28
#define COMP_PID                                            0x0204
#define COMP_STRING_COUNT                                   (5)

// device.bmAttributes & 0x40   USB_CONFIG_SELF_POWERED
// device.bmAttributes & 0x20   USB_CONFIG_REMOTE_WAKEUP
#define COMP_DEV_STATUS                                    (0 | 0)


// Endpoint usage:
#define COMP_MAX_EP                                         (9)
#define COMP_EP_NUM                                         (COMP_MAX_EP + 1)

///////////////////////////////////////////////
//// Endpoint define for STM32 FS Core
///////////////////////////////////////////////

#ifdef COMP_BTABLE_ADDRESS
#undef COMP_BTABLE_ADDRESS
#endif
#define COMP_BTABLE_ADDRESS                                 (0)
#define COMP_EP_BUF_DESC_TABLE_SIZE                         (8)
// PMA buffer reserved for buffer description table
#define COMP_USB_BUF_START                                  (COMP_EP_BUF_DESC_TABLE_SIZE * COMP_EP_NUM)

// EndPoints 0 defines
#define COMP_EP0_RX_SIZE                                    (64)
#define COMP_EP0_RX_ADDR                                    (COMP_USB_BUF_START + (0))
#define COMP_EP0_TX_SIZE                                    (64)
#define COMP_EP0_TX_ADDR                                    (COMP_USB_BUF_START + (64))
#define COMP_EP0_RX_TYPE                                    USB_EP_CONTROL
#define COMP_EP0_TX_TYPE                                    USB_EP_CONTROL

#define COMP_EP0_TYPE                                       USB_EP_CONTROL
#define COMP_EP0_TX0_ADDR                                   (COMP_USB_BUF_START + (0))
#define COMP_EP0_TX1_ADDR                                   (COMP_USB_BUF_START + (64))
#define COMP_EP0_RX0_ADDR                                   (COMP_USB_BUF_START + (0))
#define COMP_EP0_RX1_ADDR                                   (COMP_USB_BUF_START + (64))

// EndPoints 1 defines
#define COMP_EP1_RX_SIZE                                    (64)
#define COMP_EP1_RX_ADDR                                    (COMP_USB_BUF_START + (128))
#define COMP_EP1_TX_SIZE                                    (64)
#define COMP_EP1_TX_ADDR                                    (COMP_USB_BUF_START + (192))
#define COMP_EP1_RX_TYPE                                    USB_EP_INTERRUPT
#define COMP_EP1_TX_TYPE                                    USB_EP_INTERRUPT

#define COMP_EP1_TYPE                                       USB_EP_INTERRUPT
#define COMP_EP1_TX0_ADDR                                   (COMP_USB_BUF_START + (128))
#define COMP_EP1_TX1_ADDR                                   (COMP_USB_BUF_START + (192))
#define COMP_EP1_RX0_ADDR                                   (COMP_USB_BUF_START + (128))
#define COMP_EP1_RX1_ADDR                                   (COMP_USB_BUF_START + (192))

// EndPoints 2 defines
#define COMP_EP2_RX_SIZE                                    (32)
#define COMP_EP2_RX_ADDR                                    (COMP_USB_BUF_START + (256))
#define COMP_EP2_TX_SIZE                                    (32)
#define COMP_EP2_TX_ADDR                                    (COMP_USB_BUF_START + (288))
#define COMP_EP2_RX_TYPE                                    USB_EP_BULK
#define COMP_EP2_TX_TYPE                                    USB_EP_BULK

#define COMP_EP2_TYPE                                       USB_EP_BULK
#define COMP_EP2_TX0_ADDR                                   (COMP_USB_BUF_START + (256))
#define COMP_EP2_TX1_ADDR                                   (COMP_USB_BUF_START + (288))
#define COMP_EP2_RX0_ADDR                                   (COMP_USB_BUF_START + (256))
#define COMP_EP2_RX1_ADDR                                   (COMP_USB_BUF_START + (288))

// EndPoints 3 defines
#define COMP_EP3_RX_SIZE                                    (0)
#define COMP_EP3_RX_ADDR                                    (COMP_USB_BUF_START + (320))
#define COMP_EP3_TX_SIZE                                    (0)
#define COMP_EP3_TX_ADDR                                    (COMP_USB_BUF_START + (320))
#define COMP_EP3_RX_TYPE                                    USB_EP_Invalid
#define COMP_EP3_TX_TYPE                                    USB_EP_Invalid

#define COMP_EP3_TYPE                                       USB_EP_Invalid
#define COMP_EP3_TX0_ADDR                                   (COMP_USB_BUF_START + (320))
#define COMP_EP3_TX1_ADDR                                   (COMP_USB_BUF_START + (320))
#define COMP_EP3_RX0_ADDR                                   (COMP_USB_BUF_START + (320))
#define COMP_EP3_RX1_ADDR                                   (COMP_USB_BUF_START + (320))

// EndPoints 4 defines
#define COMP_EP4_RX_SIZE                                    (32)
#define COMP_EP4_RX_ADDR                                    (COMP_USB_BUF_START + (320))
#define COMP_EP4_TX_SIZE                                    (32)
#define COMP_EP4_TX_ADDR                                    (COMP_USB_BUF_START + (352))
#define COMP_EP4_RX_TYPE                                    USB_EP_BULK
#define COMP_EP4_TX_TYPE                                    USB_EP_BULK

#define COMP_EP4_TYPE                                       USB_EP_BULK
#define COMP_EP4_TX0_ADDR                                   (COMP_USB_BUF_START + (320))
#define COMP_EP4_TX1_ADDR                                   (COMP_USB_BUF_START + (352))
#define COMP_EP4_RX0_ADDR                                   (COMP_USB_BUF_START + (320))
#define COMP_EP4_RX1_ADDR                                   (COMP_USB_BUF_START + (352))

// EndPoints 5 defines
#define COMP_EP5_RX_SIZE                                    (32)
#define COMP_EP5_RX_ADDR                                    (COMP_USB_BUF_START + (384))
#define COMP_EP5_TX_SIZE                                    (32)
#define COMP_EP5_TX_ADDR                                    (COMP_USB_BUF_START + (416))
#define COMP_EP5_RX_TYPE                                    USB_EP_BULK
#define COMP_EP5_TX_TYPE                                    USB_EP_BULK

#define COMP_EP5_TYPE                                       USB_EP_BULK
#define COMP_EP5_TX0_ADDR                                   (COMP_USB_BUF_START + (384))
#define COMP_EP5_TX1_ADDR                                   (COMP_USB_BUF_START + (416))
#define COMP_EP5_RX0_ADDR                                   (COMP_USB_BUF_START + (384))
#define COMP_EP5_RX1_ADDR                                   (COMP_USB_BUF_START + (416))

// EndPoints 6 defines
#define COMP_EP6_RX_SIZE                                    (32)
#define COMP_EP6_RX_ADDR                                    (COMP_USB_BUF_START + (448))
#define COMP_EP6_TX_SIZE                                    (32)
#define COMP_EP6_TX_ADDR                                    (COMP_USB_BUF_START + (480))
#define COMP_EP6_RX_TYPE                                    USB_EP_BULK
#define COMP_EP6_TX_TYPE                                    USB_EP_BULK

#define COMP_EP6_TYPE                                       USB_EP_BULK
#define COMP_EP6_TX0_ADDR                                   (COMP_USB_BUF_START + (448))
#define COMP_EP6_TX1_ADDR                                   (COMP_USB_BUF_START + (480))
#define COMP_EP6_RX0_ADDR                                   (COMP_USB_BUF_START + (448))
#define COMP_EP6_RX1_ADDR                                   (COMP_USB_BUF_START + (480))

// EndPoints 7 defines
#define COMP_EP7_RX_SIZE                                    (32)
#define COMP_EP7_RX_ADDR                                    (COMP_USB_BUF_START + (512))
#define COMP_EP7_TX_SIZE                                    (32)
#define COMP_EP7_TX_ADDR                                    (COMP_USB_BUF_START + (544))
#define COMP_EP7_RX_TYPE                                    USB_EP_BULK
#define COMP_EP7_TX_TYPE                                    USB_EP_BULK

#define COMP_EP7_TYPE                                       USB_EP_BULK
#define COMP_EP7_TX0_ADDR                                   (COMP_USB_BUF_START + (512))
#define COMP_EP7_TX1_ADDR                                   (COMP_USB_BUF_START + (544))
#define COMP_EP7_RX0_ADDR                                   (COMP_USB_BUF_START + (512))
#define COMP_EP7_RX1_ADDR                                   (COMP_USB_BUF_START + (544))

// EndPoints 8 defines
#define COMP_EP8_RX_SIZE                                    (32)
#define COMP_EP8_RX_ADDR                                    (COMP_USB_BUF_START + (576))
#define COMP_EP8_TX_SIZE                                    (32)
#define COMP_EP8_TX_ADDR                                    (COMP_USB_BUF_START + (608))
#define COMP_EP8_RX_TYPE                                    USB_EP_BULK
#define COMP_EP8_TX_TYPE                                    USB_EP_BULK

#define COMP_EP8_TYPE                                       USB_EP_BULK
#define COMP_EP8_TX0_ADDR                                   (COMP_USB_BUF_START + (576))
#define COMP_EP8_TX1_ADDR                                   (COMP_USB_BUF_START + (608))
#define COMP_EP8_RX0_ADDR                                   (COMP_USB_BUF_START + (576))
#define COMP_EP8_RX1_ADDR                                   (COMP_USB_BUF_START + (608))

// EndPoints 9 defines
#define COMP_EP9_RX_SIZE                                    (0)
#define COMP_EP9_RX_ADDR                                    (COMP_USB_BUF_START + (640))
#define COMP_EP9_TX_SIZE                                    (16)
#define COMP_EP9_TX_ADDR                                    (COMP_USB_BUF_START + (640))
#define COMP_EP9_RX_TYPE                                    USB_EP_Invalid
#define COMP_EP9_TX_TYPE                                    USB_EP_INTERRUPT

#define COMP_EP9_TYPE                                       USB_EP_INTERRUPT
#define COMP_EP9_TX0_ADDR                                   (COMP_USB_BUF_START + (640))
#define COMP_EP9_TX1_ADDR                                   (COMP_USB_BUF_START + (656))
#define COMP_EP9_RX0_ADDR                                   (COMP_USB_BUF_START + (640))
#define COMP_EP9_RX1_ADDR                                   (COMP_USB_BUF_START + (656))


// EndPoint max packed sizes
extern const uint8_t COMP_txEpMaxSize[];
#define COMP_TXEP_MAX_SIZE                                  \
const uint8_t COMP_txEpMaxSize[] = \
{ COMP_EP0_TX_SIZE, COMP_EP1_TX_SIZE, COMP_EP2_TX_SIZE, 0, COMP_EP4_TX_SIZE, COMP_EP5_TX_SIZE, COMP_EP6_TX_SIZE, COMP_EP7_TX_SIZE, COMP_EP8_TX_SIZE, COMP_EP9_TX_SIZE,  };
extern const uint8_t COMP_rxEpMaxSize[];
#define COMP_RXEP_MAX_SIZE                                  \
const uint8_t COMP_rxEpMaxSize[] = \
{ COMP_EP0_RX_SIZE, COMP_EP1_RX_SIZE, COMP_EP2_RX_SIZE, 0, COMP_EP4_RX_SIZE, COMP_EP5_RX_SIZE, COMP_EP6_RX_SIZE, COMP_EP7_RX_SIZE, COMP_EP8_RX_SIZE, 0,  };

// EndPoints init function for USB FS core
#define COMP_TUSB_INIT_EP_FS(dev) \
  do{\
    /* Init ep0 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP0, COMP_EP0_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP0, COMP_EP0_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP0, COMP_EP0_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP0, COMP_EP0_RX_SIZE);  \
    /* Init ep1 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP1, COMP_EP1_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP1, COMP_EP1_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP1, COMP_EP1_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP1, COMP_EP1_RX_SIZE);  \
    /* Init ep2 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP2, COMP_EP2_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP2, COMP_EP2_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP2, COMP_EP2_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP2, COMP_EP2_RX_SIZE);  \
    /* Init ep3 */ \
    /* Init ep4 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP4, COMP_EP4_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP4, COMP_EP4_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP4, COMP_EP4_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP4, COMP_EP4_RX_SIZE);  \
    /* Init ep5 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP5, COMP_EP5_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP5, COMP_EP5_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP5, COMP_EP5_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP5, COMP_EP5_RX_SIZE);  \
    /* Init ep6 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP6, COMP_EP6_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP6, COMP_EP6_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP6, COMP_EP6_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP6, COMP_EP6_RX_SIZE);  \
    /* Init ep7 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP7, COMP_EP7_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP7, COMP_EP7_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP7, COMP_EP7_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP7, COMP_EP7_RX_SIZE);  \
    /* Init ep8 */ \
    INIT_EP_BiDirection(dev, PCD_ENDP8, COMP_EP8_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP8, COMP_EP8_TX_ADDR);  \
    SET_RX_ADDR(dev, PCD_ENDP8, COMP_EP8_RX_ADDR);  \
    SET_RX_CNT(dev, PCD_ENDP8, COMP_EP8_RX_SIZE);  \
    /* Init ep9 */ \
    INIT_EP_TxOnly(dev, PCD_ENDP9, COMP_EP9_TYPE);  \
    SET_TX_ADDR(dev, PCD_ENDP9, COMP_EP9_TX_ADDR);  \
}while(0)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG Core
///////////////////////////////////////////////
#define COMP_OTG_MAX_OUT_SIZE                               (64)
#define COMP_OTG_CONTROL_EP_NUM                             (1)
#define COMP_OTG_OUT_EP_NUM                                 (7)
// RX FIFO size / 4 > (CONTROL_EP_NUM * 5 + 8) +  (MAX_OUT_SIZE / 4 + 1) + (OUT_EP_NUM*2) + 1 = 45

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG FS Core
///////////////////////////////////////////////
#define COMP_OTG_RX_FIFO_SIZE_FS                            (256)
#define COMP_OTG_RX_FIFO_ADDR_FS                            (0)
// Sum of IN ep max packet size is 336
// Remain Fifo size is 1024 in bytes, Rx Used 256 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define COMP_EP0_TX_FIFO_ADDR_FS                            (256)
#define COMP_EP0_TX_FIFO_SIZE_FS                            (COMP_EP0_TX_SIZE * 3)
#define COMP_EP1_TX_FIFO_ADDR_FS                            (448)
#define COMP_EP1_TX_FIFO_SIZE_FS                            (COMP_EP1_TX_SIZE * 3)
#define COMP_EP2_TX_FIFO_ADDR_FS                            (640)
#define COMP_EP2_TX_FIFO_SIZE_FS                            (COMP_EP2_TX_SIZE * 3)
#define COMP_EP4_TX_FIFO_ADDR_FS                            (736)
#define COMP_EP4_TX_FIFO_SIZE_FS                            (COMP_EP4_TX_SIZE * 3)
#define COMP_EP5_TX_FIFO_ADDR_FS                            (832)
#define COMP_EP5_TX_FIFO_SIZE_FS                            (COMP_EP5_TX_SIZE * 3)
#define COMP_EP6_TX_FIFO_ADDR_FS                            (928)
#define COMP_EP6_TX_FIFO_SIZE_FS                            (COMP_EP6_TX_SIZE * 3)
#define COMP_EP7_TX_FIFO_ADDR_FS                            (1024)
#define COMP_EP7_TX_FIFO_SIZE_FS                            (COMP_EP7_TX_SIZE * 3)
#define COMP_EP8_TX_FIFO_ADDR_FS                            (1120)
#define COMP_EP8_TX_FIFO_SIZE_FS                            (COMP_EP8_TX_SIZE * 3)
#define COMP_EP9_TX_FIFO_ADDR_FS                            (1216)
#define COMP_EP9_TX_FIFO_SIZE_FS                            (COMP_EP9_TX_SIZE * 3)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_FS)
#define COMP_TUSB_INIT_EP_OTG_FS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_FS) { \
      SET_RX_FIFO(dev, COMP_OTG_RX_FIFO_ADDR_FS, COMP_OTG_RX_FIFO_SIZE_FS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, COMP_EP0_TX_TYPE, COMP_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, COMP_EP0_TX_FIFO_ADDR_FS, COMP_EP0_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, COMP_EP0_RX_TYPE, COMP_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, COMP_EP1_TX_TYPE, COMP_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, COMP_EP1_TX_FIFO_ADDR_FS, COMP_EP1_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP1, COMP_EP1_RX_TYPE, COMP_EP1_RX_SIZE); \
      /* Init Ep2  */\
      INIT_EP_Tx(dev, PCD_ENDP2, COMP_EP2_TX_TYPE, COMP_EP2_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP2, COMP_EP2_TX_FIFO_ADDR_FS, COMP_EP2_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP2, COMP_EP2_RX_TYPE, COMP_EP2_RX_SIZE); \
      /* Init Ep4  */\
      INIT_EP_Tx(dev, PCD_ENDP4, COMP_EP4_TX_TYPE, COMP_EP4_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP4, COMP_EP4_TX_FIFO_ADDR_FS, COMP_EP4_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP4, COMP_EP4_RX_TYPE, COMP_EP4_RX_SIZE); \
      /* Init Ep5  */\
      INIT_EP_Tx(dev, PCD_ENDP5, COMP_EP5_TX_TYPE, COMP_EP5_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP5, COMP_EP5_TX_FIFO_ADDR_FS, COMP_EP5_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP5, COMP_EP5_RX_TYPE, COMP_EP5_RX_SIZE); \
      /* Init Ep6  */\
      INIT_EP_Tx(dev, PCD_ENDP6, COMP_EP6_TX_TYPE, COMP_EP6_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP6, COMP_EP6_TX_FIFO_ADDR_FS, COMP_EP6_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP6, COMP_EP6_RX_TYPE, COMP_EP6_RX_SIZE); \
      /* Init Ep7  */\
      INIT_EP_Tx(dev, PCD_ENDP7, COMP_EP7_TX_TYPE, COMP_EP7_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP7, COMP_EP7_TX_FIFO_ADDR_FS, COMP_EP7_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP7, COMP_EP7_RX_TYPE, COMP_EP7_RX_SIZE); \
      /* Init Ep8  */\
      INIT_EP_Tx(dev, PCD_ENDP8, COMP_EP8_TX_TYPE, COMP_EP8_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP8, COMP_EP8_TX_FIFO_ADDR_FS, COMP_EP8_TX_FIFO_SIZE_FS);  \
      INIT_EP_Rx(dev, PCD_ENDP8, COMP_EP8_RX_TYPE, COMP_EP8_RX_SIZE); \
      /* Init Ep9  */\
      INIT_EP_Tx(dev, PCD_ENDP9, COMP_EP9_TX_TYPE, COMP_EP9_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP9, COMP_EP9_TX_FIFO_ADDR_FS, COMP_EP9_TX_FIFO_SIZE_FS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_FS)
#define COMP_TUSB_INIT_EP_OTG_FS(dev) 
    
#endif  // #if defined(USB_OTG_FS)

///////////////////////////////////////////////
//// Endpoint define for STM32 OTG HS Core
///////////////////////////////////////////////
#define COMP_OTG_RX_FIFO_SIZE_HS                            (512)
#define COMP_OTG_RX_FIFO_ADDR_HS                            (0)
// Sum of IN ep max packet size is 336
// Remain Fifo size is 3584 in bytes, Rx Used 512 bytes 

// TODO:
// I don't know why the max count of TX fifo should <= (7 * EpMaxPacket)
// But it seems the STM32F7xx can be large than (7 * EpMaxPacket)
#define COMP_EP0_TX_FIFO_ADDR_HS                            (512)
#define COMP_EP0_TX_FIFO_SIZE_HS                            (COMP_EP0_TX_SIZE * 7)
#define COMP_EP1_TX_FIFO_ADDR_HS                            (960)
#define COMP_EP1_TX_FIFO_SIZE_HS                            (COMP_EP1_TX_SIZE * 7)
#define COMP_EP2_TX_FIFO_ADDR_HS                            (1408)
#define COMP_EP2_TX_FIFO_SIZE_HS                            (COMP_EP2_TX_SIZE * 7)
#define COMP_EP4_TX_FIFO_ADDR_HS                            (1632)
#define COMP_EP4_TX_FIFO_SIZE_HS                            (COMP_EP4_TX_SIZE * 7)
#define COMP_EP5_TX_FIFO_ADDR_HS                            (1856)
#define COMP_EP5_TX_FIFO_SIZE_HS                            (COMP_EP5_TX_SIZE * 7)
#define COMP_EP6_TX_FIFO_ADDR_HS                            (2080)
#define COMP_EP6_TX_FIFO_SIZE_HS                            (COMP_EP6_TX_SIZE * 7)
#define COMP_EP7_TX_FIFO_ADDR_HS                            (2304)
#define COMP_EP7_TX_FIFO_SIZE_HS                            (COMP_EP7_TX_SIZE * 7)
#define COMP_EP8_TX_FIFO_ADDR_HS                            (2528)
#define COMP_EP8_TX_FIFO_SIZE_HS                            (COMP_EP8_TX_SIZE * 7)
#define COMP_EP9_TX_FIFO_ADDR_HS                            (2752)
#define COMP_EP9_TX_FIFO_SIZE_HS                            (COMP_EP9_TX_SIZE * 7)
// EndPoints init function for USB OTG core
#if defined(USB_OTG_HS)
#define COMP_TUSB_INIT_EP_OTG_HS(dev) \
  do{\
    if(GetUSB(dev) == USB_OTG_HS) { \
      SET_RX_FIFO(dev, COMP_OTG_RX_FIFO_ADDR_HS, COMP_OTG_RX_FIFO_SIZE_HS);  \
      /* Init Ep0  */\
      INIT_EP_Tx(dev, PCD_ENDP0, COMP_EP0_TX_TYPE, COMP_EP0_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP0, COMP_EP0_TX_FIFO_ADDR_HS, COMP_EP0_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP0, COMP_EP0_RX_TYPE, COMP_EP0_RX_SIZE); \
      /* Init Ep1  */\
      INIT_EP_Tx(dev, PCD_ENDP1, COMP_EP1_TX_TYPE, COMP_EP1_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP1, COMP_EP1_TX_FIFO_ADDR_HS, COMP_EP1_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP1, COMP_EP1_RX_TYPE, COMP_EP1_RX_SIZE); \
      /* Init Ep2  */\
      INIT_EP_Tx(dev, PCD_ENDP2, COMP_EP2_TX_TYPE, COMP_EP2_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP2, COMP_EP2_TX_FIFO_ADDR_HS, COMP_EP2_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP2, COMP_EP2_RX_TYPE, COMP_EP2_RX_SIZE); \
      /* Init Ep4  */\
      INIT_EP_Tx(dev, PCD_ENDP4, COMP_EP4_TX_TYPE, COMP_EP4_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP4, COMP_EP4_TX_FIFO_ADDR_HS, COMP_EP4_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP4, COMP_EP4_RX_TYPE, COMP_EP4_RX_SIZE); \
      /* Init Ep5  */\
      INIT_EP_Tx(dev, PCD_ENDP5, COMP_EP5_TX_TYPE, COMP_EP5_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP5, COMP_EP5_TX_FIFO_ADDR_HS, COMP_EP5_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP5, COMP_EP5_RX_TYPE, COMP_EP5_RX_SIZE); \
      /* Init Ep6  */\
      INIT_EP_Tx(dev, PCD_ENDP6, COMP_EP6_TX_TYPE, COMP_EP6_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP6, COMP_EP6_TX_FIFO_ADDR_HS, COMP_EP6_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP6, COMP_EP6_RX_TYPE, COMP_EP6_RX_SIZE); \
      /* Init Ep7  */\
      INIT_EP_Tx(dev, PCD_ENDP7, COMP_EP7_TX_TYPE, COMP_EP7_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP7, COMP_EP7_TX_FIFO_ADDR_HS, COMP_EP7_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP7, COMP_EP7_RX_TYPE, COMP_EP7_RX_SIZE); \
      /* Init Ep8  */\
      INIT_EP_Tx(dev, PCD_ENDP8, COMP_EP8_TX_TYPE, COMP_EP8_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP8, COMP_EP8_TX_FIFO_ADDR_HS, COMP_EP8_TX_FIFO_SIZE_HS);  \
      INIT_EP_Rx(dev, PCD_ENDP8, COMP_EP8_RX_TYPE, COMP_EP8_RX_SIZE); \
      /* Init Ep9  */\
      INIT_EP_Tx(dev, PCD_ENDP9, COMP_EP9_TX_TYPE, COMP_EP9_TX_SIZE);  \
      SET_TX_FIFO(dev, PCD_ENDP9, COMP_EP9_TX_FIFO_ADDR_HS, COMP_EP9_TX_FIFO_SIZE_HS);  \
    }\
  }while(0)

#else  // #if defined(USB_OTG_HS)
#define COMP_TUSB_INIT_EP_OTG_HS(dev) 
    
#endif  // #if defined(USB_OTG_HS)
#define COMP_TUSB_INIT_EP_OTG(dev) \
  do{\
    COMP_TUSB_INIT_EP_OTG_FS(dev); \
    COMP_TUSB_INIT_EP_OTG_HS(dev); \
  }while(0)


#if defined(USB)
#define COMP_TUSB_INIT_EP(dev) COMP_TUSB_INIT_EP_FS(dev)

// Teeny USB device init function for FS core
#define COMP_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = COMP_DEV_STATUS;         \
    dev->rx_max_size = COMP_rxEpMaxSize;         \
    dev->tx_max_size = COMP_txEpMaxSize;         \
    dev->descriptors = &COMP_descriptors;         \
  }while(0)

#endif

#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
#define COMP_TUSB_INIT_EP(dev) COMP_TUSB_INIT_EP_OTG(dev)

// Teeny USB device init function for OTG core
#define COMP_TUSB_INIT_DEVICE(dev) \
  do{\
    /* Init device features */       \
    memset(&dev->addr, 0, TUSB_DEVICE_SIZE);    \
    dev->status = COMP_DEV_STATUS;         \
    dev->descriptors = &COMP_descriptors;         \
  }while(0)

#endif

#define COMP_TUSB_INIT(dev) \
  do{\
    COMP_TUSB_INIT_EP(dev);   \
    COMP_TUSB_INIT_DEVICE(dev);   \
  }while(0)

// Get End Point count
#ifndef  EP_NUM
#define  EP_NUM 1
#endif
#if COMP_EP_NUM > EP_NUM
#undef   EP_NUM
#define  EP_NUM  COMP_EP_NUM
#endif

extern const uint8_t* const COMP_StringDescriptors[COMP_STRING_COUNT];
extern const tusb_descriptors COMP_descriptors;

#define COMP_REPORT_DESCRIPTOR_SIZE_IF0  (24)
extern const uint8_t COMP_ReportDescriptor_if0[COMP_REPORT_DESCRIPTOR_SIZE_IF0];

// Enable WCID related code
#define  HAS_WCID

#ifndef WCID_VENDOR_CODE
#define  WCID_VENDOR_CODE       0x17
extern const uint8_t WCID_StringDescriptor_MSOS[];
#endif


#endif   // #ifndef __COMP_TEENY_USB_INIT_H__
