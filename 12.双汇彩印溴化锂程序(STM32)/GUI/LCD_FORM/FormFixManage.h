/*
定值管理菜单项牵涉的所有子菜单及函数
*/

#ifndef _FORM_FIX_MANAGE_H
#define _FORM_FIX_MANAGE_H

#ifdef _FORM_FIX_MANAGE_C
#define EXT_FORM_FIX_MANAGE
#else
#define EXT_FORM_FIX_MANAGE extern
#endif


//--------------------------------------------------------


EXT_FORM_FIX_MANAGE void R_Fixed_Manage_Execute(struct _rq_t *item);
EXT_FORM_FIX_MANAGE void Fixed_load_exe(struct _rq_t *item);


//--------------------------------------------------------





#endif