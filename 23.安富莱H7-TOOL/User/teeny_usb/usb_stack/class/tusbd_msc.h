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

#ifndef __TUSBD_MSC_H__
#define __TUSBD_MSC_H__
#include "tusbd.h"
#include "tusb_msc.h"
#ifndef MSC_DATA_PACKET_LENGTH
    #define MSC_DATA_PACKET_LENGTH   512
#endif

typedef struct _tusb_msc_device tusb_msc_device_t;

typedef struct _tusb_msc_state
{
    tusb_msc_cbw_t cbw;
    tusb_msc_csw_t csw;
    uint8_t data_buffer[MSC_DATA_PACKET_LENGTH];

    uint16_t data_out_length;
    uint16_t data_in_length;
    uint16_t last_tx_length;
    uint8_t stage;
    uint8_t sense_key;
    uint8_t sense_code;
    uint8_t sense_qualifier;

} tusb_msc_state_t;

typedef struct _tusb_scsi_ops
{
    int (*init)(tusb_msc_device_t *msc);
    int (*reset)(tusb_msc_device_t *msc);
    int (*is_ready)(tusb_msc_device_t *msc, uint8_t lun);
    int (*is_writable)(tusb_msc_device_t *msc, uint8_t lun);
    int (*inquiry)(tusb_msc_device_t *msc, uint8_t lun, const scsi_inquiry_cmd_t *cmd, void *inquiry_data);
    int (*loadEject)(tusb_msc_device_t *msc, uint8_t lun, uint8_t isLoad);
} tusb_scsi_ops_t;

struct _tusb_msc_device
{
    tusb_device_t *dev;
    const tusb_device_backend_t *backend;
    uint8_t ep_in;
    uint8_t ep_out;
    uint8_t max_lun;
    uint8_t pad1;
    int (*get_cap)(tusb_msc_device_t *msc, uint8_t lun, uint32_t *block_num, uint32_t *block_size);
    // return actual bytes read/write, or -1 to indicates an error
    int (*block_read)(tusb_msc_device_t *msc, uint8_t lun, uint8_t *buf, uint32_t block_addr, uint16_t block_len);
    int (*block_write)(tusb_msc_device_t *msc, uint8_t lun, const uint8_t *buf, uint32_t block_addr, uint16_t block_len);
    const tusb_scsi_ops_t *scsi_ops; /**<  Additional scsi handlers */
    void *user_data;
    tusb_msc_state_t state;
};

extern const tusb_device_backend_t msc_device_backend;


void tusb_msc_device_loop(tusb_msc_device_t *msc);

#endif
