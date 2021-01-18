/*
*********************************************************************************************************
*
*    模块名称 : BSP模块(For STM32H7)
*    文件名称 : bsp.c
*    版    本 : V1.0
*    说    明 : 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h" 来包含所有的外设驱动模块。
*               bsp = Borad surport packet 板级支持包
*    修改记录 :
*        版本号  日期         作者       说明
*        V1.0    2018-07-29  Eric2013   正式发布
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "nvic_prio_cfg.h"

static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
static void MPU_Config(void);

uint8_t s_D0State = 2;
uint8_t s_D2State = 2;

/*
*********************************************************************************************************
*	函 数 名: System_Init
*	功能说明: 系统初始化，主要是MPU，Cache和系统时钟配置
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void System_Init(void)
{
	
	/* 配置MPU */
	MPU_Config();
	
	/* 使能L1 Cache */
	CPU_CACHE_Enable();

	/* 
       STM32H7xx HAL 库初始化，此时系统用的还是H7自带的64MHz，HSI时钟:
	   - 调用函数HAL_InitTick，初始化滴答时钟中断1ms。
	   - 设置NVIV优先级分组为4。
	 */
	HAL_Init();

	/* 
       配置系统时钟到400MHz
       - 切换使用HSE。
       - 此函数会更新全局变量SystemCoreClock，并重新配置HAL_InitTick。
    */
	SystemClock_Config();

	/* 
	   Event Recorder：
	   - 可用于代码执行时间测量，MDK5.25及其以上版本才支持，IAR不支持。
	   - 默认不开启，如果要使能此选项，务必看V7开发板用户手册第8章
	*/	
#if Enable_EventRecorder == 1  
	/* 初始化EventRecorder并开启 */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif

}

/*
*********************************************************************************************************
*    函 数 名: bsp_Init
*    功能说明: 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。只需要调用一次
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
#include "param.h"
void bsp_Init(void)
{
	System_Init();

    bsp_InitKey();     /* 按键初始化，要放在滴答定时器之前，因为按钮检测是通过滴答定时器扫描 */
    bsp_InitTimer();    /* 初始化滴答定时器 */

    ENABLE_INT();

    PERIOD_InitVar();

    bsp_InitUart();     /* 初始化串口 */
    bsp_InitLed();      /* 初始化LED */
    bsp_InitI2C();      /* 初始化I2C总线 */
    bsp_InitSPIBus();   /* 初始化SPI总线 */
                                        //    bsp_InitSFlash();    /* 识别串行flash W25Q64 */

    BEEP_InitHard();
    ee_CheckOk();       /* 检测EEPROM */

    HC595_InitHard();   /* 配置示波器模块上的GPIO芯片 */

    bsp_InitDAC1();     /* 配置DAC引脚 */
   //bsp_InitTVCC();     /* TVCC控制引脚 -- 放到后面读完参数后设置 */

    bsp_InitMCP4725();  /* 示波器偏置电压 */

    bsp_InitExtIO();    /* 输出端口初始化 */
    s_D0State = 2;      /* 用于调试, DEBUG_D0_TRIG()  */
    s_D2State = 2;      /* 用于调试, DEBUG_D2_TRIG()  */

    LCD_InitHard();

    bsp_InitQSPI_W25Q256(); /* 初始化QSPI */

    bsp_InitRTC();      /* 初始化时钟 */
    
    bsp_InitRNG();      /* 配置启用随机数模块 */
    
    HAL_NVIC_SetPriority(SysTick_IRQn, SysTick_IRQ_PRIO, 0U);  /* 重置SysTick优先级最高 */
}


