/*
*********************************************************************************************************
*
*    模块名称 : ESP32 串口WIFI模块驱动程序
*    文件名称 : bsp_esp32_at.c
*    版    本 : V2.3
*    说    明 : 封装 ESP32 模块相关的AT命令
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2014-11-29  armfly  正式发布
*        V1.1    2014-12-11  armfly  修改 ESP32_WaitResponse() 函数, 实现任意字符判断。增加TCP数据发送函数.
*        V1.2    2014-12-22  armfly  增加GPIO2， GPIO0 引脚的配置。适应新版硬件。
*        V1.3    2015-07-24  armfly    
*                    (1) 增加函数 uint8_t ESP32_CreateTCPServer(void);
*                    (2) 修改ESP32_JoinAP() 增加返回值
*        V1.4    2015-12-22  armfly  重大更新，支持多连接，修改了很多函数
*        V1.5    2015-12-27  armfly  发送数据前，不清接收缓冲区，避免指令丢失。
*        V2.0    2015-12-31  armfly  添加 ESP32_PT_JoinAP函数，非阻塞模式。
*        V2.1    2016-01-01  armfly  添加 smart link智能配置函数。
*        V2.2    2016-01-19  armfly 
*                        - 添加 ESP32_QueryIPStatus ，返回值， IPS_BUSY，应对模块内部忙的情况.
*                        - ESP32_SendTcpUdp(), 需要处理 busy s... 的情况
*                        - ESP32_RxNew() 更名为 ESP32_RxData增加形参，支持解码TCP UDP数据包和普通指令包，
*        V3.0    2019-09-14  arfmly  根据esp8266驱动程序修改，改为esp32. 大部分功能差不多.
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#include "param.h"
#include "modbus_slave.h"

/* ESP32 模块接线图
    ESP32模块   
        UTXD   ---  PH14/UART4_RX
        URXD   ---  PH13/UART4_TX
        GND    ---  GND
        
        CH_PD  ---  PF6/ESP32-S_RESET  ( 0表示掉电  1表示正常上电工作）
        GPIO0  ---  PG4/BOOT_OPTION ( 0代表进入系统升级，1表示正常引导用户程序（AT指令）)


    模块缺省波特率 9600;  支持的范围：110~460800bps          ---- 本例子会将模块波特率切换为 115200
    在板子上电初始跑boot rom的一段log，需要在 74880 的波特率下正常打印。下面是打印出来的内容.

    ----------- PD = 1 之后 74880bps 打印如下内容 ------

     ets Jan  8 2013,rst cause:1, boot mode:(3,6)

    load 0x40100000, len 25052, room 16
    tail 12
    chksum 0x0b
    ho 0 tail 12 room 4
    load 0x3ffe8000, len 3312, room 12
    tail 4
    chksum 0x53
    load 0x3ffe8cf0, len 6576, room 4
    tail 12
    chksum 0x0d
    csum 0x0d

    ----------- 之后是 9600bps 打印 ---------------

    [Vendor:www.ai-thinker.com Version:0.9.2.4]

    ready


    使用串口超级终端软件时，需要设置 终端 - 仿真 - 模式 页面勾选“新行模式”.


    【修改波特率】
    AT+CIOBAUD=?     ---- 查询命令参数
    +CIOBAUD:(9600-921600)

    OK

    AT+CIOBAUD=115200
    BAUD->115200

    【选择 WIFI 应用模式 】
    AT+CWMODE=1
        1   Station 模式
        2   AP 模式
        3   AP 兼 Station 模式

    【列出当前可用 AP】
    AT+CWLAP=<ssid>,< mac >,<ch>
    AT+CWLAP

    【AT+CWJAP加入 AP】
    AT+CWJAP=<ssid>,< pwd >
    
    
    "AT+CWJAP="Tenda_4FD138","123456887mdh"

*/

/*
    获得版本信息
    AT+GMR
    AT version:0.21.0.0
    SDK version:0.9.5
    
    
    
    AT+CWMODE?     
    1： station模式
    2： softAP模式
    3： softAp + station
*/

/*
ESP32有三种BOOT模式，由MTDO（GPIO15）,GPIO0,GPIO2决定
 13     15       14
MTDO   GPIO0   GPIO2   Mode  Description
 L       L       H     UART Download code from UART 
 L       H       H     Flash Boot from SPI Flash 
 H       x       x     SDIO Boot from SD-card 
 
上电时打印的信息中boot mode:(x,y)， x低3位为 {MTDO, GPIO0, GPIO2}.
*/

#define AT_CR '\r'
#define AT_LF '\n'

#define ALL_ESP_GPIO_CLK_ENABLE() \
    __HAL_RCC_GPIOF_CLK_ENABLE();   \
    __HAL_RCC_GPIOG_CLK_ENABLE()

/* 硬件掉电控制引脚 -- 接 3.3V 开始工作  */
#define GPIO_CH_PD          GPIOF
#define PIN_CH_PD           GPIO_PIN_6
#define ESP_CH_PD_0()       BSP_SET_GPIO_0(GPIO_CH_PD, PIN_CH_PD)
#define ESP_CH_PD_1()       BSP_SET_GPIO_1(GPIO_CH_PD, PIN_CH_PD)

/* 1表示进入固件升级模式 0表示正常AT指令模式 */
#define GPIO_GPIO0          GPIOG
#define PIN_GPIO0           GPIO_PIN_4
#define ESP_GPIO0_0()       BSP_SET_GPIO_0(GPIO_GPIO0, PIN_GPIO0)   /* 固件升级模式 */
#define ESP_GPIO0_1()       BSP_SET_GPIO_1(GPIO_GPIO0, PIN_GPIO0)   /* 正常运行模式 */

uint8_t g_EspBuf[2048]; /* 用于解码 */

/* 保存模块主动发送的消息 */
uint8_t g_RxMsgBuf[RX_MSG_MAX];
uint8_t g_RxMsgLen;

static void ESP32_CH_PD_0(void)
{
    ESP_CH_PD_0();
}

static void ESP32_CH_PD_1(void)
{
    ESP_CH_PD_1();
}

/*
*********************************************************************************************************
*    函 数 名: bsp_InitESP32
*    功能说明: 配置无线模块相关的GPIO,  该函数被 bsp_Init() 调用。
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitESP32(void)
{
    GPIO_InitTypeDef gpio_init;

    /* 第1步：打开GPIO时钟 */
    ALL_ESP_GPIO_CLK_ENABLE();

    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        /* 设置开漏输出 */
    gpio_init.Pull = GPIO_NOPULL;                        /* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度等级 */

    gpio_init.Pin = PIN_CH_PD;
    HAL_GPIO_Init(GPIO_CH_PD, &gpio_init);

    gpio_init.Pin = PIN_GPIO0;
    HAL_GPIO_Init(GPIO_GPIO0, &gpio_init);

    ESP_CH_PD_0(); /* 先让ESP模块处于掉电状态 */

    ESP_GPIO0_1(); /* 1表示正常运行 */
}

