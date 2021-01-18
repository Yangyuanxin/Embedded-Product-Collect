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

#ifndef __TEENY_USB_H__
#define __TEENY_USB_H__

#include "teeny_usb_platform.h"
#include "teeny_usb_init.h"

/* Public Interface - May be used in end-application: */

#define USB_FEATURE_B_HNP_ENABLE                3       // SET FEATURE OTG - Enable B device to perform HNP
#define USB_FEATURE_A_HNP_SUPPORT               4       // SET FEATURE OTG - A device supports HNP
#define USB_FEATURE_A_ALT_HNP_SUPPORT           5       // SET FEATURE OTG - Another port on the A device supports HNP

/* Macros: */

/**  Get the low byte in a uint16_t value */
#define LO_BYTE(x)   (uint8_t)(x)

/**  Get the high byte in a uint16_t value */
#define HI_BYTE(x)  (uint8_t)((x)>>8)

#ifndef EP_NUM
/**  Endpoint number used in TeenyUSB stack */
#define EP_NUM 1
#endif

/* Type Defines: */
/** Type define for setup packet
 */
typedef struct _tusb_setup_packet{
  uint8_t  bmRequestType;
  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} tusb_setup_packet;

/** Type define for endpoint data structure
 */
typedef struct _tusb_ep_data
{
  const uint8_t*  tx_buf;              /**< data transmit buffer for IN endpoint */
  uint8_t*        rx_buf;              /**< data receive buffer for OUT endpoint */
  uint16_t        tx_remain_size;      /**< data reamin size in transmit buffer */
  uint16_t        rx_size;             /**< data buffer total size in receive buffer */
  uint16_t        rx_count;            /**< current received data length */
  uint16_t        tx_last_size;        /**< last transmit data length */
  uint16_t        tx_need_zlp: 1;      /**< need transmit zero length packet */
  uint16_t        bit_field_padding:15;/**< padding to 16 bit boundary */
#if defined(USB)
  uint16_t        tx_pushed;           /**< data packet number in the PMA buffer */
#elif defined(USB_OTG_FS) || defined(USB_OTG_HS)
  uint16_t        tx_total_size;       /**< total transmit data length */
#else
  uint16_t        padding;             /**< padding to 32bit boundary */
#endif
}tusb_ep_data;

/** Type define for descriptor type
 */
typedef const uint8_t* desc_t;

/** Type define for device descriptor
 */
typedef struct _tusb_descriptors
{
  desc_t  device;            /**< device descriptor */
  desc_t  config;            /**< configuration descriptor */
  const desc_t*  strings;    /**< string descriptor array */
  uint32_t string_cnt;       /**< string descriptor count */
#if defined(HAS_WCID)
  desc_t  wcid_desc;         /**< WCID descriptor for windows campatible device */
  const desc_t*  wcid_properties;   /**< WCID properties descriptor */
#endif
#if defined(HAS_WCID_20)
  desc_t  wcid_bos;
  desc_t  wcid_desc_set;
#endif
}tusb_descriptors;

/** Type define for usb core
 */
typedef struct _tusb_core
{
#if defined(USB_CORE_HANDLE_TYPE)
  USB_CORE_HANDLE_TYPE  handle;           /**< USB hardware handle */
#else
  void*                 handle_not_used;  /**< fix compile issue */
#endif
}tusb_core_t;


typedef struct _tusb_device tusb_device_t; /**< forward declaration of \ref tusb_device_t */

/** Type define for endpoint 0 callback function
 */
typedef void(*tusb_callback_t)(tusb_device_t* dev);

/** Type define for usb device
 */
