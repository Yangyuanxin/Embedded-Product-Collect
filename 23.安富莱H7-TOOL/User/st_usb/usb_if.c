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

#include "usb_if.h"
#include "usbd_msc.h"
#include "usbd_storage.h"

USBD_HandleTypeDef USBD_Device = {0};
extern PCD_HandleTypeDef hpcd;
extern void SelectCDCUart(uint8_t _com);

extern USBD_DescriptorsTypeDef MSC_Desc;

/*
*********************************************************************************************************
*    函 数 名: usbd_Init
*    功能说明: 初始化USB协议栈
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void usbd_Init(void)
{
    //tusb_init();
}

/*
*********************************************************************************************************
*    函 数 名: usbd_UnInit
*    功能说明: 退出USB协议栈
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void usbd_UnInit(void)
{
    if (USBD_Device.dev_config > 0)
    {
        USBD_Stop(&USBD_Device);
        
        USBD_DeInit(&USBD_Device);
    }
}

/*
*********************************************************************************************************
*	函 数 名: usbd_OpenCDC
*	功能说明: 打开USB. 上电只运行一次
*	形    参: _com : 1, 4
*	返 回 值: 无
*********************************************************************************************************
*/
void usbd_OpenCDC(uint8_t _com)
{
    static uint8_t s_first_run = 0;
    
	SelectCDCUart(_com);		/* 选择uart1或uart4作为虚拟串口 */
	
    if (s_first_run == 0)
    {
        s_first_run = 1;
        
        /* Init Device Library */
        USBD_Init(&USBD_Device, &VCP_Desc, 0);

        /* Add Supported Class */
        USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);

        /* Add CDC Interface Class */
        USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops);

        /* Start Device Process */
        USBD_Start(&USBD_Device);
    }
}

/* 强制执行 */
void usbd_OpenCDC2(uint8_t _com)
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
    #if 0    
        if (USBD_Device.dev_config == 0)
        {
            return;		
        }
        
        USBD_Stop(&USBD_Device);
        
        USBD_DeInit(&USBD_Device);
    #endif    
}


/*
*********************************************************************************************************
*    函 数 名: usbd_OpenMassStorage
*    功能说明: 打开USB
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void bsp_DelayMS(uint32_t n);
void usbd_OpenMassStorage(void)
{
//    {
//        if (USBD_Device.dev_config != 0)
//        {
//            USBD_Stop(&USBD_Device);    
//            USBD_DeInit(&USBD_Device);
//            bsp_DelayMS(1000);            
//        }
//    }
        
    /* Init Device Library */
    USBD_Init(&USBD_Device, &MSC_Desc, 0);
    
    /* Add Supported Class */
    USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);

    /* Add Storage callbacks for MSC Class */
    USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);

    /* Start Device Process */
    USBD_Start(&USBD_Device);

    HAL_PWREx_EnableUSBVoltageDetector();    
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
//	USBD_Init(&USBD_Device, &MSC_Desc, 0);
//    /* Add Supported Class */
//    USBD_RegisterClass(&USBD_Device, USBD_MSC_CLASS);
		
    USBD_Stop(&USBD_Device);
    
    USBD_DeInit(&USBD_Device);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
