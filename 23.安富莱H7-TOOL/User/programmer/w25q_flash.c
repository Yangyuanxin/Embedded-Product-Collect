/*
*********************************************************************************************************
*
*    模块名称 : 串行FLASH驱动
*    文件名称 : w25q_flash.c
*    版    本 : V1.0
*    说    明 : SPI串行FLASH驱动. 支持4路同步操作.
*    修改记录 :
*        版本号  日期       作者    说明
*        V1.0    2020-10-29 armfly  原创
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "prog_if.h"
#include "DAP_config.h"
#include "n76e003_flash.h"
#include "w25q_flash.h"
#include "SW_DP_Multi.h"

/* 常用的厂商JEDEC ID */
#define SF_ID_CYPRESS                             0x01
#define SF_ID_FUJITSU                             0x04
#define SF_ID_EON                                 0x1C
#define SF_ID_ATMEL                               0x1F
#define SF_ID_MICRON                              0x20
#define SF_ID_AMIC                                0x37
#define SF_ID_SANYO                               0x62
#define SF_ID_INTEL                               0x89
#define SF_ID_ESMT                                0x8C
#define SF_ID_FUDAN                               0xA1
#define SF_ID_HYUNDAI                             0xAD
#define SF_ID_SST                                 0xBF
#define SF_ID_MICRONIX                            0xC2
#define SF_ID_GIGADEVICE                          0xC8
#define SF_ID_ISSI                                0xD5
#define SF_ID_WINBOND                             0xEF


#define CMD_DISWR	  0x04		/* 禁止写, 退出AAI状态 */
#define CMD_EWRSR	  0x50		/* 允许写状态寄存器的命令 */
#define CMD_WRSR      0x01  	/* 写状态寄存器命令 */
#define CMD_WREN      0x06		/* 写使能命令 */
#define CMD_READ      0x03  	/* 读数据区命令 */
#define CMD_RDSR      0x05		/* 读状态寄存器命令 */
#define CMD_RDID      0x9F		/* 读器件ID命令 */
#define CMD_SE        0x20		/* 擦除扇区命令 */
#define CMD_BE        0xC7		/* 批量擦除命令 */


//#define CMD_AAI         0xAD  	/* AAI 连续编程指令(FOR SST25VF016B) */
#define CMD_ERASE_CHIP  0xC7		/* CHIP擦除命令, 镁光的片子只支持C7，很多芯片同时支持60和C7 */
#define CMD_PAGE_PROG   0x02		/* page编程256字节 */

#define DUMMY_BYTE    0xA5		/* 哑命令，可以为任意值，用于读操作 */

#define WIP_FLAG      0x01		/* 状态寄存器中的正在编程标志（WIP) */

/*
    单路模式: 
    D0  PD14 PA15 PI0    - DIR PH8       CS2    
    D1  PD15 PA8 PH19    - DIR PG8       CS3
    
    D2  PE6  PD0 PB7     - DIR PD9       MOSI_1  (硬件SPI信号)
    D3  PE5  PD1 PH11    - DIR PG10      MISO_1  (硬件SPI信号)
    D4  PE4  PE7 PH12    - DIR PG12      CS1    (软件控制片选)
    D5  PE2  PE8 PI5     - DIR PG7       SCK     (硬件SPI信号)
    D6  PE9  PD3 PA0     - DIR PD10      MOSI_2
    D7  PE10 PI6         - DIR PI1       MISO_2  (第2路MISO) 
    D8  PE11 PD4  PI3    - DIR PG9       MISO_3  (第3路MISO)
    D9  PE12 PD5         - DIR PI12      MISO_4  (第4路MISO)
    
    RS232_TX             - RS232串口机台信号扩展 START BUSY OK NG
    RS232_RX             - RS232串口机台信号扩展


    SPI 时钟上升沿采集数据，下降沿改变数据
    
    四路模式: 

    D0  PD14 PA15 PI0    - DIR PH8       CS    =  PI0
        D1  PD15 PA8 PH19    - DIR PG8       SCK = PD15
    
        D2  PE6  PD0 PB7     - DIR PD9       MOSI_1 = PD0(写时) PE6(读时)
        D3  PE5  PD1 PH11    - DIR PG10      MOSI_2 = PD1(写时) PE5(读时)
    D4  PE4  PE7 PH12    - DIR PG12      MISO_1 = PE4
    D5  PE2  PE8 PI5     - DIR PG7       MISO_2 = PE2
    D6  PE9  PD3 PA0         - DIR PD10      MOSI_3 = PD3(写时) PE9(读时)
    D7  PE10 PI6         - DIR PI1       MISO_3 = PE10
        D8  PE11 PD4  PI3    - DIR PG9       MOSI_4 = PD4(写时) PE11(读时)
    D9  PE12 PD5         - DIR PI12      MISO_4 = PE12(读时)
    
    RS232_TX             - RS232串口机台信号扩展 START BUSY OK NG
    RS232_RX             - RS232串口机台信号扩展
    
    
    ----整理下----
    D0  = CS 
    D1  = SCK
    
    D2  = MOSI_1
    D3  = MOSI_2
    D4  = MISO_1
    D5  = MISO_2
    D6  = MOSI_3
    D7  = MISO_3
    D8  = MOSI_4
    D9  = MISO_4

    RS232_TX
    RS232_RX
*/

__forceinline void PIN_DELAY_SLOW (uint32_t delay) {
  uint32_t count;

  count = delay;
  while (count--);
}

#define PIN_DELAY_S()       PIN_DELAY_SLOW(g_tProg.SwdClockDelay)

/* 片选四路共用 */
#define W25_CS_0()          BSP_SET_GPIO_0(GPIOI, GPIO_PIN_0) 
#define W25_CS_1()          BSP_SET_GPIO_1(GPIOI, GPIO_PIN_0) 

