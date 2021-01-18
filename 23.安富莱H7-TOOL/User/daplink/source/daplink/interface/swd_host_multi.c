/**
 * @file    MUL_swd_host.c
 * @brief   Implementation of MUL_swd_host.h
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2019, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TARGET_MCU_CORTEX_A
//#include "cmsis_os2.h"
#include "target_reset.h"
#include "target_config.h"
#include "swd_host_multi.h"
#include "debug_cm.h"
#include "DAP_config.h"
#include "DAP.h"
#include "target_family.h"
#include "prog_if.h"
#include "file_lib.h"
#include "SW_DP_Multi.h"

// Default NVIC and Core debug base addresses
// TODO: Read these addresses from ROM.
#define NVIC_Addr    (0xe000e000)
#define DBG_Addr     (0xe000edf0)

// AP CSW register, base value
#define CSW_VALUE (CSW_RESERVED | CSW_MSTRDBG | CSW_HPROT | CSW_DBGSTAT | CSW_SADDRINC)

#define DCRDR 0xE000EDF8
#define DCRSR 0xE000EDF4
#define DHCSR 0xE000EDF0
#define REGWnR (1 << 16)

#define MAX_SWD_RETRY 1000       //10
#define MAX_TIMEOUT   1000000  // Timeout for syscalls on target

//#define SCB_AIRCR_PRIGROUP_Pos              8                                             /*!< SCB AIRCR: PRIGROUP Position */
#define SCB_AIRCR_PRIGROUP_Msk             (7UL << SCB_AIRCR_PRIGROUP_Pos)                /*!< SCB AIRCR: PRIGROUP Mask */

typedef struct {
    uint32_t select;
    uint32_t csw;
} DAP_STATE;

typedef struct {
    uint32_t r[16];
    uint32_t xpsr;
} DEBUG_STATE;


extern SWD_CONNECT_TYPE reset_connect;
extern const char * swd_arm_core_info(uint32_t cpuid);
//static SWD_CONNECT_TYPE reset_connect = CONNECT_NORMAL;

static DAP_STATE dap_state;
static uint32_t  soft_reset = SYSRESETREQ;

static uint8_t s_reset_state = 0;

/*
    判断ACK = DAP_TRANSFER_OK
*/
uint8_t MUL_CheckAckTransferOk(uint8_t *_ack)
{
    uint8_t err = 0;
    uint8_t i;
    
    if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
    {
        for (i = 0; i < 4; i++)
        {
            if (g_gMulSwd.Active[i] == 1)
            {
                if (_ack[i] != DAP_TRANSFER_OK)
                {
                    err++;
                }
            }
        }
    }
    else            
    {
        err = 1;
        for (i = 0; i < 4; i++)
        {
            if (g_gMulSwd.Active[i] == 1)
            {
                if (_ack[i] != DAP_TRANSFER_OK)
                {
                    ;
                }
                else
                {
                    err = 0;
                }
            }
        }        
    }
    
    if (err == 0)
    {
        return 1;
    }
    
    return 0;   
}

void MUL_swd_set_target_reset(uint8_t asserted)
{       
//    if(asserted == 0)
//	{
//		swd_write_word((uint32_t)&SCB->AIRCR, ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | SCB_AIRCR_SYSRESETREQ_Msk));
//	}   
//    
    if (asserted)
    {
        s_reset_state = 1;        
//        printf("reset gpio = %d\r\n", 0);
        EIO_SetOutLevel(EIO_D0, 1);        
    }
    else
    {
        s_reset_state = 0;
//        printf("reset gpio = %d\r\n", 1);        
        EIO_SetOutLevel(EIO_D0, 0);     /* 转接板NRESET 反相 */
    }
}


uint8_t MUL_swd_get_target_reset(void)
{    
    return s_reset_state;
}

uint32_t MUL_target_get_apsel()
{
//    if (g_target_family && g_target_family->apsel) {
//        return g_target_family->apsel;
//    } else {
//        return 0;
//    }
    return 0;
}


static uint32_t MUL_swd_get_apsel(uint32_t adr)
{
    uint32_t apsel = MUL_target_get_apsel();
    if (!apsel)
        return adr & 0xff000000;
    else
        return apsel;
}

void MUL_swd_set_reset_connect(SWD_CONNECT_TYPE type)
{
//    reset_connect = type;
}

/* 小端转换 */
void MUL_int2array(uint8_t *res, uint32_t data, uint8_t len)
{
    uint8_t i = 0;

    for (i = 0; i < len; i++) {
        res[i] = (data >> 8 * i) & 0xff;
    }
}

/* 1拖4用    
    data : 缓冲区，4个单元
    返回值: 1表示正确

    // DAP Transfer Response
    #define DAP_TRANSFER_OK                 (1U<<0)
    #define DAP_TRANSFER_WAIT               (1U<<1)
    #define DAP_TRANSFER_FAULT              (1U<<2)
    #define DAP_TRANSFER_ERROR              (1U<<3)
    #define DAP_TRANSFER_MISMATCH           (1U<<4)    
*/
uint8_t *MUL_swd_transfer_retry(uint32_t req, uint32_t *data)
{
    uint8_t *ack; 
    static uint8_t  ret_ack[4];
    uint32_t done = 0;
    uint8_t i;
    uint16_t err_cout[4];

    /* */
    for (i = 0; i < 4; i++)
    {
        err_cout[i] = 0;
        
        ret_ack[i] = 0;     /* 函数返回 */
        
        g_gMulSwd.Ignore[i] = 0;            /* 先不忽略 */
        g_gMulSwd.TempIgnore[i] = 0; 
        if (g_gMulSwd.Active[i] == 0)       /* 通道未激活 */
        {
            g_gMulSwd.Ignore[i] = 1;        /* 传送数据时，忽略该通道 */            
            done |= (1 << i);
        }
        else    /* 通道已经激活 */
        {
//            if (g_gMulSwd.Error[i] != 0)    /* 该通道已经错误，后续不在重试 */
//            {
//                g_gMulSwd.Ignore[i] = 1;    /* 传送数据时，忽略该通道 */
//                done |= (1 << i);
//            }
        }
    }
    
    /* 四个通道全部传输完毕才结束 */
    while (done != 0x0F)
    {
        ack = MUL_SWD_Transfer(req, data);
        
        for (i = 0; i < 4; i++)
        {
            if (g_gMulSwd.Ignore[i] == 0)
            {
                if (ack[i] == DAP_TRANSFER_OK)
                {
                    ret_ack[i] = DAP_TRANSFER_OK;
                    done |= (1 << i);
                    g_gMulSwd.Ignore[i] = 1;            /* 传送数据时，忽略该通道 */
                }                
                else if (ack[i] == DAP_TRANSFER_WAIT)
                {
                    if (++err_cout[i] >= MAX_SWD_RETRY)
                    {
                        done |= (1 << i);
                        g_gMulSwd.Ignore[i] = 1; 
                        ret_ack[i] = DAP_TRANSFER_WAIT;                        
                        g_gMulSwd.Error[i] = 1;         /* 设置错误标志 */       
                    }
                }
                else    /* 错误 */
                {
                    done |= (1 << i);
                    g_gMulSwd.Ignore[i] = 1; 
                    ret_ack[i] = DAP_TRANSFER_ERROR;                    
                    g_gMulSwd.Error[i] = 1;             /* 设置错误标志 */   
                }                
            }
        }        
    }    
    return ret_ack;
}

void MUL_swd_set_soft_reset(uint32_t soft_reset_type)
{
    soft_reset = soft_reset_type;
}

uint8_t MUL_swd_init(void)
{
    static uint8_t s_first_run = 0;
    
    //TODO - DAP_Setup puts GPIO pins in a hi-z state which can
    //       cause problems on re-init.  This needs to be investigated
    //       and fixed.
    DAP_Setup();
    
    
    //    Set SWCLK HIGH
    //    Set SWDIO HIGH
    //    Set RESET LOW  转接板有反相器三极管
    MUL_PORT_SWD_SETUP();

    if (s_first_run == 0)
    {
        s_first_run = 1;
        EIO_SetOutLevel(0, 0);    /* D0输出0V, 转接板RESET输出高 */    
    }
    
    return 1;
}

