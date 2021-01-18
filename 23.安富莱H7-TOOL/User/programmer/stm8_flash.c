/*
*********************************************************************************************************
*
*    模块名称 : stm8系列flash访问程序
*    文件名称 : stm8_flash.c
*    版    本 : V1.0
*    说    明 : STM8 flash编程函数。根据STM8各个系列的固件库汇总而成。
*    修改记录 :
*        版本号  日期       作者    说明
*        V1.0    2020-02-08 armfly  原创。
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "DAP_config.h"
#include "stm8_swim.h"
#include "stm8_flash.h"
#include "stm8.h"

///* STM8S 寄存器定义 */
//#define STM8_FLASH_OPT		    0x00004800
//#define STM8_FLASH_PUKR		    0x00005062
//#define STM8_FLASH_DUKR         0x00005064
//#define STM8_FLASH_CR2          0x0000505B
//#define STM8_FLASH_IAPSR        0x0000505f
//#define STM8_FLASH_FPR          0x0000505D
//#define CLK_SWIMCCR             0x000050CD
//#define SWIM_CSR                0x00007F80
//#define DM_CSR2                 0x00007F99

//#define STM8_FLASH_EOF          0x04
//#define STM8_FLASH_WR_PG_DIS    0x01
//#define STM8_FLASH_HVOFF        0x40

//#define PAGE_BLOCK_SIZE_64      64
//#define PAGE_BLOCK_SIZE_128     128

//static const uint8_t OPT_UNL[16] = {0x00,0x00,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0x55};
//static const uint8_t OPT_LCK[16] = {0xAA,0x00,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0x55};



uint16_t s_STM8_SerialType = 0;      /* 0 = STM8S, 1 = STM8L */  
uint16_t s_STM8_BlockSize = 64;      /* 块大小，64或者128字节 */
uint16_t s_STM8_HVOFF = 0;           /* 等待完成的标志选择 */

uint32_t s_STM8_FlashSize = 32 * 1024;      /* FLASH 容量 */
uint32_t s_STM8_EEPromSize = 32 * 1024;     /* EEPROM 容量 */

//#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S105) || 
//    defined(STM8S005) || defined (STM8AF52Ax) || defined (STM8AF62Ax) || defined(STM8AF626x)
//    FLASH_FLAG_HVOFF     = (uint8_t)0x40,     /*!< End of high voltage flag */
//#endif /* STM8S208, STM8S207, STM8S105, STM8AF62Ax, STM8AF52Ax, STM8AF626x */

#define STM8_ERASE_BLOCK_TIMEOUT     100    /* 100ms */

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_WaitForLastOperation
*    功能说明: 等待内部写操作完成.   STM8_FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
*    形    参: _uiTimeout : 超时时间，ms
*    返 回 值:
*********************************************************************************************************
*/
uint8_t STM8_FLASH_WaitForLastOperation(uint8_t FLASH_MemType, uint32_t _uiTimeout)
{
    uint32_t re = 0;
    int32_t s_time;
        
    s_time = bsp_GetRunTime();
    
    if (FLASH_MemType == FLASH_MEMTYPE_PROG)
    {
        while (1)
        {
            if (s_STM8_SerialType == STM8S)
            {
                re = SWIM_ReadByte(STM8S_FLASH_IAPSR);
            }
            else if (s_STM8_SerialType == STM8L)
            {
                re = SWIM_ReadByte(STM8L_FLASH_IAPSR);
            }

            if (re & (STM8_FLASH_IAPSR_EOP | STM8_FLASH_IAPSR_WR_PG_DIS))
            {
                re = 1;
                break;
            }                
    
            if (bsp_CheckRunTime(s_time) >= _uiTimeout)
            {
                re = 0;
                break;
            }
        }
        return re;
    }
    else if (FLASH_MemType == FLASH_MEMTYPE_DATA)   /* EEPROM 的判断条件复杂些 */
    {
        while (1)
        {            
            if (s_STM8_SerialType == STM8S)
            {
                re = SWIM_ReadByte(STM8S_FLASH_IAPSR);
            }
            else if (s_STM8_SerialType == STM8L)
            {
                re = SWIM_ReadByte(STM8L_FLASH_IAPSR);
            }

            if (s_STM8_HVOFF == 1)
            {
                if (re & (STM8_FLASH_IAPSR_HVOFF | STM8_FLASH_IAPSR_WR_PG_DIS))
                {
                    re = 1;
                    break;
                }
            }
            else
            {
                if (re & (STM8_FLASH_IAPSR_EOP | STM8_FLASH_IAPSR_WR_PG_DIS))
                {
                    re = 1;
                    break;
                }                
            }
            
            if (bsp_CheckRunTime(s_time) >= _uiTimeout)
            {
                re = 0;
                break;
            }
        }
        return re;
    }
    
    return 0;
}    

