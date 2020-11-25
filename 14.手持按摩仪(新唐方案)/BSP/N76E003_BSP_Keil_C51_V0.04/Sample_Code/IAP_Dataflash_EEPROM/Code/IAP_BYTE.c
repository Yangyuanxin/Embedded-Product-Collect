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
//  File Function: N76E003 APROM program DATAFLASH as EEPROM way 
//***********************************************************************************************************
#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"

bit BIT_TMP;

//-------------------------------------------------------------------------
UINT8 read_APROM_BYTE(UINT16 code *u16_addr)
{
	UINT8 rdata;
	rdata = *u16_addr>>8;
	return rdata;
}
//-------------------------------------------------------------------------


/*****************************************************************************************************************
write_DATAFLASH_BYTE :
user can copy all this subroutine into project, then call this function in main.
******************************************************************************************************************/		
void write_DATAFLASH_BYTE(UINT16 u16_addr,UINT8 u8_data)
{
	UINT8 looptmp=0,u8_addrl_r;
	unsigned char code *cd_longaddr;
	unsigned char xdata *xd_tmp;
	
//Check page start address
	u8_addrl_r = u16_addr;
	if (u8_addrl_r<0x80)
	{
		u8_addrl_r = 0;
	}
	else 
	{
		u8_addrl_r = 0x80;
	}
//Save APROM data to XRAM
	xd_tmp = 0x80;
	cd_longaddr = (u16_addr&0xff00)+u8_addrl_r;	
	while (xd_tmp !=0x100)
	{
		*xd_tmp = *cd_longaddr;
		looptmp++;
		xd_tmp++;
		cd_longaddr++;
	}
// Modify customer data in XRAM
	u8_addrl_r = u16_addr;
	if (u8_addrl_r<0x80)
	{
		xd_tmp = u8_addrl_r+0x80;
	}
	else
	{
		xd_tmp = u8_addrl_r+0;
	}
	*xd_tmp = u8_data;
//Erase APROM DATAFLASH page
		IAPAL = u16_addr;
		IAPAH = u16_addr>>8;
		IAPFD = 0xFF;
	  set_IAPEN; 
		set_APUEN;
    IAPCN = 0x22; 		
 		set_IAPGO; 
//Save changed RAM data to APROM DATAFLASH
	u8_addrl_r = u16_addr;
	if (u8_addrl_r<0x80)
	{
		u8_addrl_r =0;
	}
	else
	{
		u8_addrl_r = 0x80;
	}
		xd_tmp = 0x80;
	  IAPAL = u8_addrl_r;
    IAPAH = u16_addr>>8;
		set_IAPEN; 
		set_APUEN;
	  IAPCN = 0x21;
		while (xd_tmp !=0xFF)
		{
			IAPFD = *xd_tmp;
			set_IAPGO;
			IAPAL++;
			xd_tmp++;
		}
		clr_APUEN;
		clr_IAPEN;
}	
/******************************************************************************************************************/		


void main (void) 
{
		UINT8 datatemp;
/* -------------------------------------------------------------------------*/
/*  Dataflash use APROM area 				      				      				            */
/*	APROM 0x3800~0x38FF demo as dataflash 				      				      			*/
/* Please use Memory window key in C:0x3800 to check earse result					  */	      
/* -------------------------------------------------------------------------*/
	
//call write byte 
		write_DATAFLASH_BYTE (0x3881,0x55);
		write_DATAFLASH_BYTE (0x3882,0x56);
		write_DATAFLASH_BYTE (0x3855,0xaa);
		write_DATAFLASH_BYTE (0x3856,0x66);
//call read byte
		datatemp = read_APROM_BYTE(0x3882);
    while(1);
}
//-----------------------------------------------------------------------------------------------------------
