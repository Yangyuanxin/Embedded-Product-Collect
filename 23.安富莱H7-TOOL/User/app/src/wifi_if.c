/*
*********************************************************************************************************
*
*    模块名称 : wifi模块驱动接口 interface
*    文件名称 : wifi_if.c
*    版    本 : V1.0
*    说    明 : 兼容不同厂家的模块，提供api共wifi通信程序使用。隔离WiFi模块之间的差异。
*    修改记录 :
*        版本号  日期       作者    说明
*        V1.0    2015-12-17 armfly  正式发布
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"

uint8_t link_id = 0xFF; /* 当前的TCP连接id,  0-4有效。  FF表示无TCP连接 */
uint8_t wifi_state = WIFI_STOP;

/* 记录发送缓冲区和数据长度 */
uint8_t s_wifi_send_req = 0;
uint8_t *s_wifi_send_buf;
uint8_t s_wifi_send_len;

uint8_t wifi_link_tcp_ok = 0;
uint8_t wifi_join_ap_ok = 0;

uint8_t wifi_server_ack_ok = 0; /* 发送到平台，平台应答OK */

void wifi_ReprotIOEvent(void);
void wifi_ReprotIOEvent(void);
void wifi_WatchDog(void);
uint8_t wifi_SendRegisterPackage(void);

void wifi_Poll(void);

// wifi正在连接中....
void wifi_led_joinap(void)
{
    PERIOD_Start(&g_tRunLed, 50, 30, 0); /* 连接wifi过程中快速闪烁 */
}

// wifi成功， home wifi模式
void wifi_led_ok(void)
{
    PERIOD_Stop(&g_tRunLed); // 连接成功关闭LED
                                                     //PERIOD_Start(&g_tRunLed, 500, 1500, 0);    /* 每2秒闪烁一次 */
}

// 连接TCP Server 失败
void wifi_led_err_link_tcp_server(void)
{
    PERIOD_Start(&g_tRunLed, 250, 250, 0); /* 每500ms闪烁一次 */
}

// wifi成功, P2P 模式
void wifi_led_ok_p2p(void)
{
    PERIOD_Start(&g_tRunLed, 200, 800, 0); /* 每1秒闪烁一次 */
}

// wifi连接失败
void wifi_led_err(void)
{
    //PERIOD_Stop(&g_tWiFiLed);
}

// 收到指令，闪烁1次
void wifi_led_rx_data(void)
{
    PERIOD_Start(&g_tRunLed, 100, 10, 1); // 3秒钟才一个数据，太慢了，干脆不只是数据流了
}

// 发送指令时闪烁1次
void wifi_led_tx_data(void)
{
    PERIOD_Start(&g_tRunLed, 100, 10, 1); // 3秒钟才一个数据，太慢了，干脆不只是数据流了
}

uint8_t wifi_WatchDog_PT(uint8_t _init);