/*
*********************************************************************************************************
*    函 数 名: STM8_WaitIAPSR_Flag
*    功能说明: 等待内部操作完成. 读STM8S_FLASH_IAPSR
*    形    参:  _Flag : 判断标志位
*               _BitValue : 0或1，表示返回的条件
*               _uiTimeout : 超时时间，ms
*    返 回 值: 0 超时，1 成功
*********************************************************************************************************
*/
uint8_t STM8_WaitIAPSR_Flag(uint8_t _Flag, uint8_t _BitValue, uint32_t _uiTimeout)
{
    uint8_t re = 0;
    int32_t s_time;
        
    s_time = bsp_GetRunTime();
    
    while (1)
    {
        if (s_STM8_SerialType == STM8S)
        {
            re = SWIM_ReadByte(STM8S_FLASH_IAPSR);
        }
        else if (s_STM8_SerialType == STM8L)
        {
            re = SWIM_ReadByte(STM8L_FLASH_IAPSR);
        }

        if (_BitValue == 0)
        {
            if ((re & _Flag) == 0)
            {
                re = 1;
                break;
            } 
        }
        else
        {
            if ((re & _Flag) == _Flag)
            {
                re = 1;
                break;
            }            
        }
         

        if (bsp_CheckRunTime(s_time) >= _uiTimeout)
        {
            re = 0;
            break;
        }
    }
    return re;
}    

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_Unlock
*    功能说明: 解锁允许写flash和eeprom。 STM8S、STM8L个系列均相同
*    形    参: FlashAddr : 地址
*    返 回 值: 无
*********************************************************************************************************
*/ 
void STM8_FLASH_Unlock(void)
{        
    if (s_STM8_SerialType == STM8S)
    {
        /* Unlock program memory */
        SWIM_WriteByte(STM8S_FLASH_PUKR, STM8_FLASH_RASS_KEY1);
//        bsp_DelayUS(50);
        
        SWIM_WriteByte(STM8S_FLASH_PUKR, STM8_FLASH_RASS_KEY2);
//        bsp_DelayUS(50);
        
        /* Unlock data memory */
        /* Warning: keys are reversed on data memory !!! */
        SWIM_WriteByte(STM8S_FLASH_DUKR, STM8_FLASH_RASS_KEY2);
//        bsp_DelayUS(50);
        
        SWIM_WriteByte(STM8S_FLASH_DUKR, STM8_FLASH_RASS_KEY1);        
//        bsp_DelayUS(50);        
    }
    else if (s_STM8_SerialType == STM8L)
    {
        /* Unlock program memory */
        SWIM_WriteByte(STM8L_FLASH_PUKR, STM8_FLASH_RASS_KEY1);
//        bsp_DelayUS(50);
        
        SWIM_WriteByte(STM8L_FLASH_PUKR, STM8_FLASH_RASS_KEY2);
//        bsp_DelayUS(50);
       
        /* Unlock data memory */
        /* Warning: keys are reversed on data memory !!! */
        SWIM_WriteByte(STM8L_FLASH_DUKR, STM8_FLASH_RASS_KEY2);
//        bsp_DelayUS(50);
        
        SWIM_WriteByte(STM8L_FLASH_DUKR, STM8_FLASH_RASS_KEY1); 
//        bsp_DelayUS(50);        
    }
    
    STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_DUL | STM8_FLASH_IAPSR_PUL, 1,  20);
}

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_Lock
*    功能说明: Locks the program or data EEPROM memory
*    形    参: _FLASH_MemType : FLASH_MEMTYPE_PROG、FLASH_MEMTYPE_DATA
*    返 回 值: 无
*********************************************************************************************************
*/ 
void STM8_FLASH_Lock(uint32_t FlashAddr)
{  
    FLASH_MemType_TypeDef FLASH_MemType; 
    
    if (FlashAddr < 0x8000)
    {
        FLASH_MemType = FLASH_MEMTYPE_DATA;
    }
    else
    {
        FLASH_MemType = FLASH_MEMTYPE_PROG;
    }
    
    if (s_STM8_SerialType == STM8S)
    {
        uint8_t iapsr;
        
        iapsr = SWIM_ReadByte(STM8S_FLASH_IAPSR);        
        iapsr &= (uint8_t)FLASH_MemType;
        
        /* Lock memory */
        SWIM_WriteByte(STM8S_FLASH_IAPSR, iapsr);
    }
    else if (s_STM8_SerialType == STM8L)
    {
        uint8_t iapsr;
        
        iapsr = SWIM_ReadByte(STM8L_FLASH_IAPSR);        
        iapsr &= (uint8_t)FLASH_MemType;
        
        /* Lock memory */
        SWIM_WriteByte(STM8L_FLASH_IAPSR, iapsr);
    }    
}