uint8_t MUL_swd_off(void)
{
    PORT_OFF();
    return 1;
}

uint8_t MUL_swd_clear_errors(void)
{
    if (!MUL_swd_write_dp(DP_ABORT, STKCMPCLR | STKERRCLR | WDERRCLR | ORUNERRCLR)) {
        return 0;
    }
    return 1;
}

// Read debug port register.
uint8_t * MUL_swd_read_dp(uint8_t adr, uint32_t *val)
{
    uint32_t tmp_in;
//    uint8_t tmp_out[4];
//    uint8_t *ack;
//    uint32_t tmp;
    
    tmp_in = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(adr);
//    ack = MUL_swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
//    
//    val[0] = 0;
//    tmp = tmp_out[3];
//    val[0] |= (tmp << 24);
//    tmp = tmp_out[2];
//    val[0] |= (tmp << 16);
//    tmp = tmp_out[1];
//    val[0] |= (tmp << 8);
//    tmp = tmp_out[0];
//    val[0] |= (tmp << 0);
//    ack = MUL_swd_transfer_retry(tmp_in, val);
//    return (ack == 0x01);
    return MUL_swd_transfer_retry(tmp_in, val);
}

// Write debug port register
uint8_t MUL_swd_write_dp(uint8_t adr, uint32_t val)
{
    uint32_t req;
    uint8_t data[4];
    uint8_t ack;
    uint8_t *pAck;
    //check if the right bank is already selected
    if ((adr == DP_SELECT) && (dap_state.select == val)) {
        return 1;
    }

    req = SWD_REG_DP | SWD_REG_W | SWD_REG_ADR(adr);
    MUL_int2array(data, val, 4);
    
    pAck = MUL_swd_transfer_retry(req, (uint32_t *)data);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    }       
    else
    {
        ack = 0;
    }
    if ((ack == DAP_TRANSFER_OK) && (adr == DP_SELECT)) {
        dap_state.select = val;
    }
    return (ack == 0x01);
}

// Read access port register.
#if 0      /* 该函数未使用 */
uint8_t MUL_swd_read_ap(uint32_t adr, uint32_t *val)
{
    uint8_t tmp_in, ack;
    uint8_t tmp_out[4];
    uint32_t tmp;
    uint32_t apsel = MUL_swd_get_apsel(adr);
    uint32_t bank_sel = adr & APBANKSEL;

    if (!MUL_swd_write_dp(DP_SELECT, apsel | bank_sel)) {
        return 0;
    }

    tmp_in = SWD_REG_AP | SWD_REG_R | SWD_REG_ADR(adr);
    // first dummy read
    MUL_swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
    ack = MUL_swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
    *val = 0;
    tmp = tmp_out[3];
    *val |= (tmp << 24);
    tmp = tmp_out[2];
    *val |= (tmp << 16);
    tmp = tmp_out[1];
    *val |= (tmp << 8);
    tmp = tmp_out[0];
    *val |= (tmp << 0);
    return (ack == 0x01);
}
#endif

// Write access port register
uint8_t MUL_swd_write_ap(uint32_t adr, uint32_t val)
{
    uint8_t data[4];
    uint8_t req, ack;
    uint8_t *pAck;
    uint32_t apsel = MUL_swd_get_apsel(adr);
    uint32_t bank_sel = adr & APBANKSEL;

    if (!MUL_swd_write_dp(DP_SELECT, apsel | bank_sel)) {
        return 0;
    }

    switch (adr) {
        case AP_CSW:
            if (dap_state.csw == val) {
                return 1;
            }

            dap_state.csw = val;
            break;

        default:
            break;
    }

    req = SWD_REG_AP | SWD_REG_W | SWD_REG_ADR(adr);
    int2array(data, val, 4);

    pAck = MUL_swd_transfer_retry(req, (uint32_t *)data);
    if (MUL_CheckAckTransferOk(pAck)) 
    {
        ack = 1;
    } 
    else
    {
        ack = 0;
    }    
    if (ack != 0x01) {
        return 0;
    }

    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    pAck =  MUL_swd_transfer_retry(req, NULL);
    if (MUL_CheckAckTransferOk(pAck)) 
    {
        ack = 1;
    } 
    else
    {
        ack = 0;
    }
    return (ack == 0x01);
}


// Write 32-bit word aligned values to target memory using address auto-increment.
// size is in bytes.
static uint8_t MUL_swd_write_block(uint32_t address, uint8_t *data, uint32_t size)
{
    uint8_t tmp_in[4], req;
    uint32_t size_in_words;
    uint32_t i, ack;
    uint8_t *pAck;

    if (size == 0) {
        return 0;
    }

    size_in_words = size / 4;

    // CSW register
    if (!MUL_swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    // TAR write
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);
    int2array(tmp_in, address, 4);
    pAck = MUL_swd_transfer_retry(req, (uint32_t *)tmp_in);
    if (MUL_CheckAckTransferOk(pAck)) 
    {
        ack = 1;
    } 
    else
    {
        ack = 0;
    }
    if (ack != 0x01) {
        return 0;
    }

    // DRW write
    req = SWD_REG_AP | SWD_REG_W | (3 << 2);

    for (i = 0; i < size_in_words; i++) {
        pAck = MUL_swd_transfer_retry(req, (uint32_t *)data);
        if (MUL_CheckAckTransferOk(pAck)) 
        {
            ack = 1;
        } 
        else
        {
            ack = 0;
        }        
        if (ack != 0x01) {
            return 0;
        }

        data += 4;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    pAck = MUL_swd_transfer_retry(req, NULL);
    if (MUL_CheckAckTransferOk(pAck)) 
    {
        ack = 1;
    } 
    else
    {
        ack = 0;
    }      
    return (ack == 0x01);
}

// Read 32-bit word aligned values from target memory using address auto-increment.
// size is in bytes.
static uint8_t MUL_swd_read_block(uint32_t address, uint8_t *p1,uint8_t *p2,uint8_t *p3,uint8_t *p4, uint32_t size)
{
    uint8_t tmp_in[4], req, ack;
    uint8_t *pAck;
    uint32_t size_in_words;
    uint32_t i;
    uint32_t buf32[4];

    if (size == 0) {
        return 0;
    }

    size_in_words = size / 4;

    if (!MUL_swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    // TAR write
    req = SWD_REG_AP | SWD_REG_W | AP_TAR;
    int2array(tmp_in, address, 4);

    pAck = MUL_swd_transfer_retry(req, (uint32_t *)tmp_in);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    } 
    else
    {
        ack = 0;
    }  
    if (ack != DAP_TRANSFER_OK) {
        return 0;
    }

    // read data
    req = SWD_REG_AP | SWD_REG_R | AP_DRW;

    // initiate first read, data comes back in next read
    pAck = MUL_swd_transfer_retry(req, NULL);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    } 
    else
    {
        ack = 0;
    }     
    if (ack != 0x01) {
        return 0;
    }

    for (i = 0; i < (size_in_words - 1); i++) 
    {
        pAck = MUL_swd_transfer_retry(req, (uint32_t *)buf32);
        if (MUL_CheckAckTransferOk(pAck))
        {
            ack = DAP_TRANSFER_OK;
        } 
        else
        {
            ack = 0;
        }         
        if (ack != DAP_TRANSFER_OK) {
            return 0;
        }

        *p1++ = buf32[0];            
        *p1++ = buf32[0]>>8;
        *p1++ = buf32[0]>>16;
        *p1++ = buf32[0]>>24;

        *p2++ = buf32[1];            
        *p2++ = buf32[1]>>8;
        *p2++ = buf32[1]>>16;
        *p2++ = buf32[1]>>24;

        *p3++ = buf32[2];            
        *p3++ = buf32[2]>>8;
        *p3++ = buf32[2]>>16;
        *p3++ = buf32[2]>>24;

        *p4++ = buf32[3];            
        *p4++ = buf32[3]>>8;
        *p4++ = buf32[3]>>16;
        *p4++ = buf32[3]>>24;        
    }

    // read last word
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    pAck = MUL_swd_transfer_retry(req, (uint32_t *)buf32);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;

        //        *p1++ = buf32[0];     错误用法，如果地址非4字节整数倍，则会产生内存访问异常
        //        *p2++ = buf32[1];
        //        *p3++ = buf32[2];
        //        *p4++ = buf32[3]; 
        *p1++ = buf32[0];            
        *p1++ = buf32[0]>>8;
        *p1++ = buf32[0]>>16;
        *p1++ = buf32[0]>>24;

        *p2++ = buf32[1];            
        *p2++ = buf32[1]>>8;
        *p2++ = buf32[1]>>16;
        *p2++ = buf32[1]>>24;

        *p3++ = buf32[2];            
        *p3++ = buf32[2]>>8;
        *p3++ = buf32[2]>>16;
        *p3++ = buf32[2]>>24;

        *p4++ = buf32[3];            
        *p4++ = buf32[3]>>8;
        *p4++ = buf32[3]>>16;
        *p4++ = buf32[3]>>24;         
    } 
    else
    {
        ack = 0;
    }     
    return (ack == 0x01);
}

