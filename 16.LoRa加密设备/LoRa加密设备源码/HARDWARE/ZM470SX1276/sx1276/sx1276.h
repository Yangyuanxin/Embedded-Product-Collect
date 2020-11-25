#ifndef __SX1276_H__
#define __SX1276_H__


#include "sys.h"
#include "delay.h"
#include <stdbool.h>

//定义检测SPI值
#define 	CHECK_ERR		1
#define		CHECK_OK		0

//声明SX1276寄存器组数组
extern uint8_t SX1276Regs[0x70];

u8 		SX1276CheckSPI(void);
void 	SX1276SetLoRaOn(void);
bool 	SX1276GetLoRaOn(void);
void 	SX1276Init(void);
void 	SX1276Reset(void);
void 	SX1276SetOpMode(uint8_t opMode);
u8 		SX1276GetOpMode(void);
u8 		SX1276ReadRxGain(void);
double 	SX1276ReadRssi(void);
u8 		SX1276GetPacketRxGain(void);
int8_t 	SX1276GetPacketSnr(void);
double 	SX1276GetPacketRssi(void);
u32 	SX1276GetPacketAfc(void);
void 	SX1276StartRx(void);
void 	SX1276GetRxPacket(void *buffer, uint16_t *size);
void 	SX1276SetTxPacket(const void *buffer, uint16_t size);
u8 		SX1276GetRFState(void);
void 	SX1276SetRFState(uint8_t state);
u32 	SX1276Process(void);

//增加的函数
/*************************************************************/
//u8 		SX1276Dio0State(void);
void 	SX1276RxStateEnter( void );
void 	SX1276RxDataRead(uint8_t *pbuf, uint8_t *size );
u8 		SX1276TxData(uint8_t *pbuf, uint8_t size );
void 	SX1276TxPower(uint8_t pwr );
void 	SX1276FreqSet(uint32_t freq);
u8 		SX1276CheckSPI(void);
u8 		SX1276TxDataCRC32(uint8_t *pbuf, uint8_t size );
u8 		SX1276RxDataReadCRC32(uint8_t *pbuf, uint8_t *size );
/**************************************************************/
#endif 