/*
*********************************************************************************************************
*    函 数 名: STM8_WriteReg_CR2
*    功能说明: 写CR2寄存器.  比如  STM8_WriteReg_CR2(STM8_FLASH_CR2_OPT)
*    形    参: _value : 0禁止， 1是使能
*    返 回 值: 无
*********************************************************************************************************
*/ 
void STM8_WriteReg_CR1(uint8_t _value)
{
    uint8_t buf[1];
    
    buf[0] = _value;
    if (s_STM8_SerialType == STM8S)
    {
        SWIM_WriteBuf(STM8S_FLASH_CR1, buf, 1);
    }
    else if (s_STM8_SerialType == STM8L)
    { 
        SWIM_WriteBuf(STM8L_FLASH_CR1, buf, 1);            
    } 
}

/*
*********************************************************************************************************
*    函 数 名: STM8_WriteReg_CR2
*    功能说明: 写CR2寄存器.  比如  STM8_WriteReg_CR2(STM8_FLASH_CR2_OPT)
*    形    参: _value : 0禁止， 1是使能
*    返 回 值: 无
*********************************************************************************************************
*/ 
void STM8_WriteReg_CR2(uint8_t _value)
{
    uint8_t buf[2];
    
    buf[0] = _value;
    buf[1] = ~buf[0];
    if (s_STM8_SerialType == STM8S)
    {
        SWIM_WriteBuf(STM8S_FLASH_CR2, buf, 2);
    }
    else if (s_STM8_SerialType == STM8L)
    { 
        SWIM_WriteBuf(STM8L_FLASH_CR2, buf, 1);            
    } 
}
        
/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_EraseBlock
*    功能说明: Erases a block in the program or data memory.
*    形    参: FlashAddr : 绝对地址。 必须是block首地址
*              FLASH_MemType : FLASH_MEMTYPE_PROG、FLASH_MEMTYPE_DATA
*    返 回 值: 无
*********************************************************************************************************
*/ 
void STM8_FLASH_EraseBlock(uint32_t FlashAddr)
{
    uint32_t BlockAddr = 0;
    const uint8_t zefo[4] = {0,0,0,0};
  
    BlockAddr = (FlashAddr / s_STM8_BlockSize) * s_STM8_BlockSize;
    
    /* Enable erase block mode */
    STM8_WriteReg_CR2(STM8_FLASH_CR2_ERASE);   
    
    SWIM_WriteBuf(BlockAddr, (uint8_t *)zefo, 4);   
    
    STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_EOP, 1, 10);    
    
