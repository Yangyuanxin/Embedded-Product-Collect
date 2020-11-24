/**
  ******************************************************************************
  * @file    platform_config.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Evaluation board specific configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

#define HW_V3     1

#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)
 #include "stm32l1xx.h"
// #if defined (USE_STM32L152_EVAL)
//  #include "stm32l152_eval.h"
//  #include "stm32l152_eval_spi_sd.h"
// 
//#elif defined (USE_STM32L152D_EVAL)
//  #include "stm32l152d_eval_sdio_sd.h"
//  #include "stm32l152d_eval.h"
//
//#else
//  #error "Missing define: USE_STM32L152_EVAL or USE_STM32L152D_EVAL"
// #endif /* USE_STM32L152_EVAL */
#elif defined (STM32F10X_MD) || defined (STM32F10X_HD) || defined (STM32F10X_XL)
 #include "stm32f10x.h"
// #if defined (USE_STM3210B_EVAL)
//  #include "stm3210b_eval.h"
//  #include "stm3210b_eval_spi_sd.h"
// #elif defined (USE_STM3210E_EVAL)
//  #include "stm3210e_eval_sdio_sd.h"
//  #include "stm3210e_eval.h"
//  #include "fsmc_nand.h"
//  #include "nand_if.h"
// #else
//  #error "Missing define: USE_STM3210B_EVAL or USE_STM3210E_EVAL"
// #endif /* USE_STM3210B_EVAL */
#elif defined (USE_STM32373C_EVAL)
 #include "stm32f37x.h"
 #include "stm32373c_eval.h"
 #include "stm32373c_eval_spi_sd.h"

#elif defined (USE_STM32303C_EVAL)
 #include "stm32f30x.h"
 #include "stm32303c_eval.h"
 #include "stm32303c_eval_spi_sd.h"
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL)  &&  !defined (USE_STM32L152_EVAL) &&  !defined (USE_STM32L152D_EVAL) && !defined (USE_STM32373C_EVAL) && !defined (USE_STM32303C_EVAL)
//#define USE_STM3210B_EVAL
//#define USE_STM3210E_EVAL
//#define USE_STM32L152_EVAL
//#define USE_STM32L152D_EVAL
//#define (USE_STM32373C_EVAL)
//#define (USE_STM32303C_EVAL)
#endif

/*Unique Devices IDs register set*/

#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD) || defined(STM32L1XX_MD_PLUS) 

#define         ID1          (0x1FF80050)
#define         ID2          (0x1FF80054)
#define         ID3          (0x1FF80064)

#elif defined (STM32F37X) || defined(STM32F30X)

#define         ID1          (0x1FFFF7AC)
#define         ID2          (0x1FFFF7B0)
#define         ID3          (0x1FFFF7B4)

#else /*STM32F1x*/

#define         ID1          (0x1FFFF7E8)
#define         ID2          (0x1FFFF7EC)
#define         ID3          (0x1FFFF7F0)

#endif

#define RCC_AHBPeriph_ALLGPIO                 (RCC_AHBPeriph_GPIOA \
                                              | RCC_AHBPeriph_GPIOB \
                                              | RCC_AHBPeriph_GPIOC \
                                              | RCC_AHBPeriph_GPIOD \
                                              | RCC_AHBPeriph_GPIOE \
                                              | RCC_AHBPeriph_GPIOF )    
//*-------------------------------------------------------------------------------------*/     
  #define PORTA_NULL                          GPIOA
  #define PORTA_NULL_PIN                      GPIO_Pin_2|GPIO_Pin_3
  #define RCC_APB2Periph_GPIO_PORTA_NULL      RCC_APB2Periph_GPIOA
  
  #define PORTB_NULL                          GPIOB
  #define PORTB_NULL_PIN                      GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_10
  #define RCC_APB2Periph_GPIO_PORTB_NULL      RCC_APB2Periph_GPIOB

