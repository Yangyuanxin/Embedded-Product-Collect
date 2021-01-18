/*
*********************************************************************************************************
*
*    模块名称 : SWIM GPIO驱动程序
*    文件名称 : stm8_swim.c
*    版    本 : V1.0
*    说    明 : STM8 SWIM接口底层驱动函数
*    修改记录 :
*        版本号  日期       作者    说明
*        V1.0    2020-02-08 armfly 
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

/*
    参考文档: UM0470

*/

#include "bsp.h"
#include "stm8_swim.h"
#include "DAP_config.h"
#include "prog_if.h"

#define DEBUG_SWIM_TRIG()
//#define DEBUG_SWIM_TRIG()     DEBUG_D0_TRIG()     /* 调试用，在采样时刻翻转 */

#define CFG_GPIO_OTYPER(GPIOx, pin_bit) GPIOx->OTYPER = GPIOx->OTYPER & (~(0x00000001 << pin_bit))

#define CFG_GPIO_OUT(GPIOx, pin_bit)    GPIOx->MODER = (GPIOx->MODER & (~(0x3U << (pin_bit * 2)))) |  ((0x00000001U & 0x00000003) << (pin_bit * 2));
#define CFG_GPIO_IN(GPIOx, pin_bit)     GPIOx->MODER = GPIOx->MODER & (~(0x3U << (pin_bit * 2)))

#define SWIM_OUT_0()        BSP_SET_GPIO_0(GPIOD, GPIO_PIN_5)
#define SWIM_OUT_1()        BSP_SET_GPIO_1(GPIOD, GPIO_PIN_5)

#define SWIM_IS_HIGH()     (GPIOD->IDR & GPIO_PIN_5)
#define SWIM_IS_LOW()      ((GPIOD->IDR & GPIO_PIN_5) == 0)

#define SWIM_OUT_ENABLE()   BSP_SET_GPIO_1(GPIOI, GPIO_PIN_2); pin_out_init(GPIOD, 5);
#define SWIM_OUT_DISABLE()  BSP_SET_GPIO_0(GPIOI, GPIO_PIN_2); pin_in_init(GPIOD, 5); 

#if 1   /* 比较保守的时序。 上拉电阻2K欧 */
    #define SWIM_0_DELAY_2BIT()     bsp_DelayNS(2 * 125 - (400 - 2 * 125))
    #define SWIM_1_DELAY_2BIT()     bsp_DelayNS(2 * 125 - (456 - 2 * 125))

    #define SWIM_0_DELAY_20BIT()    bsp_DelayNS(20 * 125 - (2684 - 20 * 125)) 
    #define SWIM_1_DELAY_20BIT()    bsp_DelayNS(20 * 125 - (2710 - 20 * 125)) 

    #define SWIM_0_DELAY_8BIT()     bsp_DelayNS(8 * 125 - (1168 - 8 * 125)) 
    #define SWIM_1_DELAY_8BIT()     bsp_DelayNS(8 * 125 - (1230 - 8 * 125)) 

    #define SWIM_DELAY_4BIT()       bsp_DelayNS(4 * 125 - 100) 
    #define SWIM_DELAY_6BIT()       bsp_DelayNS(6 * 125 - 100) 
    #define SWIM_DELAY_8BIT()       bsp_DelayNS(8 * 125 - 100) 
    #define SWIM_DELAY_18BIT()      bsp_DelayNS(18 * 125 - 100) //  - 210) 
