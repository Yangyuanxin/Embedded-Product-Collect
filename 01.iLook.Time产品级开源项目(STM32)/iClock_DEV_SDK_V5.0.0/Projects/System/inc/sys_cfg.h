/**
  ******************************************************************************
  * @file    sys_cfg.h
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   系统配置文件
  *          相关函数在ilook_file.c & ilook_file.h
  ******************************************************************************
**/
#ifndef __SYS_CFG_H
#define __SYS_CFG_H

bool load_sys_config(void);
int check_itarcker_cfg(void);

extern SYS_CFG_TYPE iLookCfg;

#endif

/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/



