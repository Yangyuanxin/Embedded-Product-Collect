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
//  File Function: N76E003 UART-0 Mode1 demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"

/******************************************************************************
 * FUNCTION_PURPOSE: Serial interrupt, echo received data.
 * FUNCTION_INPUTS : P0.7(RXD) serial input
 * FUNCTION_OUTPUTS: P0.6(TXD) serial output
 * Following setting in Common.c
 ******************************************************************************/
#if 0
//void InitialUART0_Timer1(UINT32 u32Baudrate)    //T1M = 1, SMOD = 1
//{
//		P06_Quasi_Mode;		
//		P07_Quasi_Mode;
//	
//    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
//    TMOD |= 0x20;    //Timer1 Mode1
//    
//    set_SMOD;        //UART0 Double Rate Enable
//    set_T1M;
//    clr_BRCK;        //Serial port 0 baud rate clock source = Timer1

// 
//#ifdef FOSC_160000
//    TH1 = 256 - (1000000/u32Baudrate+1);               /*16 MHz */
//#endif    	
//    set_TR1;
//}
////---------------------------------------------------------------
//void InitialUART0_Timer3(UINT32 u32Baudrate) //use timer3 as Baudrate generator
//{
//		P06_Quasi_Mode;
//		P07_Quasi_Mode;	
//	
//    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
//    set_SMOD;        //UART0 Double Rate Enable
//    T3CON &= 0xF8;   //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
//    set_BRCK;        //UART0 baud rate clock source = Timer3

//#ifdef FOSC_160000
//	RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);  		/*16 MHz */
//  RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);			/*16 MHz */
//#endif
//    set_TR3;         //Trigger Timer3
//}
#endif

/*******************************************************************************
 * FUNCTION_PURPOSE: Main function 
 ******************************************************************************/
void main (void)
{

#if 1	
    InitialUART0_Timer1(9600);						//UART0 Baudrate from Timer1
    while(1)
    Send_Data_To_UART0(0x33);
#else
		InitialUART0_Timer3(115200);					//UART0 Baudrate from Timer3
    while(1)
    Send_Data_To_UART0(0x55);
#endif
			

		
		
}
  
