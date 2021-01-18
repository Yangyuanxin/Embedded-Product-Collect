/*
*********************************************************************************************************
*
*    模块名称 : FMC总线扩展IO驱动程序
*    文件名称 : bsp_fmc_io.c
*    版    本 : V1.0
*    说    明 : H7开发板在FMC总线上扩展了32位输出IO。FMC地址为 (0x6820 0000)
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2015-10-11  armfly  正式发布
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"

#define H7_GPIO_SPEED       GPIO_SPEED_MEDIUM    // GPIO_SPEED_MEDIUM GPIO_SPEED_FREQ_HIGH


/* 配置GPIO为推挽输出 */
#define GPIO_INIT_OUT_PP(gpio, pin)            \
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        \
    gpio_init.Pull = GPIO_NOPULL;                \
    gpio_init.Speed = H7_GPIO_SPEED; \
    gpio_init.Pin = pin;                         \
    HAL_GPIO_Init(gpio, &gpio_init);

#define GPIO_INIT_OUT_OD(gpio, pin)            \
    gpio_init.Mode = GPIO_MODE_OUTPUT_OD;        \
    gpio_init.Pull = GPIO_NOPULL;                \
    gpio_init.Speed = H7_GPIO_SPEED; \
    gpio_init.Pin = pin;                         \
    HAL_GPIO_Init(gpio, &gpio_init);
    
#define GPIO_INIT_INPUT(gpio, pin)             \
    gpio_init.Mode = GPIO_MODE_INPUT;            \
    gpio_init.Pull = GPIO_NOPULL;                \
    gpio_init.Speed = H7_GPIO_SPEED; \
    gpio_init.Pin = pin;                         \
    HAL_GPIO_Init(gpio, &gpio_init);

#define GPIO_INIT_FMC(gpio, pin)               \
    gpio_init.Mode = GPIO_MODE_AF_PP;            \
    gpio_init.Pull = GPIO_NOPULL;                \
    gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH; \
    gpio_init.Alternate = GPIO_AF12_FMC;         \
    gpio_init.Pin = pin;                         \
    HAL_GPIO_Init(gpio, &gpio_init);

#define GPIO_INIT_UART7(gpio, pin)               \
    gpio_init.Mode = GPIO_MODE_AF_PP;            \
    gpio_init.Pull = GPIO_NOPULL;                \
    gpio_init.Speed = GPIO_SPEED_MEDIUM; \
    gpio_init.Alternate = GPIO_AF11_UART7;         \
    gpio_init.Pin = pin;                         \
    HAL_GPIO_Init(gpio, &gpio_init);   

#define GPIO_INIT_AF_PP(gpio, pin)            \
    gpio_init.Mode = GPIO_MODE_AF_PP;        \
    gpio_init.Pull = GPIO_NOPULL;                \
    gpio_init.Speed = H7_GPIO_SPEED; \
    gpio_init.Pin = pin;                         \
    HAL_GPIO_Init(gpio, &gpio_init);
    
#define GPIO_DIR_SET_OUT(gpio, pin)     BSP_SET_GPIO_1(gpio, pin)       /* DIR = 1 输出 */
#define GPIO_DIR_SET_IN(gpio, pin)      BSP_SET_GPIO_0(gpio, pin)       /* DIR = 0 输入 */

#define GPIO_SET_HIGH(gpio, pin)        BSP_SET_GPIO_1(gpio, pin)       /* pin = 1 */
#define GPIO_SET_LOW(gpio, pin)         BSP_SET_GPIO_0(gpio, pin)       /* pin = 0 */

#define GPIO_IN_IS_HIGH(gpio, pin)      (gpio->IDR & pin)
#define GPIO_OUT_IS_HIGH(gpio, pin)     (gpio->ODR & pin)

/*
    H7-TOOL 输出端口定义 （电平转换器， A(CPU)  B(外部）， DIR = 1时， A->B
             
    【D0】 - 方向 PE0/D0_DIR
    PD14/FMC_D0
    PI0/TIM5_CH4
    PA15/UART7_TX
    
    【D1】 - 方向 PG8/D1_DIR
    PD15/FMC_D1
    PH10/TIM5_CH1/ENCODE1
    PA8/UART7_RX
    
    【D2】 - 方向 PG15/D2_DIR
    PD0/FMC_D2
    PE6/SPI4_MOSI
    PB7/TIM4_CH2

    【D3】 - 方向 PH7/D3_DIR
    PD1/FMC_D3
    PE5/SPI4_MISO
    PH11/TIM5_CH2/ENCODE1

    【D4】 - 方向 PG12/D4_DIR
    PE7/FMC_D4
    PE4/SPI4_NSS
    PB8/TIM4_CH3
    
    【D5】 - 方向 PF4/D5_DIR  
    PE8/FMC_D5
    PE2/SPI4_SCK
    PI5/TIM8_CH1/ENCODE2

    【D6】 - 方向 PD10/D6_DIR
    PE9/FMC_D6
    PD3/SPI2_SCK
    PH6/SPI5_SCK
    PA0/TIM2_CH1

    【D7】 - 方向 PF5/D7_DIR -修改为-> PI1/D7_DIR
    PE10/FMC_D7
    PI6/TIM8_CH2/ENCODE2
    
    【D8】 - 方向 PG9/NOE_DIR
    PE11/FMC_D8
    PD4/FMC_NOE
    PI3/SPI2_MOSI
    PF0/I2C2_SDA
    
    【D9】 - 方向 PI2/NWE_DIR
    PE12/FMC_D9
    PD5/FMC_NWE
    PF1/I2C2_SCL
    
    ----------------------------------
    
    【D10  TTLTX】- 方向 PE3 = 1
    PA9/USART1_TX
    PE13/FMC_D10
             
    【D11  TTLRX】- 方向 PG0 = 0
    PA10/USART1_RX
    PE14/FMC_D11        

    【D12 CAN_TX】
    PA12/CAN1_TX
    PE15/FMC_D12
    
    【D13 CAN_RX】
    PA11/CAN1_RX
    PD8/FMC_D13

    
    ---------------------------------------------
    PD0/FMC_D2
    PD1/FMC_D3
    PD4/FMC_NOE        ---- 读控制信号，OE = Output Enable ， N 表示低有效
    PD5/FMC_NWE        -XX- 写控制信号，AD7606 只有读，无写信号
    PD8/FMC_D13
    PD9/FMC_D14
    PD10/FMC_D15
    PD14/FMC_D0
    PD15/FMC_D1

    PE7/FMC_D4
    PE8/FMC_D5
    PE9/FMC_D6
    PE10/FMC_D7
    PE11/FMC_D8
    PE12/FMC_D9
    PE13/FMC_D10
    PE14/FMC_D11
    PE15/FMC_D12
    

     +-------------------+------------------+
     +   32-bits Mode: D31-D16              +
     +-------------------+------------------+
     | PH8 <-> FMC_D16   | PI0 <-> FMC_D24  |
     | PH9 <-> FMC_D17   | PI1 <-> FMC_D25  |
     | PH10 <-> FMC_D18  | PI2 <-> FMC_D26  |
     | PH11 <-> FMC_D19  | PI3 <-> FMC_D27  |
     | PH12 <-> FMC_D20  | PI6 <-> FMC_D28  |
     | PH13 <-> FMC_D21  | PI7 <-> FMC_D29  |
     | PH14 <-> FMC_D22  | PI9 <-> FMC_D30  |
     | PH15 <-> FMC_D23  | PI10 <-> FMC_D31 |
     +------------------+-------------------+    
*/

