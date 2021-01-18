/*
*********************************************************************************************************
*
*    模块名称 : SPI接口串行FLASH 读写模块
*    文件名称 : bsp_spi_flash.c
*    版    本 : V1.0
*    说    明 : 支持 SST25VF016B、MX25L1606E 和 W25Q64BVSSIG
*               SST25VF016B 的写操作采用AAI指令，可以提高写入效率。
*
*               缺省使用STM32F4的硬件SPI1接口，时钟频率最高为 42MHz （超频使用）
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2013-02-01 armfly  正式发布
*
*    Copyright (C), 2013-2014, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

/* 串行Flsh的片选GPIO端口， PD13  */
#define SF_CS_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define SF_CS_GPIO GPIOD
#define SF_CS_PIN GPIO_PIN_13
#define SF_CS_0() SF_CS_GPIO->BSRRH = SF_CS_PIN
#define SF_CS_1() SF_CS_GPIO->BSRRL = SF_CS_PIN

#define CMD_AAI 0xAD        /* AAI 连续编程指令(FOR SST25VF016B) */
#define CMD_DISWR 0x04    /* 禁止写, 退出AAI状态 */
#define CMD_EWRSR 0x50    /* 允许写状态寄存器的命令 */
#define CMD_WRSR 0x01        /* 写状态寄存器命令 */
#define CMD_WREN 0x06        /* 写使能命令 */
#define CMD_READ 0x03        /* 读数据区命令 */
#define CMD_RDSR 0x05        /* 读状态寄存器命令 */
#define CMD_RDID 0x9F        /* 读器件ID命令 */
#define CMD_SE 0x20            /* 擦除扇区命令 */
#define CMD_BE 0xC7            /* 批量擦除命令 */
#define DUMMY_BYTE 0xA5 /* 哑命令，可以为任意值，用于读操作 */

#define WIP_FLAG 0x01 /* 状态寄存器中的正在编程标志（WIP) */

SFLASH_T g_tSF;

#if 0
static void sf_WriteStatus(uint8_t _ucValue);
#endif

static void sf_WriteEnable(void);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t *_ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _uiLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
static uint8_t sf_AutoWritePage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

static uint8_t s_spiBuf[4 * 1024]; /* 用于写函数，先读出整个page，修改缓冲区后，再整个page回写 */

/*
*********************************************************************************************************
*    函 数 名: bsp_InitSFlash
*    功能说明: 串行falsh硬件初始化。 配置CS GPIO， 读取ID。
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSFlash(void)
{
    /* 配置CS GPIO */
    {
        GPIO_InitTypeDef gpio_init;

        /* 打开GPIO时钟 */
        SF_CS_CLK_ENABLE();

        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;        /* 设置推挽输出 */
        gpio_init.Pull = GPIO_NOPULL;                        /* 上下拉电阻不使能 */
        gpio_init.Speed = GPIO_SPEED_FREQ_HIGH; /* GPIO速度等级 */
        gpio_init.Pin = SF_CS_PIN;
        HAL_GPIO_Init(SF_CS_GPIO, &gpio_init);
    }

    /* 读取芯片ID, 自动识别芯片型号 */
    sf_ReadInfo();
}

/*
*********************************************************************************************************
*    函 数 名: sf_SetCS
*    功能说明: 串行FALSH片选控制函数
*    形    参: 无
*    返 回 值: 无
*********************************************************************************************************
*/
void sf_SetCS(uint8_t _Level)
{
    if (_Level == 0)
    {
        bsp_SpiBusEnter();
        bsp_InitSPIParam(SPI_BAUDRATEPRESCALER_8, SPI_PHASE_1EDGE, SPI_POLARITY_LOW);
        SF_CS_0();
    }
    else
    {
        SF_CS_1();
        bsp_SpiBusExit();
    }
}

