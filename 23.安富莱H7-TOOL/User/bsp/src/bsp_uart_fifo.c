/*
*********************************************************************************************************
*
*    模块名称 : 串口中断+FIFO驱动模块
*    文件名称 : bsp_uart_fifo.c
*    版    本 : V2.0
*    说    明 : 采用串口中断+FIFO模式实现多个串口的同时访问。
*    修改记录 :
*        版本号  日期       作者    说明
*        V1.0    2013-02-01 armfly  正式发布
*        V1.1    2013-06-09 armfly  FiFo结构增加TxCount成员变量，方便判断缓冲区满; 增加 清FiFo的函数
*        V1.2    2014-09-29 armfly  增加RS485 MODBUS接口。接收到新字节后，直接执行回调函数。
*        V1.3    2015-07-23 armfly  增加 UART_T 结构的读写指针几个成员变量必须增加 __IO 修饰,否则优化后
*                    会导致串口发送函数死机。
*        V1.4    2015-08-04 armfly  解决UART4配置bug  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART1);
*        V1.5    2015-10-08 armfly  增加修改波特率的接口函数
*        V1.6    2018-09-07 armfly  移植到STM32H7平台
*        V1.7    2018-10-01 armfly  增加 Sending 标志，表示正在发送中
*        V1.8    2018-11-26 armfly  增加UART8，第8个串口
*        V1.9    2019-11-22 armfly  解决波特率低于2400时实际波特率错乱的BUG。 支持110 - 6M 波特率范围
*        V2.0    2019-11-23 armfly  支持串口硬件参数可读取，可单独修改。修改了 UART_T结构体成员.
*
*    Copyright (C), 2015-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "main.h"
#include "nvic_prio_cfg.h"

#define UART_GPIO_SPEED     GPIO_SPEED_FREQ_HIGH

#if UART1_FIFO_EN == 1
    /* 串口1的GPIO  PA9, PA10   RS323 DB9接口 */
    #define USART1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()

    #define USART1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
    #define USART1_TX_GPIO_PORT             GPIOA
    #define USART1_TX_PIN                   GPIO_PIN_9
    #define USART1_TX_AF                    GPIO_AF7_USART1

    #define USART1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
    #define USART1_RX_GPIO_PORT             GPIOA   
    #define USART1_RX_PIN                   GPIO_PIN_10
    #define USART1_RX_AF                    GPIO_AF7_USART1
#endif

#if UART2_FIFO_EN == 1
    /* 串口2的GPIO --- PA2 PA3  GPS (只用RX。 TX被以太网占用） */
    #define USART2_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE()

    #define USART2_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
    #define USART2_TX_GPIO_PORT             GPIOA
    #define USART2_TX_PIN                   GPIO_PIN_2
    #define USART2_TX_AF                    GPIO_AF7_USART2

    #define USART2_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
    #define USART2_RX_GPIO_PORT             GPIOA
    #define USART2_RX_PIN                   GPIO_PIN_3
    #define USART2_RX_AF                    GPIO_AF7_USART2
#endif

#if UART3_FIFO_EN == 1
    /* 串口3的GPIO --- PB10 PB11  RS485 */
    #define USART3_CLK_ENABLE()             __HAL_RCC_USART3_CLK_ENABLE()

    #define USART3_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
    #define USART3_TX_GPIO_PORT             GPIOB
    #define USART3_TX_PIN                   GPIO_PIN_10
    #define USART3_TX_AF                    GPIO_AF7_USART3

    #define USART3_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
    #define USART3_RX_GPIO_PORT             GPIOB
    #define USART3_RX_PIN                   GPIO_PIN_11
    #define USART3_RX_AF                    GPIO_AF7_USART3
#endif

#if UART4_FIFO_EN == 1
    /* 串口4的GPIO --- PH13 PH14 */
    #define UART4_CLK_ENABLE()              __HAL_RCC_UART4_CLK_ENABLE()

    #define UART4_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
    #define UART4_TX_GPIO_PORT              GPIOH
    #define UART4_TX_PIN                    GPIO_PIN_13
    #define UART4_TX_AF                     GPIO_AF8_UART4

    #define UART4_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
    #define UART4_RX_GPIO_PORT              GPIOH
    #define UART4_RX_PIN                    GPIO_PIN_14
    #define UART4_RX_AF                     GPIO_AF8_UART4
#endif

#if UART5_FIFO_EN == 1
    /* 串口5的GPIO --- PC12/UART5_TX PD2/UART5_RX (被SD卡占用） */
    #define UART5_CLK_ENABLE()              __HAL_RCC_UART5_CLK_ENABLE()

    #define UART5_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
    #define UART5_TX_GPIO_PORT              GPIOC
    #define UART5_TX_PIN                    GPIO_PIN_12
    #define UART5_TX_AF                     GPIO_AF8_UART5

    #define UART5_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOD_CLK_ENABLE()
    #define UART5_RX_GPIO_PORT              GPIOD
    #define UART5_RX_PIN                    GPIO_PIN_2
    #define UART5_RX_AF                     GPIO_AF8_UART5
#endif

#if UART6_FIFO_EN == 1
    /* 串口6的GPIO --- PG14 PC7  GPRS */
    #define USART6_CLK_ENABLE()             __HAL_RCC_USART6_CLK_ENABLE()

    #define USART6_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOG_CLK_ENABLE()
    #define USART6_TX_GPIO_PORT             GPIOG
    #define USART6_TX_PIN                   GPIO_PIN_14
    #define USART6_TX_AF                    GPIO_AF7_USART6

    #define USART6_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
    #define USART6_RX_GPIO_PORT             GPIOC
    #define USART6_RX_PIN                   GPIO_PIN_7
    #define USART6_RX_AF                    GPIO_AF7_USART6
#endif

#if UART7_FIFO_EN == 1
    /* 串口7的GPIO --- PA15  PA8 */
    #define UART7_CLK_ENABLE()              __HAL_RCC_UART7_CLK_ENABLE()

    #define UART7_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
    #define UART7_TX_GPIO_PORT              GPIOA
    #define UART7_TX_PIN                    GPIO_PIN_15
    #define UART7_TX_AF                     GPIO_AF11_UART7

    #define UART7_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
    #define UART7_RX_GPIO_PORT              GPIOA
    #define UART7_RX_PIN                    GPIO_PIN_8
    #define UART7_RX_AF                     GPIO_AF11_UART7
#endif

#if UART8_FIFO_EN == 1
    /* 串口8 只接收，不发送  PE0/UART8_RX */
    #define UART8_CLK_ENABLE()              __HAL_RCC_UART8_CLK_ENABLE()

//    #define UART8_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOJ_CLK_ENABLE()
//    #define UART8_TX_GPIO_PORT              GPIOJ
//    #define UART8_TX_PIN                    GPIO_PIN_8
//    #define UART8_TX_AF                     GPIO_AF8_UART8

    #define UART8_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOE_CLK_ENABLE()
    #define UART8_RX_GPIO_PORT              GPIOE
    #define UART8_RX_PIN                    GPIO_PIN_0
    #define UART8_RX_AF                     GPIO_AF8_UART8
#endif

