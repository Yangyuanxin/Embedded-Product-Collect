/**
 * @file    swd_host.c
 * @brief   Implementation of swd_host.h
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
#include "swd_host.h"
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

#define DCRDR   0xE000EDF8
#define DCRSR   0xE000EDF4
#define DHCSR   0xE000EDF0
#define REGWnR  (1 << 16)

#define MAX_SWD_RETRY 100//10
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

SWD_CONNECT_TYPE reset_connect = CONNECT_NORMAL;   // CONNECT_NORMAL;CONNECT_UNDER_RESET

static DAP_STATE dap_state;
static uint32_t  soft_reset = SYSRESETREQ;

static uint8_t s_reset_state = 0;

#if  0  // armfly debug
__attribute__((weak)) void swd_set_target_reset(uint8_t asserted)
{
    (asserted) ? PIN_nRESET_OUT(0) : PIN_nRESET_OUT(1);
}
#else
void swd_set_target_reset(uint8_t asserted)
{    
//    (asserted) ? PIN_nRESET_OUT(0) : PIN_nRESET_OUT(1);  
    if (asserted)
    {
        s_reset_state = 1;
        //printf("reset gpio = 0\r\n");        
        PIN_nRESET_OUT(0);
    }
    else
    {
        s_reset_state = 0;
        
        //printf("reset gpio = 1\r\n");   
        PIN_nRESET_OUT(1);   
    }
//    if(asserted == 0)
//	{
//		swd_write_word((uint32_t)&SCB->AIRCR, ((0x5FA << SCB_AIRCR_VECTKEY_Pos) |(SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) | SCB_AIRCR_SYSRESETREQ_Msk));
//	}    
}
#endif

uint8_t swd_get_target_reset(void)
{    
    return s_reset_state;
}

uint32_t target_get_apsel()
{
//    if (g_target_family && g_target_family->apsel) {
//        return g_target_family->apsel;
//    } else {
//        return 0;
//    }
    return 0;
}


static uint32_t swd_get_apsel(uint32_t adr)
{
    uint32_t apsel = target_get_apsel();
    if (!apsel)
        return adr & 0xff000000;
    else
        return apsel;
}

void swd_set_reset_connect(SWD_CONNECT_TYPE type)
{
    reset_connect = type;
}

void int2array(uint8_t *res, uint32_t data, uint8_t len)
{
    uint8_t i = 0;

    for (i = 0; i < len; i++) {
        res[i] = (data >> 8 * i) & 0xff;
    }
}

uint8_t swd_transfer_retry(uint32_t req, uint32_t *data)
{
    uint8_t i, ack;

    for (i = 0; i < MAX_SWD_RETRY; i++) {
        ack = SWD_Transfer(req, data);

        // if ack != WAIT
        if (ack != DAP_TRANSFER_WAIT) {
            return ack;
        }
    }

    return ack;
}

void swd_set_soft_reset(uint32_t soft_reset_type)
{
    soft_reset = soft_reset_type;
}

uint8_t swd_init(void)
{
    static uint8_t s_first_run = 0;
    //TODO - DAP_Setup puts GPIO pins in a hi-z state which can
    //       cause problems on re-init.  This needs to be investigated
    //       and fixed.
    DAP_Setup();
    
    PORT_SWD_SETUP();   /* V1.31内部已取消reset硬件设置 */
    
       // Set RESET HIGH
    //pin_out_od_init(nRESET_PIN_PORT, nRESET_PIN_Bit);//TODO - fix reset logic
    //BSP_SET_GPIO_1(nRESET_PIN_PORT, nRESET_PIN);
    
    if (s_first_run == 0)
    {
        s_first_run = 1;
        BSP_SET_GPIO_1(nRESET_PIN_PORT, nRESET_PIN);
    
    }

    return 1;
}

uint8_t swd_off(void)
{
    PORT_OFF();
    return 1;
}

uint8_t swd_clear_errors(void)
{
    if (!swd_write_dp(DP_ABORT, STKCMPCLR | STKERRCLR | WDERRCLR | ORUNERRCLR)) {
        return 0;
    }
    return 1;
}

