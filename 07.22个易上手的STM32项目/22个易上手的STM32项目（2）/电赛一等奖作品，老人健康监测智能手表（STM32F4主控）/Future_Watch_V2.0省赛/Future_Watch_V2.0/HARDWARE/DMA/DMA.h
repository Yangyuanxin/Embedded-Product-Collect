#ifndef _DMA_H_
#define _DMA_H_

#include "sys.h"
#include "delay.h"

u8 MyAdc_DMAConfig(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr);//DMA配置
u8 MyAdc_DMAEnable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);	//使能一次DMA传输		

#endif
