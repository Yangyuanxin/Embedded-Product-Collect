/*
*********************************************************************************************************
*
*    模块名称 : W25Q256 QSPI驱动模块
*    文件名称 : bsp_qspi_w25q256.c
*    版    本 : V1.0
*    说    明 : 使用CPU的QSPI总线驱动串行FLASH，提供基本的读写函数，采用4线方式，MDMA传输
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-02-12  armfly  正式发布
*        V1.1    2019-07-07  armfly  增加H7-TOOL引脚定义。
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "nvic_prio_cfg.h"

/* 
    STM32-V7开发板接线

    PG6/QUADSPI_BK1_NCS     AF10
    PF10/QUADSPI_CLK        AF9
    PF8/QUADSPI_BK1_IO0     AF10
    PF9/QUADSPI_BK1_IO1     AF10
    PF7/QUADSPI_BK1_IO2     AF9
    PF6/QUADSPI_BK1_IO3     AF9

    W25Q256JV有512块，每块有16个扇区，每个扇区Sector有16页，每页有256字节，共计32MB
        
    H7-TOOL开发板接线

    PG6/QUADSPI_BK1_NCS     AF10
    PB2/QUADSPI_CLK         AF9
    PD11/QUADSPI_BK1_IO0    AF10
    PD12/QUADSPI_BK1_IO1    AF10
    PF7/QUADSPI_BK1_IO2     AF9
    PD13/QUADSPI_BK1_IO3    AF9
*/

/* QSPI引脚和时钟相关配置宏定义 */
#if 1
#define QSPI_CLK_ENABLE()               __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()              __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_CS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOG_CLK_ENABLE()
#define QSPI_CLK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_BK1_D0_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()
#define QSPI_BK1_D1_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()
#define QSPI_BK1_D2_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_BK1_D3_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOD_CLK_ENABLE()

#define QSPI_MDMA_CLK_ENABLE()          __HAL_RCC_MDMA_CLK_ENABLE()
#define QSPI_FORCE_RESET()              __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()            __HAL_RCC_QSPI_RELEASE_RESET()

#define QSPI_CS_PIN                     GPIO_PIN_6
#define QSPI_CS_GPIO_PORT               GPIOG
#define QSPI_CS_GPIO_AF                 GPIO_AF10_QUADSPI

#define QSPI_CLK_PIN                    GPIO_PIN_2
#define QSPI_CLK_GPIO_PORT              GPIOB
#define QSPI_CLK_GPIO_AF                GPIO_AF9_QUADSPI

#define QSPI_BK1_D0_PIN                 GPIO_PIN_11
#define QSPI_BK1_D0_GPIO_PORT           GPIOD
#define QSPI_BK1_D0_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_BK1_D1_PIN                 GPIO_PIN_12
#define QSPI_BK1_D1_GPIO_PORT           GPIOD
#define QSPI_BK1_D1_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_BK1_D2_PIN                 GPIO_PIN_7
#define QSPI_BK1_D2_GPIO_PORT           GPIOF
#define QSPI_BK1_D2_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_BK1_D3_PIN                 GPIO_PIN_13
#define QSPI_BK1_D3_GPIO_PORT           GPIOD
#define QSPI_BK1_D3_GPIO_AF             GPIO_AF9_QUADSPI
#else
#define QSPI_CLK_ENABLE()               __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()              __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_CS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOG_CLK_ENABLE()
#define QSPI_CLK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_BK1_D0_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_BK1_D1_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_BK1_D2_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOF_CLK_ENABLE()
#define QSPI_BK1_D3_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOF_CLK_ENABLE()

#define QSPI_MDMA_CLK_ENABLE()          __HAL_RCC_MDMA_CLK_ENABLE()
#define QSPI_FORCE_RESET()              __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()            __HAL_RCC_QSPI_RELEASE_RESET()

#define QSPI_CS_PIN                     GPIO_PIN_6
#define QSPI_CS_GPIO_PORT               GPIOG
#define QSPI_CS_GPIO_AF                 GPIO_AF10_QUADSPI

#define QSPI_CLK_PIN                    GPIO_PIN_10
#define QSPI_CLK_GPIO_PORT              GPIOF
#define QSPI_CLK_GPIO_AF                GPIO_AF9_QUADSPI

