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

#include "teeny_usb.h"
#include "tusbd.h"

void tusb_set_device_config(tusb_device_t *dev, tusb_device_config_t *dev_config)
{
    memset(dev_config->ep_in_interface, 0, sizeof(dev_config->ep_in_interface));
    memset(dev_config->ep_out_interface, 0, sizeof(dev_config->ep_out_interface));
    for (int i = 0; i < dev_config->if_count; i++)
    {
        tusb_device_interface_t *itf = dev_config->interfaces[i];
        if (itf)
        {
            if (itf->dev != dev)
            {
                itf->dev = dev;
            }
            if (itf->ep_in && (itf->ep_in & 0x7f) <= MAX_EP_ID)
            {
                dev_config->ep_in_interface[(itf->ep_in & 0x7f) - 1] = itf;
            }
            if (itf->ep_out && (itf->ep_out & 0x7f) <= MAX_EP_ID)
            {
                dev_config->ep_out_interface[(itf->ep_out & 0x7f) - 1] = itf;
            }
        }
    }
    dev->user_data = dev_config;
}

/**
 * general device reconfig handler
 */
void tusb_reconfig(tusb_device_t *dev)
{
    tusb_device_config_t *dev_config = dev->user_data;
    if (dev_config)
    {
        dev_config->ep_init(dev);
        for (int i = 0; i < dev_config->if_count; i++)
        {
            tusb_device_interface_t *itf = dev_config->interfaces[i];
            if (itf && itf->backend && itf->backend->device_init)
            {
                itf->backend->device_init(itf);
            }
        }
    }
}

/**
 * General device class request
 */
int tusb_class_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    tusb_device_config_t *dev_config = dev->user_data;
    if (dev_config &&
            (setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE)
    {
        uint16_t iInterfce = setup_req->wIndex;
        if (iInterfce < dev_config->if_count)
        {
            tusb_device_interface_t *itf = dev_config->interfaces[iInterfce];
            if (itf && itf->backend && itf->backend->device_request)
            {
                return itf->backend->device_request(itf, setup_req);
            }
        }
    }
    // the setup packet will be processed in Teeny USB stack
    return 0;
}

/**
 * General device transfer complete callback handler
 */
void tusb_on_tx_done(tusb_device_t *dev, uint8_t EPn)
{
    tusb_device_config_t *dev_config = dev->user_data;
    if (EPn & 0x7f)
    {
        tusb_device_interface_t *itf = dev_config->ep_in_interface[(EPn & 0x7f) - 1];
        if (itf && itf->backend && itf->backend->device_send_done)
        {
            itf->backend->device_send_done(itf, EPn);
            return;
        }
    }
}

/**
 * General device receive complete callback handler
 */
int tusb_on_rx_done(tusb_device_t *dev, uint8_t EPn, const void *data, uint16_t len)
{
    tusb_device_config_t *dev_config = dev->user_data;
    if (EPn & 0x7f)
    {
        tusb_device_interface_t *itf = dev_config->ep_out_interface[(EPn & 0x7f) - 1];
        if (itf && itf->backend && itf->backend->device_recv_done)
        {
            return itf->backend->device_recv_done(itf, EPn, data, len);
        }
    }
    return 0;
}