//    if (BlockAddr < 0x8000)
//    {
//        STM8_FLASH_WaitForLastOperation(FLASH_MEMTYPE_DATA, 100);
//    }
//    else
//    {
//        STM8_FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG, 100);
//    }
}

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_ProgramBlock
*    功能说明: Programs a memory block
*    形    参: FlashAddr : 绝对地址。 必须是block首地址
*              Buffer : Pointer to buffer containing source data.
*    返 回 值: 0:失败  1:成功
*********************************************************************************************************
*/ 
uint8_t STM8_FLASH_ProgramBlock(uint32_t FlashAddr, uint8_t *Buffer)
{  
    FLASH_MemType_TypeDef FLASH_MemType;
    uint32_t BlockAddr = 0;    
    uint8_t re;

    BlockAddr = (FlashAddr / s_STM8_BlockSize) * s_STM8_BlockSize;
    if (BlockAddr < 0x8000)
    {
        FLASH_MemType = FLASH_MEMTYPE_DATA;
    }
    else
    {
        FLASH_MemType = FLASH_MEMTYPE_PROG;
    }    
    
    if (s_STM8_SerialType == STM8L)
    {
        STM8_WriteReg_CR2(STM8_FLASH_CR2_PRG); 
    }
    else
    {
        STM8_WriteReg_CR1(STM8_FLASH_PROGRAMTIME_TPROG);     /* 设置快速编程模式 */
        bsp_DelayUS(100);
        STM8_WriteReg_CR2(STM8_FLASH_CR2_FPRG);        
    } 

    re = SWIM_WriteBuf(BlockAddr, Buffer, s_STM8_BlockSize);
    if (re == 0) goto err_quit;
    bsp_DelayUS(100);
    
    #if 1
        re = STM8_FLASH_WaitForLastOperation(FLASH_MemType, STM8_ERASE_BLOCK_TIMEOUT);
    #else        
        STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_EOP, 1,  20);
    #endif
    if (re == 0) goto err_quit;
        
    return 1;
    