/* 定义每个串口结构体变量 */
#if UART1_FIFO_EN == 1
static UART_T g_tUart1;
static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if UART2_FIFO_EN == 1
static UART_T g_tUart2;
static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if UART3_FIFO_EN == 1
static UART_T g_tUart3;
static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if UART4_FIFO_EN == 1
static UART_T g_tUart4;
static uint8_t g_TxBuf4[UART4_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf4[UART4_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if UART5_FIFO_EN == 1
static UART_T g_tUart5;
static uint8_t g_TxBuf5[UART5_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf5[UART5_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if UART6_FIFO_EN == 1
static UART_T g_tUart6;
static uint8_t g_TxBuf6[UART6_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf6[UART6_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if UART7_FIFO_EN == 1
static UART_T g_tUart7;
static uint8_t g_TxBuf7[UART7_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf7[UART7_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if UART8_FIFO_EN == 1
static UART_T g_tUart8;
static uint8_t g_TxBuf8[UART8_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBuf8[UART8_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if USB1_FIFO_EN == 1
static UART_T g_tUartUSB1;
static uint8_t g_TxBufUSB1[USB1_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBufUSB1[USB1_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

#if USB2_FIFO_EN == 1
static UART_T g_tUartUSB2;
static uint8_t g_TxBufUSB2[USB2_TX_BUF_SIZE]; /* 发送缓冲区 */
static uint8_t g_RxBufUSB2[USB2_RX_BUF_SIZE]; /* 接收缓冲区 */
#endif

static void UartVarInit(void);

static void InitHardUart(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
//static void UartIRQ(UART_T *_pUart);

void RS485_InitTXE(void);

/*
*********************************************************************************************************
*    函 数 名: bsp_InitUart
*    功能说明: 初始化串口硬件，并对全局变量赋初值.
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUart(void)
{

    UartVarInit(); /* 必须先初始化全局变量,再配置硬件 */

    InitHardUart(); /* 配置串口的硬件参数(波特率等) */

    RS485_InitTXE(); /* 配置RS485芯片的发送使能硬件，配置为推挽输出 */
}


void bsp_DeInitUart(void)
{
    HAL_UART_DeInit(&(g_tUart1.huart));
    HAL_UART_DeInit(&(g_tUart4.huart));   
    HAL_UART_DeInit(&(g_tUart7.huart));   
}

/*
*********************************************************************************************************
*    函 数 名: ComToUart
*    功能说明: 将COM端口号转换为UART指针
*    形    参: _ucPort: 端口号(COM1 - COM6)
*    返 回 值: uart指针
*********************************************************************************************************
*/
UART_T *ComToUart(COM_PORT_E _ucPort)
{
    if (_ucPort == COM1)
    {
#if UART1_FIFO_EN == 1
        return &g_tUart1;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM2)
    {
#if UART2_FIFO_EN == 1
        return &g_tUart2;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM3)
    {
#if UART3_FIFO_EN == 1
        return &g_tUart3;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM4)
    {
#if UART4_FIFO_EN == 1
        return &g_tUart4;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM5)
    {
#if UART5_FIFO_EN == 1
        return &g_tUart5;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM6)
    {
#if UART6_FIFO_EN == 1
        return &g_tUart6;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM7)
    {
#if UART7_FIFO_EN == 1
        return &g_tUart7;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM8)
    {
#if UART8_FIFO_EN == 1
        return &g_tUart8;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM_USB1)
    {
#if USB1_FIFO_EN == 1
        return &g_tUartUSB1;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM_USB2)
    {
#if USB2_FIFO_EN == 1
        return &g_tUartUSB2;
#else
        return 0;
#endif
    }    
    else
    {
        Error_Handler(__FILE__, __LINE__);
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: ComToUart
*    功能说明: 将COM端口号转换为 USART_TypeDef* USARTx
*    形    参: _ucPort: 端口号(COM1 - COM6)
*    返 回 值: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5
*********************************************************************************************************
*/
USART_TypeDef *ComToUSARTx(COM_PORT_E _ucPort)
{
    if (_ucPort == COM1)
    {
#if UART1_FIFO_EN == 1
        return USART1;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM2)
    {
#if UART2_FIFO_EN == 1
        return USART2;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM3)
    {
#if UART3_FIFO_EN == 1
        return USART3;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM4)
    {
#if UART4_FIFO_EN == 1
        return UART4;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM5)
    {
#if UART5_FIFO_EN == 1
        return UART5;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM6)
    {
#if UART6_FIFO_EN == 1
        return USART6;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM7)
    {
#if UART7_FIFO_EN == 1
        return UART7;
#else
        return 0;
#endif
    }
    else if (_ucPort == COM8)
    {
#if UART8_FIFO_EN == 1
        return UART8;
#else
        return 0;
#endif
    }

    else
    {
        /* 不做任何处理 */
        return 0;
    }
}

/*
*********************************************************************************************************
*    函 数 名: comSendBuf
*    功能说明: 向串口发送一组数据。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*    形    参: _ucPort: 端口号(COM1 - COM6)
*              _ucaBuf: 待发送的数据缓冲区
*              _usLen : 数据长度
*    返 回 值: 无
*********************************************************************************************************
*/
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }

    if (pUart->SendBefor != 0)
    {
        pUart->SendBefor(); /* 如果是RS485通信，可以在这个函数中将RS485设置为发送模式 */
    }

    UartSend(pUart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*    函 数 名: comSendChar
*    功能说明: 向串口发送1个字节。数据放到发送缓冲区后立即返回，由中断服务程序在后台完成发送
*    形    参: _ucPort: 端口号(COM1 - COM6)
*              _ucByte: 待发送的数据
*    返 回 值: 无
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
    comSendBuf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*    函 数 名: comGetChar
*    功能说明: 从接收缓冲区读取1字节，非阻塞。无论有无数据均立即返回。
*    形    参: _ucPort: 端口号(COM1 - COM5)
*              _pByte: 接收到的数据存放在这个地址
*    返 回 值: 0 表示无数据, 1 表示读取到有效字节
*********************************************************************************************************
*/
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return 0;
    }

    return UartGetChar(pUart, _pByte);
}

/*
*********************************************************************************************************
*    函 数 名: comClearTxFifo
*    功能说明: 清零串口发送缓冲区
*    形    参: _ucPort: 端口号(COM1 - COM6)
*    返 回 值: 无
*********************************************************************************************************
*/
void comClearTxFifo(COM_PORT_E _ucPort)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }

    pUart->usTxWrite = 0;
    pUart->usTxRead = 0;
    pUart->usTxCount = 0;
}

/*
*********************************************************************************************************
*    函 数 名: comClearRxFifo
*    功能说明: 清零串口接收缓冲区
*    形    参: _ucPort: 端口号(COM1 - COM6)
*    返 回 值: 无
*********************************************************************************************************
*/
void comClearRxFifo(COM_PORT_E _ucPort)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }

    pUart->usRxWrite = 0;
    pUart->usRxRead = 0;
    pUart->usRxCount = 0;
}

/*
*********************************************************************************************************
*    函 数 名: comGetBaud
*    功能说明: 读取串口的波特率
*    形    参: _ucPort: 端口号(COM1 - COM6)
*    返 回 值: 波特率
*********************************************************************************************************
*/
uint32_t comGetBaud(COM_PORT_E _ucPort)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return 0;
    }
    
    return pUart->huart.Init.BaudRate;
}

/*
*********************************************************************************************************
*    函 数 名: comSetBaud
*    功能说明: 设置串口的波特率. 本函数固定设置为无校验，收发都使能模式
*    形    参: _ucPort: 端口号(COM1 - COM5)
*              _BaudRate: 波特率，0-4500000， 最大4.5Mbps
*    返 回 值: 无
*********************************************************************************************************
*/
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }
    
    pUart->huart.Init.BaudRate = _BaudRate;
    
    if (pUart->huart.Init.BaudRate < 300)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV64;
    }    
    else if (pUart->huart.Init.BaudRate < 1200)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV8;
    }
    else if (pUart->huart.Init.BaudRate < 2400)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV2;
    }
    else
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    }
        
    HAL_UART_Init(&pUart->huart);
}