/*
*********************************************************************************************************
*    函 数 名: sf_EraseSector
*    功能说明: 擦除指定的扇区
*    形    参:  _uiSectorAddr : 扇区地址
*    返 回 值: 无
*********************************************************************************************************
*/
void sf_EraseSector(uint32_t _uiSectorAddr)
{
    sf_WriteEnable(); /* 发送写使能命令 */

    /* 擦除扇区操作 */
    sf_SetCS(0); /* 使能片选 */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = CMD_SE;                                                         /* 发送擦除命令 */
    g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xFF0000) >> 16); /* 发送扇区地址的高8bit */
    g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xFF00) >> 8);         /* 发送扇区地址中间8bit */
    g_spiTxBuf[g_spiLen++] = (_uiSectorAddr & 0xFF);                         /* 发送扇区地址低8bit */
    bsp_spiTransfer();
    sf_SetCS(1); /* 禁能片选 */

    sf_WaitForWriteEnd(); /* 等待串行Flash内部写操作完成 */
}

/*
*********************************************************************************************************
*    函 数 名: sf_EraseChip
*    功能说明: 擦除整个芯片
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void sf_EraseChip(void)
{
    sf_WriteEnable(); /* 发送写使能命令 */

    /* 擦除扇区操作 */
    sf_SetCS(0); /* 使能片选 */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = CMD_BE; /* 发送整片擦除命令 */
    bsp_spiTransfer();
    sf_SetCS(1); /* 禁能片选 */

    sf_WaitForWriteEnd(); /* 等待串行Flash内部写操作完成 */
}

/*
*********************************************************************************************************
*    函 数 名: sf_PageWrite
*    功能说明: 向一个page内写入若干字节。字节个数不能超出页面大小（4K)
*    形    参:      _pBuf : 数据源缓冲区；
*                _uiWriteAddr ：目标区域首地址
*                _usSize ：数据个数，不能超过页面大小
*    返 回 值: 无
*********************************************************************************************************
*/
void sf_PageWrite(uint8_t *_pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
    uint32_t i, j;

    if (g_tSF.ChipID == SST25VF016B_ID)
    {
        /* AAI指令要求传入的数据个数是偶数 */
        if ((_usSize < 2) && (_usSize % 2))
        {
            return;
        }

        sf_WriteEnable(); /* 发送写使能命令 */

        sf_SetCS(0); /* 使能片选 */
        g_spiLen = 0;
        g_spiTxBuf[g_spiLen++] = CMD_AAI;                                                        /* 发送AAI命令(地址自动增加编程) */
        g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xFF0000) >> 16); /* 发送扇区地址的高8bit */
        g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xFF00) >> 8);        /* 发送扇区地址中间8bit */
        g_spiTxBuf[g_spiLen++] = (_uiWriteAddr & 0xFF);                            /* 发送扇区地址低8bit */
        g_spiTxBuf[g_spiLen++] = (*_pBuf++);                                                /* 发送第1个数据 */
        g_spiTxBuf[g_spiLen++] = (*_pBuf++);                                                /* 发送第2个数据 */
        bsp_spiTransfer();
        sf_SetCS(1); /* 禁能片选 */

        sf_WaitForWriteEnd(); /* 等待串行Flash内部写操作完成 */

        _usSize -= 2; /* 计算剩余字节数 */

        for (i = 0; i < _usSize / 2; i++)
        {
            sf_SetCS(0); /* 使能片选 */
            g_spiLen = 0;
            g_spiTxBuf[g_spiLen++] = (CMD_AAI);    /* 发送AAI命令(地址自动增加编程) */
            g_spiTxBuf[g_spiLen++] = (*_pBuf++); /* 发送数据 */
            g_spiTxBuf[g_spiLen++] = (*_pBuf++); /* 发送数据 */
            bsp_spiTransfer();
            sf_SetCS(1);                    /* 禁能片选 */
            sf_WaitForWriteEnd(); /* 等待串行Flash内部写操作完成 */
        }

        /* 进入写保护状态 */
        sf_SetCS(0);
        g_spiLen = 0;
        g_spiTxBuf[g_spiLen++] = (CMD_DISWR);
        bsp_spiTransfer();
        sf_SetCS(1);

        sf_WaitForWriteEnd(); /* 等待串行Flash内部写操作完成 */
    }
    else /* for MX25L1606E 、 W25Q64BV */
    {
        for (j = 0; j < _usSize / 256; j++)
        {
            sf_WriteEnable(); /* 发送写使能命令 */

            sf_SetCS(0); /* 使能片选 */
            g_spiLen = 0;
            g_spiTxBuf[g_spiLen++] = (0x02);                                                        /* 发送AAI命令(地址自动增加编程) */
            g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xFF0000) >> 16); /* 发送扇区地址的高8bit */
            g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xFF00) >> 8);        /* 发送扇区地址中间8bit */
            g_spiTxBuf[g_spiLen++] = (_uiWriteAddr & 0xFF);                            /* 发送扇区地址低8bit */
            for (i = 0; i < 256; i++)
            {
                g_spiTxBuf[g_spiLen++] = (*_pBuf++); /* 发送数据 */
            }
            bsp_spiTransfer();
            sf_SetCS(1); /* 禁止片选 */

            sf_WaitForWriteEnd(); /* 等待串行Flash内部写操作完成 */

            _uiWriteAddr += 256;
        }

        /* 进入写保护状态 */
        sf_SetCS(0);
        g_spiLen = 0;
        g_spiTxBuf[g_spiLen++] = (CMD_DISWR);
        bsp_spiTransfer();
        sf_SetCS(1);

        sf_WaitForWriteEnd(); /* 等待串行Flash内部写操作完成 */
    }
}

