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
#include "tusbd_msc.h"
#include "tusb_msc.h"

#define MSC_STAGE_CMD    0
#define MSC_STAGE_DATA   1
#define MSC_STAGE_STATUS 2

#define min(a,b)  ((a) > (b) ? (b) : (a))

#define GET_BE16(field)  \
   ( ( (uint16_t)(field)[0]<<8)  \
   | ( (uint16_t)(field) [1])    )

#define GET_BE32(field)  \
   ( ( (uint32_t)(field)[0]<<24) \
   | ( (uint32_t)(field)[1]<<16) \
   | ( (uint32_t)(field)[2]<< 8) \
   | ( (uint32_t)(field)[3]<< 0) )

#define SET_BE16(field, value)  \
  do{                                      \
    (field)[0] = (uint8_t) ((value)>>8);   \
    (field)[1] = (uint8_t) ((value)>>0);   \
  }while(0)

#define SET_BE24(field, value)  \
  do{                                       \
    (field)[0] = (uint8_t) ((value)>>16);   \
    (field)[1] = (uint8_t) ((value)>> 8);   \
    (field)[2] = (uint8_t) ((value)>> 0);   \
  }while(0)

#define SET_BE32(field, value)  \
  do{                                      \
    (field)[0] = (uint8_t) ((value)>>24);  \
    (field)[1] = (uint8_t) ((value)>>16);  \
    (field)[2] = (uint8_t) ((value)>> 8);  \
    (field)[3] = (uint8_t) ((value)>> 0);  \
  }while(0)

static inline void msc_prepare_rx(tusb_msc_device_t *msc, void *data, uint16_t len)
{
    tusb_set_recv_buffer(msc->dev, msc->ep_out, data, len);
    tusb_set_rx_valid(msc->dev, msc->ep_out);
}

static inline void msc_send_data(tusb_msc_device_t *msc, const void *data, uint16_t len)
{
    msc->state.last_tx_length = len;
    tusb_send_data(msc->dev, msc->ep_in, data, len, 0);
}

static inline void msc_scsi_sense(tusb_msc_device_t *msc, uint8_t lun, uint8_t sense_key, uint8_t sense_code, uint8_t sense_qualifier)
{
    msc->state.sense_key = sense_key;
    msc->state.sense_code = sense_code;
    msc->state.sense_qualifier = sense_qualifier;
}

#define REQUEST_SENSE_DATA_LEN                      0x12
static int msc_scsi_request_sense(tusb_msc_device_t *msc)
{
    scsi_request_sense_cmd_t *cmd = (scsi_request_sense_cmd_t *)msc->state.cbw.command;
    if (cmd->descriptor)
    {
        return -1;
    }
    scsi_sense_fixed_resp_t *rsp = (scsi_sense_fixed_resp_t *)msc->state.data_buffer;
    memset(rsp, 0, REQUEST_SENSE_DATA_LEN);
    rsp->response_code = SCSI_SENSE_RESP_FIX_CURRENT;
    rsp->add_sense_len = REQUEST_SENSE_DATA_LEN - 6;
    rsp->sense_key = msc->state.sense_key;
    rsp->add_sense_code = msc->state.sense_code;
    rsp->add_sense_qualifier = msc->state.sense_qualifier;
    int len = (int)min(cmd->length, REQUEST_SENSE_DATA_LEN);
    return len;
}

static int msc_test_unit_ready(tusb_msc_device_t *msc)
{
    if (msc->scsi_ops && msc->scsi_ops->is_ready)
    {
        if (!msc->scsi_ops->is_ready(msc, msc->state.cbw.lun))
        {
            msc_scsi_sense(msc, msc->state.cbw.lun, SCSI_SENSE_NOT_READY, MEDIUM_NOT_PRESENT, 0);
            return -1;
        }
    }
    return 0;
}

