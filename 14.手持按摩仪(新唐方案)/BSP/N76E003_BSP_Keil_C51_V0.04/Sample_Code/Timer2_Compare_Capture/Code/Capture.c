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
//  File Function: N76E003 Timer2 Capture Mode demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"

//*****************  The Following is in define in Fucntion_Define.h  ***************************
//****** Always include Function_define.h call the define you want, detail see main(void) *******
//***********************************************************************************************
#if 0
///*----------------------------------------------------------------------------------------------------------
//*    Capture Pin Selection Define
//----------------------------------------------------------------------------------------------------------*/
////--- Falling Edge -----
//#define IC0_P12_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0
//#define	IC1_P11_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x01;
//#define	IC2_P10_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x02;
//#define	IC3_P00_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x03;
//#define	IC3_P04_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x04;
//#define	IC4_P01_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x05;
//#define	IC5_P03_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x06;
//#define	IC6_P05_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x07;
//#define	IC7_P15_CAP0_FallingEdge_Capture		CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xCF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x08;

//#define IC0_P12_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F
//#define	IC1_P11_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x10;
//#define	IC2_P10_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x20;
//#define	IC3_P00_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x30;
//#define	IC3_P04_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x40;
//#define	IC4_P01_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x50;
//#define	IC5_P03_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x60;
//#define	IC6_P05_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x70;
//#define	IC7_P15_CAP1_FallingEdge_Capture		CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF3;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x80;

//#define IC0_P12_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0
//#define	IC1_P11_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x10;
//#define	IC2_P10_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x20;
//#define	IC3_P00_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x30;
//#define	IC3_P04_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x40;
//#define	IC4_P01_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x50;
//#define	IC5_P03_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x60;
//#define	IC6_P05_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x70;
//#define	IC7_P15_CAP2_FallingEdge_Capture		CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFC;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x80;

////----- Rising edge ----
//#define IC0_P12_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0
//#define	IC1_P11_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x01;
//#define	IC2_P10_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x02;
//#define	IC3_P00_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x03;
//#define	IC3_P04_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x04;
//#define	IC4_P01_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x05;
//#define	IC5_P03_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x06;
//#define	IC6_P05_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x07;
//#define	IC7_P15_CAP0_RisingEdge_Capture			CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xDF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x08;

//#define IC0_P12_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F
//#define	IC1_P11_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x10;
//#define	IC2_P10_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x20;
//#define	IC3_P00_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x30;
//#define	IC3_P04_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x40;
//#define	IC4_P01_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x50;
//#define	IC5_P03_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x60;
//#define	IC6_P05_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x70;
//#define	IC7_P15_CAP1_RisingEdge_Capture			CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xF7;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x80;

//#define IC0_P12_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0
//#define	IC1_P11_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x01;
//#define	IC2_P10_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x02;
//#define	IC3_P00_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x03;
//#define	IC3_P04_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x04;
//#define	IC4_P01_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x05;
//#define	IC5_P03_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x06;
//#define	IC6_P05_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x07;
//#define	IC7_P15_CAP3_RisingEdge_Capture			CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFD;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x08;

////-----BOTH  edge ----
//#define IC0_P12_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0
//#define	IC1_P11_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x01;
//#define	IC2_P10_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x02;
//#define	IC3_P00_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x03;
//#define	IC3_P04_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x04;
//#define	IC4_P01_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x05;
//#define	IC5_P03_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x06;
//#define	IC6_P05_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x07;
//#define	IC7_P15_CAP0_BothEdge_Capture				CAPCON0|=SET_BIT4;CAPCON1|=0x30;CAPCON1&=0xBF;CAPCON2|=SET_BIT4;CAPCON3&=0xF0;CAPCON3|=0x08;

//#define IC0_P12_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F
//#define	IC1_P11_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x10;
//#define	IC2_P10_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x20;
//#define	IC3_P00_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x30;
//#define	IC3_P04_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x40;
//#define	IC4_P01_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x50;
//#define	IC5_P03_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x60;
//#define	IC6_P05_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x70;
//#define	IC7_P15_CAP1_BothEdge_Capture				CAPCON0|=SET_BIT5;CAPCON1|=0x0C;CAPCON1&=0xEF;CAPCON2|=SET_BIT5;CAPCON3&=0x0F;CAPCON3|=0x80;

//#define IC0_P12_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0
//#define	IC1_P11_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x01;
//#define	IC2_P10_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x02;
//#define	IC3_P00_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x03;
//#define	IC3_P04_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x04;
//#define	IC4_P01_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x05;
//#define	IC5_P03_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x06;
//#define	IC6_P05_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x07;
//#define	IC7_P15_CAP3_BothEdge_Capture				CAPCON0|=SET_BIT6;CAPCON1|=0x03;CAPCON1&=0xFB;CAPCON2|=SET_BIT6;CAPCON4&=0xF0;CAPCON4|=0x08;

//#define TIMER2_IC2_DISABLE									CAPCON0&=~SET_BIT6			 
//#define TIMER2_IC1_DISABLE									CAPCON0&=~SET_BIT5			
//#define TIMER2_IC0_DISABLE									CAPCON0&=~SET_BIT4	
///*----------------------------------------------------------------------------------------------------------
//*    Timer2 mode define
//------------------------------------------------------------------------------------------------------------*/
//#define TIMER2_Auto_Reload_Delay_Mode				T2CON&=~SET_BIT0;T2MOD|=SET_BIT7;T2MOD|=SET_BIT3
//#define TIMER2_Auto_Reload_Capture_Mode			T2CON&=~SET_BIT0;T2MOD|=SET_BIT7
//#define	TIMER2_Compare_Capture_Mode					T2CON|=SET_BIT0;T2MOD&=~SET_BIT7;T2MOD|=SET_BIT2
///*----------------------------------------------------------------------------------------------------------
//*   Timer devider define
//----------------------------------------------------------------------------------------------------------*/
//#define TIMER2_DIV_4			T2MOD|=0x10;T2MOD&=0x9F
//#define TIMER2_DIV_16			T2MOD|=0x20;T2MOD&=0xAF
//#define TIMER2_DIV_32			T2MOD|=0x30;T2MOD&=0xBF
//#define TIMER2_DIV_64			T2MOD|=0x40;T2MOD&=0xCF
//#define TIMER2_DIV_128		T2MOD|=0x50;T2MOD&=0xDF
//#define TIMER2_DIV_256		T2MOD|=0x60;T2MOD&=0xEF
//#define TIMER2_DIV_512		T2MOD|=0x70
#endif


/************************************************************************************************************
*    Timer2 Capture interrupt subroutine
************************************************************************************************************/
void Capture_ISR (void) interrupt 12
{
        clr_CAPF0;                          // clear capture0 interrupt flag
        P1 = C0L;														// For capture mode CxL/CxH with data capture from I/O pin
        P2 = C0H;														
        P12 = ~P12;													//toggle GPIO1 to show int
}
/************************************************************************************************************
*    Main function 
************************************************************************************************************/
void main (void)
{
    Set_All_GPIO_Quasi_Mode;
		TIMER2_Compare_Capture_Mode;
		IC3_P00_CAP0_BothEdge_Capture;
		
    set_ET2;                                    //Enable Timer2 interrupt
    set_ECAP;                                   //Enable Capture interrupt
    set_TR2;                                    //Triger Timer2
    set_EA;
    
    while(1);
}
