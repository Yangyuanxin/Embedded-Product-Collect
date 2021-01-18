/*
*********************************************************************************************************
*
*    模块名称 : 新唐N76E003编程驱动
*    文件名称 : n76e003_flash.h
*
*********************************************************************************************************
*/

#ifndef _N76E003_FLASH_H
#define _N76E003_FLASH_H

void N76E_EnterIAP(void);
void N76E_ExitIAP(void);

uint8_t N76E_DetectIC(uint32_t *_id);

uint8_t N76E_ReadCfg(uint8_t _addr, uint8_t _len, uint8_t *_cfg);
uint8_t N76E_ReadBuf(uint32_t _Addr, uint8_t *_Buf, uint16_t _Len);

void N76E_EraseChip(void);
void N76E_ErasePage(uint32_t _addr);
uint8_t N76E_FLASH_ProgramBuf(uint32_t _FlashAddr, uint8_t *_Buff, uint32_t _Size);
void N76E_ProgramCfg(uint8_t *_cfg);
void N76E_EraseCfg(void);
void N76E_SetResetPin(uint8_t _state);

#endif