/*
*********************************************************************************************************
*    函 数 名: sf_ReadBuffer
*    功能说明: 连续读取若干字节。字节个数不能超出芯片容量。
*    形    参:      _pBuf : 数据源缓冲区；
*                _uiReadAddr ：首地址
*                _usSize ：数据个数, 可以大于PAGE_SIZE,但是不能超出芯片总容量
*    返 回 值: 无
*********************************************************************************************************
*/
void sf_ReadBuffer(uint8_t *_pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
    uint16_t rem;
    uint16_t i;

    /* 如果读取的数据长度为0或者超出串行Flash地址空间，则直接返回 */
    if ((_uiSize == 0) || (_uiReadAddr + _uiSize) > g_tSF.TotalSize)
    {
        return;
    }

    /* 擦除扇区操作 */
    sf_SetCS(0); /* 使能片选 */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = (CMD_READ);                                             /* 发送读命令 */
    g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xFF0000) >> 16); /* 发送扇区地址的高8bit */
    g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xFF00) >> 8);         /* 发送扇区地址中间8bit */
    g_spiTxBuf[g_spiLen++] = (_uiReadAddr & 0xFF);                         /* 发送扇区地址低8bit */
    bsp_spiTransfer();

    /* 开始读数据，疑问底层DMA缓冲区有限，必须分包读 */
    for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; i++)
    {
        g_spiLen = SPI_BUFFER_SIZE;
        bsp_spiTransfer();

        memcpy(_pBuf, g_spiRxBuf, SPI_BUFFER_SIZE);
        _pBuf += SPI_BUFFER_SIZE;
    }

    rem = _uiSize % SPI_BUFFER_SIZE; /* 剩余字节 */
    if (rem > 0)
    {
        g_spiLen = rem;
        bsp_spiTransfer();

        memcpy(_pBuf, g_spiRxBuf, rem);
    }

    sf_SetCS(1); /* 禁能片选 */
}

