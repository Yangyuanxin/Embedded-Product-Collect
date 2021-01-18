/*
*********************************************************************************************************
*
*	模块名称 : USB devie 虚拟串口驱动
*	文件名称 : usbd_user.c
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

#include "usbd_user.h"

USBD_HandleTypeDef USBD_Device = {0};
extern PCD_HandleTypeDef hpcd;

extern void SelectCDCUart(uint8_t _com);
/*
*********************************************************************************************************
*	函 数 名: usbd_OpenCDC
*	功能说明: 打开USB
*	形    参: _com : 1, 4
*	返 回 值: 无
*********************************************************************************************************
*/
void usbd_OpenCDC(uint8_t _com)
{
	SelectCDCUart(_com);		/* 选择uart1或uart4作为虚拟串口 */
	
	/* Init Device Library */
	USBD_Init(&USBD_Device, &VCP_Desc, 0);

	/* Add Supported Class */
	USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

	/* Add CDC Interface Class */
	USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

	/* Start Device Process */
	USBD_Start(&USBD_Device);
}

/*
*********************************************************************************************************
*	函 数 名: usbd_CloseCDC
*	功能说明: 关闭USB
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void usbd_CloseCDC(void)
{
	if (USBD_Device.dev_config == 0)
	{
		return;		
	}
	
	USBD_Stop(&USBD_Device);
	
	USBD_DeInit(&USBD_Device);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
