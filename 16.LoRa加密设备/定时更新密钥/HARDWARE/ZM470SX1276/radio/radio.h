#ifndef __RADIO_H__
#define __RADIO_H__

#include "sys.h"
#include "sx1276.h"
#include "platform.h"

#define			LORA			1		//选择LoRa模式

//RF process function return codes
typedef enum
{
    RF_IDLE,							//空闲状态
    RF_BUSY,							//忙			
    RF_RX_DONE,							//接收完成
    RF_RX_TIMEOUT,						//接收超时
    RF_TX_DONE,							//发送完成
    RF_TX_TIMEOUT,						//发送超时
    RF_LEN_ERROR,						//LEN错误
    RF_CHANNEL_EMPTY,					//信道空
    RF_CHANNEL_ACTIVITY_DETECTED,		//发现活跃信道
}tRFProcessReturnCodes;


//定义radio函数指针结构体
typedef struct sRadioDriver
{
    void 	( *Init )( void );
    void 	( *Reset )( void );
    void 	( *StartRx )( void );
    void 	( *GetRxPacket )( void *buffer, u16 *size );
    void 	( *SetTxPacket )( const void *buffer, u16 size );
   	u32 	( *Process )( void );
	/*************************增加的函数**************************/
	inline u8 		( *RFDio0State )	(void);
	int8_t  ( *RFGetPacketSnr)	( void );
	double 	( *RFGetPacketRssi)	( void );
	double 	( *RFReadRssi)		( void );
	void 	( *RFRxStateEnter )	(void);
    void 	( *RFRxDataRead )	(u8 *pbuf, u8 *size );
    u8 		( *RFTxData )		(u8 *pbuf, u8 size );
	void 	( *RFTxPower)		(u8  pwr   );
	void 	( *RFFreqSet)		(u32 freq  );
	void 	( *RFOpModeSet)		(u8  opMode);
	/**************************************************************/
}tRadioDriver;


//函数结构体初始化
tRadioDriver *RadioDriverInit( void );

#endif