static void EIO_ConfigGPIO(void);
static void EIO_ConfigFMC(void);

void EIO_485TXEN_Config(EIO_SELECT_E _mode);

/*
*********************************************************************************************************
*    函 数 名: bsp_InitExtIO
*    功能说明: 配置扩展IO相关的GPIO. 上电只能执行一次。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitExtIO(void)
{
    EIO_ConfigGPIO();
    EIO_ConfigFMC();

    EIO_ConfigPort(EIO_D0, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D1, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D2, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D3, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D4, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D5, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D6, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D7, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D8, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D9, ES_GPIO_IN);

    EIO_ConfigPort(EIO_D10, ES_GPIO_UART);
    EIO_ConfigPort(EIO_D11, ES_GPIO_UART);
    
    EIO_ConfigPort(EIO_D12, ES_GPIO_IN);
    EIO_ConfigPort(EIO_D13, ES_GPIO_IN);

    {
        uint8_t i;

        for (i = 0; i < 16; i++)
        {
            g_tVar.GpioMode[i] = ES_GPIO_IN;
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: EIO_ConfigPort
*    功能说明: 配置端口功能
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
void EIO_ConfigPort(uint8_t _eio, EIO_SELECT_E _mode)
{
    switch (_eio)
    {
        case EIO_D0:
            EIO_D0_Config(_mode);
            break;

        case EIO_D1:
            EIO_D1_Config(_mode);
            break;

        case EIO_D2:
            EIO_D2_Config(_mode);
            break;

        case EIO_D3:
            EIO_D3_Config(_mode);
            break;

        case EIO_D4:
            EIO_D4_Config(_mode);
            break;

        case EIO_D5:
            EIO_D5_Config(_mode);
            break;

        case EIO_D6:
            EIO_D6_Config(_mode);
            break;

        case EIO_D7:
            EIO_D7_Config(_mode);
            break;

        case EIO_D8: /* I2C_SDA */
            EIO_D8_Config(_mode);
            break;

        case EIO_D9: /* I2C_SCL */
            EIO_D9_Config(_mode);
            break;

        case EIO_D10: /* TTL_TX */
            EIO_D10_Config(_mode);
            break;

        case EIO_D11: /* TTL_RX */
            EIO_D11_Config(_mode);
            break;

        case EIO_D12: /* CAN_TX */
            EIO_D12_Config(_mode);
            break;

        case EIO_D13: /* CAN_RX */
            EIO_D13_Config(_mode);
            break;

        case EIO_485_TXEN:
            EIO_485TXEN_Config(_mode);
            break;
    }
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D0_Config
*    功能说明: 配置D0端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D0_DIR GPIOH
#define PIN_D0_DIR GPIO_PIN_8
void EIO_D0_Config(EIO_SELECT_E _mode)
{
    /*
        【D0】 - 方向 PE0/D0_DIR ---> PH8/DO_DIR
            PD14/FMC_D0
            PI0/TIM5_CH4     -- 做普通GPIO控制脚
            PA15/UART7_TX
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D0_DIR, PIN_D0_DIR);      /* 配置方向引脚 */
    GPIO_INIT_FMC(GPIOD, GPIO_PIN_14);              /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_15);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D0_DIR, PIN_D0_DIR);   /* 设置为输入方向  - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D0_DIR, PIN_D0_DIR);  /* 设置为输出方向  - 先执行 */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_15);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOI, GPIO_PIN_0);        /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D0_DIR, PIN_D0_DIR);  /* 设置为输出方向  - 先执行 */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_15);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_GPIO_UART)
    {
        GPIO_DIR_SET_OUT(GPIO_D0_DIR, PIN_D0_DIR);  /* 设置为输出方向  - 先执行 */
        
        GPIO_INIT_UART7(GPIOA, GPIO_PIN_15);        /* 配置GPIO为UART7功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_PROG_SPI_FLASH)
    {
        GPIO_DIR_SET_OUT(GPIO_D0_DIR, PIN_D0_DIR);  /* 设置为输出方向  - 先执行 */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_15);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOI, GPIO_PIN_0);        /* 配置为GPIO 输出功能 */        
    }
    else
    {
        g_tVar.GpioMode[0] = 0;
    }
    g_tVar.GpioMode[0] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D1_Config