/*
*********************************************************************************************************
*    函 数 名: WIFI_CheckAck
*    功能说明: 检查应答
*    形    参: _str : 等待的字符串
*              _timeout : 等待超时，ms
*    返 回 值: 0:不满足条件  1:成功检测到字符串  2:超时了
*********************************************************************************************************
*/
uint8_t WIFI_CheckAck(uint8_t *_str, int32_t _timeout)
{
    uint8_t ch;
    static uint8_t s_cmp_len = 0;
    static int32_t s_time = 0;

    if (_str[0] == 0)
    {
        s_cmp_len = 0;
        s_time = bsp_GetRunTime();
        return 0;
    }

    if (ESP32_GetChar(&ch))
    {
        if (ch == _str[s_cmp_len])
        {
            s_cmp_len++;
            if (_str[s_cmp_len] == 0)
            {
                return 1; /* 收到正确应答 */
            }
        }
        else
        {
            s_cmp_len = 0;
        }
    }

    if (_timeout > 0)
    {
        if (bsp_CheckRunTime(s_time) > _timeout)
        {
            return 2;
        }
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_SendBuf
*    功能说明: 发送数据到WiFi模块
*    形    参: _cmd : 数组
*             _len : 数据长度
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_SendBuf(uint8_t *_cmd, uint16_t _len)
{
    //#ifdef ESP32_TO_COM1_EN
    if (g_tVar.WiFiDebugEn == 1) /* RS485串口输入AT指令时，进入WiFi Debug状态，持续120秒自动退出 */
    {
        //        {
        //            char buf[20];
        //
        //            static int32_t s_time = 0;
        //
        //            if (bsp_CheckRunTime(s_time) > 10)
        //            {
        //                sprintf(buf, "\r\n(%d)=>", bsp_GetRunTime());
        //                comSendBuf(COM_DEBUG, (uint8_t *)buf, strlen(buf));
        //            }
        //            s_time = bsp_GetRunTime();
        //        }
        //        comSendBuf(COM_DEBUG, _cmd, _len);        /* 将接收到数据打印到调试串口1 */
    }
    //#endif

    comSendBuf(COM_ESP32, _cmd, _len);
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_GetChar
*    功能说明: 发送数据到WiFi模块
*    形    参: _data : 存放数据的变量地址
*    返 回 值: 0 表示无数据，1表示有数据。 数据存放在 *_data
*********************************************************************************************************
*/
extern uint8_t link_id;
uint8_t ESP32_GetChar(uint8_t *_data)
{
    uint8_t re;

    re = comGetChar(COM_ESP32, _data);
    if (re != 0)
    {
        if (g_tVar.WiFiDebugEn == 1) /* RS485串口输入AT指令时，进入WiFi Debug状态，持续120秒自动退出 */
        {
            //            {
            //                static int32_t s_time = 0;
            //
            //                if (bsp_CheckRunTime(s_time) > 20)
            //                {
            //                    char buf[20];
            //
            //                    sprintf(buf, "\r\n(%d)<==", bsp_GetRunTime());
            //                    comSendBuf(COM_DEBUG, (uint8_t *)buf, strlen(buf));
            //                }
            //                s_time = bsp_GetRunTime();
            //                comSendChar(COM_DEBUG, *_data);        /* 将接收到数据打印到调试串口1 */
            //            }
        }

        /*     
            #define RX_MSG_MAX    32;
            uint8_t g_RxMsgBuf[RX_MSG_MAX]; 
            uint8_t g_RxMsgLen;
        */
        {
            uint8_t ucData;
            static uint8_t s_flag = 0;            /* IPD 还是其他消息 */
            static uint16_t s_data_len = 0; /* UDP TCP数据长度 */
            char *p1;
            static int32_t s_last_rx_time = 0;
            static uint16_t s_ipd_pos = 0;

            /* +IPD,0,7:ledon 1 */
            ucData = *_data;

            /* 如果上次收到的数据距今超过100ms，则重新做帧同步 */
            if (bsp_CheckRunTime(s_last_rx_time) > 1000)
            {
                s_flag = 0;
                g_RxMsgLen = 0;
                s_ipd_pos = 0;
            }

            if (s_flag == 0)
            {
                if (ucData == 0x0D || ucData == 0xA)
                {
                    if (s_ipd_pos >= 2)
                    {
                        g_RxMsgLen = s_ipd_pos; /* 接收到非数据包的应答 */

                        /* 识别断线消息 - 此处不处理。交给上层处理 */
                        {
                            /* TCP服务器关闭了TCP连接 */
                            if (g_RxMsgLen >= 8 && memcmp(g_RxMsgBuf, "4,CLOSED", 8) == 0)
                            {
                                g_tVar.RemoteTCPServerOk = 0;
                            }

                            /* WIFI路由器断网 */
                            if (g_RxMsgLen >= 15 && memcmp(g_RxMsgBuf, "WIFI DISCONNECT", 15) == 0)
                            {
                                g_tVar.HomeWiFiLinkOk = 0;
                                g_tVar.RemoteTCPServerOk = 0;
                            }
                        }

                        s_ipd_pos = 0;
                    }
                    else
                    {
                        g_RxMsgLen = 0;
                        s_data_len = 0;
                        s_ipd_pos = 0;
                    }
                }
                else
                {
                    if (s_ipd_pos < RX_MSG_MAX)
                    {
                        g_RxMsgBuf[s_ipd_pos++] = ucData; /* 保存接收到的数据 */
                    }

                    if (g_RxMsgBuf[0] == '+' && s_ipd_pos > 7 && ucData == ':')
                    {
                        p1 = (char *)&g_RxMsgBuf[5];
                        link_id = str_to_int(p1); /* 解析出连接id */

                        p1 = (char *)&g_RxMsgBuf[7];
                        s_data_len = str_to_int(p1); /* 解析出数据包长度 */
                        s_flag = 1;                                     /* 进入数据包接收状态 */
                        s_ipd_pos = 0;
                    }
                }
            }
            else /* 这是接收 +IPD数据包的分之 */
            {
                if (s_ipd_pos < RX_BUF_SIZE)
                {
                    g_EspBuf[s_ipd_pos++] = ucData; /* 保存接收到的UDP,TCP数据体 */

                    if (s_ipd_pos == s_data_len)
                    {
                        s_flag = 0;
                        g_tModS.RxCount = s_data_len; /* wifi_poll 会处理modbus帧 */
                        g_tVar.WiFiRecivedIPD = 1;        /* 收到UDP, TCP数据包 */

                        s_ipd_pos = 0;
                    }
                }
                else
                {
                    s_flag = 0;
                }
            }

            s_last_rx_time = bsp_GetRunTime();
        }

        return 1;
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_PowerOn
*    功能说明: 给ESP32模块上电
*    形    参: 无
*    返 回 值: 0 表示上电失败，1表示OK，检测到模块
*********************************************************************************************************
*/
uint8_t ESP32_PowerOn(void)
{
    /*
        2018-08-08 采购ESP-01模块，固件更新过。打印信息如下：
    从上电脉冲，到发送完毕，266ms  ( 74880 bsp)
    到发送ready信号。

     ets Jan  8 2013,rst cause:1, boot mode:(3,6)

    load 0x40100000, len 1856, room 16 
    tail 0
    chksum 0x63
    load 0x3ffe8000, len 776, room 8 
    tail 0
    chksum 0x02
    load 0x3ffe8310, len 552, room 8 
    tail 0
    chksum 0x79
    csum 0x79

    2nd boot version : 1.5
      SPI Speed      : 40MHz
      SPI Mode       : DIO
      SPI Flash Size & Map: 8Mbit(512KB+512KB)
    jump to run user1 @ 1000


    (219)<==rf cal sector: 249
    rf[112] : 00
    rf[113] : 00
    rf[114] : 01

    SDK ver: 1.5.4.1(39cb9a32) compiled @ Jul  1 2016 20:04:35
    phy ver: 972, pp ver: 10.1
    */

    /*  旧版本
        ESP-01 模块上电时，会以74880波特率打印如下信息:
    
         ets Jan  8 2013,rst cause:1, boot mode:(3,6)

        load 0x40100000, len 25052, room 16
        tail 12
        chksum 0x0b
        ho 0 tail 12 room 4
        load 0x3ffe8000, len 3312, room 12
        tail 4
        chksum 0x53
        load 0x3ffe8cf0, len 6576, room 4
        tail 12
        chksum 0x0d
        csum 0x0d        <-----  程序识别 csum 后，再自动切换到正常波特率
    */

    /* 
        ESP-07 模块上电时，会以74880波特率打印如下信息:  (实测 310ms后收到ready）
    
         ets Jan  8 2013,rst cause:1, boot mode:(3,7)

        load 0x40100000, len 816, room 16 
        tail 0
        chksum 0x8d
        load 0x3ffe8000, len 788, room 8 
        tail 12
        chksum 0xcf
        ho 0 tail 12 room 4
        load 0x3ffe8314, len 288, room 12 
        tail 4
        chksum 0xcf
        csum 0xcf

        2nd boot version : 1.2
          SPI Speed      : 40MHz
          SPI Mode       : QIO
          SPI Flash Size : 4Mbit
        jump to run user1
    */
    uint8_t re;

    ESP32_CH_PD_0();

    bsp_DelayMS(20);

    ESP32_CH_PD_1();

#if 0
    bsp_SetUartBaud(74880);
    ESP32_WaitResponse("phy ver", 5000);
#endif

    /* 等待模块完成上电，判断是否接收到 ready */
    comSetBaud(COM_ESP32, 115200);
    re = ESP32_WaitResponse("ready", 300);
    if (re == 0)
    {
        return 0;
    }

    /* 关闭回显功能，主机发送的字符，模块无需返回 */
    ESP32_SendAT("ATE0");
    ESP32_WaitResponse("OK\r\n", 100);

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_PowerOff
*    功能说明: 控制ESP32模块关机
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_PowerOff(void)
{
    ESP32_CH_PD_0();
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_Reset
*    功能说明: 复位ESP32模块
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_Reset(void)
{
    ESP32_CH_PD_0();
    bsp_DelayMS(20);
    ESP32_CH_PD_1();

    bsp_DelayMS(10);
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_EnterISP
*    功能说明: 控制ESP32模块进入固件升级模式
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_EnterISP(void)
{
    ESP_CH_PD_0();
    ESP_GPIO0_0(); /* 0 表示进入固件升级模式 */
    bsp_DelayMS(10);
    ESP_CH_PD_1();
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_EnterAT
*    功能说明: 控制ESP32模块进入AT模式
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_EnterAT(void)
{
    ESP_CH_PD_0();
    ESP_GPIO0_1(); /* 1 表示进入用户程序（AT指令）模式 */
    bsp_DelayMS(10);
    ESP_CH_PD_1();
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_9600to115200
*    功能说明: 9600波特率切换到115200
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_9600to115200(void)
{
    comSetBaud(COM_ESP32, 9600);
    ESP32_SendAT("AT+CIOBAUD=115200");    /* 按 9600bps 发送指令切换为 115200 */
    ESP32_WaitResponse("OK\r\n", 2000); /* 这个 OK 是模块按 9600 应答的 */
    comSetBaud(COM_ESP32, 115200);            /* 切换STM32的波特率为 115200 */

    /* 切换为 Station模式 */
    bsp_DelayMS(100);
    ESP32_SendAT("AT+CWMODE=1");
    ESP32_WaitResponse("OK\r\n", 2000);
    bsp_DelayMS(1500);
    ESP32_SendAT("AT+RST");
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_WaitResponse
*    功能说明: 等待ESP32返回指定的应答字符串, 可以包含任意字符。只要接收齐全即可返回。
*    形    参: _pAckStr : 应答的字符串， 长度不得超过255. 支持检测多个字符串\t间隔.
*             _usTimeOut : 命令执行超时，0表示一直等待. >０表示超时时间，单位1ms
*    返 回 值: 1 表示成功  0 表示失败
*********************************************************************************************************
*/
uint8_t ESP32_WaitResponse(char *_pAckStr, uint16_t _usTimeOut)
{
    uint8_t ucData;
    uint16_t pos = 0;
    uint32_t len;
    uint8_t ret;
    int32_t time1;

    len = strlen(_pAckStr);
    if (len > 255)
    {
        return 0;
    }

    time1 = bsp_GetRunTime();
    while (1)
    {
        bsp_Idle(); /* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

        if (_usTimeOut > 0) /* _usTimeOut == 0 表示无限等待 */
        {
            if (bsp_CheckRunTime(time1) >= _usTimeOut)
            {
                ret = 0; /* 超时 */
                break;
            }
        }

        if (ESP32_GetChar(&ucData))
        {

            {
                if (ucData == _pAckStr[pos])
                {
                    pos++;

                    if (pos == len)
                    {
                        ret = 1; /* 收到指定的应答数据，返回成功 */
                        break;
                    }
                }
                else
                {
                    pos = 0;
                }
            }
        }
    }
    return ret;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_ReadLine
*    功能说明: 读取ESP32返回的一行应答字符串(0x0D 0x0A结束)。该函数根据字符间超时判断结束。 本函数需要紧跟AT命令发送函数。
*    形    参: _pBuf : 存放模块返回的完整字符串
*              _usBufSize : 缓冲区最大长度
*             _usTimeOut : 命令执行超时，0表示一直等待. >0 表示超时时间，单位1ms
*    返 回 值: 0 表示错误（超时）  > 0 表示应答的数据长度
*********************************************************************************************************
*/
uint16_t ESP32_ReadLine(char *_pBuf, uint16_t _usBufSize, uint16_t _usTimeOut)
{
    uint8_t ucData;
    uint16_t pos = 0;
    uint8_t ret;
    int32_t time1;
    uint16_t time_out;

    time1 = bsp_GetRunTime();
    time_out = _usTimeOut; /* 首次超时 */
    while (1)
    {
        bsp_Idle(); /* CPU空闲执行的操作， 见 bsp.c 和 bsp.h 文件 */

        if (_usTimeOut > 0)
        {
            if (bsp_CheckRunTime(time1) >= time_out)
            {
                _pBuf[pos] = 0; /* 结尾加0， 便于函数调用者识别字符串结束 */
                ret = pos;            /* 成功。 返回数据长度 */
                break;
            }
        }

        if (ESP32_GetChar(&ucData))
        {
            time1 = bsp_GetRunTime(); /* 收到首字符后，字符间超时设置为 0.5秒 */
            time_out = 500;

            if (pos < _usBufSize)
            {
                _pBuf[pos++] = ucData; /* 保存接收到的数据 */
            }
            if (ucData == 0x0A)
            {
                _pBuf[pos] = 0;
                ret = pos; /* 成功。 返回数据长度 */
                break;
            }
        }
    }
    return ret;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_ReadLine
*    功能说明: 读取ESP32返回的一行应答字符串(0x0D 0x0A结束)。该函数根据字符间超时判断结束。 本函数需要紧跟AT命令发送函数。
*    形    参: _pBuf : 存放模块返回的完整字符串
*              _usBufSize : 缓冲区最大长度
*             _usTimeOut : 命令执行超时，0表示一直等待. >0 表示超时时间，单位1ms
*    返 回 值: 0表示未遇到结束符号  > 0 表示应答的数据长度
*********************************************************************************************************
*/
uint16_t ESP32_ReadLineNoWait(char *_pBuf, uint16_t _usBufSize)
{
    static uint16_t s_pos = 0;
    uint8_t ucData;
    uint8_t ret = 0;

    if (ESP32_GetChar(&ucData))
    {
        if (s_pos < _usBufSize)
        {
            _pBuf[s_pos++] = ucData; /* 保存接收到的数据 */
        }
        if (ucData == 0x0A)
        {
            _pBuf[s_pos] = 0;
            ret = s_pos; /* 成功。 返回数据长度 */
            s_pos = 0;
        }
    }
    return ret;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_SendAT
*    功能说明: 向模块发送AT命令。 本函数自动在AT字符串口增加<CR>字符
*    形    参: _Str : AT命令字符串，不包括末尾的回车<CR>. 以字符0结束
*    返 回 值: 无T
*********************************************************************************************************
*/
void ESP32_SendAT(char *_Cmd)
{
    //comClearRxFifo(COM_ESP32);

    /* 等待上包发送完毕 */
    while (comTxEmpty(COM_ESP32) == 0)
        ;

    ESP32_SendBuf((uint8_t *)_Cmd, strlen(_Cmd));
    ESP32_SendBuf("\r\n", 2);
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_SetWiFiMode
*    功能说明: 设置WiFi模块工作模式
*    形    参: _mode : 1 = Station模式,  2 = AP模式,  3 = AP兼Station模式
*    返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP32_SetWiFiMode(uint8_t _mode)
{
    char cmd_buf[30];

    if (_mode == 0 || _mode > 3)
    {
        _mode = 3;
    }
    sprintf(cmd_buf, "AT+CWMODE_CUR=%d", _mode);
    ESP32_SendAT(cmd_buf);
    if (ESP32_WaitResponse("OK\r\n", 2000) == 0)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_SetMac
*    功能说明: 设置MAC地址
*    形    参: _mac : 
*    返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP32_GetMac(uint8_t *_mac)
{
    /*
        AT+CIPSTAMAC_CUR?
    
        +CIPSTAMAC_CUR:"18:fe:34:d1:b0:07"
        OK    
    */
    char buf[64];

    memset(_mac, 0, 6);
    ESP32_SendAT("AT+CIPSTAMAC_CUR?");
    while (1)
    {
        if (ESP32_ReadLine(buf, sizeof(buf), 200))
        {
            if (memcmp(buf, "+CIPSTAMAC_CUR:", 15) == 0)
            {
                /* 解析mac地址 */
                char *p = &buf[16];

                _mac[0] = TwoCharToInt(p);
                p += 3;
                _mac[1] = TwoCharToInt(p);
                p += 3;
                _mac[2] = TwoCharToInt(p);
                p += 3;
                _mac[3] = TwoCharToInt(p);
                p += 3;
                _mac[4] = TwoCharToInt(p);
                p += 3;
                _mac[5] = TwoCharToInt(p);
            }
            else if (memcmp(buf, "OK", 2) == 0)
            {
                return 1;
            }
        }
        else /* 读超时 */
        {
            break;
        }
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_SetMac
*    功能说明: 设置MAC地址. ESP32 MAC   bit 0 不能是计数
*    形    参: _mac : 
*    返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP32_SetMac(uint8_t *_mac)
{
    /*
        AT+CIPSTAMAC_CUR="18:fe:35:98:d3:7b"
    */
    char cmd_buf[64];

    sprintf(cmd_buf, "AT+CIPSTAMAC_CUR=\"%02x:%02x:%02x:%02x:%02x:%02x\"",
                    _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
    ESP32_SendAT(cmd_buf);
    if (ESP32_WaitResponse("OK\r\n", 200) == 0)
    {
        return 0;
    }
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_CIPMUX
*    功能说明: 启动多连接模式
*    形    参: _mode : 0,表示关闭， 1表示启动
*    返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP32_CIPMUX(uint8_t _mode)
{
    char cmd_buf[30];

    if (_mode > 0)
    {
        _mode = 1;
    }
    sprintf(cmd_buf, "AT+CIPMUX=%d", _mode);
    ESP32_SendAT(cmd_buf);
    if (ESP32_WaitResponse("OK\r\n", 200) == 0)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_Set_AP_IP
*    功能说明: AT+CIPAP 设置 AP 的 IP地址
*    形    参: _ip :AP的IP地址，标准字符串
*    返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP32_Set_AP_IP(char *_ip)
{
    char cmd_buf[30];

    sprintf(cmd_buf, "AT+CIPAP=\"%s\"", _ip);
    ESP32_SendAT(cmd_buf);
    if (ESP32_WaitResponse("OK\r\n", 500) == 0)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_Set_AP_NamePass
*    功能说明: 设置SoftAP的名字，加密方式和密码.  加密方式为 。 
*    形    参: _name :AP的名字，字符串参数，密码最长 64 字节 ASCII
*             _pwd : AP的密码，字符串参数，密码最长 64 字节 ASCII
*             _ch : 通道号
*    返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP32_Set_AP_NamePass(char *_name, char *_pwd, uint8_t _ch, uint8_t _ecn)
{
    char cmd_buf[40];

    /* AT+CWSAP="ESP32","1234567890",5,3 */
    sprintf(cmd_buf, "AT+CWSAP_DEF=\"%s\",\"%s\",%d,%d", _name, _pwd, _ch, _ecn);
    ESP32_SendAT(cmd_buf);
    if (ESP32_WaitResponse("OK\r\n", 2000) == 0)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_CreateTCPServer
*    功能说明: 创建一个TCP服务端。 必须在连接到AP之后才行。 需要先启用多连接
*    形    参：_TcpPort : TCP 端口号
*    返 回 值: 0 表示失败。 1表示创建TCP成功
*********************************************************************************************************
*/
uint8_t ESP32_CreateTCPServer(uint16_t _TcpPort)
{
    char cmd_buf[30];

    /* 开启TCP server, 端口为 _TcpPort */
    sprintf(cmd_buf, "AT+CIPSERVER=1,%d", _TcpPort);
    ESP32_SendAT(cmd_buf);
    if (ESP32_WaitResponse("OK\r\n", 2000) == 0)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_CreateUDPServer
*    功能说明: 创建一个TUDP服务端。 必须在连接到AP之后才行。 需要先启用多连接
*    形    参:   _id : 连接ID, 0-4
*                _LaocalPort : UDP 端口号
*    返 回 值: 0 表示失败。 1表示创建TCP成功
*********************************************************************************************************
*/
uint8_t ESP32_CreateUDPServer(uint8_t _id, uint16_t _LaocalPort)
{
    char cmd_buf[64];

    /* 多连接 UDP */
    //AT+CIPSTART=0,"UDP","255.255.255.255",8080,8080,0           <----- 没有试通
    sprintf(cmd_buf, "AT+CIPSTART=%d,\"UDP\",\"255.255.255.255\",8080,%d,2", _id, _LaocalPort);

    ESP32_SendAT(cmd_buf);
    if (ESP32_WaitResponse("OK\r\n", 3000) == 0)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_LinkTCPServer
*    功能说明: 连接到一个TCP服务端。  仅(+CIPMUX=1) 多连接模式。
*    形    参: _id : 连接的id号
*              _server_ip : 服务器域名或IP地址
*              _TcpPort : TCP 端口号
*    返 回 值: 0 表示失败。 1表示创建TCP成功
*********************************************************************************************************
*/
uint8_t ESP32_LinkTCPServer(uint8_t _id, char *_server_ip, uint16_t _TcpPort)
{
    char cmd_buf[64];

#if 0 /* 单连接 */
    //AT+CIPSTART="TCP","192.168.101.110",1000
    sprintf(cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",%d",_server_ip, _TcpPort);
#else /* 多连接 */
    //AT+CIPSTART=0, "TCP","192.168.101.110",1000
    sprintf(cmd_buf, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d", _id, _server_ip, _TcpPort);
#endif
    ESP32_SendAT(cmd_buf);
    if (ESP32_WaitResponse("OK\r\n", 3000) == 0)
    {
        return 0;
    }

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_SendUdp
*    功能说明: 发送UDP数据包. 不占用连接号
*    形    参: _databuf 数据
*               _len 数据长度
*             
*    返 回 值: 0 表示失败， 1表示OK
*********************************************************************************************************
*/
uint8_t ESP32_SendUdp(char *_RemoteIP, uint16_t _RemotePort, uint8_t *_databuf, uint16_t _len)
{
    char buf[48];

    if (_len > 2048)
    {
        _len = 2048;
    }
    /* AT+CIPSEND=0,"192.168.168.168",6200,200 */
    sprintf(buf, "AT+CIPSEND=0,%d,\"%s\",%d\r\n", _len, _RemoteIP, _RemotePort);
    ESP32_SendBuf((uint8_t *)buf, strlen(buf));

    /* 模块先返回OK, 然后返回 > 表示等待数据输入 */
    if (ESP32_WaitResponse(">", 50) == 0)
    {
        return 0;
    }

    ESP32_SendBuf(_databuf, _len);

    /* 2018-08-21 在等待SEND OK的时间（大概20ms）内，服务器客户发送 +IPD 数据包. */
    ESP32_WaitResponse("SEND OK\r\n", 200);

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_SendTcpUdp
*    功能说明: 发送TCP或UDP数据包
*    形    参: _id : 多连接时，连接ID （0-4）
*              _databuf 数据
*              _len 数据长度
*    返 回 值: 0 表示失败， 1表示OK
*********************************************************************************************************
*/
uint8_t ESP32_SendTcpUdp(uint8_t _id, uint8_t *_databuf, uint16_t _len)
{
    char buf[32];

    if (_len > 2048)
    {
        _len = 2048;
    }

    sprintf(buf, "AT+CIPSEND=%d,%d\r\n", _id, _len);
    ESP32_SendBuf((uint8_t *)buf, strlen(buf));

    /* 模块先返回OK, 然后返回 > 表示等待数据输入 */
    if (ESP32_WaitResponse(">", 200) == 0) /* 2018-12-12, 增加延迟 50 -> 200 */
    {
        return 0;
    }

    ESP32_SendBuf(_databuf, _len);

    /* 2018-08-21 在等待SEND OK的时间（大概20ms）内，服务器客户发送 +IPD 数据包. */

    return ESP32_WaitResponse("SEND OK\r\n", 1000); /* 2018-12-12， 增加延迟，200 -> 1000 */
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_CloseTcpUdp
*    功能说明: 关闭TCP或UDP连接. 用于多路连接
*    形    参: _databuf 数据
*             _len 数据长度
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_CloseTcpUdp(uint8_t _id)
{
    char buf[32];

    //ESP32_SendAT("ATE0");        /* 关闭回显功能 */
    //ESP32_WaitResponse("SEND OK", 50);

    sprintf(buf, "AT+CIPCLOSE=%d", _id);
    ESP32_SendAT(buf);
    ESP32_WaitResponse("OK", 1000);

    /* ZHG : ---- 此处需要处理应答 */
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_QueryIPStatus
*    功能说明: 查询当前连接状态。（0-4）. 还不完善，未解析出当前链接的id
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t ESP32_QueryIPStatus(void)
{
    char buf[64];
    uint8_t i;
    uint8_t ret = IPS_TIMEOUT;

    ESP32_SendAT("AT+CIPSTATUS");

    /*　模块将应答:
        
        AT+CIPSTATUS
        STATUS:4

        OK
    
    
    有一种异常情况，模块内部忙。
    手机频繁关闭TCP连接，立即发送数据，可能导致wifi模块出现发送数据忙的情况
    
    busy s...
    */

    for (i = 0; i < 8; i++)
    {
        ESP32_ReadLine(buf, sizeof(buf), 50); /* 100ms超时 */
        if (memcmp(buf, "STATUS:", 7) == 0)
        {
            ret = buf[7];
        }
        else if (memcmp(buf, "OK", 2) == 0)
        {
            break;
        }
        else if (memcmp(buf, "busy", 4) == 0)
        {
            ret = IPS_BUSY;
        }
    }
    return ret;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_GetLocalIP
*    功能说明: 查询本机IP地址和MAC
*    形    参: _ssid : AP名字字符串
*              _pwd : 密码字符串
*    返 回 值: 1 表示OK， 0 表示未知
*********************************************************************************************************
*/
uint8_t ESP32_GetLocalIP(char *_ip, char *_mac)
{
    char buf[64];
    uint8_t i, m;
    uint8_t ret = 0;
    uint8_t temp;

    ESP32_SendAT("AT+CIFSR");

    /*　模块将应答:
        
    +CIFSR:STAIP,"192.168.1.18"
    +CIFSR:STAMAC,"18:fe:34:a6:44:75"
    
    OK    
    */

    _ip[0] = 0;
    _mac[0] = 0;
    for (i = 0; i < 6; i++)
    {
        ESP32_ReadLine(buf, sizeof(buf), 500);
        if (memcmp(buf, "+CIFSR:STAIP", 12) == 0)
        {

            for (m = 0; m < 20; m++)
            {
                temp = buf[14 + m];
                _ip[m] = temp;
                if (temp == '"')
                {
                    _ip[m] = 0;
                    ret = 1;
                    break;
                }
            }
        }
        else if (memcmp(buf, "+CIFSR:STAMAC,", 14) == 0)
        {
            for (m = 0; m < 20; m++)
            {
                temp = buf[15 + m];
                _mac[m] = temp;
                if (temp == '"')
                {
                    _mac[m] = 0;
                    break;
                }
            }
        }
        else if (memcmp(buf, "OK", 2) == 0)
        {
            break;
        }
    }
    return ret;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_SetLocalIP
*    功能说明: 设置本机IP地址和MAC
*    形    参: _ip : IP地址 
*              _gateway : 网关
*              _netmask : 子网掩码
*    返 回 值: 1 表示OK， 0 表示未知
*********************************************************************************************************
*/
uint8_t ESP32_SetLocalIP(uint8_t *_ip, uint8_t *_netmask, uint8_t *_gateway)
{
    char buf[64];

    // AT+CIPSTA_DEF="192.168.6.100","192.168.6.1","255.255.255.0"

    sprintf(buf, "AT+CIPSTA_DEF=\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"",
                    _ip[0], _ip[1], _ip[2], _ip[3],
                    _gateway[0], _gateway[1], _gateway[2], _gateway[3],
                    _netmask[0], _netmask[1], _netmask[2], _netmask[3]);
    ESP32_SendAT(buf);

    return ESP32_WaitResponse("OK", 200);
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_JoinAP
*    功能说明: 加入AP
*    形    参: _ssid : AP名字字符串
*              _pwd : 密码字符串
*              _timeout : 超时，ms
*    返 回 值: 
*                0 表示 0K, 
*                1 连接超时
*                2 密码错误
*                3 找不到目标AP
*                4 连接失败
*********************************************************************************************************
*/

uint8_t ESP32_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout)
{

    /*  如果已经连接上了，则会返回如下信息：
    
        (268)=>AT+CWJAP="MERCURY_603","123456887af"
        (269)=>

        (2488)<==WIFI CONNECTED

        (3208)<==WIFI GOT IP

        (3274)<==
        OK
    

        OK
        (320583)=>AT+CWJAP="Tenda_446248","123456887af"
        (320634)=>

        (320648)<==
        WIFI DISCONNECT

        (322820)<==WIFI CONNECTED
        WIFI GOT IP

        (323649)<==
        OK    
        
        ----------- SSID 和 密码不对的情况 ----------
        (158892)=>AT+CWJAP="Tenda_446248","123456887af"
        (158893)=>


        (173898)<==+CWJAP:3

        FAIL
    */

    char buf[64];
    uint8_t err_code = 4;

    if (ESP32_ValidSSID(_ssid) == 0 || ESP32_ValidPassword(_pwd) == 0)
    {
        //printf("WiFi SSID和密码参数异常\r\n");
        return 0;
    }

    sprintf(buf, "AT+CWJAP_CUR=\"%s\",\"%s\"", _ssid, _pwd);
    ESP32_SendAT(buf);

    while (1)
    {
        if (ESP32_ReadLine(buf, 64, _timeout))
        {
            if (memcmp(buf, "AT+CWJAP_CUR", 12) == 0) /* ATE1回显情况，第1次读到的是 命令本身 */
            {
                ;
            }
            else if (memcmp(buf, "WIFI CONNECTED", 14) == 0)
            {
                ;
            }
            else if (memcmp(buf, "OK", 2) == 0)
            {
                return 0; /* 连接AP OK */
            }
            else if (memcmp(buf, "+CWJAP:", 7) == 0)
            {
                err_code = buf[7] - '0'; /* 错误代码，ASCII */
            }
            else if (memcmp(buf, "FAIL", 4) == 0)
            {
                break;
            }
        }
        else /* 读超时 */
        {
            err_code = 1;
            break;
        }
    }
    return err_code;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_QuitAP
*    功能说明: 退出当前的AP连接
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_QuitAP(void)
{
    ESP32_SendAT("AT+ CWQAP");
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_ScanAP
*    功能说明: 扫描AP。结果存放在_pList 结构体数组. 此函数会占用最长5秒时间。直到收到OK或ERROR。
*    形    参: _pList : AP列表数组;
*              _MaxNum : 搜索的最大AP个数。主要是防止缓冲区溢出。
*    返 回 值: -1 表示失败; 0 表示搜索到0个; 1表示1个。
*********************************************************************************************************
*/
int16_t ESP32_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum)
{
    uint16_t i;
    uint16_t count;
    char buf[128];
    WIFI_AP_T *p;
    char *p1, *p2;
    uint16_t timeout;

    buf[127] = 0;
    ESP32_SendAT("AT+CWLAP");

    p = (WIFI_AP_T *)_pList;
    count = 0;
    timeout = 8000;
    for (i = 0; i < _MaxNum; i++)
    {
        ESP32_ReadLine(buf, 128, timeout);
        if (memcmp(buf, "OK", 2) == 0)
        {
            break;
        }
        else if (memcmp(buf, "ERROR", 5) == 0)
        {
            break;
        }
        else if (memcmp(buf, "+CWLAP:", 7) == 0)
        {
            p1 = buf;

            /* +CWLAP:(4,"BaiTu",-87,"9c:21:6a:3c:89:52",1) */
            /* 解析加密方式 */
            p1 = strchr(p1, '('); /* 搜索到(*/
            p1++;
            p->ecn = str_to_int(p1);

            /* 解析ssid */
            p1 = strchr(p1, '"'); /* 搜索到第1个分号 */
            p1++;
            p2 = strchr(p1, '"'); /* 搜索到第2个分号 */
            memcpy(p->ssid, p1, p2 - p1);
            p->ssid[p2 - p1] = 0;

            /* 解析 rssi */
            p1 = strchr(p2, ','); /* 搜索到逗号*/
            p1++;
            p->rssi = str_to_int(p1);

            /* 解析mac */
            p1 = strchr(p1, '"'); /* 搜索到分号*/
            p1++;
            p2 = strchr(p1, '"'); /* 搜索到分号*/
            memcpy(p->mac, p1, p2 - p1);
            p->mac[p2 - p1] = 0;

            /* 解析ch */
            p1 = strchr(p2, ','); /* 搜索到逗号*/
            p1++;
            p->ch = str_to_int(p1);

            /* 有效的AP名字 */
            count++;

            p++;

            timeout = 2000;
        }
    }

    return count;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_RxData
*    功能说明: 读取wifi模块数据，并分析数据类型（+IPD数据包 或者其他指令的应答包）
*    形    参: _pRxBuf : 接收到的数据存放在此缓冲区（已过滤0x0D,0x0A,  +IPD等字符）
*              _pDataLen : 数据长度存在此缓冲区 （
*             _link_id : 连接的id， 
*    返 回 值:  ESP_RX_NONE  表示无数据  
*              ESP_RX_BYTE   表示接收到字节（数据不全，未解码。 用于主程序判断超时）
*              ESP_RX_IPD    +IPD数据包， _pRxBuf 存放有效的TCP UDP数据体， _link_id存放连接ID
*              ESP_RX_OTHER  表示接收到其他字符串（比如 busy, connected, wdt reset)
*********************************************************************************************************
*/
uint8_t ESP32_RxData(uint8_t *_buf, uint16_t *_len, uint16_t _buf_size, uint8_t *_link_id)
{
    uint8_t ucData;
    uint8_t rx_flag = 0;
    static uint8_t s_flag = 0;
    static uint16_t s_data_len = 0; /* UDP TCP数据长度 */
    char *p1;
    static int32_t s_last_rx_time = 0;

    /* +IPD,0,7:ledon 1 */

    while (ESP32_GetChar(&ucData))
    {
        rx_flag = 1; /* 表示收到1个字节 */

        /* 如果上次收到的数据距今超过100ms，则重新做帧同步 */
        if (bsp_CheckRunTime(s_last_rx_time) > 1000)
        {
            s_flag = 0;
            *_len = 0;
        }

        if (s_flag == 0)
        {
            if (*_len < _buf_size)
            {
                if (ucData == 0x0D || ucData == 0xA)
                {
                    if (*_len >= 2)
                    {
                        s_data_len = 0;
                        return ESP_RX_OTHER; /* 接收到非数据包的应答 */
                    }
                    else
                    {
                        *_len = 0;
                        s_data_len = 0;
                    }
                }
                else
                {
                    _buf[(*_len)++] = ucData; /* 保存接收到的数据 */

                    if (_buf[0] == '+' && (*_len) > 7 && ucData == ':')
                    {
                        p1 = (char *)&_buf[5];
                        *_link_id = str_to_int(p1); /* 解析出连接id */

                        p1 = (char *)&_buf[7];
                        s_data_len = str_to_int(p1); /* 解析出数据包长度 */
                        s_flag = 1;                                     /* 进入数据包接收状态 */
                        *_len = 0;
                    }
                }
            }
        }
        else /* 这是接收 +IPD数据包的分之 */
        {
            if (*_len < _buf_size)
            {
                _buf[(*_len)++] = ucData; /* 保存接收到的数据 */

                if (*_len == s_data_len)
                {
                    s_flag = 0;
                    //*_len = 0;

                    return ESP_RX_IPD; /* 表示接收到+IPD数据包 */
                }
            }
            else
            {
                s_flag = 0;
            }
        }

        s_last_rx_time = bsp_GetRunTime();
    }

    if (rx_flag == 1)
    {
        return ESP_RX_BYTE; /* 表示收到至少1个字节 */
    }

    return ESP_RX_NONE; /* 表示未收到任何数据 */
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_GetIPStatus
*    功能说明: 查询网络状态
*    形    参: _mac : 
*    返 回 值: 0 表示失败。 1表示成功
*********************************************************************************************************
*/
uint8_t ESP32_GetIPStatus(uint8_t *_mac)
{
    /*
    正确的应答包：
    
    (4451)=>AT+CIPSTATUS
    (4451)=>
    STATUS:3        <- 
    +CIPSTATUS:0,"UDP","255.255.255.255",8080,6200,0
    +CIPSTATUS:4,"TCP","192.168.1.3",9800,37299,0

    OK
    
    ----------------------
    
    STATUS: 定义
        2: ESP32 station 已连接AP，获得IP地址
        3: 已建立TCP或UDP传输
        4：ESP32 已断开网络连接
        5：未连接到AP
    
    
    ----------------------- 断网，复位后 ---
    AT+CIPSTATUS
    STATUS:5

    OK
    */
    char buf[64];

    memset(_mac, 0, 6);
    ESP32_SendAT("AT+CIPSTAMAC_CUR?");
    while (1)
    {
        if (ESP32_ReadLine(buf, sizeof(buf), 200))
        {
            if (memcmp(buf, "+CIPSTAMAC_CUR:", 15) == 0)
            {
                /* 解析mac地址 */
                char *p = &buf[16];

                _mac[0] = TwoCharToInt(p);
                p += 3;
                _mac[1] = TwoCharToInt(p);
                p += 3;
                _mac[2] = TwoCharToInt(p);
                p += 3;
                _mac[3] = TwoCharToInt(p);
                p += 3;
                _mac[4] = TwoCharToInt(p);
                p += 3;
                _mac[5] = TwoCharToInt(p);
            }
            else if (memcmp(buf, "OK", 2) == 0)
            {
                return 1;
            }
        }
        else /* 读超时 */
        {
            break;
        }
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_ValidSSID
*    功能说明: 判断ssid字符串是否正确
*    形    参: _ssid : AP名字字符串
*    返 回 值: 1 表示 0K  0 表示无效
*********************************************************************************************************
*/
uint8_t ESP32_ValidSSID(char *_ssid)
{
    uint8_t i;

    for (i = 0; i < SSID_MAX_LEN + 2; i++)
    {
        if (_ssid[i] > 127)
        {
            return 0;
        }

        if (_ssid[i] == 0)
        {
            break;
        }
    }

    if (i <= SSID_MAX_LEN)
    {
        return 1;
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_ValidSSID
*    功能说明: 判断wifi密码字符串是否正确
*    形    参: _pass : AP密码字符串
*    返 回 值: 1 表示 0K  0 表示无效
*********************************************************************************************************
*/
uint8_t ESP32_ValidPassword(char *_pass)
{
    uint8_t i;

    for (i = 0; i < PASSWORD_MAX_LEN + 2; i++)
    {
        if (_pass[i] > 127)
        {
            return 0;
        }

        if (_pass[i] == 0)
        {
            break;
        }
    }

    if (i <= PASSWORD_MAX_LEN)
    {
        return 1;
    }
    return 0;
}

#if 0
/*
*********************************************************************************************************
*    下面的代码段是非阻塞，虚拟线程应用。 
***************************************************************************************
*/

/*
*********************************************************************************************************
*    函 数 名: ESP32_PT_JoinAP
*    功能说明: 加入AP， 虚拟线程。发完指令后，立即返回。
*    形    参: _ssid : AP名字字符串
*              _pwd : 密码字符串
*    返 回 值: 1 表示 0K  0 表示失败
*********************************************************************************************************
*/
void ESP32_PT_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout)
{
    char buf[64];

    if (ESP32_ValidSSID(_ssid) == 0 || ESP32_ValidPassword(_pwd) == 0)
    {
        printf("WiFi SSID和密码参数异常\r\n");
        return;
    }
    
    sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"", _ssid, _pwd);
    ESP32_SendAT(buf);
    
    s_tAT.Len1 = 0;
    s_tAT.Len2 = 0;
    s_tAT.Timeout = _timeout;
    
    s_tAT.pStr1 = "OK\r\n";
    s_tAT.pStr2 = "FAIL\r\n";
    
    s_tAT.LastTime = bsp_GetRunTime();
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_PT_WaitJoinAP
*    功能说明: 等待模块应答
*    形    参: _StrOk : 收到此字符串，表示执行OK
*    返 回 值: PT_NULL 表示需要继续等待  1 表示 0K  2 表示失败  3 表示超时
*********************************************************************************************************
*/
uint8_t ESP32_PT_WaitResonse(void)
{
    {    
        uint8_t ucData;
        uint8_t ch;
        
        if (ESP32_GetChar(&ucData))
        {                            
            /* 比较执行成功的应答字符串 */
            ch = s_tAT.pStr1[s_tAT.Len1];
            if (ucData == ch)
            {
                s_tAT.Len1++;
                
                if (s_tAT.Len1 == strlen(s_tAT.pStr1))
                {
                    return PT_OK;
                }
            }
            else
            {
                s_tAT.Len1 = 0;
            }

            /* 比较执行失败的应答字符串 */
            ch = s_tAT.pStr2[s_tAT.Len2];
            if (ucData == ch)
            {
                s_tAT.Len2++;
                
                if (s_tAT.Len2 == strlen(s_tAT.pStr2))
                {
                    return PT_ERR;
                }
            }
            else
            {
                s_tAT.Len2 = 0;
            }        
        }

        if (s_tAT.Timeout > 0)
        {
            if (bsp_CheckRunTime(s_tAT.LastTime) >= s_tAT.Timeout)
            {
                return PT_TIMEOUT;    /* 命令超时 */
            }
        }
    }
    return PT_NULL;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_PT_SmartStrat
*    功能说明: 通知模块进入智能连接状态
*    形    参: mode : 智能连接的方式。
*                0: 使用 安信可 AI-LINK技术
*                1: 使用 ESP-TOUCH技术
*                2: 使用 AIR-KISS
*    返 回 值: 无
*********************************************************************************************************
*/
void ESP32_PT_SmartStrat(uint8_t _mode)
{
    ESP32_SendAT("AT+CWSMARTSTART=0");

    s_tAT.Len1 = 0;
    s_tAT.Len2 = 0;
    s_tAT.Len3 = 0;    
    
    s_tAT.pStr1 = "\r\nSSID:";
    s_tAT.pStr2 = "\r\nPASSWORD:";
    s_tAT.pStr3 = "OK\r\n";    
    
    s_tAT.RunFirst = 1;
}

/*
*********************************************************************************************************
*    函 数 名: ESP32_PT_SmartWait
*    功能说明: 读取手机发来的ssid 和密码。  ESP32_PT_SmartStrat执行一次后，轮流执行ESP32_PT_SmartWait函数。
*    形    参: _ssid : 保存ssdi的缓冲区
*             _password : 保存密码的缓冲区。 应用程序至少要开辟  PASSWORD_MAX_LEN + 1的缓冲区存放密码
*    返 回 值: 0 表示无效。 1表示成功配置。
*********************************************************************************************************
*/
uint8_t ESP32_PT_SmartWait(char *_ssid,  char *_password)
{
    /* 指令执行过程 : 

    AT+CWSMARTSTART=0

    OK
    
    -------当手机发送配置信息过来是，模块会返回 --------
    SMART SUCCESS
    SSID:Tenda_4FD138
    PASSWORD:123456887mdh

    OK
            
    */    
    uint8_t ucData;
    uint8_t ch;
    static uint8_t s_ssid_len;
    static uint8_t s_password_len;
    static uint8_t s_RxSSID;
    static uint8_t s_RxPASSWORD;
    
    if (s_tAT.RunFirst == 1)
    {
        s_tAT.RunFirst = 0;
        
        s_ssid_len = 0;
        s_password_len = 0;
        
        s_RxSSID = 0;
        s_RxPASSWORD = 0;        
    }
    
    if (ESP32_GetChar(&ucData))
    {                
        /* 接收保存 SSID */
        if (s_RxSSID == 1)
        {
            if (s_ssid_len < SSID_MAX_LEN)
            {
                _ssid[s_ssid_len++] = ucData;
            }
            
            if (ucData == 0x0D || ucData == 0x0A)
            {
                _ssid[s_ssid_len - 1] = 0;    /* 字符串末尾加0 */
                s_RxSSID = 2;        /* 表示成功接收到SSID */
            }
        }

        /* 接收保存密码 */
        if (s_RxPASSWORD == 1)
        {
            if (s_password_len < PASSWORD_MAX_LEN)
            {            
                _password[s_password_len++] = ucData;
            }
            
            if (ucData == 0x0D || ucData == 0x0A)
            {
                _password[s_password_len - 1] = 0;    /* 字符串末尾加0 */
                s_RxPASSWORD = 2;        /* 表示成功接收到密码*/
            }
        }        
        
        /* 比较第1个字符串 SSID: */
        ch = s_tAT.pStr1[s_tAT.Len1];
        if (ucData == ch)
        {
            s_tAT.Len1++;
            
            if (s_tAT.Len1 == strlen(s_tAT.pStr1))
            {
                s_RxSSID = 1;    /* 以后的数据是SSID, 直到 0x0D 0x0A结束  */
            }
        }
        else
        {
            s_tAT.Len1 = 0;
        }

        /* 比较第2个字符串  PASSWORD: */
        ch = s_tAT.pStr2[s_tAT.Len2];
        if (ucData == ch)
        {
            s_tAT.Len2++;
            
            if (s_tAT.Len2 == strlen(s_tAT.pStr2))
            {
                s_RxPASSWORD = 1;    /* 以后的数据是密码, 直到 0x0D 0x0A结束  */
            }
        }
        else
        {
            s_tAT.Len2 = 0;
        }
        
        /* 比较第3个字符串 OK */
        ch = s_tAT.pStr3[s_tAT.Len3];
        if (ucData == ch)
        {
            s_tAT.Len3++;
            
            if (s_tAT.Len3 == strlen(s_tAT.pStr3))
            {
                
                if (s_RxSSID == 2 && s_RxPASSWORD == 2)
                {                
                    return PT_OK;
                }
                
                /* 忽略第一个OK, 这是AT指令的 OK */
                s_tAT.Len3 = 0;
            }
        }
        else
        {
            s_tAT.Len3 = 0;
        }            
    }
    return PT_NULL;
}
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
