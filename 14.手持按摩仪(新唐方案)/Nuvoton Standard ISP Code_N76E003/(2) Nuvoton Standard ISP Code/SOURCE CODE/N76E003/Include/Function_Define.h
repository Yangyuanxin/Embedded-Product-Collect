#include <intrins.h>

#define nop _nop_();


//16 --> 8 x 2
#define HIBYTE(v1)              ((UINT8)((v1)>>8))                      //v1 is UINT16
#define LOBYTE(v1)              ((UINT8)((v1)&0xFF))
//8 x 2 --> 16
#define MAKEWORD(v1,v2)         ((((UINT16)(v1))<<8)+(UINT16)(v2))      //v1,v2 is UINT8
//8 x 4 --> 32
#define MAKELONG(v1,v2,v3,v4)   (UINT32)((v1<<32)+(v2<<16)+(v3<<8)+v4)  //v1,v2,v3,v4 is UINT8
//32 --> 16 x 2
#define YBYTE1(v1)              ((UINT16)((v1)>>16))                    //v1 is UINT32
#define YBYTE0(v1)              ((UINT16)((v1)&0xFFFF))
//32 --> 8 x 4
#define TBYTE3(v1)              ((UINT8)((v1)>>24))                     //v1 is UINT32
#define TBYTE2(v1)              ((UINT8)((v1)>>16))
#define TBYTE1(v1)              ((UINT8)((v1)>>8)) 
#define TBYTE0(v1)              ((UINT8)((v1)&0xFF))

#define SET_BIT0        0x01
#define SET_BIT1        0x02
#define SET_BIT2        0x04
#define SET_BIT3        0x08
#define SET_BIT4        0x10
#define SET_BIT5        0x20
#define SET_BIT6        0x40
#define SET_BIT7        0x80
#define SET_BIT8        0x0100
#define SET_BIT9        0x0200
#define SET_BIT10       0x0400
#define SET_BIT11       0x0800
#define SET_BIT12       0x1000
#define SET_BIT13       0x2000
#define SET_BIT14       0x4000
#define SET_BIT15       0x8000

#define CLR_BIT0        0xFE
#define CLR_BIT1        0xFD
#define CLR_BIT2        0xFB
#define CLR_BIT3        0xF7
#define CLR_BIT4        0xEF
#define CLR_BIT5        0xDF
#define CLR_BIT6        0xBF
#define CLR_BIT7        0x7F

#define CLR_BIT8        0xFEFF
#define CLR_BIT9        0xFDFF
#define CLR_BIT10       0xFBFF
#define CLR_BIT11       0xF7FF
#define CLR_BIT12       0xEFFF
#define CLR_BIT13       0xDFFF
#define CLR_BIT14       0xBFFF
#define CLR_BIT15       0x7FFF

#define FAIL            1
#define PASS            0