*    功能说明: 配置D1端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D1_DIR GPIOG
#define PIN_D1_DIR GPIO_PIN_8
void EIO_D1_Config(EIO_SELECT_E _mode)
{
    /*
        【D1】 - 方向 PG8/D1_DIR
        PD15/FMC_D1
        PH10/TIM5_CH1/ENCODE1   -- 做普通GPIO控制脚
        PA8/UART7_RX
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D1_DIR, PIN_D1_DIR);      /* 配置方向引脚 */
    GPIO_INIT_FMC(GPIOD, GPIO_PIN_15);              /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOA, PIN_D1_DIR);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_10);        /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D1_DIR, PIN_D1_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D1_DIR, PIN_D1_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_8);         /* 配置为GPIO输入功能 */
        GPIO_INIT_OUT_PP(GPIOH, GPIO_PIN_10);       /* 配置为GPIO输出功能 */
    }
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D1_DIR, PIN_D1_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_8);         /* 配置为GPIO输入功能 */
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_10);        /* 配置为GPIO输入功能 */
    }
    else if (_mode == ES_GPIO_UART)
    {        
        GPIO_INIT_UART7(GPIOA, GPIO_PIN_8);         /* 配置GPIO为UART7功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D1_DIR, PIN_D1_DIR);   /* 设置为输入方向 - 后执行 */        
    }    
    else if (_mode == ES_PROG_SPI_FLASH)            /* PD15 输出 */
    {
        GPIO_DIR_SET_OUT(GPIO_D1_DIR, PIN_D1_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_8);         /* 配置为GPIO输入功能 */
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_10);        /* 配置为GPIO输入功能 */
        GPIO_INIT_OUT_PP(GPIOD, GPIO_PIN_15);        /* 配置为GPIO输出功能 */
    }    
    else
    {
        g_tVar.GpioMode[1] = 0;
    }
    g_tVar.GpioMode[1] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D2_Config
*    功能说明: 配置D2端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D2_DIR     GPIOD
#define PIN_D2_DIR      GPIO_PIN_9
void EIO_D2_Config(EIO_SELECT_E _mode)
{
    /*
        【D2】 - 方向 PG15/D2_DIR --> PD9/D2_DIR
        PD0/FMC_D2
        PE6/SPI4_MOSI   --- GPIO
        PB7/TIM4_CH2
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D2_DIR, PIN_D2_DIR);      /* 配置方向引脚 */
    GPIO_INIT_FMC(GPIOD, GPIO_PIN_0);               /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOB, GPIO_PIN_7);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_6);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D2_DIR, PIN_D2_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D2_DIR, PIN_D2_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOB, GPIO_PIN_7);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOE, GPIO_PIN_6);        /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D2_DIR, PIN_D2_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOB, GPIO_PIN_7);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_6);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_GPIO_SPI)
    {
        GPIO_DIR_SET_OUT(GPIO_D2_DIR, PIN_D2_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOB, GPIO_PIN_7);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_AF_PP(GPIOE, GPIO_PIN_6);         /* 配置为GPIO AF功能 */
    }  
    else if (_mode == ES_PROG_SPI_FLASH)    /* MOSI_1 = PD0(写时) PE6(读时) */
    {
        GPIO_DIR_SET_OUT(GPIO_D2_DIR, PIN_D2_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOB, GPIO_PIN_7);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_6);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOD, GPIO_PIN_0);        /* 配置为GPIO 输出功能 */
    }    
    else
    {
        g_tVar.GpioMode[2] = 0;
    }
    g_tVar.GpioMode[2] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D3_Config
*    功能说明: 配置D3端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D3_DIR GPIOG
#define PIN_D3_DIR GPIO_PIN_10
void EIO_D3_Config(EIO_SELECT_E _mode)
{
    /*
        【D3】 - 方向 PG10/D3_DIR
        PD1/FMC_D3
        PE5/SPI4_MISO  （GPIO)
        PH11/TIM5_CH2/ENCODE1
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D3_DIR, PIN_D3_DIR);      /* 配置方向引脚 */
    GPIO_INIT_FMC(GPIOD, GPIO_PIN_1);               /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_11);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_5);                /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D3_DIR, PIN_D3_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D3_DIR, PIN_D3_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_11);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOE, GPIO_PIN_5);        /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D3_DIR, PIN_D3_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_11);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_GPIO_SPI)
    {
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_11);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_AF_PP(GPIOE, GPIO_PIN_5);         /* 配置为GPIO AF */
        GPIO_DIR_SET_IN(GPIO_D3_DIR, PIN_D3_DIR);   /* 设置为输入方向 - 后执行 */
    }  
    else if (_mode == ES_PROG_SPI_FLASH)    /* MOSI_2 = PD1(写时) PE5(读时) */
    {
        GPIO_DIR_SET_OUT(GPIO_D3_DIR, PIN_D3_DIR);  /* 设置为输出方向 - 先执行 */
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_11);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_5);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOD, GPIO_PIN_1);        /* 配置为GPIO 输出功能 */                
    }
    else
    {
        g_tVar.GpioMode[3] = 0;
    }
    g_tVar.GpioMode[3] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D4_Config
*    功能说明: 配置D4端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D4_DIR GPIOG
#define PIN_D4_DIR GPIO_PIN_12
void EIO_D4_Config(EIO_SELECT_E _mode)
{
    /*
        【D4】 - 方向 PG12/D4_DIR
        PE7/FMC_D4
        PE4/SPI4_NSS  --- gpio
        PH12/TIM5_CH3
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D4_DIR, PIN_D4_DIR);      /* 配置方向引脚 */
    GPIO_INIT_FMC(GPIOE, GPIO_PIN_7);               /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_12);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_4);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D4_DIR, PIN_D4_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D4_DIR, PIN_D4_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_12);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOE, GPIO_PIN_4);        /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D4_DIR, PIN_D4_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_12);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_4);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_PROG_SPI_FLASH)    /* MISO_1 = PE4 */
    {
        GPIO_INIT_INPUT(GPIOH, GPIO_PIN_12);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_4);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D4_DIR, PIN_D4_DIR);   /* 设置为输入方向 - 后执行 */        
    }
    else
    {
        g_tVar.GpioMode[4] = 0;
    }
    g_tVar.GpioMode[4] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D5_Config
