#ifndef __SX1276_LORA_H__
#define __SX1276_LORA_H__

#include <stdbool.h>
#include "sys.h"
#include "led.h"
#include "timer.h"
#include "lora.h"


//SX1276 LoRa General parameters definition
typedef struct sLoRaSettings
{
    u32  RFFrequency;
    int8_t Power;
    u8   SignalBw;					// LORA [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
									// 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]  
    u8 	 SpreadingFactor;			// LORA [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    u8 	 ErrorCoding;				// LORA [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    bool CrcOn;						// [0: OFF, 1: ON]
    bool ImplicitHeaderOn;			// [0: OFF, 1: ON]
    bool RxSingleOn;				// [0: Continuous, 1 Single]
    bool FreqHopOn;					// [0: OFF, 1: ON]
    u8   HopPeriod;					// Hops every frequency hopping period symbols
    u32  TxPacketTimeout;
    u32  RxPacketTimeout;
    u8   PayloadLength;
}tLoRaSettings;


//RF数据包大小(模块配备了256Byte的RAM缓存，该缓存仅能通过LoRa模式访问)
#define		RF_BUFFER_SIZE_MAX				256
#define 	RF_BUFFER_SIZE					256

extern u16 Tx_Counter;

//RF state machine
//LoRa
typedef enum
{
    RFLR_STATE_IDLE,
    RFLR_STATE_RX_INIT,
    RFLR_STATE_RX_RUNNING,
    RFLR_STATE_RX_DONE,
    RFLR_STATE_RX_TIMEOUT,
    RFLR_STATE_TX_INIT,
    RFLR_STATE_TX_RUNNING,
    RFLR_STATE_TX_DONE,
    RFLR_STATE_TX_TIMEOUT,
    RFLR_STATE_CAD_INIT,
    RFLR_STATE_CAD_RUNNING,
}tRFLRStates;



//SX1276 definitions
#define 			XTAL_FREQ						32000000		//32MHz		
#define 			FREQ_STEP						61.03515625		//频率步长	--	32000000/(2^19)


//---------------------------->SX1276内部寄存器地址
#define 			REG_LR_FIFO						0x00 	//FIFO读/写访问(SPI协议)

//通用设置
#define 			REG_LR_OPMODE                   0x01	//运行模式	--	LoRa/FSK选择
#define 			REG_LR_BANDSETTING              0x04	//频率偏移设置(最高有效位)
#define 			REG_LR_FRFMSB                   0x06 	//RF载波频率最高有效位
#define 			REG_LR_FRFMID                   0x07	//RF载波频率中间有效位
#define 			REG_LR_FRFLSB                   0x08 	//RF载波频率最低有效位

//发送设置
#define 			REG_LR_PACONFIG					0x09 	//PA选择和输出功率控制
#define 			REG_LR_PARAMP					0x0A 	//LoRa模式下斜升/斜降时间
#define 			REG_LR_OCP						0x0B 	//过流保护控制

//接收设置
#define 			REG_LR_LNA						0x0C	//LNA设置

//LoRa寄存器
#define 			REG_LR_FIFOADDRPTR				0x0D 	//FIFO SPI指针
#define 			REG_LR_FIFOTXBASEADDR			0x0E	//起始Tx数据
#define 			REG_LR_FIFORXBASEADDR			0x0F 	//起始Rx数据
#define 			REG_LR_FIFORXCURRENTADDR		0x10 	//最后接收数据包的起始地址
#define 			REG_LR_IRQFLAGSMASK				0x11 	//可选IRQ标志屏蔽
#define 			REG_LR_IRQFLAGS					0x12 	//IRQ标志
#define 			REG_LR_NBRXBYTES				0x13 	//最近一次接收到的数据包的负载字节数
#define 			REG_LR_RXHEADERCNTVALUEMSB		0x14 
#define 			REG_LR_RXHEADERCNTVALUELSB		0x15 
#define 			REG_LR_RXPACKETCNTVALUEMSB		0x16 
#define 			REG_LR_RXPACKETCNTVALUELSB		0x17 
#define 			REG_LR_MODEMSTAT				0x18 
#define 			REG_LR_PKTSNRVALUE				0x19 	//最后一个数据包的信噪比估值
#define 			REG_LR_PKTRSSIVALUE				0x1A 
#define 			REG_LR_RSSIVALUE				0x1B 
#define 			REG_LR_HOPCHANNEL				0x1C 	//FHSS起始信道
#define 			REG_LR_MODEMCONFIG1				0x1D 	//调制解调器物理层配置1
#define 			REG_LR_MODEMCONFIG2				0x1E 	//调制解调器物理层配置2
#define 			REG_LR_SYMBTIMEOUTLSB			0x1F 
#define 			REG_LR_PREAMBLEMSB				0x20 
#define 			REG_LR_PREAMBLELSB				0x21 
#define 			REG_LR_PAYLOADLENGTH			0x22 	//LoRa负载长度
#define 			REG_LR_PAYLOADMAXLENGTH			0x23 
#define 			REG_LR_HOPPERIOD				0x24 
#define 			REG_LR_FIFORXBYTEADDR			0x25
#define 			REG_LR_MODEMCONFIG3				0x26	//调制解调器物理层配置3
//end of documented register in datasheet

