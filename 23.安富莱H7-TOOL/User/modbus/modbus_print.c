/*
*********************************************************************************************************
*
*    模块名称 : MODBUS发送print数据
*    文件名称 : modbus_print.c
*    版    本 : V1.0
*    说    明 : 将print内容包装为modbus标准帧，由H7-TOOL主动发送给PC机，支持USB链路或者UDP链路
*
*    Copyright (C), 2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "Includes.h"
#include "modbus_host.h"
#include "modbus_print.h"
#include "net_udp.h"

#define PRINT_FIFO_SIZE 8*1024

typedef struct
{
    uint8_t  TxBuf[PRINT_FIFO_SIZE];
    uint32_t Write;
    uint32_t Read;
    int32_t Count;
}PRINT_FIFO_T;

static PRINT_FIFO_T s_tPrintFiFo;

static void print_send(void);

extern void udp_print_send(uint8_t *_buf, uint16_t _len);
    
/*
*********************************************************************************************************
*    函 数 名: MODH_Send61H
*    功能说明: 发送61H数据包, 主要用以将print信息传送到上位机。不要求应答
*    形    参: _Ch : 通道号, 0表示print， 其他值功能保留
*              _TxBuf : 数据缓冲区
*              _TxLen : 长度, 0-1024
*    返 回 值: 无
*********************************************************************************************************
*/
void MODH_Send61H(uint8_t _Ch, uint8_t *_TxBuf, uint16_t _TxLen)
{
    /*
        H7-TOOL主动发送: 
            01  ; 从机地址 ，为1
            61  ; 功能码
            00  ; 通道号，00表示print
            01  : 长度高字节
            08  : 长度低字节
            xx xx xx xx ... : 数据体           
            CC CC : CRC16
    */
    uint16_t i;
    
    g_tModH.TxCount = 0;
    g_tModH.TxBuf[g_tModH.TxCount++] = g_tParam.Addr485;        /* 本机地址 */
    g_tModH.TxBuf[g_tModH.TxCount++] = 0x61;                         /* 功能码 */
    g_tModH.TxBuf[g_tModH.TxCount++] = _Ch;
    g_tModH.TxBuf[g_tModH.TxCount++] = _TxLen >> 8;
    g_tModH.TxBuf[g_tModH.TxCount++] = _TxLen;
        
    for (i = 0; i < _TxLen; i++)
    {
        g_tModH.TxBuf[g_tModH.TxCount++] = _TxBuf[i];
    }

    MODH_SendWithCRC();
}

/*
*********************************************************************************************************
*    函 数 名: MODH_PrintByte
*    功能说明: 打印的字符填入FIFO
*    形    参: _ch : 输入字符 
*    返 回 值: 无
*********************************************************************************************************
*/
void MODH_PrintByte(char _ch)
{ 
    uint16_t stime;
    
    s_tPrintFiFo.TxBuf[s_tPrintFiFo.Write] = _ch;
    if (++s_tPrintFiFo.Write >= PRINT_FIFO_SIZE)
    {
        s_tPrintFiFo.Write = 0;
    }
    s_tPrintFiFo.Count++;
        
    if (s_tPrintFiFo.Count >= 512)     
    {
        stime = 100;        /* 如果已经快满了，则尽快启动一次发送 */
    }
    else
    {
        if (_ch == 0x0A)
        {
            stime = 500;    /* 如果遇到回车换行符号，则 0.5ms 后启动硬件传输 */
        }
        else
        {
            stime = 5000;   /* 5ms后启动硬件传输 */
        }
    } 
    bsp_StartHardTimer(3, stime, print_send);      
}