#define QSPI_BK1_D0_PIN                 GPIO_PIN_8
#define QSPI_BK1_D0_GPIO_PORT           GPIOF
#define QSPI_BK1_D0_GPIO_AF             GPIO_AF10_QUADSPI

#define QSPI_BK1_D1_PIN                 GPIO_PIN_9
#define QSPI_BK1_D1_GPIO_PORT           GPIOF
#define QSPI_BK1_D1_GPIO_AF             GPIO_AF10_QUADSPI

#define QSPI_BK1_D2_PIN                 GPIO_PIN_7
#define QSPI_BK1_D2_GPIO_PORT           GPIOF
#define QSPI_BK1_D2_GPIO_AF             GPIO_AF9_QUADSPI

#define QSPI_BK1_D3_PIN                 GPIO_PIN_6
#define QSPI_BK1_D3_GPIO_PORT           GPIOF
#define QSPI_BK1_D3_GPIO_AF             GPIO_AF9_QUADSPI
#endif

/* 供本文件调用的全局变量 */
static MDMA_HandleTypeDef hmdma;
static QSPI_HandleTypeDef QSPIHandle;
static __IO uint8_t CmdCplt, RxCplt, TxCplt, StatusMatch, TimeOut;

/* 供本文件调用的函数 */
static void QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
static void QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi);

//static void QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi);

uint8_t QSPI_MemoryMapped(void);

