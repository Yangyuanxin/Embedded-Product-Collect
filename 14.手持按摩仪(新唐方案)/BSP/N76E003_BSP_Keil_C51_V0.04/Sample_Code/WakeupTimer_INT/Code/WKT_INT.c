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
//  File Function: N76E003 wake up timer interrupt demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"


void WakeUp_Timer_ISR (void)   interrupt 17     //ISR for self wake-up timer
{
		set_GPIO1;
		Timer2_Delay1ms(100);
		clr_GPIO1;
		Timer2_Delay1ms(100);
		set_GPIO1;
	  clr_WKTF;                    		           //clear interrupt flag   
}


/************************************************************************************************************
*    Main function 
************************************************************************************************************/
void main (void)
{
   
    Set_All_GPIO_Quasi_Mode;
//-----------------------------------------------------
//	WKT initial 	
//-----------------------------------------------------
		WKCON = 0x02; 										//timer base 10k, Pre-scale = 1/16
//		RWK = 0XFF;											//	if prescale is 0x00, never set RWK = 0xff
		RWK = 0X00;
   	set_EWKT;													// enable WKT interrupt
		set_WKTR; 												// Wake-up timer run 
		EA = 1;
		
	  while(1)
		{
			set_P04;
			Timer1_Delay1ms(100);
			clr_P04;
			Timer1_Delay1ms(100);
			set_P04;
			Timer1_Delay1ms(100);
			clr_P04;
		}
}

