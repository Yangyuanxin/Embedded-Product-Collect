#define _FORM_C

#include "head.h"

extern k_rq_t mainmenu;

extern const void *main_menu_plst[];
extern const void *realtimedata_plist[];
extern const void *yxdata_plist[];
extern const void *sys_manage_plist[];
extern const void *fixed_manage_plist[];
extern const void *para_xishu_manage_plist[];
extern const void *para_manage_plist[];
extern const void *soe_report_plist[];
//
extern k_rq_t realtime_menu;

extern k_rq_t rq_yx_menu;
extern k_rq_t rq_yx_menu_easy;
extern k_rq_t rq_yx_menu_all;
//
extern k_rq_t rq_sys_manage_menu;
//
extern k_rq_t rq_fixed_manage_menu;
//
extern k_rq_t rq_para_manage_menu;
//
extern k_rq_t rq_soe_report_menu;


//---------------------------主菜单---------------------------------
//主菜单容器
k_rq_t mainmenu =
{
  0,			//基行
  0,			//显示的当前行
  0,			//焦点值
  0,0,			//容器起始坐标
  0,			//本屏显示的个数
  16,8,			//容器宽高 
  6,			//控件个数
  0,			//操作的容器
  0,			//确定后操作的容器
  &mainmenu,			//取消后操作的容器
  0,			//当前控件
  R_SetFocus,	//向容器上的东东发送焦点
  R_Initsize,	//初始化
  R_Execute,
  (void**)main_menu_plst,
};

