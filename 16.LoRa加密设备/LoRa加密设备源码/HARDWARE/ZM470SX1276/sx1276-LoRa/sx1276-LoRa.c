#include <string.h>
#include "platform.h"
#include "usart.h"

#if defined( USE_SX1276_RADIO )

#include "radio.h"
#include "sx1276.h"
#include "sx1276-Hal.h"
#include "sx1276-LoRa.h"
#include "sx1276-LoRaMisc.h"

//Constant values need to compute the RSSI value
#define 	RSSI_OFFSET_LF								-155.0
#define 	RSSI_OFFSET_HF								-150.0

#define 	NOISE_ABSOLUTE_ZERO							-174.0

#define		NOISE_FIGURE_LF								4.0
#define 	NOISE_FIGURE_HF								6.0 

/*!
 * Precomputed signal bandwidth log values
 * Used to compute the Packet RSSI value.
 */
const double SignalBwLog[] =
{
    3.8927900303521316335038277369285,  // 7.8 kHz
    4.0177301567005500940384239336392,  // 10.4 kHz
    4.193820026016112828717566631653,   // 15.6 kHz
    4.31875866931372901183597627752391, // 20.8 kHz
    4.4948500216800940239313055263775,  // 31.2 kHz
    4.6197891057238405255051280399961,  // 41.6 kHz
    4.795880017344075219145044421102,   // 62.5 kHz
    5.0969100130080564143587833158265,  // 125 kHz
    5.397940008672037609572522210551,   // 250 kHz
    5.6989700043360188047862611052755   // 500 kHz
};

const double RssiOffsetLF[] =
{   
	// These values need to be specify in the Lab
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
};

const double RssiOffsetHF[] =
{   
	// These values need to be specify in the Lab
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
};

//Frequency hopping frequencies table
const int32_t HoppingFrequencies[] =
{
    916500000,
    923500000,
    906500000,
    917500000,
    917500000,
    909000000,
    903000000,
    916000000,
    912500000,
    926000000,
    925000000,
    909500000,
    913000000,
    918500000,
    918500000,
    902500000,
    911500000,
    926500000,
    902500000,
    922000000,
    924000000,
    903500000,
    913000000,
    922000000,
    926000000,
    910000000,
    920000000,
    922500000,
    911000000,
    922000000,
    909500000,
    926000000,
    922000000,
    918000000,
    925500000,
    908000000,
    917500000,
    926500000,
    908500000,
    916000000,
    905500000,
    916000000,
    903000000,
    905000000,
    915000000,
    913000000,
    907000000,
    910000000,
    926500000,
    925500000,
    911000000,
};

//LoRa调制参数设置
tLoRaSettings LoRaSettings =
{
	470000000,		// 发射频率 RFFrequency	
	20,				// 发射功率 Power		
	8,		//250k	// 信号带宽	SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
					// 			5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
	7,		//128	// 扩频因数 [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
	1,		//4/5	// 纠错编码率	ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
	true,			// CRC校验	CrcOn [0: OFF, 1: ON]  CRC
	false,			// 报头模式	ImplicitHeaderOn [0: OFF, 1: ON]
	0,				// 接收模式	RxSingleOn [0: Continuous(持续接收), 1 Single(单一接收)]
	0,				// FreqHopOn [0: OFF, 1: ON]
	4,				// HopPeriod Hops every frequency hopping period symbols
	100,			// TxPacketTimeout
	100,			// RxPacketTimeout
	128,			// 负载长度PayloadLength (用于隐式报头模式)
};

//定义结构体指针，指向LoRa寄存器组
tSX1276LR	*SX1276LR;

//Local接收缓冲区
static u8 RFBuffer[RF_BUFFER_SIZE];


//RF state machine variable
static u8 RFLRState = RFLR_STATE_IDLE;

//Rx management support variables
static u8 		RxGain = 1;
static int8_t 	RxPacketSnrEstimate;
static u16 		RxPacketSize = 0;
static u32 		RxTimeoutTimer = 0;
static double 	RxPacketRssiValue;

//PacketTimeout Stores the Rx window time value for packet reception
static u32 PacketTimeout;

//Tx management support variables
static u16 TxPacketSize = 0;


