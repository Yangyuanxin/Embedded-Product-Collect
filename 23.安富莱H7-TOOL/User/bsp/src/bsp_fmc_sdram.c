/*
*********************************************************************************************************
*
*    模块名称 : 外部SDRAM驱动模块
*    文件名称 : bsp_fmc_sdram.c
*    版    本 : V2.4
*    说    明 : 安富莱STM32-X6 V6开发板标配的 SDRAM为美光 MT48LC4M32B2TG-7  容量32M字节，32Bit, 7ns速度 (133MHz)
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2014-05-04 armfly  正式发布
*
*    Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/* #define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_8  */
/* #define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_16 */
#define SDRAM_MEMORY_WIDTH FMC_SDRAM_MEM_BUS_WIDTH_32

#define SDCLOCK_PERIOD FMC_SDRAM_CLOCK_PERIOD_2
/* #define SDCLOCK_PERIOD                FMC_SDRAM_CLOCK_PERIOD_3 */

#define SDRAM_TIMEOUT ((uint32_t)0xFFFF)
#define REFRESH_COUNT ((uint32_t)0x0603) /* SDRAM refresh counter */

/* FMC SDRAM Mode definition register defines */
#define SDRAM_MODEREG_BURST_LENGTH_1 ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2 ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t)0x0200)

static void SDRAM_GPIOConfig(void);
static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command);

/*
*********************************************************************************************************
*    函 数 名: bsp_InitExtSDRAM
*    功能说明: 配置连接外部SDRAM的GPIO和FMC
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitExtSDRAM(void)
{
    SDRAM_HandleTypeDef hsdram;
    FMC_SDRAM_TimingTypeDef SDRAM_Timing;
    FMC_SDRAM_CommandTypeDef command;

    /* GPIO configuration for FMC SDRAM bank */
    SDRAM_GPIOConfig();

    /*##-1- Configure the SDRAM device #########################################*/
    /* SDRAM device configuration */
    hsdram.Instance = FMC_SDRAM_DEVICE;

    /*##-1- Configure the SDRAM device #########################################*/
    /* SDRAM device configuration */
    hsdram.Instance = FMC_SDRAM_DEVICE;

    /* Timing configuration for 100Mhz as SDRAM clock frequency (System clock is up to 200Mhz) */
    SDRAM_Timing.LoadToActiveDelay = 2;
    SDRAM_Timing.ExitSelfRefreshDelay = 7;
    SDRAM_Timing.SelfRefreshTime = 4;
    SDRAM_Timing.RowCycleDelay = 7;
    SDRAM_Timing.WriteRecoveryTime = 2;
    SDRAM_Timing.RPDelay = 2;
    SDRAM_Timing.RCDDelay = 2;

    hsdram.Init.SDBank = FMC_SDRAM_BANK1;
    hsdram.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
    hsdram.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
    hsdram.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32; /* 32位 */
    hsdram.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hsdram.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
    hsdram.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram.Init.SDClockPeriod = SDCLOCK_PERIOD;
    hsdram.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hsdram.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;

    /* Initialize the SDRAM controller */
    if (HAL_SDRAM_Init(&hsdram, &SDRAM_Timing) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler(__FILE__, __LINE__);
    }

    /* Program the SDRAM external device */
    SDRAM_Initialization_Sequence(&hsdram, &command);
}

