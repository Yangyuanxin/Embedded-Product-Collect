#ifndef _FORM_H
#define _FORM_H


#ifndef _FORM_C

#define EXT_FORM_C extern
#else
#define EXT_FORM_C
#endif

//extern k_rq_t mainmenu;

extern const INT8 *dlist_jz_qy[];
extern const INT8 *dlist_jz_qy_f[];
extern const INT8 *dlist_c_d[];
extern const INT8 *dlist_c_d_f[];
extern const INT8 *dlist_tc_tr[];
extern const INT8 *dlist_tc_tr_f[];
extern const INT8 *dlist_tgj_ttz[];
extern const INT8 *dlist_tgj_ttz_f[];
extern const INT8 *dlist_txh_ttz_f[];
extern const INT8 *dlist_txh_ttz[];
extern const INT8 *dlist_zb_hb[];
extern const INT8 *dlist_zb_hb_f[];
extern const INT8 *dlist_jb_wcxb[];
extern const INT8 *dlist_jb_wcxb_f[];
extern const INT8 *dlist_jx1_jx2[];
extern const INT8 *dlist_jx1_jx2_f[];
extern const INT8 *dlist_rcz_ycz[];
//
extern const INT8 *list_comm_type[];
extern const INT8 *list_comm_baud[];
extern const INT8 *list_comm_databit[];
extern const INT8 *list_comm_parity[];
extern const INT8 *list_comm_protocol[];


extern k_rq_t rq_fixed_manage_menu;
extern k_menu_t fixed_manage_seq;

//
extern k_rq_t rq_sys_manage_menu;
EXT_FORM_C const void *comm_manage_plist[];
EXT_FORM_C k_rq_t rq_comm_manage_menu;

EXT_FORM_C k_menu_t comm_manage_jiankong;
EXT_FORM_C k_menu_t comm_manage_gprs;
EXT_FORM_C k_menu_t comm_manage_sensor;
//extern k_menu_t sys_manage_communication;
extern k_menu_t sys_manage_addr;
extern k_menu_t sys_manage_password;
extern k_menu_t sys_manage_rtc;
extern k_menu_t sys_manage_disp;

//
extern k_rq_t rq_para_manage_menu;
extern k_menu_t ki_set_seq;
extern k_menu_t adj_coef_edit;
extern k_menu_t adj_coef_seq;
extern k_menu_t adj_coef_load;

EXT_FORM_C k_menu_t jiayao_para_seq;
EXT_FORM_C k_menu_t jiayao_para_edit;



extern k_rq_t rq_ctrl_manage_menu;
EXT_FORM_C k_rq_t rq_fixed_manage_load;
EXT_FORM_C const void *ctrl_manage_plist[];
EXT_FORM_C k_menu_t ctrl_manage_opt;
EXT_FORM_C k_menu_t ctrl_manage_debug;

EXT_FORM_C k_rq_t rq_yc_menu;
EXT_FORM_C k_rq_t rq_xishu_manage_load;
EXT_FORM_C k_rq_t rq_work_manage_load;
EXT_FORM_C k_menu_t para_work_menu;
EXT_FORM_C k_menu_t para_xishu_menu;
EXT_FORM_C k_rq_t rq_para_xishu_manage_menu;
EXT_FORM_C k_rq_t rq_para_work_manage_menu;

EXT_FORM_C k_rq_t rq_para_jiayao_manage_menu;
EXT_FORM_C k_rq_t rq_para_paiwu_manage_menu;
EXT_FORM_C const void *para_jiayao_manage_plist[];
EXT_FORM_C const void *para_work_manage_plist[];


#endif



