#include "DMA.h"

u8 MyAdc_DMAConfig(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr)
{
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;//DMA2时钟使能 
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<21;//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;						//清空之前该stream上的所有中断标志
	
	DMA_Streamx->PAR=par;		//DMA外设地址
	DMA_Streamx->M0AR=mar;		//DMA 存储器0地址
	DMA_Streamx->NDTR=ndtr;		//DMA 存储器0地址
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	DMA_Streamx->CR|=0<<6;		//外设到存储器模式(0)  存储器到外设模式(1)
	DMA_Streamx->CR|=1<<8;		//非循环模式(0)  	  循环模式(1)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式(0)	  外设非增量模式(1)
	DMA_Streamx->CR|=0<<10;		//存储器非增量模式(0)  存储器非增量模式(1)
	DMA_Streamx->CR|=1<<11;		//外设数据长度:16位(1) 8位为(0)
	DMA_Streamx->CR|=1<<13;		//存储器数据长度:16位(1) 8位为(0)
	DMA_Streamx->CR|=1<<16;		//中等优先级	
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	DMA_Streamx->CR|=(u32)chx<<25;//通道选择
	
	DMA_Streamx->CR|=1<<0;		//开启DMA传输
	return 0;
}


//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量
//
u8 MyAdc_DMAEnable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0); 	//关闭DMA传输 
	while(DMA_Streamx->CR&0X1);	//确保DMA可以被设置  
	DMA_Streamx->NDTR=ndtr;		//DMA 存储器0地址 
	DMA_Streamx->CR|=1<<0;		//开启DMA传输
	return 0;
//	
}	 