struct _tusb_device{
#if defined(USB_CORE_HANDLE_TYPE)
  USB_CORE_HANDLE_TYPE  handle;           /**< USB hardware handle */
#endif
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
  uint8_t*  desc_buffer;                  /**< descriptor buffer, used for qualifier or DMA */
#endif
  void*     user_data;                    /**<  User data for device                        */
  uint8_t   addr;                         /**< device address                               */
  uint8_t   config;                       /**< device current configurtaion ID              */
  uint8_t   alt_cfg;                      /**< device interface alernate configuration      */
  uint8_t   remote_wakeup:1;              /**< device feature of remote wakeup              */
  uint8_t   b_hnp:1;                      /**< OTG only, hnp for B device                   */
  uint8_t   a_hnp:1;                      /**< OTG only, hnp for A device                   */
  uint8_t   a_alt_hnp:1;                  /**< OTG only, hnp for A device on other port     */
  uint8_t   flag_padding:4;               /**< padding the bit flags                        */
  uint16_t  status;                       /**< device status                                */
  uint16_t  padding;                      /**< padding struct to 32bit boundary             */
  tusb_callback_t ep0_tx_done;            /**< endpoint 0 transmit done callback            */
  tusb_callback_t ep0_rx_done;            /**< endpoint 0 receive done callback             */
#if defined(NEED_MAX_PACKET)
  const uint8_t* rx_max_size;             /**< RX/OUT endpoint max packet size buffer       */
  const uint8_t* tx_max_size;             /**< TX/IN endpoint max packet size buffer        */
#endif
  tusb_setup_packet  setup;               /**< setup packet buffer                           */
  tusb_ep_data   Ep[EP_NUM];              /**< end point                                     */
  const tusb_descriptors*  descriptors;   /**< device descriptors, can be override at runtime */
};

/* Macros: */
/** device structure data size, started from the addr field */
#define  TUSB_DEVICE_SIZE   (sizeof(tusb_device_t) -  (uint32_t) (&((tusb_device_t*)0)->addr) )

/** Flag used to send data, to indicate that the core will send zero length packet as need */
#define  TUSB_TXF_ZLP       ((uint8_t)0x80)
/** Flag used to send data, to indicate that the core will send 1 packet pre frame in high speed periodic endpoint */
#define  TUSB_TXF_MC1       ((uint8_t)0x00)
/** Flag used to send data, to indicate that the core will send 2 packet pre frame in high speed periodic endpoint */
#define  TUSB_TXF_MC2       ((uint8_t)0x01)
/** Flag used to send data, to indicate that the core will send 3 packet pre frame in high speed periodic endpoint */
#define  TUSB_TXF_MC3       ((uint8_t)0x02)


/** Host channel retry forever */
#define  RETRY_FOREVER              0xffffffff

#ifndef  MAX_HC_NUM
/** Max host channel number */
#define  MAX_HC_NUM                    16
#endif

/** USB port speed high speed */
#define PORT_SPEED_HIGH                0U
/** USB port speed full speed */
#define PORT_SPEED_FULL                1U
/** USB port speed low speed */
#define PORT_SPEED_LOW                 2U

/** USB host channel max retry time when error occur */
#define MAX_ERROR_RETRY_TIME           16
/** USB host channel max retry time when get NAK, 
    this value is used to avoid infinite wait in Bulk/Control transfer */
#define MAX_NAK_RETRY_TIME             0x160000

/* Type Defines: */
/** Type define for Host channel data */
typedef struct _tusb_hc_data
{
  uint8_t*  ch_buf;                  /**< channel data buffer                                              */
  uint16_t  size;                    /**< total size of the channel buffer                                 */
  uint16_t  count;                   /**< current xfered data count of the channel buffer                  */
  uint32_t  nak_count;               /**< channel NAK count                                                */
  uint32_t  error_count;             /**< channel error count                                              */
  uint32_t  error_reason;            /**< channel error reason                                             */
  uint8_t   state;                   /**< channel state   \ref  channel_state_t                            */
  uint8_t   toggle_in:1;             /**< toggle bit for IN pipe                                           */
  uint8_t   toggle_out:1;            /**< toggle bit for OUT pipe                                          */
  uint8_t   do_ping:1;               /**< do ping flag                                                     */
  uint8_t   is_use:1;                /**< used flag:  1-channel is used, 0-channel is free                 */
  uint8_t   is_data:1;               /**< Channel data type flag: 1-data packet, 0-setup packet            */
  uint8_t   is_cancel:1;             /**< Cancel current transfer                                          */
  uint8_t   xfer_done:1;             /**< 1: xfer done, 0: xfer on goning                                  */
  uint8_t   padding:1;               /**< bit field padding                                                */
  uint8_t   speed;                   /**< port speed,  \ref PORT_SPEED_HIGH, \ref PORT_SPEED_FULL, \ref PORT_SPEED_LOW */
  uint8_t   padding32;               /**< padding to 32bit boundary                                        */
  void*     user_data;               /**< User data for host channel                                       */
}tusb_hc_data_t;