#else   /* 加速10% ，不稳定。 需要上拉电阻680欧姆*/
    #define SWIM_0_DELAY_2BIT()     bsp_DelayNS(2 * 125 - (400 - 2 * 125) -30)
    #define SWIM_1_DELAY_2BIT()     bsp_DelayNS(2 * 125 - (456 - 2 * 125) -30)

    #define SWIM_0_DELAY_20BIT()    bsp_DelayNS(20 * 125 - (2684 - 20 * 125)) 
    #define SWIM_1_DELAY_20BIT()    bsp_DelayNS(20 * 125 - (2710 - 20 * 125)) 

    #define SWIM_0_DELAY_8BIT()     bsp_DelayNS(8 * 125 - (1168 - 8 * 125) -50) 
    #define SWIM_1_DELAY_8BIT()     bsp_DelayNS(8 * 125 - (1230 - 8 * 125) -50) 

    #define SWIM_DELAY_4BIT()       bsp_DelayNS(4 * 125 - 100) 
    #define SWIM_DELAY_6BIT()       bsp_DelayNS(6 * 125 - 100) 
    #define SWIM_DELAY_8BIT()       bsp_DelayNS(8 * 125 - 100) 
    #define SWIM_DELAY_18BIT()      bsp_DelayNS(18 * 125 - 100) //  - 210) 

#endif

/* 校验错后重传次数 */
#define SWIM_RETRY_MAX      10

static uint8_t g_HighSpeed = 0;

static void SWIM_SendBit(uint8_t _ucValue);
static void SWIM_SendLastBit(uint8_t _ucValue);
static uint8_t SWIM_ReadBit(void);
static uint8_t SWIM_SendCommand(uint8_t _ucCmd);
static uint8_t SWIM_SendByte(uint8_t _ucData);
static uint8_t SWIM_SendLastByte(uint8_t _ucData);
static uint8_t SWIM_ReciveByte(void);

/*
*********************************************************************************************************
*    函 数 名: SWIM_InitHard
*    功能说明: SWIM接口GPIO硬件初始化
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void SWIM_InitHard(void)
{   
    EIO_D4_Config(ES_GPIO_OUT);         /* RESET - 输出 */
    EIO_D5_Config(ES_GPIO_OUT);         /* RESET - 输出，方便接线 */
    
    EIO_D9_Config(ES_GPIO_OUT);         /* SWIM  - 数据线 */    
    
    g_HighSpeed = 0;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_SetResetPin