#define STANDARD_INQUIRY_DATA_LEN    0x24
static int msc_scsi_inquiry(tusb_msc_device_t *msc)
{
    int len = -1;
    scsi_inquiry_cmd_t *cmd = (scsi_inquiry_cmd_t *)msc->state.cbw.command;
    memset(msc->state.data_buffer, 0, STANDARD_INQUIRY_DATA_LEN);
    if (msc->scsi_ops && msc->scsi_ops->inquiry)
    {
        len = msc->scsi_ops->inquiry(msc, msc->state.cbw.lun, cmd, msc->state.data_buffer);
        if (len > 0)
        {
            // this inquiry has been processed by the user
            return len;
        }
    }
    if (cmd->EVPD)
    {
        scsi_inquiry_vpd_response_t *vpd = (scsi_inquiry_vpd_response_t *)msc->state.data_buffer;
        vpd->page_code = cmd->page_code;
        switch (cmd->page_code)
        {
        case VPD_SUPPORTED_VPDS:
            len = 4 + 3;
            vpd->vpd_data[0] = VPD_SUPPORTED_VPDS;
            vpd->vpd_data[1] = VPD_UNIT_SERIAL_NUM;
            vpd->vpd_data[2] = VPD_DEVICE_ID;
            break;
        case VPD_UNIT_SERIAL_NUM:
            len = 4 + 10;
            memcpy(vpd->vpd_data, "TUSB123456", 10);
            break;
        case VPD_DEVICE_ID:
            len = 4;
            break;
        default:
            break;
        }
        SET_BE16(vpd->length, len);
    }
    else
    {
        scsi_inquiry_std_response_t *rsp = (scsi_inquiry_std_response_t *)msc->state.data_buffer;
        rsp->removable = 1;
        rsp->version = 2;
        rsp->response_data_format = 2;
        rsp->additional_length = STANDARD_INQUIRY_DATA_LEN - 5;
        memcpy(rsp->vid,  "TeenyUSB", 8);
        memcpy(rsp->pid, "MSC BOT DEMO    ", 16);
        memcpy(rsp->product_version, "1.0 ", 4);
        len = STANDARD_INQUIRY_DATA_LEN;
    }
    int a_len =  GET_BE16(cmd->length);
    msc->state.csw.data_residue = len;
    return min(len, a_len);
}

static int msc_scsi_start_stop_unit(tusb_msc_device_t *msc)
{
    scsi_start_stop_cmd_t *cmd = (scsi_start_stop_cmd_t *)msc->state.cbw.command;
    if (cmd->power_cond == SCSI_PWR_COND_START_VALID)
    {
        if (cmd->load_eject)
        {
            if (msc->scsi_ops && msc->scsi_ops->loadEject)
            {
                msc->scsi_ops->loadEject(msc, msc->state.cbw.lun, cmd->start);
            }
        }
    }
    return 0;
}

static int msc_scsi_prevent_allow_medium_removal(tusb_msc_device_t *msc)
{
    scsi_prevent_removal_cmd_t *cmd = (scsi_prevent_removal_cmd_t *)msc->state.cbw.command;
    if (cmd->prevent)
    {
        // always success
    }
    return 0;
}

static int msc_scsi_mode_select_6(tusb_msc_device_t *msc)
{
    msc_scsi_sense(msc, msc->state.cbw.lun, SCSI_SENSE_ILLEGAL_REQUEST, INVALID_CDB, 0);
    return -1;
}

static int msc_scsi_mode_sense_6(tusb_msc_device_t *msc)
{
    scsi_mode_sense_6_cmd_t *cmd = (scsi_mode_sense_6_cmd_t *)msc->state.cbw.command;
    scsi_mode_6_resp_header_t *rsp = (scsi_mode_6_resp_header_t *)msc->state.data_buffer;
    memset(msc->state.data_buffer, 0, sizeof(scsi_mode_6_resp_header_t));
    rsp->write_protect = 0;
    if (msc->scsi_ops && msc->scsi_ops->is_writable)
    {
        rsp->write_protect =
            msc->scsi_ops->is_writable(msc, msc->state.cbw.lun) ? 0 : 1;
    }
    rsp->mode_data_length = 3;
    msc->state.csw.data_residue = 4;
    return min(cmd->length, 4);
}

static int msc_scsi_mode_select_10(tusb_msc_device_t *msc)
{
    msc_scsi_sense(msc, msc->state.cbw.lun, SCSI_SENSE_ILLEGAL_REQUEST, INVALID_CDB, 0);
    return -1;
}

static int msc_scsi_mode_sense_10(tusb_msc_device_t *msc)
{
    scsi_mode_sense_10_cmd_t *cmd = (scsi_mode_sense_10_cmd_t *)msc->state.cbw.command;
    scsi_mode_10_resp_header_t *rsp = (scsi_mode_10_resp_header_t *)msc->state.data_buffer;
    memset(msc->state.data_buffer, 0, sizeof(scsi_mode_10_resp_header_t));
    rsp->write_protect = 0;
    if (msc->scsi_ops && msc->scsi_ops->is_writable)
    {
        rsp->write_protect =
            msc->scsi_ops->is_writable(msc, msc->state.cbw.lun) ? 0 : 1;
    }
    SET_BE16(rsp->block_desc_length, 6);
    int a_len =  GET_BE16(cmd->length);
    msc->state.csw.data_residue = 8;
    return min(8, a_len);
}