/*****************************************************************************************
* For GPIO setting 
*****************************************************************************************/
//------------------- Define Port as Quasi mode  -------------------
#define P00_Quasi_Mode				P0M1&=~SET_BIT0;P0M2&=~SET_BIT0
#define P01_Quasi_Mode				P0M1&=~SET_BIT1;P0M2&=~SET_BIT1
#define P02_Quasi_Mode				P0M1&=~SET_BIT2;P0M2&=~SET_BIT2
#define P03_Quasi_Mode				P0M1&=~SET_BIT3;P0M2&=~SET_BIT3
#define P04_Quasi_Mode				P0M1&=~SET_BIT4;P0M2&=~SET_BIT4
#define P05_Quasi_Mode				P0M1&=~SET_BIT5;P0M2&=~SET_BIT5
#define P06_Quasi_Mode				P0M1&=~SET_BIT6;P0M2&=~SET_BIT6
#define P07_Quasi_Mode				P0M1&=~SET_BIT7;P0M2&=~SET_BIT7
#define P10_Quasi_Mode				P1M1&=~SET_BIT0;P1M2&=~SET_BIT0
#define P11_Quasi_Mode				P1M1&=~SET_BIT1;P1M2&=~SET_BIT1
#define P12_Quasi_Mode				P1M1&=~SET_BIT2;P1M2&=~SET_BIT2
#define P13_Quasi_Mode				P1M1&=~SET_BIT3;P1M2&=~SET_BIT3
#define P14_Quasi_Mode				P1M1&=~SET_BIT4;P1M2&=~SET_BIT4
#define P15_Quasi_Mode				P1M1&=~SET_BIT5;P1M2&=~SET_BIT5
#define P16_Quasi_Mode				P1M1&=~SET_BIT6;P1M2&=~SET_BIT6
#define P17_Quasi_Mode				P1M1&=~SET_BIT7;P1M2&=~SET_BIT7
#define P30_Quasi_Mode				P3M1&=~SET_BIT0;P3M2&=~SET_BIT0
//------------------- Define Port as Push Pull mode -------------------
#define P00_PushPull_Mode			P0M1&=~SET_BIT0;P0M2|=SET_BIT0
#define P01_PushPull_Mode			P0M1&=~SET_BIT1;P0M2|=SET_BIT1
#define P02_PushPull_Mode			P0M1&=~SET_BIT2;P0M2|=SET_BIT2
#define P03_PushPull_Mode			P0M1&=~SET_BIT3;P0M2|=SET_BIT3
#define P04_PushPull_Mode			P0M1&=~SET_BIT4;P0M2|=SET_BIT4
#define P05_PushPull_Mode			P0M1&=~SET_BIT5;P0M2|=SET_BIT5
#define P06_PushPull_Mode			P0M1&=~SET_BIT6;P0M2|=SET_BIT6
#define P07_PushPull_Mode			P0M1&=~SET_BIT7;P0M2|=SET_BIT7
#define P10_PushPull_Mode			P1M1&=~SET_BIT0;P1M2|=SET_BIT0
#define P11_PushPull_Mode			P1M1&=~SET_BIT1;P1M2|=SET_BIT1
#define P12_PushPull_Mode			P1M1&=~SET_BIT2;P1M2|=SET_BIT2
#define P13_PushPull_Mode			P1M1&=~SET_BIT3;P1M2|=SET_BIT3
#define P14_PushPull_Mode			P1M1&=~SET_BIT4;P1M2|=SET_BIT4
#define P15_PushPull_Mode			P1M1&=~SET_BIT5;P1M2|=SET_BIT5
#define P16_PushPull_Mode			P1M1&=~SET_BIT6;P1M2|=SET_BIT6
#define P17_PushPull_Mode			P1M1&=~SET_BIT7;P1M2|=SET_BIT7
#define P30_PushPull_Mode			P3M1&=~SET_BIT0;P3M2|=SET_BIT0
#define GPIO1_PushPull_Mode		P1M1&=~SET_BIT0;P1M2|=SET_BIT0
//------------------- Define Port as Input Only mode -------------------
#define P00_Input_Mode				P0M1|=SET_BIT0;P0M2&=~SET_BIT0
#define P01_Input_Mode				P0M1|=SET_BIT1;P0M2&=~SET_BIT1
#define P02_Input_Mode				P0M1|=SET_BIT2;P0M2&=~SET_BIT2
#define P03_Input_Mode				P0M1|=SET_BIT3;P0M2&=~SET_BIT3
#define P04_Input_Mode				P0M1|=SET_BIT4;P0M2&=~SET_BIT4
#define P05_Input_Mode				P0M1|=SET_BIT5;P0M2&=~SET_BIT5
#define P06_Input_Mode				P0M1|=SET_BIT6;P0M2&=~SET_BIT6
#define P07_Input_Mode				P0M1|=SET_BIT7;P0M2&=~SET_BIT7
#define P10_Input_Mode				P1M1|=SET_BIT0;P1M2&=~SET_BIT0
#define P11_Input_Mode				P1M1|=SET_BIT1;P1M2&=~SET_BIT1
#define P12_Input_Mode				P1M1|=SET_BIT2;P1M2&=~SET_BIT2
#define P13_Input_Mode				P1M1|=SET_BIT3;P1M2&=~SET_BIT3
#define P14_Input_Mode				P1M1|=SET_BIT4;P1M2&=~SET_BIT4
#define P15_Input_Mode				P1M1|=SET_BIT5;P1M2&=~SET_BIT5
#define P16_Input_Mode				P1M1|=SET_BIT6;P1M2&=~SET_BIT6
#define P17_Input_Mode				P1M1|=SET_BIT7;P1M2&=~SET_BIT7
#define P30_Input_Mode				P3M1|=SET_BIT0;P3M2&=~SET_BIT0
//-------------------Define Port as Open Drain mode -------------------
#define P00_OpenDrain_Mode		P0M1|=SET_BIT0;P0M2|=SET_BIT0
#define P01_OpenDrain_Mode		P0M1|=SET_BIT1;P0M2|=SET_BIT1
#define P02_OpenDrain_Mode		P0M1|=SET_BIT2;P0M2|=SET_BIT2
#define P03_OpenDrain_Mode		P0M1|=SET_BIT3;P0M2|=SET_BIT3
#define P04_OpenDrain_Mode		P0M1|=SET_BIT4;P0M2|=SET_BIT4
#define P05_OpenDrain_Mode		P0M1|=SET_BIT5;P0M2|=SET_BIT5
#define P06_OpenDrain_Mode		P0M1|=SET_BIT6;P0M2|=SET_BIT6
#define P07_OpenDrain_Mode		P0M1|=SET_BIT7;P0M2|=SET_BIT7
#define P10_OpenDrain_Mode		P1M1|=SET_BIT0;P1M2|=SET_BIT0
#define P11_OpenDrain_Mode		P1M1|=SET_BIT1;P1M2|=SET_BIT1
#define P12_OpenDrain_Mode		P1M1|=SET_BIT2;P1M2|=SET_BIT2
#define P13_OpenDrain_Mode		P1M1|=SET_BIT3;P1M2|=SET_BIT3
#define P14_OpenDrain_Mode		P1M1|=SET_BIT4;P1M2|=SET_BIT4
#define P15_OpenDrain_Mode		P1M1|=SET_BIT5;P1M2|=SET_BIT5
#define P16_OpenDrain_Mode		P1M1|=SET_BIT6;P1M2|=SET_BIT6
#define P17_OpenDrain_Mode		P1M1|=SET_BIT7;P1M2|=SET_BIT7
#define P30_OpenDrain_Mode		P3M1|=SET_BIT0;P3M2|=SET_BIT0
//--------- Define all port as quasi mode ---------
#define Set_All_GPIO_Quasi_Mode			P0M1=0;P0M1=0;P1M1=0;P1M2=0;P3M1=0;P3M2=0