//I/O口映射
#define 			REG_LR_DIOMAPPING1              0x40	//DIO0~DIO3引脚映射
#define 			REG_LR_DIOMAPPING2              0x41	//DIO4~DIO5引脚映射

//版本
#define 			REG_LR_VERSION                  0x42	//芯片版本ID

//Additional settings
#define 			REG_LR_PLLHOP                   0x44
#define 			REG_LR_TCXO                     0x4B
#define 			REG_LR_PADAC					0x4D
#define 			REG_LR_FORMERTEMP				0x5B
#define 			REG_LR_BITRATEFRAC				0x5D
#define 			REG_LR_AGCREF					0x61
#define 			REG_LR_AGCTHRESH1				0x62
#define 			REG_LR_AGCTHRESH2				0x63
#define 			REG_LR_AGCTHRESH3				0x64


/*!
 * SX1276 LoRa bit control definition
 */

/*!
 * RegFifo
 */

/*!
 * RegOpMode
 */
#define RFLR_OPMODE_LONGRANGEMODE_MASK              0x7F 	//0111 1111
#define RFLR_OPMODE_LONGRANGEMODE_OFF               0x00 	//Default
#define RFLR_OPMODE_LONGRANGEMODE_ON                0x80 	//1000 0000

#define RFLR_OPMODE_ACCESSSHAREDREG_MASK            0xBF 
#define RFLR_OPMODE_ACCESSSHAREDREG_ENABLE          0x40 
#define RFLR_OPMODE_ACCESSSHAREDREG_DISABLE         0x00 	//Default

#define RFLR_OPMODE_FREQMODE_ACCESS_MASK            0xF7
#define RFLR_OPMODE_FREQMODE_ACCESS_LF              0x08 	//Default
#define RFLR_OPMODE_FREQMODE_ACCESS_HF              0x00 

#define RFLR_OPMODE_MASK                            0xF8 	//1111 1000		--	bit2~0
#define RFLR_OPMODE_SLEEP                           0x00 	//0000 0000		--	睡眠模式
#define RFLR_OPMODE_STANDBY                         0x01 	//0000 0001		--	待机模式(缺省值)
#define RFLR_OPMODE_SYNTHESIZER_TX                  0x02 	//0000 0010		--	频率合成发送模式
#define RFLR_OPMODE_TRANSMITTER                     0x03	//0000 0011		--	发送模式
#define RFLR_OPMODE_SYNTHESIZER_RX                  0x04 	//0000 0100		--	频率合成接收模式
#define RFLR_OPMODE_RECEIVER                        0x05 	//0000 0101		--	持续接收模式
//LoRa specific modes
#define RFLR_OPMODE_RECEIVER_SINGLE                 0x06 	//0000 0110		--	单次接收
#define RFLR_OPMODE_CAD                             0x07	//0000 0111		--	信道活动检测

/*!
 * RegBandSetting 
 */
#define RFLR_BANDSETTING_MASK                   	0x3F 
#define RFLR_BANDSETTING_AUTO                    	0x00 	//Default
#define RFLR_BANDSETTING_DIV_BY_1                	0x40
#define RFLR_BANDSETTING_DIV_BY_2                	0x80
#define RFLR_BANDSETTING_DIV_BY_6                	0xC0

/*!
 * RegFrf (MHz)
 */
 
#define RFLR_FRFMSB_434_MHZ                         0x6C 	//Default
#define RFLR_FRFMID_434_MHZ                         0x80 	//Default
#define RFLR_FRFLSB_434_MHZ                         0x00 	//Default