// Read target memory.
static uint8_t MUL_swd_read_data(uint32_t addr, uint32_t *val)
{
    uint8_t tmp_in[4];
//    uint8_t tmp_out[4];
    uint8_t req, ack;
    uint8_t *pAck;

    // put addr in TAR register
    int2array(tmp_in, addr, 4);
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);

    pAck = MUL_swd_transfer_retry(req, (uint32_t *)tmp_in);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    } 
    else
    {
        ack = 0;
    }     
    if (ack != 0x01) {
        return 0;
    }

    // read data
    req = SWD_REG_AP | SWD_REG_R | (3 << 2);

    pAck = MUL_swd_transfer_retry(req, (uint32_t *)val);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    } 
    else
    {
        ack = 0;
    }    
    if (ack != 0x01) {
        return 0;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    pAck = MUL_swd_transfer_retry(req, (uint32_t *)val);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    } 
    else
    {
        ack = 0;
    }    
//    *val = 0;
//    tmp = tmp_out[3];
//    *val |= (tmp << 24);
//    tmp = tmp_out[2];
//    *val |= (tmp << 16);
//    tmp = tmp_out[1];
//    *val |= (tmp << 8);
//    tmp = tmp_out[0];
//    *val |= (tmp << 0);
    return (ack == 0x01);
}

// Write target memory.
static uint8_t MUL_swd_write_data(uint32_t address, uint32_t data)
{
    uint8_t tmp_in[4];
    uint8_t req, ack;
    uint8_t *pAck;
    
    // put addr in TAR register
    int2array(tmp_in, address, 4);
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);

    pAck = MUL_swd_transfer_retry(req, (uint32_t *)tmp_in);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    } 
    else
    {
        ack = 0;
    }     
    if (ack != 0x01) {
        return 0;
    }

    // write data
    int2array(tmp_in, data, 4);
    req = SWD_REG_AP | SWD_REG_W | (3 << 2);

    pAck = MUL_swd_transfer_retry(req, (uint32_t *)tmp_in);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    } 
    else
    {
        ack = 0;
    }     
    if (ack != 0x01) {
        return 0;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    pAck = MUL_swd_transfer_retry(req, NULL);
    if (MUL_CheckAckTransferOk(pAck))
    {
        ack = DAP_TRANSFER_OK;
    } 
    else
    {
        ack = 0;
    }     
    return (ack == 0x01) ? 1 : 0;
}

// Read 32-bit word from target memory.
uint8_t MUL_swd_read_word(uint32_t addr, uint32_t *val)
{
    if (!MUL_swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    if (!MUL_swd_read_data(addr, val)) {
        return 0;
    }

    return 1;
}

// Write 32-bit word to target memory.
uint8_t MUL_swd_write_word(uint32_t addr, uint32_t val)
{
    if (!MUL_swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    if (!MUL_swd_write_data(addr, val)) {
        return 0;
    }

    return 1;
}

// Read 8-bit byte from target memory.
uint8_t MUL_swd_read_byte(uint32_t addr, uint8_t *val)
{
    uint32_t tmp[4];

    if (!MUL_swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE8)) {
        return 0;
    }

    if (!MUL_swd_read_data(addr, tmp)) {
        return 0;
    }

    //*val = (uint8_t)(tmp >> ((addr & 0x03) << 3));
    val[0] = (uint8_t)(tmp[0] >> ((addr & 0x03) << 3));
    val[1] = (uint8_t)(tmp[1] >> ((addr & 0x03) << 3));
    val[2] = (uint8_t)(tmp[2] >> ((addr & 0x03) << 3));
    val[3] = (uint8_t)(tmp[3] >> ((addr & 0x03) << 3));
    return 1;
}

// Write 8-bit byte to target memory.
uint8_t MUL_swd_write_byte(uint32_t addr, uint8_t val)
{
    uint32_t tmp;

    if (!MUL_swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE8)) {
        return 0;
    }

    tmp = val << ((addr & 0x03) << 3);

    if (!MUL_swd_write_data(addr, tmp)) {
        return 0;
    }

    return 1;
}

// Read unaligned data from target memory.
// size is in bytes.

/*
    读4个芯片的内存数据。
    ata存放规则: 第1个芯片数据 + 第2个芯片数据 + 第3个芯片数据  + 第4个芯片数据
*/
uint8_t MUL_swd_read_memory(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t n;
    uint8_t Buf8[4];
    uint8_t *p1,*p2,*p3,*p4;

    p1 = data;
    p2 = p1 + size;
    p3 = p2 + size;
    p4 = p3 + size;

    // Read bytes until word aligned
    while ((size > 0) && (address & 0x3)) {
        if (!MUL_swd_read_byte(address, Buf8)) {
            return 0;
        }

        address++;
        size--;
        
        *p1++ = Buf8[0];
        *p2++ = Buf8[1];
        *p3++ = Buf8[2];
        *p4++ = Buf8[3];
    }

    // Read word aligned blocks
    while (size > 3) {
        // Limit to auto increment page size
        n = TARGET_AUTO_INCREMENT_PAGE_SIZE - (address & (TARGET_AUTO_INCREMENT_PAGE_SIZE - 1));

        if (size < n) {
            n = size & 0xFFFFFFFC; // Only count complete words remaining
        }

        if (!MUL_swd_read_block(address, p1, p2, p3, p4, n)) {
            return 0;
        }

        address += n;
        p1 += n;
        p2 += n;
        p3 += n;
        p4 += n;
        size -= n;
    }

    // Read remaining bytes
    while (size > 0) {
        if (!MUL_swd_read_byte(address, Buf8)) {
            return 0;
        }

        address++;
        size--;
        *p1++ = Buf8[0];
        *p2++ = Buf8[1];
        *p3++ = Buf8[2];
        *p4++ = Buf8[3];        
    }

    return 1;
}

// Write unaligned data to target memory.
// size is in bytes.
uint8_t MUL_swd_write_memory(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t n = 0;
    
    // Write bytes until word aligned
    while ((size > 0) && (address & 0x3)) {
        if (!MUL_swd_write_byte(address, *data)) {
            goto err_quit;
        }

        address++;
        data++;
        size--;
    }

    // Write word aligned blocks
    while (size > 3) {
        // Limit to auto increment page size
        n = TARGET_AUTO_INCREMENT_PAGE_SIZE - (address & (TARGET_AUTO_INCREMENT_PAGE_SIZE - 1));

        if (size < n) {
            n = size & 0xFFFFFFFC; // Only count complete words remaining
        }

        if (!MUL_swd_write_block(address, data, n)) {
           goto err_quit;;
        }

        address += n;
        data += n;
        size -= n;
    }

    // Write remaining bytes
    while (size > 0) {
        if (!MUL_swd_write_byte(address, *data)) {
           goto err_quit;
        }

        address++;
        data++;
        size--;
    }
    return 1;
    
err_quit:    
    return 0;
}

