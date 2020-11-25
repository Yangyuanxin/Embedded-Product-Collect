#ifndef __HSC32I1_H
#define __HSC32I1_H

#include "delay.h"
#include "hsc32_rst_io.h"
#include "iic.h"  


void 	HSC32I1BaseInit(void);
u8 		HSC32I1_ReadLenByte(u8 ReadAddr,u8 *Buf,u16 Size);
void 	HSC32I1_WriteLenByte(u8 WriteAddr,u8 *Buf,u16 Size);

#endif
















