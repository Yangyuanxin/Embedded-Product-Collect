/*
*********************************************************************************************************
*
*    模块名称 : 新唐N76E003编程驱动
*    文件名称 : n76e003_flash.c
*    版    本 : V1.0
*    说    明 : 新唐N76E003编程驱动 (仅支持单路模式)
*    修改记录 :
*        版本号  日期       作者    说明
*        V1.0    2020-09-26 armfly  原创
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "prog_if.h"
#include "DAP_config.h"
#include "n76e003_flash.h"
#include "stm8_flash.h"

/*

    ICP_CLK = PD3
    ICP_DAT = PD4
    
    ICP_RST = PE4

*/

#define DEBUG_SWIM_TRIG()
//#define DEBUG_SWIM_TRIG()     DEBUG_D0_TRIG()     /* 调试用，在采样时刻翻转 */

#define CFG_GPIO_OTYPER(GPIOx, pin_bit) GPIOx->OTYPER = GPIOx->OTYPER & (~(0x00000001 << pin_bit))

#define CFG_GPIO_OUT(GPIOx, pin_bit)    GPIOx->MODER = (GPIOx->MODER & (~(0x3U << (pin_bit * 2)))) |  ((0x00000001U & 0x00000003) << (pin_bit * 2));
#define CFG_GPIO_IN(GPIOx, pin_bit)     GPIOx->MODER = GPIOx->MODER & (~(0x3U << (pin_bit * 2)))

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

/* N76E003 IAP 指令定义 */
#define IAPCMD_CID_READ         0x0B
#define IAPCMD_DID_READ         0x0C
#define IAPCMD_READ_UID         0x04

#define IAPCMD_ERASE_APROM      0x22
#define IAPCMD_ERASE_LDROM		0x62

#define IAPCMD_PROGRAM_APROM    0x21
#define IAPCMD_PROGRAM_LDROM    0x61

#define IAPCMD_READ_APROM       0x00
#define IAPCMD_READ_LDROM		0x40		

#define IAPCMD_ERASE_CFG		0xE2
#define IAPCMD_READ_CFG         0xC0
#define IAPCMD_PROGRAM_CFG	    0xE1

static void N76E_SendBit8Fast(uint8_t _data);
static void N76E_SendBit8Slow(uint8_t _data);
static uint8_t N76E_RaedBit8Fast(void);
static void N76E_SendAck(uint8_t _ack);
static void N76E_WriteByte(uint8_t _cmd, uint16_t _addr, uint8_t _data);

/*
*********************************************************************************************************
*    函 数 名: N76E_InitHard
*    功能说明: ICP接口GPIO硬件初始化
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void N76E_InitHard(void)
{   
    EIO_D4_Config(ES_GPIO_OUT);         /* ICP_REST - 复位输出 */
    
    EIO_D8_Config(ES_GPIO_OUT);         /* ICP_DAT  - 数据线 */    
    
    EIO_D6_Config(ES_GPIO_OUT);         /* ICP_CLK  - 数据线 */  

    ICP_RST_1();
    ICP_DAT_0();
    ICP_CLK_0();
}

/*
*********************************************************************************************************
*    函 数 名: N76E_SetResetPin
*    功能说明: 设置复位脚电平
*    形    参: _state : 电平状态
*    返 回 值: 无
*********************************************************************************************************
*/
void N76E_SetResetPin(uint8_t _state)
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

/*
*********************************************************************************************************
*    函 数 名: N76E_EnterIAP
*    功能说明: 进入IAP状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void N76E_EnterIAP(void)
{
    N76E_InitHard();
    
    ICP_DAT_OUT_ENABLE();
    ICP_DAT_0();
    
    ICP_RST_0();
    
    bsp_DelayUS(1500);  /* 延迟1.5ms */
    
    N76E_SendBit8Slow(0x5A);
    N76E_SendBit8Slow(0xA5);
    N76E_SendBit8Slow(0x03);
    
    bsp_DelayUS(100);
    
    /* RST暂时不恢复, 后面继续IAP操作 */
    
    //bsp_DelayMS(5);
}

/*
*********************************************************************************************************
*    函 数 名: N76E_ExitIAP
*    功能说明: 退出IAP状态
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void N76E_ExitIAP(void)
{
    bsp_DelayUS(1500);  /* 延迟1.5ms */
    