/* Test pin */
  #define TEST_CLK                          GPIOB
  #define TEST_CLK_PIN                      GPIO_Pin_10
  #define RCC_APB2Periph_GPIO_TEST_CLK      RCC_APB2Periph_GPIOB

  #define TEST_CLK_HIGH()                   GPIO_SetBits(TEST_CLK, TEST_CLK_PIN);
  #define TEST_CLK_LOW()                    GPIO_ResetBits(TEST_CLK, TEST_CLK_PIN);
  
/* WAKE UP KEY */
  #define WKUP_KEY                          GPIOA
  #define WKUP_KEY_PIN                      GPIO_Pin_0
  #define RCC_APB2Periph_GPIO_WKUP_KEY      RCC_APB2Periph_GPIOA
  

/* SPI FLASH CS */
  #define SPI_CS_FL                         GPIOA
  #define SPI_CS_FL_PIN                     GPIO_Pin_4
  #define RCC_APB2Periph_GPIO_SPI_CS_FL     RCC_APB2Periph_GPIOA
  
/* SPI LCM CS */
  #define SPI_CS_LCM                        GPIOA
  #define SPI_CS_LCM_PIN                    GPIO_Pin_8
  #define RCC_APB2Periph_GPIO_SPI_CS_LCM    RCC_APB2Periph_GPIOA

/* GPS VCC CTRL */
  #define PWR_GPS_CTRL                      GPIOB
  #define PWR_GPS_CTRL_PIN                  GPIO_Pin_8
  #define RCC_APB2Periph_GPIO_PWR_GPS_CTRL  RCC_APB2Periph_GPIOB

/* USB_DISCONNECT */
  #define USB_DISCONNECT                    GPIOB
  #define USB_DISCONNECT_PIN                GPIO_Pin_2
  #define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOB
/*
  #define USB_DISCONNECT                    GPIOA
  #define USB_DISCONNECT_PIN                GPIO_Pin_8
  #define RCC_APB2Periph_GPIO_DISCONNECT    RCC_APB2Periph_GPIOA
*/

/* 指南针HMC5883L中断输出, GYPRO_INT */
  #define GYPRO_INT                         GPIOB
  #define GYPRO_INT_PIN                     GPIO_Pin_4
  #define RCC_APB2Periph_GPIO_GYPRO_INT     RCC_APB2Periph_GPIOB

/* 陀螺仪BMA250中端出, XYZ_INT */
  #define XYZ_INT                           GPIOB
  #define XYZ_INT_PIN                       GPIO_Pin_5
  #define RCC_APB2Periph_GPIO_XYZ_INT       RCC_APB2Periph_GPIOB

/* 陀螺仪BMA250中端出, XYZ_INT2 */
  #define XYZ_INT2                          GPIOB
  #define XYZ_INT2_PIN                      GPIO_Pin_11
  #define RCC_APB2Periph_GPIO_XYZ_INT       RCC_APB2Periph_GPIOB


/* 充电状态 */
  #define CHRG_ST                          GPIOB
  #define CHRG_ST_PIN                      GPIO_Pin_9
  #define RCC_APB2Periph_GPIO_CHRG_ST      RCC_APB2Periph_GPIOB
  #define GetChargeState()                 GPIO_ReadInputDataBit(CHRG_ST,CHRG_ST_PIN)

/* DEVICE POWER CTRL */
  #define PWR_DEV_CTRL                      GPIOB
  #define PWR_DEV_CTRL_PIN                  GPIO_Pin_12
  #define RCC_APB2Periph_GPIO_PWR_DEV_CTRL  RCC_APB2Periph_GPIOB
  
/* LCM POWER CTRL */
  #define PWR_LCM_CTRL                      GPIOB
  #define PWR_LCM_CTRL_PIN                  GPIO_Pin_13
  #define RCC_APB2Periph_GPIO_PWR_LCM_CTRL  RCC_APB2Periph_GPIOB

