/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2015 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Apr/21/2016
//***********************************************************************************************************

//***********************************************************************************************************
//  File Function: N76E003 I/O Interrupt demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "SFR_Macro.h"
#include "Common.h"
#include "Delay.h"
#include "Function_Define.h"

#if 0
//Following setting in Function_define.h

///****************************************************************************
//   Enable INT port 0~3
//***************************************************************************/
//#define 	Enable_INT_Port0					PICON &= 0xFB;
//#define		Enable_INT_Port1					PICON |= 0x01;
//#define		Enable_INT_Port2					PICON |= 0x02;
//#define		Enable_INT_Port3					PICON |= 0x03;
///*****************************************************************************
// Enable each bit low level trig mode
//*****************************************************************************/
//#define		Enable_BIT7_LowLevel_Trig			PICON&=0x7F;PINEN|=0x80;PIPEN&=0x7F
//#define		Enable_BIT6_LowLevel_Trig			PICON&=0x7F;PINEN|=0x40;PIPEN&=0xBF
//#define		Enable_BIT5_LowLevel_Trig			PICON&=0xBF;PINEN|=0x20;PIPEN&=0xDF
//#define		Enable_BIT4_LowLevel_Trig			PICON&=0xBF;PINEN|=0x10;PIPEN&=0xEF
//#define		Enable_BIT3_LowLevel_Trig			PICON&=0xDF;PINEN|=0x08;PIPEN&=0xF7
//#define		Enable_BIT2_LowLevel_Trig			PICON&=0xEF;PINEN|=0x04;PIPEN&=0xFB
//#define		Enable_BIT1_LowLevel_Trig			PICON&=0xF7;PINEN|=0x02;PIPEN&=0xFD
//#define		Enable_BIT0_LowLevel_Trig			PICON&=0xFD;PINEN|=0x01;PIPEN&=0xFE
///*****************************************************************************
// Enable each bit high level trig mode
//*****************************************************************************/
//#define		Enable_BIT7_HighLevel_Trig			PICON&=0x7F;PINEN&=0x7F;PIPEN|=0x80
//#define		Enable_BIT6_HighLevel_Trig			PICON&=0x7F;PINEN&=0xBF;PIPEN|=0x40
//#define		Enable_BIT5_HighLevel_Trig			PICON&=0xBF;PINEN&=0xDF;PIPEN|=0x20
//#define		Enable_BIT4_HighLevel_Trig			PICON&=0xBF;PINEN&=0xEF;PIPEN|=0x10
//#define		Enable_BIT3_HighLevel_Trig			PICON&=0xDF;PINEN&=0xF7;PIPEN|=0x08
//#define		Enable_BIT2_HighLevel_Trig			PICON&=0xEF;PINEN&=0xFB;PIPEN|=0x04
//#define		Enable_BIT1_HighLevel_Trig			PICON&=0xF7;PINEN&=0xFD;PIPEN|=0x02
//#define		Enable_BIT0_HighLevel_Trig			PICON&=0xFD;PINEN&=0xFE;PIPEN|=0x01
///*****************************************************************************
// Enable each bit falling edge trig mode
//*****************************************************************************/
//#define		Enable_BIT7_FallEdge_Trig			PICON|=0x80;PINEN|=0x80;PIPEN&=0x7F
//#define		Enable_BIT6_FallEdge_Trig			PICON|=0x80;PINEN|=0x40;PIPEN&=0xBF
//#define		Enable_BIT5_FallEdge_Trig			PICON|=0x40;PINEN|=0x20;PIPEN&=0xDF
//#define		Enable_BIT4_FallEdge_Trig			PICON|=0x40;PINEN|=0x10;PIPEN&=0xEF
//#define		Enable_BIT3_FallEdge_Trig			PICON|=0x20;PINEN|=0x08;PIPEN&=0xF7
//#define		Enable_BIT2_FallEdge_Trig			PICON|=0x10;PINEN|=0x04;PIPEN&=0xFB
//#define		Enable_BIT1_FallEdge_Trig			PICON|=0x08;PINEN|=0x02;PIPEN&=0xFD
//#define		Enable_BIT0_FallEdge_Trig			PICON|=0x04;PINEN|=0x01;PIPEN&=0xFE
///*****************************************************************************
// Enable each bit rasing edge trig mode
//*****************************************************************************/
//#define		Enable_BIT7_RasingEdge_Trig			PICON|=0x80;PINEN&=0x7F;PIPEN|=0x80
//#define		Enable_BIT6_RasingEdge_Trig			PICON|=0x80;PINEN&=0xBF;PIPEN|=0x40
//#define		Enable_BIT5_RasingEdge_Trig			PICON|=0x40;PINEN&=0xDF;PIPEN|=0x20
//#define		Enable_BIT4_RasingEdge_Trig			PICON|=0x40;PINEN&=0xEF;PIPEN|=0x10
//#define		Enable_BIT3_RasingEdge_Trig			PICON|=0x20;PINEN&=0xF7;PIPEN|=0x08
//#define		Enable_BIT2_RasingEdge_Trig			PICON|=0x10;PINEN&=0xFB;PIPEN|=0x04
//#define		Enable_BIT1_RasingEdge_Trig			PICON|=0x08;PINEN|=0xFD;PIPEN&=0x02
//#define		Enable_BIT0_RasingEdge_Trig			PICON|=0x04;PINEN|=0xFE;PIPEN&=0x01
#endif

