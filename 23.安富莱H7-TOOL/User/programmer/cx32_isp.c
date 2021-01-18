/*
*********************************************************************************************************
*
*    模块名称 : CX32 ISP编程驱动
*    文件名称 : cx32_isp.c
*    版    本 : V1.0
*    说    明 : 恒烁CX32L003的SWD保护后，需要特殊的ISP时序才能解锁。
*    修改记录 :
*        版本号  日期       作者    说明
*        V1.0    2020-10-09 armfly  原创
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "prog_if.h"
#include "DAP_config.h"
#include "SW_DP_Multi.h"

/* 
    单路模式 
    ICP_CLK = PD3
    ICP_DAT = PD4
    
    ICP_RST = PE4
*/
    #define ICP_CLK_0()         BSP_SET_GPIO_0(GPIOD, GPIO_PIN_3)
    #define ICP_CLK_1()         BSP_SET_GPIO_1(GPIOD, GPIO_PIN_3)

    #define ICP_DAT_0()         BSP_SET_GPIO_0(GPIOD, GPIO_PIN_4)
    #define ICP_DAT_1()         BSP_SET_GPIO_1(GPIOD, GPIO_PIN_4)

    #define ICP_RST_0()         BSP_SET_GPIO_0(GPIOE, GPIO_PIN_4)
    #define ICP_RST_1()         BSP_SET_GPIO_1(GPIOE, GPIO_PIN_4)

    /* PG9 = 0 切换为输入方向  */    
    #define ICP_DAT_OUT_ENABLE()   BSP_SET_GPIO_1(GPIOG, GPIO_PIN_9); pin_out_init(GPIOD, 4);
    #define ICP_DAT_OUT_DISABLE()  BSP_SET_GPIO_0(GPIOG, GPIO_PIN_9); pin_in_init(GPIOD, 4); 

    #define ICP_DAT_IS_HIGH()     (GPIOD->IDR & GPIO_PIN_4)
    #define ICP_DAT_IS_LOW()      ((GPIOD->IDR & GPIO_PIN_4) == 0)

    /* PD10 = 0 切换为输入方向  */    
    #define ICP_CLK_OUT_ENABLE()   BSP_SET_GPIO_1(GPIOD, GPIO_PIN_10); pin_out_init(GPIOD, 3);
    #define ICP_CLK_OUT_DISABLE()  BSP_SET_GPIO_0(GPIOD, GPIO_PIN_10); pin_in_init(GPIOD, 3); 

    #define ICP_CLK_IS_HIGH()     (GPIOD->IDR & GPIO_PIN_3)
    #define ICP_CLK_IS_LOW()      ((GPIOD->IDR & GPIO_PIN_3) == 0)

