/*
*********************************************************************************************************
*
*    模块名称 : USB devie 虚拟磁盘驱动
*    文件名称 : usbd_usr.c
*    版    本 : V1.0
*    说    明 : 封装虚拟U盘操作函数，提供给APP使用.
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2018-09-05 armfly  正式发布
*
*    Copyright (C), 2015-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"
#include "usbd_usr.h"

USBD_HandleTypeDef USBD_Device = {0};
PCD_HandleTypeDef hpcd;

/*
*********************************************************************************************************
*    函 数 名: usbd_OpenMassStorage
*    功能说明: 打开USB
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void usbd_OpenMassStorage(void)
{
    /* Init Device Library */
    USBD_Init(&USBD_Device, &MSC_Desc, 0);


    
    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);

    /* Add Storage callbacks for MSC Class */
    USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);

    /* Start Device Process */
    USBD_Start(&USBD_Device);

//    HAL_PWREx_EnableUSBVoltageDetector();    
}

/*
*********************************************************************************************************
*    函 数 名: usbd_CloseMassStorage
*    功能说明: 关闭USB
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void usbd_CloseMassStorage(void)
{
    USBD_Stop(&USBD_Device);
    
    USBD_DeInit(&USBD_Device);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