*    功能说明: 配置D5端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D5_DIR GPIOG
#define PIN_D5_DIR GPIO_PIN_7
void EIO_D5_Config(EIO_SELECT_E _mode)
{
    /*
        【D5】 - 方向 PG7/D5_DIR 
        PE8/FMC_D5
        PE2/SPI4_SCK        --- GPIO
        PI5/TIM8_CH1/ENCODE2
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D5_DIR, PIN_D5_DIR);      /* 配置方向引脚 */
    GPIO_INIT_FMC(GPIOE, GPIO_PIN_8);               /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_2);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D5_DIR, PIN_D5_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D5_DIR, PIN_D5_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOE, GPIO_PIN_2);        /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D5_DIR, PIN_D5_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_2);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_GPIO_SPI)
    {
        GPIO_DIR_SET_OUT(GPIO_D5_DIR, PIN_D5_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_AF_PP(GPIOE, GPIO_PIN_2);         /* 配置为GPIO 为AF给 */
    }
    else if (_mode == ES_PROG_SPI_FLASH)    /* MISO_2 = PE2 */
    {
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_2);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D5_DIR, PIN_D5_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else
    {
        g_tVar.GpioMode[5] = 0;
    }
    g_tVar.GpioMode[5] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D6_Config
*    功能说明: 配置D6端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D6_DIR GPIOD
#define PIN_D6_DIR GPIO_PIN_10
void EIO_D6_Config(EIO_SELECT_E _mode)
{
    /*
        【D6】 - 方向 PD10/D6_DIR
        PE9/FMC_D6      
        PD3/SPI2_SCK    --- gpio
        PA0/TIM2_CH1
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D6_DIR, PIN_D6_DIR);      /* 配置方向引脚 */
    GPIO_INIT_FMC(GPIOE, GPIO_PIN_9);               /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_3);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D6_DIR, PIN_D6_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D6_DIR, PIN_D6_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOD, GPIO_PIN_3);        /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_GPIO_SWD_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D6_DIR, PIN_D6_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_3);         /* 配置为GPIO 输出功能 */
        GPIO_INIT_OUT_PP(GPIOE, GPIO_PIN_9);
    }    
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D6_DIR, PIN_D6_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_3);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_PROG_SPI_FLASH)    /*  MOSI_3 = PD3(写时) PE9(读时) */
    {
        GPIO_DIR_SET_OUT(GPIO_D6_DIR, PIN_D6_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOD, GPIO_PIN_3);        /* 配置为GPIO 输出功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_9);
    }
    else
    {
        g_tVar.GpioMode[6] = 0;
    }
    g_tVar.GpioMode[6] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D7_Config
*    功能说明: 配置D7端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D7_DIR     GPIOI
#define PIN_D7_DIR      GPIO_PIN_1
void EIO_D7_Config(EIO_SELECT_E _mode)
{
    /*
        【D7】 - 方向 PF5/D7_DIR -修改为-> PI1/D7_DIR
        PE10/FMC_D7
        PI6/TIM8_CH2/ENCODE2   - gpio
    
        PF0/I2C2_SDA 
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D7_DIR, PIN_D7_DIR);      /* 配置方向引脚 */
    GPIO_INIT_FMC(GPIOE, GPIO_PIN_10);              /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_6);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D7_DIR, PIN_D7_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_OUT(GPIO_D7_DIR, PIN_D7_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_OUT_PP(GPIOI, GPIO_PIN_6);        /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_GPIO_SWD_OUT)
    {
        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_6);         /* 配置为GPIO 输出功能 */
        GPIO_DIR_SET_OUT(GPIO_D7_DIR, PIN_D7_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_OUT_PP(GPIOE, GPIO_PIN_10);
    }    
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D7_DIR, PIN_D7_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_6);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_GPIO_I2C)
    {
        GPIO_INIT_OUT_OD(GPIOF, GPIO_PIN_0);        /* 配置为GPIO 输出开漏功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_6);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D7_DIR, PIN_D7_DIR);   /* 设置为输入方向 - 后执行 */        
    }
    else if (_mode == ES_PROG_SPI_FLASH)    /*  MISO_3 = PE10 */
    {
        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_0);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_6);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_10);        /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D7_DIR, PIN_D7_DIR);   /* 设置为输入方向 - 后执行 */        
    }
    else
    {
        g_tVar.GpioMode[7] = 0;
    }
    g_tVar.GpioMode[7] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D8_Config
*    功能说明: 配置D8端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D8_DIR GPIOG
#define PIN_D8_DIR GPIO_PIN_9
void EIO_D8_Config(EIO_SELECT_E _mode)
{
    /*
        【D8】 - 方向 PG9/NOE_DIR
        PE11/FMC_D8
        PD4/FMC_NOE     - GPIO
        PI3/SPI2_MOSI    
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D8_DIR, PIN_D8_DIR);      /* 配置方向引脚 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_FMC(GPIOE, GPIO_PIN_11);          /* 配置为FMC_D8功能 */

        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_4);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_3);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D8_DIR, PIN_D8_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_INIT_FMC(GPIOE, GPIO_PIN_11);          /* 配置为FMC_D8功能 */

        GPIO_DIR_SET_OUT(GPIO_D8_DIR, PIN_D8_DIR);  /* 设置为输出方向 - 先执行  */
        
        #if 0
            GPIO_INIT_INPUT(GPIOD, GPIO_PIN_4);     /* 配置为GPIO 输入功能 */
            GPIO_INIT_OUT_PP(GPIOI, GPIO_PIN_3);    /* 配置为GPIO 输出功能 */
        #else  /* FOR 软件SWD优化 */
            GPIO_INIT_OUT_PP(GPIOD, GPIO_PIN_4);    /* 配置为GPIO 输出功能 */
            GPIO_INIT_INPUT(GPIOI, GPIO_PIN_3);     /* 配置为GPIO 输入功能 */       
        #endif
    }
    else if (_mode == ES_GPIO_SWD_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D8_DIR, PIN_D8_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_OUT_PP(GPIOE, GPIO_PIN_11); 
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_4);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_3);         /* 配置为GPIO 输入功能 */       
    }    
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_INIT_FMC(GPIOE, GPIO_PIN_11);          /* 配置为FMC_D8功能 */

        GPIO_DIR_SET_OUT(GPIO_D8_DIR, PIN_D8_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_4);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_3);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_FMC_NOE)
    {
        GPIO_DIR_SET_OUT(GPIO_D8_DIR, PIN_D8_DIR);  /* 设置为输出方向 - 先执行  */

        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_3);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_11);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_FMC(GPIOD, GPIO_PIN_4);           /* 配置为FMC功能 NOE */
    }
    else if (_mode == ES_PROG_SPI_FLASH)    /*  MOSI_4 = PD4(写时) PE11(读时) */
    {
        GPIO_DIR_SET_OUT(GPIO_D8_DIR, PIN_D8_DIR);  /* 设置为输出方向 - 先执行  */
        
        GPIO_INIT_OUT_PP(GPIOD, GPIO_PIN_4);    /* 配置为GPIO 输出功能 */
        GPIO_INIT_INPUT(GPIOI, GPIO_PIN_3);     /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_3);     /* 配置为GPIO 输入功能 */         
    }
    else
    {
        g_tVar.GpioMode[8] = 0;
    }
    g_tVar.GpioMode[8] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D9_Config
