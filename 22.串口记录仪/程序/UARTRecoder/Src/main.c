
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2020 STMicroelectronics International N.V. 
  * All rights reserved.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f2xx_hal.h"
#include "dma.h"
#include "fatfs.h"
#include "rtc.h"
#include "sdio.h"
#include "usart.h"
#include "usb_device.h"
#include "usb_host.h"
#include "gpio.h"
#include "string.h"
/* USER CODE BEGIN Includes */

#include "uart_rx_dma.h"
#include "uhead.h"
#include "usbd_cdc_if.h"


/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

DMA_rxStr dmarx1Str = {0};
DMA_rxStr dmarx4Str = {0};
DMA_rxStr dmarx6Str = {0};
char timeArray[] = "2020/05/14 20:17:56	"; 
char filename[] = "1:/20200514.txt";
RTC_DateTypeDef sDate = {0};
RTC_TimeTypeDef sTime = {0};

extern	FATFS SDFatFS;    /* File system object for SD logical drive */
extern	FIL SDFile;       /* File object for SD */

extern	FATFS USBHFatFS;    /* File system object for USBH logical drive */
extern	FIL USBHFile;       /* File object for USBH */
char flag_sd = 0;			// SD卡插入标志
char flag_usb = 0;			// USB插入标志

UINT bw = 0;			// 写入的指针/* Pointer to number of bytes written */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

void MadeFileName(char* file,RTC_DateTypeDef* sDate)
{
	file[5] = (sDate->Year/10) + '0';
	file[6] = (sDate->Year%10) + '0';
	file[7] = (sDate->Month/10) + '0';
	file[8] = (sDate->Month%10) + '0';
	file[9] = (sDate->Date/10) + '0';
	file[10] = (sDate->Date%10) + '0';

}
// USB连接函数
void USB_Connect(void)
{
	f_mount (&USBHFatFS,USBHPath,1);
	filename[0] = USBHPath[0];
	MadeFileName(filename,&sDate);
	debug("filename:%s\r\nUSBHPath:%s\r\n",filename,USBHPath);
	f_open (&USBHFile,filename,FA_OPEN_ALWAYS|FA_WRITE);
	f_lseek(&USBHFile,f_size(&USBHFile));                           //移动到文件尾部	
	flag_usb = 1;
}

void USB_Disconnect(void)
{
	flag_usb = 0;
	f_close(&USBHFile);
}

