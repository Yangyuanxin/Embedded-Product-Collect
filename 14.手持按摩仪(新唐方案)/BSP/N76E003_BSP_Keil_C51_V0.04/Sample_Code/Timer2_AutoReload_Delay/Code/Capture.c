/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2016 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Apr/21/2016
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: N76E003 Timer2 Auto reload Mode counter demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"


/******************************************************************
* Following define for timer 2 also in Function_define.h
********************************************************************/
#if 0 
//#define TIMER_DIV4_VALUE_10us				65536-56				//9*4/22118400 = 10 uS,    			// Timer divider = 4
//#define TIMER_DIV4_VALUE_1ms				65536-5530			//923*4/22118400 = 1 mS,   			// Timer divider = 4
//#define TIMER_DIV4_VALUE_100us			65536-553				//553*4/22118400 = 100 us				// Timer divider = 4
//#define TIMER_DIV4_VALUE_200us			65536-1106			//1106*4/22118400 = 200 us			// Timer divider = 4
//#define TIMER_DIV4_VALUE_500us			65536-2765			//2765*4/22118400 = 500 us			// Timer divider = 4		
//#define TIMER_DIV16_VALUE_10ms			65536-13824			//1500*16/22118400 = 10 ms			// Timer divider = 16
//#define TIMER_DIV64_VALUE_30ms			65536-10368			//10368*64/22118400 = 30 ms			// Timer divider = 64
//#define	TIMER_DIV128_VALUE_100ms		65536-17280			//17280*128/22118400 = 100 ms		// Timer divider = 128
//#define	TIMER_DIV128_VALUE_200ms		65536-34560			//34560*128/22118400 = 200 ms		// Timer divider = 128
//#define TIMER_DIV256_VALUE_500ms		65536-43200			//43200*256/22118400 = 500 ms 	// Timer divider = 256
//#define TIMER_DIV512_VALUE_1s				65536-43200			//43200*512/22118400 = 1 s			// Timer divider = 512

//#define TIMER2_DIV_4			T2MOD|=0x10;T2MOD&=0x9F
//#define TIMER2_DIV_16			T2MOD|=0x20;T2MOD&=0xAF
//#define TIMER2_DIV_32			T2MOD|=0x30;T2MOD&=0xBF
//#define TIMER2_DIV_64			T2MOD|=0x40;T2MOD&=0xCF
//#define TIMER2_DIV_128		T2MOD|=0x50;T2MOD&=0xDF
//#define TIMER2_DIV_256		T2MOD|=0x60;T2MOD&=0xEF
//#define TIMER2_DIV_512		T2MOD|=0x70

//#define TIMER2_Auto_Reload_Delay_Mode				T2CON&=~SET_BIT0;T2MOD|=SET_BIT7;T2MOD|=SET_BIT3
//#define TIMER2_Auto_Reload_Capture_Mode			T2CON&=~SET_BIT0;T2MOD|=SET_BIT7
//#define	TIMER2_Compare_Capture_Mode					T2CON|=SET_BIT0;T2MOD&=~SET_BIT7;T2MOD|=SET_BIT2
#endif

/************************************************************************************************************
*    Timer2 interrupt subroutine
************************************************************************************************************/
void Timer2_ISR (void) interrupt 5
{
	  clr_TF2;                                //Clear Timer2 Interrupt Flag
	  P12 = ~P12;															// GPIO1 toggle
}
/************************************************************************************************************
*    Main function 
************************************************************************************************************/
void main (void)
{
    Set_All_GPIO_Quasi_Mode;

		TIMER2_DIV_128;
		TIMER2_Auto_Reload_Delay_Mode;

		RCMP2L = TIMER_DIV128_VALUE_100ms;
		RCMP2H = TIMER_DIV128_VALUE_100ms>>8;
		TL2 = 0;
		TH2 = 0;

    set_ET2;                                    // Enable Timer2 interrupt
	  set_EA;
    set_TR2;                                    // Timer2 run
    
    while(1);
}
