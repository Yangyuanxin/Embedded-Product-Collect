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
#include "N76E003.h"
#include "Typedef.h"
#include "Define.h"
#include "SFR_Macro.h"
#include "UART0_transfer.h"
#include "ISP_USER.h"

sfr RCTRIM0		= 0x84;
sfr RCTRIM1		= 0x85;	


unsigned char hircmap0,hircmap1;
unsigned int	trimvalue16bit;

unsigned char temp0;
unsigned char idata temp1  _at_ 0x90;
unsigned char xdata temp2;
unsigned char PID_highB,PID_lowB,CONF0,CONF1,CONF2,CONF4;
unsigned char recv_CONF0,recv_CONF1,recv_CONF2,recv_CONF4;



void READ_ID_CONFIG(void)
{
	  set_IAPEN;
    IAPCN = BYTE_READ_ID;
		IAPAL = 0x00;
		IAPAH = 0x00;
		TA=0xAA;		//trigger IAP
		TA=0x55;
		IAPTRG |= SET_BIT0;
		PID_lowB = IAPFD;
		IAPAL = 0x01;
		TA=0xAA;		//trigger IAP
		TA=0x55;
		IAPTRG |= SET_BIT0;
		PID_highB = IAPFD;

		IAPCN = BYTE_READ_CONFIG;
		IAPAL = 0x00;
		IAPAH = 0x00;
		TA=0xAA;		//trigger IAP
		TA=0x55;
		IAPTRG |= SET_BIT0;
		CONF0 = IAPFD;
		IAPAL = 0x01;
		TA=0xAA;		//trigger IAP
		TA=0x55;
		IAPTRG |= SET_BIT0;
		CONF1 = IAPFD;
		IAPAL = 0x02;
		TA=0xAA;		//trigger IAP
		TA=0x55;
		IAPTRG |= SET_BIT0;
		CONF2 = IAPFD;
		IAPAL = 0x04;
		TA=0xAA;		//trigger IAP
		TA=0x55;
		IAPTRG |= SET_BIT0;
		CONF4 = IAPFD;
		clr_IAPEN;
}

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