err_quit:
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_ProgramBuf
*    功能说明: Programs a memory block
*    形    参: _FlashAddr : 绝对地址。 
*              _Buff : Pointer to buffer containing source data.
*              _Size : 数据大小，可以大于1个block
*    返 回 值: 0 : 出错;  1 : 成功
*********************************************************************************************************
*/ 
uint8_t STM8_FLASH_ProgramBuf(uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size)
{
    uint32_t i;
    uint32_t BlockAddr = 0;
    uint8_t buf0[128];        /* 一般就是64或者128 */
    uint8_t *pSrc;
    uint32_t size;
    uint8_t re = 0;
    
    pSrc = _Buff;
    
    /* 计算block首地址 */
    BlockAddr = (_FlashAddr / s_STM8_BlockSize) * s_STM8_BlockSize;  

    /* 烧录第一个block */    
    if (BlockAddr != _FlashAddr)
    {
        uint16_t offset;
        uint16_t len;
        
        re = SWIM_ReadBuf(BlockAddr, buf0, s_STM8_BlockSize);
        if (re == 0)
        {
            goto err_quit;
        }
        
        if (CheckBlankBuf((char *)buf0, s_STM8_BlockSize, 0) == 0)
        {
            STM8_FLASH_EraseBlock(BlockAddr);
        }
        
        offset = _FlashAddr - BlockAddr;
        
        if (offset + _Size < s_STM8_BlockSize)
        {
            len = _Size;
        }
        else 
        {
            len = s_STM8_BlockSize - offset;
        }
        for (i = 0; i < len; i++)
        {
            buf0[i + offset] = *pSrc++;
            
            _Size--;
            
            _FlashAddr++;
        }        

        re = STM8_FLASH_ProgramBlock(BlockAddr, buf0);
        if (re == 0)
        {
            goto err_quit;
        }        
    }
    
    /* 烧录中间block */
    size = _Size;
    for (i = 0; i < size / s_STM8_BlockSize; i++)
    {
//        re = SWIM_ReadBuf(BlockAddr, buf0, s_STM8_BlockSize);
//        if (re == 0)
//        {
//            goto err_quit;
//        }
//        
//        if (CheckBlankBuf((char *)buf0, s_STM8_BlockSize, 0) == 0)
//        {
//            STM8_FLASH_EraseBlock(BlockAddr);
//        }
        
        re = STM8_FLASH_ProgramBlock(_FlashAddr, pSrc); 
        if (re == 0)
        {
            goto err_quit;
        }
        
        pSrc += s_STM8_BlockSize; 
        _Size -= s_STM8_BlockSize;
        _FlashAddr += s_STM8_BlockSize;
    }
    
    /* 烧录最后一个block */
    if (_Size > 0)
    {
        re = SWIM_ReadBuf(BlockAddr, buf0, s_STM8_BlockSize);
        if (re == 0)
        {
            goto err_quit;
        }
        
        if (CheckBlankBuf((char *)buf0, s_STM8_BlockSize, 0) == 0)
        {
            STM8_FLASH_EraseBlock(BlockAddr);
        }        
        
        for (i = 0; i < _Size; i++)
        {
            buf0[i] = *pSrc++;
        }                
        re = STM8_FLASH_ProgramBlock(_FlashAddr, buf0); 
        if (re == 0)
        {
            goto err_quit;
        }        
    }
    
    return 1;

err_quit:
    return 0;
}    

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_ProgramOptionBytes
*    功能说明: Programs option byte
*    形    参: _AddrBuf : 地址数组，大端模式排列.
*              _AddrLen : _AddrLen
*              _DataBuf : 输入数据
*              _DataLen : 数据大小，可以大于1个block
*    返 回 值: 0 : 出错;  1 : 成功
*********************************************************************************************************
*/ 
uint8_t STM8_ProgramOptionBytes(uint8_t *_AddrBuf, uint16_t _AddrLen, uint8_t *_DataBuf, uint16_t _DataLen)
{
    uint8_t re;
    uint16_t Address;
    uint16_t LastAddress;
    uint8_t LastData;
    
    if (_DataLen == 0 || _AddrLen == 0 || (_AddrLen % 2) != 0)
    {
        goto err_quit;
    }
     
    while (_AddrLen)
    {            
        STM8_FLASH_Unlock();                    /* 允许写Flash和EEProm */                        
        STM8_WriteReg_CR2(STM8_FLASH_CR2_OPT);  /* 编程OPT */
        
        Address = BEBufToUint16(_AddrBuf); 
        _AddrBuf += 2;
        
        bsp_DelayUS(200);
                    
        if (Address == 0xFFFF)  /* 上一个字节取反写入 */
        {
            re = SWIM_WriteByte(LastAddress + 1, ~LastData);
            if (re == 0)
            {
                goto err_quit;
            }
        }
        else
        {
            LastAddress = Address;
            LastData = *_DataBuf;  
            
            re = SWIM_WriteByte(LastAddress, LastData);
            if (re == 0)
            {
                goto err_quit;
            } 
            
            _DataBuf++;
        }
        
        _AddrLen -= 2;
                    
        /* STM8L151 实测寄存器变化  2A 2A 2A -> 4E 判断结束 */
        bsp_DelayUS(500);
        if (Address == 0x4800)
        {
            re = STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_EOP, 1, 2000);      /* 1000ms超时 */
        }
        else
        {
            re = STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_EOP, 1, 50);       /* 50ms超时 */
        }
        if (re == 0)
        {
            goto err_quit;
        }
        bsp_DelayUS(50);     
        STM8_WriteReg_CR2(0);                   /* 不确定是否需要这句话 解除编程操作*/  
        bsp_DelayUS(50); 
    }   

    return 1;   /* 成功 */
