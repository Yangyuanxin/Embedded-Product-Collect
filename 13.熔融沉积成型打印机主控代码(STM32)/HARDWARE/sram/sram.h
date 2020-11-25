#ifndef __SRAM_H
#define __SRAM_H															    
#include "sys.h" 

											  
void FSMC_SRAM_Init(void);
void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 NumHalfwordToWrite);
void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 NumHalfwordToRead);


void fsmc_sram_test_write(u8 data,u32 addr);
u8 fsmc_sram_test_read(u32 addr);

#endif