/****************************************************************************
   Enable INT port 0~3
***************************************************************************/
#define 	Enable_INT_Port0					PICON &= 0xFB;
#define		Enable_INT_Port1					PICON |= 0x01;
#define		Enable_INT_Port2					PICON |= 0x02;
#define		Enable_INT_Port3					PICON |= 0x03;
/*****************************************************************************
 Enable each bit low level trig mode
*****************************************************************************/
#define		Enable_BIT7_LowLevel_Trig			PICON&=0x7F;PINEN|=0x80;PIPEN&=0x7F
#define		Enable_BIT6_LowLevel_Trig			PICON&=0x7F;PINEN|=0x40;PIPEN&=0xBF
#define		Enable_BIT5_LowLevel_Trig			PICON&=0xBF;PINEN|=0x20;PIPEN&=0xDF
#define		Enable_BIT4_LowLevel_Trig			PICON&=0xBF;PINEN|=0x10;PIPEN&=0xEF
#define		Enable_BIT3_LowLevel_Trig			PICON&=0xDF;PINEN|=0x08;PIPEN&=0xF7
#define		Enable_BIT2_LowLevel_Trig			PICON&=0xEF;PINEN|=0x04;PIPEN&=0xFB
#define		Enable_BIT1_LowLevel_Trig			PICON&=0xF7;PINEN|=0x02;PIPEN&=0xFD
#define		Enable_BIT0_LowLevel_Trig			PICON&=0xFD;PINEN|=0x01;PIPEN&=0xFE
/*****************************************************************************
 Enable each bit high level trig mode
*****************************************************************************/
#define		Enable_BIT7_HighLevel_Trig			PICON&=0x7F;PINEN&=0x7F;PIPEN|=0x80
#define		Enable_BIT6_HighLevel_Trig			PICON&=0x7F;PINEN&=0xBF;PIPEN|=0x40
#define		Enable_BIT5_HighLevel_Trig			PICON&=0xBF;PINEN&=0xDF;PIPEN|=0x20
#define		Enable_BIT4_HighLevel_Trig			PICON&=0xBF;PINEN&=0xEF;PIPEN|=0x10
#define		Enable_BIT3_HighLevel_Trig			PICON&=0xDF;PINEN&=0xF7;PIPEN|=0x08
#define		Enable_BIT2_HighLevel_Trig			PICON&=0xEF;PINEN&=0xFB;PIPEN|=0x04
#define		Enable_BIT1_HighLevel_Trig			PICON&=0xF7;PINEN&=0xFD;PIPEN|=0x02
#define		Enable_BIT0_HighLevel_Trig			PICON&=0xFD;PINEN&=0xFE;PIPEN|=0x01
/*****************************************************************************
 Enable each bit falling edge trig mode
*****************************************************************************/
#define		Enable_BIT7_FallEdge_Trig			PICON|=0x80;PINEN|=0x80;PIPEN&=0x7F
#define		Enable_BIT6_FallEdge_Trig			PICON|=0x80;PINEN|=0x40;PIPEN&=0xBF
#define		Enable_BIT5_FallEdge_Trig			PICON|=0x40;PINEN|=0x20;PIPEN&=0xDF
#define		Enable_BIT4_FallEdge_Trig			PICON|=0x40;PINEN|=0x10;PIPEN&=0xEF
#define		Enable_BIT3_FallEdge_Trig			PICON|=0x20;PINEN|=0x08;PIPEN&=0xF7
#define		Enable_BIT2_FallEdge_Trig			PICON|=0x10;PINEN|=0x04;PIPEN&=0xFB
#define		Enable_BIT1_FallEdge_Trig			PICON|=0x08;PINEN|=0x02;PIPEN&=0xFD
#define		Enable_BIT0_FallEdge_Trig			PICON|=0x04;PINEN|=0x01;PIPEN&=0xFE
/*****************************************************************************
 Enable each bit rasing edge trig mode
*****************************************************************************/
#define		Enable_BIT7_RasingEdge_Trig			PICON|=0x80;PINEN&=0x7F;PIPEN|=0x80
#define		Enable_BIT6_RasingEdge_Trig			PICON|=0x80;PINEN&=0xBF;PIPEN|=0x40
#define		Enable_BIT5_RasingEdge_Trig			PICON|=0x40;PINEN&=0xDF;PIPEN|=0x20
#define		Enable_BIT4_RasingEdge_Trig			PICON|=0x40;PINEN&=0xEF;PIPEN|=0x10
#define		Enable_BIT3_RasingEdge_Trig			PICON|=0x20;PINEN&=0xF7;PIPEN|=0x08
#define		Enable_BIT2_RasingEdge_Trig			PICON|=0x10;PINEN&=0xFB;PIPEN|=0x04
#define		Enable_BIT1_RasingEdge_Trig			PICON|=0x08;PINEN|=0xFD;PIPEN&=0x02
#define		Enable_BIT0_RasingEdge_Trig			PICON|=0x04;PINEN|=0xFE;PIPEN&=0x01


/*****************************************************************************************
* For TIMER setting is base on " option -> C51 -> Preprocesser Symbols -> Define "
*****************************************************************************************/
#ifdef FOSC_110592		// if Fsys = 11.0592MHz 
    #define TIMER_DIV12_VALUE_10us			65536-9			//9*12/11.0592 = 10 uS,  				// Timer divider = 12 for TM0/TM1
    #define TIMER_DIV12_VALUE_1ms				65536-923		//923*12/11.0592 = 1 mS					// Timer divider = 12
		#define	TIMER_DIV12_VALUE_10ms			65536-9216	//18432*12/22118400 = 10 ms			// Timer divider = 12
    #define TIMER_DIV4_VALUE_10us				65536-28		//28*4/11.0592 = 10 uS					// Timer divider = 4	for TM2/TM3
    #define TIMER_DIV4_VALUE_1ms				65536-2765	//2765*4/11.0592 = 1 mS					// Timer divider = 4
		#define TIMER_DIV4_VALUE_100us			65536-277		//553*4/22118400 = 100 us				// Timer divider = 4
		#define TIMER_DIV4_VALUE_200us			65536-553		//1106*4/22118400 = 200 us			// Timer divider = 4
		#define TIMER_DIV4_VALUE_500us			65536-1383	//2765*4/22118400 = 500 us			// Timer divider = 4		
		#define TIMER_DIV16_VALUE_10ms			65536-6912	//1500*16/22118400 = 10 ms			// Timer divider = 16
		#define TIMER_DIV64_VALUE_30ms			65536-5184	//10368*64/22118400 = 30 ms			// Timer divider = 64
		#define	TIMER_DIV128_VALUE_100ms		65536-8640	//17280*128/22118400 = 100 ms		// Timer divider = 128
		#define	TIMER_DIV128_VALUE_200ms		65536-17280	//34560*128/22118400 = 200 ms		// Timer divider = 128
		#define TIMER_DIV256_VALUE_500ms		65536-21600	//43200*256/22118400 = 500 ms 	// Timer divider = 256
		#define TIMER_DIV512_VALUE_1s				65536-21600	//43200*512/22118400 = 1 s			// Timer divider = 512
