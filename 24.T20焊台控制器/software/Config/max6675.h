#ifndef __MAX6675_H
#define __MAX6675_H
#include "stm32f10x.h"
#include "sys.h"

#define MAX6675_CS	 	PAout(4)
#define MAX6675Error	0x00
#define MAX6675Left		0x02

void SPI_MAX6675_Init(void);
u8 MAX6675_ReadByte(void);
float getMax6675_Data(void);

#endif
