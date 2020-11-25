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

//***********************************************************************************************************
//  File Function: N76E885 UART-0 Mode2 demo code
//***********************************************************************************************************

#include <stdio.h>
#include "N76E885.h"
#include "Typedef.h"
#include "Define.h"
#include "SFR_Macro.h"
 xdata volatile uint8_t uart_rcvbuf[64]; 
 xdata volatile uint8_t uart_txbuf[64];
 data volatile uint8_t bufhead=0;
 data volatile uint16_t flash_address=0; 
 data volatile uint16_t AP_size=0;
 data volatile uint8_t g_timer1Counter;
 data volatile uint8_t count; 
 data volatile uint16_t g_timer0Counter;
 data volatile uint32_t g_checksum;
 data volatile uint32_t g_totalchecksum;
 bit volatile bUartDataReady;
 bit volatile g_timer0Over;
 bit volatile g_timer1Over;
 bit volatile g_progarmflag;
#define TRUE	   	(1)
#define FALSE   	(0)
#define CMD_CONNECT			0xAE
#define CMD_SYNC_PACKNO		 0xA4
#define CMD_GET_FWVER		0xA6
#define FW_VERSION		0x27
#define CMD_RUN_APROM		0xAB
#define CMD_GET_DEVICEID	0xB1
#define CMD_READ_CONFIG		0xA2
#define CMD_UPDATE_APROM	0xA0
#define PAGE_ERASE_AP       0x22
#define BYTE_READ_AP        0x00
#define BYTE_PROGRAM_AP     0x21

#define     PAGE_SIZE           128

#define     APROM_SIZE          8*1024  

bit BIT_TMP;

void Package_checksum(void)
{
	g_checksum=0;
   for(count=0;count<64;count++)
	{
    g_checksum =g_checksum+ uart_rcvbuf[count];    
	}
	uart_txbuf[0]=g_checksum&0xff;
	uart_txbuf[1]=(g_checksum>>8)&0xff;
	uart_txbuf[4]=uart_rcvbuf[4]+1;
	uart_txbuf[5]=uart_rcvbuf[5];
	if(uart_txbuf[4]==0x00)
	uart_txbuf[5]++;
	
}

void Send_64byte_To_UART0(void)
{
   for(count=0;count<64;count++)
	{
   	TI = 0;
    SBUF = uart_txbuf[count];
    while(TI==0);
	}
}

void Serial_ISR (void) interrupt 4 
{
    if (RI == 1)
    {   
    uart_rcvbuf[bufhead++]=	SBUF;		
    clr_RI;                                         // Clear RI (Receive Interrupt).
    }
    if (TI == 1)
    {       
        clr_TI;                                         // Clear TI (Transmit Interrupt).
    }
		if(bufhead ==1)
		{
		g_timer1Over=0;
		g_timer1Counter=90; //for check uart timeout using
		}
	if(bufhead == 64)
		{
			
			bUartDataReady = TRUE;
			g_timer1Counter=0;
			g_timer1Over=0;
			bufhead = 0;
			P10=P10^1;
		}		
}

