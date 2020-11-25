/*
传动操作菜单项牵涉的所有子菜单及函数
*/

#ifndef _FORM_CTRL_MANAGE_H
#define _FORM_CTRL_MANAGE_H

#ifdef _FORM_CTRL_MANAGE_C
#define EXT_FORM_CTRL_MANAGE
#else
#define EXT_FORM_CTRL_MANAGE extern
#endif

EXT_FORM_CTRL_MANAGE UINT8 ctrl_code;		//遥控号
EXT_FORM_CTRL_MANAGE UINT16 ctrl_opt;		//控制操作类型,初始化时应与被控对象的当前状态相反
//--------------------------------------------------------

EXT_FORM_CTRL_MANAGE void R_Ctrl_Manage_Execute(struct _rq_t *item);
//--------------------------------------------------------


#endif