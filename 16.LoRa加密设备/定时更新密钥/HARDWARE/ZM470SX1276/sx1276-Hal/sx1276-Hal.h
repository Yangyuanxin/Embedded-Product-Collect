#ifndef __SX1276_HAL_H__
#define __SX1276_HAL_H__

#include "sys.h"

//SX1276-SPI复位引脚设置
#define 	RST_IOPORT					GPIOB
#define 	RST_PIN						GPIO_Pin_1	//GPIO_Pin_1

//SX1276-SPI片选引脚设置
#define 	SEL_IOPORT					GPIOA
#define 	SEL_PIN						GPIO_Pin_4

//SX1276 DIOx引脚定义
#define 	DIO0_IOPORT					GPIOB
#define 	DIO0_PIN					GPIO_Pin_0	//GPIO_Pin_0

//片选引脚
#define 	SEL 						PAout(4)
#define		SEL_L()						SEL = 0
#define		SEL_H()						SEL = 1

//复位引脚
#define 	RST 						PBout(1)
#define		RST_L()						RST = 0
#define		RST_H()						RST = 1

//DIO state read functions mapping
#define 	DIO0						SX1276ReadDio0()
#define 	DIO1						SX1276ReadDio1()
#define 	DIO2						SX1276ReadDio2()
#define 	DIO3						SX1276ReadDio3()
#define 	DIO4						SX1276ReadDio4()
#define 	DIO5						SX1276ReadDio5()

//RXTX pin control see errata note
#define 	TickCounter 				10
#define 	GET_TICK_COUNT()			(TickCounter)
#define 	TICK_RATE_MS(ms)			(ms)

typedef enum
{
    RADIO_RESET_OFF,
    RADIO_RESET_ON,
}tRadioResetState;


void SX1276InitIo(void);
void SX1276SetReset(u8 state);
void SX1276Write(u8 addr, u8 data);
void SX1276Read(u8 addr, u8 *data);
void SX1276WriteBuffer(u8 addr, u8 *buffer, u8 size);
void SX1276ReadBuffer(u8 addr, u8 *buffer, u8 size);
void SX1276WriteFifo(u8 *buffer, u8 size);
void SX1276ReadFifo(u8 *buffer, u8 size);

//I/O口状态读取("0"|"1")
u8 SX1276ReadDio0(void);
u8 SX1276ReadDio1(void);
u8 SX1276ReadDio2(void);
u8 SX1276ReadDio3(void);
u8 SX1276ReadDio4(void);
u8 SX1276ReadDio5(void);

#endif











