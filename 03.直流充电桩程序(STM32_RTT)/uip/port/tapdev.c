#include "tapdev.h"
#include "uip.h"
#include "clock-arch.h"
#include "stm32f10x.h"
#include "stm32_eth.h"


#define ETH_RXBUFNB 2  //接收DMA描述符个数
#define ETH_TXBUFNB 2  //发送DMA描述符个数

//接收和发送描述符表
ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];
ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];
//接收和发送数据缓存区定义
u8 Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE];
u8 Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];

																				  
//STM32以太网模块硬件初始化
int tapdev_init(void)
{
	ETH_InitTypeDef ETH_InitStructure;
	
	//开PLL3   倍频到50Mhz
	RCC->CFGR2 &= ~0x0000F000;
	RCC->CFGR2 |= 0x00008000;
	RCC->CR |= 0x10000000;
	while ((RCC->CR & 0x20000000) == 0);

	//PA8 MCO  输出50Mhz
	GPIOA->CRH &= 0xFFFFFFF0;
	GPIOA->CRH |= 0x0000000B;
	RCC->CFGR &= 0xF0FFFFFF;
	RCC->CFGR |= 0x0B000000;
	

   	//打开GPIOA,B,C,D,AFIO 时钟
    RCC->APB2ENR |= 0x0000003D;

    //以太网MAC(bit14),以太网发送(bit15),以太网接收(bit16)时钟
    RCC->AHBENR |= 0x0001C000;
	
	//bit23： 使能RMII模式
    //bit21: 重映射RX_DV-PD8, RXD0-PC4, RXD1-PD10
    AFIO->MAPR |= 0x00A00000;

    //PA1 浮空输入模式                        REF_CLK
    //PA2 复用推挽输出 50MHz  MDIO
    GPIOA->CRL   &= 0xFFFFF00F;
    GPIOA->CRL   |= 0x00000B40;

    //PB11,PB12,PB13 复用推挽输出 50MHz
    //PB11 TXEN
    //PB12 TXD0
    //PB13 TXD1
    GPIOB->CRH   &= 0xFF000FFF;
    GPIOB->CRH   |= 0x00BBB000;

    //复用推挽输出 50MHz   MDC
    GPIOC->CRL   &= 0xFFFFFF0F;
    GPIOC->CRL   |= 0x000000B0;

    //PD8,PD9,PD10 浮空输入模式
    //PD8   RX_DV
    //PD9   RXD0
    //PD10  RXD1
    GPIOD->CRH   &= 0xFFFFF000;
    GPIOD->CRH   |= 0x00000444;
		
		
	RCC->CFGR2 &= ~0x0000F000;
	RCC->CFGR2 |= 0x00008000;
	RCC->CR |= 0x10000000;
	while ((RCC->CR & 0x20000000) == 0);

	//PA8 MCO  ??50Mhz
	GPIOA->CRH &= 0xFFFFFFF0;
	GPIOA->CRH |= 0x0000000B;
	RCC->CFGR &= 0xF0FFFFFF;
	RCC->CFGR |= 0x0B000000;

	//RCC_MCOConfig(RCC_MCO_HSE);
	

    //以太网模式寄存器复位
    ETH_DeInit();
  
  	//以太网复位
    ETH_SoftwareReset();
  
 	//等待复位完成
    while(ETH_GetSoftwareResetStatus() == SET);


    ETH_StructInit(&ETH_InitStructure);
  
    //
#if 0
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable  ;
    //ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    //ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#else
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable  ;
    ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
    ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#endif

    ETH_Init(&ETH_InitStructure, 0x01);
  
    //初始化以太网模块DMA接收和发送描述符表
    ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
    ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

    //使能MAC 和  DMA
    ETH_Start();

	return 0;	
}
//读取一包数据  
uint16_t tapdev_read(void)
{	
	uint16_t lenth = ETH_HandleRxPkt(uip_buf);

	return lenth;
}
//发送一包数据  
void tapdev_send(void)
{
	ETH_HandleTxPkt(uip_buf,uip_len);
}


/**
  * @brief  检查网线状态
  * @param
  * @retval >0 连接正常   0 断开
  */
int tapdev_link_up(void)
{
	return ETH_ReadPHYRegister(1, PHY_BSR) & 0x04;
}









/*---------------------------------------------------------------------------*/