static int msc_scsi_read_format_capacity(tusb_msc_device_t *msc)
{
    uint8_t *buf = msc->state.data_buffer;
    memset(buf, 0, 12);
    if (msc->get_cap)
    {
        uint32_t block_count;
        uint32_t block_size;
        if (msc->get_cap(msc, msc->state.cbw.lun, &block_count, &block_size) >= 0)
        {
            buf[3] = 0x08U;
            SET_BE32(buf + 4, block_count);
            buf[8] = 0x02U;
            SET_BE24(buf + 9, block_size);
            msc->state.csw.data_residue = 12;
            return 12;
        }
    }
    msc_scsi_sense(msc, msc->state.cbw.lun, SCSI_SENSE_NOT_READY, MEDIUM_NOT_PRESENT, 0);
    return -1;
}

static int msc_scsi_read_capacity_10(tusb_msc_device_t *msc)
{
    //scsi_read_capacity_10_cmd_t* cmd = (scsi_read_capacity_10_cmd_t*)msc->state.cbw.command;
    scsi_read_capacity_10_resp_t *rsp = (scsi_read_capacity_10_resp_t *)msc->state.data_buffer;
    memset(rsp, 0, sizeof(scsi_read_capacity_10_resp_t));
    if (msc->get_cap)
    {
        uint32_t block_count;
        uint32_t block_size;
        if (msc->get_cap(msc, msc->state.cbw.lun, &block_count, &block_size) >= 0)
        {
            block_count -= 1;
            SET_BE32(rsp->last_logical_block_address, block_count);
            SET_BE32(rsp->block_size, block_size);
            msc->state.csw.data_residue = 8;
            return 8;
        }
    }
    msc_scsi_sense(msc, msc->state.cbw.lun, SCSI_SENSE_NOT_READY, MEDIUM_NOT_PRESENT, 0);
    return -1;
}

static void  msc_bot_abort(tusb_msc_device_t *msc)
{
    if ((msc->state.cbw.dir == 0U) &&
            (msc->state.cbw.total_bytes != 0U))
    {
        tusb_set_stall(msc->dev, msc->ep_out);
    }
    tusb_set_stall(msc->dev, msc->ep_in | 0x80);
    msc_prepare_rx(msc, &msc->state.cbw, BOT_CBW_LENGTH);
}

static void msc_send_csw(tusb_msc_device_t *msc, uint8_t status)
{
    msc->state.csw.signature = MSC_CSW_SIGNATURE;
    msc->state.csw.status = status;
    msc->state.stage = MSC_STAGE_STATUS;

    msc_send_data(msc, &msc->state.csw, BOT_CSW_LENGTH);
}

static int msc_scsi_read_10(tusb_msc_device_t *msc)
{
    tusb_msc_cbw_t        *cbw = &msc->state.cbw;
    tusb_msc_csw_t        *csw = &msc->state.csw;
    int length = csw->data_residue;
    if (length > 0)
    {
        scsi_read_10_cmd_t *cmd = (scsi_read_10_cmd_t *)cbw->command;
        uint32_t block_addr = GET_BE32(cmd->logical_block_addr);
        uint32_t block_size = cbw->total_bytes / GET_BE16(cmd->transfer_length);
        length = (int) min(sizeof(msc->state.data_buffer), length);
        uint16_t block_count = length / block_size;
        int xferred_length = cbw->total_bytes - csw->data_residue;
        // TODO: support data buffer length less than the block size
        if (msc->block_read)
        {
            length = msc->block_read(msc, cbw->lun, msc->state.data_buffer, block_addr + xferred_length / block_size, block_count);
        }
        else
        {
            length = -1;
        }
        if (length < 0)
        {
            msc_scsi_sense(msc, cbw->lun, SCSI_SENSE_MEDIUM_ERROR, UNRECOVERED_READ_ERROR, 0);
            return -1;
        }
    }
    return length;
}

