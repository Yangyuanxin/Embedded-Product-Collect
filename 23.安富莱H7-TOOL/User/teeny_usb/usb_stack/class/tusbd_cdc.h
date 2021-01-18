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

#ifndef __TUSBD_CDC_H__
#define __TUSBD_CDC_H__
#include "tusbd.h"

#define TUSB_CDC_DTR   0x01
#define TUSB_CDC_RTS   0x02

/** Enums for \ref tusb_cdc_line_coding_t  stopbits field
 *  indicating the CDC stopbits
 */
typedef enum
{
    CDC_1STOP = 0,
    CDC_1_5STOP = 1,
    CDC_2_STOP = 2,
} cdc_stopbits_t;

/** Enums for \ref tusb_cdc_line_coding_t  parity field
 *  indicating the CDC parity
 */
typedef enum
{
    CDC_NONE = 0,
    CDC_ODD = 1,
    CDC_EVEN = 2,
    CDC_MARK = 3,
    CDC_SPACE = 4,
} cdc_parity_t;

/* Type Defines: */
/** Type define for CDC line coding
 */
typedef struct _tusb_cdc_line_coding
{
    uint32_t bitrate;        /**< bit rate */
    uint8_t  stopbits;       /**< stop bits, \ref cdc_stopbits_t*/
    uint8_t  parity;         /**< parity, \ref cdc_parity_t */
    uint8_t  databits;       /**< data bits: 5,6,7,8 */
} tusb_cdc_line_coding_t;


typedef struct _tusb_cdc_state
{
    tusb_setup_packet req;
    uint16_t CDC: 1;
    uint16_t DSR: 1;
    uint16_t Break: 1;
    uint16_t Ring: 1;
    uint16_t FramingError: 1;
    uint16_t ParityError: 1;
    uint16_t Overrun: 1;
    uint16_t revserved: 9;
} tusb_cdc_state;

typedef struct _tusb_cdc_device tusb_cdc_device_t;

/** Type define for CDC device
 *  Add capacity D3 in CDC ACM descriptor to enable the set break feature
 */
struct _tusb_cdc_device
{
    tusb_device_t *dev;
    const tusb_device_backend_t *backend;
    uint8_t ep_in;
    uint8_t ep_out;
    uint8_t ep_int;
    uint8_t ep_int_busy;
    int(* on_recv_data)(tusb_cdc_device_t *cdc, const void *data, uint16_t len);
    int(* on_send_done)(tusb_cdc_device_t *cdc);
    void    *rx_buf;
    uint32_t rx_size;
    void(* on_line_coding_change)(tusb_cdc_device_t *cdc);
    void(* on_line_state_change)(tusb_cdc_device_t *cdc, uint16_t state);
    void(* on_set_break)(tusb_cdc_device_t *cdc, uint16_t break_time_in_ms);
    void *user_data;
    tusb_cdc_line_coding_t line_coding;
    tusb_cdc_state state;
};


/**  send CDC data */
#define tusb_cdc_device_send(cdc, data, len) \
  tusb_send_data( (cdc)->dev, (cdc)->ep_in, data, len, TUSB_TXF_ZLP)

/** send CDC state when the input line state changed or error occurs */
int tusb_cdc_send_state(tusb_cdc_device_t *cdc);

extern const tusb_device_backend_t cdc_device_backend;

#endif