#endif
#ifdef FOSC_160000		// if Fsys = 16MHz 
		#define TIMER_DIV12_VALUE_10us			65536-13		//13*12/16000000 = 10 uS,  			// Timer divider = 12 for TM0/TM1
		#define TIMER_DIV12_VALUE_1ms				65536-1334	//2000*12/16000000 = 1 mS, 			// Timer divider = 12 
		#define TIMER_DIV12_VALUE_10ms			65536-13334	//2000*12/16000000 = 10 mS 			// Timer divider = 12 
		#define TIMER_DIV4_VALUE_10us				65536-40		//40*4/16000000 = 10 uS,    		// Timer divider = 4	for TM2/TM3
		#define TIMER_DIV4_VALUE_100us			65536-400		//400*4/16000000 = 100 us				// Timer divider = 4
		#define TIMER_DIV4_VALUE_200us			65536-800		//800*4/16000000 = 200 us				// Timer divider = 4
		#define TIMER_DIV4_VALUE_500us			65536-2000	//3000*4/16000000 = 500 us			// Timer divider = 4
    #define TIMER_DIV4_VALUE_1ms				65536-4000	//4000*4/16000000 = 1 mS,   		// Timer divider = 4
		#define TIMER_DIV16_VALUE_10ms			65536-10000	//10000*16/16000000 = 10 ms			// Timer	divider = 16
		#define TIMER_DIV64_VALUE_30ms			65536-7500	//7500*64/16000000 = 30 ms			// Timer divider = 64
		#define	TIMER_DIV128_VALUE_100ms		65536-2500	//2500*128/16000000 = 200 ms		// Timer divider = 128
		#define	TIMER_DIV128_VALUE_200ms		65536-25000	//25000*128/16000000 = 200 ms		// Timer divider = 128
		#define TIMER_DIV256_VALUE_500ms		65536-31250	//31250*256/16000000 = 500 ms 	// Timer divider = 256
		#define	TIMER_DIV512_VALUE_1s				65536-31250	//31250*512/16000000 = 1 s.  		// Timer Divider = 512
#endif
#ifdef FOSC_184320		// if Fsys = 18.432MHz 
    #define TIMER_DIV12_VALUE_10us			65536-15    //15*12/18.432 = 10 uS,  Timer Clock = Fsys/12
    #define TIMER_DIV12_VALUE_1ms				65536-1536  //1536*12/18.432 = 1 mS, Timer Clock = Fsys/12
    #define TIMER_DIV4_VALUE_10us				65536-46    //46*4/18.432 = 10 uS,   Timer Clock = Fsys/4
    #define TIMER_DIV4_VALUE_1ms				65536-4608  //4608*4/18.432 = 1 mS,  Timer Clock = Fsys/4
#endif
#ifdef FOSC_200000		// if Fsys = 20 MHz
    #define TIMER_DIV12_VALUE_10us			65536-17 	 //17*12/20000000 = 10 uS,  Timer Clock = Fsys/12
    #define TIMER_DIV12_VALUE_1ms				65536-1667  	//1667*12/20000000 = 1 mS, Timer Clock = Fsys/12
    #define TIMER_DIV4_VALUE_10us				65536-50    	//50*4/20000000 = 10 uS,    Timer Clock = Fsys/4
    #define TIMER_DIV4_VALUE_1ms				65536-5000  	//5000*4/20000000 = 1 mS,   Timer Clock = Fsys/4
#endif
#ifdef FOSC_221184		// if Fsys = 22.1184 MHz 
		#define TIMER_DIV12_VALUE_10us			65536-18   			//18*12/22118400 = 10 uS,  			// Timer divider = 12
		#define TIMER_DIV12_VALUE_1ms				65536-1843  		//1843*12/22118400 = 1 mS, 			// Timer divider = 12
		#define	TIMER_DIV12_VALUE_10ms			65536-18432			//18432*12/22118400 = 10 ms			// Timer divider = 12
		#define TIMER_DIV4_VALUE_10us				65536-56				//9*4/22118400 = 10 uS,    			// Timer divider = 4
		#define TIMER_DIV4_VALUE_1ms				65536-5530			//923*4/22118400 = 1 mS,   			// Timer divider = 4
		#define TIMER_DIV4_VALUE_100us			65536-553				//553*4/22118400 = 100 us				// Timer divider = 4
		#define TIMER_DIV4_VALUE_200us			65536-1106			//1106*4/22118400 = 200 us			// Timer divider = 4
		#define TIMER_DIV4_VALUE_500us			65536-2765			//2765*4/22118400 = 500 us			// Timer divider = 4		
		#define TIMER_DIV16_VALUE_10ms			65536-13824			//1500*16/22118400 = 10 ms			// Timer divider = 16
		#define TIMER_DIV64_VALUE_30ms			65536-10368			//10368*64/22118400 = 30 ms			// Timer divider = 64
		#define	TIMER_DIV128_VALUE_100ms		65536-17280			//17280*128/22118400 = 100 ms		// Timer divider = 128
		#define	TIMER_DIV128_VALUE_200ms		65536-34560			//34560*128/22118400 = 200 ms		// Timer divider = 128
		#define TIMER_DIV256_VALUE_500ms		65536-43200			//43200*256/22118400 = 500 ms 	// Timer divider = 256
		#define TIMER_DIV512_VALUE_1s				65536-43200			//43200*512/22118400 = 1 s			// Timer divider = 512