err_quit:
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_EraseChip
*    功能说明: 擦除全片，FLASH (PROG).  事先需要unlock
*    形    参: FlashAddr : 绝对地址。 0x08000 或者 0x04000
*              FLASH_MemType : FLASH_MEMTYPE_PROG、FLASH_MEMTYPE_DATA
*    返 回 值: 0 失败  1 成功
*********************************************************************************************************
*/ 
uint8_t STM8_FLASH_EraseChip(uint32_t _FlashAddr)
{
    uint32_t i;
    const uint8_t zefo[4] = {0,0,0,0};
    uint8_t re;
    uint32_t FlashSize;   
  
    STM8_FLASH_Unlock();
    if (_FlashAddr == 0x08000)
    {
        FlashSize = s_STM8_FlashSize;
    }
    else if (_FlashAddr == 0x04000)
    {
        FlashSize = s_STM8_EEPromSize;
    }
    else
    {
        goto err_quit;
    }
    
    for (i = 0; i < FlashSize / s_STM8_BlockSize; i++)
    {
        STM8_WriteReg_CR2(STM8_FLASH_CR2_ERASE);   
        
        re = SWIM_WriteBuf(_FlashAddr, (uint8_t *)zefo, 4);   
        if (re == 0)
        {
            goto err_quit;
        }
        
        re = STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_EOP, 1, 10);
        if (re == 0)
        {
            goto err_quit;
        }       
        
        _FlashAddr += s_STM8_BlockSize;
    }
    
    return 1;
    
err_quit:
    return 0;  
}

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_EraseSector
*    功能说明: 擦除扇区
*    形    参: FlashAddr : 绝对地址。 0x08000 或者 0x04000
*              FLASH_MemType : FLASH_MEMTYPE_PROG、FLASH_MEMTYPE_DATA
*    返 回 值: 0 失败  1 成功
*********************************************************************************************************
*/ 
uint8_t STM8_FLASH_EraseSector(uint32_t _FlashAddr)
{
    const uint8_t zefo[4] = {0,0,0,0};
    uint8_t re;  
  
    STM8_FLASH_Unlock();
    
    STM8_WriteReg_CR2(STM8_FLASH_CR2_ERASE);   
    
    re = SWIM_WriteBuf(_FlashAddr, (uint8_t *)zefo, 4);   
    if (re == 0)
    {
        goto err_quit;
    }
    
    re = STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_EOP, 1, 10);
    if (re == 0)
    {
        goto err_quit;
    }       
    return 1;
    
err_quit:
    return 0;  
}

/*
*********************************************************************************************************
*    函 数 名: STM8_FLASH_ReadBuf
*    功能说明: 读flash
*    形    参: _FlashAddr : 绝对地址。 
*              _Buff : Pointer to buffer containing source data.
*              _Size : 数据大小，可以大于1个block
*    返 回 值: 0 : 出错;  1 : 成功
*********************************************************************************************************
*/ 
uint8_t STM8_FLASH_ReadBuf(uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size)
{
    uint32_t i;
    uint8_t *pSrc;
    uint32_t size;
    uint8_t re = 0;
    
    pSrc = _Buff;

    size = _Size;
    for (i = 0; i < size / s_STM8_BlockSize; i++)
    {
        re = SWIM_ReadBuf(_FlashAddr, pSrc, s_STM8_BlockSize);
        if (re == 0)
        {
            goto err_quit;
        }
        
        pSrc += s_STM8_BlockSize; 
        _Size -= s_STM8_BlockSize;
        _FlashAddr += s_STM8_BlockSize;
    }
    
    if (_Size > 0)
    {
        re = SWIM_ReadBuf(_FlashAddr, pSrc, _Size);
        if (re == 0)
        {
            goto err_quit;
        }        
    }
    
    return 1;

err_quit:
    return 0;
}    