#define RFLR_FRFMSB_863_MHZ                         0xD7
#define RFLR_FRFMID_863_MHZ                         0xC0
#define RFLR_FRFLSB_863_MHZ                         0x00
#define RFLR_FRFMSB_864_MHZ                         0xD8
#define RFLR_FRFMID_864_MHZ                         0x00
#define RFLR_FRFLSB_864_MHZ                         0x00
#define RFLR_FRFMSB_865_MHZ                         0xD8
#define RFLR_FRFMID_865_MHZ                         0x40
#define RFLR_FRFLSB_865_MHZ                         0x00
#define RFLR_FRFMSB_866_MHZ                         0xD8
#define RFLR_FRFMID_866_MHZ                         0x80
#define RFLR_FRFLSB_866_MHZ                         0x00
#define RFLR_FRFMSB_867_MHZ                         0xD8
#define RFLR_FRFMID_867_MHZ                         0xC0
#define RFLR_FRFLSB_867_MHZ                         0x00
#define RFLR_FRFMSB_868_MHZ                         0xD9
#define RFLR_FRFMID_868_MHZ                         0x00
#define RFLR_FRFLSB_868_MHZ                         0x00
#define RFLR_FRFMSB_869_MHZ                         0xD9
#define RFLR_FRFMID_869_MHZ                         0x40
#define RFLR_FRFLSB_869_MHZ                         0x00
#define RFLR_FRFMSB_870_MHZ                         0xD9
#define RFLR_FRFMID_870_MHZ                         0x80
#define RFLR_FRFLSB_870_MHZ                         0x00

#define RFLR_FRFMSB_902_MHZ                         0xE1
#define RFLR_FRFMID_902_MHZ                         0x80
#define RFLR_FRFLSB_902_MHZ                         0x00
#define RFLR_FRFMSB_903_MHZ                         0xE1
#define RFLR_FRFMID_903_MHZ                         0xC0
#define RFLR_FRFLSB_903_MHZ                         0x00
#define RFLR_FRFMSB_904_MHZ                         0xE2
#define RFLR_FRFMID_904_MHZ                         0x00
#define RFLR_FRFLSB_904_MHZ                         0x00
#define RFLR_FRFMSB_905_MHZ                         0xE2
#define RFLR_FRFMID_905_MHZ                         0x40
#define RFLR_FRFLSB_905_MHZ                         0x00
#define RFLR_FRFMSB_906_MHZ                         0xE2
#define RFLR_FRFMID_906_MHZ                         0x80
#define RFLR_FRFLSB_906_MHZ                         0x00
#define RFLR_FRFMSB_907_MHZ                         0xE2
#define RFLR_FRFMID_907_MHZ                         0xC0
#define RFLR_FRFLSB_907_MHZ                         0x00
#define RFLR_FRFMSB_908_MHZ                         0xE3
#define RFLR_FRFMID_908_MHZ                         0x00
#define RFLR_FRFLSB_908_MHZ                         0x00
#define RFLR_FRFMSB_909_MHZ                         0xE3
#define RFLR_FRFMID_909_MHZ                         0x40
#define RFLR_FRFLSB_909_MHZ                         0x00
#define RFLR_FRFMSB_910_MHZ                         0xE3
#define RFLR_FRFMID_910_MHZ                         0x80
#define RFLR_FRFLSB_910_MHZ                         0x00
#define RFLR_FRFMSB_911_MHZ                         0xE3
#define RFLR_FRFMID_911_MHZ                         0xC0
#define RFLR_FRFLSB_911_MHZ                         0x00
#define RFLR_FRFMSB_912_MHZ                         0xE4
#define RFLR_FRFMID_912_MHZ                         0x00
#define RFLR_FRFLSB_912_MHZ                         0x00
#define RFLR_FRFMSB_913_MHZ                         0xE4
#define RFLR_FRFMID_913_MHZ                         0x40
#define RFLR_FRFLSB_913_MHZ                         0x00
#define RFLR_FRFMSB_914_MHZ                         0xE4
#define RFLR_FRFMID_914_MHZ                         0x80
#define RFLR_FRFLSB_914_MHZ                         0x00
#define RFLR_FRFMSB_915_MHZ                         0xE4  	// Default
#define RFLR_FRFMID_915_MHZ                         0xC0  	// Default
#define RFLR_FRFLSB_915_MHZ                         0x00  	// Default
#define RFLR_FRFMSB_916_MHZ                         0xE5
#define RFLR_FRFMID_916_MHZ                         0x00
#define RFLR_FRFLSB_916_MHZ                         0x00
#define RFLR_FRFMSB_917_MHZ                         0xE5
#define RFLR_FRFMID_917_MHZ                         0x40
#define RFLR_FRFLSB_917_MHZ                         0x00
#define RFLR_FRFMSB_918_MHZ                         0xE5
#define RFLR_FRFMID_918_MHZ                         0x80
#define RFLR_FRFLSB_918_MHZ                         0x00
#define RFLR_FRFMSB_919_MHZ                         0xE5
#define RFLR_FRFMID_919_MHZ                         0xC0
#define RFLR_FRFLSB_919_MHZ                         0x00
#define RFLR_FRFMSB_920_MHZ                         0xE6
#define RFLR_FRFMID_920_MHZ                         0x00
#define RFLR_FRFLSB_920_MHZ                         0x00
#define RFLR_FRFMSB_921_MHZ                         0xE6
#define RFLR_FRFMID_921_MHZ                         0x40
#define RFLR_FRFLSB_921_MHZ                         0x00
#define RFLR_FRFMSB_922_MHZ                         0xE6
#define RFLR_FRFMID_922_MHZ                         0x80
#define RFLR_FRFLSB_922_MHZ                         0x00
#define RFLR_FRFMSB_923_MHZ                         0xE6
#define RFLR_FRFMID_923_MHZ                         0xC0
#define RFLR_FRFLSB_923_MHZ                         0x00
#define RFLR_FRFMSB_924_MHZ                         0xE7
#define RFLR_FRFMID_924_MHZ                         0x00
#define RFLR_FRFLSB_924_MHZ                         0x00
#define RFLR_FRFMSB_925_MHZ                         0xE7
#define RFLR_FRFMID_925_MHZ                         0x40
#define RFLR_FRFLSB_925_MHZ                         0x00
#define RFLR_FRFMSB_926_MHZ                         0xE7
#define RFLR_FRFMID_926_MHZ                         0x80
#define RFLR_FRFLSB_926_MHZ                         0x00
#define RFLR_FRFMSB_927_MHZ                         0xE7
#define RFLR_FRFMID_927_MHZ                         0xC0
#define RFLR_FRFLSB_927_MHZ                         0x00
#define RFLR_FRFMSB_928_MHZ                         0xE8
#define RFLR_FRFMID_928_MHZ                         0x00
#define RFLR_FRFLSB_928_MHZ                         0x00

