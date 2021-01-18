/*
*********************************************************************************************************
*
*    模块名称 : 编程器接口文件
*    文件名称 : prog_if.c
*    版    本 : V1.1
*    说    明 : 
*
*    修改记录 :
*        版本号  日期        作者     说明
*        V1.0    2019-03-19  armfly  正式发布
*        V1.1    2020-12-12  armfly  修正算法执行的缺陷，Init()和UnInit()按正常规范执行.
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
*    函 数 名: PG_SWD_ProgFile
*    功能说明: SWD接口（STM32) 开始编程flash。 由lua程序调用。阻塞运行，只到编程结束。
*    形    参:  _Path : 文件名
*               _FlashAddr : flash起始地址
*               _EndAddr : 结束地址 + 1（用于避免重复写滚码加密区）
*               _CtrlByte : 控制字节，bit0 = 1表示全片擦除，0表示扇区擦除
*               _FileIndex : 文件编号（1-10）,用于滚码UID加密
*    返 回 值: 0 = ok, 其他表示错误
*********************************************************************************************************
*/
uint16_t PG_SWD_ProgFile(char *_Path, uint32_t _FlashAddr, uint32_t _EndAddr, uint32_t _CtrlByte, uint32_t _FileIndex)
{
    char path[256];
    uint16_t name_len;
    uint8_t err = 0;
    char ext_name[5];
    error_t err_t;
    uint32_t FileLen;
    uint8_t EraseChipEnable = 0;
    uint8_t fBlankChip = 0;
    uint32_t CheckLen;      /* 实际扫描的空间大小 */
    
    /* 第0步 ******************** 检查文件 **********************/
    /* 传入的文件名是相对路径 */
    if (strlen(_Path) + strlen(PROG_USER_DIR) > sizeof(path))
    {
        PG_PrintText("文件路径太长"); 
        err = 1;
        goto quit;        
    }
    
    if (_Path[0] == '0' && _Path[1] == ':')         /* 是绝对路径 */
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
    
    /* 文件仅支持 bin */
    memcpy(ext_name, &_Path[name_len - 4], 5);
    strlwr(ext_name);   /* 转换为小写 */
    if (strcmp(ext_name, ".bin") == 0)
    {
        ;
    }
    else
    {
        PG_PrintText("数据文件格式错误");
        err = 1;
        goto quit;
    }
    
    /* 是否整片擦除 */
	EraseChipEnable = _CtrlByte & 0x01;     
    
    FileLen = GetFileSize(path);
    if (FileLen == 0)
    {
        PG_PrintText("读取数据文件失败");        
        err = 1;
        goto quit;         
    }  
    if (FileLen > g_tFLM.Device.szDev)
    {
        PG_PrintText("数据文件过大");    
        err = 1;
        goto quit; 
    }
    if (_FlashAddr < g_tFLM.Device.DevAdr || _FlashAddr + FileLen > g_tFLM.Device.DevAdr + g_tFLM.Device.szDev)
    {
        PG_PrintText("目标地址超过芯片范围");    
        err = 1;
        goto quit; 
    }

    /* 计算需要处理的存储器空间大小 */                
    if (_EndAddr > (g_tFLM.Device.DevAdr +  g_tFLM.Device.szDev))
    {
        CheckLen = g_tFLM.Device.DevAdr +  g_tFLM.Device.szDev - _FlashAddr;
    }
    else
    {
        CheckLen = _EndAddr - _FlashAddr;
    }
                
    /* 第1步 ******************** 查空 **********************/
    
    /* 加载MCU的编程算法到目标机内存 */
    {
        /* SWD进入debug状态 */
        err_t = target_flash_enter_debug_program();
        if (err_t != ERROR_SUCCESS)
        {            
            PG_PrintText("error: target_flash_enter_debug_program()");
            err = 1;
            goto quit;  
        }
        /* 装载芯片厂家的FLM算法代码到目标机内存 */
        LoadAlgoToTarget();
        
        /* 装载算法并执行init函数 */
        err_t = target_flash_init(_FlashAddr, 0, FLM_INIT_ERASE);
        if (err_t != ERROR_SUCCESS)
        {
            PG_PrintText("error: target_flash_init(FLM_INIT_ERASE)");
            err = 1;
            goto quit;
        }
    }
    
    if (EraseChipEnable == 2)
    {
        fBlankChip = 0;     /* 不检查空片，直接整片擦除 */
    }
    else
    {
        uint8_t re;
        uint8_t (*flm_check_blank)(uint32_t addr, uint32_t size);   /* 查空函数指针 */
        
        /* 空片检查 */
        PG_PrintText("正在检查空片 ");  
        PG_PrintPercent(0, _FlashAddr);    

        fBlankChip = 1;
        if (flash_algo.check_blank > 0)     /* 如果FLM有查空函数，则调用该函数，提高效率 */
        {
            flm_check_blank  = target_flash_check_blank;
        } 
        else    /* 如果FLM没有查空函数，则加载通用的算法代码(flash常量数组) */
        {                           
            /* 2020-12-14 执行init函数用于校验的, 用于QSPI Flash内存映射模式 */
            #if 0
            err_t = target_flash_init(_FlashAddr, 0, FLM_INIT_VERIFY);
            if (err_t != ERROR_SUCCESS)
            {
                PG_PrintText("error: target_flash_init(FLM_INIT_VERIFY) for check_blank");
                err = 1;
                goto quit;
            }
            #endif
            
            /* 装载算法代码到目标机内存 */
            LoadCheckBlankAlgoToTarget();

            flm_check_blank = target_flash_check_blank_ex;
        }
        
        /* 检查主存储器 */
        re = flm_check_blank(_FlashAddr,  FileLen);
        if (re == CHK_BLANK_ERROR)
        {
            char buf[128];
            sprintf(buf, "target_flash_check_blank() error, FlashAddr = %X, FileLen = %X", _FlashAddr, FileLen);
            PG_PrintText(buf); 
            err = 1;
            goto quit;
        }
        else if (re == CHK_IS_BLANK)
        {
            fBlankChip = 1;
        }
        else if (re == CHK_NOT_BLANK)
        {
            fBlankChip = 0;     /* 0表示不空 */
        }
        
        /* 检查FIX区 */                
        if (fBlankChip == 1)
        {
            uint16_t m;

            for (m = 0; m < g_tFixData.Count; m++)
            {
                if (g_tFixData.Lines[m].Enable == _FileIndex
					&& g_tFixData.Lines[m].Addr + g_tFixData.Lines[m].Len > _FlashAddr
                    && g_tFixData.Lines[m].Addr <= _FlashAddr + CheckLen)
                {                    
                    /* 此处检查的空间范围没有细化，如果同一个FIX项跨文件了，则需要重新计算下地址和长度（情况应该很少） */
                    re = target_flash_check_blank(g_tFixData.Lines[m].Addr,  g_tFixData.Lines[m].Len);
                    if (re == CHK_BLANK_ERROR)
                    {
                        char buf[128];
                        
                        sprintf(buf, "target_flash_check_blank() error, FlashAddr = %X, FileLen = %X", _FlashAddr, FileLen);
                        PG_PrintText(buf); 
                        err = 1;
                        goto quit;
                    }
                    else if (re == CHK_IS_BLANK)
                    {
                        ;
                    }
                    else if (re == CHK_NOT_BLANK)
                    {
                        fBlankChip = 0;     /* 0表示不空 */
                        break;
                    }
                }
            }                    
        }
        
        if (flash_algo.check_blank > 0)     /* 如果FLM有查空函数，则调用该函数，提高效率 */
        {
            ;
        }
        else
        {
            /* 恢复芯片厂家的FLM算法代码到目标机内存 */
            LoadAlgoToTarget();
        }
        
        PG_PrintPercent(100, _FlashAddr); 
    }
    
    /* 第2步 ******************** 擦除 **********************/
    if (EraseChipEnable == 0)   /* 按扇区擦除 */
    {
        if (fBlankChip == 0)    /* 不是空片才进行擦除 */
        {
            uint32_t j;
            uint32_t addr;
            uint32_t FinishedSize = 0;
            uint8_t fEraseReq = 0;
            uint8_t waittime = 0;
            
            /* 根据算法名称判断芯片 */
            if (strstr(flash_algo.algo_file_name, "/MindMotion/"))
            {
                waittime = 5;
                
                bsp_DelayMS(20);
            }            
            
            PG_PrintText("正在擦除扇区...");    
            PG_PrintPercent(0, 0xFFFFFFFF);  
            bsp_Idle();

            /* 遍历整个flash空间，决定哪个扇区需要擦除 */
            addr = g_tFLM.Device.DevAdr; 
            j = 0;            
            while (addr < g_tFLM.Device.DevAdr + g_tFLM.Device.szDev)
            {
                if (ProgCancelKey())
                {
                    PG_PrintText("用户终止运行");    
                    err = 1;
                    goto quit;                
                }
                
                fEraseReq = 0;
                
                /* 判断数据文件的目标扇区是否需要擦除 */
                if (_FlashAddr + FileLen >= addr && _FlashAddr < addr + g_tFLM.Device.sectors[j].szSector)
                {
                    fEraseReq = 1;   /* 需要擦除 */
                    
                    FinishedSize += g_tFLM.Device.sectors[j].szSector;  /* 已擦除的字节数 */
                }
                
                /* 判断FIX区是否需要擦除 */
                if (PG_CheckFlashFix(addr, g_tFLM.Device.sectors[j].szSector, _FileIndex))
                {
                    fEraseReq = 1;   /* 需要擦除 */
                }
 
                if (fEraseReq == 1)
                {                        
                    /* MM32F031x6 扇区擦除后需要加一点延迟才能继续，原因未知 */
                    bsp_DelayMS(waittime);
                    
                    if (target_flash_erase_sector(addr) != 0)
                    {
                        //PG_PrintText("扇区擦除失败");
                        {
                            char buf[128];
                            
                            sprintf(buf, "扇区擦除失败, 0x%08X", addr);
                            PG_PrintText(buf); 
                        }                        
                        err = 1;
                        goto quit;
                    }

                    /* 擦除成功，进度指示。 STML051，扇区只有128字节，4ms一个扇区。显示进度太占用时间。 */
                    {
                        float percent;
                        static int32_t s_time1 = 0;
                                                
                        percent = ((float)FinishedSize / FileLen) * 100; 
                        if (percent >= 100)
                        {
                            percent = 100;
                            s_time1 = 0;    /* 强制显示1次 */
                        }
                        /* 控制一下显示间隔，最快100ms */
                        if (bsp_CheckRunTime(s_time1) > 100)
                        {
                            PG_PrintPercent(percent, addr);
                            s_time1 = bsp_GetRunTime();
                        }
                    }                     
                }
                
                /* 下一个扇区 */
                addr += g_tFLM.Device.sectors[j].szSector;
                if (g_tFLM.Device.sectors[j + 1].AddrSector == 0xFFFFFFFF)
                {
                    ;
                }
                else if (addr >= g_tFLM.Device.sectors[j + 1].AddrSector + g_tFLM.Device.DevAdr)
                {
                    j++;
                }               
            }
        }
    }
    else    /* 整片擦除 */
    {
        if (fBlankChip == 0)     /* 不是空片才进行擦除 */
        {            
            PG_PrintText("正在擦除整片...");
			if (lua_CheckGlobal("MCU_EraseMass") == LUA_TFUNCTION)
			{
				printf("  MCU_EraseMass() from lua\r\n");
			}
			
            PG_PrintPercent(0, 0xFFFFFFFF);   
            bsp_Idle();            
        
            /* 根据算法名称判断芯片 */
            if (strstr(flash_algo.algo_file_name, "/MindMotion/"))
            {
                if (PG_SWD_EraseChip(g_tFLM.Device.DevAdr) == 1)
                {
                    PG_PrintText("整片擦除失败");        
                    err = 1;
                    goto quit;                    
                }
            }
            else
            {
                if (lua_CheckGlobal("MCU_EraseMass") == LUA_TFUNCTION)
                {
                    lua_do("MCU_EraseMass()");
                }
                else
                {
                    /*　开始擦除 */
                    if (target_flash_erase_chip() != 0)
                    {
                        PG_PrintText("整片擦除失败");        
                        err = 1;
                        goto quit;
                    }                    
                }
                PG_PrintPercent(100, 0xFFFFFFFF);                       
            }
        }
    }

    /* 第2步 ******************** 编程 **********************/
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
        float percent = 0;  
          
            
		PageSize = g_tFLM.Device.szPage;        /* 大多数情况这个是1K, SPI Flash是4K */
		if (PageSize > sizeof(FsReadBuf))
		{
			PageSize = sizeof(FsReadBuf);
		}
		
        /* 整定文件缓冲区大小为PageSize的整数倍, 芯片的pagesize一般为 128 256 512 1024 2048 4096 */
        FileBuffSize = sizeof(FsReadBuf);   
        FileBuffSize = (FileBuffSize / PageSize) * PageSize;
        
        addr = _FlashAddr;   /* 目标地址 */
        for (j = 0; j < (FileLen + FileBuffSize - 1) / FileBuffSize; j++)
        {
            uint8_t split = 0;  /* 多路模式拆分编程标志，因为滚码不同必须分别编程 */
			
            if (ProgCancelKey())
            {
                PG_PrintText("用户终止运行");    
                err = 1;
                goto quit;
            }

            /* 读文件, 按最大缓冲区读取到内存 */
			bytes = ReadFileToMem(path, FileOffset, FsReadBuf, FileBuffSize);               
			if (bytes == 0)
			{
				PG_PrintText("读取数据文件失败");        
				err = 1;
				goto quit;
			}
			else if (bytes != FileBuffSize) /* 文件末尾，最后一包不足FileBuffSize */
			{
				/* V1.10 修正bug : 烧写非整数倍PageSize的文件失败 */
				if (bytes % PageSize)
				{
					memset(&FsReadBuf[bytes], g_tFLM.Device.valEmpty, PageSize - (bytes % PageSize));      /* 填充空值*/
					
					bytes = ((bytes + PageSize - 1) / PageSize) * PageSize;
				}                    
			}
			else    /* bytes == FileBuffSize */
			{
				;
			}                
			
            split = 0;
            if (PG_CheckFixSplit(addr, bytes, _FileIndex) == 1)
			{
                split = 1;      /* 通道数据不同需要顺序编程 */
			}				         
            
            if (split == 0)     /* 每个通道数据都相同的情况 - 确保大多情况不影响整体烧录效率 */
            {    
                /* 修改缓冲区，填充UID加密数据或产品序号 */
                PG_FixFlashMem(addr, FsReadBuf, bytes, _FileIndex, 0); 
                    
                for (i = 0; i < bytes / PageSize; i++)
                {                                                
                    if (target_flash_program_page(addr, (uint8_t *)&FsReadBuf[i * PageSize], PageSize) != 0)
                    {
                        {
                            char buf[128];
                            
                            sprintf(buf, "编程失败, 0x%08X", addr);
                            PG_PrintText(buf); 
                        }                              
                        err = 1;
                        goto quit;
                    }
                       
                    addr += PageSize;
                    FileOffset += PageSize;
                }  
            }
            else    /* 通道之间数据不相同的情况 */
            {
                uint8_t k;
                
                
                for (i = 0; i < bytes / PageSize; i++)
                {                                
                    split = 0;
                    if (PG_CheckFixSplit(addr, PageSize, _FileIndex) == 1)
                    {
                        split = 1;      /* 通道数据不同需要顺序编程 */
                    }
            
                    if (split == 0)
                    {
                        PG_FixFlashMem(addr, (char *)&FsReadBuf[i * PageSize], PageSize, _FileIndex, 0);

                        if (target_flash_program_page(addr, (uint8_t *)&FsReadBuf[i * PageSize], PageSize) != 0)
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
                    else
                    {
                        for (k = 1; k <= g_gMulSwd.MultiMode; k++)
                        {
                            /* 修改缓冲区，填充UID加密数据或产品序号 */
                            PG_FixFlashMem(addr, (char *)&FsReadBuf[i * PageSize], PageSize, _FileIndex, k);
                            
                            /* 需要切换其中一路烧录 */
                            g_gMulSwd.Active[0] = 0;
                            g_gMulSwd.Active[1] = 0;
                            g_gMulSwd.Active[2] = 0;
                            g_gMulSwd.Active[3] = 0;                           
                            g_gMulSwd.Active[k - 1] = 1;
                            
                            if (target_flash_program_page(addr, (uint8_t *)&FsReadBuf[i * PageSize], PageSize) != 0)
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
                    }
                        
                    /* 还原到多路 */
                    if (g_gMulSwd.MultiMode >= 1) g_gMulSwd.Active[0] = 1;
                    if (g_gMulSwd.MultiMode >= 2) g_gMulSwd.Active[1] = 1;
                    if (g_gMulSwd.MultiMode >= 3) g_gMulSwd.Active[2] = 1;
                    if (g_gMulSwd.MultiMode >= 4) g_gMulSwd.Active[3] = 1; 
                    
                    addr += PageSize;
                    FileOffset += PageSize;
                }
              
            }
            
            /* 进度指示 */
            {
                if (percent < 100)
                {        
                    percent = ((float)FileOffset / FileLen) * 100;
                    if (percent > 100)
                    {
                        percent = 100;
                    }                        
                    PG_PrintPercent(percent, addr);                   
                }
            }                
        }
        
		/* 处理文件之后的flash空间，写入fix区数据 */
		{
			uint32_t RemLen;
			uint8_t split;
            uint32_t bytes;

			RemLen = _FlashAddr + CheckLen - addr;	/* 剩余空间 */
			
			/* 如果UID加密数据或产品序号在文件以外的空间 */
			if (PG_CheckFlashFix(addr, RemLen, _FileIndex))
			{ 
				FileBuffSize = sizeof(FsReadBuf); 
				
                while(1)
                {
                    bytes = FileBuffSize;
                    if (bytes > RemLen)
                    {
                        bytes = RemLen; 
                    }

                    if (PG_CheckFlashFix(addr, bytes, _FileIndex))
                    {                    
                        split = 0;
                        if (PG_CheckFixSplit(addr, bytes, _FileIndex) == 1)
                        {
                            split = 1;      /* 通道数据不同需要顺序编程 */
                        }
                                            
                        if (split == 0)     /* 每个通道数据都相同的情况 - 确保大多情况不影响整体烧录效率 */
                        {    	
                            memset(FsReadBuf, g_tFLM.Device.valEmpty, FileBuffSize);   /* 填充空值 */
                            
                            /* 修改缓冲区，填充UID加密数据或产品序号 */
                            PG_FixFlashMem(addr, FsReadBuf, FileBuffSize, _FileIndex, 0);                            
                            for (i = 0; i < bytes / PageSize; i++)
                            {                       
                                if (PG_CheckFlashFix(addr, PageSize, _FileIndex))
                                {
                                    if (target_flash_program_page(addr, (uint8_t *)&FsReadBuf[i * PageSize], PageSize) != 0)
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
                                addr += PageSize;
                                RemLen -= PageSize;
                            }
                        }
                        else    /* 通道之间数据不相同的情况 */
                        {     
                            uint8_t k;
                            
                            for (i = 0; i < bytes / PageSize; i++)
                            {         
                                if (PG_CheckFlashFix(addr, PageSize, _FileIndex))
                                {
                                    for (k = 1; k <= g_gMulSwd.MultiMode; k++)
                                    {
                                        /* 修改缓冲区，填充UID加密数据或产品序号 */
                                        PG_FixFlashMem(addr, (char *)&FsReadBuf[i * PageSize], PageSize, _FileIndex, k);
                                        
                                        /* 需要切换其中一路烧录 */
                                        g_gMulSwd.Active[0] = 0;
                                        g_gMulSwd.Active[1] = 0;
                                        g_gMulSwd.Active[2] = 0;
                                        g_gMulSwd.Active[3] = 0;                           
                                        g_gMulSwd.Active[k - 1] = 1;
                                        
                                        if (target_flash_program_page(addr, (uint8_t *)&FsReadBuf[i * PageSize], PageSize) != 0)
                                        {
                                            {
                                                char buf[128];
                                                
                                                sprintf(buf, "编程失败, 0x%08X", addr);
                                                PG_PrintText(buf); 
                                            }                              
                                            err = 1;
                                            goto quit;
                                        }                                                        
                                        
                                        /* 还原到多路 */
                                        if (g_gMulSwd.MultiMode >= 1) g_gMulSwd.Active[0] = 1;
                                        if (g_gMulSwd.MultiMode >= 2) g_gMulSwd.Active[1] = 1;
                                        if (g_gMulSwd.MultiMode >= 3) g_gMulSwd.Active[2] = 1;
                                        if (g_gMulSwd.MultiMode >= 4) g_gMulSwd.Active[3] = 1;                        
                                    }
                                }
                                
                                addr += PageSize;
                                RemLen -= PageSize;
                            }              
                        }
                    }   /* if (PG_CheckFlashFix(addr, bytes, _FileIndex)) */
                    else
                    {
                        addr += bytes;
                        RemLen -= bytes;                        
                    }                    
                
                    if (addr >= _FlashAddr + CheckLen)
                    {
                        break;
                    }                   
				}   
			}
		}
    }
    
    /* 第3步 ******************** 校验 （fix区未完全校验）**********************/ 
    
    #if 1   /* 2020-12-13 加入，V1.43 */
        /* 执行Uninit函数 */
        err_t = target_flash_uninit();
        if (err_t != ERROR_SUCCESS)
        {
            PG_PrintText("error: target_flash_uninit()");
            err = 1;
            goto quit;
        }
        
        /* 装载算法并执行init函数 */
        err_t = target_flash_init(_FlashAddr, 0, FLM_INIT_VERIFY);
        if (err_t != ERROR_SUCCESS)
        {
            PG_PrintText("error: target_flash_init(FLM_INIT_VERIFY)");
            err = 1;
            goto quit;
        }
    #endif
    
    /* 
        2020-05-22 记录: STM32F207RCT6，256K Flash
            FLM_CRC32    150ms
            READ_BACK    461ms
            STM32_CRC32  170ms    
            SOFT_CRC32   275ms
    
        2020-05-23   STM32G474CET6, 512K Flash
            READ_BACK    931ms
            SOFT_CRC32   558ms  
            STM32_CRC32  316ms    
    */
    if (flash_algo.cacul_crc32 > 0 && g_tProg.VerifyMode == VERIFY_AUTO)
    {
        PG_PrintText("正在校验...(FLM_CRC32)"); 
    }
    else
    {        
        if (g_tProg.VerifyMode == VERIFY_SOFT_CRC)
        {
            PG_PrintText("正在校验...(SOFT_CRC32)");  
        }
        else if (g_tProg.VerifyMode == VERIFY_STM32_CRC)
        {
            PG_PrintText("正在校验...(STM32_CRC32)"); 
        }    
        else    /* VERIFY_READ_BACK */
        {
            if (flash_algo.verify > 0 && g_tProg.VerifyMode == VERIFY_AUTO)                
            {    
                PG_PrintText("正在校验...(FLM_Verify)");  
            }
            else
            {
                PG_PrintText("正在校验...(Readback)");  
            }
        }
        
        /* 加载MCU的编程算法到目标机内存 */
        if (g_tProg.VerifyMode == VERIFY_SOFT_CRC || g_tProg.VerifyMode == VERIFY_STM32_CRC)
        {
            /* 装载算法代码到目标机内存 */
            LoadCheckCRCAlgoToTarget();
        }
    }
    
	/* 新唐GD32 校验不通过，难道必须执行如下这句话，其他ARM暂无问题? 实际测试也不行
        新唐的片子直接用Readback模式校验
    */
    #if 0 
        target_flash_uninit();    
        target_flash_init(_FlashAddr, 0, FLM_INIT_VERIFY);
	#endif
    
    PG_PrintPercent(0, 0xFFFFFFFF);    
    {
        uint32_t addr;
		uint32_t FileOffset = 0;
		uint16_t PageSize;
		uint32_t bytes;
   
        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
        {
			if ( (flash_algo.cacul_crc32 > 0 && g_tProg.VerifyMode == VERIFY_AUTO)
                || g_tProg.VerifyMode == VERIFY_SOFT_CRC 
                || g_tProg.VerifyMode == VERIFY_STM32_CRC)  /* 由目标机执行CRC校验 */
			{
				PageSize = sizeof(flash_buff);  
			}
			else if (flash_algo.verify > 0 && g_tProg.VerifyMode == VERIFY_AUTO)     /* FLM有verify校验函数 */
			{
				PageSize = sizeof(flash_buff);  
			}
			else	/* 读回校验 */
			{
				PageSize = sizeof(flash_buff) / 4;
			}					
		}
        else
        {
            PageSize = sizeof(flash_buff);           
        }
        
        if (PageSize >= sizeof(FsReadBuf))
        {
            PageSize = sizeof(FsReadBuf);
        }

        /* 2020-12-13 FLM有verify校验函数, page_size 按照FLM中来，针对SPI FLASH, QSPI_flash */
        if (flash_algo.verify > 0 && g_tProg.VerifyMode == VERIFY_AUTO)     
        {
            PageSize = g_tFLM.Device.szPage;
        }
        
        addr = _FlashAddr - g_tFLM.Device.DevAdr;   /* 求相对地址, 方便后面计算 */
        for (; FileOffset < FileLen; )
        {
            uint8_t split;  /* 多路模式拆分编程标志，因为滚码不同必须分别编程 */
            
            split = 0;
            if (PG_CheckFixSplit(g_tFLM.Device.DevAdr + addr, bytes, _FileIndex) == 1)
			{
                split = 1;      /* 通道数据不同需要顺序编程 */
			}
            
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
                
                memset(&FsReadBuf[bytes], g_tFLM.Device.valEmpty ,PageSize - bytes);
			}    
            
            if (split == 0)     /* 每个通道数据都相同的情况 - 确保大多情况不影响整体烧录效率 */
            {                			
                /* 修改缓冲区，填充UID加密数据或产品序号 */
                PG_FixFlashMem(g_tFLM.Device.DevAdr + addr, FsReadBuf, PageSize, _FileIndex, 0);           
                
                if ( (flash_algo.cacul_crc32 > 0 && g_tProg.VerifyMode == VERIFY_AUTO)
                    || g_tProg.VerifyMode == VERIFY_SOFT_CRC 
                    || g_tProg.VerifyMode == VERIFY_STM32_CRC)  /* 由目标机执行CRC校验 */
                {
                    uint32_t crc1, crc2;

                    /* 文件长度不是4字节整数倍，则补齐后再进行硬件CRC32 */
                    {
                        uint8_t rem;
                        uint8_t k;
                        
                        rem = bytes % 4;
                        if (rem > 0)
                        {
                            rem = 4 - rem;
                            for (k = 0; k < rem; k++)
                            {
                                FsReadBuf[bytes + k] = g_tFLM.Device.valEmpty;
                            }
                            bytes += rem;
                        }
                    }
                    
                    if (flash_algo.cacul_crc32 > 0 && g_tProg.VerifyMode == VERIFY_AUTO)     /* 执行FLM中的crc算法 */
                    {
                        crc1 = target_flash_cacul_crc32(g_tFLM.Device.DevAdr + addr, bytes, 0xFFFFFFFF);
                        crc2 = STM32_CRC32_Word((uint32_t *)FsReadBuf, bytes);      /* 目前仅支持STM32算法 */                     
                    }
                    else    /* 临时加载到目标机的通用算法 */
                    {
                        crc1 = target_flash_cacul_crc32_ex(g_tFLM.Device.DevAdr + addr, bytes, 0xFFFFFFFF);
                        if (g_tProg.VerifyMode == VERIFY_STM32_CRC)
                        {
                            crc2 = STM32_CRC32_Word((uint32_t *)FsReadBuf, bytes);      /* 目前仅支持STM32算法 */     
                        }
                        else    /* (g_tProg.VerifyMode == VERIFY_SOFT_CRC) */
                        {
                            crc2 = soft_crc32((uint8_t *)FsReadBuf, bytes);   
                        }
                    }
                        
                    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                    {                    
                        uint8_t i;
                        char errstr[32];
                        uint8_t err0 = 0;                        
          
                        strcpy(errstr, "CRC校验失败");
                        
                        if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
                        {
                            for (i = 0; i < 4; i++)
                            {
                                if (g_gMulSwd.Active[i] == 1)
                                {
                                    if (crc2 != ((uint32_t *)crc1)[i])
                                    {                     
                                        err0 = 1;
                                        
                                        sprintf(&errstr[strlen(errstr)], " #%d", i + 1);
                                        g_gMulSwd.Error[i] = 1;
                                    }
                                }
                            }                           
                        }
                        else
                        {
                            err0 = 1;
                            
                            for (i = 0; i < 4; i++)
                            {
                                if (g_gMulSwd.Active[i] == 1 && g_gMulSwd.Error[i] == 0)
                                {
                                    if (crc2 != ((uint32_t *)crc1)[i])
                                    {                     
                                        //err0 = 1;
                                        
                                        sprintf(&errstr[strlen(errstr)], " #%d", i + 1);
                                        g_gMulSwd.Error[i] = 1;
                                    }
                                    else
                                    {
                                        err0 = 0; 
                                    }
                                }
                            }                            
                        }

                        if (err0 == 1)
                        {
                            PG_PrintText(errstr);  
                            err = 1;
                            goto quit;	                        
                        }                         
                    }
                    else    /* 单路模式 */
                    {
                        if (((uint32_t *)crc1)[0] != crc2)
                        {              
                            {
                                char buf[128];
                                
                                sprintf(buf, "校验失败, 0x%08X", g_tFLM.Device.DevAdr + addr);
                                PG_PrintText(buf); 
                                
                                printf("crc_read = %08X  crc_ok = %08X\r\n", crc1, crc2);
                            }
                            g_gMulSwd.Error[0] = 1;
                            err = 1;
                            goto quit;	                    
                        }                    
                    }                              
                }
                else if (flash_algo.verify > 0 && g_tProg.VerifyMode == VERIFY_AUTO)     /* FLM有verify校验函数 */
                {
                    if (target_flash_verify_page(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) != 0)
                    {
                        {
                            char buf[128];
                            
                            sprintf(buf, "校验失败, 0x%08X", g_tFLM.Device.DevAdr + addr);
                            PG_PrintText(buf); 
                        }   
                        err = 1;
                        goto quit;                    
                    }
                }
                else    /* readback 校验 */
                {
                    
                    if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                    {
                        uint8_t i;
                        
                        /* 读回进行校验 */                    
                        if (MUL_swd_read_memory(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) == 0)
                        {
                            char buf[128];
                            
                            sprintf(buf, "swd_read_memory error, addr = %X, len = %X", g_tFLM.Device.DevAdr + addr, bytes);
                            PG_PrintText(buf);    
                            err = 1;
                            goto quit;  
                        }

                        if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
                        {
                            for (i = 0; i < 4; i++)
                            {
                                if (g_gMulSwd.Active[i] == 1)
                                {                    
                                    if (memcmp(FsReadBuf, &flash_buff[bytes * i], bytes) != 0)
                                    {
                                        {
                                            char buf[128];
                                            
                                            sprintf(buf, "校验失败, 0x%08X", g_tFLM.Device.DevAdr + addr);
                                            PG_PrintText(buf); 
                                        }                     
                                        err = 1;
                                        g_gMulSwd.Error[i] = 1;
                                        //goto quit;				
                                    } 
                                }
                            }
                        }
                        else
                        {
                            err = 1;
                            for (i = 0; i < 4; i++)
                            {
                                if (g_gMulSwd.Active[i] == 1 && g_gMulSwd.Error[i] == 0)
                                {                    
                                    if (memcmp(FsReadBuf, &flash_buff[bytes * i], bytes) != 0)
                                    {
                                        {
                                            char buf[128];
                                            
                                            sprintf(buf, "校验失败, 0x%08X", g_tFLM.Device.DevAdr + addr);
                                            PG_PrintText(buf); 
                                        }                
                                        g_gMulSwd.Error[i] = 1;
                                        //err = 1;
                                        //goto quit;				
                                    }
                                    else
                                    {
                                        err = 0;    /* 至少1路OK */
                                    }
                                }
                            }      
                        }
                    }
                    else    /* 单路模式 */
                    {
                        /* 读回进行校验 */                    
                        if (swd_read_memory(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) == 0)
                        {
                            char buf[128];
                            
                            sprintf(buf, "swd_read_memory error, addr = %X, len = %X", g_tFLM.Device.DevAdr + addr, bytes);
                            PG_PrintText(buf);    
                            err = 1;
                            goto quit;  
                        }
                            
                        if (memcmp(FsReadBuf, flash_buff, bytes) != 0)
                        {
                            {
                                char buf[128];
                                
                                sprintf(buf, "校验失败, 0x%08X", g_tFLM.Device.DevAdr + addr);
                                PG_PrintText(buf); 
                            }                                               
                            err = 1;
                            goto quit;				
                        } 
                    }            
                }
                addr += PageSize;
                FileOffset += PageSize;                    
            }
            else    /* 校验分支，每个芯片不同时，至少2通道以上 */
            {
                uint8_t ch;

                for (ch = 1; ch <= g_gMulSwd.MultiMode; ch++)
                {
                    /* 需要切换其中一路烧录 */
                    g_gMulSwd.Active[0] = 0;
                    g_gMulSwd.Active[1] = 0;
                    g_gMulSwd.Active[2] = 0;
                    g_gMulSwd.Active[3] = 0;                           
                    g_gMulSwd.Active[ch - 1] = 1;								
                    
                    /* 修改缓冲区，填充UID加密数据或产品序号 */
                    PG_FixFlashMem(g_tFLM.Device.DevAdr + addr, FsReadBuf, PageSize, _FileIndex, ch);           
                    
                    if ( (flash_algo.cacul_crc32 > 0 && g_tProg.VerifyMode == VERIFY_AUTO)
                        || g_tProg.VerifyMode == VERIFY_SOFT_CRC 
                        || g_tProg.VerifyMode == VERIFY_STM32_CRC)  /* 由目标机执行CRC校验 */
                    {
                        uint32_t crc1, crc2;

                        /* 文件长度不是4字节整数倍，则补齐后再进行硬件CRC32 */
                        {
                            uint8_t rem;
                            uint8_t k;
                            
                            rem = bytes % 4;
                            if (rem > 0)
                            {
                                rem = 4 - rem;
                                for (k = 0; k < rem; k++)
                                {
                                    FsReadBuf[bytes + k] = g_tFLM.Device.valEmpty;
                                }
                                bytes += rem;
                            }
                        }
                        
                        if (flash_algo.cacul_crc32 > 0 && g_tProg.VerifyMode == VERIFY_AUTO)     /* 执行FLM中的crc算法 */
                        {
                            crc1 = target_flash_cacul_crc32(g_tFLM.Device.DevAdr + addr, bytes, 0xFFFFFFFF);
                            crc2 = STM32_CRC32_Word((uint32_t *)FsReadBuf, bytes);      /* 目前仅支持STM32算法 */                     
                        }
                        else    /* 临时加载到目标机的通用算法 */
                        {
                            crc1 = target_flash_cacul_crc32_ex(g_tFLM.Device.DevAdr + addr, bytes, 0xFFFFFFFF);
                            if (g_tProg.VerifyMode == VERIFY_STM32_CRC)
                            {
                                crc2 = STM32_CRC32_Word((uint32_t *)FsReadBuf, bytes);      /* 目前仅支持STM32算法 */     
                            }
                            else    /* (g_tProg.VerifyMode == VERIFY_SOFT_CRC) */
                            {
                                crc2 = soft_crc32((uint8_t *)FsReadBuf, bytes);   
                            }
                        }
                            
                        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                        {                    
                            uint8_t i;
                            uint8_t err0 = 0;
                            char errstr[32];
              
                            strcpy(errstr, "CRC校验失败");
                            
                            if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
                            {
                                for (i = 0; i < 4; i++)
                                {
                                    if (g_gMulSwd.Active[i] == 1)
                                    {
                                        if (crc2 != ((uint32_t *)crc1)[i])
                                        {                     
                                            err0 = 1;
                                            
                                            sprintf(&errstr[strlen(errstr)], " #%d", i + 1);
                                            g_gMulSwd.Error[i] = 1;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                err0 = 1;
                                for (i = 0; i < 4; i++)
                                {
                                    if (g_gMulSwd.Active[i] == 1 && g_gMulSwd.Error[i] == 0)
                                    {
                                        if (crc2 != ((uint32_t *)crc1)[i])
                                        {                     
                                            //err0 = 1;                                            
                                            sprintf(&errstr[strlen(errstr)], " #%d", i + 1);
                                            g_gMulSwd.Error[i] = 1;
                                        }
                                        else
                                        {
                                            err0 = 0;
                                        }
                                    }
                                }                                
                            }
                            
                            if (err0 == 1)
                            {
                                PG_PrintText(errstr);  
                                err = 1;
                                goto quit;	                        
                            }
                        }                               
                    }
                    else if (flash_algo.verify > 0 && g_tProg.VerifyMode == VERIFY_AUTO)     /* FLM有verify校验函数 */
                    {
                        if (target_flash_verify_page(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) != 0)
                        {
                            {
                                char buf[128];
                                
                                sprintf(buf, "校验失败3, 0x%08X", g_tFLM.Device.DevAdr + addr);
                                PG_PrintText(buf); 
                            }   
                            err = 1;
                            goto quit;                    
                        }
                    }
                    else    /* readback 校验 */
                    {
                        
                        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                        {
                            uint8_t i;
                            
                            /* 读回进行校验 */                    
                            if (MUL_swd_read_memory(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) == 0)
                            {
                                char buf[128];
                                
                                sprintf(buf, "swd_read_memory error, addr = %X, len = %X", g_tFLM.Device.DevAdr + addr, bytes);
                                PG_PrintText(buf);    
                                err = 1;
                                goto quit;  
                            }

                            if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
                            {
                                for (i = 0; i < 4; i++)
                                {
                                    if (g_gMulSwd.Active[i] == 1)
                                    {                    
                                        if (memcmp(FsReadBuf, &flash_buff[bytes * i], bytes) != 0)
                                        {
                                            {
                                                char buf[128];
                                                
                                                sprintf(buf, "校验失败1, 0x%08X", g_tFLM.Device.DevAdr + addr);
                                                PG_PrintText(buf); 
                                            }              
                                            g_gMulSwd.Error[i] = 1;                                            
                                            err = 1;
                                            goto quit;				
                                        } 
                                    }
                                }
                            }
                            else
                            {
                                err = 1;
                                for (i = 0; i < 4; i++)
                                {
                                    if (g_gMulSwd.Active[i] == 1 && g_gMulSwd.Error[i] == 0)
                                    {                    
                                        if (memcmp(FsReadBuf, &flash_buff[bytes * i], bytes) != 0)
                                        {
                                            {
                                                char buf[128];
                                                
                                                sprintf(buf, "校验失败2, 0x%08X", g_tFLM.Device.DevAdr + addr);
                                                PG_PrintText(buf); 
                                            }                   
                                            g_gMulSwd.Error[i] = 1;                                            
                                            //err = 1;
                                            //goto quit;				
                                        }
                                        else
                                        {
                                            err = 0;
                                        }
                                    }
                                }                               
                            }
                        }
                        else
                        {
                            /* 不会进来 */
                        }
                    }
                }

                /* 还原到多路 */
                if (g_gMulSwd.MultiMode >= 1) g_gMulSwd.Active[0] = 1;
                if (g_gMulSwd.MultiMode >= 2) g_gMulSwd.Active[1] = 1;
                if (g_gMulSwd.MultiMode >= 3) g_gMulSwd.Active[2] = 1;
                if (g_gMulSwd.MultiMode >= 4) g_gMulSwd.Active[3] = 1;   
                
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
                PG_PrintPercent(percent, g_tFLM.Device.DevAdr + addr);                
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

/*
*********************************************************************************************************
*    函 数 名: PG_SWD_ProgBuf
*    功能说明: 开始编程flash。 读修改写，限制在一个page内。 仅仅用来写OB
*    形    参:  _FlashAddr
*				_DataBuf : 数据buf
*               _BufLen : 数据长度
*               _Mode : 编程模式 0表示读回修改再写  1表示擦除所在扇区再写入
*    返 回 值: 0 = ok, 其他表示错误
*********************************************************************************************************
*/
uint16_t PG_SWD_ProgBuf(uint32_t _FlashAddr, uint8_t *_DataBuf, uint32_t _BufLen, uint8_t _Mode)
{
    uint8_t err = 0;
    error_t err_t;
    uint32_t FileLen;     
    uint32_t bytes;    
    
    FileLen = _BufLen;
    
    /* 2020-09-03 */
    if (_FlashAddr != g_tFLM.Device.DevAdr)
    {
        g_tFLM.Device.DevAdr = _FlashAddr;      /* STM32L5xx 会用到 */
    }
    
    /* SWD进入debug状态 */
    err_t = target_flash_enter_debug_program();
    if (err_t != ERROR_SUCCESS)
    {
        err = 1;
        goto quit;  
    }
    /* 装载算法代码到目标机内存 */
    LoadAlgoToTarget();
    
    /* 装载算法并执行init函数 */
	err_t = target_flash_init(_FlashAddr, 0, FLM_INIT_ERASE);
    if (err_t != ERROR_SUCCESS)
    {
        err = 1;
        goto quit;
    }
    
    /* 判断参数是否合法 */
//    if (_FlashAddr >= g_tFLM.Device.DevAdr && _FlashAddr + _BufLen <= g_tFLM.Device.DevAdr + g_tFLM.Device.szDev)
//    {
//        ;
//    }
//    else
//    {
//        PG_PrintText("数据文件长度超过芯片容量");        
//        err = 1;
//        goto quit;             
//    }    

    /* 循环执行：读回比对、查空、擦除、编程page、比对 */   
    printf("Program option bytes\r\n");   
    PG_PrintPercent(0, 0xFFFFFFFF);    
    {
        uint32_t addr;
		uint32_t FileOffset = 0;
		uint16_t PageSize;
        uint32_t PageStartAddr = 0;
   
		PageSize = g_tFLM.Device.szPage;
		if (PageSize > sizeof(flash_buff))
		{
			PageSize = sizeof(flash_buff);
		}
		
        addr = _FlashAddr - g_tFLM.Device.DevAdr;   /* 求相对地址, 方便后面计算 */
        for (; FileOffset < FileLen; )
        {        
            bytes = PageSize;
            if (FileLen < bytes)
            {
                bytes = FileLen;
            }
            
            /* page起始地址 */
            PageSize = g_tFLM.Device.szPage;
            PageStartAddr = (addr / PageSize) * PageSize;
            if (PageSize > sizeof(flash_buff))
            {
                PG_PrintText("page size 过大");        
                err = 1;
                goto quit;
            }
            
            if (_Mode == 2) /* 擦除所在扇区后，写入，其他数据会清空. 用于Options bytes编程 */
            {
                uint32_t write_size;
                                
                /*　开始擦除 - STM32F429 包含解除读保护 */ 
                //printf("\r\nOption Bytes: erase_chip()\r\n");
                if (target_flash_erase_chip() != 0)
                {
                    PG_PrintText("整片擦除失败");        
                    err = 1;
                    goto quit;
                }      

                /* STM32F103 option bytes 编程时需要执行 erase_sector */
                g_tProg.FLMFuncDispProgress = 1;
                g_tProg.FLMFuncDispAddr = g_tFLM.Device.DevAdr + PageStartAddr;
                //printf("Option Bytes: erase_sector()\r\n");
                if (target_flash_erase_sector(g_tFLM.Device.DevAdr + PageStartAddr) != 0)
                {
                    PG_PrintText("扇区擦除失败");        
                    err = 1;
                    goto quit;
                } 
                
                /* 未判断返回值。写STM32F103 OPTION BYTES会返回错误, 但是写入已经成功 */
                //PG_PrintText("正在编程...");
                g_tProg.FLMFuncDispProgress = 1;
                g_tProg.FLMFuncDispAddr = g_tFLM.Device.DevAdr + PageStartAddr;
                
                write_size = PageSize;
                if (write_size > g_tFLM.Device.szDev)
                {
                    write_size = g_tFLM.Device.szDev;
                }
                
                //printf("Option Bytes: program_page()\r\n");
                if (target_flash_program_page(g_tFLM.Device.DevAdr + PageStartAddr, _DataBuf, write_size) != 0)
                {
                    PG_PrintText("program_page failed");        
                    err = 1;
                    goto quit;                    
                }
                
                /*  */
                //PG_PrintText("正在校验..."); 
                
                if (g_tProg.VerifyOptionByteDisalbe == 0)
                {
                    //printf("\r\nOption Bytes: verify\r\n");
                    if (flash_algo.verify > 0)
                    {
                        if (target_flash_verify_page(g_tFLM.Device.DevAdr + PageStartAddr, _DataBuf, PageSize) != 0)
                        {
                            PG_PrintText("校验数据失败");        
                            err = 1;
                            goto quit;                    
                        }
                    }
                    else
                    {	
                        if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
                        {
                            uint8_t i;
                            
                            /* 读回进行校验 */
                            if (MUL_swd_read_memory(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) == 0)
                            {
                                PG_PrintText("swd_read_memory error");        
                                err = 1;
                                goto quit;				
                            } 
                            
                            if (g_tProg.AbortOnError == 1)   /* 有1个错误 则返回错误 */
                            {
                                for (i = 0; i < 4; i++)
                                {
                                    if (g_gMulSwd.Active[i] == 1)
                                    {                    
                                        if (memcmp(&_DataBuf[FileOffset], &flash_buff[bytes * i], bytes) != 0)
                                        {
                                            PG_PrintText("校验数据失败");                            
                                            err = 1;
                                            goto quit;				
                                        } 
                                    }
                                }
                            }
                            else
                            {
                                err = 1;
                                for (i = 0; i < 4; i++)
                                {
                                    if (g_gMulSwd.Active[i] == 1)
                                    {                    
                                        if (memcmp(&_DataBuf[FileOffset], &flash_buff[bytes * i], bytes) != 0)
                                        {
                                            PG_PrintText("校验数据失败");                            
                                            //err = 1;
                                            //goto quit;
                                            g_gMulSwd.Error[i] = 1;
                                        }
                                        else
                                        {
                                            err = 0;
                                        }
                                    }
                                }
                                if (err == 1)
                                {
                                    err = 1;
                                    goto quit;
                                }
                            }
                        }
                        else
                        {
                            /* 读回进行校验 */
                            if (swd_read_memory(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) == 0)
                            {
                                PG_PrintText("swd_read_memory error");        
                                err = 1;
                                goto quit;				
                            }                     
                            if (memcmp((uint8_t *)&_DataBuf[FileOffset], flash_buff, bytes) != 0)
                            {
                                PG_PrintText("校验数据失败");        
                                err = 1;
                                goto quit;				
                            }
                        }
                    }
                }
                else
                {
                    printf("Verify Option bytes cancelled\r\n");
                }
            }
            else if (_Mode == 1) /* 擦除所在扇区后，写入，其他数据会清空 */
            {
//                PG_PrintText("正在擦除扇区..."); 
                
//                /*　开始擦除扇区 */
//                if (target_flash_erase_sector(g_tFLM.Device.DevAdr + PageStartAddr) != 0)
//                {
//                    PG_PrintText("扇区擦除失败");        
//                    err = 1;
//                    goto quit;
//                }         

//                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
//                {
//                    MUL_swd_read_memory(PageStartAddr + g_tFLM.Device.DevAdr, flash_buff, PageSize);
//                }
//                else
//                {
//                    swd_read_memory(PageStartAddr + g_tFLM.Device.DevAdr, flash_buff, PageSize);
//                }
//                memcpy(&flash_buff[addr % PageSize], (uint8_t *)&_DataBuf[FileOffset], bytes);               
//                
////                PG_PrintText("正在编程..."); 
//                /* 整页编程 */
//                if (target_flash_program_page(g_tFLM.Device.DevAdr + PageStartAddr, flash_buff, PageSize) != 0)
//                {
//                    PG_PrintText("编程失败");        
//                    err = 1;
//                    goto quit;
//                }

//                memset(flash_buff, 0xFF, PageSize);
//                
//                /* 读回进行校验 */
//                if (g_gMulSwd.MultiMode > 0)   /* 多路模式 */
//                {
//                    uint8_t i;
//                    
//                    if (swd_read_memory(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) == 0)
//                    {
//                        PG_PrintText("swd_read_memory error");        
//                        err = 1;
//                        goto quit;				
//                    }                
//                    for (i = 0; i < 4; i++)
//                    {
//                        if (g_gMulSwd.Active[i] == 1)
//                        {                    
//                            if (memcmp(&_DataBuf[FileOffset], &flash_buff[bytes * i], bytes) != 0)
//                            {
//                                PG_PrintText("校验数据失败");                            
//                                err = 1;
//                                goto quit;				
//                            } 
//                        }
//                    }                     
//                }
//                else
//                {
//                    if (MUL_swd_read_memory(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) == 0)
//                    {
//                        PG_PrintText("swd_read_memory error");        
//                        err = 1;
//                        goto quit;				
//                    }       
//                    
//                    if (memcmp((uint8_t *)&_DataBuf[FileOffset], flash_buff, bytes) != 0)
//                    {
//                        PG_PrintText("校验数据失败");        
//                        err = 1;
//                        goto quit;				
//                    }
//                }
            }
            else if (_Mode == 0)    /* 读 - 修改 - 写，同page内其他数据保持不变 */
            {
// 这个分支暂时未用到，屏蔽                
//                swd_read_memory(PageStartAddr + g_tFLM.Device.DevAdr, flash_buff, PageSize);        /* 读取一个page  */
//                
//                /* 如果内容相同则不处理 */
//                if (memcmp(&flash_buff[addr % PageSize], (uint8_t *)&_DataBuf[FileOffset], bytes) != 0)
//                {
//                    /* 如果不是空值就擦除 */
//                    //if (CheckBlankBuf((char *)&flash_buff[addr % PageSize], bytes, g_tFLM.Device.valEmpty) == 0)  
//                    {
//                        /*　开始擦除 */
//                        if (target_flash_erase_sector(g_tFLM.Device.DevAdr + PageStartAddr) != 0)
//                        {
//                            PG_PrintText("扇区擦除失败");        
//                            err = 1;
//                            goto quit;
//                        }
//                        
//                        //swd_read_memory(PageStartAddr + g_tFLM.Device.DevAdr, flash_buff, PageSize);
//                    }                                
//                    
//                    memcpy(&flash_buff[addr % PageSize], (uint8_t *)&_DataBuf[FileOffset], bytes);                   
//                    
//                    /* 整页编程 */
//                    if (target_flash_program_page(g_tFLM.Device.DevAdr + PageStartAddr, flash_buff, PageSize) != 0)
//                    {
//                        PG_PrintText("编程失败");        
//                        err = 1;
//                        goto quit;
//                    }
//                    
//                    memset(flash_buff, 0xFF, PageSize);
//                    
//                    /* 读回进行校验 */
//                    if (swd_read_memory(g_tFLM.Device.DevAdr + addr, flash_buff, bytes) == 0)
//                    {
//                        PG_PrintText("swd_read_memory error");        
//                        err = 1;
//                        goto quit;				
//                    }   
//                  
//                    if (memcmp((uint8_t *)&_DataBuf[FileOffset], flash_buff, bytes) != 0)
//                    {
//                        PG_PrintText("校验数据失败");        
//                        err = 1;
//                        goto quit;				
//                    }            
//                }
            }
            
            addr += PageSize;
			FileOffset += PageSize;
            
            /* 进度指示 */
            {
                float percent;
                
                percent = ((float)addr / FileLen) * 100;
                if (percent > 100)
                {
                    percent = 100;
                }                
                PG_PrintPercent(percent, g_tFLM.Device.DevAdr + addr);           
            }
        }
    }
	
//	swd_set_target_state_hw(RUN);
quit:
    if (g_gMulSwd.MultiMode == 0)
    {
        g_gMulSwd.Error[0] = err;   /* 单路模式，借用该变量用于机台信号lua程序用 */
    }    
    return err;
}

/*
*********************************************************************************************************
*    函 数 名: PG_SWD_ProgBuf_OB
*    功能说明: 开始编程option 。解锁读保护时，擦除或编程时间可能很长（比如20秒）。
*               本函数会处理执行期间的进度
*               显示
*    形    参:  _FlashAddr
*				_DataBuf : 数据buf
*               _BufLen : 数据长度
*               _Mode : 
*    返 回 值: 0 = ok, 其他表示错误
*********************************************************************************************************
*/
uint16_t PG_SWD_ProgBuf_OB(uint32_t _FlashAddr, uint8_t *_DataBuf, uint32_t _BufLen)
{
    uint8_t err = 0;
    
    err = PG_SWD_ProgBuf(_FlashAddr, _DataBuf, _BufLen, 2);
    
    return err;
}

/*
*********************************************************************************************************
*    函 数 名: PG_SWD_EraseChip
*    功能说明: 开始擦除全片. 对于STM32 OPTION BYTES 会进行解除读保护操作
*    形    参: _FlashAddr 起始地址
*    返 回 值: 0 = ok, 其他表示错误
*********************************************************************************************************
*/
uint16_t PG_SWD_EraseChip(uint32_t _FlashAddr)
{
    uint8_t err = 0;
    error_t err_t;
    
    /* SWD进入debug状态 */
    err_t = target_flash_enter_debug_program();
    if (err_t != ERROR_SUCCESS)
    {
        err = 1;
        goto quit;  
    }
    /* 装载算法代码到目标机内存 */
    LoadAlgoToTarget();
    
    /* 装载算法并执行init函数 */
	err_t = target_flash_init(_FlashAddr, 0, FLM_INIT_ERASE);
    if (err_t != ERROR_SUCCESS)
    {
        err = 1;
        goto quit;
    }    
  
    /*　开始擦除 */
    if (target_flash_erase_chip() != 0)
    {
        PG_PrintText("整片擦除失败");        
        err = 1;
        goto quit;
    }                    

    PG_PrintPercent(100, 0xFFFFFFFF);           
	
quit:
    return err;
}

/*
*********************************************************************************************************
*    函 数 名: PG_SWD_EraseSector
*    功能说明: 开始擦除扇区
*    形    参: _FlashAddr 起始地址
*    返 回 值: 0 = ok, 其他表示错误
*********************************************************************************************************
*/
uint16_t PG_SWD_EraseSector(uint32_t _FlashAddr)
{
    uint8_t err = 0;
    error_t err_t;
    
    /* SWD进入debug状态 */
    err_t = target_flash_enter_debug_program();
    if (err_t != ERROR_SUCCESS)
    {
        err = 1;
        goto quit;  
    }    

    /* 装载算法代码到目标机内存 */
    LoadAlgoToTarget();
    
	err_t = target_flash_init(_FlashAddr, 0, FLM_INIT_ERASE);
    if (err_t == ERROR_RESET)
    {
        PG_PrintText("复位目标MCU失败");
        err = 1;
        goto quit;        

    }
    else if (err_t == ERROR_ALGO_DL)
    {
        PG_PrintText("下载算法失败");        
        err = 1;
        goto quit;        
    }
    else if (err_t == ERROR_INIT)
    {
        PG_PrintText("执行算法失败");        
        err = 1;
        goto quit;        
    } 
    
    /*　开始擦除 */
    if (target_flash_erase_sector(_FlashAddr) != 0)
    {
        PG_PrintText("擦除扇区失败");        
        err = 1;
        goto quit;
    }                          
	
quit:
    return err;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