/* 如果是RS485通信，请按如下格式编写函数， 我们仅举了 USART3作为RS485的例子 */

/*
*********************************************************************************************************
*    函 数 名: RS485_InitTXE
*    功能说明: 配置RS485发送使能口线 TXE
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void RS485_InitTXE(void)
{
    GPIO_InitTypeDef gpio_init;

    /* 打开GPIO时钟 */
    RS485_TXEN_GPIO_CLK_ENABLE();

    /* 配置引脚为推挽输出 */
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;           /* 推挽输出 */
    gpio_init.Pull = GPIO_NOPULL;                   /* 上下拉电阻不使能 */
    gpio_init.Speed = UART_GPIO_SPEED;    /* GPIO速度等级 */
    gpio_init.Pin = RS485_TXEN_PIN;
    HAL_GPIO_Init(RS485_TXEN_GPIO_PORT, &gpio_init);
}

/*
*********************************************************************************************************
*    函 数 名: RS485_SetBaud
*    功能说明: 修改485串口的波特率。
*    形    参: _baud : 波特率.0-4500000
*    返 回 值: 无
*********************************************************************************************************
*/
void RS485_SetBaud(uint32_t _baud)
{
    comSetBaud(COM_RS485, _baud);
}

/*
*********************************************************************************************************
*    函 数 名: RS485_SendBefor
*    功能说明: 发送数据前的准备工作。对于RS485通信，请设置RS485芯片为发送状态，
*              并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendBefor = RS485_SendBefor
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
    RS485_TX_EN(); /* 切换RS485收发芯片为发送模式 */
}

/*
*********************************************************************************************************
*    函 数 名: RS485_SendOver
*    功能说明: 发送一串数据结束后的善后处理。对于RS485通信，请设置RS485芯片为接收状态，
*              并修改 UartVarInit()中的函数指针等于本函数名，比如 g_tUart2.SendOver = RS485_SendOver
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
    RS485_RX_EN(); /* 切换RS485收发芯片为接收模式 */
}

/*
*********************************************************************************************************
*    函 数 名: RS485_SendBuf
*    功能说明: 通过RS485芯片发送一串数据。注意，本函数不等待发送完毕。
*    形    参: _ucaBuf : 数据缓冲区
*              _usLen : 数据长度
*    返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
    comSendBuf(COM3, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*    函 数 名: RS485_SendStr
*    功能说明: 向485总线发送一个字符串，0结束。
*    形    参: _pBuf 字符串，0结束
*    返 回 值: 无
*********************************************************************************************************
*/
void RS485_SendStr(char *_pBuf)
{
    RS485_SendBuf((uint8_t *)_pBuf, strlen(_pBuf));
}

/*
*********************************************************************************************************
*    函 数 名: RS485_ReciveNew
*    功能说明: 接收到新的数据
*    形    参: _byte 接收到的新数据
*    返 回 值: 无
*********************************************************************************************************
*/
//extern void MODH_ReciveNew(uint8_t _byte);
void RS485_ReciveNew(uint8_t _byte)
{
    //    MODH_ReciveNew(_byte);
}


/*
*********************************************************************************************************
*    函 数 名: comSetCallbackReciveNew
*    功能说明: 设置回调函数,接收到新的字节后执行
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void comSetCallbackReciveNew(COM_PORT_E _ucPort, void (*Func)(uint8_t _byte))
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }

    pUart->ReciveNew = Func;
}

/*
*********************************************************************************************************
*    函 数 名: comSetCallbackSendBefor
*    功能说明: 设置发送前回调函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void comSetCallbackSendBefor(COM_PORT_E _ucPort, void (*Func)(void))
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }

    pUart->SendBefor = Func;
}

/*
*********************************************************************************************************
*    函 数 名: comSetCallbackSendOver
*    功能说明: 设置发送完毕回调函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void comSetCallbackSendOver(COM_PORT_E _ucPort, void (*Func)(void))
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }

    pUart->SendOver = Func;
}

/*
*********************************************************************************************************
*    函 数 名: UartVarInit
*    功能说明: 初始化串口相关的变量
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void UartVarInit(void)
{
#if UART1_FIFO_EN == 1
    memset(&g_tUart1, 0, sizeof(UART_T));
    g_tUart1.huart.Instance = USART1;
    g_tUart1.huart.Instance = USART1;                       /* STM32 串口设备 */
    g_tUart1.pTxBuf = g_TxBuf1;                             /* 发送缓冲区指针 */
    g_tUart1.pRxBuf = g_RxBuf1;                             /* 接收缓冲区指针 */
    g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;               /* 发送缓冲区大小 */
    g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;               /* 接收缓冲区大小 */
    g_tUart1.usTxWrite = 0;                                 /* 发送FIFO写索引 */
    g_tUart1.usTxRead = 0;                                  /* 发送FIFO读索引 */
    g_tUart1.usRxWrite = 0;                                 /* 接收FIFO写索引 */
    g_tUart1.usRxRead = 0;                                  /* 接收FIFO读索引 */
    g_tUart1.usRxCount = 0;                                 /* 接收到的新数据个数 */
    g_tUart1.usTxCount = 0;                                 /* 待发送的数据个数 */
    g_tUart1.SendBefor = RS485_SendBefor;                   /* 发送数据前的回调函数 */
    g_tUart1.SendOver = RS485_SendOver;                     /* 发送完毕后的回调函数 */
    g_tUart1.ReciveNew = 0;                                 /* 接收到新数据后的回调函数 */
    g_tUart1.Sending = 0;                                   /* 正在发送中标志 */
#endif

#if UART2_FIFO_EN == 1
    memset(&g_tUart2, 0, sizeof(UART_T));
    g_tUart2.huart.Instance = USART2;                       /* STM32 串口设备 */
    g_tUart2.pTxBuf = g_TxBuf2;                             /* 发送缓冲区指针 */
    g_tUart2.pRxBuf = g_RxBuf2;                             /* 接收缓冲区指针 */
    g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;               /* 发送缓冲区大小 */
    g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;               /* 接收缓冲区大小 */
    g_tUart2.usTxWrite = 0;                                 /* 发送FIFO写索引 */
    g_tUart2.usTxRead = 0;                                  /* 发送FIFO读索引 */
    g_tUart2.usRxWrite = 0;                                 /* 接收FIFO写索引 */
    g_tUart2.usRxRead = 0;                                  /* 接收FIFO读索引 */
    g_tUart2.usRxCount = 0;                                 /* 接收到的新数据个数 */
    g_tUart2.usTxCount = 0;                                 /* 待发送的数据个数 */
    g_tUart2.SendBefor = 0;                                 /* 发送数据前的回调函数 */
    g_tUart2.SendOver = 0;                                  /* 发送完毕后的回调函数 */
    g_tUart2.ReciveNew = 0;                                 /* 接收到新数据后的回调函数 */
    g_tUart2.Sending = 0;                                   /* 正在发送中标志 */