/***************************************************
	*功能描述：	初始化LoRa调制解调器
	*入口参数：	无
	*返回值：	无
****************************************************/
void SX1276LoRaInit( void )
{
    SX1276LoRaSetDefaults();										//读取芯片版本
   
    SX1276ReadBuffer( REG_LR_OPMODE, SX1276Regs+1, 0x70-1 );		//读取寄存器的值
	
    SX1276LR->RegLna = RFLR_LNA_GAIN_G1;							//LAN设置为最大增益
    SX1276WriteBuffer( REG_LR_OPMODE, SX1276Regs+1, 0x70-1 );		//写入数组值到寄存器中

    //set the RF settings 
    SX1276LoRaSetRFFrequency( LoRaSettings.RFFrequency );			//设置载波频率为470 000 000
    SX1276LoRaSetSpreadingFactor( LoRaSettings.SpreadingFactor ); 	//设置扩频因数为7
    SX1276LoRaSetErrorCoding( LoRaSettings.ErrorCoding );			//设置纠错编码率 001->4/5
    SX1276LoRaSetPacketCrcOn( LoRaSettings.CrcOn );					//开启CRC 1->CRC开启
    SX1276LoRaSetSignalBandwidth( LoRaSettings.SignalBw );			//设置带宽为500kHz 1001->500kHz

    SX1276LoRaSetImplicitHeaderOn( LoRaSettings.ImplicitHeaderOn );	//设置为显示报头模式 0->显示报头模式
    SX1276LoRaSetSymbTimeout( 0x3FF );								//设置RX超时最高有效位和最低有效位 (0x3ff)
    SX1276LoRaSetPayloadLength( LoRaSettings.PayloadLength );		//设置负载字节长度为128
    SX1276LoRaSetLowDatarateOptimize( 1 );		//false				//开启低数据率优化 true

#if( ( MODULE_SX1276RF1IAS == 1 ) || ( MODULE_SX1276RF1KAS == 1 ) )
    if( LoRaSettings.RFFrequency > 860000000 )
    {
        SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_RFO );
        SX1276LoRaSetPa20dBm( false );
        LoRaSettings.Power = 14;
        SX1276LoRaSetRFPower( LoRaSettings.Power );
    }
    else
    {
        SX1276Write( REG_LR_OCP, 0x3f );							//默认参数PA最在电流为100mA,当输出20dBm时需要120mA
		SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST );	//选择PA输出引脚(PA_BOOST引脚) 输出功率<=+20dBm
        SX1276LoRaSetPa20dBm( true );								//设置最大输出功率为20dBm(最大)
        LoRaSettings.Power = 20;									//设置输出功率为20dBm
        SX1276LoRaSetRFPower( LoRaSettings.Power );					//写入输出功率值
    } 
#endif
	
//	//重设前导码长度	--	6
//	SX1276LR->RegPreambleLsb = 0x06;								//设置前导码长度为6个符号长度
//	SX1276Write(REG_LR_PREAMBLELSB, SX1276LR->RegPreambleLsb);		//往寄存器写入设定值(0x06)

//	SX1276LR->RegPreambleLsb = 0x08;								//设为缺省值
//	SX1276Read(REG_LR_PREAMBLELSB, &SX1276LR->RegPreambleLsb);		//读取寄存器的值
//	
//    SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );						//设置为待机模式
}


void SX1276LoRaSetDefaults( void )
{
    //REMARK: See SX1276 datasheet for modified default values
    SX1276Read( REG_LR_VERSION, &SX1276LR->RegVersion );
}


void SX1276LoRaReset( void )
{
    SX1276SetReset( RADIO_RESET_ON );	//拉低复位引脚实现复位	 
    delay_ms(1);						//等待1ms

    SX1276SetReset( RADIO_RESET_OFF );	//拉高复位引脚停止复位
    delay_ms(6);  						//等待6ms
}


