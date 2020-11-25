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
//  File Function: N76E003 Clock Output demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"


//======================================================================================================

void main(void)
{
	
    /* Note
       MCU power on system clock is HIRC (16.0000MHz), if need change Fsys source pleaes copy hange system closk source demo
    */
    
		Set_All_GPIO_Quasi_Mode;                    //in Function_define.h
    set_CLOEN;                                  //System Clock Output Enable
//     Please measure P1.1 (CLO) pin for system clock output, it should as system clock;

    while(1)
    {
        clr_GPIO1;
        Timer0_Delay1ms(500);
        set_GPIO1;
        Timer0_Delay1ms(500);
    }
/* =================== */
}