#endif

#if UART3_FIFO_EN == 1
    memset(&g_tUart3, 0, sizeof(UART_T));
    g_tUart3.huart.Instance = USART3;                       /* STM32 串口设备 */
    g_tUart3.pTxBuf = g_TxBuf3;                             /* 发送缓冲区指针 */
    g_tUart3.pRxBuf = g_RxBuf3;                             /* 接收缓冲区指针 */
    g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;               /* 发送缓冲区大小 */
    g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;               /* 接收缓冲区大小 */
    g_tUart3.usTxWrite = 0;                                 /* 发送FIFO写索引 */
    g_tUart3.usTxRead = 0;                                  /* 发送FIFO读索引 */
    g_tUart3.usRxWrite = 0;                                 /* 接收FIFO写索引 */
    g_tUart3.usRxRead = 0;                                  /* 接收FIFO读索引 */
    g_tUart3.usRxCount = 0;                                 /* 接收到的新数据个数 */
    g_tUart3.usTxCount = 0;                                 /* 待发送的数据个数 */
    g_tUart3.SendBefor = RS485_SendBefor;                   /* 发送数据前的回调函数 */
    g_tUart3.SendOver = RS485_SendOver;                     /* 发送完毕后的回调函数 */
    g_tUart3.ReciveNew = RS485_ReciveNew;                   /* 接收到新数据后的回调函数 */
    g_tUart3.Sending = 0;                                   /* 正在发送中标志 */
#endif

#if UART4_FIFO_EN == 1
    memset(&g_tUart4, 0, sizeof(UART_T));
    g_tUart4.huart.Instance = UART4;                        /* STM32 串口设备 */
    g_tUart4.pTxBuf = g_TxBuf4;                             /* 发送缓冲区指针 */
    g_tUart4.pRxBuf = g_RxBuf4;                             /* 接收缓冲区指针 */
    g_tUart4.usTxBufSize = UART4_TX_BUF_SIZE;               /* 发送缓冲区大小 */
    g_tUart4.usRxBufSize = UART4_RX_BUF_SIZE;               /* 接收缓冲区大小 */
    g_tUart4.usTxWrite = 0;                                 /* 发送FIFO写索引 */
    g_tUart4.usTxRead = 0;                                  /* 发送FIFO读索引 */
    g_tUart4.usRxWrite = 0;                                 /* 接收FIFO写索引 */
    g_tUart4.usRxRead = 0;                                  /* 接收FIFO读索引 */
    g_tUart4.usRxCount = 0;                                 /* 接收到的新数据个数 */
    g_tUart4.usTxCount = 0;                                 /* 待发送的数据个数 */
    g_tUart4.SendBefor = 0;                                 /* 发送数据前的回调函数 */
    g_tUart4.SendOver = 0;                                  /* 发送完毕后的回调函数 */
    g_tUart4.ReciveNew = 0;                                 /* 接收到新数据后的回调函数 */
    g_tUart4.Sending = 0;                                   /* 正在发送中标志 */
#endif

#if UART5_FIFO_EN == 1
    memset(&g_tUart5, 0, sizeof(UART_T));
    g_tUart5.huart.Instance = UART5;                        /* STM32 串口设备 */
    g_tUart5.pTxBuf = g_TxBuf5;                             /* 发送缓冲区指针 */
    g_tUart5.pRxBuf = g_RxBuf5;                             /* 接收缓冲区指针 */
    g_tUart5.usTxBufSize = UART5_TX_BUF_SIZE;               /* 发送缓冲区大小 */
    g_tUart5.usRxBufSize = UART5_RX_BUF_SIZE;               /* 接收缓冲区大小 */
    g_tUart5.usTxWrite = 0;                                 /* 发送FIFO写索引 */
    g_tUart5.usTxRead = 0;                                  /* 发送FIFO读索引 */
    g_tUart5.usRxWrite = 0;                                 /* 接收FIFO写索引 */
    g_tUart5.usRxRead = 0;                                  /* 接收FIFO读索引 */
    g_tUart5.usRxCount = 0;                                 /* 接收到的新数据个数 */
    g_tUart5.usTxCount = 0;                                 /* 待发送的数据个数 */
    g_tUart5.SendBefor = 0;                                 /* 发送数据前的回调函数 */
    g_tUart5.SendOver = 0;                                  /* 发送完毕后的回调函数 */
    g_tUart5.ReciveNew = 0;                                 /* 接收到新数据后的回调函数 */
    g_tUart5.Sending = 0;                                   /* 正在发送中标志 */
#endif

#if UART6_FIFO_EN == 1
    memset(&g_tUart6, 0, sizeof(UART_T));
    g_tUart6.huart.Instance = USART6;                       /* STM32 串口设备 */
    g_tUart6.pTxBuf = g_TxBuf6;                             /* 发送缓冲区指针 */
    g_tUart6.pRxBuf = g_RxBuf6;                             /* 接收缓冲区指针 */
    g_tUart6.usTxBufSize = UART6_TX_BUF_SIZE;               /* 发送缓冲区大小 */
    g_tUart6.usRxBufSize = UART6_RX_BUF_SIZE;               /* 接收缓冲区大小 */
    g_tUart6.usTxWrite = 0;                                 /* 发送FIFO写索引 */
    g_tUart6.usTxRead = 0;                                  /* 发送FIFO读索引 */
    g_tUart6.usRxWrite = 0;                                 /* 接收FIFO写索引 */
    g_tUart6.usRxRead = 0;                                  /* 接收FIFO读索引 */
    g_tUart6.usRxCount = 0;                                 /* 接收到的新数据个数 */
    g_tUart6.usTxCount = 0;                                 /* 待发送的数据个数 */
    g_tUart6.SendBefor = 0;                                 /* 发送数据前的回调函数 */
    g_tUart6.SendOver = 0;                                  /* 发送完毕后的回调函数 */
    g_tUart6.ReciveNew = 0;                                 /* 接收到新数据后的回调函数 */
    g_tUart6.Sending = 0;                                   /* 正在发送中标志 */
#endif

