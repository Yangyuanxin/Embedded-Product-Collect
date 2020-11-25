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
//  File Function: N76E003 System clock select demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"


//========================================================================
//  The test process:
//	1. Power on is run as default HIRC, show LED Fsys tickle faster
// 	2. toggle P3.0 to GND.
//	2. call modify Fsys code to LIRC.
//	3. LED tickle speed slowly than before.
//========================================================================
void main(void)
{

/* Note
  MCU power on system clock is HIRC (16 MHz)
	Please keep P3.0 HIGH before you want to modify Fsys to LIRC
*/
	
	  Set_All_GPIO_Quasi_Mode;                   	// In Common.h define
    set_CLOEN;    															// Also can check P1.1 CLO pin for clock to find the Fsys change.
		set_P30;
		while (P30)																	// when P3.0 keep high, clock out HIRC
		{   
				clr_GPIO1;															// Check LED output tickle time
        Timer0_Delay1ms(200);
        set_GPIO1;
        Timer0_Delay1ms(200);
		}
	
////------------------------------------------------------------------------------------------------------
///*********************************** Change system closk source ***************************************/
////------------------------------------------------------------------------------------------------------
              
////***** HIRC enable part *****               
//		set_HIRCEN;												//step1: enable HIRC clock source run
//		while((CKSWT&SET_BIT5)==0);				//step2: check ready
//		clr_OSC1;													//step3: switching system clock source if needed
//		clr_OSC0;
//		while((CKEN&SET_BIT0)==1);				//step4: check system clock switching OK or NG
//
////***** LIRC enable part*****
////** Since LIRC is always enable, switch to LIRC directly
		set_OSC1;													//step3: switching system clock source if needed
		clr_OSC0;  
		while((CKEN&SET_BIT0)==1);				//step4: check system clock switching OK or NG 	
		clr_HIRCEN;
   
////--------------------------------------------------------------------------------------------------------
    
/*
  Now Fsys = LIRC , LED tickle slowly.
*/
    while(1)
    {
				clr_GPIO1;															// Check LED output tickle time
        Timer0_Delay1ms(20);
        set_GPIO1;
        Timer0_Delay1ms(20);
    }
/* =================== */
}