*    功能说明: 配置D9端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
#define GPIO_D9_DIR GPIOI
#define PIN_D9_DIR GPIO_PIN_2
void EIO_D9_Config(EIO_SELECT_E _mode)
{
    /*
        【D9】 - 方向 PI2/NWE_DIR
        PE12/FMC_D9
        PD5/FMC_NWE   -- GPIO
        PF1/I2C2_SCL
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_OUT_PP(GPIO_D9_DIR, PIN_D9_DIR);      /* 配置方向引脚 */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_FMC(GPIOE, GPIO_PIN_12);          /* 配置为FMC功能 */

        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_1);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D9_DIR, PIN_D9_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_INIT_FMC(GPIOE, GPIO_PIN_12);          /* 配置为FMC功能 */

        GPIO_DIR_SET_OUT(GPIO_D9_DIR, PIN_D9_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_1);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_OUT_PP(GPIOD, GPIO_PIN_5);        /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_GPIO_SWD_OUT)
    {
        GPIO_DIR_SET_OUT(GPIO_D9_DIR, PIN_D9_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_1);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_5);         /* 配置为GPIO 输出功能 */
        GPIO_INIT_OUT_PP(GPIOE, GPIO_PIN_12);       /* 配置为输出功能 */
    }    
    else if (_mode == ES_FMC_OUT)
    {
        GPIO_INIT_FMC(GPIOE, GPIO_PIN_12);          /* 配置为FMC功能 */

        GPIO_DIR_SET_OUT(GPIO_D9_DIR, PIN_D9_DIR);  /* 设置为输出方向 - 先执行  */
        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_1);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_FMC_NWE)
    {
        GPIO_DIR_SET_OUT(GPIO_D9_DIR, PIN_D9_DIR);  /* 设置为输出方向 - 先执行  */

        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_1);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_12);        /* 配置为GPIO 输入功能 */
        GPIO_INIT_FMC(GPIOD, GPIO_PIN_5);           /* 配置为FMC功能 */
    }
    else if (_mode == ES_GPIO_I2C)
    {
        GPIO_INIT_FMC(GPIOE, GPIO_PIN_12);          /* 配置为FMC功能 */

        GPIO_INIT_OUT_OD(GPIOF, GPIO_PIN_1);        /* 配置为GPIO 开漏输出功能 */
        
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D9_DIR, PIN_D9_DIR);   /* 设置为输入方向 - 后执行 */
    }
    else if (_mode == ES_PROG_SPI_FLASH)    /*   MISO_4 = PE12(读时) */
    {
        GPIO_INIT_INPUT(GPIOE, GPIO_PIN_12);        /* 配置为GPIO 输入功能 */

        GPIO_INIT_INPUT(GPIOF, GPIO_PIN_1);         /* 配置为GPIO 输入功能 */
        GPIO_INIT_INPUT(GPIOD, GPIO_PIN_5);         /* 配置为GPIO 输入功能 */
        GPIO_DIR_SET_IN(GPIO_D9_DIR, PIN_D9_DIR);   /* 设置为输入方向 - 后执行 */   
    }
    else
    {
        g_tVar.GpioMode[9] = 0;
    }
    g_tVar.GpioMode[9] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D10_Config
*    功能说明: 配置D10端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
void EIO_D10_Config(EIO_SELECT_E _mode)
{
    /*
        【D10  TTLTX】方向   硬件只做输出
        PA9/USART1_TX
        PE13/FMC_D10
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_FMC(GPIOE, GPIO_PIN_13);          /* 配置为FMC功能 */

    if (_mode == ES_GPIO_IN)
    {
        ;
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_INIT_OUT_PP(GPIOA, GPIO_PIN_9);    /* 配置为GPIO 输出功能 */
    }
    else if (_mode == ES_GPIO_UART)
    {
        /* PA9 - UART  */
        gpio_init.Pin = GPIO_PIN_9;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pull = GPIO_PULLUP;
        gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio_init.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &gpio_init);   
    }
    else
    {
        g_tVar.GpioMode[10] = ES_GPIO_OUT;
    }
    g_tVar.GpioMode[10] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D11_Config
*    功能说明: 配置D11端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
void EIO_D11_Config(EIO_SELECT_E _mode)
{
    /*
        这个GPIO端口只能做TTL输入。无输出功能。
    
        【D11  TTLRX】- 方向 PG0 = 0 
        PA10/USART1_RX
        PE14/FMC_D11    
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_FMC(GPIOE, GPIO_PIN_14);          /* 配置为FMC功能 */

    /* PA10 - UART  */
    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_10);    /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        /* 禁止 */
    }
    else if (_mode == ES_GPIO_UART)
    {
        /* 配置RX引脚 */
        gpio_init.Pin = GPIO_PIN_10;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pull = GPIO_PULLUP;
        gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpio_init.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &gpio_init);        
    }
    else
    {
        g_tVar.GpioMode[11] = ES_GPIO_IN;
    }
    g_tVar.GpioMode[11] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D12_Config