/******************************************************************************
 * FUNCTION_PURPOSE: I/O Pin interrupt Service Routine
 ******************************************************************************/
void EXT_INT0(void) interrupt 0
{
				clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);
			clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);    
			clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);
			clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);  
				clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);
			clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);    
			clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);
			clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);  
		}

void PinInterrupt_ISR (void) interrupt 7
{
	if(PIF == 0x01)
	{
    PIF = 0x00;                             //clear interrupt flag
			clr_GPIO1;
			Timer0_Delay1ms(100);
			set_GPIO1;
			Timer0_Delay1ms(100);
			clr_GPIO1;
			Timer0_Delay1ms(100);
			set_GPIO1;
			Timer0_Delay1ms(100);    
		
	}
	else if (PIF == 0x80)
	{
			clr_GPIO1;
			Timer0_Delay1ms(100);
			set_GPIO1;
			Timer0_Delay1ms(100);
			clr_GPIO1;
			Timer0_Delay1ms(100);
			set_GPIO1;
			Timer0_Delay1ms(100);    
			clr_GPIO1;
			Timer0_Delay1ms(100);
			set_GPIO1;
			Timer0_Delay1ms(100);
			clr_GPIO1;
			Timer0_Delay1ms(100);
			set_GPIO1;
			Timer0_Delay1ms(100);    
	}
}
/******************************************************************************
The main C function.  Program execution starts
here after stack initialization.
******************************************************************************/
void main (void) 
{
 
    P07_Input_Mode;
		set_P0S_7;
		P00_Input_Mode;
		P01_Input_Mode;
		set_P0S_1;
		P12_PushPull_Mode;
		P30_PushPull_Mode;
		P05_PushPull_Mode;
	
		
//----------------------------------------------------
//	P0.5 set as highlevel trig pin interrupt function
//  Keep P0.5 connect to VDD LED will tickle
//  otherwise, MCU into power down mode.
//----------------------------------------------------
		Enable_INT_Port0;
		Enable_BIT7_LowLevel_Trig;
		Enable_BIT0_LowLevel_Trig;

		P30 =1;
		P05 = 0;

    set_EPI;							// Enable pin interrupt
		set_EX0;
    set_EA;								// global enable bit
    
    while(1)
		{
			P01=0;
			P30=0;
			P01=1;
			P30=1;
		}

}


