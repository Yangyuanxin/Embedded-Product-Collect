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

#ifndef __TUSB_SCSI_H__
#define __TUSB_SCSI_H__
#include "teeny_usb.h"


#define  SCSI_CMD_TEST_UNIT_READY               0x00
#define  SCSI_CMD_REQUEST_SENSE                 0x03
#define  SCSI_CMD_INQUIRY                       0x12
#define  SCSI_CMD_MODE_SELECT_6                 0x15
#define  SCSI_CMD_MODE_SENSE_6                  0x1A
#define  SCSI_CMD_MODE_SELECT_10                0x55
#define  SCSI_CMD_MODE_SENSE_10                 0x5A
#define  SCSI_CMD_START_STOP_UNIT               0x1B
#define  SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL  0x1E
#define  SCSI_CMD_READ_CAPACITY_10              0x25
#define  SCSI_CMD_READ_FORMAT_CAPACITY          0x23
#define  SCSI_CMD_READ_10                       0x28
#define  SCSI_CMD_WRITE_10                      0x2A
#define  SCSI_CMD_VERIFY_10                     0x2F
#define  SCSI_CMD_SYNC_CACHE_10                 0x35

#define SCSI_SENSE_NONE             0x00
#define SCSI_SENSE_RECOVERED_ERROR  0x01
#define SCSI_SENSE_NOT_READY        0x02
#define SCSI_SENSE_MEDIUM_ERROR     0x03
#define SCSI_SENSE_HARDWARE_ERROR   0x04
#define SCSI_SENSE_ILLEGAL_REQUEST  0x05
#define SCSI_SENSE_UNIT_ATTENTION   0x06
#define SCSI_SENSE_DATA_PROTECT     0x07
#define SCSI_SENSE_FIRMWARE_ERROR   0x08
#define SCSI_SENSE_ABORTED_COMMAND  0x0b
#define SCSI_SENSE_EQUAL            0x0c
#define SCSI_SENSE_VOLUME_OVERFLOW  0x0d
#define SCSI_SENSE_MISCOMPARE       0x0e


#define INVALID_CDB                                 0x20
#define INVALID_FIELED_IN_COMMAND                   0x24
#define PARAMETER_LIST_LENGTH_ERROR                 0x1A
#define INVALID_FIELD_IN_PARAMETER_LIST             0x26
#define ADDRESS_OUT_OF_RANGE                        0x21
#define MEDIUM_NOT_PRESENT                          0x3A
#define MEDIUM_HAVE_CHANGED                         0x28
#define WRITE_PROTECTED                             0x27
#define UNRECOVERED_READ_ERROR                      0x11
#define WRITE_FAULT                                 0x03



typedef struct
{
    uint8_t cmd_code;            /**< SCSI OpCode for \ref SCSI_CMD_INQUIRY */
    uint8_t EVPD:       1;       /**< Enable Vital Product Data */
    uint8_t CMDDT:      1;       /**< Command Support Data */
    uint8_t reserved1:  6;
    uint8_t page_code;           /**< Page code */
    uint8_t length[2];           /**< allocate length for IN data */
    uint8_t control;             /**< */
} scsi_inquiry_cmd_t;

typedef enum
{
    SCSI_DT_BLOCK_DEVICE = 0x00,              /**< Direct access block device (e.g., magnetic disk) */
    SCSI_DT_SEQ_DEVICE   = 0x01,              /**< Sequential-access device (e.g., magnetic tape) */
    SCSI_DT_PRINTER      = 0x02,              /**< printer */
    SCSI_DT_PROCESSOR    = 0x03,              /**< processor device */
    SCSI_DT_WRITE_ONCE   = 0x04,              /**< write once device */
    SCSI_DT_CD_DVD       = 0x05,              /**< CD/DVD */
} scsi_peripheral_device_type_t;

typedef struct
{
    uint8_t peripheral_device_type: 5;   /**< \ref scsi_peripheral_device_type */
    uint8_t peripheral_qualifier  : 3;

    uint8_t reserved1 : 7;
    uint8_t removable : 1;

    uint8_t version;

    uint8_t response_data_format : 4;  /**< muse be 2 */
    uint8_t hierarchical_support : 1;
    uint8_t normal_aca           : 1;
    uint8_t reserved2            : 2;

    uint8_t additional_length;

    uint8_t protect                    : 1;
    uint8_t reserved3                  : 2;
    uint8_t third_party_copy           : 1;
    uint8_t target_port_group_support  : 2;
    uint8_t access_control_coordinator : 1;
    uint8_t scc_support                : 1;

    uint8_t addr16                     : 1;
    uint8_t reserved4                  : 3;
    uint8_t multi_port                 : 1;
    uint8_t vendor1                    : 1; // vendor specific
    uint8_t enclosure_service          : 1;
    uint8_t reserved5                  : 1;

    uint8_t vendor2                    : 1; // vendor specific
    uint8_t cmd_queue                  : 1;
    uint8_t reserved6                  : 6;

    uint8_t vid[8];
    uint8_t pid[16];
    uint8_t product_version[4];
} scsi_inquiry_std_response_t;



