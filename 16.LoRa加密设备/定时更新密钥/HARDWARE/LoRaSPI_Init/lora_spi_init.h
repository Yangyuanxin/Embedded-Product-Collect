#ifndef __LORA_SPI_INIT_H__
#define __LORA_SPI_INIT_H__

#include "sys.h"

#define 	SPI_INTERFACE				SPI1
#define 	SPI_CLK						RCC_APB2Periph_SPI1

#define 	SPI_PIN_SCK_PORT			GPIOA
#define 	SPI_PIN_SCK_PORT_CLK		RCC_APB2Periph_GPIOA
#define 	SPI_PIN_SCK					GPIO_Pin_5

#define 	SPI_PIN_MISO_PORT			GPIOA
#define 	SPI_PIN_MISO_PORT_CLK		RCC_APB2Periph_GPIOA
#define 	SPI_PIN_MISO				GPIO_Pin_6

#define 	SPI_PIN_MOSI_PORT			GPIOA
#define 	SPI_PIN_MOSI_PORT_CLK		RCC_APB2Periph_GPIOA
#define 	SPI_PIN_MOSI				GPIO_Pin_7


//¹¦ÄÜº¯Êý
void 	LoRaSPI_Init(void);
u8 		SPIx_InOut(u8 outData);

#endif 




