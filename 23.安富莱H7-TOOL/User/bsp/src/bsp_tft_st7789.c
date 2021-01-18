/*
*********************************************************************************************************
*
*    模块名称 : ST7789 TFT SPI接口驱动程序
*    文件名称 : bsp_tft_st7789.c
*    版    本 : V1.2
*    说    明 : SPI接口，显示驱动IC为ST7789，分辨率为240*240,1.3寸ISP
*    修改记录 :
*        版本号  日期       作者        说明
*        V1.0    2018-12-06 armfly 
*        V1.1    2019-03-25 armfly      软件SPI，优化执行速度
*        V1.2    2019-11-29 baiyongbin  硬件SPI+DMA提高刷屏速度
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "fonts.h"
#include "param.h"
#include "nvic_prio_cfg.h"

#define LCD_DMA_CIRCULE_MODE    0

/*
    H7-TOOL LCD口线分配
    ----- 第6版 -----
    PG15  --->  LCD_RS
    PE1   --->  LCD_CS
    PH6   --->  LCD_SCK        SPI3_SCK    
    PH7   --->  LCD_SDA        SPI3_MOSI
    PH9   --->  BACK_LIGHT    TIM12_CH2
    
    PB6   --->  LCD_RESET
    PH15  --->  电源控制
    
    ----- 第4、5版 -----
    PF3  --->  LCD_RS
    PE1  --->  LCD_CS
    PG10 --->  LCD_SCK        SPI3_SCK    
    PG15 --->  LCD_SDA        SPI3_MOSI
    PH9  --->  BACK_LIGHT    TIM12_CH2
    
    PB6  --->  LCD_RESET
    PH15 --->  电源控制
    
    --------------- 旧版 -------
    PF3  --->  LCD_RS
    PG2  --->  LCD_CS
    PC12 --->  LCD_SDA        SPI3_MOSI
    PC10 --->  LCD_SCK        SPI3_SCK
    
    PH9  --->  BACK_LIGHT    TIM12_CH2
    
    第2版增加reset
    
    PB6  --->  LCD_RESET
*/

#define SPI5_SCK_CLK_ENABLE()   __HAL_RCC_GPIOH_CLK_ENABLE()
#define SPI5_SCK_GPIO           GPIOH
#define SPI5_SCK_PIN            GPIO_PIN_6
#define SPI5_SCK_AF             GPIO_AF5_SPI5

#define SPI5_MOSI_CLK_ENABLE()  __HAL_RCC_GPIOH_CLK_ENABLE()
#define SPI5_MOSI_GPIO          GPIOH
#define SPI5_MOSI_PIN           GPIO_PIN_7
#define SPI5_MOSI_AF            GPIO_AF5_SPI5

#define ALL_LCD_GPIO_CLK_ENABLE() \
    __HAL_RCC_GPIOB_CLK_ENABLE();   \
    __HAL_RCC_GPIOE_CLK_ENABLE();   \
    __HAL_RCC_GPIOG_CLK_ENABLE();   \
    __HAL_RCC_GPIOH_CLK_ENABLE();

/* LCD_RS 寄存器选择 */
#define LCD_RS_GPIO     GPIOG
#define LCD_RS_PIN      GPIO_PIN_15
#define LCD_RS_0()      BSP_SET_GPIO_0(LCD_RS_GPIO, LCD_RS_PIN)
#define LCD_RS_1()      BSP_SET_GPIO_1(LCD_RS_GPIO, LCD_RS_PIN)

/* LCD_CS SPI片选*/
#define LCD_CS_GPIO     GPIOE
#define LCD_CS_PIN      GPIO_PIN_1
#define LCD_CS_0()      BSP_SET_GPIO_0(LCD_CS_GPIO, LCD_CS_PIN)
#define LCD_CS_1()      BSP_SET_GPIO_1(LCD_CS_GPIO, LCD_CS_PIN)

/* SPI 接口 */
#define LCD_SCK_GPIO    GPIOH
#define LCD_SCK_PIN     GPIO_PIN_6
#define LCD_SCK_0()     BSP_SET_GPIO_0(LCD_SCK_GPIO, LCD_SCK_PIN)
#define LCD_SCK_1()     BSP_SET_GPIO_1(LCD_SCK_GPIO, LCD_SCK_PIN)

#define LCD_SDA_GPIO    GPIOH
#define LCD_SDA_PIN     GPIO_PIN_7
#define LCD_SDA_0()     BSP_SET_GPIO_0(LCD_SDA_GPIO, LCD_SDA_PIN)
#define LCD_SDA_1()     BSP_SET_GPIO_1(LCD_SDA_GPIO, LCD_SDA_PIN)

