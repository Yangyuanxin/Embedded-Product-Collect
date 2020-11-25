#include <stdio.h>
#include "N76E003.h"
#include "Common.h"
//#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_Define.h"
#include "time.h"
#include "pwm.h"
#include "adc.h"
volatile uint8_t  time0l;
volatile uint8_t  time0h;
volatile uint8_t  time1l;
volatile uint8_t  time1h;
volatile uint8_t  time11h;
volatile uint8_t  time11l;
volatile uint8_t pow_count;
volatile uint8_t mode_count;
volatile uint8_t rise_shift;
volatile uint8_t mode_sel;
volatile uint8_t buz_flag;
volatile uint16_t buz_count;
volatile uint8_t flash_flag;
volatile uint8_t key_flag;
volatile uint16_t time2_count;
volatile uint16_t wait_count;

 uint8_t moto_flag;
 uint8_t new_flag;
 uint8_t wait_flag;
 uint8_t on_flag;
 uint8_t nokey_count;
 uint8_t start_sound;
 uint8_t close_sound;
 uint8_t shift_sound;
 uint8_t mode_sound;
 uint8_t power_keyup_flag=1;
 uint8_t led_keyup_flag=1;
 uint8_t buzfirsh_flag=1;

/************************************************************************************************************
*用于主事件执行,升压使PWM产生不同占空比的PWM波
************************************************************************************************************/
void Timer0_ISR (void) interrupt 1          //interrupt address is 0x000B
{
    clr_TR0;
    TH0 = time0h;
    TL0 = time0l;  
    if(POW_RF==1)
		{
		  power_keyup_flag=1;
		}	
		if(MODE==1)
		{
		  led_keyup_flag=1;
		}	
	  set_TR1;
    if(POW_RF==0)
		{    
			 pow_count++;
			 if(pow_count==2)
			 {	   
				 	 pow_count=0;
			     time2_count=0;
				   key_flag=1;
				   wait_count=0;
			    if(power_keyup_flag==1)
					 {
			       rise_shift++;
						 power_keyup_flag=0;
             buzfirsh_flag=1;						 
					 }
				   
				 if(rise_shift==1)//rise voltage1 start 
				 {	 
				   set_TR2;
				   on_flag=1;
					 buz_flag=1;
					 FDC633_CTL=1;
					 AIC3415_CTL=1;
					 voltage1();
					// P04=0;	 
					 LED1=0;
					 flash_flag=0;
					 mode_sel=1;
					 LED2=LED3=LED4=1;
					 RED=YELLOW=BLUE=1;
					 PWM1_P14_OUTPUT_ENABLE; //MOS;
					 pwm_start();
				   //while(POW_RF==0); 
				 }
				 if(rise_shift==2)//rise voltage2
				 {		 // wait_count=0;
				   on_flag=1;
					 buz_flag=2;
					 voltage2();
					 LED2=0;
					 LED1=LED3=LED4=1;
				   pwm_start(); 
					 //while(POW_RF==0);
				
				 }
				 if(rise_shift==3)//rise voltage3
				 {
				   wait_count=0;
				   on_flag=1;
					 buz_flag=2;
					 voltage3();
					 LED3=0;
					 LED1=LED2=LED4=1;
					 pwm_start();
				//while(POW_RF==0);
				 }
				 if(rise_shift==4)//rise voltage4
				 {
				    // wait_count=0;
				   on_flag=1;
					 buz_flag=2;
					 voltage4();
					 LED4=0;
					 LED1=LED2=LED3=1;
					 pwm_start();  
		      //while(POW_RF==0);
				 }
				 if(rise_shift==5)//
				 {
					 on_flag=0;
					 buz_flag=3;
					 LED1=1;
					 LED2=1;
					 LED3=1;
					 LED4=1;  
					//  PCON|=0X02;      
					 RED=YELLOW=BLUE=0;
					 pwm_stop();
					 rise_shift=0; 
					 PWM1_P14_OUTPUT_DISABLE;
					 P14_PushPull_Mode;
					 P14=0;
					 
					 //while(POW_RF==0);
				 }
			  	 
		  }	 
		}
    if(MODE==0)
		{
			 mode_count++;
			 if(mode_count==2)
			 {	   
			  	mode_count=0;
				  time2_count=0;
			   if(on_flag)
			   {
					{ 
						key_flag=2;
						if(led_keyup_flag==1)
					 {
			       mode_sel++;
						 led_keyup_flag=0;
             buzfirsh_flag=1;						 
					 }
						wait_count=0;
						if(mode_sel==1)
						{	
							buz_flag=4;
							RED=YELLOW=BLUE=1;
							flash_flag=0;//no flash
							moto_flag=0;
							moto_stop();
					    //while(MODE==0);
						}
					if(mode_sel==2)
					{
						buz_flag=4;
						flash_flag=1;   //second shift:three color led is flash,and flash_flag is 1		  enter time2 isr
						moto_flag=0;
						set_TR2;
						moto_stop();
							//while(MODE==0);
					}
					if(mode_sel==3)
					{
						buz_flag=4;
						flash_flag=0;//no flash，red led is on
						moto_flag=0;
						RED=1;
						YELLOW=0;
						BLUE=0;
						moto_stop();
							//while(MODE==0);
						
					}
					if(mode_sel==4)
					{
						buz_flag=4;
						flash_flag=0;//no flash  yellow led is on
						moto_flag=0;
						YELLOW=1;
						RED=0;
						BLUE=0;
						moto_stop();
						
           	//while(MODE==0);
					}
					if(mode_sel==5)
					{
						buz_flag=4;
						flash_flag=0;//no flash blue led is on
						moto_flag=0;
						BLUE=1;
						RED=0;
						YELLOW=0;
						moto_stop();
						
						//while(MODE==0);
					}
					if(mode_sel==6)//three color led is flash,and flash_flag is 1,and moto is on
					{
						set_TR2;
						buz_flag=4;
						flash_flag=1;//flash 
				    moto_run();
				  	BLUE=0;
						RED=0;
						YELLOW=0;
							//while(MODE==0);
					
					}
				  if(mode_sel==7)
					{
						mode_sel=0;
						buz_flag=4;
						flash_flag=0;//no flash
						moto_flag=0;
						RED=YELLOW=BLUE=0;
						moto_stop();
						//while(MODE==0);
						
					}
				 }
			  }	 
			 }
		}	
		set_TR0;
		if(key_flag==0) //if no key operattion,
		{     			   
	    clr_TR2;
		}
		else 
			{	set_TR2;	}
}
/************************************************************************************************************
* OUT BUZZER SOUND
************************************************************************************************************/
void Timer1_ISR (void) interrupt 3          //interrupt address is 0x001B
{
	if(buzfirsh_flag==0)
	{
		return;
	}
  clr_TR1;
  if(new_flag==0)
	 { 
	   TL1=time11l;
	   TH1=time11h;
	 }

	//------------------------------------
 if(buz_flag==1)  //start system ,and continue sound
 {		 
	 new_flag=1;   //     
	 TH1 = time11h;
   TL1 = time11l; 
	 buz_count++;
	 
   if(buz_count<2560)//2569
	 {
		 clr_TR0;
	 }else{
	   set_TR0;
	 }
	 
	 if((buz_count<500))//&&(buz_count>666))
	 {
		 time11h = (65536-MI)/256;
     time11l = (65536-MI)%256;
		 TL1=time11l;
	   TH1=time11h;
		 BUZ=~BUZ;
	 }
	 if((buz_count<980)&&(buz_count>500))
	 {
		 time11h = (65536-FA)/256;
     time11l = (65536-FA)%256;
		 BUZ=~BUZ;
	 }
	 if((buz_count<1449)&&(buz_count>979))
	 
	 {
		 time11h = (65536-SO)/256;
     time11l = (65536-SO)%256;
		 BUZ=~BUZ;
	 }
	if((buz_count<1978)&&(buz_count>1448))
	 {
		 time11h = (65536-LA)/256;
     time11l = (65536-LA)%256;
		 BUZ=~BUZ; 
	 }
	 if((buz_count<2569)&&(buz_count>1977))
	 {
		 time11h = (65536-SI)/256;
     time11l = (65536-SI)%256;
		 BUZ=~BUZ;
	 }
	  if(buz_count>2569)
 	  {
			buz_flag=0;BUZ=0;buz_count=0;
			start_sound=1;
			buzfirsh_flag=0;
    }  
	
 }
 if(buz_flag==2)   //rise voltage,second to four shift sound is setting
 {
      new_flag=1;
	 TH1 = time11h;
   TL1 = time11l; 
	 buz_count++;
	 if((buz_count<529))
	 {
		 time11h = (65536-LA)/256;
     time11l = (65536-LA)%256;
		 BUZ=~BUZ;	 
	 }
     if(buz_count>529)
 	  {buz_flag=0;BUZ=0;buz_count=0;
			buzfirsh_flag=0;
    } //After 
	
 }
 if(buz_flag==3)    //close and fifth shift sound is setting
 {	
   new_flag=1;
	 TH1 = time11h;
   TL1 = time11l; 
	 buz_count++;
	 
	 if(buz_count<2392)//2401
	 {
		 clr_TR0;
	 }else{
	   set_TR0;
	 }
	 
	 if(buz_count<591)
	 {
		 time11h = (65536-SI)/256;
     time11l = (65536-SI)%256;
		 BUZ=~BUZ;
	 }
	 if((buz_count<1120)&&(buz_count>591))
	 {
		 time11h = (65536-LA)/256;
         time11l = (65536-LA)%256;
		 BUZ=~BUZ;
	 }
	 if((buz_count<1589)&&(buz_count>1120))
	 {
		 time11h = (65536-SO)/256;
         time11l = (65536-SO)%256;
		 BUZ=~BUZ;
	 }
	 if((buz_count<2007)&&(buz_count>1589))
	 {
		 time11h = (65536-FA)/256;
     time11l = (65536-FA)%256;
		 BUZ=~BUZ;
	 }
	 if((buz_count<2402)&&(buz_count>2007))
	 {
		 time11h = (65536-MI)/256;
     time11l = (65536-MI)%256;
		 BUZ=~BUZ; 
	 }

	  if(buz_count>2401)
 	  {
			buzfirsh_flag=0;
	    buz_flag=0;BUZ=0;buz_count=0;   //close system
	    FDC633_CTL=0;
   	  AIC3415_CTL=0;
			key_flag=0;
			//clr_TR0;
	  } //After 
	 
 }
  
 if(buz_flag==4)    //sound of color shift is setting
 {
       TH1 = time11h;
       TL1 = time11l; 
       new_flag=1;
	     buz_count++;
	  if((buz_count<469))
	 {
		 time11h = (65536-SO1)/256;
     time11l = (65536-SO1)%256;
		 BUZ=~BUZ;
		 
	 }
          if(buz_count>469)
 	       {
					 buzfirsh_flag=0;
					 buz_flag=0;BUZ=0;buz_count=0;
					
          }   
 }	
 if(buz_flag==5)  //no operation sound is setting,buzzer sound
 {  
      
       TH1 = time11h;
       TL1 = time11l; 
       new_flag=1;
  if(wait_flag==0)
   {
   	  buz_count++;
			if((buz_count<418))//591
			 {
				 time11h = (65536-SI)/256;
				 time11l = (65536-SI)%256;
				 BUZ=~BUZ;
					 if(on_flag)
					 set_TR2;
					 else
					 clr_TR2;
				 
			 }
			 else  {buz_flag=0;wait_flag=1; buz_count=0;BUZ=0;} //After 
    }
 }	
  //buzfirsh_flag=0;
  
  set_TR1;
  if(buz_flag==0)
     new_flag=0;
}
/*********************************************
*函数名称：void Timer2_ISR (void) interrupt 5 
*功能描述：用于蜂鸣器发声
**********************************************/
void Timer2_ISR (void) interrupt 5    
{
	 clr_TF2;
	 time2_count++;
	 wait_count++;
	 if(flash_flag==1)  //led flash
    { 
	   BLUE=~BLUE;YELLOW=~YELLOW;RED=~RED;
	  } 
	 if(wait_count==100)  //waitting time is 10s
	 {
		  buzfirsh_flag=1;
		  wait_count=0;
		  buz_flag=5;       //10s BUZ A TIME
		  BUZ=0;
		  set_TR3;			//检测电池电压
		  wait_flag=0;
	 }
	 
	 if(time2_count>6000)  //ten minutes  auto close system
	 {    
		  buzfirsh_flag=1;
	    on_flag=0;
	    key_flag=0;
		  buz_flag=3;
		  time2_count=0;
		  rise_shift=0;
	    mode_count=0;
		  clr_TR2;
		  clr_TR3;
		  
	   // AIC3415_CTL=0;
		  LED1=LED2=LED3=LED4=1;
		  RED=YELLOW=BLUE=0;
		  pwm_stop();
		  PWM1_P14_OUTPUT_DISABLE;
			P14_PushPull_Mode;
			P14=0;
		  P04=0;
	 }
	 
}