void Timer0_ISR (void) interrupt 1
{
if(g_timer0Counter)
  {
	g_timer0Counter--;
		if(!g_timer0Counter)
		{
		g_timer0Over=1;
		}
	}
	
	if(g_timer1Counter)
  {
	g_timer1Counter--;
		if(!g_timer1Counter)
		{
		g_timer1Over=1;
		}
	}
	
} 
/************************************************************************************************************
*    Main function 
************************************************************************************************************/
void main (void)
{
   //initial GPIO for quasi mode.  
	 P0M1 = 0x00;
   P0M2 = 0x00;
   P1M1 = 0x00;
   P1M2 = 0x00;
   P2M1 = 0x00;
   P2M2 = 0x00;
   P3M1 = 0x00;
   P3M2 = 0x00;
	 //uart initial 
   SCON = 0x52;     //UART0 Mode1,REN=1,TI=1
   TMOD |= 0x22;    //Timer1 Mode1, TIMER 0 MODE2
    
   set_SMOD;
   set_T1M;
   clr_BRCK;        //Serial port 0 baud rate clock source = Timer1
	 TH1 = 256 - (1382400/115200);                /*22.1184 MHz */
   set_TR1; 					//Start timer1
	    
		TH0=TL0=0; //interrupt timer 140us
		set_TR0;   //Start timer0
		set_PSH;   // Serial port 0 interrupt level2
		IE = 0x92;                                      //EA=1, ES=1,ET0=1
		//IE = 0x90;                                      //EA=1, ES=1,ET0=1
		g_timer0Over=0;
		g_timer0Counter=5000;
		g_progarmflag=0;

while(1)
{
	    	if(bUartDataReady == TRUE)
				{
				EA=0; //DISABLE ALL INTERRUPT									
				P11=P11^1;
				if(g_progarmflag==1)
				{

				for(count=8;count<64;count++)
			  {
				IAPCN = BYTE_PROGRAM_AP;
				IAPAL = flash_address&0xff;
        IAPAH = (flash_address>>8)&0xff;
				IAPFD=uart_rcvbuf[count];
				//trigger IAP
				TA=0xAA;
				TA=0x55;
				IAPTRG |= SET_BIT0 ;                                 
    
				IAPCN = BYTE_READ_AP;
   			//trigger IAP
				TA=0xAA;
				TA=0x55;
				IAPTRG |= SET_BIT0 ;                                  //trigger IAP
				if(IAPFD!=uart_rcvbuf[count])
        while(1);					
				g_totalchecksum=g_totalchecksum+uart_rcvbuf[count];
				flash_address++;
				if(flash_address==AP_size)
				{
					g_progarmflag=0;
           goto END_2;					
				}
				}
			  
END_2:	        			
				Package_checksum();
			  uart_txbuf[8]=g_totalchecksum&0xff;
	      uart_txbuf[9]=(g_totalchecksum>>8)&0xff;
				Send_64byte_To_UART0();
    				
				}
				
				
					
					switch(uart_rcvbuf[0])
					{								
						case CMD_CONNECT:
						case CMD_SYNC_PACKNO:
						{
						Package_checksum();

						Send_64byte_To_UART0();		
				    g_timer0Counter=0; //clear timer 0 for no reset
						g_timer0Over=0;
//set_IAPEN;
TA=0xAA;
TA=0x55;
CHPCON|=SET_BIT0;
//set_APUEN;
TA=0xAA;
TA=0x55;
IAPUEN|=SET_BIT0;							
						break;
						}
												
						case CMD_GET_FWVER:						
						{
						Package_checksum();
						uart_txbuf[8]=FW_VERSION;	
						Send_64byte_To_UART0();	
						break;
						}
            
						case CMD_RUN_APROM:						
						{
						//Package_checksum();
						//uart_txbuf[8]=FW_VERSION;	
						//Send_64byte_To_UART0();	
							goto _APROM;
						break;
						}
						
						
						//please tool must define devices value
						case CMD_GET_DEVICEID:						
						{
						Package_checksum();
						uart_txbuf[8]=0x00;	
						uart_txbuf[8]=0x5a;	
						uart_txbuf[8]=0x00;	
						uart_txbuf[8]=0x20;	
						Send_64byte_To_UART0();	
						break;
						}
						
            
						case CMD_READ_CONFIG:						
						{
						Package_checksum();
						uart_txbuf[8]=0x7f;	
						uart_txbuf[9]=0xff;	
						uart_txbuf[10]=0xff;	
						uart_txbuf[11]=0xff;	
					  uart_txbuf[12]=0xff;	
						uart_txbuf[13]=0xff;	
						uart_txbuf[14]=0xff;						
						uart_txbuf[15]=0xff;
						Send_64byte_To_UART0();	
						break;
						}
						
						case CMD_UPDATE_APROM:						
						{
	//lucia modify
								set_IAPEN;
							IAPFD = 0xFF;    
							IAPCN = PAGE_ERASE_AP;
							set_APUEN;
							for(flash_address=0x0000;flash_address<APROM_SIZE/PAGE_SIZE;flash_address++)
							{        
									  IAPAL = LOBYTE(flash_address*PAGE_SIZE);
										IAPAH = HIBYTE(flash_address*PAGE_SIZE);
									set_IAPGO;  
							}						
							
	//====================				
							
//       IAPCN = PAGE_ERASE_AP;		
//							
//				set_APUEN;								
//					  //aprom erase
//				    for(flash_address=0x0000;flash_address<APROM_SIZE/PAGE_SIZE;flash_address++)
//            {        
//        IAPAL = LOBYTE(flash_address*PAGE_SIZE);
//        IAPAH = HIBYTE(flash_address*PAGE_SIZE);
//				//trigger IAP
//				TA=0xAA;
//				TA=0x55;
//				IAPTRG |= SET_BIT0 ;
//			}
						g_totalchecksum=0;
				flash_address=0;
			  AP_size=0;
			  AP_size=uart_rcvbuf[12];
			  AP_size|=(uart_rcvbuf[13]<<8);	
				g_progarmflag=1;

			  for(count=16;count<64;count++)
			  {
				IAPCN = BYTE_PROGRAM_AP;
				IAPAL = flash_address&0xff;
        IAPAH = (flash_address>>8)&0xff;
				IAPFD=uart_rcvbuf[count];
				//trigger IAP
				TA=0xAA;
				TA=0x55;
				IAPTRG |= SET_BIT0 ;                                 
    
				IAPCN = BYTE_READ_AP;
   			//trigger IAP
				TA=0xAA;
				TA=0x55;
				IAPTRG |= SET_BIT0 ;                                  //trigger IAP
				if(IAPFD!=uart_rcvbuf[count])
        while(1);					
				g_totalchecksum=g_totalchecksum+uart_rcvbuf[count];
				flash_address++;
				if(flash_address==AP_size)
				{
					g_progarmflag=0;
           goto END_1;					
				}
				}
			  
END_1:	        			
				Package_checksum();
			  uart_txbuf[8]=g_totalchecksum&0xff;
	      uart_txbuf[9]=(g_totalchecksum>>8)&0xff;
				Send_64byte_To_UART0();	
				break;
				}
						
					}	
			    bUartDataReady = FALSE;
				  bufhead = 0;						
					EA=1;
			}
			//For connect timer out	
			#if 1
      if(g_timer0Over==1)
			{			 
			 goto _APROM;
			}
			#endif
			
	    //for uart time out or buffer error
     	if(g_timer1Over==1)
			{			 
			 if((bufhead<64)&&(bufhead>0)||(bufhead>64))
			   {
				 bufhead=0;				 
			   }
			}	
	
	
}   



_APROM:

		TA = 0xAA;
		TA = 0x55;
		CHPCON &= 0xFD;                  //set boot from AP
		TA = 0xAA;
		TA = 0x55;
		CHPCON |= 0x80;                   //software reset enable

	  /* Trap the CPU */
		while(1);	
}