/* LCD_RESET 复位 */
#define LCD_RESET_GPIO  GPIOB
#define LCD_RESET_PIN   GPIO_PIN_6
#define LCD_RESET_0()   BSP_SET_GPIO_0(LCD_RESET_GPIO, LCD_RESET_PIN)
#define LCD_RESET_1()   BSP_SET_GPIO_1(LCD_RESET_GPIO, LCD_RESET_PIN)

/* LCD_PWR_EN 电源控制 */
#define LCD_PWR_EN_GPIO GPIOH
#define LCD_PWR_EN_PIN  GPIO_PIN_15
#define LCD_PWR_EN_0()  BSP_SET_GPIO_0(LCD_PWR_EN_GPIO, LCD_PWR_EN_PIN)
#define LCD_PWR_EN_1()  BSP_SET_GPIO_1(LCD_PWR_EN_GPIO, LCD_PWR_EN_PIN)

static void ST7789_ConfigGPIO(void);
static void ST7789_initial(void);
static void ST7789_SendByteQuick(uint8_t data);

SPI_HandleTypeDef hspi5 = {0};
DMA_HandleTypeDef hdma_tx = {0};
__IO uint32_t wTransferState = 99;

static uint16_t *s_pDispBuf;
static uint8_t s_DispRefresh = 0;

/*
*********************************************************************************************************
*    函 数 名: ST7789_InitHard
*    功能说明: 初始化LCD
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_InitHard(void)
{
    ST7789_ConfigGPIO();    

    ST7789_initial();

    g_LcdHeight = 240;      /* 显示屏分辨率-高度 */
    g_LcdWidth = 240;       /* 显示屏分辨率-宽度 */
    
    s_pDispBuf = (uint16_t *)(0x30000000);
    
    ;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitSPI5ParamFast
