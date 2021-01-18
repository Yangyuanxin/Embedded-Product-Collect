/**
  ******************************************************************************
  * @file    stm8s.h
  * @author  MCD Application Team
  * @version V2.3.0
  * @date    16-June-2017
  * @brief   This file contains all HW registers definitions and memory mapping.
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#ifndef __STM8S_H
#define __STM8S_H

/** @addtogroup STM8S_StdPeriph_Driver
  * @{
  */
  
/* Uncomment the line below according to the target STM8S or STM8A device used in your
   application. */

 /* #define STM8S208 */      /*!< STM8S High density devices with CAN */
 /* #define STM8S207 */      /*!< STM8S High density devices without CAN */
 /* #define STM8S007 */      /*!< STM8S Value Line High density devices */
 /* #define STM8AF52Ax */    /*!< STM8A High density devices with CAN */
 /* #define STM8AF62Ax */    /*!< STM8A High density devices without CAN */
 /* #define STM8S105 */      /*!< STM8S Medium density devices */
 /* #define STM8S005 */      /*!< STM8S Value Line Medium density devices */
 /* #define STM8AF626x */    /*!< STM8A Medium density devices */
 /* #define STM8AF622x */    /*!< STM8A Low density devices */
 /* #define STM8S103 */      /*!< STM8S Low density devices */
 /* #define STM8S003 */      /*!< STM8S Value Line Low density devices */
 /* #define STM8S903 */      /*!< STM8S Low density devices */
 /* #define STM8S001 */      /*!< STM8S Value Line Low denisty devices */

/*   Tip: To avoid modifying this file each time you need to switch between these
        devices, you can define the device in your toolchain compiler preprocessor. 

  - High-Density STM8A devices are the STM8AF52xx STM8AF6269/8x/Ax,
    STM8AF51xx, and STM8AF6169/7x/8x/9x/Ax microcontrollers where the Flash memory
    density ranges between 32 to 128 Kbytes
  - Medium-Density STM8A devices are the STM8AF622x/4x, STM8AF6266/68,
    STM8AF612x/4x, and STM8AF6166/68 microcontrollers where the Flash memory 
    density ranges between 8 to 32 Kbytes
  - High-Density STM8S devices are the STM8S207xx, STM8S007 and STM8S208xx microcontrollers
    where the Flash memory density ranges between 32 to 128 Kbytes.
  - Medium-Density STM8S devices are the STM8S105x and STM8S005 microcontrollers
    where the Flash memory density ranges between 16 to 32-Kbytes.
  - Low-Density STM8A devices are the STM8AF622x microcontrollers where the Flash
    density is 8 Kbytes. 
  - Low-Density STM8S devices are the STM8S103xx, STM8S003, STM8S903xx and STM8S001 microcontrollers
    where the Flash density is 8 Kbytes. */

//#if !defined (STM8S208) && !defined (STM8S207) && !defined (STM8S105) && \
//    !defined (STM8S103) && !defined (STM8S903) && !defined (STM8AF52Ax) && \
//    !defined (STM8AF62Ax) && !defined (STM8AF626x) && !defined (STM8S007) && \
//    !defined (STM8S003)&& !defined (STM8S005) && !defined(STM8S001) && !defined (STM8AF622x) 
// #error "Please select first the target STM8S/A device used in your application (in stm8s.h file)"
//#endif

/******************************************************************************/
/*                   Library configuration section                            */
/******************************************************************************/
/* Check the used compiler */
//#if defined(__CSMC__)
// #define _COSMIC_
//#elif defined(__RCST7__)
// #define _RAISONANCE_
//#elif defined(__ICCSTM8__)
// #define _IAR_
//#else
// #error "Unsupported Compiler!"          /* Compiler defines not found */
//#endif

//#if !defined  USE_STDPERIPH_DRIVER
///* Comment the line below if you will not use the peripherals drivers.
//   In this case, these drivers will not be included and the application code will be
//   based on direct access to peripherals registers */
// #define USE_STDPERIPH_DRIVER
//#endif

///**
//  * @brief  In the following line adjust the value of External High Speed oscillator (HSE)
//   used in your application

