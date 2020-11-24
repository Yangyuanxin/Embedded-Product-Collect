/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

/* Modified by xzp21st 2009.5 */

#include "integer.h"
#include "diskio.h"


#define uint8_t   unsigned char
#define uint32_t  unsigned long
#define uint16_t  unsigned int

/*外部调用 WJH*/
#include "datetime.h"
#include "spiflash.h"

extern MEMORY_INFO gSpiFlash;


static DSTATUS Stat = STA_NOINIT;	/* Disk status */


#if 0
DWORD get_fattime(void)
{
    UCHAR y=2009-1980,m=12,d=29,h=11,n=0,s=0;
    DWORD td;
    
    td = (DWORD)s|((DWORD)n<<5)|((DWORD)h<<11)|((DWORD)d<<16)|((DWORD)m<<21)|((DWORD)y<<25);
    
    return td;
}
#else
DWORD get_fattime(void)
{
	DWORD res;
	tm *t = sys_time(0x0);
/*
    GetTime();
    y = gNowTime.tm_year;
    y = y+2000;
*/	
	res =  ((t->year - 1980) << 25)
			| ((DWORD)t->month << 21)
			| ((DWORD)t->mday << 16)
			| ((WORD)t->hour << 11)
			| ((WORD)t->min << 5)
			| ((WORD)t->sec >> 1);

	return res;
}
#endif
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive       
                                             */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0) */
)
{
    
    if (drv) return STA_NOINIT;	/* Supports only single drive */
    
    //wjh
    spiflash_init();
    
    Stat &= ~STA_NOINIT; /* Clear STA_NOINIT */

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber:(0) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	DWORD memory_offset = sector*gSpiFlash.sector_size;
	DWORD read_count = count * gSpiFlash.sector_size;

	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

    if( spiflash_read(memory_offset,buff,read_count)==read_count )
        return RES_OK;
    else
        return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UCHAR count			/* Number of sectors to write  */
)
{
	DWORD memory_offset = sector*gSpiFlash.sector_size;
	DWORD read_count = count * gSpiFlash.sector_size;

	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;


    if( spiflash_page_write(memory_offset,buff,read_count)==read_count )
        return RES_OK;
    else
        return RES_ERROR;
    
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	if (drv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;

	switch (ctrl) {
		case CTRL_SYNC :	/* Flush dirty buffer if present */
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
		    *(DWORD*)buff = gSpiFlash.flash_size/gSpiFlash.sector_size;
			res = RES_OK;
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			*(WORD*)buff = gSpiFlash.sector_size;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
		    *(DWORD*)buff = gSpiFlash.block_size;
		    res = RES_OK;
            break;
		default:
			res = RES_PARERR;
	}

	return res;
}