static int msc_scsi_write_10(tusb_msc_device_t *msc)
{
    tusb_msc_cbw_t *cbw = &msc->state.cbw;
    tusb_msc_csw_t        *csw = &msc->state.csw;
    scsi_read_10_cmd_t *cmd = (scsi_read_10_cmd_t *)cbw->command;

    if (msc->state.stage == MSC_STAGE_CMD)
    {
        int ready = 1;
        if (msc->scsi_ops && msc->scsi_ops->is_ready)
        {
            ready = msc->scsi_ops->is_ready(msc, cbw->lun);
        }
        if (!ready)
        {
            msc_scsi_sense(msc, cbw->lun, SCSI_SENSE_MEDIUM_ERROR, MEDIUM_NOT_PRESENT, 0);
            return -1;
        }
        if (msc->scsi_ops && msc->scsi_ops->is_writable)
        {
            ready = msc->scsi_ops->is_writable(msc, cbw->lun);
        }
        if (!ready)
        {
            msc_scsi_sense(msc, cbw->lun, SCSI_SENSE_DATA_PROTECT, WRITE_PROTECTED, 0);
            return -1;
        }
        int length = (int) min(sizeof(msc->state.data_buffer), csw->data_residue);
        return length;
    }
    else if (msc->state.stage == MSC_STAGE_DATA)
    {
        uint32_t block_addr = GET_BE32(cmd->logical_block_addr);
        uint32_t block_size = cbw->total_bytes / GET_BE16(cmd->transfer_length);
        int length = msc->state.data_out_length;
        uint16_t block_count = length / block_size;
        if (msc->block_write)
        {
            // TODO: support data buffer length less than the block size
            int xferred_length = cbw->total_bytes - csw->data_residue;
            length = msc->block_write(msc, cbw->lun, msc->state.data_buffer, block_addr + xferred_length / block_size, block_count);
        }
        else
        {
            length = -1;
        }
        if (length < 0)
        {
            msc_scsi_sense(msc, cbw->lun, SCSI_SENSE_MEDIUM_ERROR, WRITE_FAULT, 0);
            return -1;
        }
        csw->data_residue -= length;
        length = min(sizeof(msc->state.data_buffer), csw->data_residue);
        return length;
    }
    return -1;
}

static int msc_scsi_verify_10(tusb_msc_device_t *msc)
{
    return 0;
}

// return >0 means command inovke success, and need to send data
// return 0 means command invoke success, and no need to send data
// return <0 means error occurs
static int msc_scsi_command(tusb_msc_device_t *msc)
{
    switch (msc->state.cbw.command[0])
    {
    case SCSI_CMD_TEST_UNIT_READY:
        return msc_test_unit_ready(msc);
    case SCSI_CMD_REQUEST_SENSE:
        return msc_scsi_request_sense(msc);
    case SCSI_CMD_INQUIRY:
        return msc_scsi_inquiry(msc);
    case SCSI_CMD_START_STOP_UNIT:
        return msc_scsi_start_stop_unit(msc);
    case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
        return msc_scsi_prevent_allow_medium_removal(msc);
    case SCSI_CMD_MODE_SELECT_6:
        return msc_scsi_mode_select_6(msc);
    case SCSI_CMD_MODE_SENSE_6:
        return msc_scsi_mode_sense_6(msc);
    case SCSI_CMD_MODE_SELECT_10:
        return msc_scsi_mode_select_10(msc);
    case SCSI_CMD_MODE_SENSE_10:
        return msc_scsi_mode_sense_10(msc);
    case SCSI_CMD_READ_FORMAT_CAPACITY:
        return msc_scsi_read_format_capacity(msc);
    case SCSI_CMD_READ_CAPACITY_10:
        return msc_scsi_read_capacity_10(msc);
    case SCSI_CMD_READ_10:
        return msc_scsi_read_10(msc);
    case SCSI_CMD_WRITE_10:
        return msc_scsi_write_10(msc);
    case SCSI_CMD_VERIFY_10:
        return msc_scsi_verify_10(msc);
    case SCSI_CMD_SYNC_CACHE_10:
        // always write through
        return 0;
    }
    msc_scsi_sense(msc, msc->state.cbw.lun, SCSI_SENSE_ILLEGAL_REQUEST, INVALID_CDB, 0);
    return -1;
}

static void handle_msc_scsi_command(tusb_msc_device_t *msc)
{
    int res = 0;
    if (msc->state.stage == MSC_STAGE_CMD || msc->state.csw.data_residue)
    {
        res = msc_scsi_command(msc);
    }
    if (res > 0)
    {
        msc->state.stage = MSC_STAGE_DATA;
        if ((msc->state.cbw.dir & 0x80) == 0x80)
        {
            msc_send_data(msc, msc->state.data_buffer, res);
        }
        else
        {
            msc_prepare_rx(msc, msc->state.data_buffer, res);
        }
    }
    else if (res < 0)
    {
        if (msc->state.cbw.dir & 0x80)
        {
            tusb_set_stall(msc->dev, msc->ep_in | 0x80);
        }
        msc_send_csw(msc, MSC_CSW_STATUS_FAILED);
    }
    else
    {
        msc_send_csw(msc, MSC_CSW_STATUS_PASSED);
    }
}

