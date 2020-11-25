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
#include "SFR_Macro.h"
#include "Function_Define.h"
#include "time.h"
#include "adc.h"
#include "pwm.h"

void main (void) 
{
    
	P05_Input_Mode;
	P06_Input_Mode;
  P16_Input_Mode;
	P04_PushPull_Mode;
	P00_PushPull_Mode;
	P03_PushPull_Mode;
	
	P07_PushPull_Mode;
	P30_PushPull_Mode;
	P10_PushPull_Mode;
	P11_PushPull_Mode;
	P12_PushPull_Mode;
	P13_PushPull_Mode;
	P14_PushPull_Mode;
	P15_PushPull_Mode;
	P17_PushPull_Mode;	
  P00_PushPull_Mode;
	P01_PushPull_Mode;
	
	FDC633_CTL=0;
//	P01=1;
	AIC3415_CTL=0;
	//P07=1	;
	BUZ=0;	
	P03=0;
	//P04=0;
	RED=BLUE=YELLOW=0;
	CKDIV=1;      //Fsys=Fosc/(2*CKDIV) When CKDIV is 00,Fsys is Fosc.CKDIV between 0x00 to 0xff
	clr_CLOEN; //DISABLE clk output 
	pwm_init();
	pwm_stop();//CLOSE PWMOUT
	moto_stop();
  Time_Init();
	
		
  while(1)
  {
  }
} 