/*!
 * RegPaConfig
 */
#define RFLR_PACONFIG_PASELECT_MASK                 0x7F 
#define RFLR_PACONFIG_PASELECT_PABOOST              0x80 
#define RFLR_PACONFIG_PASELECT_RFO                  0x00 	// Default

#define RFLR_PACONFIG_MAX_POWER_MASK                0x8F

#define RFLR_PACONFIG_OUTPUTPOWER_MASK              0xF0 
 
/*!
 * RegPaRamp
 */
#define RFLR_PARAMP_TXBANDFORCE_MASK                0xEF 
#define RFLR_PARAMP_TXBANDFORCE_BAND_SEL            0x10 
#define RFLR_PARAMP_TXBANDFORCE_AUTO                0x00 	// Default

#define RFLR_PARAMP_MASK                            0xF0 
#define RFLR_PARAMP_3400_US                         0x00 
#define RFLR_PARAMP_2000_US                         0x01 
#define RFLR_PARAMP_1000_US                         0x02
#define RFLR_PARAMP_0500_US                         0x03 
#define RFLR_PARAMP_0250_US                         0x04 
#define RFLR_PARAMP_0125_US                         0x05 
#define RFLR_PARAMP_0100_US                         0x06 
#define RFLR_PARAMP_0062_US                         0x07 
#define RFLR_PARAMP_0050_US                         0x08 
#define RFLR_PARAMP_0040_US                         0x09 	// Default
#define RFLR_PARAMP_0031_US                         0x0A 
#define RFLR_PARAMP_0025_US                         0x0B 
#define RFLR_PARAMP_0020_US                         0x0C 
#define RFLR_PARAMP_0015_US                         0x0D 
#define RFLR_PARAMP_0012_US                         0x0E 
#define RFLR_PARAMP_0010_US                         0x0F 

/*!
 * RegOcp
 */
#define RFLR_OCP_MASK                               0xDF 
#define RFLR_OCP_ON                                 0x20 	// Default
#define RFLR_OCP_OFF                                0x00   

#define RFLR_OCP_TRIM_MASK                          0xE0
#define RFLR_OCP_TRIM_045_MA                        0x00
#define RFLR_OCP_TRIM_050_MA                        0x01   
#define RFLR_OCP_TRIM_055_MA                        0x02 
#define RFLR_OCP_TRIM_060_MA                        0x03 
#define RFLR_OCP_TRIM_065_MA                        0x04 
#define RFLR_OCP_TRIM_070_MA                        0x05 
#define RFLR_OCP_TRIM_075_MA                        0x06 
#define RFLR_OCP_TRIM_080_MA                        0x07  
#define RFLR_OCP_TRIM_085_MA                        0x08
#define RFLR_OCP_TRIM_090_MA                        0x09 
#define RFLR_OCP_TRIM_095_MA                        0x0A 
#define RFLR_OCP_TRIM_100_MA                        0x0B  	// Default
#define RFLR_OCP_TRIM_105_MA                        0x0C 	
#define RFLR_OCP_TRIM_110_MA                        0x0D 
#define RFLR_OCP_TRIM_115_MA                        0x0E 
#define RFLR_OCP_TRIM_120_MA                        0x0F 
#define RFLR_OCP_TRIM_130_MA                        0x10
#define RFLR_OCP_TRIM_140_MA                        0x11   
#define RFLR_OCP_TRIM_150_MA                        0x12 
#define RFLR_OCP_TRIM_160_MA                        0x13 
#define RFLR_OCP_TRIM_170_MA                        0x14 
#define RFLR_OCP_TRIM_180_MA                        0x15 
#define RFLR_OCP_TRIM_190_MA                        0x16 
#define RFLR_OCP_TRIM_200_MA                        0x17  
#define RFLR_OCP_TRIM_210_MA                        0x18
#define RFLR_OCP_TRIM_220_MA                        0x19 
#define RFLR_OCP_TRIM_230_MA                        0x1A 
#define RFLR_OCP_TRIM_240_MA                        0x1B

