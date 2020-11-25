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
//  File Function: N76E003 GPIO demo code
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
////------------------- Define Port as Quasi mode  -------------------
//#define P00_Quasi_Mode				P0M1&=~SET_BIT0;P0M2&=~SET_BIT0
//#define P01_Quasi_Mode				P0M1&=~SET_BIT1;P0M2&=~SET_BIT1
//#define P02_Quasi_Mode				P0M1&=~SET_BIT2;P0M2&=~SET_BIT2
//#define P03_Quasi_Mode				P0M1&=~SET_BIT3;P0M2&=~SET_BIT3
//#define P04_Quasi_Mode				P0M1&=~SET_BIT4;P0M2&=~SET_BIT4
//#define P05_Quasi_Mode				P0M1&=~SET_BIT5;P0M2&=~SET_BIT5
//#define P06_Quasi_Mode				P0M1&=~SET_BIT6;P0M2&=~SET_BIT6
//#define P07_Quasi_Mode				P0M1&=~SET_BIT7;P0M2&=~SET_BIT7
//#define P10_Quasi_Mode				P1M1&=~SET_BIT0;P1M2&=~SET_BIT0
//#define P11_Quasi_Mode				P1M1&=~SET_BIT1;P1M2&=~SET_BIT1
//#define P12_Quasi_Mode				P1M1&=~SET_BIT2;P1M2&=~SET_BIT2
//#define P13_Quasi_Mode				P1M1&=~SET_BIT3;P1M2&=~SET_BIT3
//#define P14_Quasi_Mode				P1M1&=~SET_BIT4;P1M2&=~SET_BIT4
//#define P15_Quasi_Mode				P1M1&=~SET_BIT5;P1M2&=~SET_BIT5
//#define P16_Quasi_Mode				P1M1&=~SET_BIT6;P1M2&=~SET_BIT6
//#define P17_Quasi_Mode				P1M1&=~SET_BIT7;P1M2&=~SET_BIT7
//#define P20_Quasi_Mode				P2M1&=~SET_BIT0;P2M2&=~SET_BIT0
//#define P30_Quasi_Mode				P3M1&=~SET_BIT0;P3M2&=~SET_BIT0
////------------------- Define Port as Push Pull mode -------------------
//#define P00_PushPull_Mode			P0M1|=SET_BIT0;P0M2&=~SET_BIT0
//#define P01_PushPull_Mode			P0M1|=SET_BIT1;P0M2&=~SET_BIT1
//#define P02_PushPull_Mode			P0M1&=~SET_BIT2;P0M2|=SET_BIT2
//#define P03_PushPull_Mode			P0M1&=~SET_BIT3;P0M2|=SET_BIT3
//#define P04_PushPull_Mode			P0M1&=~SET_BIT4;P0M2|=SET_BIT4
//#define P05_PushPull_Mode			P0M1&=~SET_BIT5;P0M2|=SET_BIT5
//#define P06_PushPull_Mode			P0M1&=~SET_BIT6;P0M2|=SET_BIT6
//#define P07_PushPull_Mode			P0M1&=~SET_BIT7;P0M2|=SET_BIT7
//#define P10_PushPull_Mode			P1M1&=~SET_BIT0;P1M2|=SET_BIT0
//#define P11_PushPull_Mode			P1M1&=~SET_BIT1;P1M2|=SET_BIT1
//#define P12_PushPull_Mode			P1M1&=~SET_BIT2;P1M2|=SET_BIT2
//#define P13_PushPull_Mode			P1M1&=~SET_BIT3;P1M2|=SET_BIT3
//#define P14_PushPull_Mode			P1M1&=~SET_BIT4;P1M2|=SET_BIT4
//#define P15_PushPull_Mode			P1M1&=~SET_BIT5;P1M2|=SET_BIT5
//#define P16_PushPull_Mode			P1M1&=~SET_BIT6;P1M2|=SET_BIT6
//#define P17_PushPull_Mode			P1M1&=~SET_BIT7;P1M2|=SET_BIT7
//#define P20_PushPull_Mode			P2M1&=~SET_BIT0;P2M2|=SET_BIT0
//#define P30_PushPull_Mode			P3M1&=~SET_BIT0;P3M2|=SET_BIT0
////------------------- Define Port as Input Only mode -------------------
//#define P00_Input_Mode				P0M1|=SET_BIT0;P0M2&=~SET_BIT0
//#define P01_Input_Mode				P0M1|=SET_BIT1;P0M2&=~SET_BIT1
//#define P02_Input_Mode				P0M1|=SET_BIT2;P0M2&=~SET_BIT2
//#define P03_Input_Mode				P0M1|=SET_BIT3;P0M2&=~SET_BIT3
//#define P04_Input_Mode				P0M1|=SET_BIT4;P0M2&=~SET_BIT4
//#define P05_Input_Mode				P0M1|=SET_BIT5;P0M2&=~SET_BIT5
//#define P06_Input_Mode				P0M1|=SET_BIT6;P0M2&=~SET_BIT6
//#define P07_Input_Mode				P0M1|=SET_BIT7;P0M2&=~SET_BIT7
//#define P10_Input_Mode				P1M1|=SET_BIT0;P1M2&=~SET_BIT0
//#define P11_Input_Mode				P1M1|=SET_BIT1;P1M2&=~SET_BIT1
//#define P12_Input_Mode				P1M1|=SET_BIT2;P1M2&=~SET_BIT2
//#define P13_Input_Mode				P1M1|=SET_BIT3;P1M2&=~SET_BIT3
//#define P14_Input_Mode				P1M1|=SET_BIT4;P1M2&=~SET_BIT4
//#define P15_Input_Mode				P1M1|=SET_BIT5;P1M2&=~SET_BIT5
//#define P16_Input_Mode				P1M1|=SET_BIT6;P1M2&=~SET_BIT6
//#define P17_Input_Mode				P1M1|=SET_BIT7;P1M2&=~SET_BIT7
//#define P20_Input_Mode				P2M1|=SET_BIT0;P2M2&=~SET_BIT0
//#define P30_Input_Mode				P3M1|=SET_BIT0;P3M2&=~SET_BIT0
////-------------------Define Port as Open Drain mode -------------------
//#define P00_OpenDrain_Mode		P0M1|=SET_BIT0;P0M2|=SET_BIT0
//#define P01_OpenDrain_Mode		P0M1|=SET_BIT1;P0M2|=SET_BIT1
//#define P02_OpenDrain_Mode		P0M1|=SET_BIT2;P0M2|=SET_BIT2
//#define P03_OpenDrain_Mode		P0M1|=SET_BIT3;P0M2|=SET_BIT3
//#define P04_OpenDrain_Mode		P0M1|=SET_BIT4;P0M2|=SET_BIT4
//#define P05_OpenDrain_Mode		P0M1|=SET_BIT5;P0M2|=SET_BIT5
//#define P06_OpenDrain_Mode		P0M1|=SET_BIT6;P0M2|=SET_BIT6
//#define P07_OpenDrain_Mode		P0M1|=SET_BIT7;P0M2|=SET_BIT7
//#define P10_OpenDrain_Mode		P1M1|=SET_BIT0;P1M2|=SET_BIT0
//#define P11_OpenDrain_Mode		P1M1|=SET_BIT1;P1M2|=SET_BIT1
//#define P12_OpenDrain_Mode		P1M1|=SET_BIT2;P1M2|=SET_BIT2
//#define P13_OpenDrain_Mode		P1M1|=SET_BIT3;P1M2|=SET_BIT3
//#define P14_OpenDrain_Mode		P1M1|=SET_BIT4;P1M2|=SET_BIT4
//#define P15_OpenDrain_Mode		P1M1|=SET_BIT5;P1M2|=SET_BIT5
//#define P16_OpenDrain_Mode		P1M1|=SET_BIT6;P1M2|=SET_BIT6
//#define P17_OpenDrain_Mode		P1M1|=SET_BIT7;P1M2|=SET_BIT7
//#define P20_OpenDrain_Mode		P2M1|=SET_BIT0;P2M2|=SET_BIT0
//#define P30_OpenDrain_Mode		P3M1|=SET_BIT0;P3M2|=SET_BIT0
////--------- Define all port as quasi mode ---------
//#define Set_All_GPIO_Quasi_Mode			P0M1=0;P0M1=0;P1M1=0;P1M2=0;P2M1=0;P2M2=0;P3M1=0;P3M2=0
#endif

/*------------------------------------------------
The main C function.  Program execution starts
here after stack initialization.
------------------------------------------------*/

void main (void) 
{
	unsigned char i;
	Set_All_GPIO_Quasi_Mode;					// Define in Function_define.h
	InitialUART0_Timer3(115200);
	set_CLOEN;  
  
	
  while(1)
  {
		i--;
			clr_GPIO1;												// Tiny board GPIO1 LED define in Common.h
			Timer2_Delay1ms(300);
			set_GPIO1;
			Timer2_Delay1ms(300);
			Send_Data_To_UART0(0x55);					//UART0 send ascii "U"
		
		P3M1 = 0xff;
		P3M2 = 0x00;
		P3M1 = 0x00;
		P3M2 = 0x00;
		if(P30) P10 = 1; 
		else P10 = 0;
		i += 6;
		
  }
}



