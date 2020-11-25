#ifndef __DMA_USART_H
#define __DMA_USART_H

#include <string.h>
#include "sys.h"
#include "usart.h"
#include "hostcomprotocol.h"


void 	DMA1_UartSendData(u8 *Str,u16 Size,u8 which_uart);

#endif