/*****************************************************************
	* 功能描述：	设置LoRa工作模式
	* 入口参数：	RFLR_OPMODE_SLEEP		--	睡眠模式
					RFLR_OPMODE_STANDBY		--	待机模式
					RFLR_OPMODE_TRANSMITTER	--	发送模式
					RFLR_OPMODE_RECEIVER	--	持续接收模式
	* 返回值：		无
*******************************************************************/
void SX1276LoRaSetOpMode( u8 opMode )
{
    static u8   opModePrev 				= RFLR_OPMODE_STANDBY;
    static bool antennaSwitchTxOnPrev 	= true;
    bool 		antennaSwitchTxOn 		= false;

    opModePrev = SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;

    if( opMode != opModePrev )
    {
        if( opMode == RFLR_OPMODE_TRANSMITTER )
        {
            antennaSwitchTxOn = true;
        }
        else
        {
            antennaSwitchTxOn = false;
        }
        if( antennaSwitchTxOn != antennaSwitchTxOnPrev )
        {
            antennaSwitchTxOnPrev = antennaSwitchTxOn;
            //RXTX( antennaSwitchTxOn ); // Antenna switch control
        }
        SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_MASK ) | opMode;

        SX1276Write( REG_LR_OPMODE, SX1276LR->RegOpMode );        
    }
}


u8 SX1276LoRaGetOpMode( void )
{
    SX1276Read( REG_LR_OPMODE, &SX1276LR->RegOpMode );
    
    return SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;
}


u8 SX1276LoRaReadRxGain( void )
{
    SX1276Read( REG_LR_LNA, &SX1276LR->RegLna );
    return( SX1276LR->RegLna >> 5 ) & 0x07;
}

/*
double SX1276LoRaReadRssi( void )
{
    // Reads the RSSI value
    SX1276Read( REG_LR_RSSIVALUE, &SX1276LR->RegRssiValue );

    if( LoRaSettings.RFFrequency < 860000000 )  // LF
    {
        return RssiOffsetLF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegRssiValue;
    }
    else
    {
        return RssiOffsetHF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegRssiValue;
    }
}
*/


u8 SX1276LoRaGetPacketRxGain( void )
{
    return RxGain;
}


int8_t SX1276LoRaGetPacketSnr( void )
{
    return RxPacketSnrEstimate;
}


double SX1276LoRaGetPacketRssi( void )
{
    return RxPacketRssiValue;
}


void SX1276LoRaStartRx( void )
{
    SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );
}


void SX1276LoRaGetRxPacket( void *buffer, uint16_t *size )
{
    *size = RxPacketSize;
    RxPacketSize = 0;
    memcpy( ( void * )buffer, ( void * )RFBuffer, ( size_t )*size );
}


void SX1276LoRaSetTxPacket( const void *buffer, u16 size )
{
    if( LoRaSettings.FreqHopOn == false )
    {
        TxPacketSize = size;
    }
    else
    {
        TxPacketSize = 255;
    }
    memcpy( ( void * )RFBuffer, buffer, ( size_t )TxPacketSize ); 

    RFLRState = RFLR_STATE_TX_INIT;
}


u8 SX1276LoRaGetRFState( void )
{
    return RFLRState;
}


void SX1276LoRaSetRFState( u8 state )
{
    RFLRState = state;
}