*    功能说明: 配置D12端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
void EIO_D12_Config(EIO_SELECT_E _mode)
{
    /*
        连接的CAN芯片，只能做输出
    
        【D12 CAN_TX】
            PA12/CAN1_TX
            PE15/FMC_D12
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_FMC(GPIOE, GPIO_PIN_15);              /* 配置为FMC功能 */

    /* PA12/CAN1_TX */

    if (_mode == ES_GPIO_IN)
    {
        //GPIO_INIT_INPUT(GPIOA, GPIO_PIN_12);      /* 配置为GPIO 输入功能 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_INIT_OUT_PP(GPIOA, GPIO_PIN_12);       /* 配置为输出 */
    }
    else if (_mode == ES_GPIO_CAN)
    {
        /* 还未做 */;
    }
    else
    {
        g_tVar.GpioMode[12] = ES_GPIO_OUT;
    }
    g_tVar.GpioMode[12] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_D13_Config
*    功能说明: 配置D13端口功能
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
void EIO_D13_Config(EIO_SELECT_E _mode)
{
    /*
        连接的CAN芯片，只能做输入
    
        【D13 CAN_RX】
        PA11/CAN1_RX
        PD8/FMC_D13
    */
    GPIO_InitTypeDef gpio_init;

    GPIO_INIT_FMC(GPIOD, GPIO_PIN_8);           /* 配置为FMC功能 */

    /* PA11/CAN1_RX */

    if (_mode == ES_GPIO_IN)
    {
        GPIO_INIT_INPUT(GPIOA, GPIO_PIN_11);    /* 配置为输出 */
    }
    else if (_mode == ES_GPIO_OUT)
    {
        //GPIO_INIT_OUT_PP(GPIOA, GPIO_PIN_11); /* 配置为输出 */
    }
    else if (_mode == ES_GPIO_CAN)
    {
        ;
    }
    else
    {
        g_tVar.GpioMode[13] = 0;
    }
    g_tVar.GpioMode[13] = _mode;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_485TXEN_Config
*    功能说明: 配置485口方向
*    形    参: _mode 模式
*    返 回 值: 无
*********************************************************************************************************
*/
void EIO_485TXEN_Config(EIO_SELECT_E _mode)
{
    /*
        PI10/RS485_TXEN
    */
    GPIO_InitTypeDef gpio_init;

    if (_mode == ES_GPIO_IN)
    {
        ;
    }
    else if (_mode == ES_GPIO_OUT)
    {
        GPIO_INIT_OUT_PP(GPIOI, GPIO_PIN_10);   /* 配置为输出 */
    }
    else if (_mode == ES_GPIO_CAN)
    {
        ;
    }
}

/*
*********************************************************************************************************
*    函 数 名: EIO_SetLeveluint8_t
*    功能说明: 设置端口输出电平
*    形    参: _eio : 哪个脚
*              _level : 0低电平 1高电平
*    返 回 值: 无
*********************************************************************************************************
*/
void EIO_SetOutLevel(uint8_t _eio, uint8_t _level)
{
    if (_level == 0)
    {
        if (_eio == EIO_D0)
            GPIO_SET_LOW(GPIOI, GPIO_PIN_0);
        else if (_eio == EIO_D1)
            GPIO_SET_LOW(GPIOH, GPIO_PIN_10);
        else if (_eio == EIO_D2)
            GPIO_SET_LOW(GPIOE, GPIO_PIN_6);
        else if (_eio == EIO_D3)
            GPIO_SET_LOW(GPIOE, GPIO_PIN_5);
        else if (_eio == EIO_D4)
            GPIO_SET_LOW(GPIOE, GPIO_PIN_4);
        else if (_eio == EIO_D5)
            GPIO_SET_LOW(GPIOE, GPIO_PIN_2);
        else if (_eio == EIO_D6)
            GPIO_SET_LOW(GPIOD, GPIO_PIN_3);
        else if (_eio == EIO_D7)
            GPIO_SET_LOW(GPIOI, GPIO_PIN_6);
        else if (_eio == EIO_D8)
            GPIO_SET_LOW(GPIOD, GPIO_PIN_4);
        else if (_eio == EIO_D9)
            GPIO_SET_LOW(GPIOD, GPIO_PIN_5);
        else if (_eio == EIO_D10)
            GPIO_SET_LOW(GPIOA, GPIO_PIN_9);
        else if (_eio == EIO_D11)
            GPIO_SET_LOW(GPIOA, GPIO_PIN_10);
        else if (_eio == EIO_D12)
            GPIO_SET_LOW(GPIOA, GPIO_PIN_12);

        else if (_eio == EIO_485_TXEN)
            GPIO_SET_LOW(GPIOI, GPIO_PIN_10);
    }
    else if (_level == 1)
    {
        if (_eio == EIO_D0)
            GPIO_SET_HIGH(GPIOI, GPIO_PIN_0);
        else if (_eio == EIO_D1)
            GPIO_SET_HIGH(GPIOH, GPIO_PIN_10);
        else if (_eio == EIO_D2)
            GPIO_SET_HIGH(GPIOE, GPIO_PIN_6);
        else if (_eio == EIO_D3)
            GPIO_SET_HIGH(GPIOE, GPIO_PIN_5);
        else if (_eio == EIO_D4)
            GPIO_SET_HIGH(GPIOE, GPIO_PIN_4);
        else if (_eio == EIO_D5)
            GPIO_SET_HIGH(GPIOE, GPIO_PIN_2);
        else if (_eio == EIO_D6)
            GPIO_SET_HIGH(GPIOD, GPIO_PIN_3);
        else if (_eio == EIO_D7)
            GPIO_SET_HIGH(GPIOI, GPIO_PIN_6);
        else if (_eio == EIO_D8)
            GPIO_SET_HIGH(GPIOD, GPIO_PIN_4);
        else if (_eio == EIO_D9)
            GPIO_SET_HIGH(GPIOD, GPIO_PIN_5);
        else if (_eio == EIO_D10)
            GPIO_SET_HIGH(GPIOA, GPIO_PIN_9);
        else if (_eio == EIO_D11)
            GPIO_SET_HIGH(GPIOA, GPIO_PIN_10);
        else if (_eio == EIO_D12)
            GPIO_SET_HIGH(GPIOA, GPIO_PIN_12);

        else if (_eio == EIO_485_TXEN)
            GPIO_SET_HIGH(GPIOI, GPIO_PIN_10);
    }
}

/*
*********************************************************************************************************
*    函 数 名: EIO_GetOutLevel
*    功能说明: 获得端口输出电平
*    形    参: _eio : 哪个脚
*              _level : 0低电平 1高电平
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t EIO_GetOutLevel(uint8_t _eio)
{
    uint8_t re = 0;

    if (_eio == EIO_D0)
        if (GPIO_OUT_IS_HIGH(GPIOI, GPIO_PIN_0))
            re = 1;
    if (_eio == EIO_D1)
        if (GPIO_OUT_IS_HIGH(GPIOH, GPIO_PIN_10))
            re = 1;
    if (_eio == EIO_D2)
        if (GPIO_OUT_IS_HIGH(GPIOE, GPIO_PIN_6))
            re = 1;
    if (_eio == EIO_D3)
        if (GPIO_OUT_IS_HIGH(GPIOE, GPIO_PIN_5))
            re = 1;
    if (_eio == EIO_D4)
        if (GPIO_OUT_IS_HIGH(GPIOE, GPIO_PIN_4))
            re = 1;
    if (_eio == EIO_D5)
        if (GPIO_OUT_IS_HIGH(GPIOE, GPIO_PIN_2))
            re = 1;
    if (_eio == EIO_D6)
        if (GPIO_OUT_IS_HIGH(GPIOD, GPIO_PIN_3))
            re = 1;
    if (_eio == EIO_D7)
        if (GPIO_OUT_IS_HIGH(GPIOI, GPIO_PIN_6))
            re = 1;
    if (_eio == EIO_D8)
        if (GPIO_OUT_IS_HIGH(GPIOD, GPIO_PIN_4))
            re = 1;
    if (_eio == EIO_D9)
        if (GPIO_OUT_IS_HIGH(GPIOD, GPIO_PIN_5))
            re = 1;
    if (_eio == EIO_D10)
        if (GPIO_OUT_IS_HIGH(GPIOA, GPIO_PIN_9))
            re = 1;
    if (_eio == EIO_D11)
        if (GPIO_OUT_IS_HIGH(GPIOA, GPIO_PIN_10))
            re = 1;
    if (_eio == EIO_D12)
        if (GPIO_OUT_IS_HIGH(GPIOA, GPIO_PIN_12))
            re = 1;
    return re;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_GetInputLevel
*    功能说明: 获得端口输入电平
*    形    参: _eio : 哪个脚
*              _level : 0低电平 1高电平
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t EIO_GetInputLevel(uint8_t _eio)
{
    uint8_t re = 0;

    if (_eio == EIO_D0)
        if (GPIO_IN_IS_HIGH(GPIOI, GPIO_PIN_0))
            re = 1;
    if (_eio == EIO_D1)
        if (GPIO_IN_IS_HIGH(GPIOH, GPIO_PIN_10))
            re = 1;
    if (_eio == EIO_D2)
        if (GPIO_IN_IS_HIGH(GPIOE, GPIO_PIN_6))
            re = 1;
    if (_eio == EIO_D3)
        if (GPIO_IN_IS_HIGH(GPIOE, GPIO_PIN_5))
            re = 1;
    if (_eio == EIO_D4)
        if (GPIO_IN_IS_HIGH(GPIOE, GPIO_PIN_4))
            re = 1;
    if (_eio == EIO_D5)
        if (GPIO_IN_IS_HIGH(GPIOE, GPIO_PIN_2))
            re = 1;
    if (_eio == EIO_D6)
        if (GPIO_IN_IS_HIGH(GPIOD, GPIO_PIN_3))
            re = 1;
    if (_eio == EIO_D7)
        if (GPIO_IN_IS_HIGH(GPIOI, GPIO_PIN_6))
            re = 1;
    if (_eio == EIO_D8)
        if (GPIO_IN_IS_HIGH(GPIOD, GPIO_PIN_4))
            re = 1;
    if (_eio == EIO_D9)
        if (GPIO_IN_IS_HIGH(GPIOD, GPIO_PIN_5))
            re = 1;

    if (_eio == EIO_D10)
        if (GPIO_IN_IS_HIGH(GPIOA, GPIO_PIN_9))
            re = 1;
    if (_eio == EIO_D11)
        if (GPIO_IN_IS_HIGH(GPIOA, GPIO_PIN_10))
            re = 1;
    if (_eio == EIO_D12)
        if (GPIO_IN_IS_HIGH(GPIOA, GPIO_PIN_12))
            re = 1;

    if (_eio == EIO_D13)
        if (GPIO_IN_IS_HIGH(GPIOA, GPIO_PIN_11))
            re = 1;

    return re;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_GetDir
*    功能说明: 获得端口方向
*    形    参: _eio : 哪个脚
*    返 回 值: 0 表示输入，1表示输出
*********************************************************************************************************
*/
uint8_t EIO_GetDir(uint8_t _eio)
{
    uint8_t re = 0;

    if (_eio == EIO_D0)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D0_DIR, PIN_D0_DIR))
            re = 1;
    }
    else if (_eio == EIO_D1)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D1_DIR, PIN_D1_DIR))
            re = 1;
    }
    else if (_eio == EIO_D2)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D2_DIR, PIN_D2_DIR))
            re = 1;
    }
    else if (_eio == EIO_D3)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D3_DIR, PIN_D3_DIR))
            re = 1;
    }
    else if (_eio == EIO_D4)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D4_DIR, PIN_D4_DIR))
            re = 1;
    }
    else if (_eio == EIO_D5)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D5_DIR, PIN_D5_DIR))
            re = 1;
    }
    else if (_eio == EIO_D6)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D6_DIR, PIN_D6_DIR))
            re = 1;
    }
    else if (_eio == EIO_D7)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D7_DIR, PIN_D7_DIR))
            re = 1;
    }
    else if (_eio == EIO_D8)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D8_DIR, PIN_D8_DIR))
            re = 1;
    }
    else if (_eio == EIO_D9)
    {
        if (GPIO_OUT_IS_HIGH(GPIO_D9_DIR, PIN_D9_DIR))
            re = 1;
    }

    else if (_eio == EIO_D10)
    {
        re = 0;
    } /* TTL-RX */
    else if (_eio == EIO_D11)
    {
        re = 1;
    } /* TTL-TX */
    else if (_eio == EIO_D12)
    {
        re = 1;
    } /* CAN-TX */
    else if (_eio == EIO_D13)
    {
        re = 0;
    } /* CAN-RX */

    return re;
}

