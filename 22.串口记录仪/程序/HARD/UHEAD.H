#ifndef	__UHEAD_H
#define	__UHEAD_H

#include "main.h"
#include "stm32f2xx_hal_gpio.h"

#define			GETSIZE(x)		(sizeof(x)/sizeof(x[0]))			//获得数组大小


typedef	unsigned char 	   u8;
typedef	unsigned short 	u16;
typedef	unsigned int 	   u32;


#define     DEBUG_LEVEL   1        

#if (DEBUG_LEVEL > 0)
#include "usbd_cdc_if.h"//#include "stdio.h"
#define  debug(...) usb_printf(__VA_ARGS__)//printf(__VA_ARGS__);
#else
#define debug(...)   
#endif 



//信号引脚
#define			GPIO_SET(pin)			HAL_GPIO_WritePin(pin##_GPIO_Port,  pin##_Pin,GPIO_PIN_SET)
#define			GPIO_RESET(pin)			HAL_GPIO_WritePin(pin##_GPIO_Port,  pin##_Pin,GPIO_PIN_RESET)
#define			GPIO_Toggle(pin)		HAL_GPIO_TogglePin(pin##_GPIO_Port,  pin##_Pin)
#define			GPIO_READ(pin)			HAL_GPIO_ReadPin(pin##_GPIO_Port,  pin##_Pin)


#endif