#endif
#ifdef FOSC_240000		// if Fsys = 20 MHz
		#define TIMER_DIV12_VALUE_10us			65536-20				//20*12/24000000 = 10 uS,  			// Timer divider = 12
    #define TIMER_DIV12_VALUE_1ms				65536-2000			//2000*12/24000000 = 1 mS, 			// Timer divider = 12
		#define TIMER_DIV12_VALUE_10ms			65536-20000			//2000*12/24000000 = 10 mS 			// Timer divider = 12
		#define TIMER_DIV4_VALUE_10us				65536-60				//60*4/24000000 = 10 uS,    		// Timer divider = 4
		#define TIMER_DIV4_VALUE_100us			65536-600				//600*4/24000000 = 100 us				// Timer divider = 4
		#define TIMER_DIV4_VALUE_200us			65536-1200			//1200*4/24000000 = 200 us			// Timer divider = 4
		#define TIMER_DIV4_VALUE_500us			65536-3000			//3000*4/24000000 = 500 us			// Timer divider = 4
    #define TIMER_DIV4_VALUE_1ms				65536-6000			//6000*4/24000000 = 1 mS,   		// Timer divider = 4
		#define TIMER_DIV16_VALUE_10ms			65536-15000			//15000*16/24000000 = 10 ms			// Timer	divider = 16
		#define TIMER_DIV64_VALUE_30ms			65536-11250			//11250*64/24000000 = 30 ms			// Timer divider = 64
		#define	TIMER_DIV128_VALUE_100ms		65536-18750			//37500*128/24000000 = 200 ms		// Timer divider = 128
		#define	TIMER_DIV128_VALUE_200ms		65536-37500			//37500*128/24000000 = 200 ms		// Timer divider = 128
		#define TIMER_DIV256_VALUE_500ms		65536-46875			//46875*256/24000000 = 500 ms 	// Timer divider = 256
		#define	TIMER_DIV512_VALUE_1s				65536-46875			//46875*512/24000000 = 1 s.  		// Timer Divider = 512
#endif
//-------------------- Timer0 function define --------------------
#define		TIMER0_MODE0_ENABLE		TMOD&=0x0F
#define		TIMER0_MODE1_ENABLE		TMOD&=0x0F;TMOD|=0x10
#define		TIMER0_MODE2_ENABLE		TMOD&=0x0F;TMOD|=0x20
#define		TIMER0_MODE3_ENABLE		TMOD&=0x0F;TMOD|=0x3F
//-------------------- Timer1 function define --------------------
#define		TIMER1_MODE0_ENABLE		TMOD&=0xF0
#define		TIMER1_MODE1_ENABLE		TMOD&=0xF0;TMOD|=0x01
#define		TIMER1_MODE2_ENABLE		TMOD&=0xF0;TMOD|=0x02
#define		TIMER1_MODE3_ENABLE		TMOD&=0xF0;TMOD|=0xF3
//-------------------- Timer2 function define --------------------
#define TIMER2_DIV_4			T2MOD|=0x10;T2MOD&=0x9F
#define TIMER2_DIV_16			T2MOD|=0x20;T2MOD&=0xAF
#define TIMER2_DIV_32			T2MOD|=0x30;T2MOD&=0xBF
#define TIMER2_DIV_64			T2MOD|=0x40;T2MOD&=0xCF
#define TIMER2_DIV_128		T2MOD|=0x50;T2MOD&=0xDF
#define TIMER2_DIV_256		T2MOD|=0x60;T2MOD&=0xEF
#define TIMER2_DIV_512		T2MOD|=0x70
#define TIMER2_Auto_Reload_Delay_Mode				T2CON&=~SET_BIT0;T2MOD|=SET_BIT7;T2MOD|=SET_BIT3
#define TIMER2_Auto_Reload_Capture_Mode			T2CON&=~SET_BIT0;T2MOD|=SET_BIT7
#define	TIMER2_Compare_Capture_Mode					T2CON|=SET_BIT0;T2MOD&=~SET_BIT7;T2MOD|=SET_BIT2
//-------------------- Timer2 Capture define --------------------
//--- Falling Edge -----
#define IC0_P12_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0
#define	IC1_P11_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x01;
#define	IC2_P10_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x02;
#define	IC3_P00_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x03;
#define	IC3_P04_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x04;
#define	IC4_P01_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x05;
#define	IC5_P03_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x06;
#define	IC6_P05_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x07;
#define	IC7_P15_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x08;

#define IC0_P12_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F
#define	IC1_P11_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x10;
#define	IC2_P10_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x20;
#define	IC3_P00_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x30;
#define	IC3_P04_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x40;
#define	IC4_P01_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x50;
#define	IC5_P03_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x60;
#define	IC6_P05_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x70;
#define	IC7_P15_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x80;