// Execute system call.
static uint8_t MUL_swd_write_debug_state(DEBUG_STATE *state)
{
    uint32_t i;  

    if (!MUL_swd_write_dp(DP_SELECT, 0)) {
        return 0;
    }

    // R0, R1, R2, R3
    for (i = 0; i < 4; i++) {
        if (!MUL_swd_write_core_register(i, state->r[i])) {
            return 0;
        }
    }

    // R9
    if (!MUL_swd_write_core_register(9, state->r[9])) {
        return 0;
    }

    // R13, R14, R15
    for (i = 13; i < 16; i++) {
        if (!MUL_swd_write_core_register(i, state->r[i])) {
            return 0;
        }
    }

    // xPSR
    if (!MUL_swd_write_core_register(16, state->xpsr)) {
        return 0;
    }

    if (!MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_MASKINTS | C_HALT)) {
        return 0;
    }

    if (!MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_MASKINTS)) {
        return 0;
    }

    // check status    
//    if (!MUL_swd_read_dp(DP_CTRL_STAT, &status)) {
//        return 0;
//    }

//    if (status & (STICKYERR | WDATAERR)) {
//        return 0;
//    }

    {
        uint8_t *pAck;
        uint32_t status[4];
        uint8_t err = 0;

        pAck = MUL_swd_read_dp(DP_CTRL_STAT, status);
 
        if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
        {
            for (i = 0; i < 4; i++)
            {
                if (g_gMulSwd.Active[i] == 1)
                {
                    if (pAck[i] == 1)                    
                    {
                        if (status[i] & (STICKYERR | WDATAERR))
                        {
                            err = 1;
                        }
                    }
                    else
                    {
                        err = 1;
                    }
                }
            }   
        }
        else
        {        
            err = 1;
            for (i = 0; i < 4; i++)
            {
                if (g_gMulSwd.Active[i] == 1)
                {
                    if (pAck[i] == 1)                    
                    {
                        if (status[i] & (STICKYERR | WDATAERR))
                        {
                            err = 1;
                        }
                        else
                        {
                            err = 0;
                        }
                    }
                    else
                    {
                        //err = 1;
                    }
                }
            }            
        }
        
        if (err == 1)
        {
            return 0;
        }
    }

    return 1;
}

uint8_t MUL_swd_read_core_register(uint32_t n, uint32_t *val)
{
    int i = 0, timeout = 100;
    uint32_t readval[4];    

    if (!MUL_swd_write_word(DCRSR, n)) {
        return 0;
    }

    // wait for S_REGRDY
    for (i = 0; i < timeout; i++) {
        uint8_t continu_wait;
        
        if (!MUL_swd_read_word(DHCSR, readval)) {
            return 0;
        }

//        if (*val & S_REGRDY) {
//            break;
//        }
        if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
        {
            continu_wait = 0;
            if (g_gMulSwd.Active[0] == 1 && ((readval[0] & S_REGRDY) == 0))
            {
                continu_wait = 1;
            }
            if (g_gMulSwd.Active[1] == 1 && ((readval[1] & S_REGRDY) == 0))
            {
                continu_wait = 1;
            }
            if (g_gMulSwd.Active[2] == 1 && ((readval[2] & S_REGRDY) == 0))
            {
                continu_wait = 1;
            }
            if (g_gMulSwd.Active[3] == 1 && ((readval[3] & S_REGRDY) == 0))
            {
                continu_wait = 1;
            }   
            if (continu_wait == 0)
            {
                break;
            }
        }
        else
        {
            continu_wait = 0;
            if (g_gMulSwd.Active[0] == 1 && g_gMulSwd.Error[0] == 0 && ((readval[0] & S_REGRDY) == 0))
            {
                continu_wait = 1;
            }
            if (g_gMulSwd.Active[1] == 1 && g_gMulSwd.Error[1] == 0 && ((readval[1] & S_REGRDY) == 0))
            {
                continu_wait = 1;
            }
            if (g_gMulSwd.Active[2] == 1 && g_gMulSwd.Error[2] == 0 && ((readval[2] & S_REGRDY) == 0))
            {
                continu_wait = 1;
            }
            if (g_gMulSwd.Active[3] == 1 && g_gMulSwd.Error[3] == 0 && ((readval[3] & S_REGRDY) == 0))
            {
                continu_wait = 1;
            }   
            if (continu_wait == 0)
            {
                break;
            }           
        }
            
    }

    if (i == timeout) {
        return 0;    
    }

    if (!MUL_swd_read_word(DCRDR, val)) {
        return 0;
    }

    return 1;
}

