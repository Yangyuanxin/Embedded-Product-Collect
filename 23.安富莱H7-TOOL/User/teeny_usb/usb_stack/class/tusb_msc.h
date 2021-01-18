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

#ifndef __TUSB_MSC_H__
#define __TUSB_MSC_H__

#include "teeny_usb.h"
#include "tusb_scsi.h"

#define MSC_CBW_SIGNATURE   0x43425355
#define MSC_CSW_SIGNATURE   0x53425355

#define BOT_GET_MAX_LUN  0xfe
#define BOT_RESET        0xff

#define BOT_CBW_LENGTH                31
#define BOT_CSW_LENGTH                13


#define MSC_CSW_STATUS_PASSED      0
#define MSC_CSW_STATUS_FAILED      1
#define MSC_CSW_STATUS_PHASE_ERROR 2



typedef struct _tusb_msc_cbw
{
    uint32_t signature;
    uint32_t tag;
    uint32_t total_bytes;
    uint8_t dir;   // 0x80: in,  0x00 : out
    uint8_t lun;
    uint8_t cmd_len;
    uint8_t command[16];
    uint8_t pad1;   // pad to 32bit boundary
} tusb_msc_cbw_t;

typedef struct _tusb_msc_csw
{
    uint32_t signature;
    uint32_t tag;
    uint32_t data_residue;
    uint8_t  status;
    uint8_t  pad[3]; // pad to 32bit boundary
} tusb_msc_csw_t;


#endif
