/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2015 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

//***********************************************************************************************************
//  Nuvoton Technoledge Corp. 
//  Website: http://www.nuvoton.com
//  E-Mail : MicroC-8bit@nuvoton.com
//  Date   : Apr/21/2015
//***********************************************************************************************************

#include "N76E885.h"
#include "Version.h"
#include "Typedef.h"
#include "Define.h"
#include "SFR_Macro.h"
#include "Common.h"
#include "Delay.h"

/***************************************************************************************************
    System Clock Source Select
****************************************************************************************************/
void System_Clock_Select(E_SYSTEM_CLOCK clock)
{
    switch (clock)
    {
        case    E_HXTEN:
        {
                //step1: enable clock source
                set_EXTEN1;
                clr_EXTEN0;
                
                //step2: check ready
                while((CKSWT&SET_BIT7)==0);
                
                //step3: switching system clock source
                clr_OSC1;
                set_OSC0;
        }break;

        case    E_LXTEN:
        {
                //step1: enable clock source
                clr_EXTEN1;
                set_EXTEN0;
                
                //step2: check ready
                while((CKSWT&SET_BIT6)==0);
                
                //step3: switching system clock source
                clr_OSC1;
                set_OSC0;
        }break;

        case    E_OSCEN:
        {
                //step1: enable clock source
                clr_EXTEN1;
                clr_EXTEN0;
                
                //step2: check ready
                while((CKSWT&SET_BIT3)==0);
                
                //step3: switching system clock source
                clr_OSC1;
                set_OSC0;
        }break;
        
        case    E_HIRCEN: //power on default
        {
                //step1: enable clock source
                set_HIRCEN;
                
                //step2: check ready
                while((CKSWT&SET_BIT5)==0);
                
                //step3: switching system clock source
                clr_OSC1;
                clr_OSC0;
        }break;

        case    E_LIRCEN:
        {
                //step1: enable clock source
                set_LIRCEN;
                
                //step2: check ready
                while((CKSWT&SET_BIT4)==0);
                
                //step3: switching system clock source
                set_OSC1;
                clr_OSC0;
        }break;
    }
    //step4: check system clock switching OK or NG?
    while((CKEN&SET_BIT0)==1);  //Check Clock switch fault flag.
}
//-----------------------------------------------------------------------------------------------------------
/* use timer1 as Baudrate generator, Maxmum Baudrate can 57600 bps @ 22.1184MHz
*/
void InitialUART0_Timer1_Type1(UINT32 u32Baudrate)    //T1M = 0, SMOD = 0
{
    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
    TMOD |= 0x20;    //Timer1 Mode1
    
    clr_SMOD;
    clr_T1M;
    clr_BRCK;        //Serial port 0 baud rate clock source = Timer1
    
#ifdef FOSC_110592
    TH1 = 256 - (28800/u32Baudrate);                /*11.0592 MHz */
#endif    
#ifdef FOSC_184320
    TH1 = 256 - (48000/u32Baudrate);                /*18.4320 MHz */
#endif
#ifdef FOSC_200000
    TH1 = 256 - (52083/u32Baudrate);                /*20.0000 MHz */
#endif
#ifdef FOSC_221184
    TH1 = 256 - (57600/u32Baudrate);                /*22.1184 MHz */
#endif
    set_TR1;
}

//-----------------------------------------------------------------------------------------------------------
/* use timer1 as Baudrate generator, Maxmum Baudrate can 115200 bps @ 22.1184MHz
*/
void InitialUART0_Timer1_Type2(UINT32 u32Baudrate)    //T1M = 0, SMOD = 1
{
    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
    TMOD |= 0x20;    //Timer1 Mode1
    
    set_SMOD;        //UART0 Double Rate Enable
    clr_T1M;
    clr_BRCK;        //Serial port 0 baud rate clock source = Timer1

#ifdef FOSC_110592
    TH1 = 256 - (57600/u32Baudrate);                /*11.0592 MHz */
#endif    
#ifdef FOSC_184320
    TH1 = 256 - (96000/u32Baudrate);                /*18.4320 MHz */ 
#endif
#ifdef FOSC_200000
    TH1 = 256 - (104167/u32Baudrate);               /*20.0000 MHz */
#endif
#ifdef FOSC_221184
    TH1 = 256 - (115200/u32Baudrate);               /*22.1184 MHz */
#endif
    set_TR1;
}