//   Tip: To avoid modifying this file each time you need to use different HSE, you
//        can define the HSE value in your toolchain compiler preprocessor.
//  */
//#if !defined  HSE_Value
// #if defined (STM8S208) || defined (STM8S207) || defined (STM8S007) || defined (STM8AF52Ax) || \
//     defined (STM8AF62Ax) || defined (STM8AF622x)
//  #define HSE_VALUE ((uint32_t)24000000) /* Value of the External oscillator in Hz*/
// #else
//  #define HSE_VALUE ((uint32_t)16000000) /* Value of the External oscillator in Hz*/
// #endif /* STM8S208 || STM8S207 || STM8S007 || STM8AF62Ax || STM8AF52Ax || STM8AF622x */
//#endif /* HSE_Value */

///**
//  * @brief  Definition of Device on-chip RC oscillator frequencies
//  */
//#define HSI_VALUE   ((uint32_t)16000000) /*!< Typical Value of the HSI in Hz */
//#define LSI_VALUE   ((uint32_t)128000)   /*!< Typical Value of the LSI in Hz */

//#ifdef _COSMIC_
// #define FAR  @far
// #define NEAR @near
// #define TINY @tiny
// #define EEPROM @eeprom
// #define CONST  const
//#elif defined (_RAISONANCE_) /* __RCST7__ */
// #define FAR  far
// #define NEAR data
// #define TINY page0
// #define EEPROM eeprom
// #define CONST  code
// #if defined (STM8S208) || defined (STM8S207) || defined (STM8S007) || defined (STM8AF52Ax) || \
//     defined (STM8AF62Ax)
//   /*!< Used with memory Models for code higher than 64K */
//  #define MEMCPY fmemcpy
// #else /* STM8S903, STM8S103, STM8S001, STM8S003, STM8S105, STM8AF626x, STM8AF622x */
//  /*!< Used with memory Models for code less than 64K */
//  #define MEMCPY memcpy
// #endif /* STM8S208 or STM8S207 or STM8S007 or STM8AF62Ax or STM8AF52Ax */ 
//#else /*_IAR_*/
// #define FAR  __far
// #define NEAR __near
// #define TINY __tiny
// #define EEPROM __eeprom
// #define CONST  const
//#endif /* __CSMC__ */

/* For FLASH routines, select whether pointer will be declared as near (2 bytes,
   to handle code smaller than 64KB) or far (3 bytes, to handle code larger 
   than 64K) */

#if defined (STM8S105) || defined (STM8S005) || defined (STM8S103) || defined (STM8S003) || \
    defined (STM8S001) || defined (STM8S903) || defined (STM8AF626x) || defined (STM8AF622x)
/*!< Used with memory Models for code smaller than 64K */
 #define PointerAttr NEAR
 #define MemoryAddressCast uint16_t
#else /* STM8S208 or STM8S207 or STM8AF62Ax or STM8AF52Ax */
/*!< Used with memory Models for code higher than 64K */
 #define PointerAttr FAR
 #define MemoryAddressCast uint32_t
#endif /* STM8S105 or STM8S103 or STM8S003 or STM8S001 or STM8S903 or STM8AF626x or STM8AF622x */

/*----------------------------------------------------------------------------*/
/**
  * @brief  Clock Controller (CLK)
  */
typedef struct CLK_struct
{
  __IO uint8_t ICKR;     /*!< Internal Clocks Control Register */
  __IO uint8_t ECKR;     /*!< External Clocks Control Register */
  uint8_t RESERVED;      /*!< Reserved byte */
  __IO uint8_t CMSR;     /*!< Clock Master Status Register */
  __IO uint8_t SWR;      /*!< Clock Master Switch Register */
  __IO uint8_t SWCR;     /*!< Switch Control Register */
  __IO uint8_t CKDIVR;   /*!< Clock Divider Register */
  __IO uint8_t PCKENR1;  /*!< Peripheral Clock Gating Register 1 */
  __IO uint8_t CSSR;     /*!< Clock Security System Register */
  __IO uint8_t CCOR;     /*!< Configurable Clock Output Register */
  __IO uint8_t PCKENR2;  /*!< Peripheral Clock Gating Register 2 */
  uint8_t RESERVED1;     /*!< Reserved byte */
  __IO uint8_t HSITRIMR; /*!< HSI Calibration Trimmer Register */
  __IO uint8_t SWIMCCR;  /*!< SWIM clock control register */
}
CLK_TypeDef;

/** @addtogroup CLK_Registers_Reset_Value
  * @{
  */