/* 时钟通用 */
#define W25_SCK_0()         BSP_SET_GPIO_0(GPIOD, GPIO_PIN_15)
#define W25_SCK_1()         BSP_SET_GPIO_1(GPIOD, GPIO_PIN_15)

/* MOSI 四路不同 */
#define W25_MOSI1_0()       BSP_SET_GPIO_0(GPIOD, GPIO_PIN_0)
#define W25_MOSI1_1()       BSP_SET_GPIO_1(GPIOD, GPIO_PIN_0)
 
#define W25_MOSI2_0()       BSP_SET_GPIO_0(GPIOD, GPIO_PIN_1)
#define W25_MOSI2_1()       BSP_SET_GPIO_1(GPIOD, GPIO_PIN_1)

#define W25_MOSI3_0()       BSP_SET_GPIO_0(GPIOD, GPIO_PIN_3)
#define W25_MOSI3_1()       BSP_SET_GPIO_1(GPIOD, GPIO_PIN_3)

#define W25_MOSI4_0()       BSP_SET_GPIO_0(GPIOD, GPIO_PIN_4)
#define W25_MOSI4_1()       BSP_SET_GPIO_1(GPIOD, GPIO_PIN_4)

#define W25_MOSI_0_SCK_0()  GPIOD->BSRR = (uint32_t)(GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4) << 16
#define W25_MOSI_1_SCK_0()  GPIOD->BSRR = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | ((uint32_t)GPIO_PIN_15 << 16)

/* MISO 四路不同 */

#define W25_READ_MISO()     GPIOE->IDR

#define W25_MISO1_PIN_A     GPIO_PIN_4
#define W25_MISO2_PIN_A     GPIO_PIN_2
#define W25_MISO3_PIN_A     GPIO_PIN_10
#define W25_MISO4_PIN_A     GPIO_PIN_12

#define W25_MISO1_PIN_B     GPIO_PIN_6
#define W25_MISO2_PIN_B     GPIO_PIN_5
#define W25_MISO3_PIN_B     GPIO_PIN_9
#define W25_MISO4_PIN_B     GPIO_PIN_11

/* MOSI设置为输出 D2 D3 D6 D8  === PD0 PD1 PD3 PD4 配置为输出  00000011 11001111 --- 00000001 01000101 */
#define W25_MOSI_SET_OUT()  BSP_SET_GPIO_1(GPIOD, GPIO_PIN_9 | GPIO_PIN_10); BSP_SET_GPIO_1(GPIOG, GPIO_PIN_9 | GPIO_PIN_10); \
    GPIOD->MODER = ((GPIOD->MODER & (~0x000003CF)) | 0x00000145);

/* MOSI设置为输入 */
#define W25_MOSI_SET_IN()  BSP_SET_GPIO_0(GPIOD, GPIO_PIN_9 | GPIO_PIN_10); BSP_SET_GPIO_0(GPIOG, GPIO_PIN_9 | GPIO_PIN_10); \
    GPIOD->MODER = GPIOD->MODER & (~0x000003CF);

static void W25Q_SendBit8Fast(uint8_t _data);
static void W25Q_SendBit8Slow(uint8_t _data);
static void W25Q_RaedBit8Fast(uint8_t *_rxbuf);
static void W25Q_RaedBit8Slow(uint8_t *_rxbuf);

void W25Q_UnlockBlock(void);
void W25Q_WriteStatusEnable(void);
uint8_t W25Q_WaitBusy(uint32_t _timeout);

W25Q_T g_tW25Q;