/*!
 * RegLna
 */
#define RFLR_LNA_GAIN_MASK                          0x1F	//0001 1111 
#define RFLR_LNA_GAIN_G1                            0x20 	//0010 0000 (缺省值) 最大增益
#define RFLR_LNA_GAIN_G2                            0x40 
#define RFLR_LNA_GAIN_G3                            0x60 
#define RFLR_LNA_GAIN_G4                            0x80 
#define RFLR_LNA_GAIN_G5                            0xA0 
#define RFLR_LNA_GAIN_G6                            0xC0 

#define RFLR_LNA_BOOST_LF_MASK                      0xE7 
#define RFLR_LNA_BOOST_LF_DEFAULT                   0x00 // Default
#define RFLR_LNA_BOOST_LF_GAIN                      0x08 
#define RFLR_LNA_BOOST_LF_IP3                       0x10 
#define RFLR_LNA_BOOST_LF_BOOST                     0x18 

#define RFLR_LNA_RXBANDFORCE_MASK                   0xFB 
#define RFLR_LNA_RXBANDFORCE_BAND_SEL               0x04
#define RFLR_LNA_RXBANDFORCE_AUTO                   0x00 // Default

#define RFLR_LNA_BOOST_HF_MASK                      0xFC 
#define RFLR_LNA_BOOST_HF_OFF                       0x00 // Default
#define RFLR_LNA_BOOST_HF_ON                        0x03 

/*!
 * RegFifoAddrPtr
 */
#define RFLR_FIFOADDRPTR                            0x00 // Default

/*!
 * RegFifoTxBaseAddr
 */
#define RFLR_FIFOTXBASEADDR                         0x80 // Default

/*!
 * RegFifoTxBaseAddr
 */
#define RFLR_FIFORXBASEADDR                         0x00 // Default

/*!
 * RegFifoRxCurrentAddr (Read Only)
 */

/*!
 * RegIrqFlagsMask
 */
#define RFLR_IRQFLAGS_RXTIMEOUT_MASK                0x80 
#define RFLR_IRQFLAGS_RXDONE_MASK                   0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK          0x20 
#define RFLR_IRQFLAGS_VALIDHEADER_MASK              0x10 
#define RFLR_IRQFLAGS_TXDONE_MASK                   0x08 
#define RFLR_IRQFLAGS_CADDONE_MASK                  0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL_MASK       0x02 
#define RFLR_IRQFLAGS_CADDETECTED_MASK              0x01 

/*!
 * RegIrqFlags
 */
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80 
#define RFLR_IRQFLAGS_RXDONE                        0x40 
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20 
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10 
#define RFLR_IRQFLAGS_TXDONE                        0x08 
#define RFLR_IRQFLAGS_CADDONE                       0x04 
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02 
#define RFLR_IRQFLAGS_CADDETECTED                   0x01 



/*!
 * RegFifoRxNbBytes (Read Only)    //
 */

 
 /*!
 * RegRxHeaderCntValueMsb (Read Only)    //
 */
 
 
  /*!
 * RegRxHeaderCntValueLsb (Read Only)    //
 */
 
 
/*!
 * RegRxPacketCntValueMsb (Read Only)    //
 */
 
 
 /*!
 * RegRxPacketCntValueLsb (Read Only)    //
 */
 
 
 /*!
 * RegModemStat (Read Only)    //
 */
#define RFLR_MODEMSTAT_RX_CR_MASK                   0x1F 
#define RFLR_MODEMSTAT_MODEM_STATUS_MASK            0xE0 
 
/*!
 * RegPktSnrValue (Read Only)    //
 */

 
 /*!
 * RegPktRssiValue (Read Only)    //
 */
 
 
/*!
 * RegRssiValue (Read Only)    //
 */

 
 /*!
 * RegModemConfig1
 */
#define RFLR_MODEMCONFIG1_BW_MASK                   0x0F 

