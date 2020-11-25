/*
参数管理菜单项牵涉的所有子菜单及函数
*/

#ifndef _FORM_PARA_MANAGE_H
#define _FORM_PARA_MANAGE_H

#ifdef _FORM_PARA_MANAGE_C
#define EXT_FORM_PARA_MANAGE
#else
#define EXT_FORM_PARA_MANAGE extern
#endif


//--------------------------------------------------------


EXT_FORM_FIX_MANAGE void R_Para_XiShu_Manage_Execute(struct _rq_t *item);
EXT_FORM_FIX_MANAGE void XiShu_load_exe(struct _rq_t *item);

EXT_FORM_FIX_MANAGE void R_Para_JiaYao_Manage_Execute(struct _rq_t *item);
EXT_FORM_FIX_MANAGE void Work_load_exe(struct _rq_t *item);


//--------------------------------------------------------





#endif