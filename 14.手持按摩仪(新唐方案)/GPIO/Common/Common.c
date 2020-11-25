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

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"

//----------------------------------------------------------------------------------
// use timer1 as Baudrate generator, Maxmum Baudrate can ???? bps @ 16MHZ
//----------------------------------------------------------------------------------
void InitialUART0_Timer1(UINT32 u32Baudrate)    //T1M = 1, SMOD = 1
{
		P06_Quasi_Mode;
		P07_Quasi_Mode;
	
    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
    TMOD |= 0x20;    //Timer1 Mode1
    
    set_SMOD;        //UART0 Double Rate Enable
    set_T1M;
    clr_BRCK;        //Serial port 0 baud rate clock source = Timer1

 
#ifdef FOSC_160000
    TH1 = 256 - (1000000/u32Baudrate+1);               /*16 MHz */
#endif    	
#ifdef FOSC_221184
    TH1 = 256 - (1382400/u32Baudrate);              /*22.1184 MHz */
#endif
    set_TR1;
}
//---------------------------------------------------------------
void InitialUART0_Timer3(UINT32 u32Baudrate) //use timer3 as Baudrate generator
{
		P06_Quasi_Mode;
		P07_Quasi_Mode;	
	
    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
    set_SMOD;        //UART0 Double Rate Enable
    T3CON &= 0xF8;   //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
    set_BRCK;        //UART0 baud rate clock source = Timer3

#ifdef FOSC_160000
		RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);  		/*16 MHz */
		RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);			/*16 MHz */
#endif
#ifdef FOSC_221184
    RH3    = HIBYTE(65536 - (1382400/u32Baudrate)); 	/*22.1184 MHz */
    RL3    = LOBYTE(65536 - (1382400/u32Baudrate)); 	/*22.1184 MHz */
#endif
    set_TR3;         //Trigger Timer3
}

UINT8 Receive_Data_From_UART0(void)
{
    UINT8 c;
    
    while (!RI);
    c = SBUF;
    RI = 0;
    return (c);
}

void Send_Data_To_UART0 (UINT8 c)
{
    TI = 0;
    SBUF = c;
    while(TI==0);
}


//---------------------------------------------------------------
void InitialUART1_Timer3(UINT32 u32Baudrate) //use timer3 as Baudrate generator
{
		P02_Quasi_Mode;
		P16_Quasi_Mode;
	
	  SCON_1 = 0x52;   //UART1 Mode1,REN_1=1,TI_1=1
    T3CON &= 0xF8;   //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
#ifdef FOSC_110592
    RH3    = HIBYTE(65536 - (691200/u32Baudrate));  /*11.0592 MHz */
    RL3    = LOBYTE(65536 - (691200/u32Baudrate));  /*11.0592 MHz */
#endif
#ifdef FOSC_184320
    RH3    = HIBYTE(65536 - (1152000/u32Baudrate)); /*18.4320 MHz */
    RL3    = LOBYTE(65536 - (1152000/u32Baudrate)); /*18.4320 MHz */
#endif
#ifdef FOSC_160000
		RH3    = HIBYTE(65536 - (1000000/u32Baudrate)-1);  		/*16 MHz */
		RL3    = LOBYTE(65536 - (1000000/u32Baudrate)-1);			/*16 MHz */
#endif
#ifdef FOSC_200000
    RH3    = HIBYTE(65536 - (1250000/u32Baudrate)); /*20.0000 MHz */
    RL3    = LOBYTE(65536 - (1250000/u32Baudrate)); /*20.0000 MHz */
#endif
#ifdef FOSC_221184
    RH3    = HIBYTE(65536 - (1382400/u32Baudrate)); 	/*22.1184 MHz */
    RL3    = LOBYTE(65536 - (1382400/u32Baudrate)); 	/*22.1184 MHz */
#endif
    set_TR3;         //Trigger Timer3
}

UINT8 Receive_Data_From_UART1(void)
{
    UINT8 c;
    
    while (!RI_1);
    c = SBUF_1;
    RI_1 = 0;
    return (c);
}

void Send_Data_To_UART1 (UINT8 c)
{
    TI_1 = 0;
    SBUF_1 = c;
    while(TI_1==0);
}


/*==========================================================================*/
#ifdef SW_Reset
void SW_Reset(void)
{
    TA = 0xAA;
    TA = 0x55;
    set_SWRST;
}
#endif
/*==========================================================================*/