#define RFLR_MODEMCONFIG1_BW_7_81_KHZ               0x00 
#define RFLR_MODEMCONFIG1_BW_10_41_KHZ              0x10 
#define RFLR_MODEMCONFIG1_BW_15_62_KHZ              0x20 
#define RFLR_MODEMCONFIG1_BW_20_83_KHZ              0x30 
#define RFLR_MODEMCONFIG1_BW_31_25_KHZ              0x40 
#define RFLR_MODEMCONFIG1_BW_41_66_KHZ              0x50 
#define RFLR_MODEMCONFIG1_BW_62_50_KHZ              0x60 
#define RFLR_MODEMCONFIG1_BW_125_KHZ                0x70 // Default
#define RFLR_MODEMCONFIG1_BW_250_KHZ                0x80 
#define RFLR_MODEMCONFIG1_BW_500_KHZ                0x90 
                                                    
#define RFLR_MODEMCONFIG1_CODINGRATE_MASK           0xF1 	//1111 0001
#define RFLR_MODEMCONFIG1_CODINGRATE_4_5            0x02
#define RFLR_MODEMCONFIG1_CODINGRATE_4_6            0x04 	//缺省值
#define RFLR_MODEMCONFIG1_CODINGRATE_4_7            0x06 
#define RFLR_MODEMCONFIG1_CODINGRATE_4_8            0x08 
                                                    
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK       0xFE	//1111 1110
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_ON         0x01	//0000 0001 
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF        0x00 	//缺省值

 /*!
 * RegModemConfig2
 */
#define RFLR_MODEMCONFIG2_SF_MASK                   0x0F 
#define RFLR_MODEMCONFIG2_SF_6                      0x60 
#define RFLR_MODEMCONFIG2_SF_7                      0x70 	//缺省值
#define RFLR_MODEMCONFIG2_SF_8                      0x80 
#define RFLR_MODEMCONFIG2_SF_9                      0x90 
#define RFLR_MODEMCONFIG2_SF_10                     0xA0 
#define RFLR_MODEMCONFIG2_SF_11                     0xB0 
#define RFLR_MODEMCONFIG2_SF_12                     0xC0 

#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_MASK     0xF7 
#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_ON       0x08 
#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_OFF      0x00 

#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK         0xFB 
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON           0x04 
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_OFF          0x00 // Default
 
#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK       0xFC 
#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB            0x00 // Default
                                                    
                                                    
/*!                                                 
 * RegHopChannel (Read Only)                        
 */                                                 
#define RFLR_HOPCHANNEL_PLL_LOCK_TIMEOUT_MASK       0x7F 
#define RFLR_HOPCHANNEL_PLL_LOCK_FAIL               0x80 
#define RFLR_HOPCHANNEL_PLL_LOCK_SUCCEED            0x00 // Default
                                                    
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_MASK          0xBF
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_ON            0x40
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_OFF           0x00 // Default

#define RFLR_HOPCHANNEL_CHANNEL_MASK                0x3F 


/*!
 * RegSymbTimeoutLsb
 */
#define RFLR_SYMBTIMEOUTLSB_SYMBTIMEOUT             0x64 // Default

/*!
 * RegPreambleLengthMsb
 */
#define RFLR_PREAMBLELENGTHMSB                      0x00 // Default

/*!
 * RegPreambleLengthLsb
 */
#define RFLR_PREAMBLELENGTHLSB                      0x08 // Default

/*!
 * RegPayloadLength
 */
#define RFLR_PAYLOADLENGTH                          0x0E // Default

/*!
 * RegPayloadMaxLength
 */
#define RFLR_PAYLOADMAXLENGTH                       0xFF // Default

/*!
 * RegHopPeriod
 */
#define RFLR_HOPPERIOD_FREQFOPPINGPERIOD            0x00 // Default


/*!
 * RegDioMapping1
 */
#define RFLR_DIOMAPPING1_DIO0_MASK                  0x3F	//0011 1111
#define RFLR_DIOMAPPING1_DIO0_00                    0x00  	//0000 0000 (缺省值)
#define RFLR_DIOMAPPING1_DIO0_01                    0x40	//0100 0000
#define RFLR_DIOMAPPING1_DIO0_10                    0x80	//1000 0000
#define RFLR_DIOMAPPING1_DIO0_11                    0xC0	//1100 0000

#define RFLR_DIOMAPPING1_DIO1_MASK                  0xCF
#define RFLR_DIOMAPPING1_DIO1_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO1_01                    0x10
#define RFLR_DIOMAPPING1_DIO1_10                    0x20
#define RFLR_DIOMAPPING1_DIO1_11                    0x30

#define RFLR_DIOMAPPING1_DIO2_MASK                  0xF3
#define RFLR_DIOMAPPING1_DIO2_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO2_01                    0x04
#define RFLR_DIOMAPPING1_DIO2_10                    0x08
#define RFLR_DIOMAPPING1_DIO2_11                    0x0C

