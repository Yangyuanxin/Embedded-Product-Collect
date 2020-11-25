/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2015 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : May/1/2015
//***********************************************************************************************************

#include <stdio.h>
#include "N76E885.h"
#include "Version.h"
#include "Typedef.h"
#include "Define.h"
#include "Common.h"
#include "Delay.h"

#define FW_Version_Major                    0x01
#define FW_Version_Minor                    0x00
#define FW_Version_Modifi                   0x00

#define Date_Y                              2015
#define Date_M                              5
#define Date_D                              1

//-----------------------------------------------------------------------------------------------------------
void Show_FW_Version_Number_To_PC(void)
{
    printf ("\n====================================================================");
    printf ("\n*  Copyright(c) 2015 Nuvoton Technology Corp. All rights reserved.  ");
    printf ("\n*  Contact E-Mail: MicroC-8bit@nuvoton.com                          ");
    printf ("\n*  Demo Version  : %2d.%2d.%2d",(uint16_t)FW_Version_Major,(uint16_t)FW_Version_Minor,(uint16_t)FW_Version_Modifi);
    printf ("\n*  Finished Date : %2d.%2d.%2d",(uint16_t)Date_Y,(uint16_t)Date_M,(uint16_t)Date_D);
    printf ("\n====================================================================");
}