/*!
 * \brief Process the LoRa modem Rx and Tx state machines depending on the
 *        SX1276 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
u32 SX1276LoRaProcess( void )
{
    u32 result = RF_BUSY;
    
    switch( RFLRState )
    {
		case RFLR_STATE_IDLE:
			break;
		
		case RFLR_STATE_RX_INIT:
			SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );							//设置为待机模式

			SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT 			|
										//RFLR_IRQFLAGS_RXDONE 				|
										//RFLR_IRQFLAGS_PAYLOADCRCERROR 	|
										RFLR_IRQFLAGS_VALIDHEADER 			|
										RFLR_IRQFLAGS_TXDONE 				|
										RFLR_IRQFLAGS_CADDONE 				|
										//RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL 	|
										RFLR_IRQFLAGS_CADDETECTED;
			SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );		//设置为接收、CRC错误、
																				//FHSS改变信道中断
			if( LoRaSettings.FreqHopOn == true )
			{
				SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

				SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
				SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
			}
			else
			{
				SX1276LR->RegHopPeriod = 255;
			}
			
			SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
					
										// RxDone                    RxTimeout                   FhssChangeChannel           CadDone
			SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
										// CadDetected               ModeReady
			SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
			SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );
			//单次接收模式
			if( LoRaSettings.RxSingleOn == true ) 
			{

				SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
			}
			//持续接收模式
			else 
			{
				SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;			//
				SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );	//
				SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER );					//设置为持续接收模式
			}
			
			memset( RFBuffer, 0, (size_t)RF_BUFFER_SIZE );						//将BUF清零
			PacketTimeout	= LoRaSettings.RxPacketTimeout;
			RxTimeoutTimer = GET_TICK_COUNT();									//设置超时时间
			RFLRState 		= RFLR_STATE_RX_RUNNING;							//设置为进入接收状态
			break;
			
		case RFLR_STATE_RX_RUNNING:
			//接收完成
			if( DIO0 == 1 ) 
			{
				RxTimeoutTimer = GET_TICK_COUNT();
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
					SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				//清除标志位
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );
				RFLRState = RFLR_STATE_RX_DONE;
			}
			//FHSS改变信道
			if( DIO2 == 1 ) 
			{
				RxTimeoutTimer = GET_TICK_COUNT();
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
					SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
				// Debug
				RxGain = SX1276LoRaReadRxGain( );
			}
			//Rx single mode
			if( LoRaSettings.RxSingleOn == true ) 
			{
				if( ( GET_TICK_COUNT() - RxTimeoutTimer ) > PacketTimeout )
				{
					RFLRState = RFLR_STATE_RX_TIMEOUT;
				}
			}
			break;
			
		//接收完成
		case RFLR_STATE_RX_DONE:
			SX1276Read( REG_LR_IRQFLAGS, &SX1276LR->RegIrqFlags );
			if( ( SX1276LR->RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR ) == RFLR_IRQFLAGS_PAYLOADCRCERROR )
			{
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR  );
				
				if( LoRaSettings.RxSingleOn == true ) // Rx single mode
				{
					RFLRState = RFLR_STATE_RX_INIT;
				}
				else
				{
					RFLRState = RFLR_STATE_RX_RUNNING;
				}
				break;
			}
			
			{
				u8 rxSnrEstimate;
				SX1276Read( REG_LR_PKTSNRVALUE, &rxSnrEstimate );
				if( rxSnrEstimate & 0x80 ) // The SNR sign bit is 1
				{
					// Invert and divide by 4
					RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;
					RxPacketSnrEstimate = -RxPacketSnrEstimate;
				}
				else
				{
					// Divide by 4
					RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;
				}
			}
			
			if( LoRaSettings.RFFrequency < 860000000 )  // LF
			{    
				if( RxPacketSnrEstimate < 0 )
				{
					RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + NOISE_FIGURE_LF + ( double )RxPacketSnrEstimate;
				}
				else
				{    
					SX1276Read( REG_LR_PKTRSSIVALUE, &SX1276LR->RegPktRssiValue );
					RxPacketRssiValue = RssiOffsetLF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegPktRssiValue;
				}
			}
			else                                        // HF
			{    
				if( RxPacketSnrEstimate < 0 )
				{
					RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + NOISE_FIGURE_HF + ( double )RxPacketSnrEstimate;
				}
				else
				{    
					SX1276Read( REG_LR_PKTRSSIVALUE, &SX1276LR->RegPktRssiValue );
					RxPacketRssiValue = RssiOffsetHF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegPktRssiValue;
				}
			}

			if( LoRaSettings.RxSingleOn == true ) // Rx single mode
			{
				SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;
				SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );

				if( LoRaSettings.ImplicitHeaderOn == true )
				{
					RxPacketSize = SX1276LR->RegPayloadLength;
					SX1276ReadFifo( RFBuffer, SX1276LR->RegPayloadLength );
				}
				else
				{
					SX1276Read( REG_LR_NBRXBYTES, &SX1276LR->RegNbRxBytes );
					RxPacketSize = SX1276LR->RegNbRxBytes;
					SX1276ReadFifo( RFBuffer, SX1276LR->RegNbRxBytes );
				}
			}
			else // Rx continuous mode
			{
				SX1276Read( REG_LR_FIFORXCURRENTADDR, &SX1276LR->RegFifoRxCurrentAddr );

				if( LoRaSettings.ImplicitHeaderOn == true )
				{
					RxPacketSize = SX1276LR->RegPayloadLength;
					SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
					SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );
					SX1276ReadFifo( RFBuffer, SX1276LR->RegPayloadLength );
				}
				else
				{
					SX1276Read( REG_LR_NBRXBYTES, &SX1276LR->RegNbRxBytes );
					RxPacketSize = SX1276LR->RegNbRxBytes;
					SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
					SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );
					SX1276ReadFifo( RFBuffer, SX1276LR->RegNbRxBytes );
				}
			}
			
			if( LoRaSettings.RxSingleOn == true ) // Rx single mode
			{
				RFLRState = RFLR_STATE_RX_INIT;
			}
			else // Rx continuous mode
			{
				RFLRState = RFLR_STATE_RX_RUNNING;
			}
			result = RF_RX_DONE;
			break;
			
		case RFLR_STATE_RX_TIMEOUT:
			RFLRState = RFLR_STATE_RX_INIT;
			result = RF_RX_TIMEOUT;
			break;
		
		case RFLR_STATE_TX_INIT:

			SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );

			if( LoRaSettings.FreqHopOn == true )
			{
				SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
											RFLR_IRQFLAGS_RXDONE |
											RFLR_IRQFLAGS_PAYLOADCRCERROR |
											RFLR_IRQFLAGS_VALIDHEADER |
											//RFLR_IRQFLAGS_TXDONE |
											RFLR_IRQFLAGS_CADDONE |
											//RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
											RFLR_IRQFLAGS_CADDETECTED;
				SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

				SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
				SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
			}
			else
			{
				SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
											RFLR_IRQFLAGS_RXDONE |
											RFLR_IRQFLAGS_PAYLOADCRCERROR |
											RFLR_IRQFLAGS_VALIDHEADER |
											//RFLR_IRQFLAGS_TXDONE |
											RFLR_IRQFLAGS_CADDONE |
											RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
											RFLR_IRQFLAGS_CADDETECTED;
				SX1276LR->RegHopPeriod = 0;
			}
			SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
			SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );

			// Initializes the payload size
			SX1276LR->RegPayloadLength = TxPacketSize;
			SX1276Write( REG_LR_PAYLOADLENGTH, SX1276LR->RegPayloadLength );
			
			SX1276LR->RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
			SX1276Write( REG_LR_FIFOTXBASEADDR, SX1276LR->RegFifoTxBaseAddr );

			SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoTxBaseAddr;
			SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );
			
			// Write payload buffer to LORA modem
			SX1276WriteFifo( RFBuffer, SX1276LR->RegPayloadLength );
											// TxDone               RxTimeout                   FhssChangeChannel          ValidHeader         
			SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
											// PllLock              Mode Ready
			SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
			SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );

			SX1276LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );

			RFLRState = RFLR_STATE_TX_RUNNING;
			break;
			
		case RFLR_STATE_TX_RUNNING:
			if( DIO0 == 1 ) // TxDone
			{
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );
				RFLRState = RFLR_STATE_TX_DONE;   
			}
			if( DIO2 == 1 ) // FHSS Changed Channel
			{
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
					SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
			}
			break;
			
		case RFLR_STATE_TX_DONE:
			// optimize the power consumption by switching off the transmitter as soon as the packet has been sent
			SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );

			RFLRState 	= RFLR_STATE_IDLE;
			result 		= RF_TX_DONE;
			break;
		
		case RFLR_STATE_CAD_INIT:    
			SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
		
			SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
										RFLR_IRQFLAGS_RXDONE |
										RFLR_IRQFLAGS_PAYLOADCRCERROR |
										RFLR_IRQFLAGS_VALIDHEADER |
										RFLR_IRQFLAGS_TXDONE |
										//RFLR_IRQFLAGS_CADDONE |
										RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL; // |
										//RFLR_IRQFLAGS_CADDETECTED;
			SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );
			   
										// RxDone                   RxTimeout                   FhssChangeChannel           CadDone
			SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
										// CAD Detected              ModeReady
			SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
			SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );
				
			SX1276LoRaSetOpMode( RFLR_OPMODE_CAD );
			RFLRState = RFLR_STATE_CAD_RUNNING;
			break;
			
		//CAD Done interrupt
		case RFLR_STATE_CAD_RUNNING:
			if( DIO3 == 1 ) 
			{ 
				//清除CAD标志位
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE  );
				// CAD Detected interrupt
				if( DIO4 == 1 ) 
				{
					//清除标志位
					SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED  );
					// CAD detected, we have a LoRa preamble
					RFLRState = RFLR_STATE_RX_INIT;
					result = RF_CHANNEL_ACTIVITY_DETECTED;
				} 
				else
				{    
					// The device goes in Standby Mode automatically    
					RFLRState = RFLR_STATE_IDLE;
					result = RF_CHANNEL_EMPTY;
				}
			}   
			break;
    
		default:
			break;
    } 
	return result;
}


///////////////////////////////////////////////////////////////////////////////
/********************************************************************
	* 功能描述：	获取调用本函数前最后接到那包数据的信噪比
	* 入口参数：	无
	* 返回值：		 信噪比，有正有负
*********************************************************************/
int8_t GetLoRaSNR (void)
{  
	u8 rxSnrEstimate;																
	//取最后一包数据的信噪比         									//记录信噪比估值
	SX1276Read(REG_LR_PKTSNRVALUE, &rxSnrEstimate );					//信噪比                     
	//The SNR sign bit is 1												//信噪比小于0的情况，负数     
	if( rxSnrEstimate & 0x80 ) 
	{ 
		RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;	//Invert and divide by 4      
		RxPacketSnrEstimate = -RxPacketSnrEstimate;
	} 
	else
	{																	//信噪比大于0的情况，正数     
		RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;			//Divide by 4                
	}
	return RxPacketSnrEstimate;
}