/*
*********************************************************************************************************
*    函 数 名: W25Q_InitHard
*    功能说明: ICP接口GPIO硬件初始化
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void W25Q_InitHard(void)
{   
    EIO_D0_Config(ES_PROG_SPI_FLASH);
    EIO_D1_Config(ES_PROG_SPI_FLASH);
    EIO_D2_Config(ES_PROG_SPI_FLASH);
    EIO_D4_Config(ES_PROG_SPI_FLASH);
    EIO_D5_Config(ES_PROG_SPI_FLASH);
    EIO_D6_Config(ES_PROG_SPI_FLASH);

    EIO_D3_Config(ES_PROG_SPI_FLASH);    
    EIO_D7_Config(ES_PROG_SPI_FLASH);
    EIO_D8_Config(ES_PROG_SPI_FLASH);
    EIO_D9_Config(ES_PROG_SPI_FLASH);
    
    /* 选MODE3，平时SCK为1 */
    W25_SCK_1();   

    W25Q_UnlockBlock();         /* SST26VFxxx 需要解除块保护 */
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_SendBit8Fast
*    功能说明: 传输8bit
*    形    参: _data：数据
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void W25Q_SendBit8Fast(uint8_t _data)
{
    uint8_t i;
    
    /* SCK下降沿改变数据，上升沿采集数据, 平时SCK = 1 */
    for (i = 0; i < 8; i++)
    {
        if (_data & 0x80)
        {
            W25_MOSI_1_SCK_0();
        }
        else
        {
            W25_MOSI_0_SCK_0();
        }        
        W25_SCK_1();         
        _data <<= 1;      
    }
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_SendBit8Slow
*    功能说明: 传输8bit, 低速模式, 用于进入和退出IAP
*    形    参: _data：数据
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void W25Q_SendBit8Slow(uint8_t _data)
{
    uint8_t i;
      
    /* SCK下降沿改变数据，上升沿采集数据, 平时SCK = 1 */
    for (i = 0; i < 8; i++)
    {
        if (_data & 0x80)
        {
            W25_MOSI_1_SCK_0();
        }
        else
        {
            W25_MOSI_0_SCK_0();
        }        
        PIN_DELAY_S();
        W25_SCK_1();         
        _data <<= 1;
        PIN_DELAY_S();        

    }
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_RaedBit8Fast
*    功能说明: 读8bit数据，4通道同步
*    形    参: 无
*    返 回 值: 4通道的数据
*********************************************************************************************************
*/ 
static void W25Q_RaedBit8Fast(uint8_t *_rxbuf)
{
    uint8_t i;
    uint32_t gpio;
    uint32_t ret = 0;
    
    /* SCK下降沿改变数据，上升沿采集数据, 平时SCK = 1 */
    for (i = 0; i < 8; i++)
    {        
        W25_SCK_0();
        ret <<= 1;
        W25_SCK_1();
        gpio = W25_READ_MISO();    
       
        if (gpio & W25_MISO1_PIN_A)
        {
            ret |= 0x00000001;
        }
        if (gpio & W25_MISO2_PIN_A)
        {
            ret |= 0x00000100;
        }
        if (gpio & W25_MISO3_PIN_A)
        {
            ret |= 0x00010000;
        }
        if (gpio & W25_MISO4_PIN_A)
        {
            ret |= 0x01000000;
        }        
        
    }
    
    _rxbuf[0] = ret;
    _rxbuf[1] = ret >> 8;
    _rxbuf[2] = ret >> 16;
    _rxbuf[3] = ret >> 24;
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_RaedBit8FastDaul
*    功能说明: 读8bit数据，4通道同步， 双线
*    形    参: 无
*    返 回 值: 4通道的数据
*********************************************************************************************************
*/ 
static void W25Q_RaedBit8FastDaul(uint8_t *_rxbuf)
{
    uint8_t i;
    uint32_t gpio;
    uint32_t ret = 0;
    
    /* SCK下降沿改变数据，上升沿采集数据, 平时SCK = 1 */
    for (i = 0; i < 4; i++)
    {        
        W25_SCK_0();
        if (gpio & W25_MISO1_PIN_B)
        {
            ret |= 0x00000001;
        }
        if (gpio & W25_MISO1_PIN_A)
        {
            ret |= 0x00000002;
        }
        
        if (gpio & W25_MISO2_PIN_B)
        {
            ret |= 0x00000100;
        }
        if (gpio & W25_MISO2_PIN_A)
        {
            ret |= 0x00000200;
        }
        
        if (gpio & W25_MISO3_PIN_B)
        {
            ret |= 0x00010000;
        }
        if (gpio & W25_MISO3_PIN_A)
        {
            ret |= 0x00020000;
        }
        
        if (gpio & W25_MISO4_PIN_B)
        {
            ret |= 0x01000000;
        }
        if (gpio & W25_MISO4_PIN_A)
        {
            ret |= 0x02000000;
        }
        ret <<= 2;        
        W25_SCK_1();
        gpio = W25_READ_MISO();            
    }

    if (gpio & W25_MISO1_PIN_B)
    {
        ret |= 0x00000001;
    }
    if (gpio & W25_MISO1_PIN_A)
    {
        ret |= 0x00000002;
    }
    
    if (gpio & W25_MISO2_PIN_B)
    {
        ret |= 0x00000100;
    }
    if (gpio & W25_MISO2_PIN_A)
    {
        ret |= 0x00000200;
    }
    
    if (gpio & W25_MISO3_PIN_B)
    {
        ret |= 0x00010000;
    }
    if (gpio & W25_MISO3_PIN_A)
    {
        ret |= 0x00020000;
    }
    
    if (gpio & W25_MISO4_PIN_B)
    {
        ret |= 0x01000000;
    }
    if (gpio & W25_MISO4_PIN_A)
    {
        ret |= 0x02000000;
    }    

    _rxbuf[0] = ret;
    _rxbuf[1] = ret >> 8;
    _rxbuf[2] = ret >> 16;
    _rxbuf[3] = ret >> 24;
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_RaedBit8Slow
*    功能说明: 读8bit数据，4通道同步， 双线
*    形    参: 无
*    返 回 值: 4通道的数据
*********************************************************************************************************
*/ 
static void W25Q_RaedBit8Slow(uint8_t *_rxbuf)
{
    uint8_t i;
    uint32_t gpio;
    uint32_t ret = 0;
    
    /* SCK下降沿改变数据，上升沿采集数据, 平时SCK = 1 */
    for (i = 0; i < 8; i++)
    {
        W25_SCK_0();
        PIN_DELAY_S();
        ret <<= 1;
        W25_SCK_1();         
        gpio = W25_READ_MISO();
        PIN_DELAY_S();
        
        if (gpio & W25_MISO1_PIN_A)
        {
            ret |= 0x00000001;
        }
        if (gpio & W25_MISO2_PIN_A)
        {
            ret |= 0x00000100;
        }
        if (gpio & W25_MISO3_PIN_A)
        {
            ret |= 0x00010000;
        }
        if (gpio & W25_MISO4_PIN_A)
        {
            ret |= 0x01000000;
        }                      
    }
    _rxbuf[0] = ret;
    _rxbuf[1] = ret >> 8;
    _rxbuf[2] = ret >> 16;
    _rxbuf[3] = ret >> 24;       
}

static void W25Q_RaedBit8SlowDaul(uint8_t *_rxbuf)
{
    uint8_t i;
    uint32_t gpio;
    uint32_t ret = 0;
    
    /* SCK下降沿改变数据，上升沿采集数据, 平时SCK = 1 */
    for (i = 0; i < 4; i++)
    {
        W25_SCK_0();
        PIN_DELAY_S();             
        ret <<= 2;
        W25_SCK_1();
        PIN_DELAY_S();
        gpio = W25_READ_MISO();     
        if (gpio & W25_MISO1_PIN_B)
        {
            ret |= 0x00000001;
        }
        if (gpio & W25_MISO1_PIN_A)
        {
            ret |= 0x00000002;
        }
        
        if (gpio & W25_MISO2_PIN_B)
        {
            ret |= 0x00000100;
        }
        if (gpio & W25_MISO2_PIN_A)
        {
            ret |= 0x00000200;
        }
        
        if (gpio & W25_MISO3_PIN_B)
        {
            ret |= 0x00010000;
        }
        if (gpio & W25_MISO3_PIN_A)
        {
            ret |= 0x00020000;
        }
        
        if (gpio & W25_MISO4_PIN_B)
        {
            ret |= 0x01000000;
        }
        if (gpio & W25_MISO4_PIN_A)
        {
            ret |= 0x02000000;
        }       
    }
    _rxbuf[0] = ret;
    _rxbuf[1] = ret >> 8;
    _rxbuf[2] = ret >> 16;
    _rxbuf[3] = ret >> 24;       
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_SendData
*    功能说明: 发送数据
*    形    参: _cmd : 命令
*              _addr : 地址     
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void W25Q_SendData(uint8_t *_txbuf, uint16_t _txlen)
{
    uint16_t i;
    
    W25_CS_0();
    if (g_tProg.SwdClockDelay == 0)
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Fast(_txbuf[i]);
        }
    }
    else
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Slow(_txbuf[i]);
        }
    }
    W25_CS_1();
}

// 不拉高CS
static void W25Q_SendData0(uint8_t *_txbuf, uint16_t _txlen)
{
    uint16_t i;
    
    W25_CS_0();
    if (g_tProg.SwdClockDelay == 0)
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Fast(_txbuf[i]);
        }
    }
    else
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Slow(_txbuf[i]);
        }
    }
    //W25_CS_1();
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_ReadData
*    功能说明: 读回数据. 4路同步读回
*    形    参:  
*    返 回 值: 无
*********************************************************************************************************
*/
#if 0
static void W25Q_ReadData(uint8_t *_rxbuf, uint16_t _rxlen)
{
    uint16_t i;
    uint8_t buf[4];
    
    W25_CS_0();
    if (g_tProg.SwdClockDelay == 0)
    {
        for (i = 0;i < _rxlen; i++)
        {
            W25Q_RaedBit8Fast(buf);
            
            _rxbuf[4 * i] = buf[0];
            _rxbuf[4 * i + 1] = buf[1];
            _rxbuf[4 * i + 2] = buf[2];
            _rxbuf[4 * i + 3] = buf[3];
        }        
    }
    else
    {
        for (i = 0;i < _rxlen; i++)
        {
            W25Q_RaedBit8Slow(buf);
            
            _rxbuf[4 * i] = buf[0];
            _rxbuf[4 * i + 1] = buf[1];
            _rxbuf[4 * i + 2] = buf[2];
            _rxbuf[4 * i + 3] = buf[3];
        }
    }
    W25_CS_1();
}
#endif