/*
*********************************************************************************************************
*    函 数 名: STM8_UnlockReadProtect
*    功能说明: 解除读保护. 逻辑分析仪跟踪ST-LINK解锁过程获得的数据。
*    形    参: 无
*    返 回 值: 1表示OK, 0表示出错
*********************************************************************************************************
*/
uint8_t STM8_UnlockReadProtect(void)
{
    uint8_t buf[8];
    uint8_t re;
    
//    SWIM_WriteByte(0x7F80, 0xB0);       /* 切换到速模式 */
//    
//    g_HighSpeed = 1;                    
//    
//    bsp_DelayUS(400);                   /* 延迟400us */
//    
//    SWIM_WriteByte(0x7F80, 0xB4);       /*  */
//    
//    bsp_DelayUS(200);                   /* 延迟200us */
    
//    SWIM_WriteByte(STM8_CLK_CKDIVR, 0x00);       /* STM8_CLK_CKDIVR = 0x50C6 */
//    
//    SWIM_ReadBuf(0x4FFC, buf, 4);       /* 返回 0x67 0x67 0x10 0x03 */
//    
//    bsp_DelayMS(5);                     /* 延迟5ms */
//    
//    SWIM_ReadByte(0x505F);              /* 返回 0x80 */    
    
    
    re = SWIM_ReadByte(0x4800);              /* 返回 0x71*/
    if (re == 0)
    
    bsp_DelayUS(500);                   /* 延迟500us */
    
    // 42681
    SWIM_WriteByte(STM8S_FLASH_PUKR, 0x56);       /* STM8S_FLASH_PUKR = 0x5062 */
    
    bsp_DelayUS(260);                   /* 延迟260us */
    
    // 43058
    SWIM_WriteByte(STM8S_FLASH_PUKR, 0xAE);       /* STM8S_FLASH_PUKR = 0x5062 */
    
    bsp_DelayUS(260);                   /* 延迟260us */
    
    SWIM_WriteByte(STM8S_FLASH_DUKR, 0xAE);       /* STM8S_FLASH_DUKR = 0x5064 */
     bsp_DelayUS(260); 
    SWIM_WriteByte(STM8S_FLASH_DUKR, 0x56); 
     bsp_DelayUS(260); 
     
    
//#define STM8_FLASH_IAPSR_HVOFF     ((uint8_t)0x40) /*!< End of high voltage flag mask */
//#define STM8_FLASH_IAPSR_DUL       ((uint8_t)0x08) /*!< Data EEPROM unlocked flag mask */
//#define STM8_FLASH_IAPSR_EOP       ((uint8_t)0x04) /*!< End of operation flag mask */
//#define STM8_FLASH_IAPSR_PUL       ((uint8_t)0x02) /*!< Flash Program memory unlocked flag mask */
//#define STM8_FLASH_IAPSR_WR_PG_DIS ((uint8_t)0x01) /*!< Write attempted to protected page mask */
    SWIM_ReadByte(0x505F);              /* 返回 0xCA  1100 1010    */
    
    bsp_DelayUS(500); 
    
    buf[0] = STM8_FLASH_CR2_OPT;          /* STM8_FLASH_CR2_OPT = 0x80 */
    buf[1] = ~buf[0]; 
    SWIM_WriteBuf(STM8S_FLASH_CR2, buf, 2);    /* STM8S_FLASH_CR2 =  0x505B */
    
    SWIM_WriteByte(0x4800, 0); 
    
//    SWIM_ReadByte(0x505F);      /* 返回 AB 1010 1011  STM8S_FLASH_IAPSR = 0x505F */
//    SWIM_ReadByte(0x505F);      /* 返回 AB 1010 1011 */
//    
//    SWIM_ReadByte(0x505F);      /* 返回 AA 1010 1010 */
//    SWIM_ReadByte(0x505F);      /* 返回 AA 1010 1010 */
//        
//    // 48063
//    SWIM_ReadByte(0x505F);      /* 返回 4E 0100 1110 */
    
    STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_WR_PG_DIS | STM8_FLASH_IAPSR_EOP, 1, 50);

    bsp_DelayUS(500); 
    buf[0] = 0x00; 
    buf[1] = 0xFF; 
    SWIM_WriteBuf(STM8S_FLASH_CR2, buf, 2);     /* STM8S_FLASH_CR2 =  0x505B */
    
    // 50261
    SWIM_WriteByte(STM8S_FLASH_PUKR, 0x56);     /* STM8S_FLASH_PUKR = 0x5062 */
    bsp_DelayUS(1500); 
    // 50661
    SWIM_WriteByte(STM8S_FLASH_PUKR, 0xAE);         
    bsp_DelayUS(300);
    //51072
    SWIM_WriteByte(STM8S_FLASH_DUKR, 0xAE);               /* STM8S_FLASH_DUKR = 0x5064 */ 
  
