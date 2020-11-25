/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gm_wdt.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *  Copyright Statement:
 *  --------------------
 * www.goome.net
 *
*****************************************************************************/
 

#ifndef __GM_WDT_H__
#define __GM_WDT_H__


extern void GM_SoftwareWdtCounterEnable(u8 enable);
extern u8 GM_GetSoftwareWdtCounter(void);
extern void GM_SetSoftwareWdtCounter(u8 counter);
extern void GM_SetSwLogicCounterEnable(u8 enable);
extern void GM_SetSwLogicCounter(u32 counter);
extern void GM_SetSwLogicCounterMax(u32 counter);
extern u32 GM_GetSwLogicCounter(void);
extern u32 GM_GetSwLogicCounterMax(void);

//系统内部默认打开了WDT

#endif