//    ICP_RST_1();
//    bsp_DelayMS(5);     /* 延迟5ms */        
    
    ICP_RST_0();
    
    ICP_DAT_OUT_ENABLE();
    ICP_DAT_0();    
    
    bsp_DelayUS(1500);  /* 延迟1.5ms */
    
    N76E_SendBit8Slow(0x0F);
    N76E_SendBit8Slow(0x78);
    N76E_SendBit8Slow(0xF0);
    
    bsp_DelayUS(1500);  /* 延迟1.5ms */
    
    ICP_RST_1();   
    
    bsp_DelayMS(5);    /* 延迟5ms */
}

/*
*********************************************************************************************************
*    函 数 名: N76E_SendBit8Fast
*    功能说明: 传输8bit
*    形    参: _data：数据
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void N76E_SendBit8Fast(uint8_t _data)
{
    uint8_t i;
    
    for (i = 0; i < 8; i++)
    {
        if (_data & 0x80)
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

/*
*********************************************************************************************************
*    函 数 名: N76E_SendBit8Slow
*    功能说明: 传输8bit, 低速模式, 用于进入和退出IAP
*    形    参: _data：数据
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void N76E_SendBit8Slow(uint8_t _data)
{
    uint8_t i;
    
    for (i = 0; i < 8; i++)
    {
        if (_data & 0x80)
        {
            ICP_DAT_1();
        }
        else
        {
            ICP_DAT_0();
        }
        bsp_DelayUS(50);
        _data <<= 1;
        ICP_CLK_1();        
        bsp_DelayUS(50);
        ICP_CLK_0();        
    }
}

/*
*********************************************************************************************************
*    函 数 名: N76E_SendAck
*    功能说明: 发送ACK
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void N76E_SendAck(uint8_t _ack)
{
    ICP_DAT_OUT_ENABLE();
    
    if (_ack == 0)
    {
        ICP_DAT_0();
    }
    else
    {
        ICP_DAT_1();
    }
    
    bsp_DelayNS(1000);
    ICP_CLK_1();        
    bsp_DelayNS(1000);
    ICP_CLK_0();
    
    bsp_DelayNS(1000);
}

/*
*********************************************************************************************************
*    函 数 名: N76E_RaedBit8Fast
*    功能说明: 传输8bit,读取
*    形    参: _data：数据
*    返 回 值: 无
*********************************************************************************************************
*/ 
static uint8_t N76E_RaedBit8Fast(void)
{
    uint8_t i;
    uint8_t data;    
    
    data = 0;
    for (i = 0; i < 8; i++)
    {        
        bsp_DelayNS(800);  /* 提速 1000 -> 800 */
        data <<= 1;
        if (ICP_DAT_IS_HIGH())
        {
            data++;
        }

        ICP_CLK_1();        
        bsp_DelayNS(800);
        ICP_CLK_0();        
    }
    return data;
}

/*
*********************************************************************************************************
*    函 数 名: N76E_ReadByte
*    功能说明: 发送3字节命令,读取1字节数据
*    形    参: _cmd : 命令
*              _addr : 地址     
*    返 回 值: 无
*********************************************************************************************************
*/ 
static uint8_t N76E_ReadByte(uint8_t _cmd, uint16_t _addr)
{
    uint8_t ch;
    
    ICP_DAT_OUT_ENABLE();
    
    N76E_SendBit8Fast((_cmd & 0xC0) + (_addr >> 10));           /* 地址高6bit */
    N76E_SendBit8Fast((_addr >> 2) & 0xFF);                     /* 地址中间8bit */
    N76E_SendBit8Fast(((_addr << 6) & 0xC0) + (_cmd & 0x3F));   /* 地址低2bit */

    ICP_DAT_OUT_DISABLE();    

    ch = N76E_RaedBit8Fast();
    
    N76E_SendAck(1);   

    return ch;
}

