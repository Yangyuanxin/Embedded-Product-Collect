#ifndef INC__FLASH_INFO_H__
#define INC__FLASH_INFO_H__
#pragma once

#include <stdio.h>
#include "ChipDefs.h"

/* Get Flash Info by PID */
typedef struct
{
	unsigned int uProgramMemorySize;
	unsigned int uDataFlashSize;
	unsigned int uRAMSize;
	unsigned int uDataFlashStartAddress;
	unsigned int uISPFlashSize;
	unsigned int uPID;
} FLASH_PID_INFO_BASE_T;

void *GetInfo(unsigned int uPID,
			  FLASH_PID_INFO_BASE_T *pInfo);

bool GetInfo(unsigned int uPID,
			 unsigned int uConfig0,
			 unsigned int uConfig1,
			 unsigned int *puLDROM_Addr,
			 unsigned int *puAPROM_Addr,
			 unsigned int *puNVM_Addr,
			 unsigned int auSPROM_Addr[3],
			 unsigned int *puKPROM_Addr,
			 unsigned int *puLDROM_Size,
			 unsigned int *puAPROM_Size,
			 unsigned int *puNVM_Size,
			 unsigned int auSPROM_Size[3],
			 unsigned int *puKPROM_Size);

/* Get Flash Info by DID */
typedef struct
{
	unsigned int uProgramMemorySize;
	unsigned int uDataFlashSize;
	unsigned int uRAMSize;
	unsigned int uDataFlashStartAddress;
	unsigned int uDID;
} FLASH_DID_INFO_BASE_T;

/* 8051 1T Series */
void *GetInfo_N76E1T(unsigned int uDID,
					 FLASH_DID_INFO_BASE_T *pInfo);

bool GetInfo_N76E1T(unsigned int uDID,
					unsigned int uConfig0,
					unsigned int uConfig1,
					unsigned int *puLDROM_Addr,
					unsigned int *puAPROM_Addr,
					unsigned int *puNVM_Addr,
					unsigned int *puLDROM_Size,
					unsigned int *puAPROM_Size,
					unsigned int *puNVM_Size);

extern FLASH_PID_INFO_BASE_T gsPidInfo;
extern FLASH_DID_INFO_BASE_T gsDidInfo;

#endif