/*******************************************************************************
	** Function name:       GetPackLoRaRSSI
	** Descriptions:        获取调用本函数前最后接到那包数据的信号强度
	** input parameters:    无
	** output parameters:   无
	** Returned value:      信号强度，有正有负
********************************************************************************/
double GetPackLoRaRSSI(void)
{  
	if( GetLoRaSNR() < 0 ) 
	{                                           
		/***************************************************************************************
			*功率：P=-174（dBm） + BW(dB) + NF(dB) + SNR(dB); 
			*在信号被噪声淹没的情况下用此公式反推RSSI，前三项是热噪声功率,最后一项是信噪比
		****************************************************************************************/
		//信号被噪声淹没             
		RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + \
							NOISE_FIGURE_LF     + (double)RxPacketSnrEstimate;	
	} 
	else 
	{   
		//信号强于噪声
		SX1276Read( REG_LR_PKTRSSIVALUE, &SX1276LR->RegPktRssiValue );
		RxPacketRssiValue = RssiOffsetLF[LoRaSettings.SignalBw] + (double)SX1276LR->RegPktRssiValue;
	}
	return RxPacketRssiValue;
}


/*********************************************************************************************************
	** Function name:       SX1276LoRaReadRssi
	** Descriptions:        读取当前信号强度,返回的是调用这个函数那一刻的信号强度
							需要在接收状态下调用才有用。 
	** input parameters:    无
	** output parameters:   无
*********************************************************************************************************/
double SX1276LoRaReadRssi( void )
{
    SX1276Read( REG_LR_RSSIVALUE, &SX1276LR->RegRssiValue );	//寄存器0x1B，当前信号强度的值 
	return RssiOffsetHF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegRssiValue;	
	//此处只能测量输入到天线接口处的功率；包括信号和噪声。
    //与包信号强度不一样，因为包信号强度可通过SNR来还原噪声之下的信号强度  
}


