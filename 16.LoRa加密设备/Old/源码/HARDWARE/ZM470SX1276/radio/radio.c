#include "radio.h"


//定义一个结构体
tRadioDriver RadioDriver;

tRadioDriver* RadioDriverInit( void )
{
    RadioDriver.Init 		= SX1276Init;
    RadioDriver.Reset 		= SX1276Reset;
    RadioDriver.StartRx		= SX1276StartRx;
    RadioDriver.GetRxPacket = SX1276GetRxPacket;
    RadioDriver.SetTxPacket = SX1276SetTxPacket;
    RadioDriver.Process 	= SX1276Process;
	/**************************************************/
	//RadioDriver.RFDio0State 	= SX1276ReadDio0;	
	RadioDriver.RFGetPacketSnr 	= SX1276GetPacketSnr;
	RadioDriver.RFGetPacketRssi = SX1276GetPacketRssi;
	RadioDriver.RFReadRssi 		= SX1276ReadRssi;	
	RadioDriver.RFRxStateEnter 	= SX1276RxStateEnter;
	RadioDriver.RFRxDataRead 	= SX1276RxDataRead;
	RadioDriver.RFTxData 		= SX1276TxData;
	RadioDriver.RFTxPower 		= SX1276TxPower;
	RadioDriver.RFFreqSet 		= SX1276FreqSet;
	RadioDriver.RFOpModeSet 	= SX1276SetOpMode;
	/**************************************************/
    return &RadioDriver;
}

