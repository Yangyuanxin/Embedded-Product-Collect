/**
  ******************************************************************************
  * @file    USB_Device/MSC_Standalone/Src/usbd_storage.c
  * @author  MCD Application Team
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include "bsp.h"
#include "usbd_storage.h"
//#include "bsp_sdio_sd.h"
#include "bsp_emmc.h"

//#define printf_ok            printf
#define printf_ok(...)

#define printf_err            printf
//#define printf_err(...)

#define STORAGE_LUN_NBR                  1        // 3
//#define STORAGE_BLK_NBR                  0x10000
//#define STORAGE_BLK_SIZ                  0x200

#define LUN_SDRAM    0
#define LUN_SD        0
//#define LUN_NAND    2


/* 定义SDRAM 虚拟磁盘的地址和空间。 4M字节 */
#define SDRAM_DISK_ADDR        0x30000000
#define SDRAM_DISK_SIZE        (128 * 1024)

/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
/* USB Mass storage Standard Inquiry Data */
int8_t STORAGE_Inquirydata[] = {  /* 36 */
    /* LUN 0 */
    0x00,
    0x80,
    0x02,
    0x02,
    (STANDARD_INQUIRY_DATA_LEN - 5),
    0x00,
    0x00,
    0x00,
    'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer: 8 bytes */
    'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product : 16 Bytes */
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    '0', '.', '0', '1',           /* Version : 4 Bytes */
};

/* Private function prototypes ----------------------------------------------- */
int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t * block_num,
                           uint16_t * block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                    uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                     uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

USBD_StorageTypeDef USBD_DISK_fops = {
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata,
};


HAL_MMC_CardInfoTypeDef g_emmcInfo;

/* Private functions --------------------------------------------------------- */

/**
  * @brief  Initializes the storage unit (medium)       
  * @param  lun: Logical unit number
  * @retval Status (0 : OK / -1 : Error)
  */
int8_t STORAGE_Init(uint8_t lun)
{
    int8_t ret = -1;
    
	printf_ok("STORAGE_Init\r\n");
    switch (lun)
    {
        case LUN_SD:    
            BSP_MMC_Init();

            BSP_MMC_GetCardInfo(&g_emmcInfo);
            ret = 0;
            break;
    }
    return ret;    
}

/**
  * @brief  Returns the medium capacity.      
  * @param  lun: Logical unit number
  * @param  block_num: Number of total block number
  * @param  block_size: Block size
  * @retval Status (0: OK / -1: Error)
  */
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t * block_num,
                           uint16_t * block_size)
{
    int8_t ret = -1;
    
    switch (lun)
    {
        case LUN_SD:    
            {
                *block_num = g_emmcInfo.LogBlockNbr - 1;
                *block_size = g_emmcInfo.LogBlockSize;
                ret = 0;
				
				printf_ok("STORAGE_GetCapacity ^%d, %d\r\n", *block_num, *block_size);
            }
            break;
    }    
    return ret; 
}

/**
  * @brief  Checks whether the medium is ready.  
  * @param  lun: Logical unit number
  * @retval Status (0: OK / -1: Error)
  */
int8_t STORAGE_IsReady(uint8_t lun)
{
    int8_t ret = -1;
	
    switch (lun)
    {
        case LUN_SD:    
            {
                if (BSP_MMC_GetCardState() == MMC_TRANSFER_OK)
                {
                    ret = 0;
                }
            }
            break;
    }
    return ret;
}

/**
  * @brief  Checks whether the medium is write protected.
  * @param  lun: Logical unit number
  * @retval Status (0: write enabled / -1: otherwise)
  */
int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
	printf_ok("STORAGE_IsWriteProtected\r\n");
	return 0;
}

/**
  * @brief  Reads data from the medium.
  * @param  lun: Logical unit number
  * @param  blk_addr: Logical block address
  * @param  blk_len: Blocks number
  * @retval Status (0: OK / -1: Error)
  */
int8_t STORAGE_Read(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                    uint16_t blk_len)
{
    int8_t ret = -1;
	
	printf_ok("STORAGE_Read %d, %d\r\n", blk_addr, blk_len);
	
    switch (lun)
    {
        case LUN_SD:    
            {
//                uint8_t re;
                                                                
                BSP_MMC_ReadBlocks((uint32_t *) buf, blk_addr, blk_len, 1000);									
                while(BSP_MMC_GetCardState() != MMC_TRANSFER_OK);
//                    /* Wait until SD card is ready to use for new operation */
//                    while(BSP_MMC_GetCardState() != MMC_TRANSFER_OK);                   
                
//                if (re == MMC_OK)
//                {
//                    printf_ok("  ok %02X %02X %02X %02X\r\n", buf[0],buf[1],buf[2],buf[3]);
//                }
//                else
//                {
//                    printf_ok("  err %02X %02X %02X %02X\r\n",  buf[0],buf[1],buf[2],buf[3]);
//                }
                
                ret = 0;
            }
            break;
            
//        case LUN_SDRAM:
//            {
//                uint32_t i;
//                uint32_t *p_sdram;
//                uint32_t *p_buf;
//                
//                if (blk_len * 512 > SDRAM_DISK_SIZE)
//                {
//                    break;    /* 异常 */
//                }
//                
//                p_buf = (uint32_t *)buf;
//                p_sdram = (uint32_t *)(SDRAM_DISK_ADDR + blk_addr * 512);
//                for (i = 0; i < blk_len * 512 / 4; i++)
//                {
//                    *p_buf++ = *p_sdram++;
//                }
//            }
//            ret = 0;
//            break;
    }
    return ret;
}

/**
  * @brief  Writes data into the medium.
  * @param  lun: Logical unit number
  * @param  blk_addr: Logical block address
  * @param  blk_len: Blocks number
  * @retval Status (0 : OK / -1 : Error)
  */
int8_t STORAGE_Write(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                     uint16_t blk_len)
{
    int8_t ret = -1;
	
	printf_ok("STORAGE_Write %d, %d\r\n", blk_addr, blk_len);
    switch (lun)
    {
        case LUN_SD:    
            {
                BSP_MMC_WriteBlocks((uint32_t *)buf, blk_addr, blk_len, 5000);
                while(BSP_MMC_GetCardState() != MMC_TRANSFER_OK);		

                ret = 0;
            }
            break;
            
//        case LUN_SDRAM:
//            {
//                uint32_t i;
//                uint32_t *p_sdram;
//                uint32_t *p_buf;
//                
//                if (blk_len * 512 > SDRAM_DISK_SIZE)
//                {
//                    break;    /* 异常 */
//                }
//                
//                printf_ok("sdram_Write ok: BlockNo=%d, Count=%d", blk_addr, blk_len);                
//                
//                p_buf = (uint32_t *)buf;
//                p_sdram = (uint32_t *)(SDRAM_DISK_ADDR + blk_addr * 512);
//                for (i = 0; i < blk_len * 512 / 4; i++)
//                {
//                    *p_sdram++ = *p_buf++;
//                }
//            }
//            ret = 0;
//            break;
    }
    return ret;
}

/**
  * @brief  Returns the Max Supported LUNs.   
  * @param  None
  * @retval Lun(s) number
  */
int8_t STORAGE_GetMaxLun(void)
{
  return (STORAGE_LUN_NBR - 1);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
