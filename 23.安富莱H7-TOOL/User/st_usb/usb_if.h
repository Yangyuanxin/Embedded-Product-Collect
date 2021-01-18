/*
*********************************************************************************************************
*
*    模块名称 : USB接口模块
*    文件名称 : usb_if.h
*
*********************************************************************************************************
*/

#ifndef _USB_IF_H_
#define _USB_IF_H_

#include "stm32h7xx_hal.h"
#include "usbd_core.h"

#include "usbd_desc.h"
#include "usbd_cdc.h" 
#include "usbd_cdc_interface.h"

void usbd_Init(void);
void usbd_UnInit(void);
void usbd_OpenCDC(uint8_t _com);
void usbd_OpenCDC2(uint8_t _com);
void usbd_CloseCDC(void);

void usbd_OpenMassStorage(void);
void usbd_CloseMassStorage(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