/*
*********************************************************************************************************
*    函 数 名: bsp_InitQSPI_W25Q256
*    功能说明: QSPI Flash硬件初始化，配置基本参数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitQSPI_W25Q256(void)
{
    /* 复位QSPI */
    QSPIHandle.Instance = QUADSPI;
    if (HAL_QSPI_DeInit(&QSPIHandle) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 设置时钟速度，QSPI clock = 200MHz / (ClockPrescaler+1) = 100MHz */
    QSPIHandle.Init.ClockPrescaler = 1;

    /* 设置FIFO阀值，范围1 - 32 */
    QSPIHandle.Init.FifoThreshold = 32;

    /* 
        QUADSPI在FLASH驱动信号后过半个CLK周期才对FLASH驱动的数据采样。
        在外部信号延迟时，这有利于推迟数据采样。
    */
    QSPIHandle.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;

    /* Flash大小是2^(FlashSize + 1) = 2^25 = 32MB */
    /* 正确值应该是 QSPI_FLASH_SIZE - 1; 但内存映射模式访问最后一个字节时，CPU预取下个数据会导致异常。扩大一点解决问题。 */
    QSPIHandle.Init.FlashSize = QSPI_FLASH_SIZE;    

    /* 命令之间的CS片选至少保持1个时钟周期的高电平 */
    QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;

    /*
       MODE0: 表示片选信号空闲期间，CLK时钟信号是低电平
       MODE3: 表示片选信号空闲期间，CLK时钟信号是高电平
    */
    QSPIHandle.Init.ClockMode = QSPI_CLOCK_MODE_0;

    /* QSPI有两个BANK，这里使用的BANK1 */
    QSPIHandle.Init.FlashID = QSPI_FLASH_ID_1;

    /* V7开发板仅使用了BANK1，这里是禁止双BANK */
    QSPIHandle.Init.DualFlash = QSPI_DUALFLASH_DISABLE;

    /* 初始化配置QSPI */
    if (HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
	
	// QSPI_MemoryMapped();  内存映射，没有调通。读字库数据错乱
}

/*
*********************************************************************************************************
*    函 数 名: HAL_QSPI_MspInit
*    功能说明: QSPI底层初始化，被HAL_QSPI_Init调用的底层函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 使能QPSI时钟  */
    QSPI_CLK_ENABLE();

    /* 复位时钟接口 */
    QSPI_FORCE_RESET();
    QSPI_RELEASE_RESET();

    /* 使能GPIO时钟 */
    QSPI_CS_GPIO_CLK_ENABLE();
    QSPI_CLK_GPIO_CLK_ENABLE();
    QSPI_BK1_D0_GPIO_CLK_ENABLE();
    QSPI_BK1_D1_GPIO_CLK_ENABLE();
    QSPI_BK1_D2_GPIO_CLK_ENABLE();
    QSPI_BK1_D3_GPIO_CLK_ENABLE();

    /* QSPI CS GPIO 引脚配置 */
    GPIO_InitStruct.Pin = QSPI_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO_SPEED_FREQ_HIGH  GPIO_SPEED_FREQ_VERY_HIGH */
    GPIO_InitStruct.Alternate = QSPI_CS_GPIO_AF;
    HAL_GPIO_Init(QSPI_CS_GPIO_PORT, &GPIO_InitStruct);

    /* QSPI CLK GPIO 引脚配置 */
    GPIO_InitStruct.Pin = QSPI_CLK_PIN;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = QSPI_CLK_GPIO_AF;
    HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &GPIO_InitStruct);

    /* QSPI D0 GPIO pin 引脚配置 */
    GPIO_InitStruct.Pin = QSPI_BK1_D0_PIN;
    GPIO_InitStruct.Alternate = QSPI_BK1_D0_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_D0_GPIO_PORT, &GPIO_InitStruct);

    /* QSPI D1 GPIO 引脚配置 */
    GPIO_InitStruct.Pin = QSPI_BK1_D1_PIN;
    GPIO_InitStruct.Alternate = QSPI_BK1_D1_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_D1_GPIO_PORT, &GPIO_InitStruct);

    /* QSPI D2 GPIO 引脚配置 */
    GPIO_InitStruct.Pin = QSPI_BK1_D2_PIN;
    GPIO_InitStruct.Alternate = QSPI_BK1_D2_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_D2_GPIO_PORT, &GPIO_InitStruct);

    /* QSPI D3 GPIO 引脚配置 */
    GPIO_InitStruct.Pin = QSPI_BK1_D3_PIN;
    GPIO_InitStruct.Alternate = QSPI_BK1_D3_GPIO_AF;
    HAL_GPIO_Init(QSPI_BK1_D3_GPIO_PORT, &GPIO_InitStruct);

    /* 使能QSPI中断 */
    HAL_NVIC_SetPriority(QUADSPI_IRQn, QSPI_IRQ_PRIO, 0); /* 0x0F --> 0x04， 要高于USB中断 */
    HAL_NVIC_EnableIRQ(QUADSPI_IRQn);

    /* 配置MDMA时钟 */
    QSPI_MDMA_CLK_ENABLE();

    hmdma.Instance = MDMA_Channel1;                            		/* 使用MDMA的通道1 */
    hmdma.Init.Request = MDMA_REQUEST_QUADSPI_FIFO_TH;             	/* QSPI的FIFO阀值触发中断 */
    hmdma.Init.TransferTriggerMode = MDMA_BUFFER_TRANSFER;     		/* 使用MDMA的buffer传输 */
    hmdma.Init.Priority = MDMA_PRIORITY_HIGH;            			/* 优先级高 */
    hmdma.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE; 		/* 小端格式 */
    hmdma.Init.SourceInc = MDMA_SRC_INC_BYTE;            			/* 源地址字节递增 */
    hmdma.Init.DestinationInc = MDMA_DEST_INC_DISABLE;             	/* 目的地址无效自增 */
    hmdma.Init.SourceDataSize = MDMA_SRC_DATASIZE_BYTE;             /* 源地址数据宽度字节 */
    hmdma.Init.DestDataSize = MDMA_DEST_DATASIZE_BYTE;             	/* 目的地址数据宽度字节 */
    hmdma.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;        	/* 小端，右对齐 */
    hmdma.Init.BufferTransferLength = 128;                 			/* 每次传输128个字节 */
    hmdma.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;             	/* 源数据单次传输 */
    hmdma.Init.DestBurst = MDMA_DEST_BURST_SINGLE;                     /* 目的数据单次传输 */

    hmdma.Init.SourceBlockAddressOffset = 0; 	/* 用于block传输，buffer传输用不到 */
    hmdma.Init.DestBlockAddressOffset = 0;     	/* 用于block传输，buffer传输用不到 */

    /* 关联MDMA句柄到QSPI句柄里面  */
    __HAL_LINKDMA(hqspi, hmdma, hmdma);

    /* 先复位，然后配置MDMA */
    HAL_MDMA_DeInit(&hmdma);
    HAL_MDMA_Init(&hmdma);

    /* 使能MDMA中断，并配置优先级 */
    HAL_NVIC_SetPriority(MDMA_IRQn, QSPI_MDMA_IRQ_PRIO, 0);
    HAL_NVIC_EnableIRQ(MDMA_IRQn);
}

