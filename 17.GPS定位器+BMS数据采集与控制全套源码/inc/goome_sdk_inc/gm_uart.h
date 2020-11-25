/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of goome Co., Ltd. 2018
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_uart.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *  Uart APIs defines.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
 

#ifndef __GM_UART_H__
#define __GM_UART_H__


typedef enum {
    UART_PORT1 = 0,  // general com
    UART_PORT2 = 1,  // gps com
    UART_PORT3 = 2,  // stm8 com
    UART_PORT_END = 3
}Enum_SerialPort;

typedef enum {
    DB_5BIT = 5,
    DB_6BIT,
    DB_7BIT,
    DB_8BIT
} Enum_DataBits;

typedef enum {
    SB_ONE = 1,
    SB_TWO,
    SB_ONE_DOT_FIVE
} Enum_StopBits;

typedef enum {
    PB_NONE = 0,
    PB_ODD,
    PB_EVEN,
    PB_SPACE,
    PB_MARK
} Enum_ParityBits;

typedef enum {
    GM_FC_NONE=1,  // None Flow Control
    GM_FC_HW,      // Hardware Flow Control 
    GM_SW       // Software Flow Control
} Enum_FlowCtrl;

typedef struct {
    u32                 baudrate; 
    Enum_DataBits       dataBits;
    Enum_StopBits       stopBits;
    Enum_ParityBits     parity;
    Enum_FlowCtrl       flowCtrl;
}ST_UARTDCB;




/*****************************************************************
* Function:     GM_UartOpen 
* 
* Description:
*               This function opens a specified UART port with the specified 
*               flow control mode. 
*               Which task call this function, which task will own the specified UART port.
*
* Parameters:
*               [in]port:       
*                       Port name
*               [in]baud:      
*                       The baud rate of the UART to be opened
*                       for the physical the baud rate support 75,150,300,600,1200,2400,4800,7200,
*                       9600,14400,19200,28800,38400,57600,115200,230400,460800
*                       for the UART_PORT1 support auto-baud rate, when the baud set to 0.
*
*                       This parameter don't take,effect on the VIRTUAL_PORT1,VIRTUAL_PORT2,just set to 0
*
*              
* Return:        
*               GM_RET_OK indicates success; otherwise failure.       
*
*****************************************************************/
extern s32 GM_UartOpen(Enum_SerialPort port,u32 baudrate);



/*****************************************************************
* Function:     GM_UartWrite 
* 
* Description:
*               This function is used to send data to the specified UART port. 
*
*               When the number of bytes actually write data is less than that to send,
*               Application should stop sending data, and application will receive an event 
*               EVENT_UART_READY_TO_WRITE later. 
*               After receiving this Event Application can continue to send data, 
*               and previously unsent data should be resend.
*
*               NOTE:
*               The RX/TX buffer size of VIRTUAL_PORT1 and VIRTUAL_PORT2 both are 1024-byte,
*               So the number of bytes to write should generally not more than 1024.
*  
* Parameters:
*               [in] port:
*                       Port name
*               [in]data:
*                       Pointer to data to be send
*               [in]writeLen:
*                       The length of the data to be written
*
* Return:        
*               If >= 0, indicates success, and the return value 
*                        is the length of actual write data length
*               If <  0, indicates failure to read      
*
*****************************************************************/
extern s32 GM_UartWrite(Enum_SerialPort port, u8* data, u32 writeLen );


/*****************************************************************
* Function:     GM_UartRead 
* 
* Description:
*               This function read data from the specified UART port.
*               When the UART callback is called, and the message type is 
*               EVENT_UART_READ_TO_READ,
*               user need read all of the data in the data buffer 
*               which means the real read len is less than the to be read len,
*               otherwise later the UART data is coming, there will be no such event to notify user get data.
*
* Parameters:
*               [in]port:
*                        Port name
*               [out]data:
*                       Point to the read data
*               [in]readLen:
*			    then length of data want to read
*
* Return:        
*               If >= 0, indicates success, and the return value 
*                        is the length of actual read data length
*               If <  0, indicates failure to read
*
*****************************************************************/
extern s32 GM_UartRead(Enum_SerialPort port, u8* data, u32 readLen);



/*****************************************************************
* Function:     GM_UartClrRxBuffer 
* 
* Description:
*               This function clears receive-buffer of specified UART port
*
* Parameters:
*               [in]port:
*                        Port name
*
* Return:        
*              NONE   
*
*****************************************************************/
extern void  GM_UartClrRxBuffer(Enum_SerialPort port);


/*****************************************************************
* Function:    GM_UartClrTxBuffer 
* 
* Description:
*               This function clears send-buffer of specified UART port.
*
* Parameters:
*               [in]port:
*                       Port name
*
* Return:        
*               NONE  
*
*****************************************************************/
extern void  GM_UartClrTxBuffer(Enum_SerialPort port);


/*****************************************************************
* Function:     GM_UartClose 
* 
* Description:
*               This function close a specified UART port.
* Parameters:
*               [in] port:
*                      Port name
* Return:        
*             NONE  
*
*****************************************************************/
extern void GM_UartClose(Enum_SerialPort port);

#endif  // End-of __GM_UART_H__