// Read debug port register.
uint8_t swd_read_dp(uint8_t adr, uint32_t *val)
{
    uint32_t tmp_in;
    uint8_t tmp_out[4];
    uint8_t ack;
    uint32_t tmp;
    tmp_in = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(adr);
    ack = swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
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

// Write debug port register
uint8_t swd_write_dp(uint8_t adr, uint32_t val)
{
    uint32_t req;
    uint8_t data[4];
    uint8_t ack;

    //check if the right bank is already selected
    if ((adr == DP_SELECT) && (dap_state.select == val)) {
        return 1;
    }

    req = SWD_REG_DP | SWD_REG_W | SWD_REG_ADR(adr);
    int2array(data, val, 4);
    ack = swd_transfer_retry(req, (uint32_t *)data);
    if ((ack == DAP_TRANSFER_OK) && (adr == DP_SELECT)) {
        dap_state.select = val;
    }
    return (ack == 0x01);
}

// Read access port register.
uint8_t swd_read_ap(uint32_t adr, uint32_t *val)
{
    uint8_t tmp_in, ack;
    uint8_t tmp_out[4];
    uint32_t tmp;
    uint32_t apsel = swd_get_apsel(adr);
    uint32_t bank_sel = adr & APBANKSEL;

    if (!swd_write_dp(DP_SELECT, apsel | bank_sel)) {
        return 0;
    }

    tmp_in = SWD_REG_AP | SWD_REG_R | SWD_REG_ADR(adr);
    // first dummy read
    swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
    ack = swd_transfer_retry(tmp_in, (uint32_t *)tmp_out);
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

// Write access port register
uint8_t swd_write_ap(uint32_t adr, uint32_t val)
{
    uint8_t data[4];
    uint8_t req, ack;
    uint32_t apsel = swd_get_apsel(adr);
    uint32_t bank_sel = adr & APBANKSEL;

    if (!swd_write_dp(DP_SELECT, apsel | bank_sel)) {
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

    if (swd_transfer_retry(req, (uint32_t *)data) != 0x01) {
        return 0;
    }

    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, NULL);
    return (ack == 0x01);
}


// Write 32-bit word aligned values to target memory using address auto-increment.
// size is in bytes.
static uint8_t swd_write_block(uint32_t address, uint8_t *data, uint32_t size)
{
    uint8_t tmp_in[4], req;
    uint32_t size_in_words;
    uint32_t i, ack;

    if (size == 0) {
        return 0;
    }

    size_in_words = size / 4;

    // CSW register
    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    // TAR write
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);
    int2array(tmp_in, address, 4);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != 0x01) {
        return 0;
    }

    // DRW write
    req = SWD_REG_AP | SWD_REG_W | (3 << 2);

    for (i = 0; i < size_in_words; i++) {
        if (swd_transfer_retry(req, (uint32_t *)data) != 0x01) {
            return 0;
        }

        data += 4;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, NULL);
    return (ack == 0x01);
}

// Read 32-bit word aligned values from target memory using address auto-increment.
// size is in bytes.
static uint8_t swd_read_block(uint32_t address, uint8_t *data, uint32_t size)
{
    uint8_t tmp_in[4], req, ack;
    uint32_t size_in_words;
    uint32_t i;

    if (size == 0) {
        return 0;
    }

    size_in_words = size / 4;

    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    // TAR write
    req = SWD_REG_AP | SWD_REG_W | AP_TAR;
    int2array(tmp_in, address, 4);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != DAP_TRANSFER_OK) {
        return 0;
    }

    // read data
    req = SWD_REG_AP | SWD_REG_R | AP_DRW;

    // initiate first read, data comes back in next read
    if (swd_transfer_retry(req, NULL) != 0x01) {
        return 0;
    }

    for (i = 0; i < (size_in_words - 1); i++) {
        if (swd_transfer_retry(req, (uint32_t *)data) != DAP_TRANSFER_OK) {
            return 0;
        }

        data += 4;
    }

    // read last word
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, (uint32_t *)data);
    return (ack == 0x01);
}

// Read target memory.
static uint8_t swd_read_data(uint32_t addr, uint32_t *val)
{
    uint8_t tmp_in[4];
    uint8_t tmp_out[4];
    uint8_t req, ack;
    uint32_t tmp;
    // put addr in TAR register
    int2array(tmp_in, addr, 4);
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != 0x01) {
        return 0;
    }

    // read data
    req = SWD_REG_AP | SWD_REG_R | (3 << 2);

    if (swd_transfer_retry(req, (uint32_t *)tmp_out) != 0x01) {
        return 0;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, (uint32_t *)tmp_out);
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

// Write target memory.
static uint8_t swd_write_data(uint32_t address, uint32_t data)
{
    uint8_t tmp_in[4];
    uint8_t req, ack;
    // put addr in TAR register
    int2array(tmp_in, address, 4);
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != 0x01) {
        return 0;
    }

    // write data
    int2array(tmp_in, data, 4);
    req = SWD_REG_AP | SWD_REG_W | (3 << 2);

    if (swd_transfer_retry(req, (uint32_t *)tmp_in) != 0x01) {
        return 0;
    }

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = swd_transfer_retry(req, NULL);
    return (ack == 0x01) ? 1 : 0;
}

// Read 32-bit word from target memory.
uint8_t swd_read_word(uint32_t addr, uint32_t *val)
{
    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    if (!swd_read_data(addr, val)) {
        return 0;
    }

    return 1;
}

// Write 32-bit word to target memory.
uint8_t swd_write_word(uint32_t addr, uint32_t val)
{
    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE32)) {
        return 0;
    }

    if (!swd_write_data(addr, val)) {
        return 0;
    }

    return 1;
}

// Read 8-bit byte from target memory.
uint8_t swd_read_byte(uint32_t addr, uint8_t *val)
{
    uint32_t tmp;

    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE8)) {
        return 0;
    }

    if (!swd_read_data(addr, &tmp)) {
        return 0;
    }

    *val = (uint8_t)(tmp >> ((addr & 0x03) << 3));
    return 1;
}

// Write 8-bit byte to target memory.
uint8_t swd_write_byte(uint32_t addr, uint8_t val)
{
    uint32_t tmp;

    if (!swd_write_ap(AP_CSW, CSW_VALUE | CSW_SIZE8)) {
        return 0;
    }

    tmp = val << ((addr & 0x03) << 3);

    if (!swd_write_data(addr, tmp)) {
        return 0;
    }

    return 1;
}