// 51432.US   
    SWIM_WriteByte(STM8S_FLASH_DUKR, 0x56);
    
    bsp_DelayUS(200); 
    SWIM_ReadByte(0x505F);      /* 返回  */
    
    bsp_DelayUS(500);  
    buf[0] = STM8_FLASH_CR2_OPT;          /* STM8_FLASH_CR2_OPT = 0x80 */
    buf[1] = ~buf[0];           /* 7F */
    SWIM_WriteBuf(STM8S_FLASH_CR2, buf, 2);    /* STM8S_FLASH_CR2 =  0x505B */    

    bsp_DelayUS(200);
    SWIM_WriteByte(0x4801, 0x00); 
//    SWIM_ReadByte(0x505F);      /* 返回 2A  0010 1010 */
//    SWIM_ReadByte(0x505F);      /* 返回 2A  0010 1010 */
//    SWIM_ReadByte(0x505F);      /* 返回 6A  0110 1010 */
//    SWIM_ReadByte(0x505F);      /* 返回 4E  0100 1110 */
    STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_WR_PG_DIS | STM8_FLASH_IAPSR_EOP, 1, 50);
    
    // 56544
    SWIM_WriteByte(0x4802, 0xFF); 
//    SWIM_ReadByte(0x505F);      /* 返回 4E */
//    SWIM_ReadByte(0x505F); 
//    SWIM_ReadByte(0x505F); 
//    SWIM_ReadByte(0x505F); 
//    SWIM_ReadByte(0x505F); 
    STM8_WaitIAPSR_Flag(STM8_FLASH_IAPSR_WR_PG_DIS | STM8_FLASH_IAPSR_EOP, 1, 50);
    
    // 60.01877
    buf[0] = 0x00; 
    buf[1] = ~buf[0]; 
    SWIM_WriteBuf(STM8S_FLASH_CR2, buf, 2); /* STM8S_FLASH_CR2 =  0x505B */   


    bsp_DelayUS(900);
    
    
    //61129 
    SWIM_WriteByte(0x5062, 0x56); 
    bsp_DelayUS(200);
    // 61526
    SWIM_WriteByte(0x5062, 0xAE); 
    bsp_DelayUS(200);
    // 61915
    SWIM_WriteByte(0x5064, 0xAE);
    bsp_DelayUS(200);
    // 62217
    SWIM_WriteByte(0x5064, 0x56);  
    bsp_DelayUS(200);   

    // 62527
    SWIM_ReadByte(0x505F);      /* 返回 4A  0100 1010  */
    
    // 62992
    bsp_DelayUS(500);  
    buf[0] = STM8_FLASH_CR2_OPT;          /* STM8_FLASH_CR2_OPT = 0x80 */
    buf[1] = ~buf[0];           /* 7F */
    SWIM_WriteBuf(STM8S_FLASH_CR2, buf, 2);    /* STM8S_FLASH_CR2 =  0x505B */ 
    
    return 1;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
