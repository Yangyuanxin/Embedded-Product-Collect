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

#ifndef __TUSBD_HID_H__
#define __TUSBD_HID_H__
#include "tusbd.h"

typedef struct _tusb_hid_state
{
    uint8_t idle;
    uint8_t protocol;
} tusb_hid_state_t;

typedef struct _tusb_hid_device tusb_hid_device_t;

struct _tusb_hid_device
{
    tusb_device_t *dev;
    const tusb_device_backend_t *backend;
    uint8_t ep_in;
    uint8_t ep_out;
    uint8_t pad1;
    uint8_t pad2;
    int(* on_recv_data)(tusb_hid_device_t *hid, const void *data, uint16_t len);
    int(* on_send_done)(tusb_hid_device_t *hid);
    int(* on_set_report)(tusb_hid_device_t *hid, uint8_t reportType, uint8_t reportID, const void *data, uint16_t len);
    int(* on_get_report)(tusb_hid_device_t *hid, uint8_t reportType, uint8_t reportID, void *data, uint16_t len);
    int(* on_state_change)(tusb_hid_device_t *hid);
    void    *rx_buf;
    uint32_t rx_size;
    desc_t  report_desc;
    void *user_data;
    uint32_t report_desc_size;
    tusb_hid_state_t state;
};
#define tusb_hid_device_send(hid, data, len) \
  tusb_send_data( (hid)->dev, (hid)->ep_in, data, len, 0)

extern const tusb_device_backend_t hid_device_backend;


#endif
