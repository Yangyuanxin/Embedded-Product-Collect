/*
*********************************************************************************************************
*
*    模块名称 : 串口中断+FIFO驱动模块
*    文件名称 : bsp_uart_fifo.h
*    说    明 : 头文件
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

/* 定义下面这行，1表示printf 到UDP端口. 0表示输出到USB串口 */
#define PRINT_TO_UDP 1
#define LUA_UDP_PORT 777

/*
    H7-TOOL 串口分配：
    【串口1】 RS485， RS232，TTL串口，3合一
        PA9/USART1_TX
        P10/USART1_RX

    【串口2】 --- 未用
        PA2/USART2_TX
        PA3/USART2_RX

    【串口3】 --- 未用
        PB10/USART3_TX
        PB11/USART3_RX

    【串口4】 --- ESP-32模块用
        PH13/UART4_TX
        PH14/UART4_RX
        
    【串口5】 ---  未用

    【串口6】---  未用
        PC6/USART6_TX
        PC7/USART6_RX
        
    【串口7】 ---  引出，和FMC_D0 D1复用.
        PA15/UART7_TX       （FMC_DO)
        PA8/UART7_RX        （FMC_D1)
    
    【串口8】 ---  第2路RS232_RX
        PE0/UART8_RX
*/

#define UART1_FIFO_EN       1
#define UART2_FIFO_EN       0
#define UART3_FIFO_EN       0
#define UART4_FIFO_EN       1
#define UART5_FIFO_EN       0
#define UART6_FIFO_EN       0
#define UART7_FIFO_EN       1
#define UART8_FIFO_EN       0

#define USB1_FIFO_EN        1   /* 虚拟USB串口用 */
#define USB2_FIFO_EN        1   /* 虚拟USB串口用 */

/* PI10 控制RS485芯片的发送使能 */
#define RS485_TXEN_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOI_CLK_ENABLE()
#define RS485_TXEN_GPIO_PORT            GPIOI
#define RS485_TXEN_PIN                  GPIO_PIN_10

#define RS485_RX_EN()   BSP_SET_GPIO_0(RS485_TXEN_GPIO_PORT, RS485_TXEN_PIN)
#define RS485_TX_EN()   BSP_SET_GPIO_1(RS485_TXEN_GPIO_PORT, RS485_TXEN_PIN)

/* 定义端口号 */
typedef enum
{
    COM1 = 1, /* USART1 */
    COM2 = 2, /* USART2 */
    COM3 = 3, /* USART3 */
    COM4 = 4, /* UART4 */
    COM5 = 5, /* UART5 */
    COM6 = 6, /* USART6 */
    COM7 = 7, /* UART7 */
    COM8 = 8, /* UART8 */
    
    COM_USB1 = 9,
    COM_USB2 = 10,
} COM_PORT_E;

#define COM_RS485       COM1        /* 硬件串口UART1, RS485， 232， TTL-UART */
#define COM_ESP32       COM4        /* 硬件串口UART4 */
#define COM_UART7       COM7        /* 硬件串口UART7 - 和 D0 D1输出口复用 */
#define COM_RS232_2     COM8        /* 硬件串口UART8 - 第2路RS232接收 */

/* 下面是内存虚拟串口，用于USB CDC通信 */
#if USB1_FIFO_EN == 1
    #define COM_USB_PC      COM_USB1      /* 和PC软件通信的端口 */
#endif

#if USB2_FIFO_EN == 1
    #define COM_USB_LUA     COM_USB2      /* printf Debug, Lua输出串口 */
#endif

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
#define UART1_BAUD          115200
#define UART1_TX_BUF_SIZE   1 * 1024
#define UART1_RX_BUF_SIZE   1 * 1024
#endif

#if UART2_FIFO_EN == 1
#define UART2_BAUD          9600
#define UART2_TX_BUF_SIZE   10
#define UART2_RX_BUF_SIZE   2 * 1024
#endif

#if UART3_FIFO_EN == 1
#define UART3_BAUD          9600
#define UART3_TX_BUF_SIZE   1 * 1024
#define UART3_RX_BUF_SIZE   1 * 1024
#endif

