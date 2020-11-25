#ifndef __INCLUDE_CONF_H
#define __INCLUDE_CONF_H

#include "stm32f10x.h"

//USER
//#include "marlin_main.h"
#include "Configuration.h"
#include "Configuration_adv.h"
#include "ConfigurationStore.h"
#include "language.h"
#include "pins.h"
#include "cardreader.h"

//SOFTWARE
#include "picdecode.h"
//#include "menu_conf.h"
//#include "gui_conf.h"
#include "malloc.h"  
#include "ff.h"  
#include "exfuns.h"
#include "fontupd.h"
#include "fontshow.h"

//HARDWARE
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "beep.h"
#include "24cxx.h"
#include "flash.h"
#include "rtc.h"
#include "sram.h"
#include "planner.h"
#include "stepper.h"
#include "motion_control.h"
#include "temperature.h"
#include "sdio_sdcard.h"
#include "lcd12864.h"
#include "lcd12864_menu.h"


//SYSTEM
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "iic.h"
#include "spi.h"
#include "watchdog.h"
#include "exti.h"





#endif //__INCLUDE_H