/*
*********************************************************************************************************
*    函 数 名: sf_CmpData
*    功能说明: 比较Flash的数据.
*    形    参:      _ucpTar : 数据缓冲区
*                _uiSrcAddr ：Flash地址
*                _uiSize ：数据个数, 可以大于PAGE_SIZE,但是不能超出芯片总容量
*    返 回 值: 0 = 相等, 1 = 不等
*********************************************************************************************************
*/
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize)
{
    uint16_t i, j;
    uint16_t rem;

    /* 如果读取的数据长度为0或者超出串行Flash地址空间，则直接返回 */
    if ((_uiSrcAddr + _uiSize) > g_tSF.TotalSize)
    {
        return 1;
    }

    if (_uiSize == 0)
    {
        return 0;
    }

    sf_SetCS(0); /* 使能片选 */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = (CMD_READ);                                            /* 发送读命令 */
    g_spiTxBuf[g_spiLen++] = ((_uiSrcAddr & 0xFF0000) >> 16); /* 发送扇区地址的高8bit */
    g_spiTxBuf[g_spiLen++] = ((_uiSrcAddr & 0xFF00) >> 8);        /* 发送扇区地址中间8bit */
    g_spiTxBuf[g_spiLen++] = (_uiSrcAddr & 0xFF);                            /* 发送扇区地址低8bit */
    bsp_spiTransfer();

    /* 开始读数据，因为底层DMA缓冲区有限，必须分包读 */
    for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; i++)
    {
        g_spiLen = SPI_BUFFER_SIZE;
        bsp_spiTransfer();

        for (j = 0; j < SPI_BUFFER_SIZE; j++)
        {
            if (g_spiRxBuf[j] != *_ucpTar++)
            {
                goto NOTEQ; /* 不相等 */
            }
        }
    }

    rem = _uiSize % SPI_BUFFER_SIZE; /* 剩余字节 */
    if (rem > 0)
    {
        g_spiLen = rem;
        bsp_spiTransfer();

        for (j = 0; j < rem; j++)
        {
            if (g_spiRxBuf[j] != *_ucpTar++)
            {
                goto NOTEQ; /* 不相等 */
            }
        }
    }
    sf_SetCS(1);
    return 0; /* 相等 */

NOTEQ:
    sf_SetCS(1); /* 不相等 */
    return 1;
}

/*
*********************************************************************************************************
*    函 数 名: sf_NeedErase
*    功能说明: 判断写PAGE前是否需要先擦除。
*    形    参:   _ucpOldBuf ： 旧数据
*               _ucpNewBuf ： 新数据
*               _uiLen ：数据个数，不能超过页面大小
*    返 回 值: 0 : 不需要擦除， 1 ：需要擦除
*********************************************************************************************************
*/
static uint8_t sf_NeedErase(uint8_t *_ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen)
{
    uint16_t i;
    uint8_t ucOld;

    /*
    算法第1步：old 求反, new 不变
          old    new
          1101   0101
    ~     1111
        = 0010   0101

    算法第2步: old 求反的结果与 new 位与
          0010   old
    &      0101   new
         =0000

    算法第3步: 结果为0,则表示无需擦除. 否则表示需要擦除
    */

    for (i = 0; i < _usLen; i++)
    {
        ucOld = *_ucpOldBuf++;
        ucOld = ~ucOld;

        /* 注意错误的写法: if (ucOld & (*_ucpNewBuf++) != 0) */
        if ((ucOld & (*_ucpNewBuf++)) != 0)
        {
            return 1;
        }
    }
    return 0;
}

