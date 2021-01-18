/*
*********************************************************************************************************
*
*	模块名称 : USB device 中断服务程序
*	文件名称 : usb_it.c
*	版    本 : V1.0
*	说    明 : 本文件存放USB device中断服务程序。只需将该文件加入工程即可，无需再到 stm32f4xx_it.c 中添加这些ISR程序
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-09-08 armfly  正式发布
*
*	Copyright (C), 201118-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
/* Includes ------------------------------------------------------------------ */
#include "usb_if.h"

/** @addtogroup STM32H7xx_HAL_Applications
  * @{
  */

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
/* Private macro ------------------------------------------------------------- */
/* Private variables --------------------------------------------------------- */
extern PCD_HandleTypeDef hpcd;

/* UART handler declared in "usbd_cdc_interface.c" file */
extern UART_HandleTypeDef *NowUartHandle;

/* TIM handler declared in "usbd_cdc_interface.c" file */
extern TIM_HandleTypeDef TimHandle;
/* Private function prototypes ----------------------------------------------- */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void);
#else
void OTG_HS_IRQHandler(void);
#endif
void USARTx_DMA_TX_IRQHandler(void);
void USARTx_IRQHandler(void);
void TIMx_IRQHandler(void);

/**
  * @brief  This function handles USB-On-The-Go FS/HS global interrupt request.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  HAL_PCD_IRQHandler(&hpcd);
}

///******************************************************************************/
///* STM32H7xx Peripherals Interrupt Handlers */
///* Add here the Interrupt Handler for the used peripheral(s) (PPP), for the */
///* available peripheral interrupt handler's name please refer to the startup */
///* file (startup_stm32h7xx.s).  */
///******************************************************************************/
///**
//  * @brief  This function handles DMA interrupt request.
//  * @param  None
//  * @retval None
//  */
//void A_USARTx_DMA_TX_IRQHandler(void)
//{
//  HAL_DMA_IRQHandler(NowUartHandle->hdmatx);
//}

//void B_USARTx_DMA_TX_IRQHandler(void)
//{
//  HAL_DMA_IRQHandler(NowUartHandle->hdmatx);
//}


///**
//  * @brief  This function handles UART interrupt request.  
//  * @param  None
//  * @retval None
//  */
//void A_USARTx_IRQHandler(void)
//{
//  HAL_UART_IRQHandler(NowUartHandle);
//}

//void B_USARTx_IRQHandler(void)
//{
//  HAL_UART_IRQHandler(NowUartHandle);
//}

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIMx_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
