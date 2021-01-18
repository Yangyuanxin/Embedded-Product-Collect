/*
*********************************************************************************************************
*
*    模块名称 : W25Q256 QSPI驱动模块
*    文件名称 : bsp_qspi_w25q256.h
*
*    Copyright (C),  2019-2030. 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_QSPI_W25Q256_H
#define _BSP_QSPI_W25Q256_H

enum
{
    SST25VF016B_ID = 0xBF2541,
    MX25L1606E_ID = 0xC22015,
    W25Q64_ID = 0xEF4017,
    W25Q128_ID = 0xEF4018,
    W25Q256_ID = 0xEF4019
};

/* W25Q256JV基本信息 */
#define QSPI_FLASH_SIZE 25                                     /* Flash大小，2^25 = 32MB*/
#define QSPI_SECTOR_SIZE (4 * 1024)                     /* 扇区大小，4KB */
#define QSPI_PAGE_SIZE 256                                     /* 页大小，256字节 */
#define QSPI_END_ADDR (1 << QSPI_FLASH_SIZE) /* 末尾地址 */
#define QSPI_FLASH_SIZES 32 * 1024 * 1024         /* Flash大小，2^25 = 32MB*/

/* W25Q256JV相关命令 */
#define WRITE_ENABLE_CMD 0x06                                            /* 写使能指令 */
#define READ_ID_CMD2 0x9F                                                    /* 读取ID命令 */
#define READ_STATUS_REG_CMD 0x05                                    /* 读取状态命令 */
#define SUBSECTOR_ERASE_4_BYTE_ADDR_CMD 0x21            /* 32bit地址扇区擦除指令, 4KB */
#define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD 0x34        /* 32bit地址的4线快速写入命令 */
#define QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEC /* 32bit地址的4线快速读取命令 */

/* 备份下面的命令，待以后扩展API使用 */
#if 1

/* Reset Operations */
#define RESET_ENABLE_CMD 0x66
#define RESET_MEMORY_CMD 0x99

/* Identification Operations */
#define READ_ID_CMD 0x9E
#define READ_ID_CMD2 0x9F
#define MULTIPLE_IO_READ_ID_CMD 0xAF
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD 0x5A

/* Read Operations */
#define READ_CMD 0x03
#define READ_4_BYTE_ADDR_CMD 0x13

#define FAST_READ_CMD 0x0B
#define FAST_READ_DTR_CMD 0x0D
#define FAST_READ_4_BYTE_ADDR_CMD 0x0C

#define DUAL_OUT_FAST_READ_CMD 0x3B
#define DUAL_OUT_FAST_READ_DTR_CMD 0x3D
#define DUAL_OUT_FAST_READ_4_BYTE_ADDR_CMD 0x3C

#define DUAL_INOUT_FAST_READ_CMD 0xBB
#define DUAL_INOUT_FAST_READ_DTR_CMD 0xBD
#define DUAL_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xBC

#define QUAD_OUT_FAST_READ_CMD 0x6B
#define QUAD_OUT_FAST_READ_DTR_CMD 0x6D
#define QUAD_OUT_FAST_READ_4_BYTE_ADDR_CMD 0x6C

#define QUAD_INOUT_FAST_READ_CMD 0xEB
#define QUAD_INOUT_FAST_READ_DTR_CMD 0xED
#define QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEC

/* Write Operations */
#define WRITE_ENABLE_CMD 0x06
#define WRITE_DISABLE_CMD 0x04

/* Register Operations */
#define READ_STATUS_REG_CMD 0x05
#define WRITE_STATUS_REG_CMD 0x01

#define READ_LOCK_REG_CMD 0xE8
#define WRITE_LOCK_REG_CMD 0xE5

#define READ_FLAG_STATUS_REG_CMD 0x70
#define CLEAR_FLAG_STATUS_REG_CMD 0x50

#define READ_NONVOL_CFG_REG_CMD 0xB5
#define WRITE_NONVOL_CFG_REG_CMD 0xB1

#define READ_VOL_CFG_REG_CMD 0x85
#define WRITE_VOL_CFG_REG_CMD 0x81

#define READ_ENHANCED_VOL_CFG_REG_CMD 0x65
#define WRITE_ENHANCED_VOL_CFG_REG_CMD 0x61

#define READ_EXT_ADDR_REG_CMD 0xC8
#define WRITE_EXT_ADDR_REG_CMD 0xC5

/* Program Operations */
#define PAGE_PROG_CMD 0x02
#define PAGE_PROG_4_BYTE_ADDR_CMD 0x12

#define DUAL_IN_FAST_PROG_CMD 0xA2
#define EXT_DUAL_IN_FAST_PROG_CMD 0xD2

#define QUAD_IN_FAST_PROG_CMD 0x32
#define EXT_QUAD_IN_FAST_PROG_CMD 0x12 /*0x38*/
#define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD 0x34

/* Erase Operations */
#define SUBSECTOR_ERASE_CMD 0x20
#define SUBSECTOR_ERASE_4_BYTE_ADDR_CMD 0x21

#define SECTOR_ERASE_CMD 0xD8
#define SECTOR_ERASE_4_BYTE_ADDR_CMD 0xDC

#define BULK_ERASE_CMD 0xC7

#define PROG_ERASE_RESUME_CMD 0x7A
#define PROG_ERASE_SUSPEND_CMD 0x75

/* One-Time Programmable Operations */
#define READ_OTP_ARRAY_CMD 0x4B
#define PROG_OTP_ARRAY_CMD 0x42

/* 4-byte Address Mode Operations */
#define ENTER_4_BYTE_ADDR_MODE_CMD 0xB7
#define EXIT_4_BYTE_ADDR_MODE_CMD  0xE9

/* Quad Operations */
#define ENTER_QUAD_CMD 0x35
#define EXIT_QUAD_CMD 0xF5

/* Default dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ 8
#define DUMMY_CLOCK_CYCLES_READ_QUAD 10

#define DUMMY_CLOCK_CYCLES_READ_DTR 6
#define DUMMY_CLOCK_CYCLES_READ_QUAD_DTR 8

#endif

void bsp_InitQSPI_W25Q256(void);
void QSPI_EraseSector(uint32_t address);
uint8_t QSPI_WriteBuffer(uint8_t *_pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize);
void QSPI_ReadBuffer(uint8_t *_pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
uint32_t QSPI_ReadID(void);

uint8_t QSPI_MemoryMapped(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
