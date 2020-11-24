/*
 * File      : w25xx.c
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-11-10     aozima       first implementation
 * 2010-11-30     aozima       port to W25x16/W25X32/W25X64
*/

#include "hw_config.h"
#include "sys_hw.h"
#include "spiflash.h"

MEMORY_INFO gSpiFlash;

// bsp support:
// port_init()                         : init hardware GPIO.
// CS_HIGH()                           :
// CS_LOW()                            :
// spi_lock()                          : spi lock
// spi_unlock()                        : spi unlock
// spi_config()                        : config spi for w25xx

/********* bsp **********/
// CS       PA2
// SPI <--> SPI1

#include "stm32f10x.h"
//#include "rtthread.h"

#define FLASH_TRACE(...)
//#define FLASH_TRACE  rt_kprintf

#define CS_HIGH()    GPIO_SetBits(SPI_CS_FL,SPI_CS_FL_PIN)
#define CS_LOW()     GPIO_ResetBits(SPI_CS_FL,SPI_CS_FL_PIN)

#define spi_lock()
#define spi_unlock()

#define spi_config()          SPI_Config(CFG_SPI_FLASH)


/*******************************************************************************
* Function Name  : spiflash_read_status, spiflash_wait_busy
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
// status register
// S7    S6   S5   S4   S3   S2   S1   S0
// SRP   (R)  TB  BP2  BP1  BP0  WEL  BUSY

static uint8_t spiflash_read_status(void)
{
    uint8_t tmp;

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_RDSR );
    tmp=SPI_RdWrByte(0XFF);
    CS_HIGH();
    return tmp;
}

static void spiflash_wait_busy(void)
{
    while( spiflash_read_status() & (0x01));
}

/** \brief w25xx SPI flash init
 *
 * \param void
 * \return void
 *
 */
uint16_t spiflash_init(void)
{
    spi_lock();
    spi_config();
    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_WRDI );
    CS_HIGH();

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_JEDEC_ID );
    gSpiFlash.device_id  = SPI_RdWrByte(0xFF);      // 0xEF M7-M0
    gSpiFlash.device_id |= SPI_RdWrByte(0xFF)<<8;   // 0x30 memory type
    gSpiFlash.device_id |= SPI_RdWrByte(0xFF)<<16;  // 0x15~0x17 capacity
    CS_HIGH();

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_WRSR );
    SPI_RdWrByte( 0 );
    CS_HIGH();

    spi_unlock();
    //
    switch(gSpiFlash.device_id)
    {
        case W25X16:
            gSpiFlash.state = 1;
            gSpiFlash.flash_size = 2UL*1024*1024;
            gSpiFlash.block_size = 4096;
            gSpiFlash.sector_size = 4096;
            break;
        case W25X32:
            gSpiFlash.state = 1;
            gSpiFlash.flash_size = 4UL*1024*1024;
            gSpiFlash.block_size = 4096;
            gSpiFlash.sector_size = 4096;
            break;
        case W25X64:
        case W25Q64:
    	    gSpiFlash.state = 1;
            gSpiFlash.flash_size = 8UL*1024*1024;
            gSpiFlash.block_size = 4096;
            gSpiFlash.sector_size = 4096;
            break;

        case W25Q128:
          gSpiFlash.state = 1;
          gSpiFlash.flash_size = 16UL*1024*1024;
          gSpiFlash.block_size = 4096;
          gSpiFlash.sector_size = 4096;
          break;
          
        default:
            gSpiFlash.state = 0;
    		gSpiFlash.flash_size = 0x0;
    		gSpiFlash.block_size = 0x0;
    		gSpiFlash.sector_size = 0x0;
            break;
    }
    //
    return (gSpiFlash.state);
}


/** \brief read [size] byte from [offset] to [buffer]
 *
 * \param offset uint32_t unit : byte
 * \param buffer uint8_t*
 * \param size uint32_t   unit : byte
 * \return uint32_t byte for read
 *
 */
uint32_t spiflash_read(uint32_t offset,uint8_t * buffer,uint32_t size)
{
    uint32_t index;

    if( buffer==0x0 )
      return 0;

    spi_lock();
    spi_config();

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_WRDI );
    CS_HIGH();

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_READ);  
    SPI_RdWrByte(  offset>>16 );
    SPI_RdWrByte(  offset>>8 );
    SPI_RdWrByte(  offset );

    for(index=0; index<size; index++)
    {
        *buffer++ = SPI_RdWrByte(0xFF);
    }
    CS_HIGH();

    spi_unlock();

    return size;
}

/** \brief write N page on [page]
 *
 * \param page uint32_t unit : byte (4096 * N,1 page = 4096byte)
 * \param buffer const uint8_t*
 * \param size uint32_t unit : byte ( 4096*N )
 * \return uint32_t
 *
 */
uint32_t spiflash_page_write(uint32_t page,const uint8_t * buffer,uint32_t size)
{
    uint32_t index;

    page &= ~0xFFF; // page = 4096byte

    spi_lock();
    spi_config();

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_WREN );//write en
    CS_HIGH();

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_ERASE_4K );
    SPI_RdWrByte( page >> 16 );
    SPI_RdWrByte( page >> 8 );
    SPI_RdWrByte( page  );

    CS_HIGH();

    spiflash_wait_busy(); // wait erase done.

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_WREN );//write en
    CS_HIGH();

    for(index=0; index<size/256; index++)
    {
        uint32_t i;

        spiflash_wait_busy();

        CS_LOW();
        SPI_RdWrByte( SPI_FLASH_CMD_WREN );//write en
        CS_HIGH();

        CS_LOW();
        SPI_RdWrByte( SPI_FLASH_CMD_PAGE_PROG );
        SPI_RdWrByte(  page>>16 );
        SPI_RdWrByte(  page>>8 );
        SPI_RdWrByte(  page );
        page += 256;

        for(i=0; i<256; i++)
        {
            SPI_RdWrByte(*buffer++);
        }
        CS_HIGH();

        spiflash_wait_busy();
    }
    CS_HIGH();

    CS_LOW();
    SPI_RdWrByte( SPI_FLASH_CMD_WRDI );
    CS_HIGH();

    spi_unlock();
    return size;
}