/* VPD pages (fetched by INQUIRY command) */
#define VPD_SUPPORTED_VPDS 0x0
#define VPD_UNIT_SERIAL_NUM 0x80
#define VPD_IMP_OP_DEF 0x81     /* obsolete in SPC-2 */
#define VPD_ASCII_OP_DEF 0x82   /* obsolete in SPC-2 */
#define VPD_DEVICE_ID 0x83
#define VPD_SOFTW_INF_ID 0x84
#define VPD_MAN_NET_ADDR 0x85
#define VPD_EXT_INQ 0x86
#define VPD_MODE_PG_POLICY 0x87
#define VPD_SCSI_PORTS 0x88
#define VPD_ATA_INFO 0x89
#define VPD_POWER_CONDITION 0x8a
#define VPD_DEVICE_CONSTITUENTS 0x8b
#define VPD_CFA_PROFILE_INFO 0x8c
#define VPD_POWER_CONSUMPTION 0x8d
#define VPD_3PARTY_COPY 0x8f
#define VPD_PROTO_LU 0x90
#define VPD_PROTO_PORT 0x91
#define VPD_SCSI_FEATURE_SETS 0x92      /* spc5r11 */
#define VPD_BLOCK_LIMITS 0xb0           /* SBC-3 */
#define VPD_SA_DEV_CAP 0xb0             /* SSC-3 */
#define VPD_OSD_INFO 0xb0               /* OSD */
#define VPD_BLOCK_DEV_CHARS 0xb1        /* SBC-3 */
#define VPD_MAN_ASS_SN 0xb1             /* SSC-3, ADC-2 */
#define VPD_SECURITY_TOKEN 0xb1         /* OSD */
#define VPD_ES_DEV_CHARS 0xb1           /* SES-4 */
#define VPD_TA_SUPPORTED 0xb2           /* SSC-3 */
#define VPD_LB_PROVISIONING 0xb2        /* SBC-3 */
#define VPD_REFERRALS 0xb3              /* SBC-3 */
#define VPD_AUTOMATION_DEV_SN 0xb3      /* SSC-3 */
#define VPD_SUP_BLOCK_LENS 0xb4         /* sbc4r01 */
#define VPD_DTDE_ADDRESS 0xb4           /* SSC-4 */
#define VPD_BLOCK_DEV_C_EXTENS 0xb5     /* sbc4r02 */
#define VPD_LB_PROTECTION 0xb5          /* SSC-5 */
#define VPD_ZBC_DEV_CHARS 0xb6          /* zbc-r01b */
#define VPD_BLOCK_LIMITS_EXT 0xb7       /* sbc4r08 */
#define VPD_NOT_STD_INQ -2      /* request for standard inquiry */


// Vital product data
typedef struct
{
    uint8_t peripheral_device_type: 5;   /**< \ref scsi_peripheral_device_type */
    uint8_t peripheral_qualifier  : 3;
    uint8_t page_code;
    uint8_t length[2];
    uint8_t vpd_data[4];         /**<  Variant length VPD data */
} scsi_inquiry_vpd_response_t;

// Power condition
#define SCSI_PWR_COND_START_VALID  0x00
#define SCSI_PWR_COND_ACTIVE       0x01
#define SCSI_PWR_COND_IDLE         0x02
#define SCSI_PWR_COND_STANDBY      0x03
#define SCSI_PWR_COND_LU_CONTROL    0x07
#define SCSI_PWR_COND_FORCE_IDLE_0    0x0a
#define SCSI_PWR_COND_FORCE_STANDBY_0 0x0b

typedef struct
{
    uint8_t cmd_code;

    uint8_t immediate: 1;
    uint8_t reserved1: 7;

    uint8_t reserved2;

    uint8_t power_cond_modifier: 4;
    uint8_t reserved3: 4;

    uint8_t start: 1;
    uint8_t load_eject: 1;
    uint8_t no_flush: 1;
    uint8_t reserved4: 1;
    uint8_t power_cond: 4;   /**  \ref  SCSI_PC_xxx */

    uint8_t control;
} scsi_start_stop_cmd_t;


