/*
*********************************************************************************************************
*
*    模块名称 : I2C总线驱动模块(外部IO)
*    文件名称 : bsp_ext_i2c_gpio_ext.c
*    版    本 : V1.0
*    说    明 : 用gpio模拟i2c总线,
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2020-10-23 armfly  正式发布
*
*    Copyright (C), 2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

/*
    应用说明：
    在访问I2C设备前，请先调用 ext_i2c_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO
*/

#include "bsp.h"

/*
    i2c总线GPIO:
         PF1/I2C2_SCL
         PF0/I2C2_SDA
*/

/* 定义I2C总线连接的GPIO端口 */
#define I2C_SCL_GPIO    GPIOF          /* 连接到SCL时钟线的GPIO */
#define I2C_SCL_PIN     GPIO_PIN_1     /* 连接到SCL时钟线的GPIO */

#define I2C_SDA_GPIO    GPIOF          /* 连接到SDA数据线的GPIO */
#define I2C_SDA_PIN     GPIO_PIN_0     /* 连接到SDA数据线的GPIO */

#define ALL_I2C_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()

/* 定义读写SCL和SDA的宏 */
#define I2C_SCL_1() BSP_SET_GPIO_1(I2C_SCL_GPIO, I2C_SCL_PIN)   /* SCL = 1 */
#define I2C_SCL_0() BSP_SET_GPIO_0(I2C_SCL_GPIO, I2C_SCL_PIN)   /* SCL = 0 */

#define I2C_SDA_1() BSP_SET_GPIO_1(I2C_SDA_GPIO, I2C_SDA_PIN)   /* SDA = 1 */
#define I2C_SDA_0() BSP_SET_GPIO_0(I2C_SDA_GPIO, I2C_SDA_PIN)   /* SDA = 0 */

#define I2C_SDA_READ() ((I2C_SDA_GPIO->IDR & I2C_SDA_PIN) != 0) /* 读SDA口线状态 */
#define I2C_SCL_READ() ((I2C_SCL_GPIO->IDR & I2C_SCL_PIN) != 0) /* 读SCL口线状态 */

static uint32_t s_HalfTimeNS;   /* 时钟一半时长，ns单位 */

/*
*********************************************************************************************************
*    函 数 名: bsp_InitExtI2C
*    功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*    形    参: freq : 总线时钟,KH
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitExtI2C(uint32_t freq)
{
    uint32_t ns;
    
    EIO_D7_Config(ES_GPIO_I2C);
    EIO_D9_Config(ES_GPIO_I2C);

    ns = 1000000 / (freq / 1000);
    
    s_HalfTimeNS = ns / 2;
    
    if (s_HalfTimeNS > 205)
    {
        s_HalfTimeNS -= 205;    /* 补偿函数调用时间 */
    }
    
    /* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
    ext_i2c_Stop();
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_Delay
*    功能说明: I2C总线位延迟，最快400KHz
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
static void ext_i2c_Delay(uint32_t ns)
{
    bsp_DelayNS(s_HalfTimeNS);
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_Start
*    功能说明: CPU发起I2C总线启动信号
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void ext_i2c_Start(void)
{
    /* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
    I2C_SDA_1();
    I2C_SCL_1();
    ext_i2c_Delay(s_HalfTimeNS);
    I2C_SDA_0();
    ext_i2c_Delay(s_HalfTimeNS);

    I2C_SCL_0();
    ext_i2c_Delay(s_HalfTimeNS);
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_Start
*    功能说明: CPU发起I2C总线停止信号
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void ext_i2c_Stop(void)
{
    /* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
    I2C_SDA_0();
    I2C_SCL_1();
    ext_i2c_Delay(s_HalfTimeNS);
    I2C_SDA_1();
    ext_i2c_Delay(s_HalfTimeNS);
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_SendByte
*    功能说明: CPU向I2C总线设备发送8bit数据
*    形    参:  _ucByte ： 等待发送的字节
*    返 回 值: 无
*********************************************************************************************************
*/
void ext_i2c_SendByte(uint8_t _ucByte)
{
    uint8_t i;

    /* 先发送字节的高位bit7 */
    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            I2C_SDA_1();
        }
        else
        {
            I2C_SDA_0();
        }
        ext_i2c_Delay(s_HalfTimeNS);
        I2C_SCL_1();
        ext_i2c_Delay(s_HalfTimeNS);
        I2C_SCL_0();
        if (i == 7)
        {
            I2C_SDA_1(); // 释放总线
        }
        _ucByte <<= 1; /* 左移一个bit */
                                     //        ext_i2c_Delay();
    }
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_ReadByte
*    功能说明: CPU从I2C总线设备读取8bit数据
*    形    参:  无
*    返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t ext_i2c_ReadByte(void)
{
    uint8_t i;
    uint8_t value;

    /* 读到第1个bit为数据的bit7 */
    value = 0;
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        I2C_SCL_1();
        ext_i2c_Delay(s_HalfTimeNS);
        if (I2C_SDA_READ())
        {
            value++;
        }
        I2C_SCL_0();
        ext_i2c_Delay(s_HalfTimeNS);
    }
    return value;
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_WaitAck
*    功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*    形    参:  无
*    返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t ext_i2c_WaitAck(void)
{
    uint8_t re;

    I2C_SDA_1(); /* CPU释放SDA总线 */
    ext_i2c_Delay(s_HalfTimeNS);
    I2C_SCL_1(); /* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
    ext_i2c_Delay(s_HalfTimeNS);
    if (I2C_SDA_READ()) /* CPU读取SDA口线状态 */
    {
        re = 1;
    }
    else
    {
        re = 0;
    }
    I2C_SCL_0();
    ext_i2c_Delay(s_HalfTimeNS);
    return re;
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_Ack
*    功能说明: CPU产生一个ACK信号
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void ext_i2c_Ack(void)
{
    I2C_SDA_0(); /* CPU驱动SDA = 0 */
    ext_i2c_Delay(s_HalfTimeNS);
    I2C_SCL_1(); /* CPU产生1个时钟 */
    ext_i2c_Delay(s_HalfTimeNS);
    I2C_SCL_0();
    ext_i2c_Delay(s_HalfTimeNS);
    I2C_SDA_1(); /* CPU释放SDA总线 */
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_NAck
*    功能说明: CPU产生1个NACK信号
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void ext_i2c_NAck(void)
{
    I2C_SDA_1(); /* CPU驱动SDA = 1 */
    ext_i2c_Delay(s_HalfTimeNS);
    I2C_SCL_1(); /* CPU产生1个时钟 */
    ext_i2c_Delay(s_HalfTimeNS);
    I2C_SCL_0();
    ext_i2c_Delay(s_HalfTimeNS);
}

/*
*********************************************************************************************************
*    函 数 名: ext_i2c_CheckDevice
*    功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*    形    参:  _Address：设备的I2C总线地址
*    返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t ext_i2c_CheckDevice(uint8_t _Address)
{
    uint8_t ucAck;

    if (I2C_SDA_READ() && I2C_SCL_READ())
    {
        ext_i2c_Start(); /* 发送启动信号 */

        /* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
        ext_i2c_SendByte(_Address | I2C_WR);
        ucAck = ext_i2c_WaitAck(); /* 检测设备的ACK应答 */

        ext_i2c_Stop(); /* 发送停止信号 */

        return ucAck;
    }
    return 1; /* I2C总线异常 */
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