#define RFLR_DIOMAPPING1_DIO3_MASK                  0xFC
#define RFLR_DIOMAPPING1_DIO3_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO3_01                    0x01
#define RFLR_DIOMAPPING1_DIO3_10                    0x02
#define RFLR_DIOMAPPING1_DIO3_11                    0x03

/*!
 * RegDioMapping2
 */
#define RFLR_DIOMAPPING2_DIO4_MASK                  0x3F
#define RFLR_DIOMAPPING2_DIO4_00                    0x00  // Default
#define RFLR_DIOMAPPING2_DIO4_01                    0x40
#define RFLR_DIOMAPPING2_DIO4_10                    0x80
#define RFLR_DIOMAPPING2_DIO4_11                    0xC0

#define RFLR_DIOMAPPING2_DIO5_MASK                  0xCF
#define RFLR_DIOMAPPING2_DIO5_00                    0x00  // Default
#define RFLR_DIOMAPPING2_DIO5_01                    0x10
#define RFLR_DIOMAPPING2_DIO5_10                    0x20
#define RFLR_DIOMAPPING2_DIO5_11                    0x30

#define RFLR_DIOMAPPING2_MAP_MASK                   0xFE
#define RFLR_DIOMAPPING2_MAP_PREAMBLEDETECT         0x01
#define RFLR_DIOMAPPING2_MAP_RSSI                   0x00  // Default

/*!
 * RegVersion (Read Only)
 */

/*!
 * RegAgcRef
 */

/*!
 * RegAgcThresh1
 */

/*!
 * RegAgcThresh2
 */

/*!
 * RegAgcThresh3
 */
 
/*!
 * RegFifoRxByteAddr (Read Only)
 */
 
/*!
 * RegPllHop
 */
#define RFLR_PLLHOP_FASTHOP_MASK                    0x7F
#define RFLR_PLLHOP_FASTHOP_ON                      0x80
#define RFLR_PLLHOP_FASTHOP_OFF                     0x00 // Default

/*!
 * RegTcxo
 */
#define RFLR_TCXO_TCXOINPUT_MASK                    0xEF
#define RFLR_TCXO_TCXOINPUT_ON                      0x10
#define RFLR_TCXO_TCXOINPUT_OFF                     0x00  // Default

/*!
 * RegPaDac
 */
#define RFLR_PADAC_20DBM_MASK                       0xF8
#define RFLR_PADAC_20DBM_ON                         0x07
#define RFLR_PADAC_20DBM_OFF                        0x04  // Default

/*!
 * RegPll
 */
#define RFLR_PLL_BANDWIDTH_MASK                     0x3F
#define RFLR_PLL_BANDWIDTH_75                       0x00
#define RFLR_PLL_BANDWIDTH_150                      0x40
#define RFLR_PLL_BANDWIDTH_225                      0x80
#define RFLR_PLL_BANDWIDTH_300                      0xC0  // Default

/*!
 * RegPllLowPn
 */
#define RFLR_PLLLOWPN_BANDWIDTH_MASK                0x3F
#define RFLR_PLLLOWPN_BANDWIDTH_75                  0x00
#define RFLR_PLLLOWPN_BANDWIDTH_150                 0x40
#define RFLR_PLLLOWPN_BANDWIDTH_225                 0x80
#define RFLR_PLLLOWPN_BANDWIDTH_300                 0xC0  // Default

/*!
 * RegModemConfig3
 */
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK  0xF7	//1111 0111(bit3)
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON    0x08	//0000 1000(开启) 
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF   0x00 	//0000 0000(关闭)
#define RFLR_MODEMCONFIG3_AGCAUTO_MASK              0xFB 	//1111 1011(bit2)
#define RFLR_MODEMCONFIG3_AGCAUTO_ON                0x04 	//
#define RFLR_MODEMCONFIG3_AGCAUTO_OFF               0x00 

/*!
 * RegFormerTemp
 */