#define SCSI_PWR_CTRL_CURRENT     0
#define SCSI_PWR_CTRL_CHANGEABLE  1
#define SCSI_PWR_CTRL_DEFAULT     2
#define SCSI_PWR_CTRL_SAVED       3

typedef struct
{
    uint8_t cmd_code;

    uint8_t reserved1: 3;
    uint8_t disable_block_descriptor: 1;
    uint8_t reserved2: 4;

    uint8_t page_code: 6;
    uint8_t page_control: 2;

    uint8_t subpage_code;
    uint8_t length;
    uint8_t control;
} scsi_mode_sense_6_cmd_t;

typedef struct
{
    uint8_t mode_data_length;
    uint8_t medium_type;

    uint8_t reserved1: 4;
    uint8_t DPO_FUA: 1;    /**< [Disable Page Out] and [Force Unit Access] in the Read10 command is valid or not */
    uint8_t reserved2: 2;
    uint8_t write_protect: 1;

    uint8_t block_desc_length;
} scsi_mode_6_resp_header_t;


typedef struct
{
    uint8_t cmd_code;

    uint8_t reserved1: 3;
    uint8_t disable_block_descriptor: 1;
    uint8_t long_LBA: 1;
    uint8_t reserved2: 3;

    uint8_t page_code: 6;
    uint8_t page_control: 2;

    uint8_t subpage_code;

    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;

    uint8_t length[2];

    uint8_t control;
} scsi_mode_sense_10_cmd_t;

typedef struct
{
    uint8_t mode_data_length_high;
    uint8_t mode_data_length_low;
    uint8_t medium_type;

    uint8_t reserved1: 4;
    uint8_t DPO_FUA: 1;    /**< [Disable Page Out] and [Force Unit Access] in the Read10 command is valid or not */
    uint8_t reserved2: 2;
    uint8_t write_protect: 1;

    uint8_t long_LBA: 1;
    uint8_t reserved3: 7;

    uint8_t reserved4;
    uint8_t block_desc_length[2];
} scsi_mode_10_resp_header_t;

typedef struct
{
    uint8_t cmd_code;
    uint8_t descriptor: 1;
    uint8_t reserved1: 7;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t length;
    uint8_t control;
} scsi_request_sense_cmd_t;


#define SCSI_SENSE_RESP_FIX_CURRENT    0x70
#define SCSI_SENSE_RESP_FIX_DEFERRED   0x71
#define SCSI_SENSE_RESP_DESC_CURRENT   0x72
#define SCSI_SENSE_RESP_DESC_DEFERRED  0x73

typedef struct
{
    uint8_t response_code : 7;
    uint8_t valid         : 1;

    uint8_t reserved1;

    uint8_t sense_key     : 4;
    uint8_t reserved2    : 1;
    uint8_t incorrect_length: 1; /**< Incorrect length indicator */
    uint8_t end_of_medium : 1;
    uint8_t filemark      : 1;

    uint8_t  information[4];
    uint8_t  add_sense_len;
    uint8_t  command_specific_info[4];
    uint8_t  add_sense_code;
    uint8_t  add_sense_qualifier;
    uint8_t  field_replaceable_unit_code;

    uint8_t  sense_ks_2: 7;
    uint8_t  sense_ks_valid: 1;

    uint8_t  sense_ks_1;
    uint8_t  sense_ks_0;

} scsi_sense_fixed_resp_t;



typedef struct
{
    uint8_t cmd_code;

    uint8_t  reserved1: 2;
    uint8_t  RARC: 1;    /**<  rebuild assist recovery control */
    uint8_t  FUA: 1;     /**<  Force Unit Access */
    uint8_t  DPO: 1;     /**<  Disable Page Out */
    uint8_t  protect_info: 3;

    uint8_t logical_block_addr[4];

    uint8_t group_number: 5;
    uint8_t reserved2: 3;

    uint8_t transfer_length[2];

    uint8_t control;

} scsi_read_10_cmd_t, scsi_write_10_cmd_t;

typedef struct
{
    uint8_t  cmd_code;
    uint8_t  reserved1;
    uint8_t  logical_block_address[4];
    uint8_t  reserved2[2];
    uint8_t  partial_medium_indicator: 1;
    uint8_t  reserved3: 7;
    uint8_t  control;
} scsi_read_capacity_10_cmd_t;

typedef struct
{
    uint8_t last_logical_block_address[4];
    uint8_t block_size[4];
} scsi_read_capacity_10_resp_t;


typedef struct
{
    uint8_t  cmd_code;
    uint8_t  reserved1;
    uint8_t  reserved2;
    uint8_t  prevent: 2;
    uint8_t  reserved3: 6;
    uint8_t  control;
} scsi_prevent_removal_cmd_t;


#endif