static void msc_data_out(tusb_msc_device_t *msc)
{
    switch (msc->state.stage)
    {
    case MSC_STAGE_CMD:
        if (msc->state.cbw.signature != MSC_CBW_SIGNATURE || msc->state.data_out_length != BOT_CBW_LENGTH)
        {
            // Got an error command
            msc_scsi_sense(msc, msc->state.cbw.lun, SCSI_SENSE_ILLEGAL_REQUEST, INVALID_CDB, 0);
            msc_bot_abort(msc);
            return;
        }
        msc->state.csw.signature = MSC_CSW_SIGNATURE;
        msc->state.csw.tag = msc->state.cbw.tag;
        msc->state.csw.data_residue = msc->state.cbw.total_bytes;
        handle_msc_scsi_command(msc);
        break;
    case MSC_STAGE_DATA:
        handle_msc_scsi_command(msc);
        break;
    default:
        msc->state.stage = MSC_STAGE_CMD;
        msc_prepare_rx(msc, &msc->state.cbw, BOT_CBW_LENGTH);
        break;
    }
}


static void msc_data_in(tusb_msc_device_t *msc)
{
    switch (msc->state.stage)
    {
    case MSC_STAGE_CMD:
        // should never reach here
        break;
    case MSC_STAGE_DATA:
        msc->state.csw.data_residue -= msc->state.last_tx_length;
        handle_msc_scsi_command(msc);
        break;
    default:
        msc->state.stage = MSC_STAGE_CMD;
        msc_prepare_rx(msc, &msc->state.cbw, BOT_CBW_LENGTH);
        break;
    }
}

static void tusb_msc_device_init(tusb_msc_device_t *msc)
{
    msc_prepare_rx(msc, &msc->state.cbw, BOT_CBW_LENGTH);
    msc->state.stage = MSC_STAGE_CMD;
    if (msc->scsi_ops && msc->scsi_ops->init)
    {
        msc->scsi_ops->init(msc);
    }
}

static int tusb_msc_device_request(tusb_msc_device_t *msc, tusb_setup_packet *setup_req)
{
    tusb_device_config_t *dev_config = (tusb_device_config_t *)msc->dev->user_data;
    if ((setup_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS)
    {
        switch (setup_req->bRequest)
        {
        case BOT_GET_MAX_LUN :
            if ((setup_req->wValue  == 0) &&
                    (setup_req->wLength == 1) &&
                    ((setup_req->bmRequestType & 0x80) == 0x80))
            {
                dev_config->cmd_buffer[0] = msc->max_lun;
                tusb_control_send(msc->dev, dev_config->cmd_buffer, 1);
            }
            return 1;
        case BOT_RESET :
            if ((setup_req->wValue  == 0) &&
                    (setup_req->wLength == 0) &&
                    ((setup_req->bmRequestType & 0x80) != 0x80))
            {
                if (msc->scsi_ops && msc->scsi_ops->reset)
                {
                    msc->scsi_ops->reset(msc);
                }
            }
            tusb_send_status(msc->dev);
            return 1;
        }
    }
    return 0;
}


void tusb_msc_device_loop(tusb_msc_device_t *msc)
{
    if (msc->state.data_out_length)
    {
        msc_data_out(msc);
        msc->state.data_out_length = 0;
    }
    if (msc->state.data_in_length)
    {
        msc_data_in(msc);
        msc->state.data_in_length = 0;
    }
}

static int tusb_msc_device_send_done(tusb_msc_device_t *msc, uint8_t EPn)
{
    msc->state.data_in_length = msc->state.last_tx_length;
    return 0;
}

static int tusb_msc_device_recv_done(tusb_msc_device_t *msc, uint8_t EPn, const void *data, uint16_t len)
{
    msc->state.data_out_length = len;
    return 1; // Mark the out ep invalid, it will be enable after process the command
}

const tusb_device_backend_t msc_device_backend =
{
    .device_init = (int(*)(tusb_device_interface_t *))tusb_msc_device_init,
    .device_request = (int(*)(tusb_device_interface_t *, tusb_setup_packet *))tusb_msc_device_request,
    .device_send_done = (int(*)(tusb_device_interface_t *, uint8_t))tusb_msc_device_send_done,
    .device_recv_done = (int(*)(tusb_device_interface_t *, uint8_t, const void *, uint16_t))tusb_msc_device_recv_done,
};