/*
*********************************************************************************************************
*    函 数 名: W25Q_SendAndReadData
*    功能说明: 发送并且读回数据(单线)
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void W25Q_SendAndReadData(uint8_t *_txbuf, uint32_t _txlen, uint8_t *_rxbuf, uint32_t _rxlen)
{
    uint32_t i;
    uint8_t buf[4];    
    
    W25_CS_0();
    if (g_tProg.SwdClockDelay == 0)
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Fast(_txbuf[i]);
        }
        
        for (i = 0; i < _rxlen; i++)
        {
            W25Q_RaedBit8Fast(buf);
            
            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {
                _rxbuf[i] = buf[0];
                _rxbuf[_rxlen + i] = buf[1];
                _rxbuf[2 * _rxlen + i] = buf[2];
                _rxbuf[3 * _rxlen + i] = buf[3];
            }
            else    /* 单路模式 */
            {
                _rxbuf[i] = buf[0];
            }
        }
    }
    else
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Slow(_txbuf[i]);
        }
        
        for (i = 0;i < _rxlen; i++)
        {
            W25Q_RaedBit8Slow(buf);
            
            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {
                _rxbuf[i] = buf[0];
                _rxbuf[_rxlen + i] = buf[1];
                _rxbuf[2 * _rxlen + i] = buf[2];
                _rxbuf[3 * _rxlen + i] = buf[3];
            }
            else
            {
                _rxbuf[i] = buf[0];
            }
        }        
    }
   
    W25_CS_1();
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_SendAndReadDataDaul
*    功能说明: 发送并且读回数据(双线)
*    形    参:  
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void W25Q_SendAndReadDataDaul(uint8_t *_txbuf, uint32_t _txlen, uint8_t *_rxbuf, uint32_t _rxlen)
{
    uint32_t i;
    uint8_t buf[4];    
    
    W25_CS_0();
    if (g_tProg.SwdClockDelay == 0)
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Fast(_txbuf[i]);
        }
        
        /* 切换MOSI 为输入 */
        W25_MOSI_SET_IN();
        
        for (i = 0; i < _rxlen; i++)
        {
            W25Q_RaedBit8FastDaul(buf);
            
            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {
                _rxbuf[i] = buf[0];
                _rxbuf[_rxlen + i] = buf[1];
                _rxbuf[2 * _rxlen + i] = buf[2];
                _rxbuf[3 * _rxlen + i] = buf[3];
            }
            else    /* 单路模式 */
            {
                _rxbuf[i] = buf[0];
            }
        }
        /* 切换MOSI 为输出 */
        W25_MOSI_SET_OUT();         
    }
    else
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Slow(_txbuf[i]);
        }
        
        /* 切换MOSI 为输入 */
        W25_MOSI_SET_IN();
        
        for (i = 0;i < _rxlen; i++)
        {
            W25Q_RaedBit8SlowDaul(buf);
            
            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {
                _rxbuf[i] = buf[0];
                _rxbuf[_rxlen + i] = buf[1];
                _rxbuf[2 * _rxlen + i] = buf[2];
                _rxbuf[3 * _rxlen + i] = buf[3];
            }
            else
            {
                _rxbuf[i] = buf[0];
            }
        }

        /* 切换MOSI 为输出 */
        W25_MOSI_SET_OUT();        
    }
    W25_CS_1();
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_SendAndReadDataBlank
*    功能说明: 发送并且读回数据(单线), 判断是否为空
*    形    参: 
*    返 回 值: 1 表示空片， 0表示不空需要擦除
*********************************************************************************************************
*/ 
static uint8_t W25Q_SendAndReadDataBlank(uint8_t *_txbuf, uint32_t _txlen, uint32_t _rxlen)
{
    uint32_t i;
    uint8_t buf[4];
    uint8_t blank = 1;
    
    W25_CS_0();
    if (g_tProg.SwdClockDelay == 0)
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Fast(_txbuf[i]);
        }
        
        for (i = 0; i < _rxlen; i++)
        {
            W25Q_RaedBit8Fast(buf);
            
            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {
                uint8_t k;
                
                for (k = 0; k < g_gMulSwd.MultiMode; k++)
                {
                    if (buf[k] != 0xFF)
                    {
                        blank = 0;  /* 不空 */
                        break;
                    }
                }
            }
            else    /* 单路模式 */
            {
                if (buf[0] != 0xFF)
                {
                    blank = 0;  /* 不空 */
                    break;                    
                }
            }
        }
    }
    else
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Slow(_txbuf[i]);
        }
        
        for (i = 0;i < _rxlen; i++)
        {
            W25Q_RaedBit8Slow(buf);
            
            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {
                uint8_t k;
                
                for (k = 0; k < g_gMulSwd.MultiMode; k++)
                {
                    if (buf[k] != 0xFF)
                    {
                        blank = 0;  /* 不空 */
                        break;
                    }
                }
            }
            else
            {
                if (buf[0] != 0xFF)
                {
                    blank = 0;  /* 不空 */
                    break;                    
                }
            }
        }        
    }
   
    W25_CS_1();
    
    return blank;
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_SendAndReadDataBlankDaul
*    功能说明: 发送并且读回数据(双线), 判断是否为空
*    形    参:  
*    返 回 值: 无
*********************************************************************************************************
*/ 
static uint8_t W25Q_SendAndReadDataBlankDaul(uint8_t *_txbuf, uint32_t _txlen, uint32_t _rxlen)
{
    uint32_t i;
    uint8_t buf[4];    
    uint8_t blank = 1;
    
    W25_CS_0();
    if (g_tProg.SwdClockDelay == 0)
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Fast(_txbuf[i]);
        }
        
        /* 切换MOSI 为输入 */
        W25_MOSI_SET_IN();
        
        for (i = 0; i < _rxlen; i++)
        {
            W25Q_RaedBit8FastDaul(buf);
            
            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {
                uint8_t k;
                
                for (k = 0; k < g_gMulSwd.MultiMode; k++)
                {
                    if (buf[k] != 0xFF)
                    {
                        blank = 0;  /* 不空 */
                        break;
                    }
                }
            }
            else    /* 单路模式 */
            {
                if (buf[0] != 0xFF)
                {
                    blank = 0;  /* 不空 */
                    break;                    
                }
            }
        }
        /* 切换MOSI 为输出 */
        W25_MOSI_SET_OUT();         
    }
    else
    {
        for (i = 0;i < _txlen; i++)
        {
            W25Q_SendBit8Slow(_txbuf[i]);
        }
        
        /* 切换MOSI 为输入 */
        W25_MOSI_SET_IN();
        
        for (i = 0;i < _rxlen; i++)
        {
            W25Q_RaedBit8SlowDaul(buf);
            
            if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
            {
                uint8_t k;
                
                for (k = 0; k < g_gMulSwd.MultiMode; k++)
                {
                    if (buf[k] != 0xFF)
                    {
                        blank = 0;  /* 不空 */
                        break;
                    }
                }
            }
            else
            {
                if (buf[0] != 0xFF)
                {
                    blank = 0;  /* 不空 */
                    break;                    
                }
            }
        }

        /* 切换MOSI 为输出 */
        W25_MOSI_SET_OUT();        
    }
    W25_CS_1();
    
    return blank;    
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_DetectIC
*    功能说明: 检测IC是否在位
*    形    参: _id 读取4FFC开始的数字，当做芯片识别码
*    返 回 值: 1表示OK  0表示错误
*********************************************************************************************************
*/ 
uint8_t W25Q_DetectIC(uint32_t *_id)
{
    uint8_t txbuf[5];
    uint8_t rxbuf[3 * 4];
    uint8_t i;
    
    if (g_tW25Q.ReadIDCmd == 0x9F)
    {
        txbuf[0] = 0x9F;    
        W25Q_SendAndReadData(txbuf, 1, rxbuf,3);
        
        for (i = 0; i < 4; i++)
        {
            if (rxbuf[4 * i + 0] == 0 || rxbuf[4 * i + 0] == 0xFF)  /* 判断厂家ID */
            {
                _id[i] = 0;     /* 无效ID，做无芯片处理 */
            }
            else
            {        
                _id[i] =    ((rxbuf[3 * i + 0] << 16) & 0x00FF0000) + 
                            ((rxbuf[3 * i + 1] << 8) & 0x0000FF00) + 
                            ((rxbuf[3 * i + 2] << 0) & 0x000000FF);
            }
        }
    }
    else if (g_tW25Q.ReadIDCmd == 0x90 || g_tW25Q.ReadIDCmd == 0xAB)     /* SST */
    {
        txbuf[0] = g_tW25Q.ReadIDCmd; 
        txbuf[1] = 0;
        txbuf[2] = 0;
        txbuf[3] = 0;       
        W25Q_SendAndReadData(txbuf, 4, rxbuf, 2);
        
        for (i = 0; i < 4; i++)
        {
            if (rxbuf[4 * i + 0] == 0 || rxbuf[4 * i + 0] == 0xFF)  /* 判断厂家ID */
            {
                _id[i] = 0;     /* 无效ID，做无芯片处理 */
            }
            else
            {        
                _id[i] =    ((rxbuf[2 * i + 0] << 8) & 0x00FF00) + 
                            ((rxbuf[2 * i + 1] << 0) & 0x0000FF);       
            }
        }            
    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_WriteBuf
*    功能说明: 连续写多个字节.  不支持的操作。没有内存或寄存器可以操作
*    形    参: _Addr : 3个字节的地址。整数值
*              _Buf : 输入数据缓冲区
*              _Len : 字节长度
*    返 回 值: 1表示OK, 0表示出错
*********************************************************************************************************
*/ 
uint8_t W25Q_WriteBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len)
{
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_CheckBlank
*    功能说明: 读取连续多个字节数据. APROM。  LDROM的数据地址 + 0x10000
*    形    参: _Addr : 3个字节的地址。整数值
*              _Len : 字节长度
*    返 回 值: 1表示空片，0表示不空
*********************************************************************************************************
*/ 
uint8_t W25Q_CheckBlank(uint32_t _Addr, uint32_t _Len)
{
    uint8_t txbuf[5];
    uint8_t re;

    if (g_tW25Q.Capacity > 16 * 1024 * 1024)
    {
        /* 进入四字节地址模式 */
        txbuf[0] =  0xB7;
        W25Q_SendData(txbuf, 1);
    }
        
    if (g_tW25Q.ReadMode == 0)  /* 单线 */
    {
        if (_Addr > 16 * 1024 * 1024)
        {
            txbuf[0] = 0x13;    /* 4字节地址，读 */
            txbuf[1] = _Addr >> 24;
            txbuf[2] = _Addr >> 16;
            txbuf[3] = _Addr >> 8;
            txbuf[4] = _Addr;                
            
            re = W25Q_SendAndReadDataBlank(txbuf, 5, _Len);    /* 查空 */
        }
        else    
        {
            txbuf[0] = 0x03;    /* 3字节地址，读 */
            txbuf[1] = _Addr >> 16;
            txbuf[2] = _Addr >> 8;
            txbuf[3] = _Addr;        
            re = W25Q_SendAndReadDataBlank(txbuf, 4, _Len);        
        }
    }
    else if (g_tW25Q.ReadMode == 1)  /* 双线 */
    {
        if (_Addr > 16 * 1024 * 1024)
        {
            txbuf[0] = 0x3C;    /* 4字节地址，读 Read daul output */
            txbuf[1] = _Addr >> 24;
            txbuf[2] = _Addr >> 16;
            txbuf[3] = _Addr >> 8;
            txbuf[4] = _Addr;                
            
            re = W25Q_SendAndReadDataBlankDaul(txbuf, 5, _Len);
        }
        else    
        {
            txbuf[0] = 0x3B;    /* 3字节地址，读 Read daul output */
            txbuf[1] = _Addr >> 16;
            txbuf[2] = _Addr >> 8;
            txbuf[3] = _Addr;        
            re = W25Q_SendAndReadDataBlankDaul(txbuf, 4, _Len);        
        }
    }

    return re;
}

/*
*********************************************************************************************************
*	函 数 名: W25Q_WriteEnable
*	功能说明: 向器件发送写使能命令
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25Q_WriteEnable(void)
{
    uint8_t txbuf[1];
    
    txbuf[0] = CMD_WREN;
    W25Q_SendData(txbuf, 1); 
}

/*
*********************************************************************************************************
*	函 数 名: W25Q_WriteStatusEnable
*	功能说明: 向器件发送写状态字使能命令
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25Q_WriteStatusEnable(void)
{
    uint8_t txbuf[1];
    
    txbuf[0] = CMD_EWRSR;
    W25Q_SendData(txbuf, 1); 
}

/*
*********************************************************************************************************
*	函 数 名: W25Q_WriteDisable
*	功能说明: 向器件发送写禁止命令
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25Q_WriteDisable(void)
{
    uint8_t txbuf[1];
    
    txbuf[0] = CMD_DISWR;
    W25Q_SendData(txbuf, 1); 
}


/*
*********************************************************************************************************
*	函 数 名: W25Q_WriteStatus
*	功能说明: 向器件发送写状态字指令
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void W25Q_WriteStatus(uint8_t _value)
{
    uint8_t txbuf[2];
    
    txbuf[0] = CMD_WRSR;
    txbuf[1] = _value;
    W25Q_SendData(txbuf, 2); 
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_UnlockBlock
*    功能说明: 解锁写保护。 在擦除全片和扇区时执行
*    形    参: _addr 地址
*    返 回 值: 无
*********************************************************************************************************
*/ 
void W25Q_UnlockBlock(void)
{
    if (g_tW25Q.UnlockCmd > 0x00)   // SST26 是0x98指令全局解锁保护
    {
        uint8_t txbuf[1];
        
        W25Q_WriteEnable();
        
        txbuf[0] = g_tW25Q.UnlockCmd;
        W25Q_SendData(txbuf, 1); 
    }
    else  // 0x00 表示写状态寄存器00
    {          
        if (g_tW25Q.AutoAddrInc > 0)   // SST25 特殊，缺省是保护，未启用AAI
        {
            W25Q_WriteEnable();
            
            W25Q_WaitBusy(200);
            
            W25Q_WriteStatusEnable();   
            
            W25Q_WriteStatus(0x00);			/* 解除所有BLOCK的写保护 */
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_WaitBusy
*    功能说明: 等待操作结束
*    形    参: _addr 地址
*    返 回 值: 1表示OK, 0表示出错
*********************************************************************************************************
*/ 
uint8_t W25Q_WaitBusy(uint32_t _timeout)
{
    int32_t time1;
    uint8_t txbuf[1];
    uint8_t rxbuf[4];   /* 4路的状态字 */
    uint8_t i;
    int32_t timedisp;
    uint8_t ch_num;
    
    time1 = bsp_GetRunTime();
    timedisp = bsp_GetRunTime();
    txbuf[0] = (CMD_RDSR);	        /* 发送命令， 读状态寄存器 */ 
    
    if ( g_gMulSwd.MultiMode == 0)
    {
        ch_num = 1;
    }
    else
    {
        ch_num = g_gMulSwd.MultiMode;        /* 多路烧录 */
    }
    
	while(1)
	{
        if (bsp_CheckRunTime(time1) > _timeout + 5000)
        {
            printf("W25Q_WaitBusy(%d) is timeout", _timeout);
            break;   /* 超时 */
        }
                
        if (bsp_CheckRunTime(timedisp) > 500)
        {
            timedisp = bsp_GetRunTime();

            {
                float percent;
                int32_t tt;
                
                tt = bsp_CheckRunTime(time1);
                percent = ((float)tt / _timeout) * 100;
                PG_PrintPercent(percent, 0);
                bsp_Idle();                  
            }          
        }
        
        W25Q_SendAndReadData(txbuf, 1, rxbuf, 1);        
		
        {
            uint8_t err = 0;
            
            if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
            {
                for (i = 0; i < ch_num; i++)
                {
                    if (g_gMulSwd.Active[i] == 1)
                    {
                        if ((rxbuf[i] & WIP_FLAG) != 0)	    
                        {
                            err = 1;
                        }
                    }
                }
                
                if (err == 0)
                {
                    return 1;   /* OK */
                }
            }
            else
            {
                for (i = 0; i < ch_num; i++)
                {
                    if (g_gMulSwd.Active[i] == 1 && g_gMulSwd.Error[i] == 0)
                    {
                        if ((rxbuf[i] & WIP_FLAG) != 0)	    
                        {
                            err = 1;
                        }
                    }
                }
                
                if (err == 0)
                {
                    return 1;   /* OK */
                }
            }
        }        	
	}
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_EraseSector
*    功能说明: 擦除扇区, 一般4K,  先检查是否为空
*    形    参: _addr 地址
*    返 回 值: 1表示OK, 0表示出错
*********************************************************************************************************
*/ 
uint8_t W25Q_EraseSector(uint32_t _Addr)
{
    uint8_t buf[5];    
    
    
    /* 先判断是否为空，再擦除 */
    if (W25Q_CheckBlank(_Addr, g_tW25Q.SectorSize) == 1)
    {
        return 1;
    }        
    
    W25Q_WriteEnable();
    
    buf[0] = g_tW25Q.EraseSectorCmd;    
    if (g_tW25Q.Capacity > 16 * 1024 * 1024)
    {
        buf[1] = _Addr >> 24;
        buf[2] = _Addr >> 16;
        buf[3] = _Addr >> 8;
        buf[4] = _Addr >> 0;
        W25Q_SendData(buf, 5);
    }
    else    /* 3字节模式 */
    {
        buf[1] = _Addr >> 16;
        buf[2] = _Addr >> 8;
        buf[3] = _Addr >> 0;
        W25Q_SendData(buf, 4);    
    }
    
    return W25Q_WaitBusy(g_tW25Q.EraseSectorTimeout);
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_EraseSector
*    功能说明: 擦除整片
*    形    参: 无
*    返 回 值: 1表示OK, 0表示出错
*********************************************************************************************************
*/ 
uint8_t W25Q_EraseChip(void)
{
    uint8_t buf[4];    
    
    W25Q_WriteEnable();
    
    buf[0] = g_tW25Q.EraseChipCmd;    /* 整片擦除指令 */
    W25Q_SendData(buf, 1);
    
    return W25Q_WaitBusy(g_tW25Q.EraseChipTimeout);
}

/*
*********************************************************************************************************
*    函 数 名: W25Q_FLASH_ProgramBuf
*    功能说明: Programs a memory block。  页大小256字节
*    形    参: _FlashAddr : 绝对地址。 
*              _Buff : Pointer to buffer containing source data.
*              _Size : 数据大小，可以大于1个block
*    返 回 值: 0 : 出错;  1 : 成功
*********************************************************************************************************
*/ 
uint8_t W25Q_FLASH_ProgramBuf(uint32_t _Addr, uint8_t *_Buff, uint32_t _Size)
{
    if (g_tW25Q.AutoAddrInc == 0)   /* 256 page 写入模式 */
    {
        uint8_t txbuf[5];
        uint32_t i;
        
        if (g_tW25Q.Capacity > 16 * 1024 * 1024)
        {
            /* 进入四字节地址模式 */
            txbuf[0] =  0xB7;
            W25Q_SendData(txbuf, 1);
        }
        
        for (i = 0; i < _Size / 256; i++)
        {
            W25Q_WriteEnable();
            
            txbuf[0] = CMD_PAGE_PROG;       
            if (g_tW25Q.Capacity > 16 * 1024 * 1024)
            {
                txbuf[1] = _Addr >> 24;
                txbuf[2] = _Addr >> 16;
                txbuf[3] = _Addr >> 8;
                txbuf[4] = _Addr >> 0;
                W25Q_SendData0(txbuf, 5);   /* 发送完毕不拉高CS */
            }
            else    /* 3字节模式 */
            {
                txbuf[1] = _Addr >> 16;
                txbuf[2] = _Addr >> 8;
                txbuf[3] = _Addr >> 0; 
                W25Q_SendData0(txbuf, 4);  /* 发送完毕不拉高CS */
            }
            
            W25Q_SendData(_Buff, 256);     /* 发送完毕拉高CS */
            
            if (W25Q_WaitBusy(g_tW25Q.ProgPageTimeout) == 0)
            {
                return 0;   /* 超时异常 */
            }
            
            _Addr += 256;
            _Buff += 256;
        }
    }
    else    /* 地址递增模式  SST25VF020 以下单字节  040以上双字节 */
    {
        uint8_t txbuf[6];
        uint32_t i;
        
        W25Q_WriteEnable();
        
        if (g_tW25Q.AutoAddrInc == 0xAF)    /* 单字节AAI */
        {
            txbuf[0] = g_tW25Q.AutoAddrInc;
            txbuf[1] = _Addr >> 16;
            txbuf[2] = _Addr >> 8;
            txbuf[3] = _Addr >> 0; 
            txbuf[4] = *_Buff++;
            W25Q_SendData(txbuf, 5);     /* 发送完毕拉高CS */

            if (W25Q_WaitBusy(g_tW25Q.ProgPageTimeout) == 0)
            {
                return 0;   /* 超时异常 */
            }
            
            for (i = 0; i < (_Size - 1) / 1; i++)
            {
                txbuf[0] = g_tW25Q.AutoAddrInc;
                txbuf[1] = *_Buff++;
                W25Q_SendData(txbuf, 2);     /* 发送完毕拉高CS */
                
                if (W25Q_WaitBusy(g_tW25Q.ProgPageTimeout) == 0)
                {
                    return 0;   /* 超时异常 */
                }
            }
        }
        else    /* 0xAD, 双字节AAI */
        {
            txbuf[0] = g_tW25Q.AutoAddrInc;
            txbuf[1] = _Addr >> 16;
            txbuf[2] = _Addr >> 8;
            txbuf[3] = _Addr >> 0; 
            txbuf[4] = *_Buff++;
            txbuf[5] = *_Buff++;
            W25Q_SendData(txbuf, 6);     /* 发送完毕拉高CS */

            if (W25Q_WaitBusy(g_tW25Q.ProgPageTimeout) == 0)
            {
                return 0;   /* 超时异常 */
            }
            
            for (i = 0; i < (_Size - 2) / 2; i++)
            {
                txbuf[0] = g_tW25Q.AutoAddrInc;
                txbuf[1] = *_Buff++;
                txbuf[2] = *_Buff++;
                W25Q_SendData(txbuf, 3);     /* 发送完毕拉高CS */
                
                if (W25Q_WaitBusy(g_tW25Q.ProgPageTimeout) == 0)
                {
                    return 0;   /* 超时异常 */
                }
            }            
        }
        
        // 退出AAI
        W25Q_WriteDisable();

        if (W25Q_WaitBusy(g_tW25Q.ProgPageTimeout) == 0)
        {
            return 0;   /* 超时异常 */
        }        
    }
    return 1;
}    


/*
*********************************************************************************************************
*    函 数 名: W25Q_ReadBuf
*    功能说明: 读取连续多个字节数据. APROM。  LDROM的数据地址 + 0x10000
*    形    参: _Addr : 3个字节的地址。整数值
*              _Buf : 目标缓冲区
*              _Len : 字节长度
*    返 回 值: 1表示成功，0表示失败
*********************************************************************************************************
*/ 
uint8_t W25Q_ReadBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len)
{
    uint8_t txbuf[6];

    if (g_tW25Q.Capacity > 16 * 1024 * 1024)
    {
        /* 进入四字节地址模式 */
        txbuf[0] =  0xB7;
        W25Q_SendData(txbuf, 1);
    }
    
    if (g_tW25Q.ReadMode == 0)  /* 单线 */
    {
        if (g_tW25Q.Capacity > 16 * 1024 * 1024)
        {
            txbuf[0] = 0x03;    /* 4字节地址，读 */
            txbuf[1] = _Addr >> 24;
            txbuf[2] = _Addr >> 16;
            txbuf[3] = _Addr >> 8;
            txbuf[4] = _Addr;                
            
            W25Q_SendAndReadData(txbuf, 5, _Buf, _Len);
        }
        else    
        {
            txbuf[0] = 0x03;    /* 3字节地址，读 */
            txbuf[1] = _Addr >> 16;
            txbuf[2] = _Addr >> 8;
            txbuf[3] = _Addr;        
            W25Q_SendAndReadData(txbuf, 4, _Buf, _Len);        
        }
    }
    else if (g_tW25Q.ReadMode == 1)  /* 双线 */
    {
        if (g_tW25Q.Capacity > 16 * 1024 * 1024)
        {
            txbuf[0] = 0x3B;    /* 4字节地址，读 Read daul output */
            txbuf[1] = _Addr >> 24;
            txbuf[2] = _Addr >> 16;
            txbuf[3] = _Addr >> 8;
            txbuf[4] = _Addr;                
            txbuf[5] = 0;      /* 空闲字节 */  
            W25Q_SendAndReadDataDaul(txbuf, 6, _Buf, _Len);
        }
        else    
        {
            txbuf[0] = 0x3B;    /* 3字节地址，读 Read daul output */
            txbuf[1] = _Addr >> 16;
            txbuf[2] = _Addr >> 8;
            txbuf[3] = _Addr;
            txbuf[4] = 0;      /* 空闲字节 */      
            W25Q_SendAndReadDataDaul(txbuf, 5, _Buf, _Len);        
        }
    }

    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