void MODIFY_HIRC_16588(void)
{
		set_IAPEN;
		IAPAL = 0x30;
    IAPAH = 0x00;
    IAPCN = READ_UID;
    set_IAPGO;
		hircmap0 = IAPFD;
		IAPAL = 0x31;
    IAPAH = 0x00;
    set_IAPGO;
		hircmap1 = IAPFD;
		clr_IAPEN;
		hircmap1 = hircmap1&0x01;
		trimvalue16bit = ((hircmap0<<1)+(hircmap1&0x01));
		trimvalue16bit = trimvalue16bit - 15;
		hircmap1 = trimvalue16bit&0x01;
		hircmap0 = trimvalue16bit>>1;
		TA=0XAA;
		TA=0X55;
		RCTRIM0 = hircmap0;
		TA=0XAA;
		TA=0X55;
		RCTRIM1 = hircmap1;
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
		MODIFY_HIRC_16588();
	 //uart initial 
		READ_ID_CONFIG();
//	UART0_ini_38400();
		UART0_ini_115200();
//		TM0_ini();

		temp1 = 0xaa;
		g_timer0Over=0;
		g_timer0Counter=5000;
		g_progarmflag=0;

while(1)
{
	    	if(bUartDataReady == TRUE)
				{
					EA=0; //DISABLE ALL INTERRUPT									
					if(g_progarmflag==1)
					{
						for(count=8;count<64;count++)
						{
							IAPCN = BYTE_PROGRAM_AP;					//program byte
							IAPAL = flash_address&0xff;
							IAPAH = (flash_address>>8)&0xff;
							IAPFD=uart_rcvbuf[count];
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0 ;   

					
							IAPCN = BYTE_READ_AP;							//program byte verify
							TA=0xAA;			//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0 ;  
							if(IAPFD!=uart_rcvbuf[count])
							while(1);													
							if (CHPCON==0x43)							//if error flag set, program error stop ISP
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
							
//							TA=0xAA;						//set_IAPEN;
//							TA=0x55;
//							CHPCON|=SET_BIT0;
//							TA=0xAA;						//set_APUEN;
//							TA=0x55;
//							IAPUEN|=SET_BIT0;							
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
							uart_txbuf[8]=PID_lowB;	
							uart_txbuf[9]=PID_highB;	
							uart_txbuf[10]=0x00;	
							uart_txbuf[11]=0x00;	
							Send_64byte_To_UART0();	
						break;
						}
	
						case CMD_READ_CONFIG:						
						{
							Package_checksum();

							uart_txbuf[8]=CONF0;	
							uart_txbuf[9]=CONF1;	
							uart_txbuf[10]=CONF2;	
							uart_txbuf[11]=0xff;	
							uart_txbuf[12]=CONF4;	
							uart_txbuf[13]=0xff;	
							uart_txbuf[14]=0xff;						
							uart_txbuf[15]=0xff;
							Send_64byte_To_UART0();	
						break;
						}
						
						case CMD_UPDATE_CONFIG:
						{
							recv_CONF0 = uart_rcvbuf[8];
							recv_CONF1 = uart_rcvbuf[9];
							recv_CONF2 = uart_rcvbuf[10];
							recv_CONF4 = uart_rcvbuf[12];
							
							set_IAPEN;
							set_CFUEN;
							IAPCN = PAGE_ERASE_CONFIG;
							IAPAL = 0x00;
							IAPAH = 0x00;
							IAPFD = 0xFF;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;

							IAPCN = BYTE_PROGRAM_CONFIG;
							IAPAL = 0x00;
							IAPAH = 0x00;
							IAPFD = recv_CONF0;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;
							IAPFD = recv_CONF1;
							IAPAL = 0x01;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;
							IAPAL = 0x02;
							IAPFD = recv_CONF2;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;
							IAPAL = 0x04;
							IAPFD = recv_CONF4;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;
							clr_CFUEN;

							IAPCN = BYTE_READ_CONFIG;
							IAPAL = 0x00;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;
							CONF0 = IAPFD;
							IAPAL = 0x01;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;
							CONF1 = IAPFD;
							IAPAL = 0x02;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;
							CONF2 = IAPFD;
							IAPAL = 0x04;
							TA=0xAA;		//trigger IAP
							TA=0x55;
							IAPTRG |= SET_BIT0;
							CONF4 = IAPFD;
							clr_IAPEN;
							
							Package_checksum();
							uart_txbuf[8]=CONF0;	
							uart_txbuf[9]=CONF1;	
							uart_txbuf[10]=CONF2;	
							uart_txbuf[11]=0xff;	
							uart_txbuf[12]=CONF4;	
							uart_txbuf[13]=0xff;	
							uart_txbuf[14]=0xff;						
							uart_txbuf[15]=0xff;
							Send_64byte_To_UART0();	
							break;
						}
						
						case CMD_UPDATE_APROM:						
						{
						
							TA=0xAA;	//set_IAPEN;		
							TA=0x55;
							CHPCON |= SET_BIT0 ;	
							TA=0xAA;	//set_APUEN;
							TA=0x55;
							IAPUEN |= SET_BIT0;
							
							IAPFD = 0xFF;    			//Erase must set IAPFD = 0xFF
							IAPCN = PAGE_ERASE_AP;
							
							for(flash_address=0x0000;flash_address<APROM_SIZE/PAGE_SIZE;flash_address++)
							{        
								IAPAL = LOBYTE(flash_address*PAGE_SIZE);
								IAPAH = HIBYTE(flash_address*PAGE_SIZE);
								TA=0xAA;			//trigger IAP
								TA=0x55;
								IAPTRG |= SET_BIT0 ;
							}						
							
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
			
								IAPCN = BYTE_READ_AP;								//program byte verify
								//trigger IAP
								TA=0xAA;
								TA=0x55;
								IAPTRG |= SET_BIT0 ;                               
								if(IAPFD!=uart_rcvbuf[count])
								while(1);					
								if (CHPCON==0x43)								//if error flag set, program error stop ISP
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
			#if 0
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