#define CLK_ICKR_RESET_VALUE     ((uint8_t)0x01)
#define CLK_ECKR_RESET_VALUE     ((uint8_t)0x00)
#define CLK_CMSR_RESET_VALUE     ((uint8_t)0xE1)
#define CLK_SWR_RESET_VALUE      ((uint8_t)0xE1)
#define CLK_SWCR_RESET_VALUE     ((uint8_t)0x00)
#define CLK_CKDIVR_RESET_VALUE   ((uint8_t)0x18)
#define CLK_PCKENR1_RESET_VALUE  ((uint8_t)0xFF)
#define CLK_PCKENR2_RESET_VALUE  ((uint8_t)0xFF)
#define CLK_CSSR_RESET_VALUE     ((uint8_t)0x00)
#define CLK_CCOR_RESET_VALUE     ((uint8_t)0x00)
#define CLK_HSITRIMR_RESET_VALUE ((uint8_t)0x00)
#define CLK_SWIMCCR_RESET_VALUE  ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup CLK_Registers_Bits_Definition
  * @{
  */
#define CLK_ICKR_SWUAH       ((uint8_t)0x20) /*!< Slow Wake-up from Active Halt/Halt modes */
#define CLK_ICKR_LSIRDY      ((uint8_t)0x10) /*!< Low speed internal oscillator ready */
#define CLK_ICKR_LSIEN       ((uint8_t)0x08) /*!< Low speed internal RC oscillator enable */
#define CLK_ICKR_FHWU        ((uint8_t)0x04) /*!< Fast Wake-up from Active Halt/Halt mode */
#define CLK_ICKR_HSIRDY      ((uint8_t)0x02) /*!< High speed internal RC oscillator ready */
#define CLK_ICKR_HSIEN       ((uint8_t)0x01) /*!< High speed internal RC oscillator enable */

#define CLK_ECKR_HSERDY      ((uint8_t)0x02) /*!< High speed external crystal oscillator ready */
#define CLK_ECKR_HSEEN       ((uint8_t)0x01) /*!< High speed external crystal oscillator enable */

#define CLK_CMSR_CKM         ((uint8_t)0xFF) /*!< Clock master status bits */

#define CLK_SWR_SWI          ((uint8_t)0xFF) /*!< Clock master selection bits */

#define CLK_SWCR_SWIF        ((uint8_t)0x08) /*!< Clock switch interrupt flag */
#define CLK_SWCR_SWIEN       ((uint8_t)0x04) /*!< Clock switch interrupt enable */
#define CLK_SWCR_SWEN        ((uint8_t)0x02) /*!< Switch start/stop */
#define CLK_SWCR_SWBSY       ((uint8_t)0x01) /*!< Switch busy flag*/

#define CLK_CKDIVR_HSIDIV    ((uint8_t)0x18) /*!< High speed internal clock prescaler */
#define CLK_CKDIVR_CPUDIV    ((uint8_t)0x07) /*!< CPU clock prescaler */

#define CLK_PCKENR1_TIM1     ((uint8_t)0x80) /*!< Timer 1 clock enable */ 
#define CLK_PCKENR1_TIM3     ((uint8_t)0x40) /*!< Timer 3 clock enable */
#define CLK_PCKENR1_TIM2     ((uint8_t)0x20) /*!< Timer 2 clock enable */
#define CLK_PCKENR1_TIM5     ((uint8_t)0x20) /*!< Timer 5 clock enable */
#define CLK_PCKENR1_TIM4     ((uint8_t)0x10) /*!< Timer 4 clock enable */
#define CLK_PCKENR1_TIM6     ((uint8_t)0x10) /*!< Timer 6 clock enable */
#define CLK_PCKENR1_UART3    ((uint8_t)0x08) /*!< UART3 clock enable */
#define CLK_PCKENR1_UART2    ((uint8_t)0x08) /*!< UART2 clock enable */
#define CLK_PCKENR1_UART1    ((uint8_t)0x04) /*!< UART1 clock enable */
#define CLK_PCKENR1_SPI      ((uint8_t)0x02) /*!< SPI clock enable */
#define CLK_PCKENR1_I2C      ((uint8_t)0x01) /*!< I2C clock enable */

#define CLK_PCKENR2_CAN      ((uint8_t)0x80) /*!< CAN clock enable */
#define CLK_PCKENR2_ADC      ((uint8_t)0x08) /*!< ADC clock enable */
#define CLK_PCKENR2_AWU      ((uint8_t)0x04) /*!< AWU clock enable */

#define CLK_CSSR_CSSD        ((uint8_t)0x08) /*!< Clock security system detection */
#define CLK_CSSR_CSSDIE      ((uint8_t)0x04) /*!< Clock security system detection interrupt enable */
#define CLK_CSSR_AUX         ((uint8_t)0x02) /*!< Auxiliary oscillator connected to master clock */
#define CLK_CSSR_CSSEN       ((uint8_t)0x01) /*!< Clock security system enable */

#define CLK_CCOR_CCOBSY      ((uint8_t)0x40) /*!< Configurable clock output busy */
#define CLK_CCOR_CCORDY      ((uint8_t)0x20) /*!< Configurable clock output ready */
#define CLK_CCOR_CCOSEL      ((uint8_t)0x1E) /*!< Configurable clock output selection */
#define CLK_CCOR_CCOEN       ((uint8_t)0x01) /*!< Configurable clock output enable */

#define CLK_HSITRIMR_HSITRIM ((uint8_t)0x07) /*!< High speed internal oscillator trimmer */

#define CLK_SWIMCCR_SWIMDIV  ((uint8_t)0x01) /*!< SWIM Clock Dividing Factor */

/*----------------------------------------------------------------------------*/
/**
  * @brief  FLASH program and Data memory (FLASH)
  */

typedef struct
{
  __IO uint8_t CR1;       /*!< Flash control register 1 */
  __IO uint8_t CR2;       /*!< Flash control register 2 */
  __IO uint8_t NCR2;      /*!< Flash complementary control register 2 */
  __IO uint8_t FPR;       /*!< Flash protection register */
  __IO uint8_t NFPR;      /*!< Flash complementary protection register */
  __IO uint8_t IAPSR;     /*!< Flash in-application programming status register */
  uint8_t RESERVED1;      /*!< Reserved byte */
  uint8_t RESERVED2;      /*!< Reserved byte */
  __IO uint8_t PUKR;      /*!< Flash program memory unprotection register */
  uint8_t RESERVED3;      /*!< Reserved byte */
  __IO uint8_t DUKR;      /*!< Data EEPROM unprotection register */
}
STM8S_FLASH_TypeDef;

typedef struct
{
  __IO uint8_t CR1;        /*!< Flash control register 1 */
  __IO uint8_t CR2;        /*!< Flash control register 2 */
  __IO uint8_t PUKR;       /*!< Flash program memory unprotection register */
  __IO uint8_t DUKR;       /*!< Data EEPROM unprotection register */
  __IO uint8_t IAPSR;      /*!< Flash in-application programming status register */
}
STM8L_FLASH_TypeDef;


/** @addtogroup FLASH_Registers_Reset_Value
  * @{
  */

#define FLASH_CR1_RESET_VALUE   ((uint8_t)0x00)
#define FLASH_CR2_RESET_VALUE   ((uint8_t)0x00)
#define FLASH_NCR2_RESET_VALUE  ((uint8_t)0xFF)
#define FLASH_IAPSR_RESET_VALUE ((uint8_t)0x40)
#define FLASH_PUKR_RESET_VALUE  ((uint8_t)0x00)
#define FLASH_DUKR_RESET_VALUE  ((uint8_t)0x00)

/**
  * @}
  */

/**
  * @}
  */

/*----------------------------------------------------------------------------*/
/**
  * @brief  Option Bytes (OPT)
  */
typedef struct OPT_struct
{
  __IO uint8_t OPT0;  /*!< Option byte 0: Read-out protection (not accessible in IAP mode) */
  __IO uint8_t OPT1;  /*!< Option byte 1: User boot code */
  __IO uint8_t NOPT1; /*!< Complementary Option byte 1 */
  __IO uint8_t OPT2;  /*!< Option byte 2: Alternate function remapping */
  __IO uint8_t NOPT2; /*!< Complementary Option byte 2 */
  __IO uint8_t OPT3;  /*!< Option byte 3: Watchdog option */
  __IO uint8_t NOPT3; /*!< Complementary Option byte 3 */
  __IO uint8_t OPT4;  /*!< Option byte 4: Clock option */
  __IO uint8_t NOPT4; /*!< Complementary Option byte 4 */
  __IO uint8_t OPT5;  /*!< Option byte 5: HSE clock startup */
  __IO uint8_t NOPT5; /*!< Complementary Option byte 5 */
  uint8_t RESERVED1;  /*!< Reserved Option byte*/
  uint8_t RESERVED2; /*!< Reserved Option byte*/
  __IO uint8_t OPT7;  /*!< Option byte 7: flash wait states */
  __IO uint8_t NOPT7; /*!< Complementary Option byte 7 */
}
OPT_TypeDef;

/*----------------------------------------------------------------------------*/
/**
  * @brief  Reset Controller (RST)
  */

typedef struct RST_struct
{
  __IO uint8_t SR; /*!< Reset status register */
}
RST_TypeDef;

/** @addtogroup RST_Registers_Bits_Definition
  * @{
  */

//#define RST_SR_EMCF   ((uint8_t)0x10) /*!< EMC reset flag bit mask */
#define RST_SR_SWIMF  ((uint8_t)0x08) /*!< SWIM reset flag bit mask */
//#define RST_SR_ILLOPF ((uint8_t)0x04) /*!< Illegal opcode reset flag bit mask */
//#define RST_SR_IWDGF  ((uint8_t)0x02) /*!< IWDG reset flag bit mask */
//#define RST_SR_WWDGF  ((uint8_t)0x01) /*!< WWDG reset flag bit mask */

/**
  * @}
  */

/**
  * @brief  Configuration Registers (CFG)
  */

typedef struct CFG_struct
{
  __IO uint8_t GCR; /*!< Global Configuration register */
}
CFG_TypeDef;

/** @addtogroup CFG_Registers_Reset_Value
  * @{
  */

#define CFG_GCR_RESET_VALUE ((uint8_t)0x00)

/**
  * @}
  */

/** @addtogroup CFG_Registers_Bits_Definition
  * @{
  */

#define CFG_GCR_SWD ((uint8_t)0x01) /*!< Swim disable bit mask */
#define CFG_GCR_AL  ((uint8_t)0x02) /*!< Activation Level bit mask */

/**
  * @}
  */

/******************************************************************************/
/*                          Peripherals Base Address                          */
/******************************************************************************/

/** @addtogroup MAP_FILE_Base_Addresses
  * @{
  */
#define OPT_BaseAddress         0x4800

#define FLASH_BaseAddress       0x505A
#define EXTI_BaseAddress        0x50A0
#define RST_BaseAddress         0x50B3
#define CLK_BaseAddress         0x50C0
#define WWDG_BaseAddress        0x50D1
#define IWDG_BaseAddress        0x50E0
#define AWU_BaseAddress         0x50F0
#define BEEP_BaseAddress        0x50F3
#define SPI_BaseAddress         0x5200
#define I2C_BaseAddress         0x5210
#define UART1_BaseAddress       0x5230
#define UART2_BaseAddress       0x5240
#define UART3_BaseAddress       0x5240
#define UART4_BaseAddress       0x5230
#define TIM1_BaseAddress        0x5250
#define TIM2_BaseAddress        0x5300
#define TIM3_BaseAddress        0x5320
#define TIM4_BaseAddress        0x5340
#define TIM5_BaseAddress        0x5300
#define TIM6_BaseAddress        0x5340
#define ADC1_BaseAddress        0x53E0
#define ADC2_BaseAddress        0x5400
#define CAN_BaseAddress         0x5420
#define CFG_BaseAddress         0x7F60
#define ITC_BaseAddress         0x7F70
#define DM_BaseAddress          0x7F90

//#define CLK ((CLK_TypeDef *) CLK_BaseAddress)

#define STM8S_FLASH ((STM8S_FLASH_TypeDef *) 0x505A)
#define STM8L_FLASH ((STM8L_FLASH_TypeDef *) 0x5050)

#define STM8S_FLASH_CR1         0x505A      /*!< Flash control register 1 */
#define STM8S_FLASH_CR2         0x505B      /*!< Flash control register 2 */
#define STM8S_FLASH_NCR2        0x505C      /*!< Flash complementary control register 2 */
#define STM8S_FLASH_FPR         0x505D      /*!< Flash protection register */
#define STM8S_FLASH_NFPR        0x505E      /*!< Flash complementary protection register */
#define STM8S_FLASH_IAPSR       0x505F      /*!< Flash in-application programming status register */
#define STM8S_FLASH_RESERVED1   0x5060      /*!< Reserved byte */
#define STM8S_FLASH_RESERVED2   0x5061      /*!< Reserved byte */
#define STM8S_FLASH_PUKR        0x5062      /*!< Flash program memory unprotection register */
#define STM8S_FLASH_RESERVED3   0x5063      /*!< Reserved byte */
#define STM8S_FLASH_DUKR        0x5064      /*!< Data EEPROM unprotection register */
  
#define STM8L_FLASH_CR1         0x5050      /*!< Flash control register 1 */
#define STM8L_FLASH_CR2         0x5051      /*!< Flash control register 2 */
#define STM8L_FLASH_PUKR        0x5052      /*!< Flash program memory unprotection register */
#define STM8L_FLASH_DUKR        0x5053      /*!< Data EEPROM unprotection register */
#define STM8L_FLASH_IAPSR       0x5054      /*!< Flash in-application programming status register */
 
 
#define STM8_CLK_CKDIVR         0x50C6

#define OPT ((OPT_TypeDef *) OPT_BaseAddress)

#define RST ((RST_TypeDef *) RST_BaseAddress)

typedef enum {
    FLASH_MEMTYPE_PROG      = (uint8_t)0xFD, /*!< Program memory */
    FLASH_MEMTYPE_DATA      = (uint8_t)0xF7  /*!< Data EEPROM memory */
} FLASH_MemType_TypeDef;


/** @addtogroup FLASH_Registers_Bits_Definition
  * @{
  */

    #define STM8_FLASH_CR1_HALT        ((uint8_t)0x08) /*!< Standby in Halt mode mask */
    #define STM8_FLASH_CR1_AHALT       ((uint8_t)0x04) /*!< Standby in Active Halt mode mask */
#define STM8_FLASH_CR1_IE          ((uint8_t)0x02) /*!< Flash Interrupt enable mask */
#define STM8_FLASH_CR1_FIX         ((uint8_t)0x01) /*!< Fix programming time mask */
#define STM8_FLASH_PROGRAMTIME_TPROG     0x01

#define STM8_FLASH_CR2_OPT         ((uint8_t)0x80) /*!< Select option byte mask */
#define STM8_FLASH_CR2_WPRG        ((uint8_t)0x40) /*!< Word Programming mask */
#define STM8_FLASH_CR2_ERASE       ((uint8_t)0x20) /*!< Erase block mask */
#define STM8_FLASH_CR2_FPRG        ((uint8_t)0x10) /*!< Fast programming mode mask */
#define STM8_FLASH_CR2_PRG         ((uint8_t)0x01) /*!< Program block mask */

    #define STM8_FLASH_NCR2_NOPT       ((uint8_t)0x80) /*!< Select option byte mask */
    #define STM8_FLASH_NCR2_NWPRG      ((uint8_t)0x40) /*!< Word Programming mask */
    #define STM8_FLASH_NCR2_NERASE     ((uint8_t)0x20) /*!< Erase block mask */
    #define STM8_FLASH_NCR2_NFPRG      ((uint8_t)0x10) /*!< Fast programming mode mask */
    #define STM8_FLASH_NCR2_NPRG       ((uint8_t)0x01) /*!< Program block mask */

#define STM8_FLASH_IAPSR_HVOFF     ((uint8_t)0x40) /*!< End of high voltage flag mask */
#define STM8_FLASH_IAPSR_DUL       ((uint8_t)0x08) /*!< Data EEPROM unlocked flag mask */
#define STM8_FLASH_IAPSR_EOP       ((uint8_t)0x04) /*!< End of operation flag mask */
#define STM8_FLASH_IAPSR_PUL       ((uint8_t)0x02) /*!< Flash Program memory unlocked flag mask */
#define STM8_FLASH_IAPSR_WR_PG_DIS ((uint8_t)0x01) /*!< Write attempted to protected page mask */

#define STM8_FLASH_PUKR_PUK        ((uint8_t)0xFF) /*!< Flash Program memory unprotection mask */

#define STM8_FLASH_DUKR_DUK        ((uint8_t)0xFF) /*!< Data EEPROM unprotection mask */


#endif /* __STM8S_H */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
