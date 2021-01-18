/*
*********************************************************************************************************
*
*    模块名称 : DM9162驱动
*    文件名称 : dm9162.h
*    版    本 : V1.0
*    说    明 : 头文件
*
*    Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DM9162_H
#define __DM9162_H

#ifdef __cplusplus
 extern "C" {
#endif   

#include <stdint.h>
 
#define DM9162_PHY_ADDR    1
 
#define DM9162_BCR        0x00
    #define DM9162_BCR_SOFT_RESET         ((uint16_t)0x8000U)
    #define DM9162_BCR_LOOPBACK           ((uint16_t)0x4000U)
    #define DM9162_BCR_SPEED_SELECT       ((uint16_t)0x2000U)
    #define DM9162_BCR_AUTONEGO_EN        ((uint16_t)0x1000U)
    #define DM9162_BCR_POWER_DOWN         ((uint16_t)0x0800U)
    #define DM9162_BCR_ISOLATE            ((uint16_t)0x0400U)
    #define DM9162_BCR_RESTART_AUTONEGO   ((uint16_t)0x0200U)
    #define DM9162_BCR_DUPLEX_MODE        ((uint16_t)0x0100U) 
    
#define DM9162_BSR                         ((uint16_t)0x01)                 /* PHY status register Offset */ 
    #define DM9162_BSR_100BASE_T4       ((uint16_t)0x8000U)
    #define DM9162_BSR_100BASE_TX_FD    ((uint16_t)0x4000U)
    #define DM9162_BSR_100BASE_TX_HD    ((uint16_t)0x2000U)
    #define DM9162_BSR_10BASE_T_FD      ((uint16_t)0x1000U)
    #define DM9162_BSR_10BASE_T_HD      ((uint16_t)0x0800U)
    #define DM9162_BSR_100BASE_T2_FD    ((uint16_t)0x0400U)
    #define DM9162_BSR_100BASE_T2_HD    ((uint16_t)0x0200U)
    #define DM9162_BSR_EXTENDED_STATUS  ((uint16_t)0x0100U)
    #define DM9162_BSR_AUTONEGO_CPLT    ((uint16_t)0x0020U)
    #define DM9162_BSR_REMOTE_FAULT     ((uint16_t)0x0010U)
    #define DM9162_BSR_AUTONEGO_ABILITY ((uint16_t)0x0008U)
    #define DM9162_BSR_LINK_STATUS      ((uint16_t)0x0004U)
    #define DM9162_BSR_JABBER_DETECT    ((uint16_t)0x0002U)
    #define DM9162_BSR_EXTENDED_CAP     ((uint16_t)0x0001U)

#define DM9162_PHYSCSR                  ((uint16_t)17)
    #define DM9162_PHYSCSR_AUTONEGO_MASK   ((uint16_t)0x000F)
    #define DM9162_PHYSCSR_AUTONEGO_DONE   ((uint16_t)0x0008U)
    
    #define DM9162_PHYSCSR_HCDSPEEDMASK    ((uint16_t)0xF000)
    #define DM9162_PHYSCSR_10BT_HD         ((uint16_t)0x1000)
    #define DM9162_PHYSCSR_10BT_FD         ((uint16_t)0x2000)
    #define DM9162_PHYSCSR_100BTX_HD       ((uint16_t)0x4000)
    #define DM9162_PHYSCSR_100BTX_FD       ((uint16_t)0x8000) 
    

/* DM9162_Status DM9162 Status */
#define  DM9162_STATUS_READ_ERROR            ((int32_t)-5)
#define  DM9162_STATUS_WRITE_ERROR           ((int32_t)-4)
#define  DM9162_STATUS_ADDRESS_ERROR         ((int32_t)-3)
#define  DM9162_STATUS_RESET_TIMEOUT         ((int32_t)-2)
#define  DM9162_STATUS_ERROR                 ((int32_t)-1)
#define  DM9162_STATUS_OK                    ((int32_t) 0)
#define  DM9162_STATUS_LINK_DOWN             ((int32_t) 1)
#define  DM9162_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  DM9162_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  DM9162_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  DM9162_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  DM9162_STATUS_AUTONEGO_NOTDONE      ((int32_t) 6)

typedef int32_t  (*dm9162_Init_Func) (void);
typedef int32_t  (*dm9162_DeInit_Func) (void);
typedef int32_t  (*dm9162_ReadReg_Func)   (uint32_t, uint32_t, uint32_t *);
typedef int32_t  (*dm9162_WriteReg_Func)  (uint32_t, uint32_t, uint32_t);
typedef int32_t  (*dm9162_GetTick_Func)  (void);

typedef struct 
{                   
  dm9162_Init_Func      Init; 
  dm9162_DeInit_Func    DeInit;
  dm9162_WriteReg_Func  WriteReg;
  dm9162_ReadReg_Func   ReadReg; 
  dm9162_GetTick_Func   GetTick;   
} dm9162_IOCtx_t;  

  
typedef struct 
{
  uint32_t            DevAddr;
  uint32_t            Is_Initialized;
  dm9162_IOCtx_t     IO;
  void               *pData;
}dm9162_Object_t;

int32_t DM9162_RegisterBusIO(dm9162_Object_t *pObj, dm9162_IOCtx_t *ioctx);
int32_t DM9162_Init(dm9162_Object_t *pObj);
int32_t DM9162_GetLinkState(dm9162_Object_t *pObj);

#ifdef __cplusplus
}
#endif
#endif /* __DM9162_H */

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