*    功能说明: 设置复位脚电平
*    形    参: _state : 电平状态
*    返 回 值: 无
*********************************************************************************************************
*/
void SWIM_SetResetPin(uint8_t _state)
{
    if (_state == 0)
    {
        SWIM_RESET_0();     /* 复位 */
    }
    else
    {
        SWIM_RESET_1();     /* 复位解除 */
    }
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_DetectIC
*    功能说明: 检测IC是否在位
*    形    参: _id 读取4FFC开始的数字，当做芯片识别码
*    返 回 值: 1表示OK  0表示错误
*********************************************************************************************************
*/ 
uint8_t SWIM_DetectIC(uint32_t *_id)
{
    uint8_t ok = 1;
    uint8_t re;    
    
    SWIM_RESET_0();     /* 硬件复位 */
    
    bsp_DelayUS(200);
    
    {
        SWIM_OUT_ENABLE();
        
        /* 低电平持续1.2ms */
        SWIM_OUT_0();
        bsp_DelayUS(1200);
        
        /* 4个1KHz脉冲 */
        SWIM_OUT_1();bsp_DelayUS(500);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(500);
        SWIM_OUT_1();bsp_DelayUS(500);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(500);
        SWIM_OUT_1();bsp_DelayUS(500);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(500);
        SWIM_OUT_1();bsp_DelayUS(500);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(500);
     
        /* 4个2KHz脉冲 */
        SWIM_OUT_1();bsp_DelayUS(250);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(250);
        SWIM_OUT_1();bsp_DelayUS(250);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(250);
        SWIM_OUT_1();bsp_DelayUS(250);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(250);
        SWIM_OUT_1();bsp_DelayUS(250);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(250);

        SWIM_OUT_1();
        SWIM_OUT_DISABLE();
        
        bsp_DelayUS(500);       /* 延迟500us */
    }
    
    DISABLE_INT();
    
    SWIM_OUT_ENABLE();
    
    SWIM_OUT_0();
    bsp_DelayNS(16000);    
    
    SWIM_OUT_1();
    SWIM_OUT_DISABLE();
    
    bsp_DelayNS(4000);
    
    if (SWIM_IS_HIGH())
    {
        ok = 0;
        goto err_quit;
    }
    
    bsp_DelayNS(16000);
        
    if (SWIM_IS_LOW())
    {
        ok = 0;
        goto err_quit;
    }   
    
    ENABLE_INT(); 
    
    bsp_DelayUS(500);       /* 延迟500us */
    
//    g_HighSpeed = 0;        /* 低速模式 */
//        
//    re = SWIM_SendCommand(SWIM_CMD_RESET);       /* 软件复位 */
//    if (re == 0)
//    {
//        goto err_quit;
//    }
//    
//    bsp_DelayUS(500);       /* 延迟200us ST,8L -> 500*/
    
    /* 
        0x7F80 : SWIM control status register (SWIM_CSR)     
    */        
    re = SWIM_WriteByte(0x7F80, 0xA0);
    if (re == 0)
    {
        ok = 0;
        goto err_quit;
    }    
    
    bsp_DelayUS(200);
    
    SWIM_RESET_1();     /* 复位解除 */
    
    if (ok == 1)
    {
        uint8_t buf[4];
        
        g_HighSpeed = 0; 
        
        bsp_DelayMS(5);  
        
        /* 发16us同步脉冲 */
        SWIM_OUT_ENABLE();
        
        SWIM_OUT_0();
        bsp_DelayNS(16000);    
        
        SWIM_OUT_1();
        SWIM_OUT_DISABLE();

        bsp_DelayUS(200);        
        
        re = SWIM_ReadBuf(0x004FFC, buf, 4);
        if (re == 0)
        {
            *_id = 0;
        }
        else
        {
            *_id = BEBufToUint32(buf);
            
            if (*_id == 0)
            {
                *_id = 0x08000;
            }
        }
    }

err_quit:   
    ENABLE_INT(); 
    return ok;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_EntrySequence
*    功能说明: 复位CPU，并激活SWIM
*    形    参: _ucData : 一个字节的数据
*    返 回 值: 1表示OK  0表示错误
*********************************************************************************************************
*/ 
uint8_t SWIM_EntrySequence(void)
{      
    uint8_t re;
    
    SWIM_RESET_0();     /* 硬件复位 */
    
    bsp_DelayUS(200);
    
    SWIM_OUT_ENABLE();
    
    /* 低电平持续1.2ms */
    SWIM_OUT_0();
    bsp_DelayUS(1200);
    
    /* 4个1KHz脉冲 */
    SWIM_OUT_DISABLE();bsp_DelayUS(500);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(500);
    SWIM_OUT_DISABLE();bsp_DelayUS(500);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(500);
    SWIM_OUT_DISABLE();bsp_DelayUS(500);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(500);
    SWIM_OUT_DISABLE();bsp_DelayUS(500);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(500);
 
    /* 4个2KHz脉冲 */
    SWIM_OUT_DISABLE();bsp_DelayUS(250);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(250);
    SWIM_OUT_DISABLE();bsp_DelayUS(250);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(250);
    SWIM_OUT_DISABLE();bsp_DelayUS(250);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(250);
    SWIM_OUT_DISABLE();bsp_DelayUS(250);SWIM_OUT_ENABLE();SWIM_OUT_0();bsp_DelayUS(250);

    SWIM_OUT_1();
    SWIM_OUT_DISABLE();
        
    /* 芯片需要给 16us低电平应答 */
//    while(SWIM_IS_HIGH());  /* 等待变低 */ 
//    
//    while(SWIM_IS_LOW());   /* 等待变高 */ 
    
    bsp_DelayUS(500);       /* 延迟500us */
    
//    g_HighSpeed = 0;        /* 低速模式 */
//        
//    re = SWIM_SendCommand(SWIM_CMD_RESET);       /* 软件复位 */
//    if (re == 0)
//    {
//        goto err_quit;
//    }
//    
//    bsp_DelayUS(500);       /* 延迟200us ST,8L -> 500*/
    
    /* 
        0x7F80 : SWIM control status register (SWIM_CSR)     
    */        
    re = SWIM_WriteByte(0x7F80, 0xA0);
    if (re == 0)
    {
        goto err_quit;
    }    
    
    bsp_DelayUS(200);
    
    SWIM_RESET_1();   

    bsp_DelayMS(20);        /* 延迟20ms */
    
    {
        
        SWIM_ReadByte(0x007F99);
        
        bsp_DelayUS(400);                   /* 延迟400us */
        
        /* 
            0x50CD : SWIM clock control register (CLK_SWIMCCR) 
            0: SWIM clock is divided by 2 (recommended)
            1: SWIM clock is not divided by 2 (not recommended as communication is less reliable)
        */
        re = SWIM_ReadByte(0x50CD);

        bsp_DelayUS(400);                   /* 延迟400us */
        
        SWIM_OUT_ENABLE();
        
        SWIM_OUT_0();
        bsp_DelayNS(16000);    
        
        SWIM_OUT_1();
        SWIM_OUT_DISABLE();

        bsp_DelayUS(200);
        
        SWIM_WriteByte(0x7F80, 0xB0);       /* 切换到速模式 */
        
        g_HighSpeed = 1;                    
        
        bsp_DelayUS(400);                   /* 延迟400us */
        
        SWIM_WriteByte(0x7F80, 0xB4);
        
        bsp_DelayUS(400);                   /* 延迟400us */
        
        SWIM_WriteByte(0x50C6, 0x00);       /* STM8_CLK_CKDIVR = 0x50C6 */
        
        bsp_DelayUS(400);                   /* 延迟400us */
        
//        {
//            uint8_t buf[4];
//            
//            re = SWIM_ReadBuf(0x004FFC, buf, 4);
//            if (re == 0)
//            {
//                goto err_quit;
//            }
//            
//            if (buf[0] == 0)
//            {
//                bsp_DelayUS(400);
//            }
//            
//            SWIM_ReadBuf(0x00505F, buf, 1);
//            
//            SWIM_ReadBuf(0x004800, buf, 1);     /* OPTION BYTES ,第1个字节 71表示读保护 */
//            
//            if (buf[0] != 0)    /* 读保护了 */
//            {
//                SWIM_WriteByte(0x4800, 0x00);
//            }             
//            
//            SWIM_ReadBuf(0x004801, buf, 1);
//            
//            SWIM_ReadBuf(0x004802, buf, 1);

//            if (buf[0] == 0)
//            {
//                bsp_DelayUS(400);
//            }        
//        }    
    }

    return 1;
    
err_quit:
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_WriteBuf
*    功能说明: 连续写多个字节
*    形    参: _Addr : 3个字节的地址。整数值
*              _Buf : 输入数据缓冲区
*              _Len : 字节长度
*    返 回 值: 1表示OK, 0表示出错
*********************************************************************************************************
*/ 
uint8_t SWIM_WriteBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len)
{
    uint16_t i;
    uint8_t ack;
    
    SWIM_SendCommand(SWIM_CMD_WRITE);   /* 写 */    
    SWIM_SendByte(_Len);                /* 1个字节 */   
    SWIM_SendByte(_Addr >> 16);         /* 地址 */
    SWIM_SendByte(_Addr >> 8);
    SWIM_SendByte(_Addr);  
    
    for (i = 0; i < _Len; i++)
    {
        ack = SWIM_SendByte(_Buf[i]);
    }
    return ack;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_WriteByte
*    功能说明: 写1个字节
*    形    参: _Addr : 3个字节的地址。整数值
*              _data : 数据
*    返 回 值: 1表示OK, 0表示出错
*********************************************************************************************************
*/ 
uint8_t SWIM_WriteByte(uint32_t _Addr, uint8_t _data)
{
    return SWIM_WriteBuf(_Addr, &_data, 1);
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_ReadBuf
*    功能说明: 读取连续多个字节数据
*    形    参: _Addr : 3个字节的地址。整数值
*              _Buf : 目标缓冲区
*              _Len : 字节长度
*    返 回 值: 1表示成功，0表示失败
*********************************************************************************************************
*/ 
uint8_t SWIM_ReadBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len)
{
    uint16_t i;
    uint16_t re = 1;
    uint16_t count;
    
    while (_Len)
    {
        if (_Len > 256)
        {
            count = 256;
        }
        else
        {
            count = _Len;
        }        
        _Len -= count;
        
        re = SWIM_SendCommand(SWIM_CMD_READ);   /* 读 */   
        if (re == 0) goto err_quit;;
        
        re = SWIM_SendByte(count);              /* 1个字节 */   
        if (re == 0) goto err_quit;
        
        re = SWIM_SendByte(_Addr >> 16);        /* 地址 */
        if (re == 0) goto err_quit;
        
        re = SWIM_SendByte(_Addr >> 8);
        if (re == 0) goto err_quit;

        re = SWIM_SendLastByte(_Addr);          /* 函数内部有关闭中断 */
        if (re == 0) goto err_quit;        

        for (i = 0; i < count; i++)
        {
            *_Buf++ = SWIM_ReciveByte();
        }
        ENABLE_INT(); 
        
        _Addr += count;
    }
    
err_quit:   
    ENABLE_INT();                           /* 全部接收完毕再打开中断 */
    
    return re;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_ReadByte
*    功能说明: 读取1个字节数据
*    形    参: _Addr : 3个字节的地址。整数值
*    返 回 值: 数据
*********************************************************************************************************
*/ 
uint8_t SWIM_ReadByte(uint32_t _Addr)
{
    uint8_t data;
    
    SWIM_ReadBuf(_Addr, &data, 1);
    
    return data;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_SendBit
*    功能说明: 发送1个bit, 高速模式和低速模式由全局变量g_HighSpeed决定
*    形    参: _ucValue : 0 or 1
*    返 回 值: 无
*********************************************************************************************************
*/
static void SWIM_SendBit(uint8_t _ucValue)
{
    if (g_HighSpeed == 0)
    {    
        if (_ucValue & 1)
        {
            SWIM_OUT_0();
            SWIM_0_DELAY_2BIT();
            SWIM_OUT_1();
            SWIM_1_DELAY_20BIT(); 
        }
        else
        {                
            SWIM_OUT_0();
            SWIM_0_DELAY_20BIT();
            SWIM_OUT_1();
            SWIM_1_DELAY_2BIT();  
        }
    }
    else
    {
        if (_ucValue & 1)
        {
            SWIM_OUT_0();
            SWIM_0_DELAY_2BIT();
            SWIM_OUT_1();
            SWIM_1_DELAY_8BIT();   
        }
        else
        {        
            SWIM_OUT_0();
            SWIM_0_DELAY_8BIT();
            SWIM_OUT_1();
            SWIM_1_DELAY_2BIT();        
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_SendLastBit
*    功能说明: 发送最后1个bit, 发送完毕, GPIO设置为输入状态。高速模式和低速模式由全局变量g_HighSpeed决定
*    形    参: _ucValue : 0 or 1
*    返 回 值: 无
*********************************************************************************************************
*/
static void SWIM_SendLastBit(uint8_t _ucValue)
{
    if (g_HighSpeed == 0)
    {    
        if (_ucValue & 1)
        {
            SWIM_OUT_0();
            SWIM_0_DELAY_2BIT();
            SWIM_OUT_1();SWIM_OUT_DISABLE();
        }
        else
        {                
            SWIM_OUT_0();
            SWIM_0_DELAY_20BIT();
            SWIM_OUT_1();SWIM_OUT_DISABLE();
        }
    }
    else
    {
        if (_ucValue & 1)
        {
            SWIM_OUT_0();
            SWIM_0_DELAY_2BIT();
            SWIM_OUT_1();SWIM_OUT_DISABLE(); 
        }
        else
        {        
            SWIM_OUT_0();
            SWIM_0_DELAY_8BIT();
            SWIM_OUT_1();SWIM_OUT_DISABLE();      
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_ReadBit
*    功能说明: 读取1个bit, 可以自动同步时钟。高速模式和低速模式是完全兼容的。
*    形    参: 无
*    返 回 值: 0 或者 1
*********************************************************************************************************
*/
static uint8_t SWIM_ReadBit(void)
{
    uint8_t bit;
    uint32_t delay = 1000;
    
    while(SWIM_IS_HIGH() && delay--);  /* 等待变低 */
    if (delay == 0)
    {
        return 0;
    }
    
    DEBUG_SWIM_TRIG();            /* 调试语句， D2会输出 */
    
    if (g_HighSpeed == 0)       /* 低速 */
    {
        SWIM_0_DELAY_2BIT();
        if (SWIM_IS_HIGH())
        {
            bit = 1;
        }
        else
        {
            bit = 0;
        }      
    }
    else        /* 高速 */
    {
        SWIM_0_DELAY_2BIT();
        if (SWIM_IS_HIGH())
        {
            bit = 1;
        }
        else
        {
            bit = 0;
        }  
    }  
    delay = 1000;
    while(SWIM_IS_LOW() && delay--);    /* 等待变高 */     
    return bit;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_SendCommand
*    功能说明: 发送CMD帧,5个bit。如果接收到NAK，则重新发送。.
*    形    参: _ucCmd : 0 or 1
*    返 回 值: 1表示OK  0表示错误
*********************************************************************************************************
*/
static const uint8_t SwimParityTable256[256] =  /* 奇偶校验表 */
{
#   define P2(n) n, n^1, n^1, n
#   define P4(n) P2(n), P2(n^1), P2(n^1), P2(n)
#   define P6(n) P4(n), P4(n^1), P4(n^1), P4(n)
    P6(0), P6(1), P6(1), P6(0)
};    
static uint8_t SWIM_SendCommand(uint8_t _ucCmd)
{
    uint8_t pb;
    uint8_t ack;
    uint8_t i;
    
    pb = SwimParityTable256[_ucCmd];
    
    for (i = 0; i < SWIM_RETRY_MAX; i++)
    {
    
        DISABLE_INT();      /*　关闭中断 */
        
        SWIM_OUT_ENABLE();
        
        SWIM_SendBit(0);
        SWIM_SendBit(_ucCmd >> 2);
        SWIM_SendBit(_ucCmd >> 1);
        SWIM_SendBit(_ucCmd >> 0);
        SWIM_SendLastBit(pb);
        
        ack = SWIM_ReadBit();
            
        ENABLE_INT();           /*　打开中断 */
        
        if (ack == 1)
        {
            break;            
        }
        
        SWIM_1_DELAY_20BIT();   /* 延迟20bit重发 */
    }
    return ack;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_SendByte
*    功能说明: 发送字节帧, 10个bit。 如果接收到NAK，则重新发送。.
*    形    参: _ucData : 一个字节的数据
*    返 回 值: 1表示OK  0表示错误
*********************************************************************************************************
*/ 
static uint8_t SWIM_SendByte(uint8_t _ucData)
{
    uint8_t pb;
    uint8_t ack;
    uint8_t i;
    
    pb = SwimParityTable256[_ucData];
    
    for (i = 0; i < SWIM_RETRY_MAX; i++)
    {
        DISABLE_INT();       /*　关闭中断 */
        
        SWIM_OUT_ENABLE();
        
        SWIM_SendBit(0);
        SWIM_SendBit(_ucData >> 7);
        SWIM_SendBit(_ucData >> 6);
        SWIM_SendBit(_ucData >> 5);
        SWIM_SendBit(_ucData >> 4);
        SWIM_SendBit(_ucData >> 3);
        SWIM_SendBit(_ucData >> 2);    
        SWIM_SendBit(_ucData >> 1); 
        SWIM_SendBit(_ucData >> 0);         
        SWIM_SendLastBit(pb);   

        ack = SWIM_ReadBit();

        ENABLE_INT();           /*　打开中断 */

        if (ack == 1)
        {
            break;            
        }
        
        SWIM_1_DELAY_20BIT();   /* 延迟20bit重发 */        
    }
    return ack;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_SendLastByte
*    功能说明: 发送字节帧（读数据时，发送最后1个字节时使用）。 
*              如果接收到NAK，则重新发送。.发送完毕，中断继续保持关闭状态
*    形    参: _ucData : 一个字节的数据
*    返 回 值: 1表示OK  0表示错误
*********************************************************************************************************
*/ 
static uint8_t SWIM_SendLastByte(uint8_t _ucData)
{
    uint8_t pb;
    uint8_t ack;
    uint8_t i;
    
    pb = SwimParityTable256[_ucData];
    
    for (i = 0; i < SWIM_RETRY_MAX; i++)
    {    
        DISABLE_INT();          /*　关闭中断 */
        
        SWIM_OUT_ENABLE();
        
        SWIM_SendBit(0);
        SWIM_SendBit(_ucData >> 7);
        SWIM_SendBit(_ucData >> 6);
        SWIM_SendBit(_ucData >> 5);
        SWIM_SendBit(_ucData >> 4);
        SWIM_SendBit(_ucData >> 3);
        SWIM_SendBit(_ucData >> 2);    
        SWIM_SendBit(_ucData >> 1); 
        SWIM_SendBit(_ucData >> 0);         
        SWIM_SendLastBit(pb);   
        
        ack = SWIM_ReadBit();

        if (ack == 1)
        {
            break;              /* 发送成功，返回 */  
        }       
        
        SWIM_1_DELAY_20BIT();   /* 延迟20bit重发 */ 
    }

    return ack;
}

/*
*********************************************************************************************************
*    函 数 名: SWIM_ReciveByte
*    功能说明: 接收一个字节.
*    形    参: 无
*    返 回 值: 数据（1个字节）
*********************************************************************************************************
*/ 
static uint8_t SWIM_ReciveByte(void)
{
    uint8_t bit[10];
    uint8_t data;
    uint8_t pb;
    uint8_t i;
    
    for (i = 0; i < SWIM_RETRY_MAX; i++)
    {      
        data = 0;
        
        bit[0] = SWIM_ReadBit();
        bit[1] = SWIM_ReadBit();
        bit[2] = SWIM_ReadBit();
        bit[3] = SWIM_ReadBit();
        bit[4] = SWIM_ReadBit();
        bit[5] = SWIM_ReadBit();
        bit[6] = SWIM_ReadBit();
        bit[7] = SWIM_ReadBit();
        bit[8] = SWIM_ReadBit();
        bit[9] = SWIM_ReadBit();    
            
        data = (bit[1] << 7) + (bit[2] << 6) + (bit[3] << 5) + (bit[4] << 4)
                 + (bit[5] << 3) + (bit[6] << 2) + (bit[7] << 1) + (bit[8] << 0);
        pb = SwimParityTable256[data];
        
        if (bit[0] == 1 && bit[9] == pb)
        {
            bsp_DelayNS(1000);    
            SWIM_OUT_ENABLE();
            
            SWIM_SendLastBit(1);        /* 应答ACK */
            break;
        }
        else
        {
            bsp_DelayNS(500);    
            SWIM_OUT_ENABLE();
            
            SWIM_SendLastBit(0);        /* 应答NAK, 等待重发 */
        }
    }
    
    if (i == SWIM_RETRY_MAX)
    {
        data = 0;       /* 出错 */
    }
    
    return data;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
