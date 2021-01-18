/*
*********************************************************************************************************
*
*    模块名称 : USB虚拟串口功能模块
*    文件名称 : status_usb_uart.h
*
*********************************************************************************************************
*/

#ifndef _STATUS_USB_UART_H_
#define _STATUS_USB_UART_H_

/* 这个结构在 usbd_cdc_interface.c中被引用 */
typedef struct
{
    uint32_t Baud;
    uint8_t DataBit;
    uint8_t StopBit;
    uint8_t Parity;
    uint8_t Connected;
    uint32_t PcTxCount;
    uint32_t DevTxCount;
    uint8_t Changed;
}USB_UART_T;    

extern USB_UART_T g_tUasUart;

void status_UsbUart(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