/* 
    多路模式 
    D0  PI0 PD14 PA15    - DIR PH8       RESET 共用
    D1  PD15 PA8 PH19    - DIR PG8       外部触发编程
    
    D2  PE6  PD0 PB7     - DIR PD9       SWCLK_4
    D3  PE5  PD1 PH11    - DIR PG10      SWDIO_4
    
    D6  PE9  PD3 PA0     - DIR PD10      SWCLK_3    
    D4  PE4  PE7 PH12    - DIR PG12      SWDIO_3
    
    D7  PE10 PI6         - DIR PI1       SWCLK_2    
    D5  PE2  PE8 PI5     - DIR PG7       SWDIO_2
    
    D9  PE12 PD5         - DIR PI12      SWCLK_1    
    D8  PE11 PD4  PI3    - DIR PG9       SWDIO_1     
*/
    #define MUL_ICP_RST_1()         BSP_SET_GPIO_0(GPIOI, GPIO_PIN_0)       /* 转接板有三极管反相 */
    #define MUL_ICP_RST_0()         BSP_SET_GPIO_1(GPIOI, GPIO_PIN_0)
    
    #define MUL_ICP_CLK_0()         BSP_SET_GPIO_0(GPIOE, g_ICP_CLK_PIN)
    #define MUL_ICP_CLK_1()         BSP_SET_GPIO_1(GPIOE, g_ICP_CLK_PIN)    
    
    #define MUL_ICP_DAT_0()         BSP_SET_GPIO_0(GPIOE, g_ICP_DAT_PIN)
    #define MUL_ICP_DAT_1()         BSP_SET_GPIO_1(GPIOE, g_ICP_DAT_PIN)

    #define MUL_ICP_DAT_OUT_ENABLE()   BSP_SET_GPIO_1(GPIOG, g_ICP_DAT_DIR_PIN); BSP_CFG_GPIO_OUT(GPIOE, g_ICP_DAT_PIN);
    #define MUL_ICP_DAT_OUT_DISABLE()  BSP_SET_GPIO_0(GPIOG, g_ICP_DAT_DIR_PIN); BSP_CFG_GPIO_IN(GPIOE, g_ICP_DAT_PIN); 

    #define MUL_ICP_DAT0_IS_HIGH()     (GPIOE->IDR & GPIO_PIN_11)
    #define MUL_ICP_DAT0_IS_LOW()      ((GPIOE->IDR & GPIO_PIN_11) == 0)
    #define MUL_ICP_DAT1_IS_HIGH()     (GPIOE->IDR & GPIO_PIN_2)
    #define MUL_ICP_DAT1_IS_LOW()      ((GPIOE->IDR & GPIO_PIN_2) == 0)
    #define MUL_ICP_DAT2_IS_HIGH()     (GPIOE->IDR & GPIO_PIN_4)
    #define MUL_ICP_DAT2_IS_LOW()      ((GPIOE->IDR & GPIO_PIN_4) == 0)
    #define MUL_ICP_DAT3_IS_HIGH()     (GPIOE->IDR & GPIO_PIN_5)
    #define MUL_ICP_DAT4_IS_LOW()      ((GPIOE->IDR & GPIO_PIN_5) == 0)    
 
    #define MUL_ICP_CLK_OUT_ENABLE()   BSP_SET_GPIO_1(GPIOI, g_ICP_CLK_DIR_PIN_I); BSP_SET_GPIO_1(GPIOD, g_ICP_CLK_DIR_PIN_D); BSP_CFG_GPIO_OUT(GPIOE, g_ICP_CLK_PIN);
    #define MUL_ICP_CLK_OUT_DISABLE()  BSP_SET_GPIO_0(GPIOI, g_ICP_CLK_DIR_PIN_I); BSP_SET_GPIO_0(GPIOD, g_ICP_CLK_DIR_PIN_D); BSP_CFG_GPIO_IN(GPIOE, g_ICP_CLK_PIN);

    #define MUL_ICP_CLK0_IS_HIGH()     (GPIOE->IDR & GPIO_PIN_12)
    #define MUL_ICP_CLK0_IS_LOW()      ((GPIOE->IDR & GPIO_PIN_12) == 0)
    #define MUL_ICP_CLK1_IS_HIGH()     (GPIOE->IDR & GPIO_PIN_10)
    #define MUL_ICP_CLK1_IS_LOW()      ((GPIOE->IDR & GPIO_PIN_10) == 0)
    #define MUL_ICP_CLK2_IS_HIGH()     (GPIOE->IDR & GPIO_PIN_9)
    #define MUL_ICP_CLK2_IS_LOW()      ((GPIOE->IDR & GPIO_PIN_9) == 0)
    #define MUL_ICP_CLK3_IS_HIGH()     (GPIOE->IDR & GPIO_PIN_6)
    #define MUL_ICP_CLK3_IS_LOW()      ((GPIOE->IDR & GPIO_PIN_6) == 0)    

static void CX32_SendBit16(uint16_t _data);
static void CX32_SendUart9600(uint8_t _data);

uint32_t g_ICP_CLK_PIN;
uint32_t g_ICP_DAT_PIN;

uint32_t g_ICP_DAT_DIR_PIN;

uint32_t g_ICP_CLK_DIR_PIN_I;
uint32_t g_ICP_CLK_DIR_PIN_D; 

