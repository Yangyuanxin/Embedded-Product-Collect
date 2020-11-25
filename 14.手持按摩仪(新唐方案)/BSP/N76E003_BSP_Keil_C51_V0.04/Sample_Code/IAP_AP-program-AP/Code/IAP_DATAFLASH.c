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
//  File Function: N76E003 APROM program DATAFLASH (APROM) demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"


#define     CID_READ            0x0B
#define     DID_READ            0x0C

/*
	Since the DATAFLASH is in the APROM. Program command is same as program APROM
*/
#define     PAGE_ERASE_AP       0x22
#define     BYTE_READ_AP        0x00
#define     BYTE_PROGRAM_AP     0x21
#define     PAGE_SIZE           128

#define     ERASE_FAIL          0x70
#define     PROGRAM_FAIL        0x71
#define     IAPFF_FAIL          0x72
#define     IAP_PASS            0x00

//---------------------------------------------------------------
// Following define by customer
// Please confirm the start addresss not over your code size
//---------------------------------------------------------------
#define     DATA_SIZE           4096   
#define     DATA_START_ADDR     0x3800 					

/********************************************************************************************
 Following IAP command register is also define in SFR_Macro.h
 
	#define set_IAPEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON |= SET_BIT0 ;EA=BIT_TMP
	#define clr_IAPEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;CHPCON &= ~SET_BIT0;EA=BIT_TMP
	#define set_APUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN |= SET_BIT0 ;EA=BIT_TMP
	#define clr_APUEN   BIT_TMP=EA;EA=0;TA=0xAA;TA=0x55;IAPUEN &= ~SET_BIT0;EA=BIT_TMP
	
**********************************************************************************************/



void IAP_ERROR_LED(void)
{
	while (1)
	{
		clr_P03;
		Timer1_Delay1ms(100);
		set_P03;
		Timer1_Delay1ms(100);
	}

}

//-----------------------------------------------------------------------------------------------------------/
void Trigger_IAP(void)
{   
    set_IAPGO;																	//trigger IAP
    if((CHPCON&SET_BIT6)==SET_BIT6)             // if fail flag is set, toggle error LED and IAP stop
		{
			clr_IAPFF;
			IAP_ERROR_LED();
		}
}
/*
		WARNING:
	No matter read or writer, when IAPFF is set 1, 
	this step process is fail. DATA should be ignore.
*/
//-----------------------------------------------------------------------------------------------------------/

/*****************************************************************************************************************
Erase APROM subroutine:
	

******************************************************************************************************************/		
void Erase_APROM(void)
{   
    UINT16 u16Count;

    set_IAPEN;													// Enable IAP function
		IAPFD = 0xFF;												// IMPORTANT !! To erase function must setting IAPFD = 0xFF 
    IAPCN = PAGE_ERASE_AP;
    set_APUEN;													//  APROM modify Enable
	
    for(u16Count=0x0000;u16Count<DATA_SIZE/PAGE_SIZE;u16Count++)		//
    {        
        IAPAL = LOBYTE(u16Count*PAGE_SIZE + DATA_START_ADDR);
        IAPAH = HIBYTE(u16Count*PAGE_SIZE + DATA_START_ADDR);
        Trigger_IAP(); 
    } 
    clr_APUEN;
    clr_IAPEN;
}
//-----------------------------------------------------------------------------------------------------------
void Erase_APROM_Verify(void)
{   
    UINT16 u16Count;
    set_IAPEN;
    IAPAL = LOBYTE(DATA_START_ADDR);
    IAPAH = HIBYTE(DATA_START_ADDR);
    IAPCN = BYTE_READ_AP;

    for(u16Count=0;u16Count<DATA_SIZE/PAGE_SIZE;u16Count++)
    {   
        IAPFD = 0x00;    
        Trigger_IAP();
        if(IAPFD != 0xFF)
					IAP_ERROR_LED();
        IAPAL++;
        if(IAPAL == 0x00)
          IAPAH++;
    } 
		
    clr_IAPEN;
    
}
//-----------------------------------------------------------------------------------------------------------
void Program_APROM(void)
{   
    UINT16 u16Count;

    set_IAPEN;
    set_APUEN;    
    IAPAL = LOBYTE(DATA_START_ADDR);
    IAPAH = HIBYTE(DATA_START_ADDR);
    IAPCN = BYTE_PROGRAM_AP;
    
    for(u16Count=0;u16Count<DATA_SIZE;u16Count++)
    {   
        IAPFD++;     
        Trigger_IAP();
       
        IAPAL++;
        if(IAPAL == 0)
        {
            IAPAH++;
        }
    } 
		
    clr_APUEN;
    clr_IAPEN;
}
//-----------------------------------------------------------------------------------------------------------
void Program_APROM_Verify(void)
{   
    UINT16 u16Count;
    UINT8  u8Read_Data;

    set_IAPEN;
    IAPAL = LOBYTE(DATA_START_ADDR);
    IAPAH = HIBYTE(DATA_START_ADDR);
    IAPCN = BYTE_READ_AP;
    u8Read_Data = 0x00;

    for(u16Count=0;u16Count<DATA_SIZE;u16Count++)
    {   
        Trigger_IAP();
        if(IAPFD != u8Read_Data)
					IAP_ERROR_LED();
        IAPAL++;
        if(IAPAL == 0)
        {
            IAPAH++;
        }
        u8Read_Data ++;
    } 

    clr_IAPEN;
}
//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------
void main (void) 
{

  Set_All_GPIO_Quasi_Mode;
//---------toggle GPIO1---------	
	clr_GPIO1;
	Timer0_Delay1ms(100);
	set_GPIO1;
	Timer0_Delay1ms(100);
	clr_GPIO1;
	Timer0_Delay1ms(100);
	set_GPIO1;
	Timer0_Delay1ms(100);
//---------end toggle GPIO1---------
	
    Erase_APROM();
		Erase_APROM_Verify();
    Program_APROM();
    Program_APROM_Verify();

//---------toggle GPIO1---------	
	clr_GPIO1;
	Timer0_Delay1ms(100);
	set_GPIO1;
	Timer0_Delay1ms(100);
	clr_GPIO1;
	Timer0_Delay1ms(100);
	set_GPIO1;
	Timer0_Delay1ms(100);
//---------end toggle GPIO1---------

    while(1);
}
//-----------------------------------------------------------------------------------------------------------
