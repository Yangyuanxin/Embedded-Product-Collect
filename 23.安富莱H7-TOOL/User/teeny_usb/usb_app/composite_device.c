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
#include "tusbd_user.h"
#include "tusbd_hid.h"
#include "tusbd_cdc.h"
#include "tusbd_msc.h"

#define USER_RX_EP_SIZE   32

#define CDC_RX_EP_SIZE    512

#define HID_RX_EP_SIZE    64
#define HID_REPORT_DESC         COMP_ReportDescriptor_if0
#define HID_REPORT_DESC_SIZE    COMP_REPORT_DESCRIPTOR_SIZE_IF0

// allocate more buffer for better performance
__ALIGN_BEGIN uint8_t user_buf[USER_RX_EP_SIZE * 4] __ALIGN_END;

int user_recv_data(tusb_user_device_t *raw, const void *data, uint16_t len);
int user_send_done(tusb_user_device_t *raw);

tusb_user_device_t user_dev =
{
    .backend = &user_device_backend,
    .ep_in = 2,
    .ep_out = 2,
    .on_recv_data = user_recv_data,
    .on_send_done = user_send_done,
    .rx_buf = user_buf,
    .rx_size = sizeof(user_buf),
};

// The HID recv buffer size must equal to the out report size
__ALIGN_BEGIN uint8_t hid_buf[HID_RX_EP_SIZE] __ALIGN_END;
int hid_recv_data(tusb_hid_device_t *hid, const void *data, uint16_t len);
int hid_send_done(tusb_hid_device_t *hid);

tusb_hid_device_t hid_dev =
{
    .backend = &hid_device_backend,
    .ep_in = 1,
    .ep_out = 1,
    .on_recv_data = hid_recv_data,
    .on_send_done = hid_send_done,
    .rx_buf = hid_buf,
    .rx_size = sizeof(hid_buf),
    .report_desc = HID_REPORT_DESC,
    .report_desc_size = HID_REPORT_DESC_SIZE,
};

// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer
__ALIGN_BEGIN uint8_t cdc_buf0[CDC_RX_EP_SIZE] __ALIGN_END;

int cdc_recv_data0(tusb_cdc_device_t *cdc, const void *data, uint16_t len);
int cdc_send_done0(tusb_cdc_device_t *cdc);
void cdc_line_coding_change(tusb_cdc_device_t *cdc);

tusb_cdc_device_t cdc_dev0 =
{
    .backend = &cdc_device_backend,
    .ep_in = 4,
    .ep_out = 4,
    .ep_int = 9,
    .on_recv_data = cdc_recv_data0,
    .on_send_done = cdc_send_done0,
    .on_line_coding_change = cdc_line_coding_change,
    .rx_buf = cdc_buf0,
    .rx_size = sizeof(cdc_buf0),
};

// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer
__ALIGN_BEGIN uint8_t cdc_buf1[CDC_RX_EP_SIZE] __ALIGN_END;

int cdc_recv_data1(tusb_cdc_device_t *cdc, const void *data, uint16_t len);
int cdc_send_done1(tusb_cdc_device_t *cdc);
void cdc_line_coding_change(tusb_cdc_device_t *cdc);

tusb_cdc_device_t cdc_dev1 =
{
    .backend = &cdc_device_backend,
    .ep_in = 5,
    .ep_out = 5,
    .ep_int = 10,
    .on_recv_data = cdc_recv_data1,
    .on_send_done = cdc_send_done1,
    .on_line_coding_change = cdc_line_coding_change,
    .rx_buf = cdc_buf1,
    .rx_size = sizeof(cdc_buf1),
};

// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer
__ALIGN_BEGIN uint8_t cdc_buf2[CDC_RX_EP_SIZE] __ALIGN_END;

int cdc_recv_data2(tusb_cdc_device_t *cdc, const void *data, uint16_t len);
int cdc_send_done2(tusb_cdc_device_t *cdc);
void cdc_line_coding_change(tusb_cdc_device_t *cdc);

tusb_cdc_device_t cdc_dev2 =
{
    .backend = &cdc_device_backend,
    .ep_in = 6,
    .ep_out = 6,
    .ep_int = 11,
    .on_recv_data = cdc_recv_data2,
    .on_send_done = cdc_send_done2,
    .on_line_coding_change = cdc_line_coding_change,
    .rx_buf = cdc_buf2,
    .rx_size = sizeof(cdc_buf2),
};

// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer
__ALIGN_BEGIN uint8_t cdc_buf3[CDC_RX_EP_SIZE] __ALIGN_END;

int cdc_recv_data3(tusb_cdc_device_t *cdc, const void *data, uint16_t len);
int cdc_send_done3(tusb_cdc_device_t *cdc);
void cdc_line_coding_change(tusb_cdc_device_t *cdc);

tusb_cdc_device_t cdc_dev3 =
{
    .backend = &cdc_device_backend,
    .ep_in = 7,
    .ep_out = 7,
    .ep_int = 12,
    .on_recv_data = cdc_recv_data3,
    .on_send_done = cdc_send_done3,
    .on_line_coding_change = cdc_line_coding_change,
    .rx_buf = cdc_buf3,
    .rx_size = sizeof(cdc_buf3),
};

// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer
__ALIGN_BEGIN uint8_t cdc_buf4[CDC_RX_EP_SIZE] __ALIGN_END;

int cdc_recv_data4(tusb_cdc_device_t *cdc, const void *data, uint16_t len);
int cdc_send_done4(tusb_cdc_device_t *cdc);
void cdc_line_coding_change(tusb_cdc_device_t *cdc);

tusb_cdc_device_t cdc_dev4 =
{
    .backend = &cdc_device_backend,
    .ep_in = 8,
    .ep_out = 8,
    .ep_int = 13,
    .on_recv_data = cdc_recv_data4,
    .on_send_done = cdc_send_done4,
    .on_line_coding_change = cdc_line_coding_change,
    .rx_buf = cdc_buf4,
    .rx_size = sizeof(cdc_buf4),
};


// make sure the interface order is same in "composite_desc.lua"
static tusb_device_interface_t *device_interfaces[] =
{
    (tusb_device_interface_t *) &hid_dev,
    (tusb_device_interface_t *) &user_dev,
    (tusb_device_interface_t *) &cdc_dev0, 0, // CDC need two interfaces
    (tusb_device_interface_t *) &cdc_dev1, 0, // CDC need two interfaces
    (tusb_device_interface_t *) &cdc_dev2, 0, // CDC need two interfaces
    (tusb_device_interface_t *) &cdc_dev3, 0, // CDC need two interfaces
    (tusb_device_interface_t *) &cdc_dev4, 0, // CDC need two interfaces
};

static void init_ep(tusb_device_t *dev)
{
    COMP_TUSB_INIT(dev);
}

tusb_device_config_t device_config =
{
    .if_count = sizeof(device_interfaces) / sizeof(device_interfaces[0]),
    .interfaces = &device_interfaces[0],
    .ep_init = init_ep,
};

void tusb_delay_ms(uint32_t ms)
{
    uint32_t i, j;
    for (i = 0; i < ms; ++i)
        for (j = 0; j < 200; ++j);
}


static int user_len = 0;
int user_recv_data(tusb_user_device_t *raw, const void *data, uint16_t len)
{
    user_len = (int)len;
    return 1; // return 1 means the recv buffer is busy
}

int user_send_done(tusb_user_device_t *raw)
{
    tusb_set_rx_valid(raw->dev, raw->ep_out);
    return 0;
}

static int hid_len = 0;
int hid_recv_data(tusb_hid_device_t *hid, const void *data, uint16_t len)
{
    hid_len = (int)len;
    return 1; // return 1 means the recv buffer is busy
}

int hid_send_done(tusb_hid_device_t *hid)
{
    tusb_set_rx_valid(hid->dev, hid->ep_out);
    return 0;
}

static int cdc_len = 0;
int cdc_recv_data(tusb_cdc_device_t *cdc, const void *data, uint16_t len)
{
    cdc_len = (int)len;
    return 1; // return 1 means the recv buffer is busy
}

int cdc_send_done(tusb_cdc_device_t *cdc)
{
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    return 0;
}

static int cdc_len0 = 0;
int cdc_recv_data0(tusb_cdc_device_t *cdc, const void *data, uint16_t len)
{
    cdc_len0 = (int)len;
    return 1; // return 1 means the recv buffer is busy
}