/*
*********************************************************************************************************
*    函 数 名: bsp_DeInit
*    功能说明: 禁能所有的硬件设备
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_DeInit(void)
{
    bsp_DeInitUart();   
}

/*
*********************************************************************************************************
*	函 数 名: SystemClock_Config
*	功能说明: 初始化系统时钟
*            	System Clock source            = PLL (HSE)
*            	SYSCLK(Hz)                     = 400000000 (CPU Clock)
*           	HCLK(Hz)                       = 200000000 (AXI and AHBs Clock)
*            	AHB Prescaler                  = 2
*            	D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
*            	D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
*            	D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
*            	D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
*            	HSE Frequency(Hz)              = 25000000
*           	PLL_M                          = 5
*            	PLL_N                          = 160
*            	PLL_P                          = 2
*            	PLL_Q                          = 4
*            	PLL_R                          = 2
*            	VDD(V)                         = 3.3
*            	Flash Latency(WS)              = 4
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    HAL_StatusTypeDef ret = HAL_OK;

    /* 锁住SCU(Supply configuration update) */
    MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

    /* 
      1、芯片内部的LDO稳压器输出的电压范围，可选VOS1，VOS2和VOS3，不同范围对应不同的Flash读速度，
         详情看参考手册的Table 12的表格。
        //      2、这里选择使用VOS1，电压范围1.15V - 1.26V。
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /* Enable D2 domain SRAM3 Clock (0x30040000 AXI)*/
    __HAL_RCC_D2SRAM3_CLK_ENABLE();

    //    /* Macro to configure the PLL clock source  */
    //    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

    /* 使能HSE，并选择HSE作为PLL时钟源 */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    /* PLL3-Q for USB Clock = 48M */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.PLL3.PLL3M = 5;
    PeriphClkInitStruct.PLL3.PLL3N = 48;
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 5;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* PLL3-R for LTDC */
    //    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    //    PeriphClkInitStruct.PLL3.PLL3M = 25;
    //    PeriphClkInitStruct.PLL3.PLL3N = 160;
    //    PeriphClkInitStruct.PLL3.PLL3P = 2;
    //    PeriphClkInitStruct.PLL3.PLL3Q = 2;
    //    PeriphClkInitStruct.PLL3.PLL3R = 32;
    //    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    //    /* Disable  PLL3. */
    //    __HAL_RCC_PLL3_DISABLE();

    /* 
       选择PLL的输出作为系统时钟
       配置RCC_CLOCKTYPE_SYSCLK系统时钟
       配置RCC_CLOCKTYPE_HCLK 时钟，对应AHB1，AHB2，AHB3和AHB4总线
       配置RCC_CLOCKTYPE_PCLK1时钟，对应APB1总线
       配置RCC_CLOCKTYPE_PCLK2时钟，对应APB2总线
       配置RCC_CLOCKTYPE_D1PCLK1时钟，对应APB3总线
       配置RCC_CLOCKTYPE_D3PCLK1时钟，对应APB4总线     
    */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 |
                                                                 RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1);

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    /* 此函数会更新SystemCoreClock，并重新配置HAL_InitTick */
    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
    if (ret != HAL_OK)
    {
        Error_Handler(__FILE__, __LINE__);
    }
    
    /*
      使用IO的高速模式，要使能IO补偿，即调用下面三个函数 
      （1）使能CSI clock
      （2）使能SYSCFG clock
      （3）使能I/O补偿单元， 设置SYSCFG_CCCSR寄存器的bit0
    */
    __HAL_RCC_CSI_ENABLE();

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_EnableCompensationCell();

    /* AXI SRAM的时钟是上电自动使能的，而D2域的SRAM1，SRAM2和SRAM3要单独使能 */	
    #if 1
    __HAL_RCC_D2SRAM1_CLK_ENABLE();
    __HAL_RCC_D2SRAM2_CLK_ENABLE();
    __HAL_RCC_D2SRAM3_CLK_ENABLE();

    __HAL_RCC_BKPRAM_CLKAM_ENABLE();       
    __HAL_RCC_D3SRAM1_CLKAM_ENABLE();
    #endif    
}

/*
*********************************************************************************************************
*    函 数 名: Error_Handler
*    形    参: file : 源代码文件名称。关键字 __FILE__ 表示源代码文件名。
*              line ：代码行号。关键字 __LINE__ 表示源代码行号
*    返 回 值: 无
*        Error_Handler(__FILE__, __LINE__);
*********************************************************************************************************
*/
void Error_Handler(char *file, uint32_t line)
{
    /* 
        用户可以添加自己的代码报告源代码文件名和代码行号，比如将错误文件和行号打印到串口
        printf("Wrong parameters value: file %s on line %d\r\n", file, line) 
    */

    /* 这是一个死循环，断言失败时程序会在此处死机，以便于用户查错 */
    if (line == 0)
    {
        return;
    }

    while (1)
    {
    }
}

/*
*********************************************************************************************************
*    函 数 名: MPU_Config
*    功能说明: 配置MPU
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    /* 禁止 MPU */
    HAL_MPU_Disable();

