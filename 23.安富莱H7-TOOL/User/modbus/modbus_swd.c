/*
*********************************************************************************************************
*
*    模块名称 : modbus脱机烧录操作模块
*    文件名称 : modbus_swd.c
*    版    本 : V1.0
*    说    明 : 
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2020-08-06 armfly  正式发布
*
*    Copyright (C), 2020-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "main.h"
#include "modbus_file.h"
#include "param.h"
#include "modbus_slave.h"
#include "lua_if.h"
#include "prog_if.h"
#include "swd_flash.h"
#include "SW_DP_Multi.h"
#include "swd_host.h"
#include "swd_host_multi.h"
#include "stm8_flash.h"
#include "stm8_swim.h"
#include "n76e003_flash.h"
#include "w25q_flash.h"

/* 66H帧子功能码定义 */
enum
{
    H66_READ_MEM_INIT   = 0,    /* 读内存, */
    H66_READ_MEM        = 1,    /* 写内存 */
    H66_WRITE_MEM       = 2,    /* 写内存 */        
    
    H66_READ_DISP_MEM   = 0x0100,    /* 读显存 */ 
};

static void MODS66_ReadMem(void);
static void MODS66_WriteMem(void);
static void MODS66_ReadDispMem(void);

/*
*********************************************************************************************************
*    函 数 名: MODS_66H
*    功能说明: SWD内存读写操作通信接口
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void MODS_66H(void)
{
    /*
        主机发送: 小程序数据
            01  ; 站号
            66  ; 功能码
            0000  ; 子功能
                - 0表示读内存（需要初始化pg_init())
                - 1表示读内存 (连续读，无需初始化）
                - 2 写
            0000 0000 : 偏移地址 4字节
            0020 0000 : 数据长度 4字节
            CCCC      : CRC16
    
        从机应答:
            01  ; 从机地址
            66  ; 功能码    
            0000  ; 子功能
    
            00  ; 执行结果，0表示OK  1表示错误
            CCCC : CRC16
    */
    uint16_t func;      /* 子功能代码 */

    g_tModS.RspCode = RSP_OK;

    if (g_tModS.RxCount < 11)
    {
        g_tModS.RspCode = RSP_ERR_VALUE; /* 数据值域错误 */
        goto err_ret;
    }
    
    func = BEBufToUint16(&g_tModS.RxBuf[2]);
    
    if (func == H66_READ_MEM || func == H66_READ_MEM_INIT)
    {
        MODS66_ReadMem();         
    }
    else if (func == H66_WRITE_MEM)
    {
        MODS66_WriteMem();
    }
    else if (func == H66_READ_DISP_MEM)
    {
        MODS66_ReadDispMem();   
    }
    else 
    {
        g_tModS.RspCode = RSP_ERR_VALUE; /* 数据值域错误 */
    }
    
