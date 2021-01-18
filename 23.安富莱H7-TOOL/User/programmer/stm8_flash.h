/*
*********************************************************************************************************
*
*    模块名称 : stm8系列flash访问程序
*    文件名称 : stm8_flash.h
*
*********************************************************************************************************
*/

#ifndef _STM8_FLASH_H
#define _STM8_FLASH_H

#define STM8S   0
#define STM8L   1

#define STM8_FLASH_RASS_KEY1 ((uint8_t)0x56) /*!< First RASS key */
#define STM8_FLASH_RASS_KEY2 ((uint8_t)0xAE) /*!< Second RASS key */

#define STM8_FLASH_PROG_START_ADDRESS ((uint32_t)0x008000

#define STM8_FLASH_DATA_START_ADDRESS ((uint32_t)0x004000

typedef enum {
//#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S105) || 
//    defined(STM8S005) || defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined(STM8AF626x)
    STM8_FLASH_FLAG_HVOFF     = (uint8_t)0x40,     /*!< End of high voltage flag */
//#endif /* STM8S208, STM8S207, STM8S105, STM8AF62Ax, STM8AF52Ax, STM8AF626x */
    STM8_FLASH_FLAG_DUL       = (uint8_t)0x08,     /*!< Data EEPROM unlocked flag */
    STM8_STM8_FLASH_FLAG_EOP       = (uint8_t)0x04,     /*!< End of programming (write or erase operation) flag */
    FLASH_FLAG_PUL       = (uint8_t)0x02,     /*!< Flash Program memory unlocked flag */
    STM8_FLASH_FLAG_WR_PG_DIS = (uint8_t)0x01      /*!< Write attempted to protected page flag */
} FLASH_Flag_TypeDef;


extern uint16_t s_STM8_SerialType;
extern uint16_t s_STM8_BlockSize; 
extern uint16_t s_STM8_HVOFF;
extern uint32_t s_STM8_FlashSize;      /* FLASH 容量 */
extern uint32_t s_STM8_EEPromSize;     /* EEPROM 容量 */

void SWIM_InitHard(void);
void STM8_FLASH_Unlock(void);
void STM8_FLASH_Lock(uint32_t FlashAddr);
uint8_t STM8_FLASH_ProgramBuf(uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size);
uint8_t STM8_ProgramOptionBytes(uint8_t *_AddrBuf, uint16_t _AddrLen, uint8_t *_DataBuf, uint16_t _DataLen);
uint8_t STM8_FLASH_ReadBuf(uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size);
uint8_t STM8_FLASH_EraseChip(uint32_t _FlashAddr);
uint8_t STM8_FLASH_EraseSector(uint32_t _FlashAddr);

void STM8_WriteReg_CR1(uint8_t _value);
void STM8_WriteReg_CR2(uint8_t _value);

#endif