#define IC0_P12_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0
#define	IC1_P11_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x10;
#define	IC2_P10_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x20;
#define	IC3_P00_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x30;
#define	IC3_P04_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x40;
#define	IC4_P01_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x50;
#define	IC5_P03_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x60;
#define	IC6_P05_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x70;
#define	IC7_P15_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x80;

//----- Rising edge ----
#define IC0_P12_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0
#define	IC1_P11_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x01;
#define	IC2_P10_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x02;
#define	IC3_P00_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x03;
#define	IC3_P04_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x04;
#define	IC4_P01_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x05;
#define	IC5_P03_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x06;
#define	IC6_P05_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x07;
#define	IC7_P15_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x08;

#define IC0_P12_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F
#define	IC1_P11_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x10;
#define	IC2_P10_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x20;
#define	IC3_P00_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x30;
#define	IC3_P04_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x40;
#define	IC4_P01_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x50;
#define	IC5_P03_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x60;
#define	IC6_P05_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x70;
#define	IC7_P15_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x80;

#define IC0_P12_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0
#define	IC1_P11_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x01;
#define	IC2_P10_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x02;
#define	IC3_P00_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x03;
#define	IC3_P04_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x04;
#define	IC4_P01_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x05;
#define	IC5_P03_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x06;
#define	IC6_P05_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x07;
#define	IC7_P15_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x08;

//-----BOTH  edge ----
#define IC0_P12_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0
#define	IC1_P11_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x01;
#define	IC2_P10_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x02;
#define	IC3_P00_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x03;
#define	IC3_P04_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x04;
#define	IC4_P01_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x05;
#define	IC5_P03_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x06;
#define	IC6_P05_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x07;
#define	IC7_P15_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x08;

#define IC0_P12_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F
#define	IC1_P11_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x10;
#define	IC2_P10_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x20;
#define	IC3_P00_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x30;
#define	IC3_P04_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x40;
#define	IC4_P01_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x50;
#define	IC5_P03_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x60;
#define	IC6_P05_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x70;
#define	IC7_P15_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x80;

#define IC0_P12_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0
#define	IC1_P11_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x01;
#define	IC2_P10_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x02;
#define	IC3_P00_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x03;
#define	IC3_P04_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x04;
#define	IC4_P01_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x05;
#define	IC5_P03_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x06;
#define	IC6_P05_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x07;
#define	IC7_P15_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x08;

#define TIMER2_IC2_DISABLE									CAPCON0&=~SET_BIT6			 
#define TIMER2_IC1_DISABLE									CAPCON0&=~SET_BIT5			
#define TIMER2_IC0_DISABLE									CAPCON0&=~SET_BIT4	



