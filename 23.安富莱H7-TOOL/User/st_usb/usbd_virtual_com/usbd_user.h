/*
*********************************************************************************************************
*
*	模块名称 : USB devie 虚拟串口驱动
*	文件名称 : usbd_user.h
*	版    本 : V1.0
*	说    明 : 封装虚拟串口操作函数，提供给APP使用.
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2018-12-11  armfly  正式发布
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_USER_H
#define __USBD_USER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h" 
#include "usbd_cdc_interface.h"

void usbd_OpenCDC(uint8_t _com);
void usbd_CloseCDC(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