/*
*********************************************************************************************************
*    函 数 名: wifi_task
*    功能说明: NB73任务，插入bsp_Idle()执行。！！！注意，此函数内部不得有 bsp_Idle调用，避免嵌套.
*    形    参: 无
*    返 回 值: 0:不满足条件  1:成功检测到字符串  2:超时了
*********************************************************************************************************
*/
uint8_t wifi_task(void)
{
    static int32_t s_last_time = 0;
    char buf[64];
    static uint8_t retry = 0;
    uint16_t rx_len;
    uint8_t re;

    if (g_tVar.WiFiRecivedIPD == 1) /* 收到UDP. TCP数据包 */
    {
        g_tVar.WiFiRecivedIPD = 0;

        wifi_led_rx_data(); /* 数据led闪一下 */

        wifi_server_ack_ok = 1; /* 2019-01-17, 收到平台数据包， 简单处理，未做校验 */

        return 1;
    }

    //    g_tVar.WiFiIdleDoRS485 = 1;        /* bsp_Idle 执行MODBUS RTU 485 */

    switch (wifi_state)
    {
        case WIFI_STOP:
            break;

        case WIFI_INIT:
            //            g_tVar.WiFiDebugEn  = 1;

            wifi_led_joinap(); /* 连接AP过程，快闪 */

            comSetBaud(COM_ESP32, 115200);
            bsp_InitESP32(); /* 内部有 20ms 延迟 */
            s_last_time = bsp_GetRunTime();
            WIFI_CheckAck("", 0); /* 0参数表示初始化函数内部的静态变量 */
            wifi_state++;
            break;

        case WIFI_INIT + 1:
            re = WIFI_CheckAck("ready", 300);
            if (re == 1)
            {
                wifi_state++; /* 收到正确应答 */
            }
            else if (re == 2)
            {
                wifi_state = WIFI_INIT; /* 超时 */
            }
            break;

        case WIFI_INIT + 2:
            /* 关闭回显功能，主机发送的字符，模块无需返回 */
            ESP32_SendAT("ATE0");
            ESP32_WaitResponse("OK\r\n", 100);

            /* 获取MAC */
            {
                uint8_t mac[6];
                const uint8_t mac_0[6] = {0, 0, 0, 0, 0, 0};

                ESP32_GetMac(mac);

                if (memcmp(mac, g_tParam.WiFiMac, 6) != 0 && memcmp(mac, mac_0, 6) != 0)
                {
                    memcpy(g_tParam.WiFiMac, mac, 6);
                    SaveParam();
                }
            }

            /* 根据参数决定是否扮演softAP */
            if (g_tParam.APSelfEn == 1)
            {
                wifi_state = WIFI_SOFT_AP;
            }
            else
            {
                wifi_state = WIFI_LINK_AP;
            }
            break;

        case WIFI_LINK_AP:
            ESP32_SetWiFiMode(1); /* 1 = STA, 2 = SAP,  3 = SAP + Station模式 */

            if (g_tParam.DHCPEn == 0) /* DHCH = 0, 使用静态IP */
            {
                ESP32_SendAT("AT+CWDHCP_CUR=1,0");
                ESP32_WaitResponse("OK\r\n", 300);

                ESP32_SetLocalIP(g_tParam.WiFiIPAddr, g_tParam.WiFiNetMask, g_tParam.WiFiGateway); /* 设置静态IP */
            }
            else /* DHCH = 1, 使用动态IP */
            {
                ESP32_SendAT("AT+CWDHCP_CUR=1,1");
                ESP32_WaitResponse("OK\r\n", 300);
            }
            wifi_led_joinap(); /* 连接wifi过程中快速闪烁 */
            wifi_state++;
            break;

        case WIFI_LINK_AP + 1:
            if (ESP32_ValidSSID((char *)g_tParam.AP_SSID) == 0 || ESP32_ValidPassword((char *)g_tParam.AP_PASS) == 0)
            {
                wifi_state = WIFI_STOP; /* iFi SSID和密码参数异常 */
                break;
            }

            sprintf(buf, "AT+CWJAP_CUR=\"%s\",\"%s\"", g_tParam.AP_SSID, g_tParam.AP_PASS);
            ESP32_SendAT(buf);
            s_last_time = bsp_GetRunTime();
            wifi_state++;
            break;

        case WIFI_LINK_AP + 2:
            if (ESP32_ReadLineNoWait(buf, 64))
            {
                if (memcmp(buf, "OK", 2) == 0)
                {
                    wifi_state = WIFI_LINK_AP + 3; /* 连接AP OK */
                }
                else if (memcmp(buf, "WIFI CONNECTED", 14) == 0 || memcmp(buf, "WIFI GOT IP", 11) == 0)
                {
                    ; /* 连接过程中，正常应答，不理会，继续等待最后的OK */
                }
                else if (memcmp(buf, "+CWJAP:", 7) == 0 || memcmp(buf, "FAIL", 4) == 0 || memcmp(buf, "DISCONNECT", 10) == 0)
                {
                    wifi_state = WIFI_INIT; /* 连接失败 */
                }
            }

            if (bsp_CheckRunTime(s_last_time) > 20 * 1000) /* 超时 */
            {
                wifi_state = WIFI_INIT;
            }
            break;

        case WIFI_LINK_AP + 3:             /* 连接AP OK */
            g_tVar.HomeWiFiLinkOk = 1; /* 已连接到HOME WIFI */
            wifi_led_ok();                         /* LED熄灭 */

            ESP32_CIPMUX(1); /* 启用多连接模式 */
            //ESP32_CloseTcpUdp(LINK_ID_UDP_SERVER);

            /* 创建TCP服务器. */
            ESP32_CreateTCPServer(g_tParam.LocalTCPPort);

            /* 创建UDP监听端口, id = 0 */
            ESP32_CreateUDPServer(LINK_ID_UDP_SERVER, g_tParam.LocalTCPPort);

            wifi_state = WIFI_READY;
            break;

        /*---------------------------------------------------------------------------*/
        case WIFI_WATCH_DOG: /* WiFi看护 */
            retry = 0;
            wifi_state++;
            break;

        case WIFI_WATCH_DOG + 1: /* WiFi看护 */
            wifi_link_tcp_ok = 0;
            wifi_join_ap_ok = 0;
            ESP32_SendAT("AT+CIPSTATUS");
            s_last_time = bsp_GetRunTime();
            wifi_state++;
            break;

        case WIFI_WATCH_DOG + 2:
            while (1)
            {
                rx_len = ESP32_ReadLineNoWait(buf, 64);
                if (rx_len == 0)
                {
                    break;
                }
                else if (rx_len > 7 && memcmp(buf, "STATUS:", 7) == 0)
                {
                    /*  STATUS:3 */
                    if (buf[7] == '2' || buf[7] == '3' || buf[7] == '4')
                    {
                        wifi_join_ap_ok = 1; /* 连接AP ok */
                    }
                    else
                    {
                        if (g_tParam.APSelfEn == 1) /* 做SoftAP */
                        {
                            ; /* 不判断, 根据UDP状态判断 */
                        }
                        else /* 做STA站点 */
                        {
                            wifi_join_ap_ok = 0; /* 没有连接到AP */
                        }
                    }
                }
                else if (rx_len >= 18 && memcmp(buf, "+CIPSTATUS:0,\"UDP\"", 18) == 0)
                {
                    if (g_tParam.APSelfEn == 1)
                    {
                        wifi_join_ap_ok = 1;
                    }
                }
                else if (rx_len >= 18 && memcmp(buf, "+CIPSTATUS:4,\"TCP\"", 18) == 0)
                {
                    /* +CIPSTATUS:4,"TCP","192.168.1.3",9800,37299,0 */
                    wifi_link_tcp_ok = 1;
                }
                else if (rx_len >= 2 && memcmp(buf, "OK", 2) == 0)
                {
                    if (wifi_join_ap_ok == 1)
                    {
                        wifi_state = WIFI_READY;
                    }
                    else
                    {
                        //wifi_state = WIFI_INIT; 不要立即改变状态，等3次查询失败后再走
                    }
                }
                else if (rx_len >= 5 && memcmp(buf, "busy p...", 5) == 0) /* 内部忙 */
                {
                    ;
                }
            }

            if (bsp_CheckRunTime(s_last_time) > 100) /* 超时 */
            {
                if (++retry > 2)
                {
                    if (wifi_join_ap_ok == 0)
                    {
                        wifi_state = WIFI_INIT; /* 复位WIFI模块，重连AP */
                    }
                    else
                    {
                        wifi_state = WIFI_READY; /* 应该进不来 */
                    }
                }
                else
                {
                    wifi_state--;
                }
            }
            break;

        /*---------------------------------------------------------------------------*/
        case WIFI_READY: /* wifi 就绪 */
            wifi_Poll();
            break;
    }

    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: wifi_Start_SoftAP_Station
*    功能说明: 设置WIFI模块工作在SoftAP + Station模式
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void wifi_Start_SoftAP_Station(void)
{
    uint8_t re;

    ESP32_SetWiFiMode(2); /* 1 = STA, 2 = SAP,  3 = SAP + Station模式 */

    /*
    CPU ID :
        00 43 00 3B 57 18 52 34 30 38 20 33
        00 36 00 42 57 17 52 34 30 38 20 33
           *      *  *  *
    */

    /* 设Soft AP 的SSID 和密码 */
    {
        //        char ap_name[40];
        //        uint16_t sn0, sn1, sn2;
        //
        //        sn0 = *(uint16_t *)(0x1ffff7ac);
        //        sn1 = *(uint16_t *)(0x1ffff7ac + 2);
        //        sn2 = *(uint16_t *)(0x1ffff7ac + 4);
        //
        //        /* g_tParam1.GrillID 是8位ascii */
        //        sprintf(ap_name, "R%04X_%03d_%02X%02X%04X", HARD_MODEL,  g_tParam.Addr485, sn0 & 0xFF, sn1 & 0xFF, sn2);

        /* 设置AP的IP地址 */
        {
            char ip_str[20];

            sprintf(ip_str, "%d.%d.%d.%d", g_tParam.WiFiIPAddr[0], g_tParam.WiFiIPAddr[1], g_tParam.WiFiIPAddr[2], g_tParam.WiFiIPAddr[3]);
            ESP32_Set_AP_IP(ip_str);
        }

        /* 设置AP的名字和密码 */
        ESP32_Set_AP_NamePass((char *)g_tParam.AP_SSID, (char *)g_tParam.AP_PASS, 1, ECN_WPA2_PSK);
    }

    /* DHCH = 0 */
    ESP32_SendAT("AT+CWDHCP=2,0");
    ESP32_WaitResponse("OK\r\n", 1000);

    ESP32_CIPMUX(1); /* 启用多连接模式 */

    /* 创建UDP监听端口, id = 0 */
    re = ESP32_CreateUDPServer(LINK_ID_UDP_SERVER, g_tParam.LocalTCPPort);

    if (re == 1) /* 执行成功 */
    {
        wifi_led_ok();
    }
    else /* 执行失败 */
    {
        wifi_led_err();
    }
}

/*
*********************************************************************************************************
*    函 数 名: wifi_LinkHomeAP
*    功能说明: 连接到AP，开启UDP服务和TCP服务。两个都要用的。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t wifi_LinkHomeAP(void)
{
    uint8_t re;

    /* 2018-12-12 断网重连时，硬件复位WiFi模块 */
    {
        bsp_InitESP32();
        comClearRxFifo(COM_ESP32); /* 等待发送缓冲区为空，应答结束*/
        ESP32_PowerOn();
    }

    g_tVar.HomeWiFiLinkOk = 0; /* 已连接到HOME WIFI */

    /* 关闭回显功能，主机发送的字符，模块无需返回 */
    ESP32_SendAT("ATE0");
    ESP32_WaitResponse("OK\r\n", 100);

    /* 自己做SoftAP时，无需连接外网 */
    if (g_tParam.APSelfEn == 1)
    {
        wifi_Start_SoftAP_Station(); /* 自己扮演SoftAP, 创建UDP监听服务 */
        return 1;
    }

    /* 获取MAC */
    {
        uint8_t mac[6];
        const uint8_t mac_0[6] = {0, 0, 0, 0, 0, 0};

        ESP32_GetMac(mac);

        if (memcmp(mac, g_tParam.WiFiMac, 6) != 0 && memcmp(mac, mac_0, 6) != 0)
        {
            memcpy(g_tParam.WiFiMac, mac, 6);
            SaveParam();
        }
    }

    ESP32_SetWiFiMode(1); /* 1 = STA, 2 = SAP,  3 = SAP + Station模式 */

    if (g_tParam.DHCPEn == 0)
    {
        /* DHCH = 0, 使用静态IP */
        ESP32_SendAT("AT+CWDHCP_DEF=1,0");
        ESP32_WaitResponse("OK\r\n", 300);

        ESP32_SetLocalIP(g_tParam.WiFiIPAddr, g_tParam.WiFiNetMask, g_tParam.WiFiGateway); /* 设置静态IP */
    }
    else
    {
        /* DHCH = 1, 使用动态IP */
        ESP32_SendAT("AT+CWDHCP_DEF=1,1");
        ESP32_WaitResponse("OK\r\n", 300);
    }

    wifi_led_joinap(); /* 连接wifi过程中快速闪烁 */

    re = ESP32_JoinAP((char *)g_tParam.AP_SSID, (char *)g_tParam.AP_PASS, 20000);
    if (re != 0)
    {
        g_tVar.HomeWiFiLinkOk = 0;
        g_tVar.RemoteTCPServerOk = 0;
        return 0;
    }
    if (re == 0) /* 执行成功 */
    {
        wifi_led_ok();
    }
    else /* 执行失败 */
    {
        wifi_led_err();
    }

    g_tVar.HomeWiFiLinkOk = 1; /* 已连接到HOME WIFI */

    /* 打印本地IP */
    {
        char ip[20];
        char mac[64];

        ESP32_GetLocalIP(ip, mac);

        ESP32_QueryIPStatus(); /* 查询当前IP链接状态 */
    }

    ESP32_CIPMUX(1); /* 启用多连接模式 */

    //    ESP32_CloseTcpUdp(LINK_ID_UDP_SERVER);

    /* 创建TCP服务器. */
    ESP32_CreateTCPServer(g_tParam.LocalTCPPort);

    /* 创建UDP监听端口, id = 0 */
    re = ESP32_CreateUDPServer(LINK_ID_UDP_SERVER, g_tParam.LocalTCPPort);

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: wifi_LinkSoftAP
*    功能说明: 连接ESP32软AP.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
uint8_t wifi_LinkSoftAP(void)
{
    uint8_t re;

    /* 关闭回显功能，主机发送的字符，模块无需返回 */
    ESP32_SendAT("ATE0");
    ESP32_WaitResponse("OK\r\n", 100);

    /* 获取MAC */
    {
        uint8_t mac[6];

        ESP32_GetMac(mac);

        if (memcmp(mac, g_tParam.WiFiMac, 6) != 0)
        {
            memcpy(g_tParam.WiFiMac, mac, 6);
            SaveParam();
        }
    }

    ESP32_SetWiFiMode(1); /* 1 = STA, 2 = SAP,  3 = SAP + Station模式 */

    /* DHCH = 0, 使用静态IP */
    ESP32_SendAT("AT+CWDHCP_DEF=1,0");
    ESP32_WaitResponse("OK\r\n", 300);

    ESP32_SetLocalIP(g_tParam.WiFiIPAddr, g_tParam.WiFiNetMask, g_tParam.WiFiGateway); /* 设置静态IP */

    wifi_led_joinap(); /* 连接wifi过程中快速闪烁 */

    re = ESP32_JoinAP((char *)g_tParam.AP_SSID, (char *)g_tParam.AP_PASS, 20000);
    if (re != 0)
    {
        g_tVar.HomeWiFiLinkOk = 0;
        return 0;
    }
    if (re == 0) /* 执行成功 */
    {
        wifi_led_ok();
    }
    else /* 执行失败 */
    {
        wifi_led_err();
    }

    g_tVar.HomeWiFiLinkOk = 1; /* 已连接到HOME WIFI */

    ESP32_CIPMUX(1); /* 启用多连接模式 */

    ESP32_CloseTcpUdp(LINK_ID_UDP_SERVER);

    /* 创建UDP监听端口, id = 0 */
    re = ESP32_CreateUDPServer(LINK_ID_UDP_SERVER, g_tParam.LocalTCPPort);

    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: wifi_Poll
*    功能说明: 插入main主循环，处理WiFi指令
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
void wifi_Poll(void)
{
    static int32_t s_rx_ok_time = 0; /* 最后一次收发ok的时刻 */
    uint8_t rx_byte;
    uint8_t re;

    /* 数据包收发 */
    /* 分数据帧，和AT命令帧解析 */
    /* 解析 +IPD 数据帧 */

    while (1)
    {
        re = ESP32_GetChar(&rx_byte); /* GetChar函数内部会解析断网事件，TCP CLOSE事件 */
        if (re == 0)
        {
            /* 10秒没有数据收发，则检查网路状态 */
            if (bsp_CheckRunTime(s_rx_ok_time) > 10000)
            {
                wifi_WatchDog(); /* wifi 连接看护进程, 失去连接后，自动重连 */

                s_rx_ok_time = bsp_GetRunTime();
            }
            break;
        }
        else /* 收到数据 */
        {
            ;
        }
    }

    if (g_tVar.WiFiRecivedIPD == 1) /* 收到UDP. TCP数据包 */
    {
        g_tVar.WiFiRecivedIPD = 0;

        s_rx_ok_time = bsp_GetRunTime();

        wifi_led_rx_data(); /* 数据led闪一下 */

        return;
    }

    /* 断网或TCP服务关闭连接，则重启网络.  */
    {
        if (g_tVar.HomeWiFiLinkOk == 0)
        {
            wifi_state = WIFI_INIT; /* 重新连接AP */
            return;
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: wifi_SendBuf
*    功能说明: 向当前链接发送一串数据
*    形    参: 
*    返 回 值: 无
*********************************************************************************************************
*/
void wifi_SendBuf(uint8_t *str, uint16_t len)
{
    /* 等待WIFI就绪 */
    while (wifi_state != WIFI_READY)
    {
        wifi_task();
    }

    if (link_id < 5)
    {
        if (ESP32_SendTcpUdp(link_id, str, len) == 0)
        {
            /* 如果发送失败，则需要复位wifi模块 */
            wifi_LinkHomeAP();

            ESP32_SendTcpUdp(link_id, str, len); /* V2.14增加 2018-12-12 */
        }
        else
        {
            wifi_led_tx_data(); /* tx数据led闪一下 */
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: wifi_WatchDog
*    功能说明: WiFi连接监控程序。监视是否连接到AP， 监视是否连接到TCP 服务器.
*    形    参: 无。
*    返 回 值: 无
*********************************************************************************************************
*/
void wifi_WatchDog(void)
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

/*
        SoftAP , 当客户端接入时：
                
        +STA_DISCONNECTED:"18:fe:34:d1:b0:07"
        +STA_CONNECTED:"18:fe:34:d1:b0:07"

    
    */

/* 停止通信后10秒，开始定时扫描网络状态 */
#if 1
    {
        uint8_t buf[64];
        uint16_t rx_len;
        uint8_t net_status = 0;
        uint8_t link_ap_ok = 0;
        uint8_t i;

        for (i = 0; i < 3; i++)
        {
            ESP32_SendAT("AT+CIPSTATUS");
            while (1)
            {
                rx_len = ESP32_ReadLine((char *)buf, sizeof(buf), 100);
                if (rx_len == 0) /* 超时没有收到OK */
                {
                    break;
                }
                else if (rx_len > 7 && memcmp(buf, "STATUS:", 7) == 0)
                {
                    /*  STATUS:3 */
                    net_status = buf[7];

                    if (net_status == '2' || net_status == '3' || net_status == '4')
                    {
                        link_ap_ok = 1; /* 连接AP ok */
                    }
                    else
                    {
                        if (g_tParam.APSelfEn == 1) /* 做SoftAP */
                        {
                            ; /* 不判断, 根据UDP状态判断 */
                        }
                        else /* 做STA站点 */
                        {
                            link_ap_ok = 0; /* 没有连接到AP */
                        }
                    }
                }
                else if (rx_len >= 18 && memcmp(buf, "+CIPSTATUS:0,\"UDP\"", 18) == 0)
                {
                    if (g_tParam.APSelfEn == 1)
                    {
                        link_ap_ok = 1;
                    }
                }
                //                else if (rx_len >= 18 && memcmp(buf, "+CIPSTATUS:4,\"TCP\"", 18) == 0)
                //                {
                //                    /* +CIPSTATUS:4,"TCP","192.168.1.3",9800,37299,0 */
                //                    tcp_ok = 1;
                //                }
                else if (rx_len >= 2 && memcmp(buf, "OK", 2) == 0)
                {
                    /* 不需要连接到TCP服务器 */
                    {
                        if (link_ap_ok == 1)
                        {
                            i = 4; /* 只要有1次成功就退出for */
                        }
                    }
                    break;
                }
            }
        }

        if (link_ap_ok == 0)
        {
            wifi_LinkHomeAP(); /* 阻塞，等待连接AP */
        }
        else
        {
            ;
        }
    }
#endif
}

/*
*********************************************************************************************************
*    函 数 名: wifi_DebugATCommand
*    功能说明: 解析485收到的MODBUS 数据包，如果是AT指令，则透传给WiFi模块.
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void wifi_DebugATCommand(uint8_t *_rxbuf, uint16_t _rxlen)
{
    if (_rxbuf[0] != 'A' || _rxbuf[1] != 'T')
    {
        return; /* 不是AT指令不透传 */
    }

    if (_rxbuf[_rxlen - 1] != 0x0A)
    {
        return; /* 末尾不是回车换行 */
    }

    /* 120秒内开启透传 */

    if (g_tVar.WiFiDebugEn == 1)
    {
        //    g_tVar.WiFiDebugTime = bsp_GetRunTime();

        comSendBuf(COM_DEBUG, _rxbuf, _rxlen);

        /* 将全部的数据转发到WiFi模块 */
        comSendBuf(COM_ESP32, _rxbuf, _rxlen);
    }
}

#if 0
/********************************************** 保留代码 **********************/


char Rx_Protocol[STRINGLEN] = {0};

static uint8_t s_sethomewifi_state = 0;
static uint8_t s_sethomewifi_mode = 0;

static uint8_t s_setserver_state = 0;
static char s_setserver_ip[48];
static uint16_t s_setserver_port;

uint8_t g_wifi_tx_halt = 0;        /* wifi死机,需要重启， 发送失败则设为1 */

void sethomewifi_loop(void);
void setserver_loop(void);
void WifiWatchdog(void);

wifi正在等待智能配置.... 
void wifi_led_smartlink(void)
{
    PERIOD_Start(&g_tWiFiLed, 50, 100, 0);    /* 连接wifi过程中快速闪烁 */
}


/*
*********************************************************************************************************
*    函 数 名: putcharA
*    功能说明: 向当前链接发送一个字符串，0结束符
*    形    参: str : 0结束的字符串
*    返 回 值: 无
*********************************************************************************************************
*/
void putcharA(char *str) 
{
    uint16_t len;
    
    len = strlen(str);
    if (len > STRINGLEN)
    {
        len = STRINGLEN;
    }
    
    putcharbuf(str, len);
}


// 串口接收中断分之1， 用于炉子控制
void get_char(void)
{
    uint8_t r = 0;

    //uint8_t r = UART3_ReceiveData8();  // 把接收到的数据复制给变量
    //UART3_ClearFlag(UART3_FLAG_RXNE);
        
    if (Rx_len >= STRINGLEN)
    {
        memset(Rx_String,0,STRINGLEN);
        Rx_len = 0;
    }

    //if(r == 'U')
    //{
    //    KMemset(Rx_String,0,Rx_len);
    //    Rx_len = 0;
    //    Rx_String[Rx_len] = r;
    //    Rx_len++;
    //}
    //else 
    if(r == '!')
    {
        ParseString();
        memset(Rx_String,0,Rx_len);
        Rx_len = 0;
    }
    else
    {
        Rx_String[Rx_len] = r;
        Rx_len++;
    }

}


// 串口中断服务程序2 -  用于和远程服务器通信
void recvdata(void)
{
    uint8_t r;// = UART3_ReceiveData8();  // 把接收到的数据复制给变量
    //UART3_ClearFlag(UART3_FLAG_RXNE);

    if(Rx_len >= STRINGLEN)
    {
        memset(Rx_String,0,STRINGLEN);
        Rx_len = 0;
    }

    Rx_String[Rx_len] = r;
    Rx_len++;
    if(Rx_len >= 8)
    {
        //if(r == '1')
        {
            if(strncmp(&Rx_String[Rx_len - 8],GMGHeader,8) == 0)    // 将头放到最前端
            {
                //memcpy(Rx_String,Rx_String + Rx_len - 8,8);
                Rx_String[0] = Rx_String[Rx_len - 8];
                Rx_String[1] = Rx_String[Rx_len - 7];
                Rx_String[2] = Rx_String[Rx_len - 6];
                Rx_String[3] = Rx_String[Rx_len - 5];
                Rx_String[4] = Rx_String[Rx_len - 4];
                Rx_String[5] = Rx_String[Rx_len - 3];
                Rx_String[6] = Rx_String[Rx_len - 2];
                Rx_String[7] = Rx_String[Rx_len - 1];
                Rx_len = 8;
            }
        }

        if(strncmp(&Rx_String[Rx_len - 4],GMGTail,4) == 0) 
        {
            int index = backfindGMGHead(Rx_String,Rx_len);
            
            if(index != -1)
            {
                memcpy(Rx_Protocol,Rx_String + index,Rx_len - index);
                ParseProtocol();
            }
            memset(Rx_String,0,STRINGLEN);
            Rx_len = 0;

        }
    }

    //if(UART3_GetITStatus(UART3_IT_RXNE) != RESET)
    //{
    //    UART3_ClearITPendingBit(UART3_IT_RXNE); 
    //}

}

/* */
void factory_atoupdate(void) 
{
    uint16_t i; 
    
    printf("\r\n【factory_atoupdate】\r\n");
    
    DispUPD();
    for(i = 0;i < 30;i++)
    {
        Wait1s();
        if((i & 0x01) == 1)
        {
            bsp_LedOn();   
        }
        else
        {
            bsp_LedOff();   
        }
    }
        
    setserver("dlink_lyq","123456789a","50.63.156.227","5533");
        
   // setserver("hans203","123456887af","221.235.53.92","6699");
    for(i = 0;i < 20;i++)
    {
        Wait1s();
        if((i & 0x01) == 1)
        {
            bsp_LedOn();   
        }
        else
        {
            bsp_LedOff();   
        }
    }

    iWifiCtrl = WIFI_CMD_SOFT_UPDATE;
}


/* UH 指令，手机设置炉子，告诉炉子家庭AP的名字和密码  */
/*
*********************************************************************************************************
*    函 数 名: sethomewifi
*    功能说明: 设置WiFi为Home Wifi模式。可以设置UDP服务或TCP服务。两个都要用的。
*    形    参: mode ： 0表示TCP服务。 1表示UDP服务。
*    返 回 值: 无
*********************************************************************************************************
*/
void sethomewifi(int mode,char* ssid,char* password)
{    
    uint8_t re;
    
    printf("\r\n【设置为 home wifi 模式】\r\n");
    
    ESP32_PowerOn();    /* WIFI模块掉电复位一次 */
    
    /* 切换模式前，查询下当前状态 */
    {
        ESP32_QueryIPStatus();    /* 查询当前IP链接状态 */    
    }
    
    ESP32_SetWiFiMode(1);        /* 1 = STA, 2 = SAP,  3 = SAP + Station模式 */
    
    /* 加入家庭AP */
    printf("\r\n【加入Home AP...】\r\n");
    
    wifi_led_joinap();    /* 连接wifi过程中快速闪烁 */
    re = ESP32_JoinAP(g_tParam2.wifi_ssid, g_tParam2.wifi_password, 20000);


    /* 打印本地IP */
    {
        char ip[20];
        char mac[64];
    
        ESP32_GetLocalIP(ip, mac);

        ESP32_QueryIPStatus();    /* 查询当前IP链接状态 */        
    }
    
    /* 启动DHCH = 1 , 此命令会写flash参数 */
    //SP8266_SendAT("AT+CWDHCP=1,1");    
    //ESP32_WaitResponse("OK\r\n", 1000);
    
    ESP32_CIPMUX(1);        /* 启用多连接模式 */

    if (mode == 0)
    {
        /* 创建TCP服务器 */
        printf("\r\n【创建TCP服务器.】\r\n");
        re  = ESP32_CreateTCPServer(8080);        
    }
    else
    {
        /* 创建UDP监听端口 */
        printf("\r\n【创建UDP监听端口】\r\n");
        re  = ESP32_CreateUDPServer(0, 8080);
    }
    
    if (re == 1)    /* 执行成功 */
    {
        wifi_led_ok();
    }
    else    /* 执行失败 */
    {
        wifi_led_err();
    }    
}

/*
*********************************************************************************************************
*    函 数 名: sethomewifi_start
*    功能说明: 设置WiFi为Home Wifi模式。可以设置UDP服务或TCP服务。两个都要用的。 
*             非阻塞，结合sethomewifi_loop函数使用
*    形    参: mode ： 0表示TCP服务。 1表示UDP服务。 2表示同时建立2个服务。 
*    返 回 值: 无
*********************************************************************************************************
*/
void sethomewifi_start(int mode)
{    
    s_sethomewifi_state = 1;
    s_sethomewifi_mode = mode;

    g_tParam2.CommState = WIFI_STATE_HOMEWIFI;        /* 设置WIFI模块为STA模式  */
        
    SaveParam2();        
}

void sethomewifi_loop(void)
{    
    static int32_t s_last_time = 0;
    static uint8_t s_err_cout = 0;
    
    switch (s_sethomewifi_state)
    {
        case 0:            /* 执行完毕，休眠状态 */
            break;

        case 1:            /* 进入一次 */
            s_err_cout = 0;        /* 清出错次数 */
            s_sethomewifi_state++;
            break;
        
        case 2:        /* <------ 出错时，循环入口  */
            printf("\r\n【设置为 home wifi 模式】\r\n");
        
            ESP32_PowerOn();    /* WIFI模块掉电复位一次 */

            s_sethomewifi_state++;
            break;
    
        case 3:
            /* 切换模式前，查询下当前状态 */
            {
                ESP32_QueryIPStatus();    /* 查询当前IP链接状态 */    
            }                        
            ESP32_SetWiFiMode(1);        /* 1 = STA, 2 = SAP,  3 = SAP + Station模式 */        

            s_sethomewifi_state++;
            break;
            
        case 4:
            /* 加入家庭AP */
            printf("\r\n【加入Home AP...】\r\n");    
            wifi_led_joinap();    /* 连接wifi过程中快速闪烁 */
            ESP32_PT_JoinAP(g_tParam2.wifi_ssid, g_tParam2.wifi_password, 30000);    
            
            s_sethomewifi_state++;
            break;

        case 5:    /* 在这个状态等待AP应答 30s超时 */    
            {
                uint8_t re;
                
                re = ESP32_PT_WaitResonse();
                if (re == PT_NULL)    /* 空操作，需要继续等待 */
                {
                    ;
                }
                else if (re == PT_OK)    /* 成功执行 */
                {
                    s_sethomewifi_state++;        /* 执行成功 */
                }
                else if (re == PT_ERR || re == PT_TIMEOUT)
                {                    
                    wifi_led_err();    /* 执行失败 */
                    
                    s_sethomewifi_state = 10;    /* 失败 */
                }                
            }
            break;
        
        case 6:
            /* 打印本地IP */
            {
                char ip[20];
                char mac[64];
            
                ESP32_GetLocalIP(ip, mac);

                ESP32_QueryIPStatus();    /* 查询当前IP链接状态 */        
            }
            
            /* 启动DHCH = 1 , 此命令会写flash参数 */
            //SP8266_SendAT("AT+CWDHCP=1,1");    
            //ESP32_WaitResponse("OK\r\n", 1000);
            
            ESP32_CIPMUX(1);        /* 启用多连接模式 */

            {
                uint8_t re;
                
                if (s_sethomewifi_mode == 0)
                {
                    /* 创建TCP服务器 */
                    printf("\r\n【创建TCP服务器.】\r\n");
                    re  = ESP32_CreateTCPServer(8080);        
                }
                else if (s_sethomewifi_mode == 1)
                {
                    /* 创建UDP监听端口 */
                    printf("\r\n【创建UDP监听端口】\r\n");
                    re  = ESP32_CreateUDPServer(0, 8080);
                }
                else   /* 3 表示同时建立UDP和TCP */
                {
                    /* 创建UDP监听端口 */
                    printf("\r\n【创建UDP监听端口】\r\n");
                    re  = ESP32_CreateUDPServer(0, 8080);

                    /* 创建TCP服务器 */
                    printf("\r\n【创建TCP服务器.】\r\n");
                    re  = ESP32_CreateTCPServer(8080);                        
                }                
                
                if (re == 1)        /* 执行成功 */
                {
                    wifi_led_ok();
                    
                    s_sethomewifi_state = 0;    /* 成功结束 */
                }
                else                /* 执行失败 */
                {
                    wifi_led_err();
                    
                    s_sethomewifi_state = 10;    /* 失败 */
                }                                
            }                
            break;

        case 10:            /* 执行失败, 休息1秒后，重试 */
            if (++s_err_cout > 10)
            {
                s_sethomewifi_state = 0;
            }
            else
            {
                s_sethomewifi_state = 11;
                s_last_time = bsp_GetRunTime();    /* 保存当前运行时间 */    
            }
            break;
        
        case 11:
            if (bsp_CheckRunTime(s_last_time) > 1000)
            {
                //s_sethomewifi_state = 2;    2是 硬件复位WIFI模块
                s_sethomewifi_state = 4;
            }
            break;
    }
}

/*
*********************************************************************************************************
*    函 数 名: status_SmartLink
*    功能说明: 进入智能连接状态。此时wifi模块是STA模式，手机通过APP告诉wifi模块 AP的名字和密码。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void status_SmartLink(void)
{
    uint16_t time = 300;
    char ssid[SSID_MAX_LEN + 1];
    char password[PASSWORD_MAX_LEN + 1];
    uint8_t ucKey;
    uint8_t fQuit = 0;
    
    printf("\r\n【进入智能连接状态】\r\n");
    
    wifi_led_smartlink();    /* 自动闪灯 */

    ESP32_PowerOn();    /* WIFI模块掉电复位一次 */
    
    LED8_DispStr("CFG");
    
    bsp_StartAutoTimer(0, 1000);
    
    ESP32_PT_SmartStrat(0);    /* 开启智能连接。 0 = 安可信技术  1 = ESP-TOUCH   2 = AIR-KISS */
    
    while (fQuit == 0)        
    {
        /* 等待手机配置 */
        if (ESP32_PT_SmartWait(ssid, password) == PT_OK)
        {
            LED8_DispStr("SUCC");
            BEEP_Start(5, 10, 2);    /* 配置成功，叫2声 */
            
            /* 配置成功后，模块会自动退出smart link 模式 */
            //memcpy(g_tParam2.wifi_ssid, ssid, strlen(ssid));
            //memcpy(g_tParam2.wifi_password, password, strlen(password));
            strncpy(g_tParam2.wifi_ssid, ssid, SSID_MAX_LEN);
            strncpy(g_tParam2.wifi_password, password, PASSWORD_MAX_LEN);               

            g_tParam2.CommState = WIFI_STATE_HOMEWIFI;               
            
            SaveParam2();    /* 保存参数 */

            bsp_DelayMS(1000);    /* 延迟1秒 */
            
            /* 配置成功 */
            sethomewifi_start(2);    /* 连接到HOME WIFI , UDP服务，  ---> UDP + TCP同时建立 */
            break;
        }
        
        if (bsp_CheckTimer(0))
        {
            if (time > 0)
            {
                time--;
                //LED8_DispNumber(time, 0);
                
                if (time == 0)
                {
                    break;
                }
            }
        }
        
        ucKey = bsp_GetKey();
        switch (ucKey)
        {
            case KEY_PWR_DOWN:    /* 电源键按下 */
                fQuit = 1;
                break;

            case KEY_INC_DOWN:    /* 加1键按下 */
            case KEY_DEC_DOWN:    /* 减1键按下 */
                ESP32_PT_SmartStrat(0);
                time = 300;
                break;            
        }
    }
    bsp_StopTimer(0);
}

/*
*********************************************************************************************************
*    函 数 名: setserver
*    功能说明: 根据给定的ssid和密码连接到home ap， 然后访问远程TCP服务器。创建一个TCP客户端连接。 阻塞式。
*    形    参: ssid, [assword, 
*    返 回 值: 无
*********************************************************************************************************
*/
void setserver(char* ssid, char* password, char* serverip, char* port)
{
    uint8_t re;
    
    printf("\r\n【连接到Home WiFi，访问远程TCP服务器】\r\n");

    ESP32_PowerOn();    /* WIFI模块掉电复位一次 */
    
    ESP32_SetWiFiMode(1);        /* 1 = STA, 2 = SAP,  3 = SAP + Station模式 */
    
    /* 加入家庭AP -- 此时ESP32 可能复位重启 */
    wifi_led_joinap();    /* 连接wifi过程中快速闪烁 */
    re = ESP32_JoinAP(ssid, password, 20000);


    ESP32_CIPMUX(1);        /* 启用多连接模式 */
    
    /* 打印本地IP */
    {
        char ip[20];
        char mac[64];
    
        ESP32_GetLocalIP(ip, mac);
        
        ESP32_QueryIPStatus();    /* 查询当前IP链接状态 */
    }
    
    //ESP32_CIPMUX(1);        /* 启用多连接模式 */

    /* 链接到TCP服务器 */
    printf("\r\n【连接到远程TCP服务器.】\r\n");
    re = ESP32_LinkTCPServer(0, serverip, str_to_int(port));
    if (re == 1)    /* 执行成功 */
    {
        wifi_led_ok();
    }
    else    /* 执行失败 */
    {
        wifi_led_err();
    }    
    link_id = 0;
}

/*
*********************************************************************************************************
*    函 数 名: setserver_start
*    功能说明: 链接到远程服务器,TCP服务端口.  炉子做TCP客户端
*             非阻塞，结合sethomewifi_loop函数使用
*    形    参: serverip : 服务器IP地址
*              port ： 远程服务的TCP端口
*    返 回 值: 无
*********************************************************************************************************
*/
void setserver_start(char * serverip, char * port)
{    
    s_setserver_state = 1;
    strncpy(s_setserver_ip, serverip, sizeof(s_setserver_ip));
    s_setserver_port = str_to_int(port);    
    
    printf("\r\n【连接到Home WiFi，访问远程TCP服务器】\r\n");    
}

void setserver_start2(char * serverip, char * port)
{    
    s_setserver_state = 99;
    strncpy(s_setserver_ip, serverip, sizeof(s_setserver_ip));
    s_setserver_port = str_to_int(port);    
    
    printf("\r\n【连接到Home WiFi，访问远程TCP服务器】\r\n");    
}

/* 用于失败重新连接 远程服务器，仅用于程序升级中调用 */
void setserver_reconnect(void)
{            
    printf("\r\n【失败后重连，访问远程TCP服务器】\r\n");    
    ESP32_LinkTCPServer(2, s_setserver_ip, s_setserver_port);
}



/* 连接任务执行完毕 */
uint8_t setserver_finished(void)
{
    if (s_setserver_state == 0)
    {
        return 1;
    }
    return 0;
}

void setserver_loop(void)
{    
    static int32_t s_last_time = 0;
    static uint8_t s_err_cout = 0;
    
    switch (s_setserver_state)
    {
        case 0:            /* 执行完毕，休眠状态 */
            break;

        case 1:            /* 进入一次 */
            s_err_cout = 0;        /* 清出错次数 */
            s_setserver_state = 6;    /* 不复位WIFI模块 */
            break;

        case 99:
            s_err_cout = 0;        /* 清出错次数 */
            s_setserver_state = 2;    /* 复位wifi模块 */    
            break;
        
        case 2:        /* <------ 出错时，循环入口  */    
            ESP32_PowerOn();    /* WIFI模块掉电复位一次 */

            s_setserver_state++;
            break;
    
        case 3:
            /* 切换模式前，查询下当前状态 */
            {
                ESP32_QueryIPStatus();    /* 查询当前IP链接状态 */    
            }                        
            ESP32_SetWiFiMode(1);        /* 1 = STA, 2 = SAP,  3 = SAP + Station模式 */        

            s_setserver_state++;
            break;
            
        case 4:
            /* 加入家庭AP */
            printf("\r\n【加入Home AP...】\r\n");    
            wifi_led_joinap();    /* 连接wifi过程中快速闪烁 */
            ESP32_PT_JoinAP(g_tParam2.wifi_ssid, g_tParam2.wifi_password, 30000);    
            
            s_setserver_state++;
            break;

        case 5:    /* 在这个状态等待AP应答 30s超时 */    
            {
                uint8_t re;
                
                re = ESP32_PT_WaitResonse();
                if (re == PT_NULL)    /* 空操作，需要继续等待 */
                {
                    ;
                }
                else if (re == PT_OK)    /* 成功执行 */
                {
                    s_setserver_state++;        /* 执行成功 */
                }
                else if (re == PT_ERR || re == PT_TIMEOUT)
                {                    
                    wifi_led_err();    /* 执行失败 */
                    
                    s_setserver_state = 10;    /* 失败 */
                }                
            }
            break;
        
        case 6:
            /* 打印本地IP */
            {
                char ip[20];
                char mac[64];
            
                ESP32_GetLocalIP(ip, mac);

                ESP32_QueryIPStatus();    /* 查询当前IP链接状态 */        
            }
            
            /* 启动DHCH = 1 , 此命令会写flash参数 */
            //SP8266_SendAT("AT+CWDHCP=1,1");    
            //ESP32_WaitResponse("OK\r\n", 1000);
            
            ESP32_CIPMUX(1);        /* 启用多连接模式 */

            {
                uint8_t re;
                
                printf("\r\n【连接到远程TCP服务器.】\r\n");
                re = ESP32_LinkTCPServer(2, s_setserver_ip, s_setserver_port);  // ZHG 使用连接ID2，  id = 1是8080 tcp
                if (re == 1)    /* 执行成功 */
                {
                    wifi_led_ok();
                }
                else    /* 执行失败 */
                {
                    wifi_led_err();
                }    
                link_id = 2;
                
                if (re == 1)        /* 执行成功 */
                {
                    wifi_led_ok();
                    
                    s_setserver_state = 0;    /* 成功结束 */
                }
                else                /* 执行失败 */
                {
                    wifi_led_err();
                    
                    s_setserver_state = 10;    /* 失败 */
                }                                
            }                
            break;

        case 10:            /* 执行失败, 休息1秒后，重试10次 */
            if (++s_err_cout > 10)
            {
                s_setserver_state = 0;
            }
            else
            {
                s_setserver_state = 11;
                s_last_time = bsp_GetRunTime();    /* 保存当前运行时间 */    
            }
            break;
        
        case 11:
            if (bsp_CheckRunTime(s_last_time) > 1000)        /* 5秒后重试 */
            {
                //s_setserver_state = 2;
                s_setserver_state = 4;
            }
            break;
    }
}

#endif

/********************************************************/
