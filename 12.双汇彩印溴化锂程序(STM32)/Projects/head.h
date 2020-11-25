//USER_TYPE

#ifndef _HEAD_H
#define _HEAD_H

#ifndef _HEAD_C
#define EXT_HEAD extern 
#else
#define EXT_HEAD
#endif

#define _FLASH_PROG

#include "os_cpu.h"
#include "app_cfg.h"

//-----------------------------
#include "stm32f10x_conf.h"

#include "stm32f10x_adc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_crc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_fsmc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_map.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_systick.h"
#include "stm32f10x_type.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stdio.h"

#include "stm32f10x_it.h"
//----------------------------

#include "para.h"

#include "SPI_12864LCD.H"
#include "view.h"

#include "uart.h"
#include "measure.h"
#include "led.h"
#include "relay.h"
#include "PAIWU.h"

#include "ki.h"
#include "MODBUS.H"
#include "MODBUS_MASTER.H"

#include "key.h"

#include "iap.h"






//gui
#include "GUI_CONFIG.H"
#include "windows.h"
#include "convertcolor.h"
#include "font_macro.h"
#include "font24_32.h"
#include "font5_7.h"
#include "font8_8.h"
#include "font16_16.h"
#include "gui_basic.h"
#include "gui_stockc.h"
#include "loadbit.h"
#include "menu.h"
#include "spline.h"

#include "lcd_interface.h"    //add it 2010-03-19 by hyman
/*
#include "cursor.h"


#include "ocx.h"
#include "FORM.H"
#include "SetFocus.h"
#include "GetFocus.h"
#include "LossFocus.h"
#include "qbch.h"
#include "KeyProcess.h"
#include "InitSize.h"
#include "Execute.h"

#include "USER.H"

#include "all_para.h"*/
#include "lcd_protocol.h"
#include "moudle.h"
#include "comm.h"




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define TRACE(str) send_str(COM1,str);


#endif