//-----------------------------------------------------------------------------------------------------------
/* use timer1 as Baudrate generator, Maxmum Baudrate can 691200 bps @ 22.1184MHz
*/
void InitialUART0_Timer1_Type3(UINT32 u32Baudrate)    //T1M = 1, SMOD = 0
{
    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
    TMOD |= 0x20;    //Timer1 Mode1
    
    clr_SMOD;
    set_T1M;
    clr_BRCK;        //Serial port 0 baud rate clock source = Timer1

#ifdef FOSC_110592
    TH1 = 256 - (345600/u32Baudrate);               /*11.0592 MHz */
#endif    
#ifdef FOSC_184320
    TH1 = 256 - (576000/u32Baudrate);               /*18.4320 MHz */
#endif
#ifdef FOSC_200000
    TH1 = 256 - (625000/u32Baudrate);               /*20.0000 MHz */
#endif
#ifdef FOSC_221184
    TH1 = 256 - (691200/u32Baudrate);               /*22.1184 MHz */
#endif
    set_TR1;
}

//-----------------------------------------------------------------------------------------------------------
/* use timer1 as Baudrate generator, Maxmum Baudrate can 1382400 bps @ 22.1184MHz
*/
void InitialUART0_Timer1_Type4(UINT32 u32Baudrate)    //T1M = 1, SMOD = 1
{
    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
    TMOD |= 0x20;    //Timer1 Mode1
    
    set_SMOD;        //UART0 Double Rate Enable
    set_T1M;
    clr_BRCK;        //Serial port 0 baud rate clock source = Timer1

#ifdef FOSC_110592
    TH1 = 256 - (691200/u32Baudrate);               /*11.0592 MHz */
#endif    
#ifdef FOSC_184320
    TH1 = 256 - (1152000/u32Baudrate);              /*18.4320 MHz */
#endif
#ifdef FOSC_200000
    TH1 = 256 - (1250000/u32Baudrate);              /*20.0000 MHz */
#endif
#ifdef FOSC_221184
    TH1 = 256 - (1382400/u32Baudrate);              /*22.1184 MHz */
#endif
    set_TR1;
}
//---------------------------------------------------------------
void InitialUART0_Timer3_Type5(UINT32 u32Baudrate) //use timer3 as Baudrate generator
{
    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
    clr_SMOD;
    T3CON &= 0xF8;   //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
    set_BRCK;        //Serial port 0 baud rate clock source = Timer3
    
#ifdef FOSC_110592
    RH3    = HIBYTE(65536 - (345600/u32Baudrate)); /*11.0592 MHz */
    RL3    = LOBYTE(65536 - (345600/u32Baudrate)); /*11.0592 MHz */
#endif
#ifdef FOSC_184320
    RH3    = HIBYTE(65536 - (576000/u32Baudrate)); /*18.4320 MHz */
    RL3    = LOBYTE(65536 - (576000/u32Baudrate)); /*18.4320 MHz */
#endif
#ifdef FOSC_200000
    RH3    = HIBYTE(65536 - (640000/u32Baudrate)); /*20.0000 MHz */
    RL3    = LOBYTE(65536 - (640000/u32Baudrate)); /*20.0000 MHz */
#endif
#ifdef FOSC_221184
    RH3    = HIBYTE(65536 - (691200/u32Baudrate)); /*22.1184 MHz */
    RL3    = LOBYTE(65536 - (691200/u32Baudrate)); /*22.1184 MHz */
#endif
    set_TR3;         //Trigger Timer3
}
//---------------------------------------------------------------
void InitialUART0_Timer3_Type6(UINT32 u32Baudrate) //use timer3 as Baudrate generator
{
    SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
    set_SMOD;        //UART0 Double Rate Enable
    T3CON &= 0xF8;   //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
    set_BRCK;        //UART0 baud rate clock source = Timer3
    
#ifdef FOSC_110592
    RH3    = HIBYTE(65536 - (691200/u32Baudrate));  /*11.0592 MHz */
    RL3    = LOBYTE(65536 - (691200/u32Baudrate));  /*11.0592 MHz */
#endif
#ifdef FOSC_184320
    RH3    = HIBYTE(65536 - (1152000/u32Baudrate)); /*18.4320 MHz */
    RL3    = LOBYTE(65536 - (1152000/u32Baudrate)); /*18.4320 MHz */
#endif
#ifdef FOSC_200000
    RH3    = HIBYTE(65536 - (1250000/u32Baudrate)); /*20.0000 MHz */
    RL3    = LOBYTE(65536 - (1250000/u32Baudrate)); /*20.0000 MHz */
#endif
#ifdef FOSC_221184
    RH3    = HIBYTE(65536 - (1382400/u32Baudrate)); /*22.1184 MHz */
    RL3    = LOBYTE(65536 - (1382400/u32Baudrate)); /*22.1184 MHz */
#endif
    set_TR3;         //Trigger Timer3
}
//---------------------------------------------------------------
void InitialUART1_Timer3(UINT32 u32Baudrate) //use timer3 as Baudrate generator
{
    SCON_1 = 0x52;   //UART1 Mode1,REN_1=1,TI_1=1
    T3CON &= 0xF8;   //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
    
#ifdef FOSC_110592  //max BR=230400 bps
    RH3    = HIBYTE(65536 - (691200/u32Baudrate));  /*11.0592 MHz */
    RL3    = LOBYTE(65536 - (691200/u32Baudrate));  /*11.0592 MHz */
#endif
#ifdef FOSC_184320
    RH3    = HIBYTE(65536 - (1152000/u32Baudrate)); /*18.4320 MHz */
    RL3    = LOBYTE(65536 - (1152000/u32Baudrate)); /*18.4320 MHz */
#endif
#ifdef FOSC_200000
    RH3    = HIBYTE(65536 - (1250000/u32Baudrate)); /*20.0000 MHz */
    RL3    = LOBYTE(65536 - (1250000/u32Baudrate)); /*20.0000 MHz */
#endif
#ifdef FOSC_221184  //max BR=460800 bps
    RH3    = HIBYTE(65536 - (1382400/u32Baudrate)); /*22.1184 MHz */
    RL3    = LOBYTE(65536 - (1382400/u32Baudrate)); /*22.1184 MHz */
#endif
    set_TR3;         //Trigger Timer3
}
/**************************************************************************
 *
 *  Function Name: Receive_Data_From_UART0()
 *
 *  Descriptions: Receive PC data to SBUF of N76E885
 *
 *  Arguments:
 *
 *  Returns: PC Data
 *
 **************************************************************************/
