#ifndef _GUI_USER_H
#define _GUI_USER_H

#ifndef _GUI_USER_C
#define EXT_GUI_USER extern
#else
#define EXT_GUI_USER
#endif

//-----------------------------------------------------------------------------------
/*
定值的显示方式如下：
因为定值的组数是可变的，故，定值容器也是动态更新的
所以，定值操作的容器是一个RQ变量
此变量在进行定值管理时，由定值参数进行初始化，初始化的东东包括：
所有的变量
//
当按下确认键时，会进入相应的定值组别，所以在进入定值组别时，又要
初始化定值组别对应的容器，其容器内的控件由定值的详细参数进行填充
因不知道其每组内的控件类型及数量，故也要动态更改
*/
#define MENU_PLIST_MAX		20					//动态填入的最多菜单列表项
#define MENU_PLIST_SUB_MAX	10					//动态填入的最多菜单列表项
#define TEXT_OCX_MAX		20
#define LIST_OCX_MAX		10
#define IPAD_OCX_MAX		1
//
EXT_GUI_USER void *share_plist[MENU_PLIST_MAX];		//可放入的菜单列表区
EXT_GUI_USER void *share_plist_sub[MENU_PLIST_MAX];	//可放入的菜单列表区
//动态容哭的定义
EXT_GUI_USER k_rq_t rq_first_rank;					//第一级容器
EXT_GUI_USER k_rq_t rq_second_rank;					//第二级容器

EXT_GUI_USER UINT8  rq_first_rank_enter_flag;		//第一级容器首次进入标志
//第一次进入定值固化函数标志，主要防止多次在此函数循环时多次清屏与刷屏，造成有时字不能显示
EXT_GUI_USER u8     load_fix_first_enter_flag;  
EXT_GUI_USER u8     load_xishu_first_enter_flag;
EXT_GUI_USER u8     load_jiayao_first_enter_flag;

//
EXT_GUI_USER k_menu_t menu_t_ocx[MENU_PLIST_MAX];
EXT_GUI_USER k_text_t text_t_ocx[TEXT_OCX_MAX];
EXT_GUI_USER k_list_t list_t_ocx[LIST_OCX_MAX];
EXT_GUI_USER k_ipad_t ipad_t_ocx[IPAD_OCX_MAX];

//-----------------------------------------------------------------------------------

EXT_GUI_USER void DispRealTimeYc(struct _rq_t *item);
EXT_GUI_USER void DispRealTimeYxEasy(struct _rq_t *item);
EXT_GUI_USER void DispRealTimeYxAll(struct _rq_t *item);


extern const INT8 *fix_group_type[];
extern const INT8 *fix_name_list[];
extern const INT8 *yc_unit[];



EXT_GUI_USER const INT8 *Work_Para_Group_Type[];
EXT_GUI_USER const INT8 *Adj_name_list[];
EXT_GUI_USER const INT8 *adj_group_type[];
EXT_GUI_USER const INT8 *jiayao_name_list[];
EXT_GUI_USER const INT8 *jiankong_group_type[];
EXT_GUI_USER const INT8 *gprs_group_type[];
EXT_GUI_USER const INT8 *sensor_group_type[];

EXT_GUI_USER void usr_SetVal_T(struct _text_t *item, int32s vl);
EXT_GUI_USER void usr_GetVal_T(struct _text_t *item, int32s *vl);
EXT_GUI_USER void usr_SetVal_L(struct _list_t *item, UINT8 vl);
EXT_GUI_USER void usr_GetVal_L(struct _list_t *item, UINT8 *vl);

EXT_GUI_USER void usr_SetVal_L_Gern(struct _list_t *item, UINT8 vl);
EXT_GUI_USER void usr_GetVal_L_Gern(struct _list_t *item, UINT8 *vl);

#endif