#if UART7_FIFO_EN == 1
    memset(&g_tUart7, 0, sizeof(UART_T));
    g_tUart7.huart.Instance = UART7;                        /* STM32 串口设备 */
    g_tUart7.pTxBuf = g_TxBuf7;                             /* 发送缓冲区指针 */
    g_tUart7.pRxBuf = g_RxBuf7;                             /* 接收缓冲区指针 */
    g_tUart7.usTxBufSize = UART7_TX_BUF_SIZE;               /* 发送缓冲区大小 */
    g_tUart7.usRxBufSize = UART7_RX_BUF_SIZE;               /* 接收缓冲区大小 */
    g_tUart7.usTxWrite = 0;                                 /* 发送FIFO写索引 */
    g_tUart7.usTxRead = 0;                                  /* 发送FIFO读索引 */
    g_tUart7.usRxWrite = 0;                                 /* 接收FIFO写索引 */
    g_tUart7.usRxRead = 0;                                  /* 接收FIFO读索引 */
    g_tUart7.usRxCount = 0;                                 /* 接收到的新数据个数 */
    g_tUart7.usTxCount = 0;                                 /* 待发送的数据个数 */
    g_tUart7.SendBefor = 0;                                 /* 发送数据前的回调函数 */
    g_tUart7.SendOver = 0;                                  /* 发送完毕后的回调函数 */
    g_tUart7.ReciveNew = 0;                                 /* 接收到新数据后的回调函数 */
    g_tUart7.Sending = 0;                                   /* 正在发送中标志 */
#endif

#if UART8_FIFO_EN == 1
    memset(&g_tUart8, 0, sizeof(UART_T));
    g_tUart8.huart.Instance = UART8;                        /* STM32 串口设备 */
    g_tUart8.pTxBuf = g_TxBuf8;                             /* 发送缓冲区指针 */
    g_tUart8.pRxBuf = g_RxBuf8;                             /* 接收缓冲区指针 */
    g_tUart8.usTxBufSize = UART8_TX_BUF_SIZE;               /* 发送缓冲区大小 */
    g_tUart8.usRxBufSize = UART8_RX_BUF_SIZE;               /* 接收缓冲区大小 */
    g_tUart8.usTxWrite = 0;                                 /* 发送FIFO写索引 */
    g_tUart8.usTxRead = 0;                                  /* 发送FIFO读索引 */
    g_tUart8.usRxWrite = 0;                                 /* 接收FIFO写索引 */
    g_tUart8.usRxRead = 0;                                  /* 接收FIFO读索引 */
    g_tUart8.usRxCount = 0;                                 /* 接收到的新数据个数 */
    g_tUart8.usTxCount = 0;                                 /* 待发送的数据个数 */
    g_tUart8.SendBefor = 0;                                 /* 发送数据前的回调函数 */
    g_tUart8.SendOver = 0;                                  /* 发送完毕后的回调函数 */
    g_tUart8.ReciveNew = 0;                                 /* 接收到新数据后的回调函数 */
    g_tUart8.Sending = 0;                                   /* 正在发送中标志 */
#endif

#if USB1_FIFO_EN == 1
    memset(&g_tUartUSB1, 0, sizeof(UART_T));
    g_tUartUSB1.huart.Instance = 0;                         /* 虚拟USB */
    g_tUartUSB1.pTxBuf = g_TxBufUSB1;                       /* 发送缓冲区指针 */
    g_tUartUSB1.pRxBuf = g_RxBufUSB1;                       /* 接收缓冲区指针 */
    g_tUartUSB1.usTxBufSize = USB1_TX_BUF_SIZE;             /* 发送缓冲区大小 */
    g_tUartUSB1.usRxBufSize = USB1_RX_BUF_SIZE;             /* 接收缓冲区大小 */
    g_tUartUSB1.usTxWrite = 0;                              /* 发送FIFO写索引 */
    g_tUartUSB1.usTxRead = 0;                               /* 发送FIFO读索引 */
    g_tUartUSB1.usRxWrite = 0;                              /* 接收FIFO写索引 */
    g_tUartUSB1.usRxCount = 0;                              /* 接收到的新数据个数 */
    g_tUartUSB1.usTxCount = 0;                              /* 待发送的数据个数 */
    g_tUartUSB1.SendBefor = 0;                              /* 发送数据前的回调函数 */
    g_tUartUSB1.SendOver = 0;                               /* 发送完毕后的回调函数 */
    g_tUartUSB1.ReciveNew = 0;                              /* 接收到新数据后的回调函数 */
    g_tUartUSB1.Sending = 0;                                /* 正在发送中标志 */
#endif

#if USB2_FIFO_EN == 1
    memset(&g_tUartUSB2, 0, sizeof(UART_T));
    g_tUartUSB2.huart.Instance = 0;                         /* 虚拟USB */
    g_tUartUSB2.pTxBuf = g_TxBufUSB2;                       /* 发送缓冲区指针 */
    g_tUartUSB2.pRxBuf = g_RxBufUSB2;                       /* 接收缓冲区指针 */
    g_tUartUSB2.usTxBufSize = USB2_TX_BUF_SIZE;             /* 发送缓冲区大小 */
    g_tUartUSB2.usRxBufSize = USB2_RX_BUF_SIZE;             /* 接收缓冲区大小 */
    g_tUartUSB2.usTxWrite = 0;                              /* 发送FIFO写索引 */
    g_tUartUSB2.usTxRead = 0;                               /* 发送FIFO读索引 */
    g_tUartUSB2.usRxWrite = 0;                              /* 接收FIFO写索引 */
    g_tUartUSB2.usRxCount = 0;                              /* 接收到的新数据个数 */
    g_tUartUSB2.usTxCount = 0;                              /* 待发送的数据个数 */
    g_tUartUSB2.SendBefor = 0;                              /* 发送数据前的回调函数 */
    g_tUartUSB2.SendOver = 0;                               /* 发送完毕后的回调函数 */
    g_tUartUSB2.ReciveNew = 0;                              /* 接收到新数据后的回调函数 */
    g_tUartUSB2.Sending = 0;                                /* 正在发送中标志 */
#endif
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitUartParam
*    功能说明: 配置串口的硬件参数（波特率，数据位，停止位)
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitUartParam(COM_PORT_E _ucPort, uint32_t BaudRate, uint32_t Parity, uint32_t Mode)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }

    /* 配置串口硬件参数 */
    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) : 
                      BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
    pUart->huart.Init.BaudRate = BaudRate;
    pUart->huart.Init.WordLength = UART_WORDLENGTH_8B;
    pUart->huart.Init.StopBits = UART_STOPBITS_1;
    pUart->huart.Init.Parity = Parity;
    pUart->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    pUart->huart.Init.Mode = Mode;
    pUart->huart.Init.OverSampling = UART_OVERSAMPLING_16;
    pUart->huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;  
    if (BaudRate < 300)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV64;
    }    
    else if (BaudRate < 1200)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV8;
    }
    else if (BaudRate < 2400)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV2;
    }
    else
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    }
    pUart->huart.FifoMode = UART_FIFOMODE_DISABLE;
    pUart->huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
        
    if (HAL_UART_Init(&pUart->huart) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler(__FILE__, __LINE__);
    }
}