void Timer3_ISR (void) interrupt 16
{
	uint16_t adresult[16];
  uint8_t cn;
	uint16_t ad_value;
	clr_TF3;
	ad_value=0;
		
	for(cn=0;cn<8;cn++)
	 {
		 //adresult[cn]=Bat_Measure();
	   //ad_value=ad_value+adresult[cn];
	 }
	  //ad_value=ad_value/8;
	  if(ad_value<1918)
		{
		  //buz_flag=3;
		  //on_flag=0;
		  //key_flag=0;
		  //rise_shift=0;
		  //time2_count=0;
		  //clr_TR2;
		  //LED1=LED2=LED3=LED4=1;
			//RED=YELLOW=BLUE=0;
		  //pwm_stop(); 
		}
	  
	 clr_TR3;
	 ADCCON1|=SET_BIT0;
}

void Time_Init(void)
{
   
     TIMER0_MODE1_ENABLE;
	   TIMER1_MODE1_ENABLE;
	   TIMER2_Auto_Reload_Delay_Mode;
   	  
	   TIMER2_DIV_32;
	   TIMER3_DIV_8;
	   time0h = (65536-TMR0_INIT)/256;
     time0l = (65536-TMR0_INIT)%256;    
     time1h = (65536-TMR1_INIT)/256;
     time1l = (65536-TMR1_INIT)%256;
	   time11h = time1h;
	   time11l = time1l;
	   RCMP2H=(65536-TMR2_INIT)/256;
	   RCMP2L=(65536-TMR2_INIT)%256;//200ms,f=fsys/32
	   RH3=(65536-TMR3_INIT)/256;
	   RL3=(65536-TMR3_INIT)%256;
	
	   TL0=time0l;
	   TH0=time0h;
	   TL1=time1l;
	   TH1=time1h;
	  
       clr_T0M;   //T0clk=Fsys
       clr_T1M;   //T1clk=Fsys
       set_ET0;                                    //enable Timer0 interrupt
       set_ET1;  
       set_ET2;		                               //enable Timer2 interrupt
	   set_ET3;
                                          //enable interrupts
	   set_EA; 
       set_TR0;                                    //Timer0 run
       set_TR1;   
       clr_TR2;	
       clr_TR3;	
	    	 
}