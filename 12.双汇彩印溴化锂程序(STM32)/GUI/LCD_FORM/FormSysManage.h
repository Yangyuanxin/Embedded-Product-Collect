/*
系统管理菜单项牵涉的所有子菜单及函数
*/

#ifndef _FORM_SYS_MANAGE_H
#define _FORM_SYS_MANAGE_H

#ifdef _FORM_SYS_MANAGE_C
#define EXT_FORM_SYS_MANAGE
#else
#define EXT_FORM_SYS_MANAGE extern
#endif


//--------------------------------------------------------



EXT_FORM_SYS_MANAGE void R_Sys_Manage_Execute(struct _rq_t *item);
EXT_FORM_SYS_MANAGE void R_Comm_Manage_Execute(struct _rq_t *item);



//--------------------------------------------------------





#endif