/*
*********************************************************************************************************
*    函 数 名: MODH_PrintBuf
*    功能说明: 打印的字符填入FIFO
*    形    参: _ch : 输入字符 
*    返 回 值: 无
*********************************************************************************************************
*/
void MODH_PrintBuf(uint8_t *_buf, uint16_t _len)
{
    uint16_t i;
    uint8_t ch;
    uint8_t fCR = 0;
    uint32_t stime;
        
    for (i = 0; i < _len; i++)
    {
        ch = _buf[i];
        s_tPrintFiFo.TxBuf[s_tPrintFiFo.Write] = ch;
        if (++s_tPrintFiFo.Write >= PRINT_FIFO_SIZE)
        {
            s_tPrintFiFo.Write = 0;
        }
        s_tPrintFiFo.Count++;        
        
        if (ch == 0x0A)
        {
            fCR = 1;
        }
    }

    if (s_tPrintFiFo.Count >= PRINT_FIFO_SIZE / 2)     
    {
        stime = 100;        /* 如果已经快满了，则尽快启动一次发送 */
    }
    else
    {
        if (fCR == 1)
        {
            print_send();
            stime = 500;    /* 如果遇到回车换行符号，则 0.5ms 后启动硬件传输 */
        }
        else
        {
            stime = 5000;   /* 5ms后启动硬件传输 */
        }
    }
    
    bsp_StartHardTimer(3, stime, print_send);  
}

/*
*********************************************************************************************************
*    函 数 名: print_send
*    功能说明: 物理层发送数据
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void print_send(void)
{
    #if 1
        /*
            H7-TOOL主动发送: 
                01  ; 从机地址 ，为1
                61  ; 功能码
                00  ; 通道号，00表示print
                01  : 长度高字节
                08  : 长度低字节
                xx xx xx xx ... : 数据体           
                CC CC : CRC16
        */
        uint16_t crc;
        uint16_t len;

        udp_tx_len = 0;
        
        udp_tx_buf[udp_tx_len++] = g_tParam.Addr485;        /* 本机地址 */
        udp_tx_buf[udp_tx_len++] = 0x61;
        udp_tx_buf[udp_tx_len++] = 0x00;
        udp_tx_buf[udp_tx_len++] = 0;
        udp_tx_buf[udp_tx_len++] = 0; 

        len = 0;
        while (1)
        {        
            if (s_tPrintFiFo.Read == s_tPrintFiFo.Write)
            {
                break;
            }
            
            udp_tx_buf[udp_tx_len] = s_tPrintFiFo.TxBuf[s_tPrintFiFo.Read];
            if (++s_tPrintFiFo.Read >= PRINT_FIFO_SIZE)
            {
                s_tPrintFiFo.Read = 0;
            }
            s_tPrintFiFo.Count--;        
            
            len++;
            if (++udp_tx_len >= UDP_TX_SIZE - 7)
            {
                break;
            }
        }
        udp_tx_buf[3] = len >> 8;
        udp_tx_buf[4] = len;
        
        crc = CRC16_Modbus(udp_tx_buf, udp_tx_len);
        udp_tx_buf[udp_tx_len++] = crc >> 8;
        udp_tx_buf[udp_tx_len++]  = crc;    
        
        
        if (g_tVar.LinkState == LINK_RJ45_OK || g_tVar.LinkState == LINK_WIFI_OK)
        {
            udp_print_send(udp_tx_buf, udp_tx_len);
        }
        
        if (g_tVar.LinkState == LINK_USB_OK)
        {
            USBCom_SendBufNow(0, udp_tx_buf, udp_tx_len);
        }
    #else
        udp_tx_len = 0;
        while (1)
        {        
            if (s_tPrintFiFo.Read == s_tPrintFiFo.Write)
            {
                break;
            }
            
            udp_tx_buf[udp_tx_len] = s_tPrintFiFo.TxBuf[s_tPrintFiFo.Read];
            if (++s_tPrintFiFo.Read >= PRINT_FIFO_SIZE)
            {
                s_tPrintFiFo.Read = 0;
            }
            s_tPrintFiFo.Count--;        
            
            if (++udp_tx_len >= UDP_TX_SIZE)
            {
                break;
            }
        } 

        udp_print_send(udp_tx_buf, udp_tx_len);
    #endif    
}

extern uint8_t USBCom_SendBuf(int _Port, uint8_t *_Buf, uint16_t _Len);
extern void udp_print_send(uint8_t *_buf, uint16_t _len);

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