/*
*********************************************************************************************************
*    函 数 名: CX32_InitHard
*    功能说明: ICP接口GPIO硬件初始化
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void CX32_InitHard(void)
{   
    if (g_gMulSwd.MultiMode == 0)   /* 单路烧录 */
    {        
        EIO_D4_Config(ES_GPIO_OUT);         /* ICP_REST - 复位输出 */
        
        EIO_D8_Config(ES_GPIO_OUT);         /* ICP_DAT  - 数据线 */    
        
        EIO_D6_Config(ES_GPIO_OUT);         /* ICP_CLK  - 数据线 */  

        ICP_RST_1();
        ICP_DAT_1();
        ICP_CLK_1();
    }
    else      /* 多路烧录 */
    {
        EIO_D0_Config(ES_GPIO_OUT);         /* reset */
        EIO_D1_Config(ES_GPIO_IN);          /* 输入 */
      
        g_ICP_CLK_PIN = 0;
        g_ICP_DAT_PIN = 0;
        g_ICP_DAT_DIR_PIN = 0;
        g_ICP_CLK_DIR_PIN_I = 0;
        g_ICP_CLK_DIR_PIN_D = 0; 
        
        if (g_gMulSwd.Active[0] == 1)
        {
            EIO_D8_Config(ES_GPIO_SWD_OUT);     /* 用FMC口线做GPIO。因此FMC功能失效 */
            EIO_D9_Config(ES_GPIO_SWD_OUT);     /* 用FMC口线做GPIO。因此FMC功能失效 */            
            
            g_ICP_CLK_PIN |= GPIO_PIN_12;
            g_ICP_DAT_PIN |= GPIO_PIN_11;
            g_ICP_DAT_DIR_PIN |= GPIO_PIN_9;
            g_ICP_CLK_DIR_PIN_I |= GPIO_PIN_12;
        }
        if (g_gMulSwd.Active[1] == 1)
        {
            EIO_D5_Config(ES_GPIO_OUT);
            EIO_D7_Config(ES_GPIO_SWD_OUT);     /* 用FMC口线做GPIO。因此FMC功能失效 */
            
            g_ICP_CLK_PIN |= GPIO_PIN_10;
            g_ICP_DAT_PIN |= GPIO_PIN_2;
            g_ICP_DAT_DIR_PIN |= GPIO_PIN_7;
            g_ICP_CLK_DIR_PIN_I |= GPIO_PIN_1;
            
        }
        if (g_gMulSwd.Active[2] == 1)
        {
            EIO_D4_Config(ES_GPIO_OUT);    
            EIO_D6_Config(ES_GPIO_SWD_OUT);     /* 用FMC口线做GPIO。因此FMC功能失效 */
            
            g_ICP_CLK_PIN |= GPIO_PIN_9;
            g_ICP_DAT_PIN |= GPIO_PIN_4;
            g_ICP_DAT_DIR_PIN |= GPIO_PIN_12;
            g_ICP_CLK_DIR_PIN_D |= GPIO_PIN_10;
        }
        if (g_gMulSwd.Active[3] == 1)
        {
            EIO_D2_Config(ES_GPIO_OUT);
            EIO_D3_Config(ES_GPIO_OUT);
            
            g_ICP_CLK_PIN |= GPIO_PIN_6;
            g_ICP_DAT_PIN |= GPIO_PIN_5;
            g_ICP_DAT_DIR_PIN |= GPIO_PIN_10;
            g_ICP_CLK_DIR_PIN_D |= GPIO_PIN_9;
        } 
    }
}

/*
*********************************************************************************************************
*    函 数 名: CX32_SetResetPin
*    功能说明: 设置复位脚电平
*    形    参: _state : 电平状态
*    返 回 值: 无
*********************************************************************************************************
*/
void CX32_SetResetPin(uint8_t _state)
{
    if (g_gMulSwd.MultiMode == 0)   /* 单路烧录 */
    {    
        if (_state == 0)
        {
            ICP_RST_0();
        }
        else
        {
            ICP_RST_1();
        }
    }
    else      /* 多路烧录 */
    {
        if (_state == 0)
        {
            MUL_ICP_RST_0();
        }
        else
        {
            MUL_ICP_RST_1();
        }        
    }
}

