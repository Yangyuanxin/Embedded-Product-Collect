/*
*********************************************************************************************************
*
*    模块名称 : ESP32 串口WIFI模块驱动程序
*    文件名称 : bsp_esp32_at.h
*    版    本 : V1.3
*    说    明 : 头文件
*
*    Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_ESP32_H
#define __BSP_ESP32_H

#define COM_ESP32 COM4 /* ESP32串口 */
#define COM_DEBUG COM2 /* 调试串口 */

/* 定义下面这句话, 将把收到的字符发送到调试串口1 */
#define ESP32_TO_COM1_EN

#define SSID_MAX_LEN 32            /* SSID最长32个字符，存储时需要32+1空间，末尾加0 */
#define PASSWORD_MAX_LEN 64 /* WIFI 密码 最长64个字符，存储时需要32+1空间，末尾加0 */

/* 设备结构体 */
typedef struct
{
    char ssid[33];     /* SSID是一个无线局域网络（WLAN）的名称。SSID是区分大小写的文本字符串，最大长度32个字符 */
    uint8_t ecn;         /* 加密方式 
                        0   OPEN
                        1   WEP
                        2   WPA_PSK
                        3   WPA2_PSK
                        4   WPA_WPA2_PSK
                    */
    int32_t rssi;         /* 信号强度 */
    uint8_t mac[20]; /* MAC地址字符串*/
    uint8_t ch;             /* 信道 */
} WIFI_AP_T;

/* 加密方式 */
enum
{
    ECN_OPEN = 0,
    ECN_WEP = 1,
    ECN_WPA_PSK = 2,
    ECN_WPA2_PSK = 3,
    ECN_WPA_WPA2_PSK = 4,
};

/* ESP32_PT_WaitResonse 函数的返回值定义 */
enum
{
    PT_NULL = 0, /* 空操作，需要继续等待 */
    PT_TIMEOUT,    /* 执行超时 */

    PT_OK,    /* 成功执行 */
    PT_ERR, /* 执行失败 */
};

/* ESP32_QueryIPStatus 函数的返回值定义 */
enum
{
    IPS_GET_IP = '2',         /* 获得IP */
    IPS_LINK_OK = '3',     /* 建立连接 */
    IPS_LINK_LOST = '4', /* 失去连接，模块可能看门狗复位 */

    IPS_BUSY = '8',        /* 模块内部忙， zhg 自定义的状态，不是命令正确应答 */
    IPS_TIMEOUT = '9' /* 命令应答超时， zhg 自定义的状态 */
};

/* ESP32_RxData 函数的返回值定义 */
enum
{
    ESP_RX_NONE = 0, /* 没有读到字节 */
    ESP_RX_BYTE,         /* 表示接收到字节（数据不全，未解码。 用于主程序判断超时） */
    ESP_RX_IPD,             /* +IPD数据包, TCP，UDP数据包  */
    ESP_RX_OTHER         /* 收到回车换行结束的应答字符串  */
};

/* 用于非阻塞函数 */
#define ACK_MAX_LEN 128
typedef struct
{
    uint8_t RxBuf[ACK_MAX_LEN];
    uint8_t Len1;
    uint8_t Len2;
    uint8_t Len3;

    char *pStr1;
    char *pStr2;
    char *pStr3;

    int32_t LastTime;
    uint16_t Timeout;

    uint8_t RunFirst;
} ESP32_PT_T;

/* 供外部调用的函数声明 */
void bsp_InitESP32(void);
void ESP32_Reset(void);
uint8_t ESP32_PowerOn(void);
void ESP32_PowerOff(void);
void ESP32_EnterISP(void);
void ESP32_ExitISP(void);
void ESP32_SendAT(char *_Cmd);

uint8_t ESP32_WaitResponse(char *_pAckStr, uint16_t _usTimeOut);
void ESP32_QuitAP(void);
int16_t ESP32_ScanAP(WIFI_AP_T *_pList, uint16_t _MaxNum);
uint8_t ESP32_RxData(uint8_t *_buf, uint16_t *_len, uint16_t _buf_size, uint8_t *_link_id);

uint8_t ESP32_CreateTCPServer(uint16_t _TcpPort);
uint8_t ESP32_CreateUDPServer(uint8_t _id, uint16_t _LaocalPort);
uint8_t ESP32_SendTcpUdp(uint8_t _id, uint8_t *_databuf, uint16_t _len);
uint8_t ESP32_SendUdp(char *_RemoteIP, uint16_t _RemotePort, uint8_t *_databuf, uint16_t _len);
void ESP32_CloseTcpUdp(uint8_t _id);
uint8_t ESP32_GetLocalIP(char *_ip, char *_mac);
uint8_t ESP32_QueryIPStatus(void);

uint8_t ESP32_JoinAP(char *_ssid, char *_pwd, uint16_t _timeout);
uint8_t ESP32_SetWiFiMode(uint8_t _mode);
uint8_t ESP32_CIPMUX(uint8_t _mode);
uint8_t ESP32_LinkTCPServer(uint8_t _id, char *_server_ip, uint16_t _TcpPort);

uint8_t ESP32_GetChar(uint8_t *_data);
void ESP32_SendBuf(uint8_t *_cmd, uint16_t _len);

uint8_t ESP32_Set_AP_IP(char *_ip);
uint8_t ESP32_Set_AP_NamePass(char *_name, char *_pwd, uint8_t _ch, uint8_t _ecn);

uint8_t ESP32_ValidSSID(char *_ssid);
uint8_t ESP32_ValidPassword(char *_pass);

uint8_t ESP32_SetLocalIP(uint8_t *_ip, uint8_t *_netmask, uint8_t *_gateway);

uint8_t ESP32_GetMac(uint8_t *_mac);
uint8_t ESP32_SetMac(uint8_t *_mac);

uint16_t ESP32_ReadLine(char *_pBuf, uint16_t _usBufSize, uint16_t _usTimeOut);
uint16_t ESP32_ReadLineNoWait(char *_pBuf, uint16_t _usBufSize);
uint8_t WIFI_CheckAck(uint8_t *_str, int32_t _timeout);

void ESP32_Reset(void);
void ESP32_EnterAT(void);
void ESP32_EnterISP(void);

#define RX_MSG_MAX 32
extern uint8_t g_RxMsgBuf[RX_MSG_MAX];
extern uint8_t g_RxMsgLen;

extern ESP32_PT_T s_tAT; /* 用于非阻塞模式执行AT指令 */

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
