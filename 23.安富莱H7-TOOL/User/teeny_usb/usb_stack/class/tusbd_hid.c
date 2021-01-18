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

#include "tusbd_hid.h"

#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01

static void tusb_hid_device_init(tusb_hid_device_t *hid)
{
    tusb_set_recv_buffer(hid->dev, hid->ep_out, hid->rx_buf, hid->rx_size);
    tusb_set_rx_valid(hid->dev, hid->ep_out);
}

static void hid_dataout_request(tusb_device_t *dev)
{
    tusb_device_config_t *dev_config = (tusb_device_config_t *)dev->user_data;
    int ifn = dev->setup.wIndex;
    if (ifn < dev_config->if_count)
    {
        tusb_hid_device_t *hid = (tusb_hid_device_t *)dev_config->interfaces[ifn];
        if (hid->on_recv_data)
        {
            uint8_t reportID = LO_BYTE(dev->setup.wValue);
            uint8_t reportType = HI_BYTE(dev->setup.wValue);
            if (hid->on_set_report)
            {
                hid->on_set_report(hid, reportType, reportID, dev_config->cmd_buffer, dev->setup.wLength);
            }
        }
    }
    tusb_send_status(dev);
}

static int tusb_hid_device_request(tusb_hid_device_t *hid, tusb_setup_packet *setup_req)
{
    tusb_device_t *dev = hid->dev;
    tusb_device_config_t *dev_config = (tusb_device_config_t *)dev->user_data;
    // Get interface report descriptor
    if ((setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE)
    {
        if (setup_req->bRequest == USB_REQ_GET_DESCRIPTOR)
        {
            if (HI_BYTE(setup_req->wValue) == USB_DESC_TYPE_REPORT)
            {
                uint16_t len = hid->report_desc_size;
                const uint8_t *desc = hid->report_desc;
                if (desc && len)
                {
                    len = setup_req->wLength > len ? len : setup_req->wLength;
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
                    if (dev->desc_buffer != desc)
                    {
                        memcpy(dev->desc_buffer, desc, len);
                        desc = dev->desc_buffer;
                    }
#endif
                    tusb_control_send(dev, desc, len);
                    return 1;
                }
            }
        }
    }
    // handle class request
    if ((setup_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS)
    {
        switch (setup_req->bRequest)
        {
        case HID_REQ_SET_IDLE:
            hid->state.idle = HI_BYTE(setup_req->wValue);
            if (hid->on_state_change)
            {
                hid->on_state_change(hid);
            }
            tusb_send_status(dev);
            return 1;
        case HID_REQ_GET_IDLE:
            dev_config->cmd_buffer[0] = hid->state.idle;
            tusb_control_send(dev, dev_config->cmd_buffer, 1);
            return 1;
        case HID_REQ_SET_PROTOCOL:
            hid->state.protocol = LO_BYTE(setup_req->wValue);
            if (hid->on_state_change)
            {
                hid->on_state_change(hid);
            }
            tusb_send_status(dev);
            return 1;
        case HID_REQ_GET_PROTOCOL:
            tusb_control_send(dev,  dev_config->cmd_buffer, 1);
            return 1;
        case HID_REQ_SET_REPORT:
            tusb_set_recv_buffer(dev, 0, dev_config->cmd_buffer, setup_req->wLength);
            dev->ep0_rx_done = hid_dataout_request;
            return 1;
        case HID_REQ_GET_REPORT:
        {
            // TODO: handle the get report request
            uint8_t reportID = LO_BYTE(dev->setup.wValue);
            uint8_t reportType = HI_BYTE(dev->setup.wValue);
            uint16_t len = 0;
            if (hid->on_get_report)
            {
                len = hid->on_get_report(hid, reportType, reportID, dev_config->cmd_buffer, setup_req->wLength);
            }
            tusb_control_send(dev,  dev_config->cmd_buffer, len);
            return 1;
        }
        default:
            return 0;
        }
    }
    return 0;
}

static int tusb_hid_device_send_done(tusb_hid_device_t *hid, uint8_t EPn)
{
    if (hid->on_send_done)
    {
        return hid->on_send_done(hid);
    }
    return 0;
}

static int tusb_hid_device_recv_done(tusb_hid_device_t *hid, uint8_t EPn, const void *data, uint16_t len)
{
    if (hid->on_recv_data)
    {
        return hid->on_recv_data(hid, data, len);
    }
    return 0;
}


const tusb_device_backend_t hid_device_backend =
{
    .device_init = (int(*)(tusb_device_interface_t *))tusb_hid_device_init,
    .device_request = (int(*)(tusb_device_interface_t *, tusb_setup_packet *))tusb_hid_device_request,
    .device_send_done = (int(*)(tusb_device_interface_t *, uint8_t))tusb_hid_device_send_done,
    .device_recv_done = (int(*)(tusb_device_interface_t *, uint8_t, const void *, uint16_t))tusb_hid_device_recv_done,
};
