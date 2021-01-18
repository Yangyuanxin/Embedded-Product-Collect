/*
*********************************************************************************************************
*
*    模块名称 : CRC32算法，STM32硬件专用
*    文件名称 : crc32_stm32.c
*    版    本 : V1.0
*    说    明 : 
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2020-08-26  armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

#include "crc32_stm32.h"

/*
*********************************************************************************************************
*    函 数 名: STM32_CRC32_LE
*    功能说明: 硬件CRC32计算。不足4字节，补0. 小端模式传递数据
*    形    参: _pBuf : 缓冲区. 32bit对齐
*              _Len : 字节长度
*              _InitValue : CRC初值.   缺省是 0xFFFFFFFF
*    返 回 值: 无
*********************************************************************************************************
*/
uint32_t STM32_CRC32_LE(uint8_t *_pBuf, uint32_t _Len, uint32_t _InitValue)
{
    uint32_t i;
    uint32_t dd;
    uint8_t *p;
    
    p = (uint8_t *)&dd;
    
    __HAL_RCC_CRC_CLK_ENABLE();
    
    CRC->INIT = _InitValue;
    
    CRC->CR |= CRC_CR_RESET;
    
    for (i = 0; i < _Len / 4; i++)
    {
        p[0] = *_pBuf++;
        p[1] = *_pBuf++;
        p[2] = *_pBuf++;
        p[3] = *_pBuf++;
        
        CRC->DR = dd;
    }
    
    if (_Len % 4)
    {
        p[1] = 0; 
        p[2] = 0;
        p[3] = 0;
        for (i = 0; i < _Len % 4; i++)
        {
            p[i] = *_pBuf++;
        }
        CRC->DR = dd;
    }
    
    return (CRC->DR);
}

// 赋初值 0xFFFFFFFFUL
void CRC32_Init(void)
{
    __HAL_RCC_CRC_CLK_ENABLE();
    
    CRC->INIT = 0xFFFFFFFFUL;
    
    CRC->CR |= CRC_CR_RESET;
}

//更新数据
void CRC32_Update(char *_pBuf, uint32_t _Len)
{
    uint32_t i;
    uint32_t dd;
    uint8_t *p;
    
    p = (uint8_t *)&dd;
    
    for (i = 0; i < _Len / 4; i++)
    {
        p[0] = *_pBuf++;
        p[1] = *_pBuf++;
        p[2] = *_pBuf++;
        p[3] = *_pBuf++;        
        CRC->DR = dd;
    }
    
    // 补齐4字节校验
    if (_Len % 4)
    {
        p[1] = 0; 
        p[2] = 0;
        p[3] = 0;
        for (i = 0; i < _Len % 4; i++)
        {
            p[i] = *_pBuf++;
        }
        CRC->DR = dd;
    }
}

// 最终结果 异或 0xFFFFFFFFUL;
uint32_t CRC32_Final(void)
{
    uint32_t crc;
    
    crc = CRC->DR ^ 0xFFFFFFFFUL;
    
    return crc;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