/** Type define for USB host */
typedef struct _tusb_host{
#if defined(USB_CORE_HANDLE_TYPE)
  USB_CORE_HANDLE_TYPE  handle;           /**< USB hardware handle */
#endif
  void*           user_data;              /**< User data for host */
  uint32_t        state;                  /**< current host state */
  tusb_hc_data_t  hc[MAX_HC_NUM];         /**< host channel array */
  uint32_t        nptx_pending;           /**< Non periodic pending trasmit/out pipe */
  uint32_t        ptx_pending;            /**< Periodic pending trasmit/out pipe     */
}tusb_host_t;

/** Type define for USB OTG */
typedef struct _tusb_otg
{
  tusb_device_t device;                   /**<  Device mode data */
  tusb_host_t   host;                     /**<  Host mode data */
}tusb_otg_t;

/** Type define for USB host pipe */
typedef struct _tusb_pipe
{
  tusb_host_t* host;                     /**< pipe host */
  uint8_t      hc_num;                   /**< pipe channel number */
}tusb_pipe_t;

/* Enums: */
/** Enums for \ref tusb_host_port_changed  new_state parameter
 *  indicating the port state
 */
typedef enum {
  TUSB_HOST_PORT_DUMMY = 0,     /**< Initial state */
  TUSB_HOST_PORT_DISCONNECTED,  /**< Host port is disconnected */
  TUSB_HOST_PORT_CONNECTED,     /**< Host port connected */
  TUSB_HOST_PORT_ENABLED,       /**< Host port enabled, this is done by fire a reset when in connected state */
  TUSB_HOST_PORT_DISABLED,      /**< Host port disabled */
}host_port_state_t;

/** Enums for \ref tusb_hc_data_t state field
 *  indicating the host channel state
 */
typedef enum {
  TUSB_CS_INIT = 0,                 /**< Initial state */
  TUSB_CS_TRANSFER_COMPLETE = 1,    /**< Channel transfer complete */
  TUSB_CS_NAK,                      /**< Channel naked */
  TUSB_CS_ACK,                      /**< Channel acked */
  TSUB_CS_PING_SUCCESS,             /**< Channel ping acked */
  TUSB_CS_NYET,                     /**< Channel ping not yet */
  TUSB_CS_STALL,                    /**< Channel stalled */
  TUSB_CS_INT_NAK,                  /**< Interrupt channel naked */
  
  TUSB_CS_AHB_ERROR = 10,           /**< Channel AHB error */
  TUSB_CS_DT_ERROR,                 /**< Channel data toggle error */
  TUSB_CS_TRANSACTION_ERROR,        /**< Channel transaction error, CRC/Bit stuff/timeout/False EOP */
  TUSB_CS_FRAMEOVERRUN_ERROR,       /**< Channel frame overrun */
  TUSB_CS_BABBLE_ERROR,             /**< Channel babble error */
  
  TUSB_CS_XFER_ONGOING,             /**< Channel data transfer is on going */
  TUSB_CS_XFER_CANCEL,              /**< Channel data transfer is canceled */
  TUSB_CS_UNKNOWN_ERROR,            /**< Channel Unknown error */
}channel_state_t;

/** Enums for ID line state
 */
typedef enum{
  TUSB_ID_A = 0,                     /**< ID line is A/Host role */
  TUSB_ID_B = 1,                     /**< ID line is B/Device role */
}id_state_t;

/* Function Prototypes: */

//////////////////////////////////////////////
// Global functions,  used in all mode
//////////////////////////////////////////////
/** Delay millisecond
 *
 *  \ingroup Group_Global
 *
 *  \param       ms          millisecond
 */
void tusb_delay_ms(uint32_t ms);

//////////////////////////////////////////////
// Device functions,  used in device only mode
//////////////////////////////////////////////