/* LCM RST */
  #define LCM_RST                           GPIOB
  #define LCM_RST_PIN                       GPIO_Pin_15
  #define RCC_APB2Periph_GPIO_LCM_RST       RCC_APB2Periph_GPIOB

/* LCM DC */
  #define LCM_DC                            GPIOB
  #define LCM_DC_PIN                        GPIO_Pin_14
  #define RCC_APB2Periph_GPIO_LCM_DC        RCC_APB2Periph_GPIOB

/* RTC CLK */
  #define RTC_CLK                           GPIOC
  #define RTC_CLK_PIN                       GPIO_Pin_13
  #define RCC_APB2Periph_GPIO_RTC_CLK       RCC_APB2Periph_GPIOC

/* RTC RST */
  #define RTC_RST                           GPIOC
  #define RTC_RST_PIN                       GPIO_Pin_14
  #define RCC_APB2Periph_GPIO_RTC_RST       RCC_APB2Periph_GPIOC

/* RTC DAT */
  #define RTC_DAT                           GPIOC
  #define RTC_DAT_PIN                       GPIO_Pin_15
  #define RCC_APB2Periph_GPIO_RTC_DAT       RCC_APB2Periph_GPIOC

/* 温度传感器通讯接口TMP_IO */
  #define TEMP_IO                           GPIOB
  #define TEMP_IO_PIN                       GPIO_Pin_0
  #define RCC_APB2Periph_GPIO_TEMP_IO       RCC_APB2Periph_GPIOB

/* I2C */
  #define I2C_BMP280        I2C1
  #define I2C_DEV_BMP280	  0xEC
  #define I2C_DEV_HMC5883   0x3C
  #define I2C_DEV_BMA250    0x30      //0x18<<1 SDO pulled to GND; 0x19<<1 SDO pin to 'VDDIO'.

  #define I2C_BUS                           GPIOB
  #define I2C_SDA_PIN                       GPIO_Pin_7
  #define I2C_SCK_PIN                       GPIO_Pin_6
  #define I2C_BUS_PIN                       I2C_SDA_PIN|I2C_SCK_PIN
  #define RCC_APB2Periph_GPIO_I2C_BUS       RCC_APB2Periph_GPIOB

/* WKUP KEY */
  #define WKUP_KEY                          GPIOA
  #define WKUP_KEY_PIN                      GPIO_Pin_0
  #define RCC_APB2Periph_GPIO_WKUP_KEY      RCC_APB2Periph_GPIOA

/* LED */
#if HW_V3
  #define WORK_LED                          GPIOB
  #define WORK_LED_PIN                      GPIO_Pin_0
  #define RCC_APB2Periph_GPIO_WORK_LED      RCC_APB2Periph_GPIOB
#else
  #define WORK_LED                          GPIOB
  #define WORK_LED_PIN                      GPIO_Pin_8
  #define RCC_APB2Periph_GPIO_WORK_LED      RCC_APB2Periph_GPIOB
#endif
/* Functions define */
  #define LCM_PWR_ON()    GPIO_SetBits(PWR_LCM_CTRL, PWR_LCM_CTRL_PIN)
  #define LCM_PWR_OFF()   GPIO_ResetBits(PWR_LCM_CTRL, PWR_LCM_CTRL_PIN)

  #define DEV_PWR_ON()    GPIO_SetBits(PWR_DEV_CTRL, PWR_DEV_CTRL_PIN)
  #define DEV_PWR_OFF()   GPIO_ResetBits(PWR_DEV_CTRL, PWR_DEV_CTRL_PIN)

  #define GPS_PwrOn()    GPIO_SetBits(PWR_GPS_CTRL, PWR_GPS_CTRL_PIN)
  #define GPS_PwrOff()   GPIO_ResetBits(PWR_GPS_CTRL, PWR_GPS_CTRL_PIN)

  #define LED_ON()    GPIO_SetBits(WORK_LED, WORK_LED_PIN)
  #define LED_OFF()   GPIO_ResetBits(WORK_LED, WORK_LED_PIN)

  
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