/*
*********************************************************************************************************
*    函 数 名: EIO_ConfigGPIO
*    功能说明: 配置GPIO，FMC管脚设置为复用功能
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
static void EIO_ConfigGPIO(void)
{
    /*
    H7-TOOL
    
    PA9/USART1_TX/PE13/FMC_D10
    
    PD0/FMC_D2
    PD1/FMC_D3
    PD4/FMC_NOE        ---- 读控制信号，OE = Output Enable ， N 表示低有效
    PD5/FMC_NWE        -XX- 写控制信号，AD7606 只有读，无写信号
    PD8/FMC_D13
    PD9/FMC_D14
    PD10/FMC_D15
    PD14/FMC_D0
    PD15/FMC_D1

    PE7/FMC_D4
    PE8/FMC_D5
    PE9/FMC_D6
    PE10/FMC_D7
    PE11/FMC_D8
    PE12/FMC_D9
    PE13/FMC_D10
    PE14/FMC_D11
    PE15/FMC_D12
    
    PG0/FMC_A10        --- 和主片选FMC_NE2一起译码
    PG1/FMC_A11        --- 和主片选FMC_NE2一起译码
    XX --- PG9/FMC_NE2        --- 主片选（OLED, 74HC574, DM9000, AD7606）    
     --- PD7/FMC_NE1        --- 主片选（OLED, 74HC574, DM9000, AD7606）    
    
     +-------------------+------------------+
     +   32-bits Mode: D31-D16              +
     +-------------------+------------------+
     | PH8 <-> FMC_D16   | PI0 <-> FMC_D24  |
     | PH9 <-> FMC_D17   | PI1 <-> FMC_D25  |
     | PH10 <-> FMC_D18  | PI2 <-> FMC_D26  |
     | PH11 <-> FMC_D19  | PI3 <-> FMC_D27  |
     | PH12 <-> FMC_D20  | PI6 <-> FMC_D28  |
     | PH13 <-> FMC_D21  | PI7 <-> FMC_D29  |
     | PH14 <-> FMC_D22  | PI9 <-> FMC_D30  |
     | PH15 <-> FMC_D23  | PI10 <-> FMC_D31 |
     +------------------+-------------------+    
*/

    /* 使能 GPIO时钟  -- 全部都打开了 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    /* 使能FMC时钟 */
    __HAL_RCC_FMC_CLK_ENABLE();
}

