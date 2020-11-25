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
//  File Function: N76E003 Watch Dog as pure timer with interrupt demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"


/************************************************************************************************************
*    WDT interrupt sub-routine
************************************************************************************************************/
void WDT_ISR (void)   interrupt 10
{

	//---------toggle GPIO1---------	
		clr_GPIO1;
		Timer0_Delay1ms(50);
		set_GPIO1;
		Timer0_Delay1ms(50);
		clr_GPIO1;
		Timer0_Delay1ms(50);
		set_GPIO1;
		Timer0_Delay1ms(50);
	//---------end toggle GPIO1--------
		clr_WDTF;
		set_WDCLR;
}

/************************************************************************************************************
*    Main function 
************************************************************************************************************/
void main (void)
{
/* Note
  WDT timer base is LIRC 10Khz
*/
    
  Set_All_GPIO_Quasi_Mode;
	
//WDT init
//--------------------------------------------------------
//Warning:
//Pleaes always check CONFIG WDT disable first 
//only when WDT reset disable, WDT use as pure timer
//--------------------------------------------------------
	  TA=0xAA;TA=0x55;WDCON=0x06;  		//Setting WDT prescale 
		set_WDTR;                       //WDT run
		set_WDCLR;											//Clear WDT timer
		set_EWDT;
		EA =1;

#if 1
    while(1);
#else
		set_WIDPD; 											//WDT run in POWER DOWM mode setting if needed
		while (1)
		{
			set_PD;
		//---------toggle GPIO1---------	
			clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);
			clr_GPIO1;
			Timer1_Delay1ms(100);
			set_GPIO1;
			Timer1_Delay1ms(100);
		//---------end toggle GPIO1---------
		}
#endif
}
