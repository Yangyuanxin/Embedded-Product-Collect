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

#ifndef __TUSBD_H__
#define __TUSBD_H__

#include "teeny_usb.h"
#include <string.h>

#ifndef MAX_EP_ID
    #ifdef EP_NUM
        #define MAX_EP_ID   (EP_NUM-1)
    #else
        #define MAX_EP_ID   7
    #endif
#endif

typedef struct _tusb_device_backend tusb_device_backend_t;
typedef struct _tusb_device_interface tusb_device_interface_t;
typedef struct _tusb_device_config tusb_device_config_t;

struct _tusb_device_interface
{
    tusb_device_t *dev;                     /**< interface device handle */
    const tusb_device_backend_t *backend;   /**< interface backend  */
    uint8_t ep_in;                          /**< endpoint in  */
    uint8_t ep_out;                         /**< endpoint out */
};

struct _tusb_device_backend
{
    int(*device_init)(tusb_device_interface_t *itf);
    int(*device_request)(tusb_device_interface_t *itf, tusb_setup_packet *setup_req);
    int(*device_send_done)(tusb_device_interface_t *itf, uint8_t EPn);
    int(*device_recv_done)(tusb_device_interface_t *itf, uint8_t EPn, const void *data, uint16_t len);
};

struct _tusb_device_config
{
    uint32_t  if_count;                    /**< total interface count */
    tusb_device_interface_t **interfaces;  /**< Interface array        */
    void (*ep_init)(tusb_device_t *dev);   /**< Endpoint register init function */
    uint8_t   cmd_buffer[64];              /**< buffer for control transfer, not for descriptor */
    tusb_device_interface_t *
    ep_in_interface[MAX_EP_ID];     /**< array for in callback  */
    tusb_device_interface_t *
    ep_out_interface[MAX_EP_ID];    /**< array for out callback */
};

void tusb_set_device_config(tusb_device_t *dev, tusb_device_config_t *dev_config);

#endif
