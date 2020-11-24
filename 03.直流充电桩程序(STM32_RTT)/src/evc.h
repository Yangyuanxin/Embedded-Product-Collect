#ifndef EVC_H_
#define EVC_H_


#include"stm32f10x.h"



#define MAX_MODULE_CNT 8  //充电机内最大模块数


//整个充电机的全局配置参数
typedef struct
{
	u8  onoff;   //0xff:on  0x00:off
	u16 set_v;   //设置电压 (0.1v)
	u16 set_c;	 //设置电流 (0.01A)
}config_info_def;

//整个充电机的获取参数
typedef struct
{
	u16 v;  //单位0.1V
	u16 i;  //单位0.01A
}read_info_def;


#define READ_SAMP_CNT 3  //电压电流滑动滤波取样点数   必须大于1

//单个模块参数
typedef struct
{
	u8 id;  //ID号
	u8 is_err;  //标志是否有故障  1:有故障   0:正常
	u8 link_off;  //当检测到离线时加1 当连续3次则判断故障
	u8 cfg_end;  //设置通讯超时是否完成
	u8 ocp_reset_cnt;  //过流复位次数
	u32 ocp_reset_t;   //复位时间
	u32 status;  //读到的状态
	u16 v;  //读取的电压 单位0.1V
	u16 i;  //读取的电流 单位0.01A
	u16 v_temp[READ_SAMP_CNT];  //读取的电压 单位0.1V
	u16 i_temp[READ_SAMP_CNT];  //读取的电流 单位0.01A
}module_info_def;


typedef struct
{
	u8 err_ps;  //1:相序错误   0:正常
	u8 module_cnt;  //充电机内在线障模块数
	u8 module_cnt_no_err;  //充电机内无故障模块数
	u16 max_v;  //最大输出电压   0.1V
	u16 max_i;  //最大输出电流   0.01A
	config_info_def config;
	read_info_def read;
	module_info_def module_info[MAX_MODULE_CNT];
}evc_info_def;



void evc_out_onoff(u8 onoff);
void evc_set_v_c(unsigned short v, unsigned short c);

extern evc_info_def evc_info;
void task_evc(void);


#endif /* CHGMODULE_H_ */
