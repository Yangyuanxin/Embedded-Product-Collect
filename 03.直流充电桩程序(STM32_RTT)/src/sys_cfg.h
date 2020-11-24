#ifndef SYS_CFG_H_
#define SYS_CFG_H_

#include "stm32f10x.h"
//内部flash是以16位读写的  变量以 32 定义
typedef struct
{
	u16 valid;  //标志是否有效  == 0xAAAA 有效 用于第一次上电

	u32 sys_pswd;  //系统密码
	u32 id;        //充电桩ID号
	u32 e_price;   //电价  单位 分  100表示1元
	u32 ic_down_pswd;  //标志是否下载过读卡器密钥  0xAAAAAAAA为下载过

	u8 lip1;  //本机IP
	u8 lip2;
	u8 lip3;
	u8 lip4;

	u8 drip1;  //网关IP
	u8 drip2;
	u8 drip3;
	u8 drip4;

	u8 netmask1;  //子网掩码
	u8 netmask2;
	u8 netmask3;
	u8 netmask4;

	u8 rip1;  //远程IP
	u8 rip2;
	u8 rip3;
	u8 rip4;

	u16 rport;  //远程端口

	u16 gprs;  //0:移动   1:联通
}sys_cfg_def;

typedef struct
{
	u16 valid;  //标志是否有效  == 0xAAAA 有效 用于第一次上电

	u32 v_offset;  //电压AD偏移量   12bit
	float v_gain;    //电压增益  扩大1000倍
	u32 i_offset;  //电流AD偏移量   12bit
	float i_gain;    //电流增益  扩大1000倍
}meas_adj_def;

int sys_cfg_read(void);
int sys_cfg_save(void);
int meas_adj_read(void);
int meas_adj_save(void);

extern sys_cfg_def sys_cfg;
extern meas_adj_def meas_adj;
extern const sys_cfg_def sys_cfg_default;
extern const meas_adj_def meas_adj_default;

#endif /* CONFIG_H_ */