err_ret:
    if (g_tModS.RxBuf[0] != 0x00) /* 00广播地址不应答, FF地址应答g_tParam.Addr485 */
    {
        if (g_tModS.RspCode == RSP_OK) /* 正确应答 */
        {
            MODS_SendWithCRC();
        }
        else
        {
            MODS_SendAckErr(g_tModS.RspCode); /* 告诉主机命令错误 */
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: MODS66_ReadMem
*    功能说明: 读内存
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
extern void pg_init(void);
static void MODS66_ReadMem(void)
{
    /*
        主机发送: 小程序数据
            01  ; 站号
            66  ; 功能码
            0000  ; 子功能,
                - 0表示读内存（需要初始化pg_init())
                - 1表示读内存 (连续读，无需初始化）
                - 2 写
    
                - 0x0100   读显示屏缓冲区，用于截屏
            0000 0000 : 偏移地址 4字节
            0020 0000 : 数据长度 4字节
            CCCC      : CRC16
    
        从机应答:
            01  ; 从机地址
            66  ; 功能码    
            0000  ; 子功能
            0000 0000 : 偏移地址 4字节
            0020 0000 : 数据长度 4字节
    
            00  ; 执行结果，0表示OK  1表示错误
            
            XX   : 数据
            CCCC : CRC16
    */    
    uint16_t func;          /* 子功能代码 */
    uint32_t offset_addr;
    uint32_t package_len;   /* 本包数据长度 */
    uint8_t ch_num;
    
  
    func = BEBufToUint16(&g_tModS.RxBuf[2]);
    offset_addr = BEBufToUint32(&g_tModS.RxBuf[4]);
    package_len = BEBufToUint32(&g_tModS.RxBuf[8]);
    
    if (func == H66_READ_MEM_INIT)
    {
        pg_init();  /* 第1包数据，初始化硬件 */
    }
    
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            ch_num = g_gMulSwd.MultiMode;
        }
        else
        {
            ch_num = 1;
        }
        
        if (ch_num * package_len > 1024)
        {
            g_tModS.RspCode = RSP_ERR_VALUE; /* 数据值域错误 */
            return;
        }
    }
    
    /* SWD接口，读内存 */
    g_tModS.TxCount = 0;
    g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485; /* 本机地址 */
    g_tModS.TxBuf[g_tModS.TxCount++] = 0x66;                         /* 功能码 */
    g_tModS.TxBuf[g_tModS.TxCount++] = func >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = func;

    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 24;
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 16; 
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 0; 
    
    g_tModS.TxBuf[g_tModS.TxCount++] = package_len >> 24;
    g_tModS.TxBuf[g_tModS.TxCount++] = package_len >> 16; 
    g_tModS.TxBuf[g_tModS.TxCount++] = package_len >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = package_len >> 0;      
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {                
            if (MUL_swd_read_memory(offset_addr, s_lua_read_buf, package_len) == 0)
            {
                g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */
            }
            else
            {
                g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
            }
        }
        else
        {            
            if (swd_read_memory(offset_addr, s_lua_read_buf, package_len) == 0)
            {
                g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */
            }
            else
            {
                g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
            }
        }
        memcpy((char *)&g_tModS.TxBuf[g_tModS.TxCount], s_lua_read_buf, package_len * ch_num);
        g_tModS.TxCount += package_len * ch_num;        
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_ReadBuf(offset_addr, s_lua_read_buf, package_len) == 0)
        {
            g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */
        }
        else
        {
            g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
        }
        
        memcpy(&g_tModS.TxBuf[g_tModS.TxCount], s_lua_read_buf, package_len);
        g_tModS.TxCount += package_len;         
    }
    else if (g_tProg.ChipType == CHIP_NUVOTON_8051) 
    {
        if (N76E_ReadBuf(offset_addr, s_lua_read_buf, package_len) == 0)
        {
            g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */
        }
        else
        {
            g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
        }
        
        memcpy(&g_tModS.TxBuf[g_tModS.TxCount], s_lua_read_buf, package_len);
        g_tModS.TxCount += package_len;   
    }
    else if (g_tProg.ChipType == CHIP_SPI_FLASH) 
    {
        if (W25Q_ReadBuf(offset_addr, s_lua_read_buf, package_len) == 0)
        {
            g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */
        }
        else
        {
            g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
        }
        
        memcpy(&g_tModS.TxBuf[g_tModS.TxCount], s_lua_read_buf, package_len);
        g_tModS.TxCount += package_len;   
    }
    else
    {
        g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */    
    }
}