/*
*********************************************************************************************************
*    函 数 名: HAL_QSPI_MspDeInit
*    功能说明: QSPI底层复位，被HAL_QSPI_Init调用的底层函数
*    形    参: hqspi QSPI_HandleTypeDef类型句柄
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi)
{
    /* 复位QSPI引脚 */
    HAL_GPIO_DeInit(QSPI_CS_GPIO_PORT, QSPI_CS_PIN);
    HAL_GPIO_DeInit(QSPI_CLK_GPIO_PORT, QSPI_CLK_PIN);
    HAL_GPIO_DeInit(QSPI_BK1_D0_GPIO_PORT, QSPI_BK1_D0_PIN);
    HAL_GPIO_DeInit(QSPI_BK1_D1_GPIO_PORT, QSPI_BK1_D1_PIN);
    HAL_GPIO_DeInit(QSPI_BK1_D2_GPIO_PORT, QSPI_BK1_D2_PIN);
    HAL_GPIO_DeInit(QSPI_BK1_D3_GPIO_PORT, QSPI_BK1_D3_PIN);

    /* 复位QSPI */
    QSPI_FORCE_RESET();
    QSPI_RELEASE_RESET();

    /* 关闭QSPI时钟 */
    QSPI_CLK_DISABLE();
}

/*
*********************************************************************************************************
*    函 数 名: sf_EraseSector
*    功能说明: 擦除指定的扇区，扇区大小4KB
*    形    参: _uiSectorAddr : 扇区地址，以4KB为单位的地址，比如0，4096, 8192等，
*    返 回 值: 无
*********************************************************************************************************
*/
void QSPI_EraseSector(uint32_t address)
{
    QSPI_CommandTypeDef sCommand = {0};

    /* 用于命令发送完成标志 */
    CmdCplt = 0;

    /* 写使能 */
    QSPI_WriteEnable(&QSPIHandle);

    /* 基本配置 */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;         /* 1线方式发送指令 */
    sCommand.AddressSize = QSPI_ADDRESS_32_BITS;                /* 32位地址 */
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;     /* 无交替字节 */
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;                   /* W25Q256JV不支持DDR */
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;      /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;               /* 每次传输都发指令 */

    /* 擦除配置 */
    sCommand.Instruction = SUBSECTOR_ERASE_4_BYTE_ADDR_CMD;     /* 32bit地址方式的扇区擦除命令，扇区大小4KB*/
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;                 /* 地址发送是1线方式 */
    sCommand.Address = address;                                 /* 扇区首地址，保证是4KB整数倍 */
    sCommand.DataMode = QSPI_DATA_NONE;                         /* 无需发送数据 */
    sCommand.DummyCycles = 0;                                   /* 无需空周期 */

    if (HAL_QSPI_Command_IT(&QSPIHandle, &sCommand) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 等待命令发送完毕 */
    while (CmdCplt == 0)
        ;
    CmdCplt = 0;

    /* 等待编程结束 */
    StatusMatch = 0;
    QSPI_AutoPollingMemReady(&QSPIHandle);
    while (StatusMatch == 0)
        ;
    StatusMatch = 0;
}

/*
*********************************************************************************************************
*    函 数 名: QSPI_WriteBuffer
*    功能说明: 页编程，页大小256字节，任意页都可以写入
*    形    参: _pBuf : 数据源缓冲区；
*              _uiWrAddr ：目标区域首地址，即页首地址，比如0， 256, 512等。
*              _usSize ：数据个数，不能超过页面大小，范围1 - 256。
*    返 回 值: 1:成功， 0：失败
*********************************************************************************************************
*/
uint8_t QSPI_WriteBuffer(uint8_t *_pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize)
{
    QSPI_CommandTypeDef sCommand = {0};

    /* 用于等待发送完成标志 */
    TxCplt = 0;

    /* 写使能 */
    QSPI_WriteEnable(&QSPIHandle);

    /* 基本配置 */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;         /* 1线方式发送指令 */
    sCommand.AddressSize = QSPI_ADDRESS_32_BITS;                /* 32位地址 */
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;     /* 无交替字节 */
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;                   /* W25Q256JV不支持DDR */
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;      /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode = QSPI_SIOO_INST_ONLY_FIRST_CMD;          /* 仅发送一次命令 */

    /* 写序列配置 */
    sCommand.Instruction = QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD;   /* 32bit地址的4线快速写入命令 */
    sCommand.DummyCycles = 0;                                   /* 不需要空周期 */
    sCommand.AddressMode = QSPI_ADDRESS_1_LINE;                 /* 4线地址方式 */
    sCommand.DataMode = QSPI_DATA_4_LINES;                      /* 4线数据方式 */
    sCommand.NbData = _usWriteSize;                             /* 写数据大小 */
    sCommand.Address = _uiWriteAddr;                            /* 写入地址 */

    if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 启动MDMA传输 */
    if (HAL_QSPI_Transmit_DMA(&QSPIHandle, _pBuf) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 等待数据发送完毕 */
    while (TxCplt == 0)
        ;
    TxCplt = 0;

    /* 等待Flash页编程完毕 */
    StatusMatch = 0;
    QSPI_AutoPollingMemReady(&QSPIHandle);
    while (StatusMatch == 0)
        ;
    StatusMatch = 0;

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: QSPI_ReadBuffer
*    功能说明: 连续读取若干字节，字节个数不能超出芯片容量。
*    形    参: _pBuf : 数据源缓冲区。
*              _uiReadAddr ：起始地址。
*              _usSize ：数据个数, 可以大于PAGE_SIZE, 但是不能超出芯片总容量。
*    返 回 值: 无
*********************************************************************************************************
*/
void QSPI_ReadBuffer(uint8_t *_pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{

    QSPI_CommandTypeDef sCommand = {0};

    /* 用于等待接收完成标志 */
    RxCplt = 0;

    /* 基本配置 */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;             /* 1线方式发送指令 */
    sCommand.AddressSize = QSPI_ADDRESS_32_BITS;                    /* 32位地址 */
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;         /* 无交替字节 */
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;                       /* W25Q256JV不支持DDR */
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;          /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;                   /* 每次传输要发指令 */

    /* 读取数据 */
    sCommand.Instruction = QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD;    /* 32bit地址的4线快速读取命令 */
    sCommand.DummyCycles = 6;                                       /* 空周期 */
    sCommand.AddressMode = QSPI_ADDRESS_4_LINES;                    /* 4线地址 */
    sCommand.DataMode = QSPI_DATA_4_LINES;                          /* 4线数据 */
    sCommand.NbData = _uiSize;                                      /* 读取的数据大小 */
    sCommand.Address = _uiReadAddr;                                 /* 读取数据的起始地址 */

    if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* MDMA方式读取 */
    if (HAL_QSPI_Receive_DMA(&QSPIHandle, _pBuf) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* 等接受完毕 */
    //while (RxCplt == 0)
    {
        int32_t s_time;
            
        s_time = bsp_GetRunTime();

        while (1)
        {
            if (RxCplt != 0)
            {
                break;
            }
    
            if (bsp_CheckRunTime(s_time) >= 100)
            {
                break;
            }
        }
    }
        ;
    RxCplt = 0;
}

/*
*********************************************************************************************************
*    函 数 名: QSPI_WriteEnable
*    功能说明: 写使能
*    形    参: hqspi  QSPI_HandleTypeDef句柄。
*    返 回 值: 无
*********************************************************************************************************
*/
static void QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef sCommand = {0};

    /* 基本配置 */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;             /* 1线方式发送指令 */
    sCommand.AddressSize = QSPI_ADDRESS_32_BITS;                    /* 32位地址 */
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;         /* 无交替字节 */
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;                       /* W25Q256JV不支持DDR */
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;          /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;                   /* 每次传输都发指令 */

    /* 写使能 */
    sCommand.Instruction = WRITE_ENABLE_CMD;                        /* 写使能指令 */
    sCommand.AddressMode = QSPI_ADDRESS_NONE;                       /* 无需地址 */
    sCommand.DataMode = QSPI_DATA_NONE;                             /* 无需数据 */
    sCommand.DummyCycles = 0;                                       /* 空周期  */

    if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    //    /* 等待写使能完成 */
    //    StatusMatch = 0;
    //    QSPI_AutoPollingMemReady(&QSPIHandle);
    //    while(StatusMatch == 0);
    //    StatusMatch = 0;
}

/*
*********************************************************************************************************
*    函 数 名: QSPI_AutoPollingMemReady
*    功能说明: 等待QSPI Flash就绪，主要用于Flash擦除和页编程时使用
*    形    参: hqspi  QSPI_HandleTypeDef句柄
*    返 回 值: 无
*********************************************************************************************************
*/
static void QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef sCommand = {0};
    QSPI_AutoPollingTypeDef sConfig = {0};

    /* 基本配置 */
    sCommand.InstructionMode = QSPI_INSTRUCTION_1_LINE;             /* 1线方式发送指令 */
    sCommand.AddressSize = QSPI_ADDRESS_32_BITS;                    /* 32位地址 */
    sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;         /* 无交替字节 */
    sCommand.DdrMode = QSPI_DDR_MODE_DISABLE;                       /* W25Q256JV不支持DDR */
    sCommand.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;          /* DDR模式，数据输出延迟 */
    sCommand.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;                   /* 每次传输都发指令 */

    /* 读取状态*/
    sCommand.Instruction = READ_STATUS_REG_CMD;                     /* 读取状态命令 */
    sCommand.AddressMode = QSPI_ADDRESS_NONE;                       /* 无需地址 */
    sCommand.DataMode = QSPI_DATA_1_LINE;                           /* 1线数据 */
    sCommand.DummyCycles = 0;                                       /* 无需空周期 */

    /* 屏蔽位设置的bit0，匹配位等待bit0为0，即不断查询状态寄存器bit0，等待其为0 */
    sConfig.Mask = 0x01;
    sConfig.Match = 0x00;
    sConfig.MatchMode = QSPI_MATCH_MODE_AND;
    sConfig.StatusBytesSize = 1;
    sConfig.Interval = 0x10;
    sConfig.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_QSPI_AutoPolling_IT(&QSPIHandle, &sCommand, &sConfig) != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
}

/*
*********************************************************************************************************
*    函 数 名: QSPI_ReadID
*    功能说明: 读取器件ID
*    形    参: 无
*    返 回 值: 32bit的器件ID (最高8bit填0，有效ID位数为24bit）
*********************************************************************************************************
*/
uint32_t QSPI_ReadID(void)
{
    uint32_t uiID;
    QSPI_CommandTypeDef s_command = {0};
    uint8_t buf[3];

    /* 基本配置 */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;            /* 1线方式发送指令 */
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;                   /* 32位地址 */
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;        /* 无交替字节 */
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;                      /* W25Q256JV不支持DDR */
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;         /* DDR模式，数据输出延迟 */
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;                  /* 每次传输都发指令 */

    /* 读取JEDEC ID */
    s_command.Instruction = READ_ID_CMD2;                           /* 读取ID命令 */
    s_command.AddressMode = QSPI_ADDRESS_NONE;                      /* 1线地址 */
    s_command.DataMode = QSPI_DATA_1_LINE;                          /* 1线地址 */
    s_command.DummyCycles = 0;                                      /* 无空周期 */
    s_command.NbData = 3;                                           /* 读取三个数据 */

    if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
         Error_Handler(__FILE__, __LINE__);;
    }

    if (HAL_QSPI_Receive(&QSPIHandle, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
         Error_Handler(__FILE__, __LINE__);;
    }

    uiID = (buf[0] << 16) | (buf[1] << 8) | buf[2];

    return uiID;
}


/**
  * @brief  This function configure the dummy cycles on memory side.
  * @param  hqspi: QSPI handle
  * @retval None
  */
//static void QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *hqspi)
//{
//	QSPI_CommandTypeDef s_command;
//	uint16_t reg=0;

//	/* Initialize the read volatile configuration register command */
//	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
//	s_command.Instruction       = READ_VOL_CFG_REG_CMD;
//	s_command.AddressMode       = QSPI_ADDRESS_NONE;
//	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//	s_command.DataMode          = QSPI_DATA_1_LINE;
//	s_command.DummyCycles       = 0;
//	s_command.NbData            = 2;
//	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//	/* Configure the command */
//	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//	{
//		 Error_Handler(__FILE__, __LINE__);;
//	}

//	/* Reception of the data */
//	if (HAL_QSPI_Receive(hqspi, (uint8_t *)(&reg), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//	{
//		 Error_Handler(__FILE__, __LINE__);;
//	}

//	/* Enable write operations */
//	QSPI_WriteEnable(hqspi);

//	/* Update volatile configuration register (with new dummy cycles) */
//	s_command.Instruction = WRITE_VOL_CFG_REG_CMD;
//	MODIFY_REG(reg, 0xF0F0, ((DUMMY_CLOCK_CYCLES_READ_QUAD << 4) |
//						   (DUMMY_CLOCK_CYCLES_READ_QUAD << 12)));

//	/* Configure the write volatile configuration register command */
//	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//	{
//		 Error_Handler(__FILE__, __LINE__);;
//	}

//	/* Transmission of the data */
//	if (HAL_QSPI_Transmit(hqspi, (uint8_t *)(&reg), HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//	{
//		 Error_Handler(__FILE__, __LINE__);;
//	}

//}

/**
  * @brief  This function set the QSPI memory in 4-byte address mode
  * @param  hqspi: QSPI handle
  * @retval None
  */
//static void QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi)
//{
//  QSPI_CommandTypeDef s_command;

//  /* Initialize the command */
//  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
//  s_command.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD;
//  s_command.AddressMode       = QSPI_ADDRESS_NONE;
//  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
//  s_command.DataMode          = QSPI_DATA_NONE;
//  s_command.DummyCycles       = 0;
//  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
//  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
//  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

//  /* Enable write operations */
//  QSPI_WriteEnable(hqspi);
//  
//  /* Send the command */
//  if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
//  {
//     Error_Handler(__FILE__, __LINE__);;
//  }

//  /* Configure automatic polling mode to wait the memory is ready */
//  QSPI_AutoPollingMemReady(hqspi);

//}

/*
*********************************************************************************************************
*    函 数 名: QSPI_MemoryMapped
*    功能说明: QSPI内存映射，地址 0x90000000
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t QSPI_MemoryMapped(void)
{
    QSPI_CommandTypeDef s_command;
    QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

    /* ????? */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD;
    s_command.AddressMode = QSPI_ADDRESS_4_LINES;
    s_command.AddressSize = QSPI_ADDRESS_32_BITS;
    s_command.DataMode = QSPI_DATA_4_LINES;
    s_command.DummyCycles = 6;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* ???????? */
    s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    s_mem_mapped_cfg.TimeOutPeriod = 0;

    if (HAL_QSPI_MemoryMapped(&QSPIHandle, &s_command, &s_mem_mapped_cfg) != HAL_OK)
    {
        return 1;
    }

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: HAL_QSPI_CmdCpltCallback
*    功能说明: QSPI中断的回调函数，Flash擦除函数QSPI_EraseSector要使用
*    形    参: hqspi  QSPI_HandleTypeDef句柄
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    CmdCplt++;
}

/*
*********************************************************************************************************
*    函 数 名: HAL_QSPI_RxCpltCallback
*    功能说明: QSPI中断的回调函数，Flash读函数QSPI_ReadBuffer要使用
*    形    参: hqspi  QSPI_HandleTypeDef句柄
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    RxCplt++;
}

/*
*********************************************************************************************************
*    函 数 名: HAL_QSPI_TxCpltCallback
*    功能说明: QSPI中断的回调函数，Flash写函数QSPI_ReadBuffer要使用
*    形    参: hqspi  QSPI_HandleTypeDef句柄
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    TxCplt++;
}

/*
*********************************************************************************************************
*    函 数 名: HAL_QSPI_StatusMatchCallback
*    功能说明: QSPI中断的回调函数，Flash状态查询函数QSPI_AutoPollingMemReady使用
*    形    参: hqspi  QSPI_HandleTypeDef句柄
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi)
{
    StatusMatch++;
}

/*
*********************************************************************************************************
*    函 数 名: QUADSPI_IRQHandler
*    功能说明: QSPI中断服务程序
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void QUADSPI_IRQHandler(void)
{
    HAL_QSPI_IRQHandler(&QSPIHandle);
}

/*
*********************************************************************************************************
*    函 数 名: MDMA_IRQHandler
*    功能说明: MDMA中断服务程序
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void MDMA_IRQHandler(void)
{
    HAL_MDMA_IRQHandler(QSPIHandle.hmdma);
}
          
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