/*****************************************************************************************
* For PWM setting 
*****************************************************************************************/
//--------- PMW clock source select define ---------------------
#define		PWM_CLOCK_FSYS					CKCON&=0xBF
#define		PWM_CLOCK_TIMER1				CKCON|=0x40
//--------- PWM clock devide define ----------------------------
#define		PWM_CLOCK_DIV_2					PWMCON1|=0x01;PWMCON1&=0xF9
#define		PWM_CLOCK_DIV_4					PWMCON1|=0x02;PWMCON1&=0xFA
#define		PWM_CLOCK_DIV_8					PWMCON1|=0x03;PWMCON1&=0xFB
#define		PWM_CLOCK_DIV_16				PWMCON1|=0x04;PWMCON1&=0xFC
#define		PWM_CLOCK_DIV_32				PWMCON1|=0x05;PWMCON1&=0xFD
#define		PWM_CLOCK_DIV_64				PWMCON1|=0x06;PWMCON1&=0xFE
#define		PWM_CLOCK_DIV_128				PWMCON1|=0x07
//--------- PWM I/O select define ------------------------------
#define		PWM5_P15_OUTPUT_ENABLE		BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=0x01;PIOCON1|=0x20;TA=0xAA;TA=0x55;SFRS&=0xFE;EA=BIT_TMP				//P1.5 as PWM5 output enable
#define		PWM5_P03_OUTPUT_ENABLE		PIOCON0|=0x20																										//P0.3 as PWM5
#define		PWM4_P01_OUTPUT_ENABLE		PIOCON0|=0x10																										//P0.1 as PWM4 output enable
#define		PWM3_P04_OUTPUT_ENABLE		BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=0x01;PIOCON1|=0x08;TA=0xAA;TA=0x55;SFRS&=0xFE;EA=BIT_TMP				//P0.4 as PWM3 output enable
#define		PWM3_P00_OUTPUT_ENABLE		PIOCON0|=0x08																										//P0.0 as PWM3 
#define		PWM2_P05_OUTPUT_ENABLE		BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=0x01;PIOCON1|=0x04;TA=0xAA;TA=0x55;SFRS&=0xFE;EA=BIT_TMP				//P1.0 as PWM2 output enable
#define		PWM2_P10_OUTPUT_ENABLE		PIOCON0|=0x04																										//P1.0 as PWM2
#define		PWM1_P14_OUTPUT_ENABLE		BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=0x01;PIOCON1|=0x02;TA=0xAA;TA=0x55;SFRS&=0xFE;EA=BIT_TMP				//P1.4 as PWM1 output enable
#define		PWM1_P11_OUTPUT_ENABLE		PIOCON0|=0x02																										//P1.1 as PWM1 
#define		PWM0_P12_OUTPUT_ENABLE		PIOCON0|=0x01																										//P1.2 as PWM0 output enable
#define 	ALL_PWM_OUTPUT_ENABLE			PIOCON0=0xFF;PIOCON1=0xFF
#define		PWM5_P15_OUTPUT_DISABLE		BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=0x01;PIOCON1&=0xDF;TA=0xAA;TA=0x55;SFRS&=0xFE;EA=BIT_TMP				//P1.5 as PWM5 output disable
#define		PWM5_P03_OUTPUT_DISABLE		PIOCON0&=0xDF																										//P0.3 as PWM5
#define		PWM4_P01_OUTPUT_DISABLE		PIOCON0&=0xEF																										//P0.1 as PWM4 output disable
#define		PWM3_P04_OUTPUT_DISABLE		BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=0x01;PIOCON1&=0xF7;TA=0xAA;TA=0x55;SFRS&=0xFE;EA=BIT_TMP				//P0.4 as PWM3 output disable
#define		PWM3_P00_OUTPUT_DISABLE		PIOCON0&=0xF7																										//P0.0 as PWM3 
#define		PWM2_P05_OUTPUT_DISABLE		BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=0x01;PIOCON1&=0xFB;TA=0xAA;TA=0x55;SFRS&=0xFE;EA=BIT_TMP				//P1.0 as PWM2 output disable
#define		PWM2_P10_OUTPUT_DISABLE		PIOCON0&=0xFB																										//P1.0 as PWM2
#define		PWM1_P14_OUTPUT_DISABLE		BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;SFRS|=0x01;PIOCON1&=0xFD;TA=0xAA;TA=0x55;SFRS&=0xFE;EA=BIT_TMP				//P1.4 as PWM1 output disable
#define		PWM1_P11_OUTPUT_DISABLE		PIOCON0&=0xFD																										//P1.1 as PWM1 
#define		PWM0_P12_OUTPUT_DISABLE		PIOCON0&=0xFE																										//P1.2 as PWM0 output disable
#define 	ALL_PWM_OUTPUT_DISABLE		PIOCON0=0x00;PIOCON1=0x00
//--------- PWM I/O Polarity Control ---------------------------
#define		PWM5_OUTPUT_INVERSE			PNP|=0x20				
#define		PWM4_OUTPUT_INVERSE			PNP|=0x10				
#define		PWM3_OUTPUT_INVERSE			PNP|=0x08				
#define		PWM2_OUTPUT_INVERSE			PNP|=0x04				
#define		PWM1_OUTPUT_INVERSE			PNP|=0x02				
#define		PWM0_OUTPUT_INVERSE			PNP|=0x01				
#define		PWM_OUTPUT_ALL_INVERSE	PNP=0xFF
#define		PWM5_OUTPUT_NORMAL			PNP&=0xDF				
#define		PWM4_OUTPUT_NORMAL			PNP&=0xEF				
#define		PWM3_OUTPUT_NORMAL			PNP&=0xF7				
#define		PWM2_OUTPUT_NORMAL			PNP&=0xFB				
#define		PWM1_OUTPUT_NORMAL			PNP&=0xFD				
#define		PWM0_OUTPUT_NORMAL			PNP&=0xFE				
#define		PWM_OUTPUT_ALL_NORMAL		PNP=0x00
//--------- PWM type define ------------------------------------
#define		PWM_EDGE_TYPE						PWMCON1&=~SET_BIT4
#define		PWM_CENTER_TYPE					PWMCON1|=SET_BIT4
//--------- PWM mode define ------------------------------------
#define		PWM_IMDEPENDENT_MODE		PWMCON1&=0x3F
#define		PWM_COMPLEMENTARY_MODE	PWMCON1|=0x40;PWMCON1&=0x7F
#define		PWM_SYNCHRONIZED_MODE		PWMCON1|=0x80;PWMCON1&=0xBF
#define 	PWM_GP_MODE_ENABLE			PWMCON1|=0x20
#define		PWM_GP_MODE_DISABLE			PWMCON1&=0xDF
//--------- PMW interrupt setting ------------------------------
#define		PWM_FALLING_INT					PWMCON0&=0xF3
#define		PWM_RISING_INT					PWMCON0|=0x04;PWMCON0&=0xF7
#define		PWM_CENTRAL_POINT_INT		PWMCON0|=0x08;PWMCON0&=0xFB
#define		PWM_PERIOD_END_INT			PWMCON0|=0x0C
//--------- PWM interrupt pin select ---------------------------
#define		PWM_INT_PWM0						PWMCON0&=0xFC
#define		PWM_INT_PWM2						PWMCON0|=0x01;PWMCON0&=0xFD
#define		PWM_INT_PWM4						PWMCON0|=0x02;PWMCON0&=0xFE
#define		PWM_INT_PMW6						PWMCON0|=0x03
//--------- PWM Dead time setting ------------------------------
#define 	PWM45_DEADTIME_ENABLE			BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PDTEN|=0x04;EA=BIT_TMP
#define 	PWM34_DEADTIME_ENABLE			BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PDTEN|=0x02;EA=BIT_TMP
#define 	PWM01_DEADTIME_ENABLE			BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;PDTEN|=0x01;EA=BIT_TMP

