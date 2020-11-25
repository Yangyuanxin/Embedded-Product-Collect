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
//  File Function: N76E003 Timer0/1 Mode1 demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"

//*****************  The Following is in define in Fucntion_define.h  ***************************
//****** Always include Function_define.h call the define you want, detail see main(void) *******
//***********************************************************************************************
#if 0
//#define		TIMER0_MODE0_ENABLE		TMOD&=0x0F
//#define		TIMER0_MODE1_ENABLE		TMOD&=0x0F;TMOD|=0x10
//#define		TIMER0_MODE2_ENABLE		TMOD&=0x0F;TMOD|=0x20
//#define		TIMER0_MODE3_ENABLE		TMOD&=0x0F;TMOD|=0x3F

//#define		TIMER1_MODE0_ENABLE		TMOD&=0xF0
//#define		TIMER1_MODE1_ENABLE		TMOD&=0xF0;TMOD|=0x01
//#define		TIMER1_MODE2_ENABLE		TMOD&=0xF0;TMOD|=0x02
//#define		TIMER1_MODE3_ENABLE		TMOD&=0xF0;TMOD|=0xF3
#endif

#define TH0_INIT        50000 
#define TL0_INIT        50000
#define TH1_INIT        25000 
#define TL1_INIT        25000

UINT8 u8TH0_Tmp,u8TL0_Tmp,u8TH1_Tmp,u8TL1_Tmp;
/************************************************************************************************************
*    TIMER 0 interrupt subroutine
************************************************************************************************************/
void Timer0_ISR (void) interrupt 1  //interrupt address is 0x000B
{
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;    
    
    P12 = ~P12;                     // GPIO1 toggle when interrupt
}

/************************************************************************************************************
*    TIMER 1 interrupt subroutine
************************************************************************************************************/
void Timer1_ISR (void) interrupt 3  //interrupt address is 0x001B
{
    TH1 = u8TH1_Tmp;
    TL1 = u8TL1_Tmp;   

    P03 = ~P03;                     //P0.3 toggle when interrupt
}
/************************************************************************************************************
*    Main function 
************************************************************************************************************/
void main (void)
{
   
    Set_All_GPIO_Quasi_Mode;
		TIMER0_MODE1_ENABLE;
		TIMER1_MODE1_ENABLE;

    clr_T1M;
    //set_T1M;

    u8TH0_Tmp = (65536-TH0_INIT)/256;
    u8TL0_Tmp = (65536-TL0_INIT)%256;    
    u8TH1_Tmp = (65536-TH1_INIT)/256;
    u8TL1_Tmp = (65536-TL1_INIT)%256;
    
    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;
    TH1 = u8TH1_Tmp;
    TL1 = u8TL1_Tmp;
    
    set_ET0;                                    //enable Timer0 interrupt
    set_ET1;                                    //enable Timer1 interrupt
    set_EA;                                     //enable interrupts
    set_TR0;                                    //Timer0 run
    set_TR1;                                    //Timer1 run
    while(1);
}