/*
*********************************************************************************************************
*    函 数 名: sf_AutoWritePage
*    功能说明: 写1个PAGE并校验,如果不正确则再重写两次。本函数自动完成擦除操作。
*    形    参:      _pBuf : 数据源缓冲区；
*                _uiWriteAddr ：目标区域首地址
*                _usSize ：数据个数，不能超过页面大小
*    返 回 值: 0 : 错误， 1 ： 成功
*********************************************************************************************************
*/
static uint8_t sf_AutoWritePage(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
    uint16_t i;
    uint16_t j;                        /* 用于延时 */
    uint32_t uiFirstAddr; /* 扇区首址 */
    uint8_t ucNeedErase;    /* 1表示需要擦除 */
    uint8_t cRet;

    /* 长度为0时不继续操作,直接认为成功 */
    if (_usWrLen == 0)
    {
        return 1;
    }

    /* 如果偏移地址超过芯片容量则退出 */
    if (_uiWrAddr >= g_tSF.TotalSize)
    {
        return 0;
    }

    /* 如果数据长度大于扇区容量，则退出 */
    if (_usWrLen > g_tSF.PageSize)
    {
        return 0;
    }

    /* 如果FLASH中的数据没有变化,则不写FLASH */
    sf_ReadBuffer(s_spiBuf, _uiWrAddr, _usWrLen);
    if (memcmp(s_spiBuf, _ucpSrc, _usWrLen) == 0)
    {
        return 1;
    }

    /* 判断是否需要先擦除扇区 */
    /* 如果旧数据修改为新数据，所有位均是 1->0 或者 0->0, 则无需擦除,提高Flash寿命 */
    ucNeedErase = 0;
    if (sf_NeedErase(s_spiBuf, _ucpSrc, _usWrLen))
    {
        ucNeedErase = 1;
    }

    uiFirstAddr = _uiWrAddr & (~(g_tSF.PageSize - 1));

    if (_usWrLen == g_tSF.PageSize) /* 整个扇区都改写 */
    {
        for (i = 0; i < g_tSF.PageSize; i++)
        {
            s_spiBuf[i] = _ucpSrc[i];
        }
    }
    else /* 改写部分数据 */
    {
        /* 先将整个扇区的数据读出 */
        sf_ReadBuffer(s_spiBuf, uiFirstAddr, g_tSF.PageSize);

        /* 再用新数据覆盖 */
        i = _uiWrAddr & (g_tSF.PageSize - 1);
        memcpy(&s_spiBuf[i], _ucpSrc, _usWrLen);
    }

    /* 写完之后进行校验，如果不正确则重写，最多3次 */
    cRet = 0;
    for (i = 0; i < 3; i++)
    {

        /* 如果旧数据修改为新数据，所有位均是 1->0 或者 0->0, 则无需擦除,提高Flash寿命 */
        if (ucNeedErase == 1)
        {
            sf_EraseSector(uiFirstAddr); /* 擦除1个扇区 */
        }

        /* 编程一个PAGE */
        sf_PageWrite(s_spiBuf, uiFirstAddr, g_tSF.PageSize);

        if (sf_CmpData(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
        {
            cRet = 1;
            break;
        }
        else
        {
            if (sf_CmpData(_uiWrAddr, _ucpSrc, _usWrLen) == 0)
            {
                cRet = 1;
                break;
            }

            /* 失败后延迟一段时间再重试 */
            for (j = 0; j < 10000; j++)
                ;
        }
    }

    return cRet;
}

/*
*********************************************************************************************************
*    函 数 名: sf_WriteBuffer
*    功能说明: 写1个扇区并校验,如果不正确则再重写两次。本函数自动完成擦除操作。
*    形    参:      _pBuf : 数据源缓冲区；
*                _uiWrAddr ：目标区域首地址
*                _usSize ：数据个数，不能超过页面大小
*    返 回 值: 1 : 成功， 0 ： 失败
*********************************************************************************************************
*/
uint8_t sf_WriteBuffer(uint8_t *_pBuf, uint32_t _uiWriteAddr, uint16_t _usWriteSize)
{
    uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = _uiWriteAddr % g_tSF.PageSize;
    count = g_tSF.PageSize - Addr;
    NumOfPage = _usWriteSize / g_tSF.PageSize;
    NumOfSingle = _usWriteSize % g_tSF.PageSize;

    if (Addr == 0) /* 起始地址是页面首地址  */
    {
        if (NumOfPage == 0) /* 数据长度小于页面大小 */
        {
            if (sf_AutoWritePage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
            {
                return 0;
            }
        }
        else /* 数据长度大于等于页面大小 */
        {
            while (NumOfPage--)
            {
                if (sf_AutoWritePage(_pBuf, _uiWriteAddr, g_tSF.PageSize) == 0)
                {
                    return 0;
                }
                _uiWriteAddr += g_tSF.PageSize;
                _pBuf += g_tSF.PageSize;
            }
            if (sf_AutoWritePage(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
            {
                return 0;
            }
        }
    }
    else /* 起始地址不是页面首地址  */
    {
        if (NumOfPage == 0) /* 数据长度小于页面大小 */
        {
            if (NumOfSingle > count) /* (_usWriteSize + _uiWriteAddr) > SPI_FLASH_PAGESIZE */
            {
                temp = NumOfSingle - count;

                if (sf_AutoWritePage(_pBuf, _uiWriteAddr, count) == 0)
                {
                    return 0;
                }

                _uiWriteAddr += count;
                _pBuf += count;

                if (sf_AutoWritePage(_pBuf, _uiWriteAddr, temp) == 0)
                {
                    return 0;
                }
            }
            else
            {
                if (sf_AutoWritePage(_pBuf, _uiWriteAddr, _usWriteSize) == 0)
                {
                    return 0;
                }
            }
        }
        else /* 数据长度大于等于页面大小 */
        {
            _usWriteSize -= count;
            NumOfPage = _usWriteSize / g_tSF.PageSize;
            NumOfSingle = _usWriteSize % g_tSF.PageSize;

            if (sf_AutoWritePage(_pBuf, _uiWriteAddr, count) == 0)
            {
                return 0;
            }

            _uiWriteAddr += count;
            _pBuf += count;

            while (NumOfPage--)
            {
                if (sf_AutoWritePage(_pBuf, _uiWriteAddr, g_tSF.PageSize) == 0)
                {
                    return 0;
                }
                _uiWriteAddr += g_tSF.PageSize;
                _pBuf += g_tSF.PageSize;
            }

            if (NumOfSingle != 0)
            {
                if (sf_AutoWritePage(_pBuf, _uiWriteAddr, NumOfSingle) == 0)
                {
                    return 0;
                }
            }
        }
    }
    return 1; /* 成功 */
}

/*
*********************************************************************************************************
*    函 数 名: sf_ReadID
*    功能说明: 读取器件ID
*    形    参:  无
*    返 回 值: 32bit的器件ID (最高8bit填0，有效ID位数为24bit）
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
    uint32_t uiID;
    uint8_t id1, id2, id3;

    sf_SetCS(0); /* 使能片选 */
    g_spiLen = 0;
    g_spiTxBuf[0] = (CMD_RDID); /* 发送读ID命令 */
    g_spiLen = 4;
    bsp_spiTransfer();

    id1 = g_spiRxBuf[1]; /* 读ID的第1个字节 */
    id2 = g_spiRxBuf[2]; /* 读ID的第2个字节 */
    id3 = g_spiRxBuf[3]; /* 读ID的第3个字节 */
    sf_SetCS(1);                 /* 禁能片选 */

    uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

    return uiID;
}

/*
*********************************************************************************************************
*    函 数 名: sf_ReadInfo
*    功能说明: 读取器件ID,并填充器件参数
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
void sf_ReadInfo(void)
{
    /* 自动识别串行Flash型号 */
    {
        g_tSF.ChipID = sf_ReadID(); /* 芯片ID */

        switch (g_tSF.ChipID)
        {
        case SST25VF016B_ID:
            strcpy(g_tSF.ChipName, "SST25VF016B");
            g_tSF.TotalSize = 2 * 1024 * 1024; /* 总容量 = 2M */
            g_tSF.PageSize = 4 * 1024;                 /* 页面大小 = 4K */
            break;

        case MX25L1606E_ID:
            strcpy(g_tSF.ChipName, "MX25L1606E");
            g_tSF.TotalSize = 2 * 1024 * 1024; /* 总容量 = 2M */
            g_tSF.PageSize = 4 * 1024;                 /* 页面大小 = 4K */
            break;

        case W25Q64BV_ID:
            strcpy(g_tSF.ChipName, "W25Q64");
            g_tSF.TotalSize = 8 * 1024 * 1024; /* 总容量 = 8M */
            g_tSF.PageSize = 4 * 1024;                 /* 页面大小 = 4K */
            break;

        case W25Q128_ID:
            strcpy(g_tSF.ChipName, "W25Q128");
            g_tSF.TotalSize = 16 * 1024 * 1024; /* 总容量 = 8M */
            g_tSF.PageSize = 4 * 1024;                    /* 页面大小 = 4K */
            break;

        default:
            strcpy(g_tSF.ChipName, "Unknow Flash");
            g_tSF.TotalSize = 2 * 1024 * 1024;
            g_tSF.PageSize = 4 * 1024;
            break;
        }
    }
}

/*
*********************************************************************************************************
*    函 数 名: sf_WriteEnable
*    功能说明: 向器件发送写使能命令
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
static void sf_WriteEnable(void)
{
    sf_SetCS(0); /* 使能片选 */
    g_spiLen = 0;
    g_spiTxBuf[g_spiLen++] = (CMD_WREN); /* 发送命令 */
    bsp_spiTransfer();
    sf_SetCS(1); /* 禁能片选 */
}

#if 0
/*
*********************************************************************************************************
*    函 数 名: sf_WriteStatus
*    功能说明: 写状态寄存器
*    形    参:  _ucValue : 状态寄存器的值
*    返 回 值: 无
*********************************************************************************************************
*/
static void sf_WriteStatus(uint8_t _ucValue)
{
    if (g_tSF.ChipID == SST25VF016B_ID)
    {
        /* 第1步：先使能写状态寄存器 */
        sf_SetCS(0);                                    /* 使能片选 */
        g_spiLen = 0;
        g_spiTxBuf[g_spiLen++] = (CMD_EWRSR);                            /* 发送命令， 允许写状态寄存器 */
        bsp_spiTransfer();
        sf_SetCS(1);                                    /* 禁能片选 */

        /* 第2步：再写状态寄存器 */
        sf_SetCS(0);                                    /* 使能片选 */
        g_spiLen = 0;
        g_spiTxBuf[g_spiLen++] = (CMD_WRSR);                            /* 发送命令， 写状态寄存器 */
        g_spiTxBuf[g_spiLen++] = (_ucValue);                            /* 发送数据：状态寄存器的值 */
        bsp_spiTransfer();
        sf_SetCS(1);                                    /* 禁能片选 */
    }
    else
    {
        sf_SetCS(0);                                    /* 使能片选 */
        g_spiLen = 0;
        g_spiTxBuf[g_spiLen++] = (CMD_WRSR);                            /* 发送命令， 写状态寄存器 */
        g_spiTxBuf[g_spiLen++] = (_ucValue);                            /* 发送数据：状态寄存器的值 */
        bsp_spiTransfer();
        sf_SetCS(1);                                    /* 禁能片选 */
    }
}
#endif

/*
*********************************************************************************************************
*    函 数 名: sf_WaitForWriteEnd
*    功能说明: 采用循环查询的方式等待器件内部写操作完成
*    形    参:  无
*    返 回 值: 无
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
    sf_SetCS(0);                                /* 使能片选 */
    g_spiTxBuf[0] = (CMD_RDSR); /* 发送命令， 读状态寄存器 */
    g_spiLen = 2;
    bsp_spiTransfer();
    sf_SetCS(1); /* 禁能片选 */

    while (1)
    {
        sf_SetCS(0);                                /* 使能片选 */
        g_spiTxBuf[0] = (CMD_RDSR); /* 发送命令， 读状态寄存器 */
        g_spiTxBuf[1] = 0;                    /* 无关数据 */
        g_spiLen = 2;
        bsp_spiTransfer();
        sf_SetCS(1); /* 禁能片选 */

        if ((g_spiRxBuf[1] & WIP_FLAG) != SET)
            ; /* 判断状态寄存器的忙标志位 */
        {
            break;
        }
    }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
