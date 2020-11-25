#ifndef CONFIG_STORE_H
#define CONFIG_STORE_H

#include "Configuration.h"
#include "flash.h" 


#define FLASH_WRITE_VAR(address,value) SPI_Flash_Write((uint8_t*)&value,address,sizeof(value))//_EEPROM_writeData(&address, (uint8_t*)&value, sizeof(value))
#define FLASH_READ_VAR(address, value) SPI_Flash_Read((uint8_t*)&value,address,sizeof(value))//_EEPROM_readData(&address, (uint8_t*)&value, sizeof(value))
	
//void _EEPROM_writeData(u32* address, uint8_t* value, uint8_t size);
//void _EEPROM_readData(u32* address, uint8_t* value, uint8_t size);

void Config_ResetDefault(void);

//#ifdef EEPROM_CHITCHAT
void Config_PrintSettings(void);
//#else
//#endif
//#ifdef EEPROM_SETTINGS
void Config_StoreSettings(void);
void Config_RetrieveSettings(void);
//#else

//#endif

#endif//CONFIG_STORE_H