/*
*********************************************************************************************************
*    函 数 名: N76E_WriteByte
*    功能说明: 发送3字节命令,+ 1字节数据
*    形    参: _cmd : 命令
*              _addr : 地址     
*    返 回 值: 无
*********************************************************************************************************
*/ 
static void N76E_WriteByte(uint8_t _cmd, uint16_t _addr, uint8_t _data)
{
    ICP_DAT_OUT_ENABLE();
    
    N76E_SendBit8Fast((_cmd & 0xC0) + (_addr >> 10));           /* 地址高6bit */
    N76E_SendBit8Fast((_addr >> 2) & 0xFF);                     /* 地址中间8bit */
    N76E_SendBit8Fast(((_addr << 6) & 0xC0) + (_cmd & 0x3F));   /* 地址低2bit */
    N76E_SendBit8Fast(_data);   /* 8bit数据 */  
}

/*
*********************************************************************************************************
*    函 数 名: N76E_DetectIC
*    功能说明: 检测IC是否在位
*    形    参: _id 读取4FFC开始的数字，当做芯片识别码
*    返 回 值: 1表示OK  0表示错误
*********************************************************************************************************
*/ 
uint8_t N76E_DetectIC(uint32_t *_id)
{
    uint8_t id0, id1,id2;
    
    
    N76E_ExitIAP();
    
    N76E_EnterIAP();

    /* 读厂商ID */
    id0 = N76E_ReadByte(IAPCMD_CID_READ, 0x0000); 

    /* 读DID 低字节 */
    id1 = N76E_ReadByte(IAPCMD_DID_READ, 0x0000); 
    
    /* 读DID 高字节 */
    id2 = N76E_ReadByte(IAPCMD_DID_READ, 0x0001);        
    
    if (id0 == 0xFF && id1 == 0xFF && id2 == 0xFF)
    {
        *_id = 0;   /* 用于连续烧录识别芯片是否在位 */
    }
    else
    {
        *_id = (id0 << 16) + (id2 << 8) + id1;
    }
    
    
    //N76E_ExitIAP();
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: N76E_ReadCfg
*    功能说明: 读配置字
*    形    参:  _addr : 地址 0-7
*               _len : 长度
*               _cfg : 结果
*    返 回 值: 1表示OK  0表示错误
*********************************************************************************************************
*/ 
uint8_t N76E_ReadCfg(uint8_t _addr, uint8_t _len, uint8_t *_cfg)
{
    uint8_t i;
    
    for (i = 0; i < _len; i++)
    {
        _cfg[i] = N76E_ReadByte(IAPCMD_READ_CFG, _addr + i); 
    }
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: N76E_EraseChip
*    功能说明: 整片擦除指令
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void N76E_EraseChip(void)
{
    N76E_WriteByte(0xE6, 0xA5A5, 0xFF);
    
    bsp_DelayMS(200);
    
    N76E_SendAck(1);
}

/*
*********************************************************************************************************
*    函 数 名: N76E_ErasePage
*    功能说明: 擦除页, 固定128字节
*    形    参: _addr 地址, 大于0xFFFF 当做LDROM处理
*    返 回 值: 无
*********************************************************************************************************
*/ 
void N76E_ErasePage(uint32_t _addr)
{
    if (_addr > 0xFFFF)
    {
        N76E_WriteByte(IAPCMD_ERASE_APROM, _addr, 0xFF);
    }
    else
    {
        N76E_WriteByte(IAPCMD_ERASE_LDROM, _addr, 0xFF);
    }
    
    bsp_DelayMS(5);
    
    N76E_SendAck(1);
}

/*
*********************************************************************************************************
*    函 数 名: N76E_EraseCfg
*    功能说明: 擦除配置字
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void N76E_EraseCfg(void)
{
    N76E_WriteByte(IAPCMD_ERASE_CFG, 0, 0xFF);
    
    bsp_DelayMS(5);
    
    N76E_SendAck(1);
}

/*
*********************************************************************************************************
*    函 数 名: N76E_EraseCfg
*    功能说明: 擦除配置字. 必须8字节写入
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/ 
void N76E_ProgramCfg(uint8_t *_cfg)
{
    uint8_t i;
    
    N76E_WriteByte(IAPCMD_PROGRAM_CFG, 0, _cfg[0]);
    bsp_DelayUS(20);     
    N76E_SendAck(0);
    
    for (i = 1; i <= 6; i++)
    {
        N76E_SendBit8Fast(_cfg[i]);
        bsp_DelayUS(20); 
        N76E_SendAck(0);    
    }
    
    N76E_SendBit8Fast(_cfg[7]);
    bsp_DelayUS(20); 
    N76E_SendAck(1);
}

/*
*********************************************************************************************************
*    函 数 名: N76E_WriteBuf
*    功能说明: 连续写多个字节.  不支持的操作。没有内存或寄存器可以操作
*    形    参: _Addr : 3个字节的地址。整数值
*              _Buf : 输入数据缓冲区
*              _Len : 字节长度
*    返 回 值: 1表示OK, 0表示出错
*********************************************************************************************************
*/ 
uint8_t N76E_WriteBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len)
{
    uint8_t ack = 0;
    
    return ack;
}

/*
*********************************************************************************************************
*    函 数 名: N76E_ReadBuf
*    功能说明: 读取连续多个字节数据. APROM。  LDROM的数据地址 + 0x10000
*    形    参: _Addr : 3个字节的地址。整数值
*              _Buf : 目标缓冲区
*              _Len : 字节长度
*    返 回 值: 1表示成功，0表示失败
*********************************************************************************************************
*/ 
uint8_t N76E_ReadBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len)
{
    uint32_t i;
    uint16_t addr;
    uint8_t cmd;    
    
    if (_Addr > 0xFFFF)
    {
        addr = _Addr - 0x10000;        
        cmd = IAPCMD_READ_LDROM;
    }
    else
    {
        addr = _Addr;
        cmd = IAPCMD_READ_APROM;
    }
    
//    N76E_EnterIAP();    
    
    for (i = 0; i < _Len; i++)
    {
        if ((i % 32) == 0)
        {
            ICP_DAT_OUT_ENABLE();    
            N76E_SendBit8Fast((cmd & 0xC0) + (addr >> 10));             /* 地址高6bit */
            N76E_SendBit8Fast((addr >> 2) & 0xFF);                      /* 地址中间8bit */
            N76E_SendBit8Fast(((addr << 6) & 0xC0) + (cmd & 0x3F));     /* 地址低2bit */
        }
        ICP_DAT_OUT_DISABLE();         
        _Buf[i] = N76E_RaedBit8Fast();
        
        if ((i % 32) == 31 || i == _Len - 1)
        {
            N76E_SendAck(1);
            
            bsp_DelayUS(200);
        }
        else
        {
            N76E_SendAck(0);
        }
        
        addr++;
    }
    
//    N76E_ExitIAP();
    
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: N76E_FLASH_ProgramBuf
*    功能说明: Programs a memory block。  页大小128字节
*    形    参: _FlashAddr : 绝对地址。 
*              _Buff : Pointer to buffer containing source data.
*              _Size : 数据大小，可以大于1个block
*    返 回 值: 0 : 出错;  1 : 成功
*********************************************************************************************************
*/ 
uint8_t N76E_FLASH_ProgramBuf(uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size)
{
    uint32_t i;
    uint16_t addr;
    uint8_t cmd;    
    
    if (_FlashAddr > 0xFFFF)
    {
        addr = _FlashAddr - 0x10000;        
        cmd = IAPCMD_PROGRAM_LDROM;
    }
    else
    {
        addr = _FlashAddr;
        cmd = IAPCMD_PROGRAM_APROM;
    } 
    
    for (i = 0; i < _Size; i++)
    {
        if ((i % 32) == 0)
        {
            ICP_DAT_OUT_ENABLE();    
            N76E_SendBit8Fast((cmd & 0xC0) + (addr >> 10));             /* 地址高6bit */
            N76E_SendBit8Fast((addr >> 2) & 0xFF);                      /* 地址中间8bit */
            N76E_SendBit8Fast(((addr << 6) & 0xC0) + (cmd & 0x3F));     /* 地址低2bit */
        }
        N76E_SendBit8Fast(_Buff[i]);
        
        bsp_DelayUS(20);        
        if ((i % 32) == 31 || i == _Size - 1)
        {
            N76E_SendAck(1);
            
            bsp_DelayUS(200);
        }
        else
        {
            N76E_SendAck(0);
        }
        
        addr++;
    }
    
    return 1;
}    

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