/** Get USB device handle
 *
 *  \ingroup Group_Device
 *
 *  \param[in] id          ID of the USB core, 0: full speed core, 1 high speed core
 *
 *  \return A USB device handle, type is \ref tusb_device_t .
 */
tusb_device_t* tusb_get_device(uint8_t id);

/** open USB in device mode
 *
 *  \ingroup Group_Device
 *
 *  \param  dev          USB device handle, return from \ref tusb_get_device
 */
void tusb_open_device(tusb_device_t* dev);

/** close USB device 
 *
 *  \ingroup Group_Device
 *
 *  \param  dev          USB device handle, return from \ref tusb_get_device
 */
void tusb_close_device(tusb_device_t* dev);

/** USB device send data
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, return from \ref tusb_get_device
 *  \param[in]  EPn          endpoint number
 *  \param[in]  data         data pointer, for device with DMA, this pointer must 32bit aligned
 *  \param[in]  len          data length, set len to 0 to send zero length packet
 *  \param[in]  option       data send options, each of \c TUSB_TXF_* mask.
 *
 *  \return  0 for success, -1 for end point not ready or data is on going
 */
int tusb_send_data(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len, uint8_t option);


/** USB device send status packet
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, return from \ref tusb_get_device
 *
 *  \return  0 for success, -1 for end point not ready or data is on going
 */
#define tusb_send_status(dev) \
  tusb_send_data(dev, 0, 0, 0, 0)

/** USB device send control data
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, return from \ref tusb_get_device
 *  \param[in]  data         control data pointer, for device with DMA, this pointer must 32bit aligned
 *  \param[in]  len          control data length
 *
 *  \return  0 for success, -1 for end point not ready or data is on going
 */
#define tusb_control_send(dev, data, len) \
  tusb_send_data(dev, 0, data, len, 0)

/** USB device set endpoint stall state
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, return from \ref tusb_get_device
 *  \param[in]  EPn          endpoint number and direction, when endpoint is 0, both IN/OUT will be stalled
 */
void tusb_set_stall(tusb_device_t* dev, uint8_t EPn);

/** USB device clear endpoint stall state
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, return from \ref tusb_get_device
 *  \param[in]  EPn          endpoint number and direction
 */
void tusb_clear_stall(tusb_device_t* dev, uint8_t EPn);


/** USB device set the recv buffer for endpoint
 *  Call this function before \ref tusb_set_rx_valid
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, return from \ref tusb_get_device
 *  \param[in]  EPn          endpoint number
 *  \param[out] data         receive buffer pointer, for device with DMA, this pointer must 32bit aligned
 *  \param[in]  len          buffer length, length must be N * MPS, where MPS is endpoint max packet size
 *
 *  \return  0 for success, otherwise fail
 */
int tusb_set_recv_buffer(tusb_device_t* dev, uint8_t EPn, void* data, uint16_t len);

/** USB device set the Rx/In endpoint valid
 *  Call \ref tusb_set_recv_buffer before set endpoint receive valid
 *  When the return value of callback function \ref tusb_on_rx_done is other than 0
 *  the endpoint will stop receive data, call this function to set receive endpoint valid
 *  again
 *
 *  \ingroup Group_Device
 *
 *  \param[in]  dev          USB device handle, return from \ref tusb_get_device
 *  \param[in]  EPn          endpoint number
 */
void tusb_set_rx_valid(tusb_device_t* dev, uint8_t EPn);

//////////////////////////////////
// user callback functions
//////////////////////////////////

/** Called when data transmit complete
 *  This is a WEAK function, default do nothing
 *  user application can override it to get the data transmit complete event 
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Event
 *
 *  \param[in]  dev          Indicate witch USB device fire the transmit complete event
 *  \param[in]  EPn          endpoint number
 */
void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn);

