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

#include "tusbd_cdc.h"

#define SEND_ENCAPSULATED_COMMAND               0x00
#define GET_ENCAPSULATED_RESPONSE               0x01
#define SET_COMM_FEATURE                        0x02
#define GET_COMM_FEATURE                        0x03
#define CLEAR_COMM_FEATURE                      0x04
#define SET_LINE_CODING                         0x20
#define GET_LINE_CODING                         0x21
#define SET_CONTROL_LINE_STATE                  0x22
#define SEND_BREAK                              0x23
#define SERIAL_STATE                            0x20

int tusb_cdc_send_state(tusb_cdc_device_t *cdc)
{
    if (cdc->ep_int_busy) return -1;
    tusb_device_config_t *dev_config = (tusb_device_config_t *)cdc->dev->user_data;
    uint16_t ifn = 0;
    for (; ifn < dev_config->if_count; ifn++)
    {
        if (dev_config->interfaces[ifn] == (tusb_device_interface_t *)cdc)
        {
            break;
        }
    }
    cdc->state.req.bmRequestType = 0xA1;
    cdc->state.req.bRequest = SERIAL_STATE;
    cdc->state.req.wValue = 0;
    cdc->state.req.wIndex = ifn;
    cdc->state.req.wLength = 2;
    return tusb_send_data(cdc->dev, cdc->ep_int, &cdc->state, 10, 0);
}

static int tusb_cdc_device_init(tusb_cdc_device_t *cdc)
{
    tusb_set_recv_buffer(cdc->dev, cdc->ep_out, cdc->rx_buf, cdc->rx_size);
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    cdc->ep_int_busy = 0;
    if (cdc->ep_int && cdc->ep_int <= MAX_EP_ID)
    {
        tusb_device_config_t *dev_config = (tusb_device_config_t *)cdc->dev->user_data;
        dev_config->ep_in_interface[ cdc->ep_int - 1 ] = (tusb_device_interface_t *)cdc;
    }
    return 0;
}

static uint16_t cdc_ctrl(tusb_cdc_device_t *cdc, tusb_setup_packet *setup_req, uint8_t *buf)
{
    switch (setup_req->bRequest)
    {
    case SEND_ENCAPSULATED_COMMAND:
        /* Not  needed for this driver */
        break;

    case GET_ENCAPSULATED_RESPONSE:
        /* Not  needed for this driver */
        break;

    case SET_COMM_FEATURE:
        /* Not  needed for this driver */
        break;

    case GET_COMM_FEATURE:
        /* Not  needed for this driver */
        break;

    case CLEAR_COMM_FEATURE:
        /* Not  needed for this driver */
        break;

    case SET_LINE_CODING:
        cdc->line_coding.bitrate = (uint32_t)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));
        cdc->line_coding.stopbits = buf[4];
        cdc->line_coding.parity = buf[5];
        cdc->line_coding.databits = buf[6];
        break;

    case GET_LINE_CODING:
        buf[0] = (uint8_t)(cdc->line_coding.bitrate);
        buf[1] = (uint8_t)(cdc->line_coding.bitrate >> 8);
        buf[2] = (uint8_t)(cdc->line_coding.bitrate >> 16);
        buf[3] = (uint8_t)(cdc->line_coding.bitrate >> 24);
        buf[4] = cdc->line_coding.stopbits;
        buf[5] = cdc->line_coding.parity;
        buf[6] = cdc->line_coding.databits;
        break;

    case SET_CONTROL_LINE_STATE:
        if (cdc->on_line_state_change)
        {
            cdc->on_line_state_change(cdc, setup_req->wValue);
        }
        break;

    case SEND_BREAK:
        if (cdc->on_set_break)
        {
            cdc->on_set_break(cdc, setup_req->wValue);
        }
        break;

    default:
        break;
    }

    return 0;
}

static void cdc_dataout_request(tusb_device_t *dev)
{
    tusb_device_config_t *dev_config = (tusb_device_config_t *)dev->user_data;
    int ifn = dev->setup.wIndex;
    if (ifn < dev_config->if_count)
    {
        tusb_cdc_device_t *cdc = (tusb_cdc_device_t *)dev_config->interfaces[ifn];
        cdc_ctrl(cdc, &dev->setup, dev_config->cmd_buffer);
        if (cdc->on_line_coding_change)
        {
            cdc->on_line_coding_change(cdc);
        }
    }
    tusb_send_status(dev);
}

static int tusb_cdc_device_request(tusb_cdc_device_t *cdc, tusb_setup_packet *setup_req)
{
    tusb_device_t *dev = cdc->dev;
    tusb_device_config_t *dev_config = (tusb_device_config_t *)dev->user_data;
    if ((setup_req->bRequest & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS)
    {
        if (setup_req->wLength > 0)
        {
            if (setup_req->bmRequestType & 0x80)
            {
                cdc_ctrl(cdc, setup_req, dev_config->cmd_buffer);
                tusb_control_send(dev, dev_config->cmd_buffer, setup_req->wLength);
                return 1;
            }
            else
            {
                tusb_set_recv_buffer(dev, 0, dev_config->cmd_buffer, setup_req->wLength);
                dev->ep0_rx_done = cdc_dataout_request;
                return 1;
            }
        }
        else
        {
            cdc_ctrl(cdc, setup_req, dev_config->cmd_buffer);
            tusb_send_status(dev);
            return 1;
        }
    }
    return 0;
}

static int tusb_cdc_device_send_done(tusb_cdc_device_t *cdc, uint8_t EPn)
{
    if (EPn == cdc->ep_in)
    {
        if (cdc->on_send_done)
        {
            return cdc->on_send_done(cdc);
        }
    }
    else if (EPn == cdc->ep_int)
    {
        cdc->ep_int_busy = 0;
    }
    return 0;
}

static int tusb_cdc_device_recv_done(tusb_cdc_device_t *cdc, uint8_t EPn, const void *data, uint16_t len)
{
    if (cdc->on_recv_data)
    {
        return cdc->on_recv_data(cdc, data, len);
    }
    return 0;
}

const tusb_device_backend_t cdc_device_backend =
{
    .device_init = (int(*)(tusb_device_interface_t *))tusb_cdc_device_init,
    .device_request = (int(*)(tusb_device_interface_t *, tusb_setup_packet *))tusb_cdc_device_request,
    .device_send_done = (int(*)(tusb_device_interface_t *, uint8_t))tusb_cdc_device_send_done,
    .device_recv_done = (int(*)(tusb_device_interface_t *, uint8_t, const void *, uint16_t))tusb_cdc_device_recv_done,
};