/*********************************************************************************************************
	* Function name:       LoRaRxStateEnter
	* Descriptions:        进入接收状态
	* input parameters:    无
	* output parameters:   无
	* Returned value:      无
*********************************************************************************************************/
void LoRaRxStateEnter (void)
{
	//1.---->进入待机模式
	SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );		//操作寄存器只能在在Standby,sleep or FSTX 模式       
	
	//2.---->接收初始化，参数设置
	SX1276LR->RegIrqFlagsMask	= 	RFLR_IRQFLAGS_RXTIMEOUT          |	//超时中断屏蔽
                                    //RFLR_IRQFLAGS_RXDONE           |	//打开数据包接收完成中断
                                    RFLR_IRQFLAGS_PAYLOADCRCERROR    |	//负载CRC错误中断屏蔽	
                                    RFLR_IRQFLAGS_VALIDHEADER        |	//Rx模式下接收到的有效报头屏蔽
                                    RFLR_IRQFLAGS_TXDONE             |	//FIFO负载发送完成中断屏蔽
                                    RFLR_IRQFLAGS_CADDONE            |	//CAD完成中断屏蔽
                                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |	//FHSS改变信道中断屏蔽
                                    RFLR_IRQFLAGS_CADDETECTED;			//检测到CAD中断屏蔽
	SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );		//设置需要屏蔽的中断(被注释掉的中断即打开)

    SX1276LR->RegHopPeriod = 255;
	SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );			//频率跳变之间的符号周期:255
                
    //DIO0:RxDone 
    SX1276LR->RegDioMapping1 = 	RFLR_DIOMAPPING1_DIO0_00 |				//DIO0~DIO3引脚映射
								RFLR_DIOMAPPING1_DIO1_11 |		 
								RFLR_DIOMAPPING1_DIO2_11 | 
								RFLR_DIOMAPPING1_DIO3_11;
	
	SX1276LR->RegDioMapping2 = 	RFLR_DIOMAPPING2_DIO4_10 | 				//DIO4~DIO5引脚映射
								RFLR_DIOMAPPING2_DIO5_11;
								
	/*****************************************************************************
		*在ZM470SX-M上,DIO4接到射频开关的6脚,进入接收状态后此引脚需要拉高,
		*所以在LoRa模式必须设为:RFLR_DIOMAPPING2_DIO4_10
	******************************************************************************/				
    SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );	//配置5个IO的功能  
    
	//3.---->设置接收模式(单次/持续模式)
	if( LoRaSettings.RxSingleOn == true ) 
	{ 
		//设置为单次接收
		SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
	}
	else
	{ 
		//设置为持续接收模式
		SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;		  	//内容:0x00 
		SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );	//SPI访问FIFO的地址
		SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER );           			//进入接收状态
	}
	