UINT8 Receive_Data_From_UART0(void)
{
    UINT8 c;
    
    while (!RI);
    c = SBUF;
    RI = 0;
    return (c);
}
/**************************************************************************
 *
 *  Function Name: Send_Data_To_UART0()
 *
 *  Descriptions: Send SBUF of N76E885 to PC
 *
 *  Arguments:
 *
 *  Returns: None
 *
 **************************************************************************/
void Send_Data_To_UART0 (UINT8 c)
{
    TI = 0;
    SBUF = c;
    while(TI==0);
}
/**************************************************************************
 *
 *  Function Name: Receive_Data_From_UART1()
 *
 *  Descriptions: Receive PC data to SBUF of N76E885
 *
 *  Arguments:
 *
 *  Returns: PC Data
 *
 **************************************************************************/
UINT8 Receive_Data_From_UART1(void)
{
    UINT8 c;
    
    while (!RI_1);
    c = SBUF_1;
    RI_1 = 0;
    return (c);
}
/**************************************************************************
 *
 *  Function Name: Send_Data_To_UART1()
 *
 *  Descriptions: Send SBUF of N76E885 to PC
 *
 *  Arguments:
 *
 *  Returns: None
 *
 **************************************************************************/
void Send_Data_To_UART1 (UINT8 c)
{
    TI_1 = 0;
    SBUF_1 = c;
    while(TI_1==0);
}
/*==========================================================================*/
void Set_All_GPIO_Quasi_Mode(void)
{
    P0M1 = 0x00;
    P0M2 = 0x00;
    P1M1 = 0x00;
    P1M2 = 0x00;
    P2M1 = 0x00;
    P2M2 = 0x00;
    P3M1 = 0x00;
    P3M2 = 0x00;
}

/*==========================================================================*/
void SW_Reset(void)
{
    TA = 0xAA;
    TA = 0x55;
    set_SWRST;
}
/*==========================================================================*/
void Fsys_Clock_Output_On_P26(void)
{
    set_CLOEN;                                  //System Clock Output Enable
}