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
//  File Function: N76E003 Watch Dog reste functiondemo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"

/***********************************************************************
	WDT CONFIG enable 
	warning : this macro is only when ICP not enable CONFIG WDT function
	copy this marco code to you code to enable WDT reset.
************************************************************************/
void Enable_WDT_Reset_Config(void)
{
	  set_IAPEN;
    IAPAL = 0x04;
    IAPAH = 0x00;
    IAPFD = 0x0F;
    IAPCN = 0xE1;
    set_CFUEN;
    set_IAPGO;                                  //trigger IAP
		while((CHPCON&SET_BIT6)==SET_BIT6);          //check IAPFF (CHPCON.6)
    clr_CFUEN;
    clr_IAPEN;
}


/************************************************************************************************************
*    Main function 
************************************************************************************************************/
void main (void)
{
  Set_All_GPIO_Quasi_Mode;

	clr_GPIO1;
	Timer0_Delay1ms(50);					//toggle I/O to show MCU Reset
	set_GPIO1;
	Timer0_Delay1ms(50);
	clr_GPIO1;
	Timer0_Delay1ms(50);
	set_GPIO1;
	Timer0_Delay1ms(50);
	
//----------------------------------------------------------------------------------------------
// WDT Init
// Warning:
// Always check CONFIG WDT enable first, CONFIG not enable, SFR can't enable WDT reset
// Please call Enable_WDT_Reset_Config() function to enable CONFIG WDT reset
//----------------------------------------------------------------------------------------------

//	Enable_WDT_Reset_Config();
	  TA=0xAA;TA=0x55;WDCON=0x07;						//Setting WDT prescale 
		set_WDCLR;														//Clear WDT timer
		while((WDCON|~SET_BIT6)==0xFF);				//confirm WDT clear is ok before into power down mode
		EA = 1;
		set_WDTR;															//WDT run
		
    while(1)
		{
			clr_GPIO1;
			Timer0_Delay1ms(500);
			set_GPIO1;
			Timer0_Delay1ms(500);
			clr_GPIO1;
			Timer0_Delay1ms(500);
			set_GPIO1;
			Timer0_Delay1ms(500);
			set_PD;
		}
}