/*
*********************************************************************************************************
*    函 数 名: CX32_EnterIAP
*    功能说明: 进入IAP状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void CX32_EnterIAP(void)
{
    if (g_gMulSwd.MultiMode == 0)   /* 单路烧录 */
    {
        CX32_InitHard();
        
        ICP_DAT_OUT_ENABLE();
        ICP_DAT_1();
        
        ICP_RST_0();
        
        ICP_CLK_0();
        bsp_DelayUS(200);   /* 延迟200us */
        
        /* 高位bit先传: 1011 0101 1100 1001   1011 0101 1100 1001 */
        CX32_SendBit16(0xB5C9);
        bsp_DelayUS(10);
        CX32_SendBit16(0xB5C9);
     
        bsp_DelayUS(100);   /* 延迟100us */
        
        ICP_DAT_1();
        
        ICP_RST_1(); 

        bsp_DelayUS(1000);  /* 延迟1ms */
        
        ICP_CLK_OUT_DISABLE();  /* 切换为输入 */
    }
    else      /* 多路烧录 */
    {
        CX32_InitHard();        
        
        MUL_ICP_DAT_OUT_ENABLE();
        MUL_ICP_DAT_1();
        
        MUL_ICP_RST_0();
        
        MUL_ICP_CLK_0();
        bsp_DelayUS(200);   /* 延迟200us */
        
        /* 高位bit先传: 1011 0101 1100 1001   1011 0101 1100 1001 */
        CX32_SendBit16(0xB5C9);
        bsp_DelayUS(10);
        CX32_SendBit16(0xB5C9);
     
        bsp_DelayUS(100);   /* 延迟100us */
        
        MUL_ICP_DAT_1();
        
        MUL_ICP_RST_1();

        MUL_ICP_CLK_OUT_DISABLE();  /* 切换为输入 */

        bsp_DelayUS(1000);  /* 延迟1ms */        
    }
}

/*
*********************************************************************************************************
*    函 数 名: CX32_ExitIAP
*    功能说明: 退出IAP状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void CX32_ExitIAP(void)
{
    if (g_gMulSwd.MultiMode == 0)   /* 单路烧录 */
    {
        bsp_DelayUS(1000);  /* 延迟1ms */
        
        ICP_RST_0();  
        
        bsp_DelayUS(5000);  /* 延迟5ms */
        
        ICP_CLK_OUT_ENABLE();
        ICP_DAT_1();
        ICP_CLK_1();
        ICP_RST_1();

        bsp_DelayUS(1000);  /* 延迟1ms */
    }
    else      /* 多路烧录 */
    {
        bsp_DelayUS(1000);  /* 延迟1ms */
        
        MUL_ICP_RST_0();  
        
        bsp_DelayUS(5000);  /* 延迟5ms */
        
        MUL_ICP_CLK_OUT_ENABLE();
        MUL_ICP_DAT_1();
        MUL_ICP_CLK_1();
        MUL_ICP_RST_1();

        bsp_DelayUS(1000);  /* 延迟1ms */
    }
}

/*
*********************************************************************************************************
*    函 数 名: CX32_SendUart9600
*    功能说明: 传输8bit, 9600 模拟UART. bit0先传
*    形    参: _data：数据
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void CX32_SendUart9600(uint8_t _data)
{
    uint8_t i;
    
    if (g_gMulSwd.MultiMode == 0)   /* 单路烧录 */
    {
        ICP_DAT_0();
        bsp_DelayUS(104); 
        for (i = 0; i < 8; i++)
        {
            if (_data & 0x01)
            {
                ICP_DAT_1();
            }
            else
            {
                ICP_DAT_0();
            }
            _data >>= 1;
            bsp_DelayUS(104);      
        }
        ICP_DAT_1();
        bsp_DelayUS(300);
    }
    else      /* 多路烧录 */
    {
        MUL_ICP_DAT_0();
        bsp_DelayUS(104); 
        for (i = 0; i < 8; i++)
        {
            if (_data & 0x01)
            {
                MUL_ICP_DAT_1();
            }
            else
            {
                MUL_ICP_DAT_0();
            }
            _data >>= 1;
            bsp_DelayUS(104);      
        }
        MUL_ICP_DAT_1();
        bsp_DelayUS(300);
    } 
}