//主菜单的菜单项
//菜单项：实时数据
k_menu_t mmnu1 =
{
  K_MENU,
  0,
  ">实时数据",
  1,0,
  0,0,
  &realtime_menu,//  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//菜单项：系统管理
k_menu_t mmnu2 =
{
  K_MENU,
  1,//4,
  ">系统管理",
  1,2,
  0,0,
  &rq_sys_manage_menu,//&s1menu,//&ser_cnf,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//菜单项：定值管理
k_menu_t mmnu3 =
{
  K_MENU,
  2,
  ">定值管理",
  1,4,
  0,0,
  &rq_fixed_manage_menu,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//菜单项：参数管理
k_menu_t mmnu4 =
{
  K_MENU,
  3,
  ">参数管理",
  1,6,
  0,0,
  &rq_para_manage_menu,             
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//
//菜单项：事件报告
/**
k_menu_t mmnu5 =
{
  K_MENU,
  4,
  ">事件报告",
  1,8,
  0,0,
  &rq_soe_report_menu,  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
*******/
//菜单项：传动操作
k_menu_t mmnu6 =
{
  K_MENU,
  4,//2,
  ">传动操作",
  1,8,
  0,0,
  &rq_ctrl_manage_menu,
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//菜单项：关于装置
k_menu_t mmnu7 =
{
  K_MENU,
  5,//2,
  ">关于装置",
  1,10,
  0,0,
  0,
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//主菜单对象列表
const void *main_menu_plst[] =
{
  	&mmnu1,  
  	&mmnu2,  
  	&mmnu3,  
	&mmnu4,   
	&mmnu6,
  	&mmnu7,
	
  	0
};
//主菜单项相关结束
//---------------------------实时数据---------------------------------
//子菜单1容器<实时数据>
k_rq_t realtime_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  2,					//控件个数
  0,
  0,					//确定后进入的子容器
  &mainmenu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Execute,
  (void**)realtimedata_plist//slst1,
};

//实时数据菜单项
k_menu_t realtime_yc =
{
  K_MENU,
  0,
  ">遥测量",
  0,0,
  0,0,
  &rq_yc_menu,//&realtime_menu,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};

k_menu_t realtime_yx =
{
  K_MENU,
  1,
  ">信号量",
  0,2,
  0,0,
  &rq_yx_menu,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};

//<实时数据>菜单对象列表
const void *realtimedata_plist[] =
{
	&realtime_yc,  
	&realtime_yx,  

  	0
};
//实时数据子菜单->YC量容器
k_rq_t rq_yc_menu = 
{
	0,
  	0,
  	0,
  	0,0,
  	0,
  	16,8,
  	0,					//控件个数
  	0,
  	0,					//确定后进入的子容器
  	&realtime_menu,			//取消后进入的子容器
  	0,
  	R_SetFocus,
  	0,//R_Initsize,
  	DispRealTimeYc,
  	0//(void**)realtimedata_plist//slst1,
};

//实时数据子菜单->YX量容器
k_rq_t rq_yx_menu = 
{
	0,
  	0,
  	0,
  	0,0,
  	0,
  	16,8,
  	2,					//控件个数
  	0,
  	0,					//确定后进入的子容器
  	&realtime_menu,			//取消后进入的子容器
  	0,
  	R_SetFocus,
  	R_Initsize,
  	R_Execute,
  	(void**)yxdata_plist
};

//YX数据菜单项
k_menu_t realtime_yx_easy =
{
  K_MENU,
  0,
  ">简明显示",
  0,0,
  0,0,
  &rq_yx_menu_easy,//&yx_menu_easy_disp,//&realtime_menu,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};

k_menu_t realtime_yx_all =
{
  K_MENU,
  1,
  ">详细显示",
  0,2,
  0,0,
  &rq_yx_menu_all,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};

//<遥信量>菜单对象列表
const void *yxdata_plist[] =
{
	&realtime_yx_easy,  
	&realtime_yx_all,  

  	0
};

//简明显示子菜单容器
k_rq_t rq_yx_menu_easy = 
{
	0,
  	0,
  	0,
  	0,0,
  	0,
  	16,8,
  	0,					//控件个数
  	0,
  	0,					//确定后进入的子容器
  	&rq_yx_menu,			//取消后进入的子容器
  	0,
  	R_SetFocus,
  	0,//R_Initsize,
  	DispRealTimeYxEasy,
  	0//(void**)realtimedata_plist//slst1,
};

//详细显示子菜单容器
k_rq_t rq_yx_menu_all = 
{
	0,
  	0,
  	0,
  	0,0,
  	0,
  	16,8,
  	0,					//控件个数
  	0,
  	0,					//确定后进入的子容器
  	&rq_yx_menu,			//取消后进入的子容器
  	0,
  	R_SetFocus,
  	0,//R_Initsize,
  	DispRealTimeYxAll,
  	0//(void**)realtimedata_plist//slst1,
};

//---------------------------实时数据结束-----------------------------

//---------------------------系统管理开始-----------------------------
//子菜单2容器<系统管理>
k_rq_t rq_sys_manage_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  3,					//控件个数
  0,
  0,					//确定后进入的子容器
  &mainmenu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Sys_Manage_Execute,  
  (void**)sys_manage_plist//slst1,
};
//子菜单2容器<系统管理>
k_rq_t rq_comm_manage_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  3,					//控件个数
  0,
  0,					//确定后进入的子容器
  &rq_sys_manage_menu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Comm_Manage_Execute,  
  (void**)comm_manage_plist//slst1,
};

//地址设置菜单
k_menu_t sys_manage_addr =
{
  K_MENU,
  0,
  ">地址设置",
  0,0,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//通讯设置菜单
k_menu_t sys_manage_communication =
{
  K_MENU,
  1,
  ">通讯设置",
  0,2,
  0,0,
  &rq_comm_manage_menu,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//密码设置菜单
k_menu_t sys_manage_password =
{
  K_MENU,
  2,
  ">密码设置",
  0,4,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//时钟设置菜单
k_menu_t sys_manage_rtc =
{
  K_MENU,
  3,
  ">时钟设置",
  8,6,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//显示设置菜单
k_menu_t sys_manage_disp =
{
  K_MENU,
  4,
  ">显示设置",
  8,8,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};

//<系统管理>菜单对象列表
const void *sys_manage_plist[] =
{	  
	&sys_manage_addr, 
    &sys_manage_communication,
	&sys_manage_password,
//	&sys_manage_rtc,
//	&sys_manage_disp,
  	0
};
//<系统管理>菜单对象列表
const void *comm_manage_plist[] =
{	  
	&comm_manage_jiankong, 
    &comm_manage_gprs,
	&comm_manage_sensor,
//	&sys_manage_rtc,
//	&sys_manage_disp,
  	0
};
k_menu_t comm_manage_jiankong =
{
  K_MENU,
  0,
  ">监控设置",
  0,0,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
k_menu_t comm_manage_gprs =
{
  K_MENU,
  1,
  ">gprs设置",
  0,2,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
k_menu_t comm_manage_sensor =
{
  K_MENU,
  2,
  ">传感器设置",
  0,4,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};




//---------------------------系统管理结束-----------------------------

//---------------------------定值管理开始-----------------------------
//子菜单3容器<定值管理>
k_rq_t rq_fixed_manage_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  3,					//控件个数
  0,                    //操作的容器
  0,					//确定后进入的子容器
  &mainmenu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,//R_Initsize,	070702.GUOHUI.
  R_Fixed_Manage_Execute,
  (void**)fixed_manage_plist//定值管理列表,
};
//定值固化容器
k_rq_t rq_fixed_manage_load = 
{
	0,
  	0,
  	0,
  	0,0,
  	0,
  	16,8,
  	0,					//控件个数
  	0,
  	0,					//确定后进入的子容器
  	&rq_fixed_manage_menu,			//取消后进入的子容器
  	0,
  	R_SetFocus,
  	0,//R_Initsize,
  	Fixed_load_exe,
  	0//(void**)realtimedata_plist//slst1,
};

//定值浏览菜单
k_menu_t fixed_manage_seq =
{
  K_MENU,
  0,
  ">定值浏览",
  4,0,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  需要改动
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//定值整定菜单
k_menu_t fixed_manage_edit =
{
  K_MENU,
  1,
  ">定值整定",
  4,2,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //与定值浏览指向了同一个容器，只能靠其它指明操作方式  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//定值固化菜单
k_menu_t fixed_manage_load =
{
  K_MENU,
  2,
  ">定值固化",
  4,4,
  0,0,
  &rq_fixed_manage_load,//&,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//<定值管理>菜单对象列表
const void *fixed_manage_plist[] =
{
	&fixed_manage_seq,  
	&fixed_manage_edit,  
	&fixed_manage_load,	

  	0
};

//---------------------------定值管理结束-----------------------------

//---------------------------参数管理开始-----------------------------

//子菜单4容器<参数管理>
k_rq_t rq_para_manage_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  2,					//控件个数
  0,
  0,					//确定后进入的子容器
  &mainmenu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Execute,
  (void**)para_manage_plist//参数管理列表,
};

const void *para_manage_plist[] =
{
  	&para_xishu_menu,  
  	&para_work_menu,  	
  	0
};
//菜单项：参数管理
k_menu_t para_xishu_menu =
{
  K_MENU,
  0,
  ">系数参数",
  1,0,
  0,0,
  &rq_para_xishu_manage_menu,             
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//菜单项：参数管理
k_menu_t para_work_menu =
{
  K_MENU,
  1,
  ">工作参数",
  1,2,
  0,0,
  &rq_para_work_manage_menu,             
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//子菜单4容器<系数参数管理>
k_rq_t rq_para_xishu_manage_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  3,					//控件个数
  0,
  0,					//确定后进入的子容器
  &rq_para_manage_menu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Para_XiShu_Manage_Execute,
  (void**)para_xishu_manage_plist//参数管理列表,
};
//子菜单4容器<工作参数管理>
k_rq_t rq_para_work_manage_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  2,					//控件个数
  0,
  0,					//确定后进入的子容器
  &rq_para_manage_menu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Execute,
  (void**)para_work_manage_plist//参数管理列表,
};

//子菜单容器<加药参数管理>
k_rq_t rq_para_jiayao_manage_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  3,					//控件个数
  0,
  0,					//确定后进入的子容器
  &rq_para_work_manage_menu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Para_JiaYao_Manage_Execute,
  (void**)para_jiayao_manage_plist//参数管理列表,
};
//子菜单4<加药参数管理>
k_menu_t para_jiayao_manage_menu =
{
  K_MENU,
  0,
  ">加药参数",
  1,0,
  0,0,
  &rq_para_jiayao_manage_menu,             
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//子菜单4容器<排污参数管理>
k_menu_t para_paiwu_manage_menu =
{
  K_MENU,
  1,
  ">排污参数",
  1,2,
  0,0,
  0,             
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//定值固化容器
k_rq_t rq_xishu_manage_load = 
{
	0,
  	0,
  	0,
  	0,0,
  	0,
  	16,8,
  	0,					//控件个数
  	0,
  	0,					//确定后进入的子容器
  	&rq_para_xishu_manage_menu,			//取消后进入的子容器
  	0,
  	R_SetFocus,
  	0,//R_Initsize,
  	XiShu_load_exe,
  	0//(void**)realtimedata_plist//slst1,
};
//工作参数固化容器
k_rq_t rq_work_manage_load = 
{
	0,
  	0,
  	0,
  	0,0,
  	0,
  	16,8,
  	0,					//控件个数
  	0,
  	0,					//确定后进入的子容器
  	&rq_para_jiayao_manage_menu,			//取消后进入的子容器
  	0,
  	R_SetFocus,
  	0,//R_Initsize,
  	Work_load_exe,
  	0//(void**)realtimedata_plist//slst1,
};
//开入配置菜单

k_menu_t ki_set_seq =
{
  K_MENU,
  4,
  ">开入配置",
  0,0,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  需要改动
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};

//加药参数菜单

k_menu_t jiayao_para_seq =
{
  K_MENU,
  0,
  ">加药参数浏览",
  0,0,
  0,0,
  &rq_first_rank,//,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
k_menu_t jiayao_para_edit =
{
  K_MENU,
  1,
  ">加药参数修改",
  0,2,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
k_menu_t jiayao_para_load =
{
  K_MENU,
  2,
  ">加药参数固化",
  0,4,
  0,0,
  &rq_work_manage_load,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//<工作参数管理>菜单对象列表
const void *para_work_manage_plist[] =
{
    &para_jiayao_manage_menu, 
	&para_paiwu_manage_menu,  
  	0
};
//<加药参数管理>菜单对象列表
const void *para_jiayao_manage_plist[] =
{
    &jiayao_para_seq, 
	&jiayao_para_edit,  
    &jiayao_para_load, 
  	0
};
k_menu_t adj_coef_seq =
{
  K_MENU,
  0,
  ">系数浏览",
  0,0,
  0,0,
  &rq_first_rank,//,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
k_menu_t adj_coef_edit =
{
  K_MENU,
  1,
  ">系数修改",
  0,2,
  0,0,
  &rq_first_rank,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
k_menu_t adj_coef_load =
{
  K_MENU,
  2,
  ">系数固化",
  0,4,
  0,0,
  &rq_xishu_manage_load,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};

//<系数参数管理>菜单对象列表
const void *para_xishu_manage_plist[] =
{
    &adj_coef_seq, 
	&adj_coef_edit,  
    &adj_coef_load, 
  	0
};

//---------------------------参数管理结束-----------------------------

//---------------------------事件报告开始-----------------------------
//子菜单5容器<事件报告>
k_rq_t rq_soe_report_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  3,					//控件个数
  0,
  0,					//确定后进入的子容器
  &mainmenu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Execute,
  (void**)soe_report_plist//参数管理列表,
};

//事件浏览菜单
k_menu_t soe_browse =
{
  K_MENU,
  0,
  ">事件浏览",
  8,0,
  0,0,
  &rq_yx_menu_all,//&yx_menu_all_disp,  //  需要改动
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};
//修正系数菜单
k_menu_t soe_clr =
{
  K_MENU,
  1,
  ">事件打印",
  8,2,
  0,0,
  &rq_yx_menu_all,//&yx_menu_all_disp,  //  
  M_GetFocus,
  M_LossFocus,
  M_KeyProcess,
  M_qbch,
};

//<参数管理>菜单对象列表
const void *soe_report_plist[] =
{
	&soe_browse,  
	&soe_clr,  	

  	0
};

//---------------------------事件报告结束-----------------------------

//---------------------------传动操作开始-----------------------------
//
k_rq_t rq_ctrl_manage_menu =
{
  0,
  0,
  0,
  0,0,
  0,
  16,8,
  2,					//控件个数
  0,
  0,					//确定后进入的子容器
  &mainmenu,			//取消后进入的子容器
  0,
  R_SetFocus,
  R_Initsize,
  R_Ctrl_Manage_Execute,  
  (void**)ctrl_manage_plist,//slst1,
};
//常规控制菜单
k_menu_t ctrl_manage_opt =
{
	K_MENU,
	0,
	">常规操作",
	0,0,
	0,0,
	&rq_first_rank,//&yx_menu_all_disp,  //  
	M_GetFocus,
	M_LossFocus,
	M_KeyProcess,
	M_qbch,
};
//厂家调试菜单
k_menu_t ctrl_manage_debug =
{
	K_MENU,
	1,
	">厂家调试",
	0,2,
	0,0,
	&rq_first_rank,//&yx_menu_all_disp,  //  
	M_GetFocus,
	M_LossFocus,
	M_KeyProcess,
	M_qbch,
};
//<传动操作>菜单对象列表
const void *ctrl_manage_plist[] =
{
	&ctrl_manage_opt,  
	&ctrl_manage_debug,  		
	0
};
//---------------------------传动操作结束-----------------------------


//---------------------------------------------------------------
//从规约上看，当定值为位定值时，定值单位便作为位显示方式，真值表如下：
/*
SN			0			1
0000		×			√	
0001		退出		投入		

0010		禁止		启用	
0011		投告警		投跳闸

0100		投信号		投跳闸	
0101		主备		互备

0110		基   波		5次偕波
0111		进线 I		进线II
*/
//---------------------------------------------------------------
//逻辑列表值
const INT8 *dlist_jz_qy_f[] =
{
  	"启用",
  	"禁止",
  	0
};
const INT8 *dlist_jz_qy[] =
{
  	"禁止",
  	"启用",
  	0
};

const INT8 *dlist_c_d_f[] = 
{
	"√",
	"×",
	0
};

const INT8 *dlist_c_d[] = 
{
	"×",
	"√",
	0
};
const INT8 *dlist_tc_tr[] = 
{
	"退出",
	"投入",
	0
};
const INT8 *dlist_tc_tr_f[] = 
{
	"投入",
	"退出",
	0
};
const INT8 *dlist_tgj_ttz[] = 
{
	"投告警",
	"投跳闸",
	0
};

const INT8 *dlist_tgj_ttz_f[] = 
{
	"投跳闸",
	"投告警",
	0
};
const INT8 *dlist_txh_ttz_f[] = 
{
	"投跳闸",
	"投信号",
	0
};

const INT8 *dlist_txh_ttz[] = 
{
	"投信号",
	"投跳闸",
	0
};

const INT8 *dlist_zb_hb[] = 
{
	"主备",
	"互备",
	0
};

const INT8 *dlist_zb_hb_f[] = 
{
	"互备",
	"主备",
	0
};
const INT8 *dlist_jb_wcxb[] = 
{
	"基   波",
	"5次偕波",
	0
};
const INT8 *dlist_jb_wcxb_f[] = 
{
	"5次偕波",
	"基   波",
	0
};
const INT8 *dlist_jx1_jx2[] = 
{
	"进线Ⅰ",
	"进线Ⅱ",
	0
};
const INT8 *dlist_jx1_jx2_f[] = 
{
	"进线Ⅱ",
	"进线Ⅰ",
	0
};
const INT8 *dlist_rcz_ycz[] = 
{
	"Ⅱ次值",
	"Ⅰ次值",
	0
};

//--------------------------------------
const INT8 *list_comm_type[] = 
{
	"RS485 ",
	"以太网",
	"guohui",
	"abcdef",
	0
};
const INT8 *list_comm_baud[] = 
{
	"300   ",
	"600   ",	
	"1200  ",
	"2400  ",
	"4800  ",
	"9600  ",	
	"19200 ",
	"38400 ",	
	"57600 ",
	"76800 ",
	"115200",
	"153600",	
	"230400",	
	0
};
const INT8 *list_comm_databit[] = 
{
	"5位",
	"6位",
	"7位",
	"8位",
	0
};
const INT8 *list_comm_parity[] = 
{
	"无校验",
	"奇校验",
	"偶校验",
	"强制 0",
	"强制 1",
	0
};

//
const INT8 *list_comm_protocol[] = 
{
	"IEC-103   ",
	"IEC-104   ",
	"MODBUS-RTU",
	"MODBUS-TCP",
	0
};
//--------------------------------------