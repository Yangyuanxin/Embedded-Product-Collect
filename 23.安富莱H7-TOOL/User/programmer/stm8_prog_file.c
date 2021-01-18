/*
*********************************************************************************************************
*
*    模块名称 : 编程器接口文件
*    文件名称 : prog_if.c
*    版    本 : V1.0
*    说    明 : 
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-03-19  armfly  正式发布
*
*    Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "param.h"
#include "file_lib.h"
#include "lua_if.h"
#include "prog_if.h"
#include "swd_host.h"
#include "swd_flash.h"
#include "elf_file.h"
#include "main.h"
#include "stm8_flash.h"
#include "SW_DP_Multi.h"
#include "swd_host_multi.h"

extern const program_target_t flash_algo;

/*
*********************************************************************************************************
*    函 数 名: PG_SWIM_ProgFile
*    功能说明: SWIM接口（STM8) 开始编程flash。 由lua程序调用。阻塞运行，只到编程结束。
*    形    参:  _Path : 文件名
*               _FlashAddr : flash起始地址
*               _EndAddr : flash结束地址
*               _CtrlByte : 控制字节，bit0 = 1表示 滚码UID加密在本次编程中写入
*               _FileIndex : 文件编号（1-10）,用于滚码UID加密
*    返 回 值: 0 = ok, 其他表示错误
*********************************************************************************************************
*/
uint16_t PG_SWIM_ProgFile(char *_Path, uint32_t _FlashAddr, uint32_t _EndAddr, uint32_t _CtrlByte, uint32_t _FileIndex)
{
    char path[256];
    uint16_t name_len;
    uint8_t err = 0;
    char ext_name[5];
    uint32_t FileLen;
//    uint8_t EraseChipEnable = 0;
//    uint8_t fBlankChip = 0;
//    uint32_t BlockSize;
    uint8_t FlashProgFlag = 0;
    uint32_t CheckLen;      /* 实际扫描的空间大小 */    
        
    if (_FlashAddr >= 0x008000)
    {
        FlashProgFlag = 1;      /* Flash地址 */
    }
    else
    {
        FlashProgFlag = 0;      /* EEPROM地址 */
    }
    
    /* 传入的文件名是相对路径 */
    if (strlen(_Path) + strlen(PROG_USER_DIR) > sizeof(path))
    {
        PG_PrintText("文件路径太长"); 
        err = 1;
        goto quit;        
    }
    
    if (_Path[0] == '0' && _Path[1] == ':')     /* 是绝对路径 */
    {
        strncpy(path, _Path, sizeof(path));
    }
    else    /* 是相对路路径 */ 
    {    
        GetDirOfFileName(g_tProg.FilePath, path);   /* 从lua文件名、中获取目录 */
        strcat(path, "/");
        strcat(path, _Path);

        FixFileName(path);  /* 去掉路径中的..上级目录 */
    }
    
    /* 解析文件名 */
    name_len = strlen(_Path);
    if (name_len < 5)
    {      
        PG_PrintText("数据文件名过短 "); 
        err = 1;
        goto quit;
    }  
    
    /* 文件支持 1.hex,  2.bin */
    memcpy(ext_name, &_Path[name_len - 4], 5);
    strlwr(ext_name);   /* 转换为小写 */
    if (strcmp(ext_name, ".hex") == 0)
    {
        ;
    }
    else if (strcmp(ext_name, ".bin") == 0)
    {
        ;
    }
    else
    {
        PG_PrintText("数据文件扩展名不正确");
        err = 1;
        goto quit;
    }
    
    /* 是否整片擦除 */
	//EraseChipEnable = _CtrlByte & 0x01; 
        
    FileLen = GetFileSize(path);
    if (FileLen == 0)
    {
        PG_PrintText("读取数据文件失败");        
        err = 1;
        goto quit;         
    }  
    
    {
        uint32_t DevSize;
        
        if (_FlashAddr >= 0x08000)
        {
            DevSize = s_STM8_FlashSize;
        }
        else
        {
            DevSize = s_STM8_EEPromSize;
        }
    
        if (FileLen > DevSize)
        {
            PG_PrintText("数据文件过大");    
            err = 1;
            goto quit; 
        }
    }
    
    /* 编程page */
    PG_PrintText("正在编程...");    
    PG_PrintPercent(0, 0xFFFFFFFF);    
    {
        uint32_t addr;
		uint32_t FileOffset = 0;
		uint16_t PageSize;
		uint32_t bytes;
        uint32_t i;
        uint32_t j;
        uint32_t FileBuffSize;  
        uint32_t DeviceSize;
   
		PageSize = s_STM8_BlockSize;
		if (PageSize > sizeof(FsReadBuf))
		{
			PageSize = sizeof(FsReadBuf);
		}
		
        /* 整定文件缓冲区大小为PageSize的整数倍 */
        FileBuffSize = sizeof(FsReadBuf);   
        FileBuffSize = (FileBuffSize / PageSize) * PageSize;
        
        if (FlashProgFlag == 1)
        {
            addr = 0x008000;    /* Flash地址 */
            DeviceSize = s_STM8_FlashSize;
        }
        else
        {
            if (s_STM8_SerialType == STM8L)
            {
                addr = 0x001000;    /* EEPROM地址 */
            }
            else
            {
                addr = 0x004000;    /* EEPROM地址 */
            }
            DeviceSize = s_STM8_EEPromSize;
        }

        /* 计算需要处理的存储器空间大小 */                
        if (_EndAddr > (addr +  DeviceSize))
        {
            CheckLen = addr +  DeviceSize - _FlashAddr;
        }
        else
        {
            CheckLen = _EndAddr - _FlashAddr;
        }        
        
        addr = _FlashAddr;
        for (j = 0; j < (CheckLen + sizeof(FsReadBuf) - 1) / sizeof(FsReadBuf); j++)
        {
            if (ProgCancelKey())
            {
                PG_PrintText("用户终止运行");    
                err = 1;
                goto quit;                
            }
            
            /* 读文件, 按最大缓冲区读取到内存 */
            if (_FlashAddr + FileLen > addr && _FlashAddr < addr + sizeof(FsReadBuf))
            {
                bytes = ReadFileToMem(path, FileOffset, FsReadBuf, sizeof(FsReadBuf));   
                
                if (bytes != sizeof(FsReadBuf))
                {
                    if (FileOffset + sizeof(FsReadBuf) < FileLen)
                    {
                        PG_PrintText("读取数据文件失败");        
                        err = 1;
                        goto quit;
                    }
                    
                    /* V1.11 修正bug : 烧写非整数倍PageSize的文件失败 */
                    if (bytes % PageSize)
                    {
                        memset(&FsReadBuf[bytes], 0x00 ,PageSize - (bytes % PageSize));      /* 填充空值 00 */
                        
                        bytes = ((bytes + PageSize - 1) / PageSize) * PageSize;
                    }
                }
                
                /* 修改缓冲区，填充UID加密数据或产品序号 */
                PG_FixFlashMem(addr, FsReadBuf, bytes, _FileIndex, 0); 
                
                for (i = 0; i < bytes / PageSize; i++)
                {
                    STM8_FLASH_Unlock();
                    
                    if (STM8_FLASH_ProgramBuf(addr, (uint8_t *)&FsReadBuf[i * PageSize], PageSize) == 0)
                    {
                        PG_PrintText("编程失败");        
                        err = 1;
                        goto quit;
                    }
                       
                    addr += PageSize;
                    FileOffset += PageSize;
                    
                            
                    /* 进度指示 - 每2KB刷新一次 */
                    if ((FileOffset % 2048) == 0)
                    {
                        float percent;
                        
                        percent = ((float)FileOffset / FileLen) * 100;
                        if (percent > 100)
                        {
                            percent = 100;
                        }                        
                        PG_PrintPercent(percent, addr);           
                    }                    
                }                  
            }
        }

        /* 文件以外的page */
		/* 处理文件之后的flash空间，写入fix区数据 */
		{
			uint32_t RemLen;
            uint32_t bytes;

			RemLen = _FlashAddr + CheckLen - addr;	/* 剩余空间 */
			
			/* 如果UID加密数据或产品序号在文件以外的空间 */
			if (PG_CheckFlashFix(addr, RemLen, _FileIndex))
			{ 
				FileBuffSize = sizeof(FsReadBuf);   /* 16KB */
				
                while(1)
                {
                    bytes = FileBuffSize;
                    if (bytes > RemLen)
                    {
                        bytes = RemLen; 
                    }
                    
                    if (PG_CheckFlashFix(addr, bytes, _FileIndex))
					{    
                        memset(FsReadBuf, 0, bytes);
                        
						/* 修改缓冲区，填充UID加密数据或产品序号 */
                        PG_FixFlashMem(addr, FsReadBuf, bytes, _FileIndex, 0); 
           
                        if (STM8_FLASH_ProgramBuf(addr, (uint8_t *)FsReadBuf, bytes) == 0)
                        {
                            {
                                char buf[128];
                                
                                sprintf(buf, "编程失败, 0x%08X", addr);
                                PG_PrintText(buf); 
                            }                              
                            err = 1;
                            goto quit;
                        }                           
					}
                    addr += bytes;
                    RemLen -= bytes;                    
                
                    if (addr >= _FlashAddr + CheckLen)
                    {
                        break;
                    }
                }  
			}
        }
    }
    
    /* 校验 */
    PG_PrintText("正在校验...");    
    PG_PrintPercent(0, 0xFFFFFFFF);    
    {
        uint32_t addr;
		uint32_t FileOffset = 0;
		uint16_t PageSize;
		uint32_t bytes;
        uint32_t DeviceSize;
   
        if (FlashProgFlag == 1)
        {
            addr = _FlashAddr;    /* Flash地址 */
            DeviceSize = s_STM8_FlashSize;
        }
        else
        {
            addr = _FlashAddr;    /* EEPROM地址 */
            DeviceSize = s_STM8_EEPromSize;
        }

        PageSize = sizeof(flash_buff);
		if (PageSize > sizeof(flash_buff))
		{
			PageSize = sizeof(flash_buff);
		}
        
        if (PageSize >= sizeof(FsReadBuf))
        {
            PageSize = sizeof(FsReadBuf);
        }
		
        if (PageSize >= DeviceSize)
        {
            PageSize = DeviceSize;
        }
            
        for (; FileOffset < FileLen; )
        {
            if (ProgCancelKey())
            {
                PG_PrintText("用户终止运行");    
                err = 1;
                goto quit;                
            }
            
			bytes = ReadFileToMem(path, FileOffset, FsReadBuf, PageSize); 
			if (bytes != PageSize)
			{
                if (FileOffset + PageSize < FileLen)
                {
                    PG_PrintText("读取数据文件失败");        
                    err = 1;
                    goto quit;
                }
			}		
			
            /* 修改缓冲区，填充UID加密数据或产品序号 */
            PG_FixFlashMem(addr, FsReadBuf, PageSize, _FileIndex, 0);

            {
                /* 读回进行校验 */
                STM8_FLASH_ReadBuf(addr, flash_buff, bytes);			
                if (memcmp(FsReadBuf, flash_buff, bytes) != 0)
                {
                    PG_PrintText("校验数据失败");        
                    err = 1;
                    goto quit;				
                }  
                addr += PageSize;
                FileOffset += PageSize;                
            }            

            /* 进度指示 */
            {
                float percent = -1;
                
                percent = ((float)FileOffset / FileLen) * 100;
                if (percent > 100)
                {
                    percent = 100;
                }                
                PG_PrintPercent(percent, addr);           
            }
        }
    } 
quit:
    if (g_gMulSwd.MultiMode == 0)
    {
        g_gMulSwd.Error[0] = err;   /* 单路模式，借用该变量用于机台信号lua程序用 */
    }    
    return err;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