/*
*********************************************************************************************************
*    函 数 名: MODS66_WriteMem
*    功能说明: 写内存
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void MODS66_WriteMem(void)
{
    /*
        主机发送: 小程序数据
            01  ; 站号
            66  ; 功能码
            0001  ; 子功能,
                - 0表示读内存（需要初始化pg_init())
                - 1表示读内存 (连续读，无需初始化）
                - 2 写
            0000 0000 : 偏移地址 4字节
            0020 0000 : 数据长度 4字节
            ... 数据
            CCCC      : CRC16
    
        从机应答:
            01  ; 从机地址
            66  ; 功能码    
            0000  ; 子功能
            0000 0000 : 偏移地址 4字节
            0020 0000 : 数据长度 4字节
    
            00  ; 执行结果，0表示OK  1表示错误
            CCCC : CRC16
    */    
    uint16_t func;          /* 子功能代码 */
    uint32_t offset_addr;
    uint32_t package_len;   /* 本包数据长度 */
    uint8_t ch_num;
    uint8_t *pData;
      
    func = BEBufToUint16(&g_tModS.RxBuf[2]);
    offset_addr = BEBufToUint32(&g_tModS.RxBuf[4]);
    package_len = BEBufToUint32(&g_tModS.RxBuf[8]);
    
    pData = &g_tModS.RxBuf[12];
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
            ch_num = g_gMulSwd.MultiMode;
        }
        else
        {
            ch_num = 1;
        }
        
        if (ch_num * package_len > 1024)
        {
            g_tModS.RspCode = RSP_ERR_VALUE; /* 数据值域错误 */
            return;
        }
    }
    
    /* SWD接口，读内存 */
    g_tModS.TxCount = 0;
    g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485; /* 本机地址 */
    g_tModS.TxBuf[g_tModS.TxCount++] = 0x66;                         /* 功能码 */
    g_tModS.TxBuf[g_tModS.TxCount++] = func >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = func;

    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 24;
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 16; 
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 0; 
    
    g_tModS.TxBuf[g_tModS.TxCount++] = 0 >> 24;
    g_tModS.TxBuf[g_tModS.TxCount++] = 0 >> 16; 
    g_tModS.TxBuf[g_tModS.TxCount++] = 0 >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = 0 >> 0;     
    
    if (g_tProg.ChipType == CHIP_SWD_ARM)
    {
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {          
            if (MUL_swd_write_memory(offset_addr, pData, package_len) == 0)
            {
                g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */
            }
            else
            {
                g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
            }
        }
        else
        {            
            if (swd_read_memory(offset_addr, pData, package_len) == 0)
            {
                g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */
            }
            else
            {
                g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
            }
        }   
    }
    else if (g_tProg.ChipType == CHIP_SWIM_STM8)
    {
        if (SWIM_WriteBuf(offset_addr, pData, package_len) == 0)
        {
            g_tModS.TxBuf[g_tModS.TxCount++] = 0x01;    /*　出错 */
        }
        else
        {
            g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
        }     
    }
    else
    {
        ;
    }
}

/*
*********************************************************************************************************
*    函 数 名: MODS66_ReadDispMem
*    功能说明: 读显存，用于截屏
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
static void MODS66_ReadDispMem(void)
{
    /*
        主机发送: 小程序数据
            01  ; 站号
            66  ; 功能码
            0000  ; 子功能,
                - 0表示读内存（需要初始化pg_init())
                - 1表示读内存 (连续读，无需初始化）
                - 2 写
    
                - 0x0100   读显示屏缓冲区，用于截屏
            0000 0000 : 偏移地址 4字节
            0020 0000 : 数据长度 4字节
            CCCC      : CRC16
    
        从机应答:
            01  ; 从机地址
            66  ; 功能码    
            0000  ; 子功能
            0000 0000 : 偏移地址 4字节
            0020 0000 : 数据长度 4字节
    
            00  ; 执行结果，0表示OK  1表示错误
            
            XX XX ... XX : 数据
            
            CCCC : CRC16
    */    
    uint16_t func;          /* 子功能代码 */
    uint32_t offset_addr;
    uint32_t package_len;   /* 本包数据长度 */
      
    func = BEBufToUint16(&g_tModS.RxBuf[2]);
    offset_addr = BEBufToUint32(&g_tModS.RxBuf[4]);
    package_len = BEBufToUint32(&g_tModS.RxBuf[8]);
    
    /* 读内存 */
    g_tModS.TxCount = 0;
    g_tModS.TxBuf[g_tModS.TxCount++] = g_tParam.Addr485; /* 本机地址 */
    g_tModS.TxBuf[g_tModS.TxCount++] = 0x66;                         /* 功能码 */
    g_tModS.TxBuf[g_tModS.TxCount++] = func >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = func;

    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 24;
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 16; 
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = offset_addr >> 0; 
    
    g_tModS.TxBuf[g_tModS.TxCount++] = package_len >> 24;
    g_tModS.TxBuf[g_tModS.TxCount++] = package_len >> 16; 
    g_tModS.TxBuf[g_tModS.TxCount++] = package_len >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = package_len >> 0;      
    
    g_tModS.TxBuf[g_tModS.TxCount++] = 0x00;    /* 执行结果 00 */
    
    memcpy((char *)&g_tModS.TxBuf[g_tModS.TxCount], (uint8_t *)(0x30000000 + offset_addr), package_len);
    g_tModS.TxCount += package_len;   

}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