int cdc_send_done0(tusb_cdc_device_t *cdc)
{
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    return 0;
}

/************ CDC1 ************************************/

static int cdc_len1 = 0;

int cdc_recv_data1(tusb_cdc_device_t *cdc, const void *data, uint16_t len)
{
    cdc_len1 = (int)len;
    
    usbd_RxHostModbus(data, len);
    return 1; // return 1 means the recv buffer is busy
}

int cdc_send_done1(tusb_cdc_device_t *cdc)
{
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    return 0;
}

void cdc_device_send1(const void *data, uint16_t len)
{
    tusb_cdc_device_send(&cdc_dev1, data, len);
}

/*****************CDC2*******************************/

static int cdc_len2 = 0;
int cdc_recv_data2(tusb_cdc_device_t *cdc, const void *data, uint16_t len)
{
    cdc_len2 = (int)len;
    return 1; // return 1 means the recv buffer is busy
}

int cdc_send_done2(tusb_cdc_device_t *cdc)
{
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    return 0;
}

/*****************CDC3*******************************/

static int cdc_len3 = 0;
int cdc_recv_data3(tusb_cdc_device_t *cdc, const void *data, uint16_t len)
{
    cdc_len3 = (int)len;
    return 1; // return 1 means the recv buffer is busy
}

int cdc_send_done3(tusb_cdc_device_t *cdc)
{
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    return 0;
}

static int cdc_len4 = 0;
int cdc_recv_data4(tusb_cdc_device_t *cdc, const void *data, uint16_t len)
{
    cdc_len4 = (int)len;
    return 1; // return 1 means the recv buffer is busy
}

int cdc_send_done4(tusb_cdc_device_t *cdc)
{
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    return 0;
}

void cdc_line_coding_change(tusb_cdc_device_t *cdc)
{
    // TODO, handle the line coding change
    //cdc->line_coding.bitrate;
    //cdc->line_coding.databits;
    //cdc->line_coding.stopbits;
    //cdc->line_coding.parity;
}

void tusb_init(void)
{
    tusb_device_t *dev = tusb_get_device(TEST_APP_USB_CORE);
    tusb_set_device_config(dev, &device_config);
    tusb_open_device(dev);
}

void tusb_task(void)
{
    if (user_len)
    {
        for (int i = 0; i < user_len; i++)
        {
            user_buf[i] += 1;
        }
        tusb_user_device_send(&user_dev, user_buf, user_len);
        user_len = 0;
    }

    if (hid_len)
    {
        for (int i = 0; i < hid_len; i++)
        {
            hid_buf[i] += 2;
        }
        tusb_hid_device_send(&hid_dev, hid_buf, hid_len);
        hid_len = 0;
    }

    if (cdc_len0)
    {
        for (int i = 0; i < cdc_len0; i++)
        {
            cdc_buf0[i] += 3;
        }
        tusb_cdc_device_send(&cdc_dev0, cdc_buf0, cdc_len0);
        cdc_len0 = 0;
    }
    
    if (cdc_len1)
    {
        for (int i = 0; i < cdc_len1; i++)
        {
            cdc_buf1[i] += 3;
        }
        tusb_cdc_device_send(&cdc_dev1, cdc_buf1, cdc_len1);
        cdc_len1 = 0;
    }
    
    if (cdc_len2)
    {
        for (int i = 0; i < cdc_len2; i++)
        {
            cdc_buf2[i] += 3;
        }
        tusb_cdc_device_send(&cdc_dev2, cdc_buf2, cdc_len2);
        cdc_len2 = 0;
    }
    
    if (cdc_len3)
    {
        for (int i = 0; i < cdc_len3; i++)
        {
            cdc_buf3[i] += 3;
        }
        tusb_cdc_device_send(&cdc_dev3, cdc_buf3, cdc_len3);
        cdc_len3 = 0;
    }
    
    if (cdc_len4)
    {
        for (int i = 0; i < cdc_len4; i++)
        {
            cdc_buf4[i] += 3;
        }
        tusb_cdc_device_send(&cdc_dev4, cdc_buf4, cdc_len4);
        cdc_len4 = 0;
    }
}