/*
*********************************************************************************************************
*    函 数 名: SDRAM_GPIOConfig
*    功能说明: 配置连接外部SDRAM的GPIO
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void SDRAM_GPIOConfig(void)
{
    GPIO_InitTypeDef GPIO_Init_Structure;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO clocks */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    /* Enable FMC clock */
    __HAL_RCC_FMC_CLK_ENABLE();

    /*-- 安富莱STM32-V7发板 SDRAM GPIO 定义 -----------------------------------------------------*/
    /*
     +-------------------+--------------------+--------------------+--------------------+
     +                       SDRAM pins assignment                                      +
     +-------------------+--------------------+--------------------+--------------------+
     | PD0  <-> FMC_D2   | PE0  <-> FMC_NBL0  | PF0  <-> FMC_A0    | PG0 <-> FMC_A10    |
     | PD1  <-> FMC_D3   | PE1  <-> FMC_NBL1  | PF1  <-> FMC_A1    | PG1 <-> FMC_A11    |
     | PD8  <-> FMC_D13  | PE7  <-> FMC_D4    | PF2  <-> FMC_A2    | PG4 <-> FMC_A14    |
     | PD9  <-> FMC_D14  | PE8  <-> FMC_D5    | PF3  <-> FMC_A3    | PG5 <-> FMC_A15    |
     | PD10 <-> FMC_D15  | PE9  <-> FMC_D6    | PF4  <-> FMC_A4    | PG8 <-> FC_SDCLK   |
     | PD14 <-> FMC_D0   | PE10 <-> FMC_D7    | PF5  <-> FMC_A5    | PG15 <-> FMC_NCAS  |
     | PD15 <-> FMC_D1   | PE11 <-> FMC_D8    | PF11 <-> FC_NRAS   |--------------------+
     +-------------------| PE12 <-> FMC_D9    | PF12 <-> FMC_A6    | PG2  --- FMC_A12 (预留64M字节容量，和摇杆上键复用）
                         | PE13 <-> FMC_D10   | PF13 <-> FMC_A7    |
                         | PE14 <-> FMC_D11   | PF14 <-> FMC_A8    |
                         | PE15 <-> FMC_D12   | PF15 <-> FMC_A9    |
     +-------------------+--------------------+--------------------+
     | PH2 <-> FMC_SDCKE0| PI4 <-> FMC_NBL2   |
     | PH3 <-> FMC_SDNE0 | PI5 <-> FMC_NBL3   |
     | PH5 <-> FMC_SDNW  |--------------------+
     +-------------------+
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

     +-------------------+
     +  Pins remapping   +
     +-------------------+
     | PC0 <-> FMC_SDNWE |
     | PC2 <-> FMC_SDNE0 |
     | PC3 <-> FMC_SDCKE0|
     +-------------------+

    */
    /*##-2- Configure peripheral GPIO ##########################################*/
    /* Common GPIO configuration */
    GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull = GPIO_PULLUP;
    GPIO_Init_Structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_Init_Structure.Alternate = GPIO_AF12_FMC;

    /* GPIOD configuration */
    GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
                                                        GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

    /* GPIOE configuration */
    GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |
                                                        GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                                                        GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

    /* GPIOF configuration */
    GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
                                                        GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                                                        GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);

    /* GPIOG configuration */
    //    GPIO_Init_Structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |\
//                              GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
    GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 |
                                                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

    /* GPIOH configuration */
    //    GPIO_Init_Structure.Pin   = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 |\
//                              GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
//                              GPIO_PIN_15;
    GPIO_Init_Structure.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 |
                                                        GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                                                        GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH, &GPIO_Init_Structure);

    /* GPIOI configuration */
    GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
                                                        GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);
}

/*
*********************************************************************************************************
*    函 数 名: SDRAM_Initialization_Sequence
*    功能说明: Perform the SDRAM exernal memory inialization sequence
*    形    参: hsdram: SDRAM handle
*              Command: Pointer to SDRAM command structure
*    返 回 值: None
*********************************************************************************************************
*/
static void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
    __IO uint32_t tmpmrd = 0;
    /* Step 1:  Configure a clock configuration enable command */
    Command->CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    ;
    Command->AutoRefreshNumber = 1;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /* Step 2: Insert 100 us minimum delay */
    /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
    HAL_Delay(1);

    /* Step 3: Configure a PALL (precharge all) command */
    Command->CommandMode = FMC_SDRAM_CMD_PALL;
    Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber = 1;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /* Step 4 : Configure a Auto-Refresh command */
    Command->CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber = 8;
    Command->ModeRegisterDefinition = 0;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /* Step 5: Program the external memory mode register */
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
                     SDRAM_MODEREG_CAS_LATENCY_3 |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    Command->CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command->CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
    Command->AutoRefreshNumber = 1;
    Command->ModeRegisterDefinition = tmpmrd;

    /* Send the command */
    HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);

    /* Step 6: Set the refresh rate counter */
    /* Set the device refresh rate */
    HAL_SDRAM_ProgramRefreshRate(hsdram, REFRESH_COUNT);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_TestExtSDRAM