#if 1
    /* Configure the MPU attributes as Device not cacheable 
     for ETH DMA descriptors */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x30040000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_256B;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* Configure the MPU attributes as Cacheable write through 
     for LwIP RAM heap which contains the Tx buffers */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x30044000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER1;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
#endif

    /* 配置AXI SRAM的MPU属性为Write through */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x24000000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    //MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
    MPU_InitStruct.Number = MPU_REGION_NUMBER2;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* 配置显存IO空间的属性为Write through */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = 0x30000000;
	MPU_InitStruct.Size             = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;     /* 不要CASHE */
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER3;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /* 配置FMC IO空间的属性为Write through */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = 0x60000000;
	MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_32B;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;     /* 不要CASHE */
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER4;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
//    uint16_t *aADCH1ConvertedData = (uint16_t *)0x38000000;
//    uint16_t *aADCH2ConvertedData = (uint16_t *)(0x38000000 + 32 * 1024);

//    /* 0x20000000  128KB */
//    float *g_Ch1WaveBuf = (float *)0x20000000;
//    float *g_Ch2WaveBuf = (float *)(0x20000000 + 64 * 1024);    
    /* 配置ADC 空间的属性为Write through */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = 0x38000000;
	MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_64KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;     /* 不要CASHE */
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER5;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = 0x20000000;
	MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_128KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;     /* 不要CASHE */
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER6;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
	HAL_MPU_ConfigRegion(&MPU_InitStruct);
    
    /*使能 MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/*
*********************************************************************************************************
*    函 数 名: CPU_CACHE_Enable
*    功能说明: 使能L1 Cache
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void CPU_CACHE_Enable(void)
{
    /* 使能 I-Cache */
    SCB_EnableICache();

    /* 使能 D-Cache */
    SCB_EnableDCache();
}

/*
*********************************************************************************************************
*    函 数 名: bsp_GetCpuID
*    功能说明: 该CPU UID
*    形    参: _id : 返回ID
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_GetCpuID(uint32_t *_id)
{
    _id[0] = *(__IO uint32_t *)(0x1FF1E800);
    _id[1] = *(__IO uint32_t *)(0x1FF1E800 + 4);
    _id[2] = *(__IO uint32_t *)(0x1FF1E800 + 8);
}

/*
*********************************************************************************************************
*    函 数 名: bsp_RunPer10ms
*    功能说明: 该函数每隔10ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些处理时间要求不严格的
*            任务可以放在此函数。比如：按键扫描、蜂鸣器鸣叫控制等。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer10ms(void)
{
    bsp_KeyScan10ms();

    BEEP_Pro(); /* 蜂鸣器任务 */

    PERIOD_Scan(); /* 控制LED闪烁 */

    bsp_AdcTask10ms(); /* ADC后台任务 */
}

/*
*********************************************************************************************************
*    函 数 名: bsp_RunPer1ms
*    功能说明: 该函数每隔1ms被Systick中断调用1次。详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理的事务
*             可以放在此函数。比如：触摸坐标扫描。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_RunPer1ms(void)
{
    ;
}

/*
*********************************************************************************************************
*    函 数 名: bsp_Idle
*    功能说明: 空闲时执行的函数。一般主程序在for和while循环程序体中需要插入 CPU_IDLE() 宏来调用本函数。
*             本函数缺省为空操作。用户可以添加喂狗、设置CPU进入休眠模式的功能。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void lwip_pro(void);
extern void lua_Poll(void);
extern void wifi_task(void);
extern void EXIO_ScanTask(void);
extern void LCD_Task(void);
extern void PC_CmdTask(void);
extern void DelayOpenUSBTask(void);
void bsp_Idle(void)
{
    /* --- 喂狗 */

    lwip_pro();             /* 以太网协议栈轮询 */

    wifi_task();

    lua_Poll(); 
    
    EXIO_ScanTask();        /* 扩展IO任务 */
    
    LCD_Task();    			/* 显示屏任务，硬件SPI+DMA+刷屏 */   
    
    PC_CmdTask();           /* PC机控制任务 */
    
    DelayOpenUSBTask();     /* 延迟启动USB */
}

/*
*********************************************************************************************************
*    函 数 名: HAL_Delay
*    功能说明: 毫秒延迟函数。替换HAL中的函数。因为HAL中的缺省函数依赖于systick中断，如果在USB、SD卡中断中
*        有延迟函数，则会锁死。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void HAL_Delay(uint32_t Delay)
{
    bsp_DelayUS(Delay * 1000);
}    

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