// Read unaligned data from target memory.
// size is in bytes.
uint8_t swd_read_memory(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t n;

    // Read bytes until word aligned
    while ((size > 0) && (address & 0x3)) {
        if (!swd_read_byte(address, data)) {
            return 0;
        }

        address++;
        data++;
        size--;
    }

    // Read word aligned blocks
    while (size > 3) {
        // Limit to auto increment page size
        n = TARGET_AUTO_INCREMENT_PAGE_SIZE - (address & (TARGET_AUTO_INCREMENT_PAGE_SIZE - 1));

        if (size < n) {
            n = size & 0xFFFFFFFC; // Only count complete words remaining
        }

        if (!swd_read_block(address, data, n)) {
            return 0;
        }

        address += n;
        data += n;
        size -= n;
    }

    // Read remaining bytes
    while (size > 0) {
        if (!swd_read_byte(address, data)) {
            return 0;
        }

        address++;
        data++;
        size--;
    }

    return 1;
}

// Write unaligned data to target memory.
// size is in bytes.
uint8_t swd_write_memory(uint32_t address, uint8_t *data, uint32_t size)
{
    uint32_t n = 0;
    
    // Write bytes until word aligned
    while ((size > 0) && (address & 0x3)) {
        if (!swd_write_byte(address, *data)) {
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

        if (!swd_write_block(address, data, n)) {
           goto err_quit;;
        }

        address += n;
        data += n;
        size -= n;
    }

    // Write remaining bytes
    while (size > 0) {
        if (!swd_write_byte(address, *data)) {
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
static uint8_t swd_write_debug_state(DEBUG_STATE *state)
{
    uint32_t i, status;

    if (!swd_write_dp(DP_SELECT, 0)) {
        return 0;
    }

    // R0, R1, R2, R3
    for (i = 0; i < 4; i++) {
        if (!swd_write_core_register(i, state->r[i])) {
            return 0;
        }
    }

    // R9
    if (!swd_write_core_register(9, state->r[9])) {
        return 0;
    }

    // R13, R14, R15
    for (i = 13; i < 16; i++) {
        if (!swd_write_core_register(i, state->r[i])) {
            return 0;
        }
    }

    // xPSR
    if (!swd_write_core_register(16, state->xpsr)) {
        return 0;
    }

    if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_MASKINTS | C_HALT)) {
        return 0;
    }

    if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_MASKINTS)) {
        return 0;
    }

    // check status
    if (!swd_read_dp(DP_CTRL_STAT, &status)) {
        return 0;
    }

    if (status & (STICKYERR | WDATAERR)) {
        return 0;
    }

    return 1;
}

uint8_t swd_read_core_register(uint32_t n, uint32_t *val)
{
    int i = 0, timeout = 100;

    if (!swd_write_word(DCRSR, n)) {
        return 0;
    }

    // wait for S_REGRDY
    for (i = 0; i < timeout; i++) {
        if (!swd_read_word(DHCSR, val)) {
            return 0;
        }

        if (*val & S_REGRDY) {
            break;
        }
    }

    if (i == timeout) {
        return 0;
    }

    if (!swd_read_word(DCRDR, val)) {
        return 0;
    }

    return 1;
}

uint8_t swd_write_core_register(uint32_t n, uint32_t val)
{
    int i = 0, timeout = 100;

    if (!swd_write_word(DCRDR, val)) {
        return 0;
    }

    if (!swd_write_word(DCRSR, n | REGWnR)) {
        return 0;
    }

    // wait for S_REGRDY
    for (i = 0; i < timeout; i++) {
        if (!swd_read_word(DHCSR, &val)) {
            return 0;
        }

        if (val & S_REGRDY) {
            return 1;
        }
    }

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: swd_wait_until_halted
*    功能说明: 执行FLM中的函数，等待完成. 增加了超时控制，全局变量 g_tProg.FLMFuncTimeout
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void PG_PrintPercent(float _Percent, uint32_t _Addr);
extern uint8_t ProgCancelKey(void);
extern void PG_PrintText(char *_str);
static uint8_t swd_wait_until_halted(void)
{
#if 1
    // Wait for target to stop
    uint32_t val;
    int32_t time1;
    int32_t addtime = 0;
    int32_t tt0 = 0;

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
                    printf("error : swd_wait_until_halted() timeout\r\n");
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
        
        if (!swd_read_word(DBG_HCSR, &val)) 
        {
            break;
        }

        if (val & S_HALT) 
        {
            g_tProg.FLMFuncDispProgress = 0;
            return 1;       /* 执行OK */
        } 
        
        if (ProgCancelKey())
        {
            PG_PrintText("用户终止运行");    
            break;         
        }        
    }
    g_tProg.FLMFuncDispProgress = 0;
    return 0;
#else    
    // Wait for target to stop
    uint32_t val, i, timeout = MAX_TIMEOUT;

    for (i = 0; i < timeout; i++) {
        if (!swd_read_word(DBG_HCSR, &val)) {
            return 0;
        }

        if (val & S_HALT) {
            return 1;
        }
    }

    return 0;
#endif    
}