#if UART4_FIFO_EN == 1
#define UART4_BAUD          115200
#define UART4_TX_BUF_SIZE   1 * 1024
#define UART4_RX_BUF_SIZE   1 * 1024
#endif

#if UART5_FIFO_EN == 1
#define UART5_BAUD          115200
#define UART5_TX_BUF_SIZE   1 * 1024
#define UART5_RX_BUF_SIZE   1 * 1024
#endif

#if UART6_FIFO_EN == 1
#define UART6_BAUD          115200
#define UART6_TX_BUF_SIZE   1 * 1024
#define UART6_RX_BUF_SIZE   1 * 1024
#endif

#if UART7_FIFO_EN == 1
#define UART7_BAUD          115200
#define UART7_TX_BUF_SIZE   1 * 1024
#define UART7_RX_BUF_SIZE   1 * 1024
#endif

#if UART8_FIFO_EN == 1
#define UART8_BAUD          115200
#define UART8_TX_BUF_SIZE   1 * 1024
#define UART8_RX_BUF_SIZE   1 * 1024
#endif

/* 下面是内存虚拟串口 */
#if USB1_FIFO_EN == 1
#define USB1_BAUD           115200
#define USB1_TX_BUF_SIZE    2 * 1024
#define USB1_RX_BUF_SIZE    2 * 1024
#endif

#if USB2_FIFO_EN == 1
#define USB2_BAUD           115200
#define USB2_TX_BUF_SIZE    2 * 1024
#define USB2_RX_BUF_SIZE    2 * 1024
#endif

/* 串口设备结构体 */
typedef struct
{
    UART_HandleTypeDef  huart;      /* 主要用其中的init结构，读回串口参数 */
//    USART_TypeDef *uart;            /* STM32内部串口设备指针 */
    uint8_t *pTxBuf;                /* 发送缓冲区 */
    uint8_t *pRxBuf;                /* 接收缓冲区 */
    uint16_t usTxBufSize;           /* 发送缓冲区大小 */
    uint16_t usRxBufSize;           /* 接收缓冲区大小 */
    __IO uint16_t usTxWrite;        /* 发送缓冲区写指针 */
    __IO uint16_t usTxRead;         /* 发送缓冲区读指针 */
    __IO uint16_t usTxCount;        /* 等待发送的数据个数 */

    __IO uint16_t usRxWrite;        /* 接收缓冲区写指针 */
    __IO uint16_t usRxRead;         /* 接收缓冲区读指针 */
    __IO uint16_t usRxCount;        /* 还未读取的新数据个数 */

    void (*SendBefor)(void);            /* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
    void (*SendOver)(void);             /* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
    void (*ReciveNew)(uint8_t _byte);   /* 串口收到数据的回调函数指针 */
    void (*IdleLine)(void);             /* 字符间超时（线路空闲）中断 */
    
    uint8_t Sending;                    /* 正在发送中 */
}UART_T;

void bsp_InitUart(void);
void bsp_DeInitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);

void USART_SetBaudRate(USART_TypeDef *USARTx, uint32_t BaudRate);
void bsp_InitUartParam(COM_PORT_E _ucPort, uint32_t BaudRate, uint32_t Parity, uint32_t Mode);

void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendStr(char *_pBuf);
void RS485_SetBaud(uint32_t _baud);
uint8_t comTxEmpty(COM_PORT_E _ucPort);

void comPutRxFifo(COM_PORT_E _ucPort, uint8_t *_buf, uint16_t _len);

void comSetCallbackReciveNew(COM_PORT_E _ucPort, void (*Func)(uint8_t _byte));
void comSetCallbackSendBefor(COM_PORT_E _ucPort, void (*Func)(void));
void comSetCallbackSendOver(COM_PORT_E _ucPort, void (*Func)(void));

void bsp_SetUartParam(COM_PORT_E _ucPort, uint32_t BaudRate, uint32_t Parity, uint32_t WordLength, uint32_t StopBits);

uint32_t comGetBaud(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate);
void comSetCallBackReciveNew(COM_PORT_E _ucPort, void (*_ReciveNew)(uint8_t _byte));
void comSetCallBackIdleLine(COM_PORT_E _ucPort, void (*_IdleLine)(void));
void comSetReciverTimeout(COM_PORT_E _ucPort, uint32_t _Timeout);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