/*****************************************************************************************
* For ADC setting 
*****************************************************************************************/
#define Enable_ADC_AIN0			ADCCON0&=0xF0;P17_Input_Mode;AINDIDS=0x00;AINDIDS|=SET_BIT0;ADCCON1|=SET_BIT0									//P17
#define Enable_ADC_AIN1			ADCCON0&=0xF0;ADCCON0|=0x01;P30_Input_Mode;AINDIDS=0x00;AINDIDS|=SET_BIT1;ADCCON1|=SET_BIT0		//P30
#define Enable_ADC_AIN2			ADCCON0&=0xF0;ADCCON0|=0x02;P07_Input_Mode;AINDIDS=0x00;AINDIDS|=SET_BIT2;ADCCON1|=SET_BIT0		//P07
#define Enable_ADC_AIN3			ADCCON0&=0xF0;ADCCON0|=0x03;P06_Input_Mode;AINDIDS=0x00;AINDIDS|=SET_BIT3;ADCCON1|=SET_BIT0		//P06
#define Enable_ADC_AIN4			ADCCON0&=0xF0;ADCCON0|=0x04;P05_Input_Mode;AINDIDS=0x00;AINDIDS|=SET_BIT4;ADCCON1|=SET_BIT0		//P05
#define Enable_ADC_AIN5			ADCCON0&=0xF0;ADCCON0|=0x05;P04_Input_Mode;AINDIDS=0x00;AINDIDS|=SET_BIT5;ADCCON1|=SET_BIT0		//P04
#define Enable_ADC_AIN6			ADCCON0&=0xF0;ADCCON0|=0x06;P03_Input_Mode;AINDIDS=0x00;AINDIDS|=SET_BIT6;ADCCON1|=SET_BIT0		//P03
#define Enable_ADC_AIN7			ADCCON0&=0xF0;ADCCON0|=0x07;P11_Input_Mode;AINDIDS=0x00;AINDIDS|=SET_BIT7;ADCCON1|=SET_BIT0		//P11
#define Enable_ADC_BandGap	ADCCON0|=SET_BIT3;ADCCON0&=0xF8;ADCCON1|=SET_BIT0																							//Band-gap 1.22V

#define PWM0_FALLINGEDGE_TRIG_ADC		ADCCON0&=~SET_BIT5;ADCCON0&=~SET_BIT4;ADCCON1&=~SET_BIT3;ADCCON1&=~SET_BIT2;ADCCON1|=SET_BIT1
#define PWM2_FALLINGEDGE_TRIG_ADC		ADCCON0&=~SET_BIT5;ADCCON0|=SET_BIT4;ADCCON1&=~SET_BIT3;ADCCON1&=~SET_BIT2;ADCCON1|=SET_BIT1
#define PWM4_FALLINGEDGE_TRIG_ADC		ADCCON0|=SET_BIT5;ADCCON0&=~SET_BIT4;ADCCON1&=~SET_BIT3;ADCCON1&=~SET_BIT2;ADCCON1|=SET_BIT1
#define PWM0_RISINGEDGE_TRIG_ADC		ADCCON0&=~SET_BIT5;ADCCON0&=~SET_BIT4;ADCCON1&=~SET_BIT3;ADCCON1|=SET_BIT2;ADCCON1|=SET_BIT1
#define PWM2_RISINGEDGE_TRIG_ADC		ADCCON0&=~SET_BIT5;ADCCON0|=SET_BIT4;ADCCON1&=~SET_BIT3;ADCCON1|=SET_BIT2;ADCCON1|=SET_BIT1
#define PWM4_RISINGEDGE_TRIG_ADC		ADCCON0|=SET_BIT5;ADCCON0&=~SET_BIT4;ADCCON1&=~SET_BIT3;ADCCON1|=SET_BIT2;ADCCON1|=SET_BIT1
#define PWM0_CENTRAL_TRIG_ADC				ADCCON0&=~SET_BIT5;ADCCON0&=~SET_BIT4;ADCCON1|=SET_BIT3;ADCCON1&=~SET_BIT2;ADCCON1|=SET_BIT1
#define PWM2_CENTRAL_TRIG_ADC				ADCCON0&=~SET_BIT5;ADCCON0|=SET_BIT4;ADCCON1|=SET_BIT3;ADCCON1&=~SET_BIT2;ADCCON1|=SET_BIT1
#define PWM4_CENTRAL_TRIG_ADC				ADCCON0|=SET_BIT5;ADCCON0&=~SET_BIT4;ADCCON1|=SET_BIT3;ADCCON1&=~SET_BIT2;ADCCON1|=SET_BIT1
#define PWM0_END_TRIG_ADC						ADCCON0&=~SET_BIT5;ADCCON0&=~SET_BIT4;ADCCON1|=SET_BIT3;ADCCON1|=SET_BIT2;ADCCON1|=SET_BIT1
#define PWM2_END_TRIG_ADC						ADCCON0&=~SET_BIT5;ADCCON0|=SET_BIT4;ADCCON1|=SET_BIT3;ADCCON1|=SET_BIT2;ADCCON1|=SET_BIT1
#define PWM4_END_TRIG_ADC						ADCCON0|=SET_BIT5;ADCCON0&=~SET_BIT4;ADCCON1|=SET_BIT3;ADCCON1|=SET_BIT2;ADCCON1|=SET_BIT1

#define P04_FALLINGEDGE_TRIG_ADC		ADCCON0|=0x30;ADCCON1&=0xF3;ADCCON1|=SET_BIT1;ADCCON1&=~SET_BIT6
#define P13_FALLINGEDGE_TRIG_ADC		ADCCON0|=0x30;ADCCON1&=0xF3;ADCCON1|=SET_BIT1;ADCCON1|=SET_BIT6
#define P04_RISINGEDGE_TRIG_ADC			ADCCON0|=0x30;ADCCON1&=~SET_BIT3;ADCCON1|=SET_BIT2;ADCCON1|=SET_BIT1;ADCCON1&=~SET_BIT6
#define P13_RISINGEDGE_TRIG_ADC			ADCCON0|=0x30;ADCCON1&=~SET_BIT3;ADCCON1|=SET_BIT2;ADCCON1|=SET_BIT1;ADCCON1|=SET_BIT6