// 将时间转换成字符串
void PutTime2Array(RTC_DateTypeDef* sDate,RTC_TimeTypeDef* sTime,char* timearray)
{
	timearray[2] = (sDate->Year/10) + '0';
	timearray[3] = (sDate->Year%10) + '0';
	timearray[5] = (sDate->Month/10) + '0';
	timearray[6] = (sDate->Month%10) + '0';
	timearray[8] = (sDate->Date/10) + '0';
	timearray[9] = (sDate->Date%10) + '0';
	timearray[11] = (sTime->Hours/10) + '0';
	timearray[12] = (sTime->Hours%10) + '0';
	timearray[14] = (sTime->Minutes/10) + '0';
	timearray[15] = (sTime->Minutes%10) + '0';	
	timearray[17] = (sTime->Seconds/10) + '0';
	timearray[18] = (sTime->Seconds%10) + '0';
	
	
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_UART4_Init();
  MX_USART6_UART_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_USB_DEVICE_Init();
  MX_RTC_Init();
  MX_USB_HOST_Init();
  /* USER CODE BEGIN 2 */
  debug("start:\r\n");
	HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN); 
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);	
	PutTime2Array(&sDate,&sTime,timeArray);
  UartRX_Start(&huart1, &hdma_usart1_rx,&dmarx1Str);	//配置并启动DMA
  UartRX_Start(&huart4, &hdma_uart4_rx,&dmarx4Str);	//配置并启动DMA
  UartRX_Start(&huart6, &hdma_usart6_rx,&dmarx6Str);	//配置并启动DMA
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


	while (1)
	{
		if(GPIO_READ(SD_EXTI))
		{
			if(flag_sd)
			{
				
				flag_sd = 0;
				debug("SD卡拔出\r\n");
				f_close(&SDFile);
			}
		}else
		{
			if(flag_sd == 0)
			{
				flag_sd = 1;
				debug("SD卡插入\r\n");
			AA :
				HAL_SD_Init(&hsd);// 硬盘初始化函数SD卡初始化函数
				HAL_SD_CardCIDTypeDef pCID = {0};
				HAL_SD_CardStateTypeDef erro = HAL_SD_GetCardState(&hsd) ;
				if(erro == HAL_SD_CARD_TRANSFER)
				{
					HAL_SD_GetCardCID(&hsd,&pCID);
					debug("SD 初始化成功：");
					debug("BlockSize = %d\r\n",hsd.SdCard.BlockSize);
					f_mount (&SDFatFS,SDPath,1);
					filename[0] = SDPath[0];
					MadeFileName(filename,&sDate);
					f_open (&SDFile,filename,FA_OPEN_ALWAYS|FA_WRITE);
					f_lseek(&SDFile,f_size(&SDFile));                           //移动到文件尾部	
					flag_sd = 1;		
				}else 
				{
					debug("SD卡初始化失败,erro = %d\r\n",erro);
					goto AA;
				}
				
			}
		}
	  if(dmarx1Str.num)
	  {
		  
		dmarx1Str.buf[dmarx1Str.num] = 0;
		debug("dmarx1Str.num = %d,time : %s RX: %s\r\n",dmarx1Str.num,timeArray,dmarx1Str.buf);	
		if(flag_sd)
		{	
			f_printf(&SDFile,"%s",timeArray);
			f_printf(&SDFile,": %s\r\n",dmarx1Str.buf);
			f_sync (&SDFile);		
		}			
		if(flag_usb)
		{	
			f_printf(&USBHFile,"%s",timeArray);
			f_printf(&USBHFile,": %s\r\n",dmarx1Str.buf);
			f_sync (&USBHFile);		
		}	
		
		dmarx1Str.num = 0;  
	  }

	  if(dmarx4Str.num)
	  {
		  
		dmarx4Str.buf[dmarx4Str.num] = 0;
		debug("dmarx4Str.num = %d,time : %s RX: %s\r\n",dmarx4Str.num,timeArray,dmarx4Str.buf);	

		if(flag_sd)
		{	
			f_printf(&SDFile,"%s",timeArray);
			f_printf(&SDFile,": %s\r\n",dmarx4Str.buf);
			f_sync (&SDFile);		
		}			
		if(flag_usb)
		{	
			f_printf(&USBHFile,"%s",timeArray);
			f_printf(&USBHFile,": %s\r\n",dmarx4Str.buf);
			f_sync (&USBHFile);		
		}	
		
		dmarx4Str.num = 0;  
	  }
	  if(dmarx6Str.num)
	  {
		  
		dmarx6Str.buf[dmarx6Str.num] = 0;
		debug("dmarx6Str.num = %d,time : %s RX: %s\r\n",dmarx6Str.num,timeArray,dmarx6Str.buf);	

		if(flag_sd)
		{	
			f_printf(&SDFile,"%s",timeArray);
			f_printf(&SDFile,": %s\r\n",dmarx6Str.buf);
			f_sync (&SDFile);		
		}			
		if(flag_usb)
		{	
			f_printf(&USBHFile,"%s",timeArray);
			f_printf(&USBHFile,": %s\r\n",dmarx6Str.buf);
			f_sync (&USBHFile);		
		}	
		
		dmarx6Str.num = 0;  
	  }	  
		HAL_RTC_GetTime(&hrtc,&sTime, RTC_FORMAT_BIN); 
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);	
		PutTime2Array(&sDate,&sTime,timeArray);

  /* USER CODE END WHILE */
    MX_USB_HOST_Process();

  /* USER CODE BEGIN 3 */

	}
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 20;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