/** called when data receive complete
 *  This is a WEAK function, default always return 0, this will set Rx/OUT endpoint valid again
 *  user application can override it to get the data send done event 
 *  there are two conditions to trigger this callback
 *    1. Received data is large or equal to the buffer length set by tusb_set_recv_buffer
 *    2. Received data packed size is less than the end point max packet size
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Event
 *
 *  \param[in]  dev          Indicate witch USB device fire the receive complete event
 *  \param[in]  EPn          endpoint number
 *  \param[in]  data         receive data point buffer, this is the same of data paramter in \ref tusb_set_recv_buffer function
 *  \param[in]  len          actual receive data length
 *
 *  \return          0 - means received data processed, the Rx/OUT endpoint will set to valid aging, previous set received buffer will be used
 *           otherwise - means the data will be processed later, call \ref tusb_set_rx_valid to enable this endpoint again
 *               in case of ISO out endpoint, return 0 will use current receive buffer to rx data again
 *               otherwise the follow data will be dropped untill call tusb_set_rx_valid        
 */
int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len);

/** Called when USB device reset
 *  This is a WEAK function, default do nothing
 *  user application can override it to setup the endpoint and initial device status
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Event
 *
 *  \param[in]  dev          Indicate witch USB device fire the reset event
 */
void tusb_reconfig(tusb_device_t* dev);

/** Called when USB device need handle class request
 *  This is a WEAK function, default return 0
 *  user application can override it to handle the class request
 *
 *  \ingroup Group_Device
 *  \ingroup Group_Event
 *
 *  \param[in]  dev          Indicate witch USB device need handle class request
 *  \param[in]  setup        Setup packet pointer
 *
 *  \return       0 - the setup request is not processed by class, will be handle in TeenyUSB stack
 *                1 - the setup request is processed by class, TeenyUSB stack will ignore it
 */
int tusb_class_request(tusb_device_t* dev, tusb_setup_packet* setup);


//////////////////////////////////////////////
// Host functions,  used in host only mode
//////////////////////////////////////////////

/** Get USB host handle
 *
 *  \ingroup Group_Host
 *
 *  \param[in] id          ID of the USB core, 0: full speed core, 1 high speed core
 *
 *  \return A USB host handle, type is \ref tusb_host_t .
 */
tusb_host_t* tusb_get_host(uint8_t id);

/** open USB in host mode
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, return from \ref tusb_get_host
 */
void tusb_open_host(tusb_host_t* host);

/** close USB in host mode
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, return from \ref tusb_get_host
 */
void tusb_close_host(tusb_host_t* host);

/** Set USB host port state
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, return from \ref tusb_get_host
 *  \param[in]  port           port id
 *  \param[in]  reset          Reset state, 1- set port to reset state, 0-set port to normal state
 */
void tusb_port_set_reset(tusb_host_t* host, uint8_t port, uint8_t reset);

/** Get USB host port speed
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, return from \ref tusb_get_host
 *  \param[in]  port           port id
 *
 *  \return   Port attached device speed: \ref PORT_SPEED_HIGH, \ref PORT_SPEED_FULL, \ref PORT_SPEED_LOW
 */
uint8_t tusb_port_get_speed(tusb_host_t* host, uint8_t port);


/** Open a USB host pipe
 *
 *  \ingroup Group_Host
 *
 *  \param[in]  host           USB host handle, return from \ref tusb_get_host
 *  \param[out] pipe           Pipe pointer
 *  \param[in]  dev_addr       device address
 *  \param[in]  ep_addr        endpoint address and direction, e.g. 0x81 for endpoint 1, direction IN, 0x01 for endpoint 1, direction OUT
 *  \param[in]  ep_type        endpoint type, a \c EP_TYPE_* mask.
 *  \param[in]  mps            endpoint max packet size
 *  \param[in]  speed          Current attached device speed, \ref PORT_SPEED_HIGH, \ref PORT_SPEED_FULL, \ref PORT_SPEED_LOW
 *
 *  \return 0 for success, otherwise fail
 */
int tusb_pipe_open(tusb_host_t* host, tusb_pipe_t* pipe, uint8_t dev_addr, uint8_t ep_addr, uint8_t ep_type, uint16_t mps, uint8_t speed);

/** Close a USB host pipe
 *
 *  \ingroup Group_Host
 *
 *  \param[in] pipe           Pipe pointer, initial by \ref tusb_pipe_open
 *
 *  \return 0 for success, otherwise fail
 */