uint8_t swd_flash_syscall_exec(const program_syscall_t *sysCallParam, uint32_t entry, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    DEBUG_STATE state = {{0}, 0};
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

    if (!swd_write_debug_state(&state)) {
        return 0;
    }

    if (!swd_wait_until_halted()) {
        return 0;
    }

    if (!swd_read_core_register(0, &state.r[0])) {
        return 0;
    }
    
    //remove the C_MASKINTS
    if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
        return 0;
    }

    // Flash functions return 0 if successful.
    if (state.r[0] != 0) {
        return 0;
    }

    return 1;
}

uint32_t swd_flash_syscall_exec_ex(const program_syscall_t *sysCallParam, uint32_t entry, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4)
{
    DEBUG_STATE state = {{0}, 0};
    static uint32_t R0[1];
    
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

    if (!swd_write_debug_state(&state)) {
        return 0;
    }

    if (!swd_wait_until_halted()) {
        return 0;
    }

    if (!swd_read_core_register(0, &state.r[0])) {
    
        return 0;
    }
    
    //remove the C_MASKINTS
    if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
        return 0;
    }

    R0[0] = state.r[0];
    return (uint32_t)R0;    /* 返回存放结果的内存地址 */
}

// SWD Reset
static uint8_t swd_reset(void)
{
    uint8_t tmp_in[8];
    uint8_t i = 0;

    for (i = 0; i < 8; i++) {
        tmp_in[i] = 0xff;
    }

    SWJ_Sequence(51, tmp_in);
    return 1;
}

// SWD Switch
static uint8_t swd_switch(uint16_t val)
{
    uint8_t tmp_in[2];
    tmp_in[0] = val & 0xff;
    tmp_in[1] = (val >> 8) & 0xff;
    SWJ_Sequence(16, tmp_in);
    return 1;
}

// SWD Read ID
//static uint8_t swd_read_idcode(uint32_t *id)
uint8_t swd_read_idcode(uint32_t *id)
{
    uint8_t tmp_in[1];
    uint8_t tmp_out[4];
    tmp_in[0] = 0x00;
    SWJ_Sequence(8, tmp_in);

    if (swd_read_dp(0, (uint32_t *)tmp_out) != 0x01) {
        return 0;
    }

    *id = (tmp_out[3] << 24) | (tmp_out[2] << 16) | (tmp_out[1] << 8) | tmp_out[0];
    return 1;
}


static uint8_t JTAG2SWD()
{
    uint32_t tmp = 0;

    if (!swd_reset()) {
        return 0;
    }

    if (!swd_switch(0xE79E)) {
        return 0;
    }

    if (!swd_reset()) {
        return 0;
    }

    /* 旧协议，J-LINK如此发送 */
    if (!swd_switch(0xEDB6)) {
        return 0;
    }

    if (!swd_reset()) {
        return 0;
    }
    
    if (!swd_read_idcode(&tmp)) {
        return 0;
    }

    return 1;
}

static uint8_t JTAG2SWD_2(uint32_t *tmp)
{
    *tmp = 0;
    
    if (!swd_reset()) {
        return 0;
    }

    if (!swd_switch(0xE79E)) {
        return 0;
    }

    if (!swd_reset()) {
        return 0;
    }

    /* 旧协议，J-LINK如此发送 */
    if (!swd_switch(0xEDB6)) {
        return 0;
    }

    if (!swd_reset()) {
        return 0;
    }
    
    if (!swd_read_idcode(tmp)) {
        return 0;
    }

    return 1;
}


// 根据CPUID返回ARM内核系列
typedef struct
{
    uint32_t PartNo;
    char *Name;
}ARM_CORE_LIST_T;

