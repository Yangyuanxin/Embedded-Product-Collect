
#ifndef GM_SPI_H
#define GM_SPI_H

#define SPI_CS_PIN      8 /*GPO GPIO_8*/ 
#define SPI_CLK_PIN     55 /*GPO  GPIO_11*/ 
#define SPI_DIN_PIN     3 /*GPO GPIO_3*/
#define SPI_DOUT_PIN    2 /*GPI GPIO_2*/ 


/***************************** FLASH API *****************************/
#define SPI_FLASH_PAGE_SIZE    256
#define SPI_FLASH_SECTOR_SIZE  4096  //扇区大小4k
#define SPI_RES_FILE_READ_SIZE  4096

#define MANAGE_ADDR_USER             0x00    //参数空间 
#define MANAGE_ADDR_MAX_SECTOR      7   // 最大扇区数n*4096
#define MANAGE_ADDR_PAGE_SIZE        2048  //以1024byte为一单位    

#define FLASH_CMD_READ_ID       0x9F  /* Read identification */
#define FLASH_CMD_READ          0x03  /* Read from Memory instruction Read Data */
#define FLASH_CMD_READ_SR       0x05  /* Read Status Register instruction  */

#define FLASH_CMD_WRITE_EN      0x06  /* Write enable instruction */
#define FLASH_CMD_PAGE_WRITE    0x02  /* Write to Memory instruction  Page Program */

#define FLASH_CMD_SECTOR_ERASE     0x20  /*Sector Erase instruction */
#define FLASH_CMD_BLOCK_ERASE1     0x52  /* Block Erase instruction  32K*/
#define FLASH_CMD_BLOCK_ERASE2     0xD8  /* Block Erase instruction  64K*/
#define FLASH_CMD_CHIP_ERASE       0xC7  /* Whole Chip Erase instruction */

#define FLASH_OP_TIMEOUT    5000 //500  /* flash operation time out */ 
#define FLASH_WIP_FLAG      0x01  /* Write In Progress (WIP) flag */


extern s32 GM_SpiReadData(u8* pBuffer, u32 Addr, u16 Length);
extern s32 GM_SpiWriteData(u8* pBuffer, u32 Addr, u16 Length);
extern u32 GM_SpiFlashReadID(void);
extern s32 GM_SpiPinInit(u8 cs_pin, u8 clk_pin, u8 din_pin, u8 dout_pin, u8 cs_pin_val);

#endif  /*GM_SPI_H*/
 