//	PacketTimeout  = LoRaSettings.RxPacketTimeout;						//超时时间为 100
//	RxTimeoutTimer = GET_TICK_COUNT( );									//接收超时时间为 10		
}


/**************************************************************
	* 功能描述：	读取接收到数据
	* 入口参数：	pbuf	--	数据缓冲区指针 
					size	--	数据字节长度指针  
	* 返回值：		无
***************************************************************/
void LoRaRxDataRead (u8 *pbuf, u8 *size )
{
	//4.---->等待数据接收完成
	if( DIO0 == 1 ) 
	{                                                  				//RxDone                      
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );		//执行写操作以清除接收中断标志
	}	
	
    SX1276Read( REG_LR_IRQFLAGS, &SX1276LR->RegIrqFlags );			//读取中断状态标志位    
	//5.---->检测CRC标志，以验证数据包的完整性 	
	if( (SX1276LR->RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR) == RFLR_IRQFLAGS_PAYLOADCRCERROR )
	{
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR  );	//执行写操作以清除接收中断标志
		*size = 0;
		return;
	}     
	
//	GetLoRaSNR();		//读取最后一包数据信噪比(如果不关心这SNR参数可把此函数删除)     	     
//	GetPackLoRaRSSI();	//读取最后一包数据信号强度(如果不关心这RSSI参数可把此函数删除)

	//6.---->读取接收到的数据 
	SX1276Read( REG_LR_FIFORXCURRENTADDR, &SX1276LR->RegFifoRxCurrentAddr );	//读取最后接收一包数据首指针
	
	//隐式报头模式
	if( LoRaSettings.ImplicitHeaderOn == true ) 
	{
		RxPacketSize = SX1276LR->RegPayloadLength;
		SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
		SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );			//写入FIFO的访问地址
		SX1276ReadFifo( RFBuffer, SX1276LR->RegPayloadLength );
	} 
	//显式报头模式
	else 
	{
		SX1276Read( REG_LR_NBRXBYTES, size );	 						//读取数据包包长度(size记录数据包的长度)
		SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;		//将最后接收的数据包的首地址赋予FIFO数据缓冲区的访问地址(SPI)
		SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );  	//写入FIFO的访问地址
		SX1276ReadFifo(pbuf, *size);									//读取FIFO中的数据，并保存在pbuf中
	}
}	