/*
*********************************************************************************************************
*    函 数 名: EIO_ConfigFMC
*    功能说明: 配置FMC并口访问时序
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
static void EIO_ConfigFMC(void)
{
    SRAM_HandleTypeDef hsram = {0};
    FMC_NORSRAM_TimingTypeDef SRAM_Timing = {0};

    hsram.Instance = FMC_NORSRAM_DEVICE;
    hsram.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

    /* SRAM 总线时序配置 4-1-2-1-2-2 不稳定，5-2-2-1-2-2 稳定 */
    SRAM_Timing.AddressSetupTime = 0;
    SRAM_Timing.AddressHoldTime = 0;
    SRAM_Timing.DataSetupTime = 0;
    SRAM_Timing.BusTurnAroundDuration = 0;
    SRAM_Timing.CLKDivision = 0;
    SRAM_Timing.DataLatency = 0;
    SRAM_Timing.AccessMode = FMC_ACCESS_MODE_A;

    //    hsram.Init.NSBank             = FMC_NORSRAM_BANK1;
    //    hsram.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
    //    hsram.Init.MemoryType         = FMC_MEMORY_TYPE_NOR; // FMC_MEMORY_TYPE_SRAM;
    //    hsram.Init.MemoryDataWidth    = FMC_NORSRAM_MEM_BUS_WIDTH_32;    /* 16位总线宽度 */
    //    hsram.Init.BurstAccessMode    = FMC_BURST_ACCESS_MODE_DISABLE;
    //    hsram.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
    //    hsram.Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;
    //    hsram.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
    //    hsram.Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;
    //    hsram.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
    //    hsram.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;
    //    hsram.Init.WriteBurst         = FMC_WRITE_BURST_DISABLE;
    //    hsram.Init.ContinuousClock    = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;

    hsram.Init.NSBank = FMC_NORSRAM_BANK1;
    hsram.Init.DataAddressMux = FMC_DATA_ADDRESS_MUX_DISABLE;
    hsram.Init.MemoryType = FMC_MEMORY_TYPE_SRAM;               // FMC_MEMORY_TYPE_NOR; // FMC_MEMORY_TYPE_SRAM;
    hsram.Init.MemoryDataWidth = FMC_NORSRAM_MEM_BUS_WIDTH_16;  /* 16位总线宽度 */
    hsram.Init.BurstAccessMode = FMC_BURST_ACCESS_MODE_DISABLE;
    hsram.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
    hsram.Init.WaitSignalActive = FMC_WAIT_TIMING_BEFORE_WS;
    hsram.Init.WriteOperation = FMC_WRITE_OPERATION_ENABLE;
    hsram.Init.WaitSignal = FMC_WAIT_SIGNAL_DISABLE;
    hsram.Init.ExtendedMode = FMC_EXTENDED_MODE_DISABLE;
    hsram.Init.AsynchronousWait = FMC_ASYNCHRONOUS_WAIT_DISABLE;
    hsram.Init.WriteBurst = FMC_WRITE_BURST_DISABLE;
    hsram.Init.ContinuousClock = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;

    /* 初始化SRAM控制器 */
    if (HAL_SRAM_Init(&hsram, &SRAM_Timing, &SRAM_Timing) != HAL_OK)
    {
        /* 初始化错误 */
        Error_Handler(__FILE__, __LINE__);
    }
}

/*
*********************************************************************************************************
*    函 数 名: EIO_ReadFMC
*    功能说明: 读取FMC总线数据。16bit。 D15-D0.  也可直接用EIO_READ_FMC()访问。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint16_t EIO_ReadFMC(void)
{
    return EIO_READ_FMC();
}

/*
*********************************************************************************************************
*    函 数 名: BSP_CFG_GPIO_OUT
*    功能说明: 配置多个GPIO为输出模式
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void BSP_CFG_GPIO_OUT(GPIO_TypeDef* GPIOx, uint16_t pin)
{
    uint32_t uiMask;
    uint32_t uiNewValue;
    uint8_t i;

    uiMask = 0;
    uiNewValue = 0;
    for (i = 0; i < 16; i++)
    {
        uiMask <<= 2;
        uiNewValue <<= 2;
        if (pin & 0x8000)
        {
            uiMask += 0x03;
            uiNewValue += 0x01;
        }
        pin <<= 1;
    }
    uiMask = ~uiMask;
    
    GPIOx->MODER = (GPIOx->MODER & uiMask) | uiNewValue;
}

/*
*********************************************************************************************************
*    函 数 名: BSP_CFG_GPIO_IN
*    功能说明: 配置多个GPIO为输入模式
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void BSP_CFG_GPIO_IN(GPIO_TypeDef* GPIOx, uint16_t pin)
{
    uint32_t uiMask;
    uint8_t i;

    uiMask = 0;
    for (i = 0; i < 16; i++)
    {
        uiMask <<= 2;
        if (pin & 0x8000)
        {
            uiMask += 0x03;
        }
        pin <<= 1;
    }
    uiMask = ~uiMask;
    
    GPIOx->MODER = GPIOx->MODER & uiMask;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