/*
*********************************************************************************************************
*    函 数 名: CX32_SendBit16
*    功能说明: 传输16bit, 低速模式, 用于进入ISP状态用。必须小于1MHz时钟
*    形    参: _data：数据
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void CX32_SendBit16(uint16_t _data)
{
    uint8_t i;
    
    if (g_gMulSwd.MultiMode == 0)   /* 单路烧录 */
    {
        for (i = 0; i < 16; i++)
        {
            if (_data & 0x8000)
            {
                ICP_DAT_1();
            }
            else
            {
                ICP_DAT_0();
            }
            bsp_DelayNS(1000);
            _data <<= 1;
            ICP_CLK_1();        
            bsp_DelayNS(1000);
            ICP_CLK_0();        
        }
    }
    else      /* 多路烧录 */
    {
        for (i = 0; i < 16; i++)
        {
            if (_data & 0x8000)
            {
                MUL_ICP_DAT_1();
            }
            else
            {
                MUL_ICP_DAT_0();
            }
            bsp_DelayNS(1000);
            _data <<= 1;
            MUL_ICP_CLK_1();        
            bsp_DelayNS(1000);
            MUL_ICP_CLK_0();        
        }        
    }
}

/*
*********************************************************************************************************
*    函 数 名: CX32_RemoveSwdLock
*    功能说明: 解锁指令
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void CX32_RemoveSwdLock(void)
{
    /*擦除发 EA 00 EA, 解除SWD 发CF 00 CF */
//    int32_t time1;

    /* 进入ISP模式, 内部延迟了 1ms */
    CX32_EnterIAP();

    DISABLE_INT();
    CX32_SendUart9600(0xCF);
    CX32_SendUart9600(0x00);
    CX32_SendUart9600(0xCF);
    ENABLE_INT();
    
    #if 1
        bsp_DelayUS(5000);
    #else
    /* 单路模式可以正确检测到ISP指令的应答字节0xA0 
    多磨模式不正确，发往命令就检测到口线为低， 原因待查
    */
    
        /* 等待MCU应答0xA0 */
        if (g_gMulSwd.MultiMode == 0)   /* 单路烧录 */
        {
            time1 = bsp_GetRunTime();
            while(1)
            {
                if (ICP_CLK_IS_LOW())
                {
                    break;                
                }
                if (bsp_CheckRunTime(time1) > 50)
                {
                    break;
                }
            }
        }
        else
        {
            uint16_t ok = 0;
            uint16_t done[5] = {0x00, 0x01, 0x03, 0x07, 0x0F};
            
            time1 = bsp_GetRunTime();
            while(1)
            {
                if (bsp_CheckRunTime(time1) > 50)
                {
                    break;
                }
                
                if (g_gMulSwd.MultiMode >= 1)
                {
                    if ((ok & 0x01) == 0)
                    {
                        if (MUL_ICP_CLK0_IS_LOW())
                        {
                            ok |= 0x01;
                        }            
                    }
                }

                if (g_gMulSwd.MultiMode >= 2)
                {
                    if ((ok & 0x02) == 0)
                    {
                        if (MUL_ICP_CLK1_IS_LOW())
                        {
                            ok |= 0x02;                
                        }
                    }
                }

                if (g_gMulSwd.MultiMode >= 3)
                {
                    if ((ok & 0x04) == 0)
                    {
                        if (MUL_ICP_CLK2_IS_LOW())
                        {
                            ok |= 0x04;                
                        }
                    }
                }             

                if (g_gMulSwd.MultiMode >= 3)
                {
                    if ((ok & 0x08) == 0)
                    {
                        if (MUL_ICP_CLK3_IS_LOW())
                        {
                            ok |= 0x08;                
                        }
                    }
                }
                
                if (ok == done[g_gMulSwd.MultiMode])
                {
                    break;
                }
            }
        }
    }
        
    bsp_DelayUS(2000);
    #endif

    CX32_ExitIAP();
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
