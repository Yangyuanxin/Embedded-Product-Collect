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

#include "tusbd_user.h"

static void tusb_user_device_init(tusb_user_device_t *user)
{
    tusb_set_recv_buffer(user->dev, user->ep_out, user->rx_buf, user->rx_size);
    tusb_set_rx_valid(user->dev, user->ep_out);
}

static int tusb_user_device_send_done(tusb_user_device_t *user, uint8_t EPn)
{
    if (user->on_send_done)
    {
        return user->on_send_done(user);
    }
    return 0;
}

static int tusb_user_device_recv_done(tusb_user_device_t *user, uint8_t EPn, const void *data, uint16_t len)
{
    if (user->on_recv_data)
    {
        return user->on_recv_data(user, data, len);
    }
    return 0;
}


const tusb_device_backend_t user_device_backend =
{
    .device_init = (int(*)(tusb_device_interface_t *))tusb_user_device_init,
    .device_request = 0,
    .device_send_done = (int(*)(tusb_device_interface_t *, uint8_t))tusb_user_device_send_done,
    .device_recv_done = (int(*)(tusb_device_interface_t *, uint8_t, const void *, uint16_t))tusb_user_device_recv_done,
};