const char * swd_arm_core_info(uint32_t cpuid)
{
   /*
    【M0】
    [31:24] Implementer  Implementer code: 0x41 = ARM.
    [23:20] Variant  Implementation defined. In ARM implementations this is the major revision number n in the rn part of the rnpn revision status, 
        Product revision status: 0x0.
    [19:16] Constant    Indicates the architecture, ARMv6-M: 0xC.
    [15:4]  Partno  Indicates part number, Cortex-M0: 0xC20.
    [3:0]   Revision    Indicates revision. In ARM implementations this is the minor revision number n in the pn part of the rnpn revision status, 
        see Product revision status. For example, for release r0p0: 0x0.

    【M0+】
    [31:24] IMPLEMENTER Implementer code: 0x41 ARM.
    [23:20] VARIANT Major revision number n in the rnpm revision status. See Product revision status:0x0.
    [19:16] ARCHITECTURE    Indicates the architecture, ARMv6-M: 0xC.
    [15:4]  PARTNO  Indicates part number, Cortex-M0+: 0xC60.
    [3:0]   REVISION    Minor revision number m in the rnpm revision status. See Product revision status. 0x1.

    【M1】
    [31:24] IMPLEMENTER  Implementer code:0x41 = ARM
    [23:20] VARIANT  Implementation defined variant number: 0x0 for r0p1
    [19:16] Constant    Reads as 0xC
    [15:4]  PARTNO  Number of processor within family: 0xC21
    [3:0]   REVISION    Implementation defined revision number: 0x1 = r0p1   

    【M23】
    [31:24] IMPLEMENTER Implementer code:0x41 = Arm.
    [23:20] VARIANT Major revision number n in the rnpm revision status: 0x1 = Revision 1.
    [19:16] ARCHITECTURE  Constant that defines the architecture of the processor: 0xC = Armv8-M architecture.
    [15:4]  PARTNO  Part number of the processor: 0xD20 = Cortex-M23.
    [3:0]   REVISION   Minor revision number m in the rnpm revision status: 0x0 = Patch 0.
        
    【M3】
    [31:24] IMPLEMENTER Indicates implementer: 0x41 = ARM?
    [23:20] VARIANT Indicates processor revision: 0x0 = Revision 0
    [19:16] (Constant)  Reads as 0xF
    [15:4]  PARTNO  Indicates part number: 0xC24 = Cortex??M3
    [3:0]   REVISION    Indicates patch release: 0x1= Patch 1.

    【M33】
    [31:24] Implementer Implementer code: 0x41  Arm?
    [23:20] Variant Variant number, the n value in the rnpm product revision identifier: 0x0    Revision 0
    [19:16] Constant    Reads as 0xF
    [15:4]  PartNo  Part number of the processor: 0xD21 Cortex??M33
    [3:0]   Revision    Revision number, the m value in the rnpm product revision identifier: 0x3   Patch 3.

    【M4】
    [31:24] IMPLEMENTER Indicates implementer: 0x41 = Arm?
    [23:20] VARIANT Indicates processor revision: 0x0 = Revision 0
    [19:16] (Constant)  Reads as 0xF
    [15:4]  PARTNO  Indicates part number: 0xC24 = Cortex?-M4
    [3:0]   REVISION    Indicates patch release: 0x1= Patch 1.
                
    【M55】
    [31:24] Implementer Implementer code: 0x41  Arm?Limited
    [23:20] Variant Variant number, the n value in the rnpm product revision identifier: 0x0    Revision 0
    [19:16] Architecture    Reads as 0b1111, Armv8.1?M with Main Extension
    [15:4]  PartNo  Part number of the processor: 0xD22 Cortex?-M55
    [3:0]   Revision    Revision number, the m value in the rnpm product revision identifier: 0x1   Patch 1.


    【M7】
    [31:24] IMPLEMENTER  Indicates implementer: 0x41  Arm.
    [23:20] VARIANT  Indicates processor revision: 0x0 Revision 0. 0x1 Revision 1.
    [19:16] ARCHITECTURE Reads as 0xF.
    [15:4]  PARTNO  Indicates part number: 0xC27  Cortex-M7.
    [3:0]   REVISION  Indicates patch release: 0x0  Patch 0.


    【M35P】
    */
    uint8_t i;
    uint32_t partno;
    char *p;
    
    const ARM_CORE_LIST_T list[] = 
    {
        {0xC60, "Cortex-M0+"},
        {0xC20, "Cortex-M0"},
        {0xC21, "Cortex-M1"},
        {0xC23, "Cortex-M3"},
        {0xC24, "Cortex-M4"},
        {0xD22, "Cortex-M55"},
        {0xD21, "Cortex-M33"},
        {0xC27, "Cortex-M7"},
        {0xD20, "Cortex-M23"},
        
        {0xFFF, "End"},      // 结束标致行
    };
        
    partno = (cpuid >> 4) & 0xFFF;
    
    for (i = 0; i < 255; i++)
    {
        if (list[i].PartNo == 0xFFF)
        {
            p = "Unknow";
            break;
        }
            
        if (list[i].PartNo == partno)
        {
            p = list[i].Name;
            break;
        }
    }
    
    return p;
}

uint8_t swd_freeze_dog(void)
{
    const char *ret_str; 
    
    lua_do("ret = InitUnderReset()");
    lua_getglobal(g_Lua, "ret"); 
    if (lua_isstring(g_Lua, -1))
    {
        ret_str = lua_tostring(g_Lua, -1); 
    }
    else
    {
        ret_str = "";
    }
    lua_pop(g_Lua, 1);

    if (strcmp(ret_str, "OK") == 0)
    {
        return 1;   /* 成功 */
    }
    
    return 0;       /* 失败 */
}       

uint8_t swd_MUC_Init(void)
{
    lua_do("if (MCU_Init ~= nil) then MCU_Init() end");
    
    return 1;
}   

// 检测SWD，用于判断芯片是否移除. 只检测一次 
uint8_t swd_detect_core(uint32_t *_id)
{
    swd_init();
        
    if (!JTAG2SWD_2(_id)) 
    {
        return 0;
    }
    return 1;
}

uint8_t swd_init_debug(void)
{
    uint32_t tmp = 0;
    int i = 0;
    int timeout = 100;
    // init dap state with fake values
    dap_state.select = 0xffffffff;
    dap_state.csw = 0xffffffff;
    
    int8_t retries = 4;
    int8_t do_abort = 0;
    do {
        if (do_abort != 0) 
        {
            //do an abort on stale target, then reset the device
            swd_write_dp(DP_ABORT, DAPABORT);
            
            swd_set_target_reset(1);
            osDelay(20);
            swd_set_target_reset(0);
            //printf("reset, delay %dms\r\n", g_tProg.SwdResetDelay);
            osDelay(g_tProg.SwdResetDelay);
            do_abort = 0;
        }        
        
        swd_init();
        
        if (!JTAG2SWD()) {
            do_abort = 1;   /* 这种情况返回，有可能是用户程序禁止了SWD口 */
            continue;
        }	

        if (!swd_clear_errors()) {
            do_abort = 2;
            continue;
        }

        if (!swd_write_dp(DP_SELECT, 0)) {
            do_abort = 3;
            continue;
            
        }
        
        // Power up
        if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
            do_abort = 4;
            continue;
        }

        for (i = 0; i < timeout; i++) {
            if (!swd_read_dp(DP_CTRL_STAT, &tmp)) {
                do_abort = 5;
                break;
            }
            if ((tmp & (CDBGPWRUPACK | CSYSPWRUPACK)) == (CDBGPWRUPACK | CSYSPWRUPACK)) {
                // Break from loop if powerup is complete
                break;
            }
        }
        if ((i == timeout) || (do_abort == 1)) {
            // Unable to powerup DP
            do_abort = 6;
            continue;
        }

        if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ | TRNNORMAL | MASKLANE)) {
            do_abort = 7;
            continue;
        }
		
        if (!swd_write_dp(DP_SELECT, 0)) {
            do_abort = 8;
            continue;
        }				

        return 1;
    
    } while (--retries > 0);
    
    return 0;
}

