/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Inc/usbd_cdc_interface.h
  * @author  MCD Application Team
  * @brief   Header for usbd_cdc_interface.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc.h"

/* 
	H7-TOOL 虚拟串口（RS232， RS485，TTL-UART）使用的PA9 PA10 
	PA9/USART1_TX/PE13/FMC_D10
	PA10/USART1_RX/PE14/FMC_D11

	临时调试ESP32时，可以将其映射到UART4口，通过PC串口助手测试AT指令。
	PH13/UART4_TX 
	PH14/UART4_RX
*/

/* 定义物理串口 A口，B口切换 */
#if 1	/* H7-TOOL 缺省使用这个UART */
	#define A_USARTx                           USART1
	#define A_USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()

	#define A_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()

	#define A_USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
	#define A_USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

	#define A_USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
	#define A_USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

	/* Definition for USARTx Pins */
	#define A_USARTx_TX_PIN                    GPIO_PIN_9
	#define A_USARTx_TX_GPIO_PORT              GPIOA
	#define A_USARTx_TX_AF                     GPIO_AF7_USART1

	#define A_USARTx_RX_PIN                    GPIO_PIN_10
	#define A_USARTx_RX_GPIO_PORT              GPIOA
	#define A_USARTx_RX_AF                     GPIO_AF7_USART1


	/* Definition for USARTx's NVIC: used for receiving data over Rx pin */
	#define A_USARTx_IRQn                      USART1_IRQn
	#define A_USARTx_IRQHandler                USART1_IRQHandler

	#define A_USARTx_TX_DMA_CHANNEL             DMA_REQUEST_USART1_TX
	#define A_USARTx_RX_DMA_CHANNEL             DMA_REQUEST_USART1_RX


	/* Definition for USARTx's DMA */
	#define A_USARTx_TX_DMA_STREAM              DMA1_Stream7
	#define A_USARTx_RX_DMA_STREAM              DMA1_Stream5

	/* Definition for USARTx's NVIC */
	#define A_USARTx_DMA_TX_IRQn                DMA1_Stream7_IRQn
	#define A_USARTx_DMA_RX_IRQn                DMA1_Stream5_IRQn
	#define A_USARTx_DMA_TX_IRQHandler          DMA1_Stream7_IRQHandler
	#define A_USARTx_DMA_RX_IRQHandler          DMA1_Stream5_IRQHandler
#endif

#if 1	/* H7-TOOL 升级ESP32模块固件时使用这个UART */
	#define B_USARTx                           UART4
	#define B_USARTx_CLK_ENABLE()              __HAL_RCC_UART4_CLK_ENABLE()

	#define B_DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()

	#define B_USARTx_FORCE_RESET()             __HAL_RCC_UART4_FORCE_RESET()
	#define B_USARTx_RELEASE_RESET()           __HAL_RCC_UART4_RELEASE_RESET()

	#define B_USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
	#define B_USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()

	/* Definition for USARTx Pins */
	#define B_USARTx_TX_PIN                    GPIO_PIN_13
	#define B_USARTx_TX_GPIO_PORT              GPIOH
	#define B_USARTx_TX_AF                     GPIO_AF8_UART4

	#define B_USARTx_RX_PIN                    GPIO_PIN_14
	#define B_USARTx_RX_GPIO_PORT              GPIOH
	#define B_USARTx_RX_AF                     GPIO_AF8_UART4

	/* Definition for USARTx's NVIC: used for receiving data over Rx pin */
	#define B_USARTx_IRQn                      UART4_IRQn
	#define B_USARTx_IRQHandler                UART4_IRQHandler

	#define B_USARTx_TX_DMA_CHANNEL             DMA_REQUEST_UART4_TX
	#define B_USARTx_RX_DMA_CHANNEL             DMA_REQUEST_UART4_RX


	/* Definition for USARTx's DMA */
	#define B_USARTx_TX_DMA_STREAM              DMA1_Stream7
	#define B_USARTx_RX_DMA_STREAM              DMA1_Stream5

	/* Definition for USARTx's NVIC */
	#define B_USARTx_DMA_TX_IRQn                DMA1_Stream7_IRQn
	#define B_USARTx_DMA_RX_IRQn                DMA1_Stream5_IRQn
	#define B_USARTx_DMA_TX_IRQHandler          DMA1_Stream7_IRQHandler
	#define B_USARTx_DMA_RX_IRQHandler          DMA1_Stream5_IRQHandler
#endif


/* Definition for TIMx clock resources   . TIM3 用于DSO示波器ADC触发源 */
#define TIMx                             TIM15	
#define TIMx_CLK_ENABLE                  __HAL_RCC_TIM15_CLK_ENABLE
#define TIMx_FORCE_RESET()               __HAL_RCC_TIM15_FORCE_RESET()
#define TIMx_RELEASE_RESET()             __HAL_RCC_TIM15_RELEASE_RESET()

/* Definition for TIMx's NVIC */
#define TIMx_IRQn                        TIM15_IRQn
#define TIMx_IRQHandler                  TIM15_IRQHandler

/* Periodically, the state of the buffer "UserTxBuffer" is checked.
   The period depends on CDC_POLLING_INTERVAL */
#define CDC_POLLING_INTERVAL             5 /* in ms. The max is 65ms and the min is 1ms */

extern USBD_CDC_ItfTypeDef  USBD_CDC_fops;

extern uint8_t USBCom_SendBuf(int _Port, uint8_t *_Buf, uint16_t _Len);
extern uint8_t USBCom_SendBufNow(int _Port, uint8_t *_Buf, uint16_t _Len);
	
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#endif /* __USBD_CDC_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
