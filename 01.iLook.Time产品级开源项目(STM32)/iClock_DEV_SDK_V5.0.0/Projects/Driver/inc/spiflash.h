/*
 * File      : sst25vfxx.h
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-11-10     aozima       first implementation
*/

#ifndef W25XX_H_INCLUDED
#define W25XX_H_INCLUDED

/* function list */
extern uint16_t spiflash_init(void);
extern uint32_t spiflash_get_device_id(void);
extern uint32_t spiflash_read(uint32_t offset,uint8_t * buffer,uint32_t size);
extern uint32_t spiflash_page_write(uint32_t page,const uint8_t * buffer,uint32_t size);

/* command list */
#define SPI_FLASH_CMD_RDSR                    0x05  /* 读状态寄存器     */
#define SPI_FLASH_CMD_WRSR                    0x01  /* 写状态寄存器     */
#define SPI_FLASH_CMD_WRDI                    0x04  /* 关闭写使能       */
#define SPI_FLASH_CMD_WREN                    0x06  /* 打开写使能       */
#define SPI_FLASH_CMD_READ                    0x03  /* 读数据           */
#define SPI_FLASH_CMD_FAST_READ               0x0B  /* 快速读           */
#define SPI_FLASH_CMD_PAGE_PROG               0x02  /* 页编程  256Byte  */
#define SPI_FLASH_CMD_ERASE_4K                0x20  /* 扇区擦除:4K      */
#define SPI_FLASH_CMD_ERASE_64K               0xD8  /* 扇区擦除:64K     */
#define SPI_FLASH_CMD_ERASE_full              0xC7  /* 全片擦除         */
#define SPI_FLASH_CMD_JEDEC_ID                0x9F  /* 读 JEDEC_ID      */
#define SPI_FLASH_CMD_POWER_DOWN              0xB9  /* */
#define SPI_FLASH_CMD_RES_DEVICE_ID           0xAB  /* release power    */
#define SPI_FLASH_CMD_MANUFACTURER_ID         0x90  /* */

/* device id define */
enum _w25xx_id
{
    unknow     = 0,
    W25X16 = 0x001530EF,
    W25X32 = 0x001630EF,
    W25X64 = 0x001730EF,
    W25Q64 = 0x001740EF,
    W25Q128 = 0x001840EF,
    W25Q256 = 0x001940EF,
};

/* memory info struct */
typedef struct{
    uint16_t state; //0 未初始化,或者不成功; 1 已经初始化
    uint32_t device_id;
    uint32_t flash_size;
    uint32_t block_size;
    uint32_t sector_size;
}MEMORY_INFO;

uint16_t spiflash_init(void);

#endif // SST25VFXX_H_INCLUDED