/*
*********************************************************************************************************
*    函 数 名: bsp_SetUartParam2
*    功能说明: 配置串口的硬件参数（波特率，数据位，停止位)
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetUartParam(COM_PORT_E _ucPort, uint32_t BaudRate, uint32_t Parity, uint32_t WordLength, uint32_t StopBits)
{
    UART_T *pUart;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }
    
    /* 第2步： 配置串口硬件参数 */
    /*##-1- Configure the UART peripheral ######################################*/
    /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
    /* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) : 
                      BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */

    pUart->huart.Init.BaudRate                    = BaudRate;
    pUart->huart.Init.WordLength                  = WordLength;
    pUart->huart.Init.StopBits                    = StopBits;
    pUart->huart.Init.Parity                      = Parity;
   
    if (BaudRate < 300)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV64;
    }    
    else if (BaudRate < 1200)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV8;
    }
    else if (BaudRate < 2400)
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV2;
    }
    else
    {
        pUart->huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    }
   
    if (HAL_UART_Init(&pUart->huart) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler(__FILE__, __LINE__);
    }
    
    comClearRxFifo(_ucPort);   /* 清除接收缓冲区 */
}

/*
*********************************************************************************************************
*    函 数 名: InitHardUart
*    功能说明: 配置串口的硬件参数（波特率，数据位，停止位，起始位，校验位，中断使能）适合于STM32-F4开发板
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void InitHardUart(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
    UART_HandleTypeDef UartHandle;

    /* Select SysClk as source of USART1 clocks */
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART16;
    RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    
