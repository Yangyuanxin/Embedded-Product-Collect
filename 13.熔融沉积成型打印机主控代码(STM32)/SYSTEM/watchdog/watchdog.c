#include "watchdog.h"



//#ifdef USE_WATCHDOG
//初始化独立看门狗
//prer:分频数:0~7(只有低3位有效!)
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).
void IWDG_Init(u8 prer,u16 rlr) 
{
	IWDG->KR=0X5555;//使能对IWDG->PR和IWDG->RLR的写		 										  
  	IWDG->PR=prer;  //设置分频系数   
  	IWDG->RLR=rlr;  //从加载寄存器 IWDG->RLR  
	IWDG->KR=0XAAAA;//reload											   
  	IWDG->KR=0XCCCC;//使能看门狗	
}

/// intialise watch dog with a 4 sec interrupt time
void watchdog_init(void)
{
//#ifdef WATCHDOG_RESET_MANUAL
    //We enable the watchdog timer, but only for the interrupt.
    //Take care, as this requires the correct order of operation, with interrupts disabled. See the datasheet of any AVR chip for details.
   /*
    wdt_reset();
    _WD_CONTROL_REG = _BV(_WD_CHANGE_BIT) | _BV(WDE);
    _WD_CONTROL_REG = _BV(WDIE) | WDTO_4S;
	*/
//#else
   	IWDG_Init(6,625);    	//与分频数为256,重载值为625,溢出时间为4s	   
//#endif
}
/// reset watchdog. MUST be called every 1s after init or avr will reset.
void watchdog_reset(void) 
{
    	IWDG->KR=0XAAAA;//
}

//===========================================================================
//=============================ISR               ============================
//===========================================================================

//Watchdog timer interrupt, called if main program blocks >1sec and manual reset is enabled.

//#ifdef WATCHDOG_RESET_MANUAL
/*
ISR(WDT_vect)
{ 
    //TODO: This message gets overwritten by the kill() call
	
    LCD_ALERTMESSAGEPGM("ERR:Please Reset");//16 characters so it fits on a 16x2 display
    lcd_update();
    SERIAL_ERROR_START;
    SERIAL_ERRORLNPGM("Something is wrong, please turn off the printer.");
    kill(); //kill blocks
    while(1); //wait for user or serial reset
	
}
*/
//#endif//RESET_MANUAL

//#endif//USE_WATCHDOG
