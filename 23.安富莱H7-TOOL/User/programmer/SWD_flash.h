#ifndef __SWD_FLASH_H__
#define __SWD_FLASH_H__

#include <stdint.h>

#include "error.h"

/* 用于 target_flash_init()函数新参 fnc */
#define FLM_INIT_ERASE      1	/*	1 stands for Erase. */
#define FLM_INIT_PROGRAM    2	/*  2 stands for Program */
#define FLM_INIT_VERIFY     3	/*  3 stands for Verify. */


#define CHK_BLANK_ERROR     0   /* 执行查空失败 */
#define CHK_IS_BLANK        1   /* 是空片 */
#define CHK_NOT_BLANK       2   /* 不是空片 */

error_t target_flash_enter_debug_program(void);
error_t target_flash_init(uint32_t flash_start, unsigned long clk, unsigned long fnc);
error_t target_flash_uninit(void);
error_t target_flash_program_page(uint32_t addr, const uint8_t *buf, uint32_t size);
error_t target_flash_verify_page(uint32_t addr, const uint8_t *buf, uint32_t size);
error_t target_flash_erase_sector(uint32_t addr);
error_t target_flash_erase_chip(void);
uint8_t target_flash_check_blank(uint32_t addr, uint32_t size);
uint32_t target_flash_cacul_crc32(uint32_t addr, uint32_t size, uint32_t ini_value);
uint8_t target_flash_check_blank_ex(uint32_t addr, uint32_t size);
uint32_t target_flash_cacul_crc32_ex(uint32_t addr, uint32_t size, uint32_t ini_value);

uint32_t target_flash_read_extid(uint32_t addr);

uint8_t LoadCheckBlankAlgoToTarget(void);
uint8_t LoadCheckCRCAlgoToTarget(void);

#endif // __SWD_FLASH_H__