#if UART1_FIFO_EN == 1 /* 串口1 */
    /* 使能 GPIO TX/RX 时钟 */
    USART1_TX_GPIO_CLK_ENABLE();
    USART1_RX_GPIO_CLK_ENABLE();

    /* 使能 USARTx 时钟 */
    USART1_CLK_ENABLE();

    /* 配置TX引脚 */
    GPIO_InitStruct.Pin = USART1_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = UART_GPIO_SPEED;
    GPIO_InitStruct.Alternate = USART1_TX_AF;
    HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置RX引脚 */
    GPIO_InitStruct.Pin = USART1_RX_PIN;
    GPIO_InitStruct.Alternate = USART1_RX_AF;
    HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置NVIC the NVIC for UART */
    HAL_NVIC_SetPriority(USART1_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    /* 配置波特率、奇偶校验 */
    bsp_InitUartParam(COM1, UART1_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

    /* 使能RX接受中断，接收超时中断  */
    SET_BIT(USART1->CR1, USART_CR1_RXNEIE | USART_CR1_RTOIE); 
#endif

#if UART2_FIFO_EN == 1 /* 串口2 */
    /* 使能 GPIO TX/RX 时钟 */
    USART2_TX_GPIO_CLK_ENABLE();
    USART2_RX_GPIO_CLK_ENABLE();

    /* 使能 USARTx 时钟 */
    USART2_CLK_ENABLE();

    /* 配置TX引脚 */
    GPIO_InitStruct.Pin = USART2_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = UART_GPIO_SPEED;
    GPIO_InitStruct.Alternate = USART2_TX_AF;
    HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置RX引脚 */
    GPIO_InitStruct.Pin = USART2_RX_PIN;
    GPIO_InitStruct.Alternate = USART2_RX_AF;
    HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置NVIC the NVIC for UART */
    HAL_NVIC_SetPriority(USART2_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    /* 配置波特率、奇偶校验 */
    bsp_InitUartParam(COM2, UART2_BAUD, UART_PARITY_NONE, UART_MODE_RX); // UART_MODE_TX_RX

    SET_BIT(USART2->CR1, USART_CR1_RXNEIE); /* 使能PE. RX接受中断 */
#endif

#if UART3_FIFO_EN == 1 /* 串口3 */
    /* 使能 GPIO TX/RX 时钟 */
    USART3_TX_GPIO_CLK_ENABLE();
    USART3_RX_GPIO_CLK_ENABLE();

    /* 使能 USARTx 时钟 */
    USART3_CLK_ENABLE();

    /* 配置TX引脚 */
    GPIO_InitStruct.Pin = USART3_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = UART_GPIO_SPEED;
    GPIO_InitStruct.Alternate = USART3_TX_AF;
    HAL_GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置RX引脚 */
    GPIO_InitStruct.Pin = USART3_RX_PIN;
    GPIO_InitStruct.Alternate = USART3_RX_AF;
    HAL_GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置NVIC the NVIC for UART */
    HAL_NVIC_SetPriority(USART3_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);

    /* 配置波特率、奇偶校验 */
    bsp_InitUartParam(COM3, UART3_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

    SET_BIT(USART3->CR1, USART_CR1_RXNEIE); /* 使能PE. RX接受中断 */
#endif

#if UART4_FIFO_EN == 1 /* 串口4    PH13/UART4_TX   PH14/UART4_RX/ */
    /* 使能 GPIO TX/RX 时钟 */
    UART4_TX_GPIO_CLK_ENABLE();
    UART4_RX_GPIO_CLK_ENABLE();

    /* 使能 USARTx 时钟 */
    UART4_CLK_ENABLE();

    /* 配置TX引脚 */
    GPIO_InitStruct.Pin = UART4_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = UART_GPIO_SPEED;
    GPIO_InitStruct.Alternate = UART4_TX_AF;
    HAL_GPIO_Init(UART4_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置RX引脚 */
    GPIO_InitStruct.Pin = UART4_RX_PIN;
    GPIO_InitStruct.Alternate = UART4_RX_AF;
    HAL_GPIO_Init(UART4_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置NVIC the NVIC for UART */
    HAL_NVIC_SetPriority(UART4_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);

    /* 配置波特率、奇偶校验 */
    bsp_InitUartParam(COM4, UART4_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

    SET_BIT(UART4->CR1, USART_CR1_RXNEIE); /* 使能RX接受中断 */
#endif

#if UART5_FIFO_EN == 1 /* 串口5 TX = PC12   RX = PD2 */
    /* 使能 GPIO TX/RX 时钟 */
    UART5_TX_GPIO_CLK_ENABLE();
    UART5_RX_GPIO_CLK_ENABLE();

    /* 使能 USARTx 时钟 */
    UART5_CLK_ENABLE();

    /* 配置TX引脚 */
    GPIO_InitStruct.Pin = UART5_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = UART_GPIO_SPEED;
    GPIO_InitStruct.Alternate = UART5_TX_AF;
    HAL_GPIO_Init(UART5_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置RX引脚 */
    GPIO_InitStruct.Pin = UART5_RX_PIN;
    GPIO_InitStruct.Alternate = UART5_RX_AF;
    HAL_GPIO_Init(UART5_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置NVIC the NVIC for UART */
    HAL_NVIC_SetPriority(UART5_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);

    /* 配置波特率、奇偶校验 */
    bsp_InitUartParam(COM5, UART5_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

    SET_BIT(UART5->CR1, USART_CR1_RXNEIE); /* 使能RX接受中断 */
#endif

#if UART6_FIFO_EN == 1 /* USART6 */
    /* 使能 GPIO TX/RX 时钟 */
    USART6_TX_GPIO_CLK_ENABLE();
    USART6_RX_GPIO_CLK_ENABLE();

    /* 使能 USARTx 时钟 */
    USART6_CLK_ENABLE();

    /* 配置TX引脚 */
    GPIO_InitStruct.Pin = USART6_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = UART_GPIO_SPEED;
    GPIO_InitStruct.Alternate = USART6_TX_AF;
    HAL_GPIO_Init(USART6_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置RX引脚 */
    GPIO_InitStruct.Pin = USART6_RX_PIN;
    GPIO_InitStruct.Alternate = USART6_RX_AF;
    HAL_GPIO_Init(USART6_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置NVIC the NVIC for UART */
    HAL_NVIC_SetPriority(USART6_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);

    /* 配置波特率、奇偶校验 */
    bsp_InitUartParam(COM6, UART6_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

    SET_BIT(USART6->CR1, USART_CR1_RXNEIE); /* 使能PE. RX接受中断 */
#endif

#if UART7_FIFO_EN == 1 /* UART7 */
    /* 使能 GPIO TX/RX 时钟 */
    UART7_TX_GPIO_CLK_ENABLE();
    UART7_RX_GPIO_CLK_ENABLE();

    /* 使能 USARTx 时钟 */
    UART7_CLK_ENABLE();

    /* 配置TX引脚 */
    GPIO_InitStruct.Pin = UART7_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = UART_GPIO_SPEED;
    GPIO_InitStruct.Alternate = UART7_TX_AF;
    HAL_GPIO_Init(UART7_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置RX引脚 */
    GPIO_InitStruct.Pin = UART7_RX_PIN;
    GPIO_InitStruct.Alternate = UART7_RX_AF;
    HAL_GPIO_Init(UART7_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置NVIC the NVIC for UART */
    HAL_NVIC_SetPriority(UART7_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(UART7_IRQn);

    /* 配置波特率、奇偶校验 */
    bsp_InitUartParam(COM7, UART7_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

    /* 使能RX接受中断，接收超时中断  */
    SET_BIT(UART7->CR1, USART_CR1_RXNEIE | USART_CR1_RTOIE);     
#endif

#if UART8_FIFO_EN == 1 /* UART8 */
    /* 使能 GPIO TX/RX 时钟 */
    #ifdef UART8_TX_PIN
    UART8_TX_GPIO_CLK_ENABLE();
    #endif
    UART8_RX_GPIO_CLK_ENABLE();

    /* 使能 USARTx 时钟 */
    UART8_CLK_ENABLE();

    /* 配置TX引脚 */

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = UART_GPIO_SPEED;
    
    #ifdef UART8_TX_PIN
    GPIO_InitStruct.Pin = UART8_TX_PIN;
    GPIO_InitStruct.Alternate = UART8_TX_AF;
    HAL_GPIO_Init(UART8_TX_GPIO_PORT, &GPIO_InitStruct);
    #endif
    
    /* 配置RX引脚 */
    GPIO_InitStruct.Pin = UART8_RX_PIN;
    GPIO_InitStruct.Alternate = UART8_RX_AF;
    HAL_GPIO_Init(UART8_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置NVIC the NVIC for UART */
    HAL_NVIC_SetPriority(UART8_IRQn, UART_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(UART8_IRQn);

    /* 配置波特率、奇偶校验 */
    bsp_InitUartParam(COM8, UART8_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

    SET_BIT(UART8->CR1, USART_CR1_RXNEIE); /* 使能PE. RX接受中断 */
#endif
}

/*
*********************************************************************************************************
*    函 数 名: UartSend
*    功能说明: 填写数据到UART发送缓冲区,并启动发送中断。中断处理函数发送完毕后，自动关闭发送中断
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
    uint16_t i;

    for (i = 0; i < _usLen; i++)
    {
        /* 如果发送缓冲区满，则等待发送W完毕 */
        while (1)
        {
            __IO uint16_t usCount;

            DISABLE_INT();
            usCount = _pUart->usTxCount;
            ENABLE_INT();

            if (usCount < _pUart->usTxBufSize)
            {
                break;
            }
            SET_BIT(_pUart->huart.Instance->CR1, USART_CR1_TXEIE); /* 使能发送中断（缓冲区空） */
        }

        /* 将新数据填入发送缓冲区 */
        _pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

        DISABLE_INT();
        if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
        {
            _pUart->usTxWrite = 0;
        }
        _pUart->usTxCount++;
        ENABLE_INT();
    }

    SET_BIT(_pUart->huart.Instance->CR1, USART_CR1_TXEIE); /* 使能发送中断（缓冲区空） */
}

/*
*********************************************************************************************************
*    函 数 名: UartGetChar
*    功能说明: 从串口接收缓冲区读取1字节数据 （用于主程序调用）
*    形    参: _pUart : 串口设备
*              _pByte : 存放读取数据的指针
*    返 回 值: 0 表示无数据  1表示读取到数据
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
    uint16_t usCount;

    /* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保护 */
    DISABLE_INT();
    usCount = _pUart->usRxCount;
    ENABLE_INT();

    /* 如果读和写索引相同，则返回0 */
    //if (_pUart->usRxRead == usRxWrite)
    if (usCount == 0) /* 已经没有数据 */
    {
        return 0;
    }
    else
    {
        *_pByte = _pUart->pRxBuf[_pUart->usRxRead]; /* 从串口接收FIFO取1个数据 */

        /* 改写FIFO读索引 */
        DISABLE_INT();
        if (++_pUart->usRxRead >= _pUart->usRxBufSize)
        {
            _pUart->usRxRead = 0;
        }
        _pUart->usRxCount--;
        ENABLE_INT();
        return 1;
    }
}

/*
*********************************************************************************************************
*   函 数 名: comTxEmpty
*   功能说明: 判断发送缓冲区是否为空。
*   形    参:  _pUart : 串口设备
*   返 回 值: 1为空。0为不空。
*********************************************************************************************************
*/
uint8_t comTxEmpty(COM_PORT_E _ucPort)
{
    UART_T *pUart;
    uint8_t Sending;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return 0;
    }

    Sending = pUart->Sending;

    if (Sending != 0)
    {
        return 0;
    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: comPutRxFifo
*    功能说明: 软件向接收FIFO填充数据
*    形    参: _ucPort : 串口设备
*    返 回 值: 无
*********************************************************************************************************
*/
void comPutRxFifo(COM_PORT_E _ucPort, uint8_t *_buf, uint16_t _len)
{
    UART_T *pUart;
    uint16_t i;

    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }

    for (i = 0; i < _len; i++)
    {
        pUart->pRxBuf[pUart->usRxWrite] = _buf[i];
        if (++pUart->usRxWrite >= pUart->usRxBufSize)
        {
            pUart->usRxWrite = 0;
        }
        if (pUart->usRxCount < pUart->usRxBufSize)
        {
            pUart->usRxCount++;
        }
    }
}

/*
*********************************************************************************************************
*   函 数 名: comSetCallBackReciveNew
*   功能说明: 配置接收回调函数
*   形    参:  _pUart : 串口设备
*              _ReciveNew : 函数指针，0表示取消
*   返 回 值: 无
*********************************************************************************************************
*/
void comSetCallBackReciveNew(COM_PORT_E _ucPort, void (*_ReciveNew)(uint8_t _byte))
{
    UART_T *pUart;
    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }
    
    pUart->ReciveNew = _ReciveNew;
}

/*
*********************************************************************************************************
*   函 数 名: comSetCallBackIdleLine
*   功能说明: 配置线路空闲中断回调函数
*   形    参:  _pUart : 串口设备
*             _IdleLine : 函数指针，0表示取消
*   返 回 值: 无
*********************************************************************************************************
*/
void comSetCallBackIdleLine(COM_PORT_E _ucPort, void (*_IdleLine)(void))
{
    UART_T *pUart;
    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }
    
    pUart->IdleLine = _IdleLine;
}

/*
*********************************************************************************************************
*   函 数 名: comSetReciverTimeout
*   功能说明: 配置接收器超时，并启用接收器中断
*   形    参:  _pUart : 串口设备
*             _Timeout : 超时间, 按bit单位
*   返 回 值: 无
*********************************************************************************************************
*/
void comSetReciverTimeout(COM_PORT_E _ucPort, uint32_t _Timeout)
{
    UART_T *pUart;
    pUart = ComToUart(_ucPort);
    if (pUart == 0)
    {
        return;
    }
    
    pUart->huart.Instance->RTOR = _Timeout;
    pUart->huart.Instance->CR2 |= USART_CR2_RTOEN;
}

/*
*********************************************************************************************************
*    函 数 名: UartIRQ
*    功能说明: 供中断服务程序调用，通用串口中断处理函数
*    形    参: _pUart : 串口设备
*    返 回 值: 无
*********************************************************************************************************
*/
void UartIRQ(UART_T *_pUart)
{
    uint32_t isrflags = READ_REG(_pUart->huart.Instance->ISR);
    uint32_t cr1its = READ_REG(_pUart->huart.Instance->CR1);
    uint32_t cr3its = READ_REG(_pUart->huart.Instance->CR3);

    /* 处理接收中断  */
    if ((isrflags & USART_ISR_RXNE_RXFNE) != 0 && (isrflags & USART_ISR_PE) == 0U)
    {
        /* 从串口接收数据寄存器读取数据存放到接收FIFO */
        uint8_t ch;

        ch = READ_REG(_pUart->huart.Instance->RDR);
        _pUart->pRxBuf[_pUart->usRxWrite] = ch;
        if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
        {
            _pUart->usRxWrite = 0;
        }
        if (_pUart->usRxCount < _pUart->usRxBufSize)
        {
            _pUart->usRxCount++;
        }

        /* 回调函数,通知应用程序收到新数据,一般是发送1个消息或者设置一个标记 */
        //if (_pUart->usRxWrite == _pUart->usRxRead)
        //if (_pUart->usRxCount == 1)
        {
            if (_pUart->ReciveNew)
            {
                _pUart->ReciveNew(ch);
            }
        }
    }
    
    /* 字节间超时中断 (MODBUS RTU 3.5字符超时） */
    if (isrflags & USART_ISR_RTOF)
    {
        SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_RTOF);
        if (_pUart->IdleLine)
        {
            _pUart->IdleLine();
        }
    }

    /* 处理发送缓冲区空中断 */
    if (((isrflags & USART_ISR_TXE_TXFNF) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
    {
        //if (_pUart->usTxRead == _pUart->usTxWrite)
        if (_pUart->usTxCount == 0)
        {
            /* 发送缓冲区的数据已取完时， 禁止发送缓冲区空中断 （注意：此时最后1个数据还未真正发送完毕）*/
            //USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);
            CLEAR_BIT(_pUart->huart.Instance->CR1, USART_CR1_TXEIE);

            /* 使能数据发送完毕中断 */
            //USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
            SET_BIT(_pUart->huart.Instance->CR1, USART_CR1_TCIE);
        }
        else
        {
            _pUart->Sending = 1;

            /* 从发送FIFO取1个字节写入串口发送数据寄存器 */
            //USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
            _pUart->huart.Instance->TDR = _pUart->pTxBuf[_pUart->usTxRead];
            if (++_pUart->usTxRead >= _pUart->usTxBufSize)
            {
                _pUart->usTxRead = 0;
            }
            _pUart->usTxCount--;
        }
    }
    /* 数据bit位全部发送完毕的中断 */
    if (((isrflags & USART_ISR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
    {
        //if (_pUart->usTxRead == _pUart->usTxWrite)
        if (_pUart->usTxCount == 0)
        {
            /* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中断 */
            //USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);
            CLEAR_BIT(_pUart->huart.Instance->CR1, USART_CR1_TCIE);

            /* 回调函数, 一般用来处理RS485通信，将RS485芯片设置为接收模式，避免抢占总线 */
            if (_pUart->SendOver)
            {
                _pUart->SendOver();
            }

            _pUart->Sending = 0;
        }
        else
        {
            /* 正常情况下，不会进入此分支 */

            /* 如果发送FIFO的数据还未完毕，则从发送FIFO取1个数据写入发送数据寄存器 */
            //USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
            _pUart->huart.Instance->TDR = _pUart->pTxBuf[_pUart->usTxRead];
            if (++_pUart->usTxRead >= _pUart->usTxBufSize)
            {
                _pUart->usTxRead = 0;
            }
            _pUart->usTxCount--;
        }
    }

    /* 清除中断标志 */
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_PEF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_FEF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_NEF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_OREF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_IDLEF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_TCF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_LBDF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_CTSF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_CMF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_WUF);
    SET_BIT(_pUart->huart.Instance->ICR, UART_CLEAR_TXFECF);

    //      *            @arg UART_CLEAR_PEF: Parity Error Clear Flag
    //  *            @arg UART_CLEAR_FEF: Framing Error Clear Flag
    //  *            @arg UART_CLEAR_NEF: Noise detected Clear Flag
    //  *            @arg UART_CLEAR_OREF: OverRun Error Clear Flag
    //  *            @arg UART_CLEAR_IDLEF: IDLE line detected Clear Flag
    //  *            @arg UART_CLEAR_TCF: Transmission Complete Clear Flag
    //  *            @arg UART_CLEAR_LBDF: LIN Break Detection Clear Flag
    //  *            @arg UART_CLEAR_CTSF: CTS Interrupt Clear Flag
    //  *            @arg UART_CLEAR_RTOF: Receiver Time Out Clear Flag
    //  *            @arg UART_CLEAR_CMF: Character Match Clear Flag
    //  *            @arg.UART_CLEAR_WUF:  Wake Up from stop mode Clear Flag
    //  *            @arg UART_CLEAR_TXFECF: TXFIFO empty Clear Flag
}

/*
*********************************************************************************************************
*    函 数 名: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler
*    功能说明: USART中断服务程序
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
    UartIRQ(&g_tUart1);
}
#endif

#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
    UartIRQ(&g_tUart2);
}
#endif

#if UART3_FIFO_EN == 1
void USART3_IRQHandler(void)
{
    UartIRQ(&g_tUart3);
}
#endif

#if UART4_FIFO_EN == 1
void UART4_IRQHandler(void)
{
    UartIRQ(&g_tUart4);
}
#endif

#if UART5_FIFO_EN == 1
void UART5_IRQHandler(void)
{
    UartIRQ(&g_tUart5);
}
#endif

#if UART6_FIFO_EN == 1
void USART6_IRQHandler(void)
{
    UartIRQ(&g_tUart6);
}
#endif

#if UART7_FIFO_EN == 1
void UART7_IRQHandler(void)
{
    UartIRQ(&g_tUart7);
}
#endif

#if UART8_FIFO_EN == 1
void UART8_IRQHandler(void)
{
    UartIRQ(&g_tUart8);
}
#endif

int stdout_putchar (int ch) 
{
   (void)ch;
    
    return ch;
}
    
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