*    功能说明: 硬件SPI配置
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPI5ParamFast(void)
{
	hspi5.Instance = SPI5;
	hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi5.Init.Direction = SPI_DIRECTION_2LINES;
	hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi5.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi5.Init.DataSize = SPI_DATASIZE_16BIT;
	hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi5.Init.CRCPolynomial = 7;
	hspi5.Init.CRCLength = SPI_CRC_LENGTH_8BIT;
	hspi5.Init.NSS = SPI_NSS_SOFT;
	hspi5.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	hspi5.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE; /* Recommanded setting to avoid glitches */
	hspi5.Init.Mode = SPI_MODE_MASTER;
	hspi5.Init.IOSwap = SPI_IO_SWAP_ENABLE;
	hspi5.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;

	HAL_SPI_Init(&hspi5);

	/* Enable DMA clock */
	__HAL_RCC_DMA2_CLK_ENABLE();

	/* Configure the DMA handler for Transmission process */
	hdma_tx.Instance                 = DMA2_Stream3;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
	hdma_tx.Init.PeriphBurst         = DMA_PBURST_SINGLE;
	hdma_tx.Init.Request             = DMA_REQUEST_SPI5_TX;
	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    
    #if LCD_DMA_CIRCULE_MODE == 1
        hdma_tx.Init.Mode            = DMA_CIRCULAR;
    #else
        hdma_tx.Init.Mode            = DMA_NORMAL; 
    #endif
    
	hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

	HAL_DMA_Init(&hdma_tx);

	/* Associate the initialized DMA handle to the the SPI handle */
	__HAL_LINKDMA(&hspi5, hdmatx, hdma_tx);
	
	HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, TFT_DMA2_STREAM3_IRQ_PRIO, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

	HAL_NVIC_SetPriority(SPI5_IRQn, TFT_SPI5_IRQ_PRIO, 0);
	HAL_NVIC_EnableIRQ(SPI5_IRQn);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitSPI5_Fast
*    功能说明: 配置SPI总线。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPI5_Fast(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 配置GPIO时钟 */
    SPI5_SCK_CLK_ENABLE();

    SPI5_MOSI_CLK_ENABLE();

    /* 使能SPI时钟 */
    __HAL_RCC_SPI5_CLK_ENABLE();

    /* 配置GPIO为SPI功能 */
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP; /* 上拉 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM; //GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = SPI5_SCK_AF;

    GPIO_InitStruct.Pin = SPI5_SCK_PIN;
    HAL_GPIO_Init(SPI5_SCK_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI5_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPI5_MOSI_AF;
    HAL_GPIO_Init(SPI5_MOSI_GPIO, &GPIO_InitStruct);

	bsp_InitSPI5ParamFast();
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	wTransferState = 1;
    
    LCD_CS_1();
}

void DMA2_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(hspi5.hdmatx);
}

void SPI5_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&hspi5);
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_ConfigLTDC
*    功能说明: 配置LTDC
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void ST7789_ConfigGPIO(void)
{
    /* 配置GPIO */
    {
        GPIO_InitTypeDef gpio_init;

        /* 打开GPIO时钟 */
        ALL_LCD_GPIO_CLK_ENABLE();

        LCD_CS_1();
        LCD_SCK_1();

        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        /* 设置推挽输出 */
        gpio_init.Pull = GPIO_NOPULL;                        /* 上下拉电阻不使能 */
        gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;     /* GPIO_SPEED_FREQ_MEDIUM GPIO_SPEED_FREQ_HIGH */

        gpio_init.Pin = LCD_RS_PIN;
        HAL_GPIO_Init(LCD_RS_GPIO, &gpio_init);

        gpio_init.Pin = LCD_CS_PIN;
        HAL_GPIO_Init(LCD_CS_GPIO, &gpio_init);

//        gpio_init.Pin = LCD_SCK_PIN;
//        HAL_GPIO_Init(LCD_SCK_GPIO, &gpio_init);

//        gpio_init.Pin = LCD_SDA_PIN;
//        HAL_GPIO_Init(LCD_SDA_GPIO, &gpio_init);

        gpio_init.Pin = LCD_RESET_PIN;
        HAL_GPIO_Init(LCD_RESET_GPIO, &gpio_init);
    }
	
	bsp_InitSPI5_Fast();
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DrawScreen
*    功能说明: 整屏刷新。 插入bsp_Idle()函数执行
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DrawScreenHard(void)
{
    ST7789_SetDispWin(0, 0, 240, 240);

	bsp_InitSPI5ParamFast();
    LCD_RS_1();
    LCD_CS_0();     /* 在DMA传输完毕后设置1 */
	
	wTransferState = 0; 
        
	HAL_SPI_Transmit_DMA(&hspi5, (uint8_t *)(0x30000000),  240 * 240);

    while (wTransferState == 0){}  
}
    
void ST7789_DrawScreen(void)
{
 #if LCD_DMA_CIRCULE_MODE == 1
    static uint8_t first_run = 1;
    
    if (first_run == 1)
    {
        first_run = 0;       

        ST7789_SetDispWin(0, 0, 240, 240);

        bsp_InitSPI5ParamFast();
        LCD_RS_1();
        LCD_CS_0();

        wTransferState = 0; 
//        SCB_CleanInvalidateDCache();

        HAL_SPI_Transmit_DMA(&hspi5, (uint8_t *)(0x30000000),  240 * 240);     
    }
#else    
    static int32_t s_time1 = 0;
    static uint8_t s_first_run = 0;     /* 上电第1次运行，准备好数据后开背光，避免短暂花屏 */
    
    if (s_DispRefresh == 0)
    {
        return;
    }
    
    /* 控制刷屏周期，限制最快50ms */
    if (bsp_CheckRunTime(s_time1) < 50)
    {
         return;
    }
        
    s_DispRefresh = 0;
    
    ST7789_DrawScreenHard();
        
    s_time1 = bsp_GetRunTime();
        
    if (s_first_run == 0)
    {
        s_first_run = 1;
        
        LCD_SetBackLight(BRIGHT_DEFAULT);   /* 打开背光 255 */
    }
#endif    
}

/*写指令到 LCD 模块*/
void Lcd_WriteIndex(uint8_t data1)
{

    LCD_RS_0();
    LCD_CS_0();

    ST7789_SendByteQuick(data1);

    LCD_CS_1();
}

/* 优化代码，快速操作 */
static void ST7789_SendByteQuick(uint8_t data)
{
#if 1   /* 硬件模式 */
	SPI5->CFG1 = SPI_BAUDRATEPRESCALER_8 | 7;
    SPI5->CR1 = SPI_CR1_SSI;
    SPI5->CR2 = 1;
    SPI5->CR1 = SPI_CR1_SPE | SPI_CR1_SSI;
    SPI5->CR1 = SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_CSTART;

    while ((SPI5->SR & SPI_FLAG_TXE) == 0);

    *((__IO uint8_t *)&SPI5->TXDR) = data;
	
    while ((SPI5->SR & SPI_SR_TXC) == 0);
	
    SPI5->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;
	
	SPI5->CR1 &= ~(SPI_CR1_SPE);
#else   /* 软件模式 */
    uint8_t bit;

    if (data & 0x80)
    {
        LCD_SDA_1();
    }
    else
    {
        LCD_SDA_0();
    }
    LCD_SCK_0();
    LCD_SCK_1();

    /* bit6 */
    bit = (data & 0xC0);
    if (bit == 0x80)
    {
        LCD_SDA_0();
    }
    else if (bit == 0x40)
    {
        LCD_SDA_1();
    }
    LCD_SCK_0();
    LCD_SCK_1();

    /* bit5 */
    data <<= 1;
    bit = (data & 0xC0);
    if (bit == 0x80)
    {
        LCD_SDA_0();
    }
    else if (bit == 0x40)
    {
        LCD_SDA_1();
    }
    LCD_SCK_0();
    LCD_SCK_1();

    /* bit4 */
    data <<= 1;
    bit = (data & 0xC0);
    if (bit == 0x80)
    {
        LCD_SDA_0();
    }
    else if (bit == 0x40)
    {
        LCD_SDA_1();
    }
    LCD_SCK_0();
    LCD_SCK_1();

    /* bit3 */
    data <<= 1;
    bit = (data & 0xC0);
    if (bit == 0x80)
    {
        LCD_SDA_0();
    }
    else if (bit == 0x40)
    {
        LCD_SDA_1();
    }
    LCD_SCK_0();
    LCD_SCK_1();

    /* bit2 */
    data <<= 1;
    bit = (data & 0xC0);
    if (bit == 0x80)
    {
        LCD_SDA_0();
    }
    else if (bit == 0x40)
    {
        LCD_SDA_1();
    }
    LCD_SCK_0();
    LCD_SCK_1();

    /* bit1 */
    data <<= 1;
    bit = (data & 0xC0);
    if (bit == 0x80)
    {
        LCD_SDA_0();
    }
    else if (bit == 0x40)
    {
        LCD_SDA_1();
    }
    LCD_SCK_0();
    LCD_SCK_1();

    /* bit0 */
    data <<= 1;
    bit = (data & 0xC0);
    if (bit == 0x80)
    {
        LCD_SDA_0();
    }
    else if (bit == 0x40)
    {
        LCD_SDA_1();
    }
    LCD_SCK_0();
    LCD_SCK_1();
#endif
}

/*写数据到 LCD 模块*/
void Lcd_WriteData(uint8_t data1)
{
    LCD_RS_1();
    LCD_CS_0();
    ST7789_SendByteQuick(data1);
    LCD_CS_1();
}

// 连写2个字节（即 16 位）数据到LCD模块
void Lcd_WriteData_16(uint16_t data2)
{
    LCD_RS_1();
    LCD_CS_0();

    ST7789_SendByteQuick(data2 >> 8);
    ST7789_SendByteQuick(data2);

    LCD_CS_1();
}

//LCD 初始化
static void ST7789_initial(void)
{
    LCD_PWR_EN_1();

    bsp_DelayUS(10 * 1000);         /* 等待电源稳定 */
    LCD_RESET_0();                  /* 低电平：复位， 只需要大于10us */
    bsp_DelayUS(20);                /* 延迟 20us */
    LCD_RESET_1();                  /* 高电平：复位结束 */
    bsp_DelayUS(10 * 1000);         /* 复位之后，要求等待至少5ms, 此处等待10ms */

    Lcd_WriteIndex(0x36);           /* 扫描方向 */
    Lcd_WriteData(0x00);

    Lcd_WriteIndex(0x3A);
    Lcd_WriteData(0x05);

    Lcd_WriteIndex(0xB2);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x33);
    Lcd_WriteData(0x33);

    Lcd_WriteIndex(0xB7);
    Lcd_WriteData(0x35);

    Lcd_WriteIndex(0xBB);
    Lcd_WriteData(0x19);

    Lcd_WriteIndex(0xC0);
    Lcd_WriteData(0x2C);

    Lcd_WriteIndex(0xC2);
    Lcd_WriteData(0x01);

    Lcd_WriteIndex(0xC3);
    Lcd_WriteData(0x12);

    Lcd_WriteIndex(0xC4);
    Lcd_WriteData(0x20);

    Lcd_WriteIndex(0xC6);
    Lcd_WriteData(0x0F);

    Lcd_WriteIndex(0xD0);
    Lcd_WriteData(0xA4);
    Lcd_WriteData(0xA1);

    Lcd_WriteIndex(0xE0);
    Lcd_WriteData(0xD0);
    Lcd_WriteData(0x04);
    Lcd_WriteData(0x0D);
    Lcd_WriteData(0x11);
    Lcd_WriteData(0x13);
    Lcd_WriteData(0x2B);
    Lcd_WriteData(0x3F);
    Lcd_WriteData(0x54);
    Lcd_WriteData(0x4C);
    Lcd_WriteData(0x18);
    Lcd_WriteData(0x0D);
    Lcd_WriteData(0x0B);
    Lcd_WriteData(0x1F);
    Lcd_WriteData(0x23);

    Lcd_WriteIndex(0xE1);
    Lcd_WriteData(0xD0);
    Lcd_WriteData(0x04);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x11);
    Lcd_WriteData(0x13);
    Lcd_WriteData(0x2C);
    Lcd_WriteData(0x3F);
    Lcd_WriteData(0x44);
    Lcd_WriteData(0x51);
    Lcd_WriteData(0x2F);
    Lcd_WriteData(0x1F);
    Lcd_WriteData(0x1F);
    Lcd_WriteData(0x20);
    Lcd_WriteData(0x23);

    Lcd_WriteIndex(0x21);

    Lcd_WriteIndex(0x11);

    Lcd_WriteIndex(0x29);
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_GetChipDescribe
*    功能说明: 读取LCD驱动芯片的描述符号，用于显示
*    形    参: char *_str : 描述符字符串填入此缓冲区
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_GetChipDescribe(char *_str)
{
    strcpy(_str, "ST7789");
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_SetDispWin
*    功能说明: 设置显示窗口，进入窗口显示模式。
*    形    参:  
*        _usX : 水平坐标
*        _usY : 垂直坐标
*        _usHeight: 窗口高度
*        _usWidth : 窗口宽度
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_SetDispWin(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{
    /* 优化代码，坐标不变化，则不更改 */
    static uint16_t s_x1 = 9999;
    static uint16_t s_x2 = 9999;
    static uint16_t s_y1 = 9999;
    static uint16_t s_y2 = 9999;
    uint16_t x1, x2, y1, y2;

    /* 设置 X 开始及结束的地址 */
    if (g_tParam.DispDir == 3)
    {
        _usX += 319 - 239;
    }
    x1 = _usX;
    x2 = _usX + _usWidth - 1;
    if (s_x1 != x1 || s_x2 != x2)
    {
        Lcd_WriteIndex(0x2a);
        Lcd_WriteData_16(x1);
        Lcd_WriteData_16(x2);
        s_x1 = x1;
        s_x2 = x2;
    }

    /* 设置 Y开始及结束的地址 */
    if (g_tParam.DispDir == 1)
    {
        /*
            The address ranges are X=0 to X=239 (Efh) and Y=0 to Y=319 (13Fh).    
        */
        _usY += 319 - 239;
    }
    y1 = _usY;
    y2 = _usY + 0 + _usHeight - 1;
    if (s_y1 != y1 || s_y2 != y2)
    {
        Lcd_WriteIndex(0x2b);
        Lcd_WriteData_16(y1);
        Lcd_WriteData_16(y2);
        s_y1 = y1;
        s_y2 = y2;
    }

    Lcd_WriteIndex(0x2c); /* 写数据开始 */
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_QuitWinMode
*    功能说明: 退出窗口显示模式，变为全屏显示模式
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_QuitWinMode(void)
{
    ST7789_SetDispWin(0, 0, g_LcdHeight, g_LcdWidth);
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DispOn
*    功能说明: 打开显示
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DispOn(void)
{
#if LCD_DMA_CIRCULE_MODE == 1
    LCD_CS_1();
    Lcd_WriteIndex(0x11);    
    
    ST7789_SetDispWin(0, 0, 240, 240);

    bsp_InitSPI5ParamFast();
    LCD_RS_1();
    LCD_CS_0();

    HAL_SPI_Transmit_DMA(&hspi5, (uint8_t *)(0x30000000),  240 * 240);  
#else    
    Lcd_WriteIndex(0x11);
#endif    
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DispOff
*    功能说明: 关闭显示
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DispOff(void)
{
#if LCD_DMA_CIRCULE_MODE == 1
    LCD_CS_1();
    Lcd_WriteIndex(0x10);    
    
    HAL_SPI_Abort(&hspi5);
#else      
    Lcd_WriteIndex(0x10);
#endif    
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_ClrScr
*    功能说明: 根据输入的颜色值清屏
*    形    参: _usColor : 背景色
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_ClrScr(uint16_t _usColor)
{
    uint16_t i;
    uint32_t *p = (uint32_t *)s_pDispBuf;
    uint32_t color;
    
    color = ((uint32_t)_usColor << 16) + _usColor;
    for (i = 0; i < 240 * 240 / 2; i++)
    {
        *p++ = color;
    }
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_PutPixel
*    功能说明: 画1个像素
*    形    参:
*            _usX,_usY : 像素坐标
*            _usColor  : 像素颜色 ( RGB = 565 格式)
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
    #if 1
        uint32_t idx;
    
        idx = _usY * 240 + _usX;
        if (idx < 240 * 240 * 2)
        {
            s_pDispBuf[idx] = _usColor;
        }
    #else
        ST7789_SetDispWin(_usX, _usY, 1, 1);
        Lcd_WriteData_16(_usColor);    
    #endif
    
    s_DispRefresh = 1;
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_GetPixel
*    功能说明: 读取1个像素
*    形    参:
*            _usX,_usY : 像素坐标
*            _usColor  : 像素颜色
*    返 回 值: RGB颜色值
*********************************************************************************************************
*/
uint16_t ST7789_GetPixel(uint16_t _usX, uint16_t _usY)
{
    #if 1    
        return s_pDispBuf[_usY * 240 + _usX];
    #else    
        return CL_BLUE;
    #endif
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DrawLine
*    功能说明: 采用 Bresenham 算法，在2点间画一条直线。
*    形    参:
*            _usX1, _usY1 : 起始点坐标
*            _usX2, _usY2 : 终止点Y坐标
*            _usColor     : 颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DrawLine(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor)
{
    int32_t dx, dy;
    int32_t tx, ty;
    int32_t inc1, inc2;
    int32_t d, iTag;
    int32_t x, y;

    /* 采用 Bresenham 算法，在2点间画一条直线 */

    ST7789_PutPixel(_usX1, _usY1, _usColor);

    /* 如果两点重合，结束后面的动作。*/
    if (_usX1 == _usX2 && _usY1 == _usY2)
    {
        return;
    }

    iTag = 0;
    /* dx = abs ( _usX2 - _usX1 ); */
    if (_usX2 >= _usX1)
    {
        dx = _usX2 - _usX1;
    }
    else
    {
        dx = _usX1 - _usX2;
    }

    /* dy = abs ( _usY2 - _usY1 ); */
    if (_usY2 >= _usY1)
    {
        dy = _usY2 - _usY1;
    }
    else
    {
        dy = _usY1 - _usY2;
    }

    if (dx < dy)        /*如果dy为计长方向，则交换纵横坐标。*/
    {
        uint16_t temp;

        iTag = 1;
        temp = _usX1;
        _usX1 = _usY1;
        _usY1 = temp;
        temp = _usX2;
        _usX2 = _usY2;
        _usY2 = temp;
        temp = dx;
        dx = dy;
        dy = temp;
    }
    tx = _usX2 > _usX1 ? 1 : -1; /* 确定是增1还是减1 */
    ty = _usY2 > _usY1 ? 1 : -1;
    x = _usX1;
    y = _usY1;
    inc1 = 2 * dy;
    inc2 = 2 * (dy - dx);
    d = inc1 - dx;
    while (x != _usX2) /* 循环画点 */
    {
        if (d < 0)
        {
            d += inc1;
        }
        else
        {
            y += ty;
            d += inc2;
        }
        if (iTag)
        {
            ST7789_PutPixel(y, x, _usColor);
        }
        else
        {
            ST7789_PutPixel(x, y, _usColor);
        }
        x += tx;
    }
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DrawHLine
*    功能说明: 绘制一条水平线. 使用STM32F429 DMA2D硬件绘制.
*    形    参:
*            _usX1, _usY1 : 起始点坐标
*            _usLen       : 线的长度
*            _usColor     : 颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DrawHLine(uint16_t _usX, uint16_t _usY, uint16_t _usLen, uint16_t _usColor)
{
#if 0
    ST7789_FillRect(_usX, _usY, 1, _usLen, _usColor);
#else
    uint16_t i;

    for (i = 0; i < _usLen; i++)
    {
        ST7789_PutPixel(_usX + i, _usY, _usColor);
    }
#endif
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DrawVLine
*    功能说明: 绘制一条垂直线。 使用STM32F429 DMA2D硬件绘制.
*    形    参:
*            _usX, _usY : 起始点坐标
*            _usLen       : 线的长度
*            _usColor     : 颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DrawVLine(uint16_t _usX, uint16_t _usY, uint16_t _usLen, uint16_t _usColor)
{
#if 0
    ST7789_FillRect(_usX, _usY, _usLen, 1, _usColor);
#else
    uint16_t i;

    for (i = 0; i < _usLen; i++)
    {
        ST7789_PutPixel(_usX, _usY + i, _usColor);
    }
#endif
}
/*
*********************************************************************************************************
*    函 数 名: ST7789_DrawPoints
*    功能说明: 采用 Bresenham 算法，绘制一组点，并将这些点连接起来。可用于波形显示。
*    形    参:
*            x, y     : 坐标数组
*            _usColor : 颜色
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DrawPoints(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor)
{
    uint16_t i;

    for (i = 0; i < _usSize - 1; i++)
    {
        ST7789_DrawLine(x[i], y[i], x[i + 1], y[i + 1], _usColor);
    }
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DrawRect
*    功能说明: 绘制水平放置的矩形。
*    形    参:
*            _usX,_usY: 矩形左上角的坐标
*            _usHeight : 矩形的高度
*            _usWidth  : 矩形的宽度
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DrawRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
    /*
     ---------------->---
    |(_usX，_usY)        |
    V                    V  _usHeight
    |                    |
     ---------------->---
          _usWidth
    */
    ST7789_DrawHLine(_usX, _usY, _usWidth, _usColor);
    ST7789_DrawVLine(_usX + _usWidth - 1, _usY, _usHeight, _usColor);
    ST7789_DrawHLine(_usX, _usY + _usHeight - 1, _usWidth, _usColor);
    ST7789_DrawVLine(_usX, _usY, _usHeight, _usColor);
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_FillRect
*    功能说明: 用一个颜色值填充一个矩形。
*    形    参:
*            _usX,_usY: 矩形左上角的坐标
*            _usHeight : 矩形的高度
*            _usWidth  : 矩形的宽度
*            _usColor  : 颜色代码
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_FillRect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
#if 1
    uint16_t i, j;
    uint16_t width, height;
    
    width = _usWidth;
    height = _usHeight;
    if (height > g_LcdHeight)
    {
        height = g_LcdHeight;
    }
    if (width > g_LcdWidth)
    {
        width = g_LcdWidth;
    }   

    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            ST7789_PutPixel(_usX + i, _usY + j, _usColor);
        }
    }
#else    
    uint32_t i;
	uint32_t Color;
	uint32_t blkcnt;
	
	Color = (uint32_t)(_usColor<<16) + _usColor;
	blkcnt = (_usHeight * _usWidth)>>1;
	
    ST7789_SetDispWin(_usX, _usY, _usHeight, _usWidth);

    LCD_RS_1();
    LCD_CS_0();
	
	SPI5->CFG1 = SPI_BAUDRATEPRESCALER_2 | 31;
    for (i = 0; i < blkcnt; i++)
    {     
		SPI5->CR1 = SPI_CR1_SSI; 
		SPI5->CR2 = 2;
		SPI5->CR1 = SPI_CR1_SPE | SPI_CR1_SSI;
		SPI5->CR1 = SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_CSTART;
		
		while ((SPI5->SR & SPI_FLAG_TXE) == 0);

		*((__IO uint32_t *)&SPI5->TXDR) = Color;

		while ((SPI5->SR & SPI_SR_TXC) == 0);
		SPI5->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

		SPI5->CR1 &= ~(SPI_CR1_SPE);
    }
	
	if((_usWidth % 2) == 1)
	{
		SPI5->CFG1 = SPI_BAUDRATEPRESCALER_2 | 15;
		SPI5->CR1 = SPI_CR1_SSI;
		SPI5->CR2 = 1;
		SPI5->CR1 = SPI_CR1_SPE | SPI_CR1_SSI;
		SPI5->CR1 = SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_CSTART;

		while ((SPI5->SR & SPI_FLAG_TXE) == 0);

		*((__IO uint16_t *)&SPI5->TXDR) = _usColor;

		while ((SPI5->SR & SPI_SR_TXC) == 0);
		SPI5->IFCR = SPI_IFCR_EOTC | SPI_IFCR_TXTFC;

		SPI5->CR1 &= ~(SPI_CR1_SPE);
	}
    LCD_CS_1();
#endif    
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DrawCircle
*    功能说明: 绘制一个圆，笔宽为1个像素
*    形    参:
*            _usX,_usY  : 圆心的坐标
*            _usRadius  : 圆的半径
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DrawCircle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
    int32_t D;         /* Decision Variable */
    uint32_t CurX; /* 当前 X 值 */
    uint32_t CurY; /* 当前 Y 值 */

    D = 3 - (_usRadius << 1);
    CurX = 0;
    CurY = _usRadius;

    while (CurX <= CurY)
    {
        ST7789_PutPixel(_usX + CurX, _usY + CurY, _usColor);
        ST7789_PutPixel(_usX + CurX, _usY - CurY, _usColor);
        ST7789_PutPixel(_usX - CurX, _usY + CurY, _usColor);
        ST7789_PutPixel(_usX - CurX, _usY - CurY, _usColor);
        ST7789_PutPixel(_usX + CurY, _usY + CurX, _usColor);
        ST7789_PutPixel(_usX + CurY, _usY - CurX, _usColor);
        ST7789_PutPixel(_usX - CurY, _usY + CurX, _usColor);
        ST7789_PutPixel(_usX - CurY, _usY - CurX, _usColor);

        if (D < 0)
        {
            D += (CurX << 2) + 6;
        }
        else
        {
            D += ((CurX - CurY) << 2) + 10;
            CurY--;
        }
        CurX++;
    }
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_DrawBMP
*    功能说明: 在LCD上显示一个BMP位图，位图点阵扫描次序：从左到右，从上到下
*    形    参:  
*            _usX, _usY : 图片的坐标
*            _usHeight  ：图片高度
*            _usWidth   ：图片宽度
*            _ptr       ：图片点阵指针
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_DrawBMP(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t *_ptr)
{
    uint16_t i, k, y;
    const uint16_t *p;

    p = _ptr;
    y = _usY;
    for (i = 0; i < _usHeight; i++)
    {
        for (k = 0; k < _usWidth; k++)
        {
            ST7789_PutPixel(_usX + k, y, *p++);
        }

        y++;
    }
}

/* 形参为 uint8 */
void ST7789_DrawBMP8(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint8_t *_ptr)
{
    uint16_t i, k, y;
    const uint8_t *p;
    uint16_t pixel;

    p = _ptr;
    y = _usY;
    for (i = 0; i < _usHeight; i++)
    {
        for (k = 0; k < _usWidth; k++)
        {
            pixel = (p[0] << 8) + p[1];
            p += 2;
            ST7789_PutPixel(_usX + k, y, pixel);
        }

        y++;
    }
}

/*
*********************************************************************************************************
*    函 数 名: ST7789_SetDirection
*    功能说明: 设置显示屏显示方向（横屏 竖屏）
*    形    参: 显示方向代码 0 横屏正常, 1 = 横屏180度翻转, 2 = 竖屏, 3 = 竖屏180度翻转
*    返 回 值: 无
*********************************************************************************************************
*/
void ST7789_SetDirection(uint8_t _dir)
{
    uint16_t temp;

    /*
        Bit D7-  MY , Page Address Order 
            “0” = Top to Bottom (When MADCTL D7=”0”). 
            “1” = Bottom to Top (When MADCTL D7=”1”). 
        Bit D6-  MX,  Column Address Order 
            “0” = Left to Right (When MADCTL D6=”0”). 
            “1” = Right to Left (When MADCTL D6=”1”). 
        Bit D5-  MV , Page/Column Order 
            “0” = Normal Mode (When MADCTL D5=”0”). 
            “1” = Reverse Mode (When MADCTL D5=”1”) 
        Note: Bits D7 to D5, alse refer to section 8.12 Address Control 
        
        Bit D4- Line Address Order 
            “0” = LCD Refresh Top to Bottom (When MADCTL D4=”0”) 
            “1” = LCD Refresh Bottom to Top (When MADCTL D4=”1”) 
        Bit D3- RGB/BGR Order 
            “0” = RGB (When MADCTL D3=”0”) 
            “1” = BGR (When MADCTL D3=”1”) 
        Bit D2- Display Data Latch Data Order 
            “0” = LCD Refresh Left to Right (When MADCTL D2=”0”) 
            “1” = LCD Refresh Right to Left (When MADCTL D2=”1”) 
    */

    /*
        D5  D6  D7
        MV  MX  MY 
         0  0  0  - 正常 横屏
         0  1  1  - X镜像 Y镜像，    横屏翻转180度
         1  0  1  - X-Y交换，Y镜像，竖屏
         1  1  0  - X-Y交换，X镜像，竖屏180度
    */

    /*
        The address ranges are X=0 to X=239 (Efh) and Y=0 to Y=319 (13Fh).    
    */

    if (_dir == 0 || _dir == 1) /* 横屏， 横屏180度 */
    {
        if (g_LcdWidth < g_LcdHeight)
        {
            temp = g_LcdWidth;
            g_LcdWidth = g_LcdHeight;
            g_LcdHeight = temp;
        }

        if (_dir == 0)
        {
            Lcd_WriteIndex(0x36); /* 扫描方向 */
            Lcd_WriteData((0 << 5) | (0 << 6) | (0 << 7));
        }
        else
        {
            Lcd_WriteIndex(0x36); /* 扫描方向 */
            Lcd_WriteData((0 << 5) | (1 << 6) | (1 << 7));
        }
    }
    else if (_dir == 2 || _dir == 3) /* 竖屏, 竖屏180°*/
    {
        if (g_LcdWidth > g_LcdHeight)
        {
            temp = g_LcdWidth;
            g_LcdWidth = g_LcdHeight;
            g_LcdHeight = temp;
        }

        if (_dir == 3)
        {
            Lcd_WriteIndex(0x36); /* 扫描方向 */
            Lcd_WriteData((1 << 5) | (0 << 6) | (1 << 7));
        }
        else
        {
            Lcd_WriteIndex(0x36); /* 扫描方向 */
            Lcd_WriteData((1 << 5) | (1 << 6) | (0 << 7));
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