*    功能说明: 扫描测试外部SRAM, 全部单元。
*    形    参: 无
*    返 回 值: 0 表示测试通过； 大于0表示错误单元的个数。
*********************************************************************************************************
*/
uint32_t bsp_TestExtSDRAM1(void)
{
    uint32_t i;
    uint32_t *pSRAM;
    uint8_t *pBytes;
    uint32_t err;
    const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

    /* 写SRAM */
    pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
    for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
    {
        *pSRAM++ = i;
    }

    /* 读SRAM */
    err = 0;
    pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
    for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
    {
        if (*pSRAM++ != i)
        {
            err++;
        }
    }

    if (err > 0)
    {
        return (4 * err);
    }

    /* 对SRAM 的数据求反并写入 */
    pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
    for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
    {
        *pSRAM = ~*pSRAM;
        pSRAM++;
    }

    /* 再次比较SDRAM的数据 */
    err = 0;
    pSRAM = (uint32_t *)EXT_SDRAM_ADDR;
    for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
    {
        if (*pSRAM++ != (~i))
        {
            err++;
        }
    }

    if (err > 0)
    {
        return (4 * err);
    }

    /* 测试按字节方式访问, 目的是验证 FSMC_NBL0 、 FSMC_NBL1 口线 */
    pBytes = (uint8_t *)EXT_SDRAM_ADDR;
    for (i = 0; i < sizeof(ByteBuf); i++)
    {
        *pBytes++ = ByteBuf[i];
    }

    /* 比较SDRAM的数据 */
    err = 0;
    pBytes = (uint8_t *)EXT_SDRAM_ADDR;
    for (i = 0; i < sizeof(ByteBuf); i++)
    {
        if (*pBytes++ != ByteBuf[i])
        {
            err++;
        }
    }
    if (err > 0)
    {
        return err;
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_TestExtSDRAM2
*    功能说明: 扫描测试外部SDRAM. 不扫描前面4M字节的显存。
*    形    参: 无
*    返 回 值: 0 表示测试通过； 大于0表示错误单元的个数。
*********************************************************************************************************
*/
uint32_t bsp_TestExtSDRAM2(void)
{
    uint32_t i;
    uint32_t *pSRAM;
    uint8_t *pBytes;
    uint32_t err;
    const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

    /* 写SRAM */
    pSRAM = (uint32_t *)SDRAM_APP_BUF;
    for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
    {
        *pSRAM++ = i;
    }

    /* 读SRAM */
    err = 0;
    pSRAM = (uint32_t *)SDRAM_APP_BUF;
    for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
    {
        if (*pSRAM++ != i)
        {
            err++;
        }
    }

    if (err > 0)
    {
        return (4 * err);
    }

#if 0
    /* 对SRAM 的数据求反并写入 */
    pSRAM = (uint32_t *)SDRAM_APP_BUF;
    for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
    {
        *pSRAM = ~*pSRAM;
        pSRAM++;
    }

    /* 再次比较SDRAM的数据 */
    err = 0;
    pSRAM = (uint32_t *)SDRAM_APP_BUF;
    for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
    {
        if (*pSRAM++ != (~i))
        {
            err++;
        }
    }

    if (err >  0)
    {
        return (4 * err);
    }
#endif

    /* 测试按字节方式访问, 目的是验证 FSMC_NBL0 、 FSMC_NBL1 口线 */
    pBytes = (uint8_t *)SDRAM_APP_BUF;
    for (i = 0; i < sizeof(ByteBuf); i++)
    {
        *pBytes++ = ByteBuf[i];
    }

    /* 比较SDRAM的数据 */
    err = 0;
    pBytes = (uint8_t *)SDRAM_APP_BUF;
    for (i = 0; i < sizeof(ByteBuf); i++)
    {
        if (*pBytes++ != ByteBuf[i])
        {
            err++;
        }
    }
    if (err > 0)
    {
        return err;
    }
    return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