typedef struct sSX1276LR
{
    u8 RegFifo;                                // 0x00 
    // Common settings
    u8 RegOpMode;                              // 0x01 
    u8 RegRes02;                               // 0x02 
    u8 RegRes03;                               // 0x03 
    u8 RegBandSetting;                         // 0x04 
    u8 RegRes05;                               // 0x05 
    u8 RegFrfMsb;                              // 0x06 
    u8 RegFrfMid;                              // 0x07 
    u8 RegFrfLsb;                              // 0x08 
    // Tx settings
    u8 RegPaConfig;                            // 0x09 
    u8 RegPaRamp;                              // 0x0A 
    u8 RegOcp;                                 // 0x0B 
    // Rx settings
    u8 RegLna;                                 // 0x0C -- LAN设置
    // LoRa registers
    u8 RegFifoAddrPtr;                         // 0x0D 
    u8 RegFifoTxBaseAddr;                      // 0x0E 
    u8 RegFifoRxBaseAddr;                      // 0x0F 
    u8 RegFifoRxCurrentAddr;                   // 0x10		接收到最后一个数据包的起始地址(数据缓冲区中)
    u8 RegIrqFlagsMask;                        // 0x11 
    u8 RegIrqFlags;                            // 0x12 
    u8 RegNbRxBytes;                           // 0x13 
    u8 RegRxHeaderCntValueMsb;                 // 0x14 
    u8 RegRxHeaderCntValueLsb;                 // 0x15 
    u8 RegRxPacketCntValueMsb;                 // 0x16 
    u8 RegRxPacketCntValueLsb;                 // 0x17 
    u8 RegModemStat;                           // 0x18 
    u8 RegPktSnrValue;                         // 0x19 
    u8 RegPktRssiValue;                        // 0x1A 
    u8 RegRssiValue;                           // 0x1B 
    u8 RegHopChannel;                          // 0x1C 
    u8 RegModemConfig1;                        // 0x1D 
    u8 RegModemConfig2;                        // 0x1E 
    u8 RegSymbTimeoutLsb;                      // 0x1F 
    u8 RegPreambleMsb;                         // 0x20 
    u8 RegPreambleLsb;                         // 0x21 
    u8 RegPayloadLength;                       // 0x22		负载字节长度 
    u8 RegMaxPayloadLength;                    // 0x23 
    u8 RegHopPeriod;                           // 0x24 
    u8 RegFifoRxByteAddr;                      // 0x25
    u8 RegModemConfig3;                        // 0x26
    u8 RegTestReserved27[0x30 - 0x27];         // 0x27-0x30
    u8 RegTestReserved31;                      // 0x31
    u8 RegTestReserved32[0x40 - 0x32];         // 0x32-0x40
    // I/O settings                
    u8 RegDioMapping1;                         // 0x40 
    u8 RegDioMapping2;                         // 0x41 
    // Version
    u8 RegVersion;                             // 0x42
    // Additional settings
    u8 RegAgcRef;                              // 0x43
    u8 RegAgcThresh1;                          // 0x44
    u8 RegAgcThresh2;                          // 0x45
    u8 RegAgcThresh3;                          // 0x46
    // Test
    u8 RegTestReserved47[0x4B - 0x47];         // 0x47-0x4A
    // Additional settings
    u8 RegPllHop;                              // 0x4B
    u8 RegTestReserved4C;                      // 0x4C
    u8 RegPaDac;                               // 0x4D
    // Test
    u8 RegTestReserved4E[0x58-0x4E];           // 0x4E-0x57
    // Additional settings
    u8 RegTcxo;                                // 0x58
    // Test
    u8 RegTestReserved59;                      // 0x59
    // Test
    u8 RegTestReserved5B;                      // 0x5B
    // Additional settings
    u8 RegPll;                                 // 0x5C
    // Test
    u8 RegTestReserved5D;                      // 0x5D
    // Additional settings
    u8 RegPllLowPn;                            // 0x5E
    // Test
    u8 RegTestReserved5F[0x6C - 0x5F];         // 0x5F-0x6B
    // Additional settings
    u8 RegFormerTemp;                          // 0x6C
    // Test
    u8 RegTestReserved6D[0x71 - 0x6D];         // 0x6D-0x70
}tSX1276LR;


extern tSX1276LR* SX1276LR;

void SX1276LoRaInit( void );
void SX1276LoRaSetDefaults( void );
void SX1276LoRaSetLoRaOn( bool enable );
void SX1276LoRaSetOpMode( u8 opMode );
u8 SX1276LoRaGetOpMode( void );
u8 SX1276LoRaReadRxGain( void );
double SX1276LoRaReadRssi( void );
u8 SX1276LoRaGetPacketRxGain( void );
int8_t SX1276LoRaGetPacketSnr( void );
double SX1276LoRaGetPacketRssi( void );
void SX1276LoRaStartRx( void );
void SX1276LoRaGetRxPacket( void *buffer, uint16_t *size );
void SX1276LoRaSetTxPacket( const void *buffer, uint16_t size );
u8 SX1276LoRaGetRFState( void );
void SX1276LoRaSetRFState( u8 state );
uint32_t SX1276LoRaProcess( void );

/**************************************增加的函数**************************************/
int8_t 	GetLoRaSNR (void);
double 	GetPackLoRaRSSI(void);
void 	LoRaRxStateEnter (void);
void 	LoRaRxDataRead (u8 *pbuf, u8 *size );
u8 		LoRaTxData (u8 *pbuf, u8 size ,u8 *pcrcbuf, u8 crcflag);
void 	LoRaTxPower (u8 pwr);
void 	LoRaFreqSet (uint32_t freq );
/***************************************************************************************/

#endif