//u16 Tx_Counter = 0;
/****************************************************************
	* 功能描述：	发送数据
	* 入口参数：	pbuf		--	数据缓冲区指针 
					size		--	数据字节数  
	* 返回值：		0			--	发送成功
					其他(1,2)	--	发送失败
******************************************************************/
u8 LoRaTxData (u8 *pbuf, u8 size ,u8 *pcrcbuf ,u8 crcflag)
{
     u32 i,j; 
	if ( pbuf==0 || size==0 )	//没有数据可发送
	{
		return 1;				//发送失败 -- 返回1
	}		 
	//1.------>进入待机状态
    SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
	
	//2.------>发送初始化
	SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT          |
								RFLR_IRQFLAGS_RXDONE             |
								RFLR_IRQFLAGS_PAYLOADCRCERROR    |
								RFLR_IRQFLAGS_VALIDHEADER        |
								//RFLR_IRQFLAGS_TXDONE           |		//打开发送完成中断(Tx)
								RFLR_IRQFLAGS_CADDONE            |
								RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
								RFLR_IRQFLAGS_CADDETECTED;
	SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );
	
	SX1276LR->RegHopPeriod = 0;											//频率跳变之间的符号时周期为：0
	SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
	
	//初始化待发送数据所占缓存区的大小
	SX1276LR->RegPayloadLength = size; 									//数据负载长度(不包括CRC长度)
	SX1276Write( REG_LR_PAYLOADLENGTH, SX1276LR->RegPayloadLength +4); 	//在implicit模式(隐式包头),必须写入FIFO长度

	SX1276LR->RegFifoTxBaseAddr = 0x00;                                 //发送基地址设置为0x00   
	SX1276Write( REG_LR_FIFOTXBASEADDR, SX1276LR->RegFifoTxBaseAddr );  //写入发送的首地址 

	SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoTxBaseAddr;				//FIFO地址指针指向Tx基地址
	SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );        //写入FIFO的访问地址
	
	//3.------>将待发送的数据写入到FIFO中
	SX1276WriteFifo( pbuf, SX1276LR->RegPayloadLength );	            //写入要发送的数据
	
	//最后写入4Byte的CRC校验值
	if(crcflag == 1)
	{
		if( size<(256-4) )
			SX1276WriteFifo( pcrcbuf, 4 );
		else
			return 1;
	}
	
	/*****************************************************************************
		*在ZM470SX-M上,DIO4接到射频开关的6脚,进入发送状态后此引脚需要拉低,
		*所以在LoRa模式必须设为:RFLR_DIOMAPPING2_DIO4_00
	******************************************************************************/
    ///////////////////////////////////////////////////////////////////////////////
	//DIO0:TxDone  端口映射设置 
	SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_11| 
	                           RFLR_DIOMAPPING1_DIO2_11 | RFLR_DIOMAPPING1_DIO3_11;
	SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_11;
	SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );
	///////////////////////////////////////////////////////////////////////////////
	
	//4.------>进入到发送模式，发送数据
    SX1276LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );		//进入发送状态    
	
	//5.------>等待发送完成(检测TxDone中断)
	printf("开始发送时间为		--	t1:%d\r\n",LoRaTransCounter);
	i = 298000; 
	while ( DIO0 == 0 && i != 0 )			//等待发送完成或者发送失败超时,完成后:DIO0为高电平
	{                 
        i--;	
		for(j = 0; j < 100; j++);			//超时等待约1ms        
	}
	printf("i的值为	-- %d\r\n",298000-i);
	//如果发送失败   
	if (i == 0) 
	{                               
		//失败 -- 返回2
		printf("发送失败	--	2\r\n");
		return 2;	
	} 
	//如果发送成功
	else 
	{					               
	//6.------>发送完成进入到待机模式
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );	//清除发送完成中断           
		SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );				//进入待机状态
		
//		printf("成功发送的时间为	--	t2:%d\r\n",LoRaTransCounter);
//		printf("发送成功	--	0\r\n");
//		Tx_Counter++;	//记录成功发送的次数		
		return 0;		//成功 -- 返回0
	}          
}


/******************************************************
	* Function name:       LoRaTxPower
	* Descriptions:        设置发射功率
	* input parameters:    pwr: 功率范围5 ~ 20
	* output parameters:   无
	* Returned value:      无
*******************************************************/
void LoRaTxPower (u8 pwr)
{  
	//默认参数PA最在电流为100mA,当输出20dBm时需要120mA,所以必须配置0x0b存器
	SX1276Write( REG_LR_OCP, 0x3f );	
	SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST );
	SX1276LoRaSetPa20dBm( true );
	SX1276LoRaSetRFPower( pwr);
}


/******************************************************
	* 功能描述：	设置载波频率
	* input parameters:    420000000~500000000
	* output parameters:   无
	* Returned value:      无
*******************************************************/
void LoRaFreqSet (u32 freq )
{  
    SX1276LoRaSetRFFrequency( freq );
}

#endif