int tusb_pipe_close(tusb_pipe_t* pipe);

/** Cancel a USB host pipe
 *
 *  \ingroup Group_Host
 *
 *  \param[in] pipe           Pipe pointer, initial by \ref tusb_pipe_open
 *
 *  \return 0 for success, otherwise fail
 */
int tusb_pipe_cancel(tusb_pipe_t* pipe);



/** Send a setup packet
 *
 *  \ingroup Group_Host
 *
 *  \param[in] pipe           Pipe pointer, initial by \ref tusb_pipe_open
 *  \param[in] setup          Setup packet pointer
 */
void tusb_pipe_setup(tusb_pipe_t* pipe, tusb_setup_packet* setup);

/** Send or receive a data packet
 *
 *  \ingroup Group_Host
 *
 *  \param[in] pipe           Pipe pointer, initial by \ref tusb_pipe_open
 *  \param[in] data           Data or buffer pointer
 *  \param[in] len            Data or buffer length
 */
void tusb_pipe_xfer_data(tusb_pipe_t* pipe, void* data, uint32_t len);

/** Get pipe xfer data length
 *
 *  \ingroup Group_Host
 *
 *  \param[in] pipe           Pipe pointer, initial by \ref tusb_pipe_open
 *
 *  \return >=0  xfer data size of the pipe, 
 *           <0  means error
 */
int tusb_pipe_get_xfer_len(tusb_pipe_t* pipe);
  

/** Wait or get a pipe transfer state
 *
 *  \ingroup Group_Host
 *
 *  \param[in] pipe           Pipe pointer, initial by \ref tusb_pipe_open
 *  \param[in] timeout        0 - don't wait, just get the current pipe state
 *                            0xffffffff - wait forever
 *  \return    Channel state, type is \ref channel_state_t
 */
channel_state_t tusb_pipe_wait(tusb_pipe_t* pipe, uint32_t timeout);


//////////////////////////////////
// Host callback functions
//////////////////////////////////

/** Called when host channel action done or error
 *  This is a WEAK function, default do nothing
 *  user application can override it to handle the channel event
 *
 *  \ingroup Group_Event
 *
 *  \param[in]  host           USB host handle, return from \ref tusb_get_host
 *  \param[in]  hc_num         indicate witch host channel fire this event
 *
 *  \return    not used
 */
int tusb_on_channel_event(tusb_host_t* host, uint8_t hc_num);

/** Called when port status changed
 *  This is a WEAK function, default do nothing
 *  user application can override it to handle the port event
 *
 *  \ingroup Group_Event
 *
 *  \param[in]  host           USB host handle, return from \ref tusb_get_host
 *  \param[in]  port           Indicate witch host port fire this event
 *  \param[in]  new_state      Current port state
 */
void tusb_host_port_changed(tusb_host_t* host, uint8_t port, host_port_state_t new_state);


//////////////////////////////////////////////
// OTG functions, used in DRD mode
//////////////////////////////////////////////

/** Get USB OTG handle
 *
 *  \ingroup Group_OTG
 *
 *  \param[in] id          ID of the USB core, 0: full speed core, 1 high speed core
 *
 *  \return A USB OTG handle, type is \ref tusb_otg_t
 */
tusb_otg_t* tusb_get_otg(uint8_t id);

/** Open USB in OTG mode
 *
 *  \ingroup Group_OTG
 *
 *  \param[in]  otg           USB OTG handle, return from \ref tusb_get_otg
 */
void tusb_open_otg(tusb_otg_t* otg);

/** Close USB in OTG mode
 *
 *  \ingroup Group_OTG
 *
 *  \param[in]  otg           USB OTG handle, return from \ref tusb_get_otg
 */
void tusb_close_otg(tusb_otg_t* otg);


/** Called when ID lined changed
 *  This is a WEAK function, default do nothing
 *  user application can override it to handle the ID line changed event
 *
 *  \ingroup Group_Event
 *
 *  \param[in]  otg            USB otg handle, return from \ref tusb_get_otg
 *  \param[in]  id_state       New ID line status \ref id_state_t
 */
void tusb_otg_id_changed(tusb_otg_t* otg, id_state_t id_state);

#endif
