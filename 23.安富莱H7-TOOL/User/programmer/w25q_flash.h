/*
*********************************************************************************************************
*
*    模块名称 : SPI Flash编程驱动
*    文件名称 : w25q_flash.h
*
*********************************************************************************************************
*/

#ifndef _W25Q_FLASH_H
#define _W25Q_FLASH_H


typedef struct
{
    uint32_t Capacity;          /* 芯片容量 */
    uint32_t SectorSize;        /* 扇区大小 */
    uint32_t EraseSectorCmd;    /* 扇区擦除指令 */
    uint32_t EraseSectorTimeout;    /* 擦除扇区超时 ms */
    uint32_t EraseChipCmd;      /* 全片擦除指令 */
    uint32_t EraseChipTimeout;  /* 擦除整片超时 ms */
    uint32_t ProgPageTimeout;   /* 编程page超时 */
    uint8_t  AutoAddrInc;       /* AAI模式写 */
    uint8_t ReadMode;           /* 0表示单线，1表示单线MOSI,双线MISO，2表示双线MOSI,双线MISO */
    uint8_t ReadIDCmd;          /* 大多数是0x9F， SST是0x90 */
    uint8_t UnlockCmd;          /* 解锁指令 */
}W25Q_T;

void W25Q_InitHard(void);
uint8_t W25Q_DetectIC(uint32_t *_id);
uint8_t W25Q_EraseSector(uint32_t _addr);
uint8_t W25Q_EraseChip(void);
uint8_t W25Q_FLASH_ProgramBuf(uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size);
uint8_t W25Q_ReadBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len);

extern W25Q_T g_tW25Q;

#endif