uint8_t swd_set_target_state_hw(TARGET_RESET_STATE state)
{
    uint32_t val;
    int8_t ap_retries = 2;
    /* Calling swd_init prior to entering RUN state causes operations to fail. */
    if (state != RUN) {
        swd_init();
    }

    switch (state) {
        case RESET_HOLD:
            swd_set_target_reset(1);
            break;

        case RESET_RUN:
            swd_set_target_reset(1);
            osDelay(2);
            swd_set_target_reset(0);
            osDelay(2);
            swd_off();
            break;

        case RESET_PROGRAM:
            {
                int k;
                int err = 0;
                
                for (k = 0; k < 3; k++)
                {
                    err = 0;
                    if (!swd_init_debug()) {
                        err = 1;
                        continue;
                    }
                    
                    if (reset_connect == CONNECT_UNDER_RESET) {
                        // Assert reset
                        swd_set_target_reset(1); 
                        osDelay(20);
                    }

                    // Enable debug
                    while(swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN) == 0) {
                        if( --ap_retries <=0 )
                            return 0;
                        // Target is in invalid state?
                        swd_set_target_reset(1);
                        osDelay(20);
                        swd_set_target_reset(0);
                        osDelay(20);
                    }

                    // Enable halt on reset
                    if (!swd_write_word(DBG_EMCR, VC_CORERESET)) {
                        err = 2;
                        continue;
                    }
                    
                    if (reset_connect == CONNECT_NORMAL) {
                        // Assert reset
                        swd_set_target_reset(1); 
                        osDelay(20);
                    }
                    
                    // Deassert reset
                    swd_set_target_reset(0);
                    osDelay(20);
                    
                    /* 2020-01-18 armfly 增加退出机制, 200ms */
                    #if 1
                    {
                        uint32_t i;
                        
                        for (i = 0; i < 500; i++)
                        {
                            if (!swd_read_word(DBG_HCSR, &val)) {
                                err = 3;
                                break;
                            }
                            
                            if ((val & S_HALT) != 0)
                            {
                                break;
                            }
                            
                            bsp_DelayUS(1000);
                        }    

                        if (err > 0)
                        {
                            continue;   
                        }
                    }
                    #else            
                        do {
                            if (!swd_read_word(DBG_HCSR, &val)) {
                                return 0;
                            }
                        } while ((val & S_HALT) == 0);
                    #endif

                    // Disable halt on reset
                    if (!swd_write_word(DBG_EMCR, 0)) {
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
            if (!swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }

            break;

        case DEBUG:
            if (!JTAG2SWD()) {
                return 0;
            }

            if (!swd_clear_errors()) {
                return 0;
            }

            // Ensure CTRL/STAT register selected in DPBANKSEL
            if (!swd_write_dp(DP_SELECT, 0)) {
                return 0;
            }

            // Power up
            if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
                return 0;
            }

            // Enable debug
            if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN)) {
                return 0;
            }

            break;

        case HALT:
            if (!swd_init_debug()) {
                return 0;
            }

            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
                return 0;
            }

            // Wait until core is halted
            do {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    return 0;
                }
            } while ((val & S_HALT) == 0);
            break;

        case RUN:
            if (!swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }
            swd_off();
            break;

        case POST_FLASH_RESET:
            // This state should be handled in target_reset.c, nothing needs to be done here.
            break;

        default:
            return 0;
    }

    return 1;
}