uint8_t MUL_swd_write_core_register(uint32_t n, uint32_t val)
{
    int i = 0, timeout = 100;
    uint32_t readval[4];

    if (!MUL_swd_write_word(DCRDR, val)) {
        return 0;
    }

    if (!MUL_swd_write_word(DCRSR, n | REGWnR)) {
        return 0;
    }

    // wait for S_REGRDY
    for (i = 0; i < timeout; i++) {
       
        
        if (!MUL_swd_read_word(DHCSR, readval)) {
            return 0;
        }

//        if ((readval[0] & S_REGRDY) && (readval[1] & S_REGRDY) && (readval[2] & S_REGRDY) && (readval[3] & S_REGRDY)) {
//            return 1;
//        }
        if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
        {
            uint8_t err;
            
            err = 0;
            if (g_gMulSwd.Active[0] == 1 && ((readval[0] & S_REGRDY) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[1] == 1 && ((readval[1] & S_REGRDY) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[2] == 1 && ((readval[2] & S_REGRDY) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[3] == 1 && ((readval[3] & S_REGRDY) == 0))
            {
                err = 1;
            }   
            if (err == 0)
            {
                return 1;
            }
        }
        else     /* 有1个ok 则返回ok */
        {
            uint8_t ok;
            
            ok = 0;
            if (g_gMulSwd.Active[0] == 1 && ((readval[0] & S_REGRDY) != 0))
            {
                ok = 1;
            }
            if (g_gMulSwd.Active[1] == 1 && ((readval[1] & S_REGRDY) != 0))
            {
                ok = 1;
            }
            if (g_gMulSwd.Active[2] == 1 && ((readval[2] & S_REGRDY) != 0))
            {
                ok = 1;
            }
            if (g_gMulSwd.Active[3] == 1 && ((readval[3] & S_REGRDY) != 0))
            {
                ok = 1;
            }   
            if (ok == 1)
            {
                return 1;
            }
         }
    }

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: MUL_swd_wait_until_halted
*    功能说明: 执行FLM中的函数，等待完成. 增加了超时控制，全局变量 g_tProg.FLMFuncTimeout
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void PG_PrintPercent(float _Percent, uint32_t _Addr);
extern uint8_t ProgCancelKey(void);
extern void PG_PrintText(char *_str);
static uint8_t MUL_swd_wait_until_halted(void)
{
    uint32_t val[4];
    int32_t time1;
    int32_t addtime = 0;
    int32_t tt0 = 0;
    uint8_t ok[4] ={0};
    
    time1 = bsp_GetRunTime();
    
    if (g_tProg.FLMFuncDispProgress == 1)
    {
        addtime = 5000;     /* 给5秒的余量 */
    }        
    while (1)
    {
        /* 超时控制 */
        {
            int32_t tt;
            float percent;
            
            tt = bsp_CheckRunTime(time1);
            if (tt0 != tt)
            {
                tt0 = tt;
                if (tt > g_tProg.FLMFuncTimeout + addtime)
                {
                    uint8_t i;
                    
                    printf("error : swd_wait_until_halted() timeout\r\n");
                    for (i = 0; i < 4; i++)
                    {
                        if (g_gMulSwd.Active[i] == 1)
                        {                    
                            if (ok[i] == 0)
                            {
                                g_gMulSwd.Error[i] = 1;                                
                            }
                        }
                    }                    
                    break;      /* 超时退出 */
                }
                else
                {
                    if (g_tProg.FLMFuncDispProgress == 1)
                    {
                        /* 250ms打印1次 */
                        if ((tt % 250) == 0)
                        {
                            percent = ((float)tt / g_tProg.FLMFuncTimeout) * 100;                                
                            PG_PrintPercent(percent, g_tProg.FLMFuncDispAddr);
                        }
                        bsp_Idle();
                    }
                }
            }                   
        }
        
        if (!MUL_swd_read_word(DBG_HCSR, val)) 
        {
            break;
        }
        
        if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
        {
            uint8_t err = 0;
            uint8_t i;
            
            /* 只要有1个没有停机则继续等待 */
            for (i = 0; i < 4; i++)
            {
                if (g_gMulSwd.Active[i] == 1)
                {                    
                    if ((val[i] & S_HALT) == 0)
                    {
                        err = 1;
                        break;
                    }
                    else
                    {
                        ok[i] = 1;
                    }
                }
            }
            if (err == 0)
            {
                g_tProg.FLMFuncDispProgress = 0;
                return 1;   /* 成功 */
            }
        }
        else
        {
            uint8_t err = 0;
            uint8_t i;
            
            /* 只要有1个没有停机则继续等待 */
            for (i = 0; i < 4; i++)
            {
                if (g_gMulSwd.Active[i] == 1)
                {                    
                    if (g_gMulSwd.Error[i] == 0)
                    {
                        if ((val[i] & S_HALT) == 0)
                        {
                            err = 1;
                            //break;
                        }
                        else
                        {
                            ok[i] = 1;
                        }
                    }
                }
            }
            if (err == 0)
            {
                g_tProg.FLMFuncDispProgress = 0;
                return 1;   /* 成功 */
            }
        }       

        if (ProgCancelKey())
        {
            PG_PrintText("用户终止运行");    
            break;         
        }        
    }
    g_tProg.FLMFuncDispProgress = 0;
    return 0;
}

uint8_t MUL_swd_flash_syscall_exec(const program_syscall_t *sysCallParam, uint32_t entry, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    DEBUG_STATE state = {{0}, 0};
    uint32_t R0[4];
    
    // Call flash algorithm function on target and wait for result.
    state.r[0]     = arg1;                   // R0: Argument 1
    state.r[1]     = arg2;                   // R1: Argument 2
    state.r[2]     = arg3;                   // R2: Argument 3
    state.r[3]     = arg4;                   // R3: Argument 4
    state.r[9]     = sysCallParam->static_base;    // SB: Static Base
    state.r[13]    = sysCallParam->stack_pointer;  // SP: Stack Pointer
    state.r[14]    = sysCallParam->breakpoint;     // LR: Exit Point
    state.r[15]    = entry;                        // PC: Entry Point
    state.xpsr     = 0x01000000;          // xPSR: T = 1, ISR = 0

    if (!MUL_swd_write_debug_state(&state)) {
        return 0;
    }

    if (!MUL_swd_wait_until_halted()) {
        return 0;
    }

    //if (!MUL_swd_read_core_register(0, &state.r[0])) {
    if (!MUL_swd_read_core_register(0, R0)) {
        return 0;
    }
    
    //remove the C_MASKINTS
    if (!MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
        return 0;
    }

    // Flash functions return 0 if successful.
    //if (state.r[0] != 0) {
    if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
    {
        uint8_t err = 0;
        uint8_t i;
        
        for (i = 0; i < 4; i++)
        {
            if (g_gMulSwd.Active[i] == 1 && R0[i] != 0)
            {
                err = 1;
            }
        }
        
        if (err == 1)
        {
            return 0;
        }
    }
    else      /* 只要有1路ok ，则返回ok */
    {
        uint8_t ok = 0;
        uint8_t i;
        
        for (i = 0; i < 4; i++)
        {
            if (g_gMulSwd.Active[i] == 1 && R0[i] == 0)
            {
                ok = 1;
            }
        }
        
        if (ok == 0)
        {
            return 0;
        }
    }
    
    return 1;   /* 成功 */
}

uint32_t MUL_swd_flash_syscall_exec_ex(const program_syscall_t *sysCallParam, uint32_t entry, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    DEBUG_STATE state = {{0}, 0};
    static uint32_t R0[4];
    
    // Call flash algorithm function on target and wait for result.
    state.r[0]     = arg1;                   // R0: Argument 1
    state.r[1]     = arg2;                   // R1: Argument 2
    state.r[2]     = arg3;                   // R2: Argument 3
    state.r[3]     = arg4;                   // R3: Argument 4
    state.r[9]     = sysCallParam->static_base;    // SB: Static Base
    state.r[13]    = sysCallParam->stack_pointer;  // SP: Stack Pointer
    state.r[14]    = sysCallParam->breakpoint;     // LR: Exit Point
    state.r[15]    = entry;                        // PC: Entry Point
    state.xpsr     = 0x01000000;          // xPSR: T = 1, ISR = 0

    if (!MUL_swd_write_debug_state(&state)) {
        return 0;
    }

    if (!MUL_swd_wait_until_halted()) {
        return 0;
    }

    if (!MUL_swd_read_core_register(0, R0)) {
        return 0;
    }
    
    //remove the C_MASKINTS
    if (!MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
        return 0;
    }

//    // Flash functions return 0 if successful.
//    if (state.r[0] != 0) {
//        return 0;
//    }

//    return 1;
    return (uint32_t)R0;
}

// SWD Reset
static uint8_t MUL_swd_reset(void)
{
    uint8_t tmp_in[8];
    uint8_t i = 0;

    for (i = 0; i < 8; i++) {
        tmp_in[i] = 0xff;
    }

    MUL_SWJ_Sequence(51, tmp_in);
    return 1;
}

// SWD Switch
static uint8_t MUL_swd_switch(uint16_t val)
{
    uint8_t tmp_in[2];
    tmp_in[0] = val & 0xff;
    tmp_in[1] = (val >> 8) & 0xff;
    MUL_SWJ_Sequence(16, tmp_in);
    return 1;
}

// SWD Read ID
//static uint8_t MUL_swd_read_idcode(uint32_t *id)
uint8_t MUL_swd_read_idcode(uint32_t *id)
{
    uint8_t tmp_in[1];
    uint32_t tmp_out[4];
    uint8_t ack;
    uint8_t *pAck;
    
    tmp_in[0] = 0x00;
    
    MUL_SWJ_Sequence(8, tmp_in);

//    if (MUL_swd_read_dp(0, (uint32_t *)tmp_out) != 0x01) {
//        return 0;
//    }

//    *id = (tmp_out[3] << 24) | (tmp_out[2] << 16) | (tmp_out[1] << 8) | tmp_out[0];
    pAck = MUL_swd_read_dp(0, tmp_out);
    if (pAck[0] == DAP_TRANSFER_OK || pAck[1] == DAP_TRANSFER_OK ||
        pAck[2] == DAP_TRANSFER_OK || pAck[3] == DAP_TRANSFER_OK)
    {
       
        if (pAck[0] == DAP_TRANSFER_OK) id[0] = g_gMulSwd.CoreID[0] = tmp_out[0];
        if (pAck[1] == DAP_TRANSFER_OK) id[1] = g_gMulSwd.CoreID[1] = tmp_out[1];
        if (pAck[2] == DAP_TRANSFER_OK) id[2] = g_gMulSwd.CoreID[2] = tmp_out[2];
        if (pAck[3] == DAP_TRANSFER_OK) id[3] = g_gMulSwd.CoreID[3] = tmp_out[3];
        
        ack = DAP_TRANSFER_OK;
    }       
    else
    {
        ack = 0;
    }
    
    if (ack == DAP_TRANSFER_OK)
        return 1;
    else
        return 0;
}


static uint8_t MUL_JTAG2SWD()
{
    uint32_t tmp[4] = {0};

    if (!MUL_swd_reset()) {
        return 0;
    }

    if (!MUL_swd_switch(0xE79E)) {
        return 0;
    }

    if (!MUL_swd_reset()) {
        return 0;
    }

    /* 旧协议，J-LINK如此发送 */
    if (!MUL_swd_switch(0xEDB6)) {
        return 0;
    }
    
    if (!MUL_swd_reset()) {
        return 0;
    }
    
    if (!MUL_swd_read_idcode(tmp)) {
        return 0;
    }

    return 1;
}

static uint8_t MUL_JTAG2SWD_2(uint32_t *tmp)
{
    tmp[0] = 0;
    tmp[1] = 0;
    tmp[2] = 0;
    tmp[3] = 0;
    
    if (!MUL_swd_reset()) {
        return 0;
    }

    if (!MUL_swd_switch(0xE79E)) {
        return 0;
    }

    if (!MUL_swd_reset()) {
        return 0;
    }

    /* 旧协议，J-LINK如此发送 */
    if (!MUL_swd_switch(0xEDB6)) {
        return 0;
    }
    
    if (!MUL_swd_reset()) {
        return 0;
    }
    
    if (!MUL_swd_read_idcode(tmp)) {
        return 0;
    }

    return 1;
}

// 检测SWD，用于判断芯片是否移除. 只检测一次 
uint8_t MUL_swd_detect_core(uint32_t *_id)
{
    MUL_swd_init();
        
    if (!MUL_JTAG2SWD_2(_id)) 
    {
        return 0;
    }
    return 1;
}

uint8_t MUL_swd_init_debug(void)
{
    uint32_t tmp[4] = {0,0,0,0};
    int i = 0;
    int timeout = 1000;
    uint8_t *pAck;
    
    // init dap state with fake values
    dap_state.select = 0xffffffff;
    dap_state.csw = 0xffffffff;
    
    int8_t retries = 4;
    int8_t do_abort = 0;
    
    g_gMulSwd.Ignore[0] = 0;
    g_gMulSwd.Ignore[1] = 0;
    g_gMulSwd.Ignore[2] = 0;
    g_gMulSwd.Ignore[3] = 0;
    
    
    g_gMulSwd.Error[0] = 0;
    g_gMulSwd.Error[1] = 0;
    g_gMulSwd.Error[2] = 0;
    g_gMulSwd.Error[3] = 0;
    
    g_gMulSwd.CoreID[0] = 0;
    g_gMulSwd.CoreID[1] = 0;
    g_gMulSwd.CoreID[2] = 0;
    g_gMulSwd.CoreID[3] = 0;
    do {
        if (do_abort != 0) 
        {
            //do an abort on stale target, then reset the device
            MUL_swd_write_dp(DP_ABORT, DAPABORT);
            
            MUL_swd_set_target_reset(1);
            osDelay(20);
            MUL_swd_set_target_reset(0);
            osDelay(g_tProg.SwdResetDelay);
            do_abort = 0;
        }
        
        MUL_swd_init();

        if (!MUL_JTAG2SWD()) 
        {
            osDelay(2);   //调试RT1052 需要再次访问
            if (!MUL_JTAG2SWD())
            {
                do_abort = 1;
                continue;
            }
        }

        if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
        {
            uint8_t err = 0;
            
            for (i = 0; i < 4; i++)
            {
                if (g_gMulSwd.Active[i] == 1)
                {
                    if (g_gMulSwd.CoreID[i] == 0)
                    {
                        err = 1;
                    }
                }
            }
            
            if (err == 1)
            {
                do_abort = 2;
                continue;      
            }
        }
        else   /* 有1路OK，则设置OK */
        {
            uint8_t ok = 0;
            
            for (i = 0; i < 4; i++)
            {
                if (g_gMulSwd.Active[i] == 1)
                {
                    if (g_gMulSwd.CoreID[i] > 0)
                    {
                        ok = 1;
                    }
                }
            }
            
            if (ok == 0)
            {
                do_abort = 2;
                continue;      
            }
        }        
        
        if (!MUL_swd_clear_errors()) {
            do_abort = 2;
            continue;
        }

        if (!MUL_swd_write_dp(DP_SELECT, 0)) {
            do_abort = 3;
            continue;
            
        }
        
        // Power up
        if (!MUL_swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
            do_abort = 4;
            continue;
        }
       
        {
            uint8_t k;
            uint8_t done = 0;
            uint8_t err = 0;
            
            for (i = 0; i < timeout; i++) 
            {
                pAck = MUL_swd_read_dp(DP_CTRL_STAT, tmp);            
                for (k = 0; k < 4; k++)
                {
                    if (g_gMulSwd.Active[i] == 0)       /* 通道未激活 */
                    {
                        done |= (1 << k);
                        err |= (1 << k);
                    }
                    else
                    {
                        if (pAck[k] == 1)
                        {
                            err &= ~(1 << k);
                            if ((tmp[k] & (CDBGPWRUPACK | CSYSPWRUPACK)) == (CDBGPWRUPACK | CSYSPWRUPACK))
                            {
                                done |= (1 << k);
                            }
                            else
                            {
                                ;   /* 收到应答，单不满足条件，继续重发 */;
                            }
                        }
                        else
                        {
                            done |= (1 << k);
                            err |= (1 << k);
                        }
                    }
                }
                
                if (done == 0x0F) 
                {
                    break;
                } 
            }
        }
        
        if (!MUL_swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ | TRNNORMAL | MASKLANE)) {
            do_abort = 7;
            continue;
        }

#if 0    // armfly debug                
        // call a target dependant function:
        // some target can enter in a lock state
        // this function can unlock these targets
        if (g_target_family && g_target_family->target_unlock_sequence) {
            g_target_family->target_unlock_sequence();
        }
#endif
        
        if (!MUL_swd_write_dp(DP_SELECT, 0)) {
            do_abort = 8;
            continue;
        }

        return 1;
    
    } while (--retries > 0);
    
    return 0;
}

uint8_t MUL_swd_set_target_state_hw(TARGET_RESET_STATE state)
{
    uint32_t val[4];
    int8_t ap_retries = 2;
    /* Calling MUL_swd_init prior to entering RUN state causes operations to fail. */
    if (state != RUN) {
        MUL_swd_init();
    }

    switch (state) {
        case RESET_HOLD:
            MUL_swd_set_target_reset(1);
            break;

        case RESET_RUN:
            MUL_swd_set_target_reset(1);
            osDelay(2);
            MUL_swd_set_target_reset(0);
            osDelay(2);
            MUL_swd_off();
            break;

        case RESET_PROGRAM:
            {
                int k;
                int err = 0;
                
                for (k = 0; k < 10; k++)
                {
                    err = 0;
                    if (!MUL_swd_init_debug()) {
                        err = 1;
                        continue;
                    }
                    
                    if (reset_connect == CONNECT_UNDER_RESET) {
                        // Assert reset
                        MUL_swd_set_target_reset(1); 
                        osDelay(20);
                    }

                    // Enable debug
                    while(MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN) == 0) {
                        if( --ap_retries <=0 )
                            return 0;
                        // Target is in invalid state?
                        MUL_swd_set_target_reset(1);
                        osDelay(20);
                        MUL_swd_set_target_reset(0);
                        osDelay(20);
                    }

                    // Enable halt on reset
                    if (!MUL_swd_write_word(DBG_EMCR, VC_CORERESET)) {
                        err = 2;
                        continue;
                    }
                    
                    if (reset_connect == CONNECT_NORMAL) {
                        // Assert reset
                        MUL_swd_set_target_reset(1); 
                        osDelay(20);
                    }
                    
                    // Deassert reset
                    MUL_swd_set_target_reset(0);
                    osDelay(20);
                    
                    /* 2020-01-18 armfly 增加退出机制 */
                    {
                        uint32_t i;

                        for (i = 0; i < 100000; i++)
                        {
                            uint8_t errflag;
                            
                            if (!MUL_swd_read_word(DBG_HCSR, val)) {
                                err = 3;
                                break;
                            }                                      
                                
                            errflag = 0;
                            if (g_gMulSwd.Active[0] == 1 && ((val[0] & S_HALT) == 0))
                            {
                                errflag = 1;
                            }
                            if (g_gMulSwd.Active[1] == 1 && ((val[1] & S_HALT) == 0))
                            {
                                errflag = 1;
                            }
                            if (g_gMulSwd.Active[2] == 1 && ((val[2] & S_HALT) == 0))
                            {
                                errflag = 1;
                            }
                            if (g_gMulSwd.Active[3] == 1 && ((val[3] & S_HALT) == 0))
                            {
                                errflag = 1;
                            }   
                            if (errflag == 0)
                            {
                                break;
                            }                     
                        }    

                        if (err > 0)
                        {
                            continue;   
                        }
                    }

                    // Disable halt on reset
                    if (!MUL_swd_write_word(DBG_EMCR, 0)) {
                        err = 4;
                        continue;
                    }  
                    break;
                }
                
                if (err > 0)
                {
                    return 0;
                }
            }
            break;

        case NO_DEBUG:
            if (!MUL_swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }

            break;

        case DEBUG:
            if (!MUL_JTAG2SWD()) {
                return 0;
            }

            if (!MUL_swd_clear_errors()) {
                return 0;
            }

            // Ensure CTRL/STAT register selected in DPBANKSEL
            if (!MUL_swd_write_dp(DP_SELECT, 0)) {
                return 0;
            }

            // Power up
            if (!MUL_swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
                return 0;
            }

            // Enable debug
            if (!MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN)) {
                return 0;
            }

            break;

        case HALT:
            if (!MUL_swd_init_debug()) {
                return 0;
            }

            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            if (!MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
                return 0;
            }

            // Wait until core is halted
            {
                uint32_t i;
                
                for (i = 0; i < 100000; i++)
                {
                    uint8_t errflag;
                    
                    if (!MUL_swd_read_word(DBG_HCSR, val)) {
                        break;
                    }                                      
                        
                    errflag = 0;
                    if (g_gMulSwd.Active[0] == 1 && ((val[0] & S_HALT) == 0))
                    {
                        errflag = 1;
                    }
                    if (g_gMulSwd.Active[1] == 1 && ((val[1] & S_HALT) == 0))
                    {
                        errflag = 1;
                    }
                    if (g_gMulSwd.Active[2] == 1 && ((val[2] & S_HALT) == 0))
                    {
                        errflag = 1;
                    }
                    if (g_gMulSwd.Active[3] == 1 && ((val[3] & S_HALT) == 0))
                    {
                        errflag = 1;
                    }   
                    if (errflag == 0)
                    {
                        break;
                    }  
                }
            }
            break;

        case RUN:
            if (!MUL_swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }
            MUL_swd_off();
            break;

        case POST_FLASH_RESET:
            // This state should be handled in target_reset.c, nothing needs to be done here.
            break;

        default:
            return 0;
    }

    return 1;
}

uint8_t MUL_swd_set_target_state_sw(TARGET_RESET_STATE state)
{
    uint32_t val[4];
    int8_t ap_retries = 2;
    /* Calling MUL_swd_init prior to enterring RUN state causes operations to fail. */
    if (state != RUN) {
        MUL_swd_init();
    }

    switch (state) {
        case RESET_HOLD:
            MUL_swd_set_target_reset(1);
            break;

        case RESET_RUN:
            MUL_swd_set_target_reset(1);
            osDelay(2);
            MUL_swd_set_target_reset(0);
            osDelay(2);
            MUL_swd_off();
            break;

        case RESET_PROGRAM:
            if (!MUL_swd_init_debug()) {
                return 0;
            }

            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            while (MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT) == 0) {
                if ( --ap_retries <=0 ) {
                    return 0;
                }
                // Target is in invalid state?
                MUL_swd_set_target_reset(1);
                osDelay(2);
                MUL_swd_set_target_reset(0);
                osDelay(2);
            }

            // Wait until core is halted
            while(1)
            {
                uint8_t err;
                
                if (!MUL_swd_read_word(DBG_HCSR, val)) 
                {
                    return 0;
                }
                
                err = 0;
                if (g_gMulSwd.Active[0] == 1 && ((val[0] & S_HALT) == 0))
                {
                    err = 1;
                }
                if (g_gMulSwd.Active[1] == 1 && ((val[1] & S_HALT) == 0))
                {
                    err = 1;
                }
                if (g_gMulSwd.Active[2] == 1 && ((val[2] & S_HALT) == 0))
                {
                    err = 1;
                }
                if (g_gMulSwd.Active[3] == 1 && ((val[3] & S_HALT) == 0))
                {
                    err = 1;
                }   
                if (err == 0)
                {
                    break;
                }
            }

            // Enable halt on reset
            if (!MUL_swd_write_word(DBG_EMCR, VC_CORERESET)) {
                return 0;
            }

            // Perform a soft reset
            if (!MUL_swd_read_word(NVIC_AIRCR, val)) {
                return 0;
            }

            if (!MUL_swd_write_word(NVIC_AIRCR, VECTKEY | (val[0] & SCB_AIRCR_PRIGROUP_Msk) | soft_reset)) {
                return 0;
            }

            osDelay(2);

            while(1)
            {
                uint8_t err;
                
                if (!MUL_swd_read_word(DBG_HCSR, val)) 
                {
                    return 0;
                }
                
                err = 0;
                if (g_gMulSwd.Active[0] == 1 && ((val[0] & S_HALT) == 0))
                {
                    err = 1;
                }
                if (g_gMulSwd.Active[1] == 1 && ((val[1] & S_HALT) == 0))
                {
                    err = 1;
                }
                if (g_gMulSwd.Active[2] == 1 && ((val[2] & S_HALT) == 0))
                {
                    err = 1;
                }
                if (g_gMulSwd.Active[3] == 1 && ((val[3] & S_HALT) == 0))
                {
                    err = 1;
                }   
                if (err == 0)
                {
                    break;
                }
            }
            // Disable halt on reset
            if (!MUL_swd_write_word(DBG_EMCR, 0)) {
                return 0;
            }

            break;

        case NO_DEBUG:
            if (!MUL_swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }

            break;

        case DEBUG:
            if (!MUL_JTAG2SWD()) {
                return 0;
            }

            if (!MUL_swd_clear_errors()) {
                return 0;
            }

            // Ensure CTRL/STAT register selected in DPBANKSEL
            if (!MUL_swd_write_dp(DP_SELECT, 0)) {
                return 0;
            }

            // Power up
            if (!MUL_swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
                return 0;
            }

            // Enable debug
            if (!MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN)) {
                return 0;
            }

            break;

        case HALT:
            if (!MUL_swd_init_debug()) {
                return 0;
            }

            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            if (!MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
                return 0;
            }

            // Wait until core is halted
            do {
                if (!MUL_swd_read_word(DBG_HCSR, val)) {
                    return 0;
                }
            } while ((val[0] & S_HALT) == 0 || (val[2] & S_HALT) == 0 || (val[2] & S_HALT) == 0 || (val[3] & S_HALT) == 0);
            break;

        case RUN:
            if (!MUL_swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }
            MUL_swd_off();
            break;

        case POST_FLASH_RESET:
            // This state should be handled in target_reset.c, nothing needs to be done here.
            break;

        default:
            return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: swd_enter_debug_program
*    功能说明: 让芯片进入debug状态。 整合了 swd_init_debug()、 swd_set_target_state_sw、
*           swd_set_target_state_hw()几个函数，增加了各种MCU的异常处理
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern uint8_t swd_freeze_dog(void);
uint8_t MUL_swd_enter_debug_program(void)
{
    uint32_t val[4];
    uint32_t i;
    uint8_t ResetMode;
    
    /* --0:自动模式,  1:软件模式  2:硬件UnderReset */   
    ResetMode = g_tProg.ResetMode;
    
    /* 自动模式暂未实现 */
    if (ResetMode == 0)
    {
        ResetMode = 1;
    }   
    
    /* 软件复位 */
    if (ResetMode == 1)
    {
        if (!MUL_swd_init_debug()) {
            return 0;
        }

        if (swd_freeze_dog() == 0)      /* 如果冻结看门狗时钟失败（STM32H7）*/
        {
            if (swd_freeze_dog() == 0) 
            {
                /* 失败 */;
            }   
        }
        
        // Enable debug and halt the core (DHCSR <- 0xA05F0003)
        for (i = 0; i < 5; i++)
        {
            if (MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT) != 0)
            {
                break;
            }
            
            // Target is in invalid state?
            MUL_swd_set_target_reset(1);
            osDelay(20);
            MUL_swd_set_target_reset(0);
            osDelay(i * 5);
        }

        // Wait until core is halted
        for (i = 0; i < 500; i++)
        {
            uint8_t err;
            
            if (!MUL_swd_read_word(DBG_HCSR, val)) 
            {
                return 0;
            }
            
            err = 0;
            if (g_gMulSwd.Active[0] == 1 && ((val[0] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[1] == 1 && ((val[1] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[2] == 1 && ((val[2] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[3] == 1 && ((val[3] & S_HALT) == 0))
            {
                err = 1;
            }   
            if (err == 0)
            {
                break;
            }
            
            bsp_DelayUS(100);
        }

        // Enable halt on reset
        if (!MUL_swd_write_word(DBG_EMCR, VC_CORERESET)) {
            return 0;
        }

        // Perform a soft reset
        if (!MUL_swd_read_word(NVIC_AIRCR, val)) {
            return 0;
        }

        if (!MUL_swd_write_word(NVIC_AIRCR, VECTKEY | (val[0] & SCB_AIRCR_PRIGROUP_Msk) | soft_reset)) {
            return 0;
        }

        osDelay(2);

        for (i = 0; i < 500; i++)
        {
            uint8_t err;
            
            if (!MUL_swd_read_word(DBG_HCSR, val)) 
            {
                return 0;
            }
            
            err = 0;
            if (g_gMulSwd.Active[0] == 1 && ((val[0] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[1] == 1 && ((val[1] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[2] == 1 && ((val[2] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[3] == 1 && ((val[3] & S_HALT) == 0))
            {
                err = 1;
            }   
            if (err == 0)
            {
                break;
            }
            
            bsp_DelayUS(100);
        }
        // Disable halt on reset
        if (!MUL_swd_write_word(DBG_EMCR, 0)) {
            return 0;
        }
        
        /* 解锁后重读 NVIC_CPUID */
        {
            uint32_t cpuid[4];
            
            /* NVIC_CPUID = 0xE000ED00 */
            if (!MUL_swd_read_word(NVIC_CPUID, cpuid))
            {
                printf(".MUL_swd_read_word(NVIC_CPUID, cpuid) error\r\n");
                return 0;             
            }
            
            if (g_gMulSwd.MultiMode >= 1)
            {
                printf(".NVIC_CPUID1 = %08X, %s\r\n", cpuid[0], swd_arm_core_info(cpuid[0]));
            }
            if (g_gMulSwd.MultiMode >= 2)
            {
                printf(".NVIC_CPUID2 = %08X, %s\r\n", cpuid[1], swd_arm_core_info(cpuid[1]));
            }
            if (g_gMulSwd.MultiMode >= 3)
            {
                printf(".NVIC_CPUID3 = %08X, %s\r\n", cpuid[2], swd_arm_core_info(cpuid[2]));
            }
            if (g_gMulSwd.MultiMode >= 4)
            {                
                printf(".NVIC_CPUID4 = %08X, %s\r\n", cpuid[3], swd_arm_core_info(cpuid[3]));
            }
            {
                uint8_t err = 0;
                
                for (i = 0; i < 4; i++)
                {
                    if (g_gMulSwd.Active[i] == 1)
                    {
                        if ((cpuid[i] & 0xFF000000) != 0x41000000)
                        {
                            err = 1;
                        }
                    }
                }
                
                if (err == 1)
                {
                    ;
                }
            }
        }   
        
        return 1;
    }    
        
    /* 硬件复位 */
    if (ResetMode == 2)
    {
        /* 进入编程状态，先复位一次，应对已看门狗低功耗程序的片子 */
        MUL_swd_set_target_reset(1);
        osDelay(10);
        MUL_swd_set_target_reset(0);
        
        if (MUL_swd_init_debug() == 0)
        {
            printf("error 1: MUL_swd_init_debug()\r\n");        
            return 0;
        }
        
        osDelay(5);
        
        if (swd_freeze_dog() == 0)      /* 如果冻结看门狗时钟失败（STM32H7）*/
        {
            if (MUL_swd_get_target_reset() == 0)
            {
                MUL_swd_set_target_reset(1);    /* 硬件复位 */ 
                osDelay(g_tProg.SwdResetDelay);
            }
            
            if (MUL_swd_init_debug() == 0)
            {
                printf("error 2: MUL_swd_init_debug()\r\n");
                return 0;
            }
            
            if (swd_freeze_dog() == 0) 
            {
                /* 失败 */;
            }   
        }

        // Enable debug and halt the core (DHCSR <- 0xA05F0003)
        for (i = 0; i < 10; i++)
        {
            if (MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT) != 0)
            {
                break;
            }
            // Target is in invalid state?
            MUL_swd_set_target_reset(1);
            osDelay(20);
            MUL_swd_set_target_reset(0);
            osDelay(i * 5);                
        }
        if (i == 10)
        {
            printf("error 3: MUL_swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)\r\n");
            return 0;
        }    

        // Enable halt on reset
        if (!MUL_swd_write_word(DBG_EMCR, VC_CORERESET)) {
            printf("error 4: MUL_swd_write_word(DBG_EMCR, VC_CORERESET)\r\n");
            return 0;   /* 超时 */
        }
        
        MUL_swd_read_word(DBG_HCSR, val);
        if ((val[0] & S_HALT) == 0)
        {
            if (MUL_swd_get_target_reset() == 0)
            {
                MUL_swd_set_target_reset(1);    /* 硬件复位 */ 
                osDelay(20);
            }
            
            if (MUL_swd_get_target_reset() == 1)
            {
                MUL_swd_set_target_reset(0);    /* 退出硬件复位 */ 
                osDelay(g_tProg.SwdResetDelay);
            }
        }

        // Wait until core is halted
        for (i = 0; i < 1000; i++)
        {
            uint8_t err;
            
            if (!MUL_swd_read_word(DBG_HCSR, val)) 
            {
                printf("error 5: MUL_swd_read_word(DBG_HCSR, val)\r\n");
                return 0;
            }
            
            err = 0;
            if (g_gMulSwd.Active[0] == 1 && ((val[0] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[1] == 1 && ((val[1] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[2] == 1 && ((val[2] & S_HALT) == 0))
            {
                err = 1;
            }
            if (g_gMulSwd.Active[3] == 1 && ((val[3] & S_HALT) == 0))
            {
                err = 1;
            }   
            
            if (err == 0)
            {
                break;
            }
            
            bsp_DelayUS(1000);
        }
        if (i == 1000)
        {
            printf("error 6: MUL_swd_read_word(DBG_HCSR, val)\r\n");
            return 0;
        }  

        // Disable halt on reset
        if (!MUL_swd_write_word(DBG_EMCR, 0)) 
        {
            printf("error 7: MUL_swd_write_word(DBG_EMCR, 0)\r\n");
            return 0;
        }
        
        /* 解锁后重读 NVIC_CPUID */
        {
            uint32_t cpuid[4];
            
            /* NVIC_CPUID = 0xE000ED00 */
            if (!MUL_swd_read_word(NVIC_CPUID, cpuid))
            {
                printf(".MUL_swd_read_word(NVIC_CPUID, cpuid) error\r\n");
                return 0;             
            }
            
            if (g_gMulSwd.MultiMode >= 1)
            {
                printf(".NVIC_CPUID1 = %08X, %s\r\n", cpuid[0], swd_arm_core_info(cpuid[0]));
            }
            if (g_gMulSwd.MultiMode >= 2)
            {
                printf(".NVIC_CPUID2 = %08X, %s\r\n", cpuid[1], swd_arm_core_info(cpuid[1]));
            }
            if (g_gMulSwd.MultiMode >= 3)
            {
                printf(".NVIC_CPUID3 = %08X, %s\r\n", cpuid[2], swd_arm_core_info(cpuid[2]));
            }
            if (g_gMulSwd.MultiMode >= 4)
            {                
                printf(".NVIC_CPUID4 = %08X, %s\r\n", cpuid[3], swd_arm_core_info(cpuid[3]));
            }
            {
                uint8_t err = 0;
                
                for (i = 0; i < 4; i++)
                {
                    if (g_gMulSwd.Active[i] == 1)
                    {
                        if ((cpuid[i] & 0xFF000000) != 0x41000000)
                        {
                            err = 1;
                        }
                    }
                }
                
                if (err == 1)
                {
                    ;
                }
            }
        }     
        return 1;
    }
    
    return 0;
}
#endif
