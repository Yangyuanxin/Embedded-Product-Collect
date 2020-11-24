/***********************************************************************/
/*                         SZQVC.Lighthouse                            */
/*                           www.szqvc.com                             */
/***********************************************************************/

#ifndef __SYS_HW_H
#define __SYS_HW_H

//#define I2C_DEV_BMP280 0x77



//
#define USE_UART1     0
#define USE_UART2     1
#define USE_UART3     0

//
typedef struct{
  uint16_t  State;      //0未初始化,1初始化
  uint32_t  IdelTickMask; //
  uint8_t   *RxBuffer;
  uint8_t   *TxBuffer;
  uint16_t  RxBfSize;
  uint16_t  TxBfSize;
  uint16_t  RxCounter;
  uint16_t  TxCounter;
}UART_INFO;
//
typedef enum _SPI_TYPE
{
    CFG_SPI_LCM,
    CFG_SPI_FLASH,
    CFG_SPI_COMM,
}SPI_TYPE;
//

//
void myPWR_EnterSleepMode(void);
void myPWR_EnterPowrOff(void);
void SoftReset(void);
void USB_Ctrl(char onoff);
//
void WriteEncrypt(void);
char JudgeEncrypt(void);
//
void GPIO_Config(void);
//
void EXTI_Configuration(void);
void NVIC_Configuration(void);
//
void I2C_Config(void);
uint8_t I2C_Read(uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);
uint8_t I2C_Write(uint8_t I2C_Addr,uint8_t addr,uint8_t value);
//
void UART1_Configuration(void);
//
void UART2_Configuration(void);
//
void SPI_Config(uint8_t type);
uint8_t SPI_RdWrByte(uint8_t dt);
void SPI_WriteByte(uint8_t dt);
//
int gps_comm_read(char *p, int len);
int gps_comm_get_len(void);
void gps_comm_clr(void);

void gps_comm_disable(void);
void gps_comm_enable(void);
//
#endif

/*********************** (C) COPYRIGHT SZQVC **************************/
/*                          END OF FILE                               */
/**********************************************************************/