uint8_t swd_set_target_state_sw(TARGET_RESET_STATE state)
{
    uint32_t val;
    int8_t ap_retries = 2;
//    /* Calling swd_init prior to enterring RUN state causes operations to fail. */
//    if (state != RUN) {
//        swd_init();
//    }

    switch (state) {
        case RESET_HOLD:
            swd_init();
            swd_set_target_reset(1);
            break;

        case RESET_RUN:
            swd_set_target_reset(1);
            osDelay(2);
            swd_set_target_reset(0);
            osDelay(2);
            swd_off();
            break;

        case RESET_PROGRAM:
//            swd_init();
//               
//            if (!swd_init_debug()) {
//                return 0;
//            }

            /*
                DBG_HCSR : 调试控制和状态寄存器. 提供内核状态信息，允许内核进入调试模式，
                    和提供单步功能。
            */
            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            while (swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT) == 0) {
                if ( --ap_retries <=0 ) {
                    printf("error : swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)\r\n");
                    return 0;
                }
                // Target is in invalid state?
                swd_set_target_reset(1);
                osDelay(2);
                swd_set_target_reset(0);
                osDelay(2);
            }          
            
            /* 2020-01-18 armfly 增加退出机制 */
            #if 1
            // Wait until core is halted
            {
                uint32_t i;
                
                for (i = 0; i < 100000; i++)
                {
                    if (!swd_read_word(DBG_HCSR, &val)) {
                        printf("error: swd_read_word(DBG_HCSR, &val) --1 i = %d\r\n", i);
                        return 0;
                    }
                    
                    if ((val & S_HALT) != 0)
                    {
                        break;
                    }
                }
                
                if (i == 100000)
                {
                    printf("error: swd_read_word(DBG_HCSR, &val) --1\r\n"); 
                    return 0;   /* 超时 */
                }
            }
            #else              
                // Wait until core is halted
                do {
                    if (!swd_read_word(DBG_HCSR, &val)) {
                        return 0;
                    }
                } while ((val & S_HALT) == 0);
            #endif
            
            // Enable halt on reset
            if (!swd_write_word(DBG_EMCR, VC_CORERESET)) {
                printf("error: swd_write_word(DBG_EMCR, VC_CORERESET)\r\n");
                return 0;
            }

            // Perform a soft reset
            if (!swd_read_word(NVIC_AIRCR, &val)) {
                printf("error: swd_read_word(NVIC_AIRCR, &val)\r\n");
                return 0;
            }

            if (!swd_write_word(NVIC_AIRCR, VECTKEY | (val & SCB_AIRCR_PRIGROUP_Msk) | soft_reset)) {
                printf("error: swd_write_word(NVIC_AIRCR, VECTKEY | (val & SCB_AIRCR_PRIGROUP_Msk) | soft_reset)\r\n");
                return 0;
            }

            osDelay(2);

//            do {
//                if (!swd_read_word(DBG_HCSR, &val)) {
//                    return 0;
//                }
//            } while ((val & S_HALT) == 0);
            /* 增加超时退出机制 */
            {
                uint32_t i;
                
                for (i = 0; i < 100000; i++)
                {
                    if (!swd_read_word(DBG_HCSR, &val)) {
                        printf("error: swd_read_word(DBG_HCSR, &val) --2 i = %d\r\n", i);
                        return 0;
                    }
                    
                    if ((val & S_HALT) != 0)
                    {
                        break;
                    }
                }
                
                if (i == 100000)
                {
                    printf("error: swd_read_word(DBG_HCSR, &val) --2\r\n"); 
                    return 0;   /* 超时 */
                }
            }            

            // Disable halt on reset
            if (!swd_write_word(DBG_EMCR, 0)) {
                return 0;
            }

            break;

        case NO_DEBUG:
            swd_init();
            if (!swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }

            break;

        case DEBUG:
            swd_init();
            if (!JTAG2SWD()) {
                return 0;
            }

            if (!swd_clear_errors()) {
                return 0;
            }

            // Ensure CTRL/STAT register selected in DPBANKSEL
            if (!swd_write_dp(DP_SELECT, 0)) {
                return 0;
            }

            // Power up
            if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
                return 0;
            }

            // Enable debug
            if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN)) {
                return 0;
            }

            break;

        case HALT:
            swd_init();
            if (!swd_init_debug()) {
                return 0;
            }

            // Enable debug and halt the core (DHCSR <- 0xA05F0003)
            if (!swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)) {
                return 0;
            }

            // Wait until core is halted
            do {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    return 0;
                }
            } while ((val & S_HALT) == 0);
            break;

        case RUN:
            if (!swd_write_word(DBG_HCSR, DBGKEY)) {
                return 0;
            }
            swd_off();
            break;

        case POST_FLASH_RESET:
            swd_init();
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
uint8_t swd_enter_debug_program(void)
{
    uint32_t val;
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
        uint32_t i;        
           
        if (!swd_init_debug()) {
            return 0;
        }

        if (swd_freeze_dog() == 0)      /* 如果冻结看门狗时钟失败（STM32H7）*/
        {            
            if (swd_freeze_dog() == 0) 
            {
                ;
            }   
        }
        
        /*
            DBG_HCSR : 调试控制和状态寄存器. 提供内核状态信息，允许内核进入调试模式，
                和提供单步功能。
        */
        // Enable debug and halt the core (DHCSR <- 0xA05F0003)
        for (i = 0; i < 5; i++)
        {
            if (swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT) != 0)
            {
                break;
            }
            swd_set_target_reset(1);
            osDelay(20);
            swd_set_target_reset(0);
            osDelay(i * 5);
        }        
        if (i >= 5)
        {    
            printf("error : swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT)\r\n");
            return 0;
        }       
        
        /* 2020-01-18 armfly 增加退出机制 */
        // Wait until core is halted
        {
            uint32_t i;
            
            for (i = 0; i < 100000; i++)
            {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    printf("error: swd_read_word(DBG_HCSR, &val) --1 i = %d\r\n", i);
                    return 0;
                }
                
                if ((val & S_HALT) != 0)
                {
                    break;
                }
            }
            
            if (i == 100000)
            {
                printf("error: swd_read_word(DBG_HCSR, &val) --1\r\n"); 
                return 0;   /* 超时 */
            }
        }
        
        // Enable halt on reset
        if (!swd_write_word(DBG_EMCR, VC_CORERESET)) {
            printf("error: swd_write_word(DBG_EMCR, VC_CORERESET)\r\n");
            return 0;
        }

        // Perform a soft reset
        if (!swd_read_word(NVIC_AIRCR, &val)) {
            printf("error: swd_read_word(NVIC_AIRCR, &val)\r\n");
            return 0;
        }

        if (!swd_write_word(NVIC_AIRCR, VECTKEY | (val & SCB_AIRCR_PRIGROUP_Msk) | soft_reset)) {
            printf("error: swd_write_word(NVIC_AIRCR, VECTKEY | (val & SCB_AIRCR_PRIGROUP_Msk) | soft_reset)\r\n");
            return 0;
        }

        osDelay(2);

        /* 增加超时退出机制 */
        {
            uint32_t i;
            
            for (i = 0; i < 100000; i++)
            {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    printf("error: swd_read_word(DBG_HCSR, &val) --2 i = %d\r\n", i);
                    return 0;
                }
                
                if ((val & S_HALT) != 0)
                {
                    break;
                }
            }
            
            if (i == 100000)
            {
                printf("error: swd_read_word(DBG_HCSR, &val) --2\r\n"); 
                return 0;   /* 超时 */
            }
        }            

        // Disable halt on reset
        if (!swd_write_word(DBG_EMCR, 0)) {
            return 0;
        }

        {
            uint32_t cpuid;
            
            /* NVIC_CPUID = 0xE000ED00 */
            if (!swd_read_memory(NVIC_CPUID, (uint8_t *)&cpuid, 4))
            {
                 return 0;          
            } 
            printf(".NVIC_CPUID = %08X, %s\r\n", cpuid, swd_arm_core_info(cpuid));        
        }
        
        return 1;
    }    
   
    /* 硬件复位 */
    if (ResetMode == 2)
    {
        /* 进入编程状态，先复位一次，应对已看门狗低功耗程序的片子 */
        swd_set_target_reset(1);
        osDelay(10);
        swd_set_target_reset(0);
        
        if (swd_init_debug() == 0)
        {
            printf("error 1: swd_init_debug()\r\n");
            return 0;
        }
        
        if (swd_freeze_dog() == 0)      /* 如果冻结看门狗时钟失败（STM32H7）*/
        {
            if (swd_get_target_reset() == 0)
            {
                swd_set_target_reset(1);    /* 硬件复位 */
                osDelay(g_tProg.SwdResetDelay);
            }
            
            if (swd_init_debug() == 0)
            {
                printf("error 2: swd_init_debug()\r\n");
                return 0;
            }
            
            if (swd_freeze_dog() == 0) 
            {
                ;
            }   
        }

        /*
            DBG_HCSR : 调试控制和状态寄存器. 提供内核状态信息，允许内核进入调试模式，
                和提供单步功能。
        */
        // Enable debug and halt the core (DHCSR <- 0xA05F0003)
        {
            uint8_t i;
            
            for (i = 0; i < 10; i++)
            {
                if (swd_write_word(DBG_HCSR, DBGKEY | C_DEBUGEN | C_HALT) != 0)
                {
                    break;
                }
                swd_set_target_reset(1);
                osDelay(20);
                swd_set_target_reset(0);
                osDelay(i * 5);     /* 硬件复位退出后 立即写指令 */            
            }
        }     

        // Enable halt on reset
        if (!swd_write_word(DBG_EMCR, VC_CORERESET)) {
            printf("error 3: swd_write_word(DBG_EMCR, VC_CORERESET)\r\n");
            return 0;   /* 超时 */
        }
        
        swd_read_word(DBG_HCSR, &val);
        if ((val & S_HALT) == 0)
        {
            if (swd_get_target_reset() == 0)
            {
                swd_set_target_reset(1); 
                osDelay(20);
            }
            
            if (swd_get_target_reset() == 1)
            {
                swd_set_target_reset(0);        /* 退出硬件复位 */    
                osDelay(g_tProg.SwdResetDelay);
            }  
        } 
                        
        /* 2020-01-18 armfly 增加退出机制 */
        {
            uint32_t i;
            
            for (i = 0; i < 100000; i++)
            {
                if (!swd_read_word(DBG_HCSR, &val)) {
                    printf("error 4: swd_read_word(DBG_HCSR, &val) i = %d\r\n", i);
                    return 0;
                }
                
                if ((val & S_HALT) != 0)
                {
                    break;
                }
            }
            
            if (i == 100000)
            {
                printf("error 5: swd_read_word(DBG_HCSR, &val)\r\n"); 
                return 0;   /* 超时 */
            }
        }
        

        // Disable halt on reset
        if (!swd_write_word(DBG_EMCR, 0)) {
            printf("error 6: swd_write_word(DBG_EMCR, 0)\r\n"); 
            return 0;
        }

        {
            uint32_t cpuid;
            
            /* NVIC_CPUID = 0xE000ED00 */
            if (!swd_read_memory(NVIC_CPUID, (uint8_t *)&cpuid, 4))
            {
                 return 0;          
            } 
            printf(".NVIC_CPUID = %08X, %s\r\n", cpuid, swd_arm_core_info(cpuid));        
        }
            
        return 1;
    }
    
    return 0;
}
#endif
