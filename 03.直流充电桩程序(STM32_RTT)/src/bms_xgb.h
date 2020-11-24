#ifndef BMS_XGB_H_
#define BMS_XGB_H_

#include"stm32f10x.h"

#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐
//多包传输协议结构
typedef struct
{
	u8  ctl_byte;   //控制字节
	u16 msg_lenth;  //消息总长度  字节
	u8  pkg_cnt;  //数据包个数
	u8  not_use;  //保留  固定 0xff
	u8  pgn[3];  //PGN
}TP_CM_RTS_Def;
#pragma pack(pop) //恢复对齐状态


//用于接收BMS多包管理
typedef struct
{
	u16 len;
	u16 received_len;  //实际收到的字节
	u8  pkg_cnt;
	u8  pgn[3];
	u8  *buf;
}Mult_pkg_info_Def;



#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐
typedef struct
{
	u8 version[3];

	//01铅酸
	//02镍氢
	//03磷酸铁锂
	//04锰酸锂
	//05钴酸锂
	//06三元材料
	//07聚合物锂离子
	//08钛酸锂
	//FF其它
	u8 battery_type;
	u16 Ah;  //容量  单位0.1Ah
	u16 V;   //额定电压  单位0.1V
	u8 manufacturer[4]; //生产商名称  ASCII
	u32 No;  //电池组序号
	u8 year;  //生产日期年份  1985偏移
	u8 month;  //生产日期月
	u8 day;  //生产日期日
	u8 cycle_cnt[3];  //充电次数
	u8 property;  //产权标识  0租赁   1车自有
	u8 not_use;  //保留
	u8 VIN[17];  //车辆标识
}PGN512_Def;

typedef struct
{
	u16 limt_v_single;  //单体动力蓄电池最高允许充电电压  单位0.01V  范围 0~24V
	u16 limt_i;  //最高充电电流  单位0.1A  -400A偏移量    范围  -400~0V
	u16 max_kWh;  //标称总能量  单位0.1kWh   范围 0~1000kWh
	u16 limt_v;   //最高允许充电总电压  单位0.1V  范围 0~750V
	u8  limt_t;   //最高允许温度  单位度    -50度偏移   范围  -50度 ~ +200度
	u16 SOC;  //整车电池电荷状态  单位0.1%  范围 0% ~ 100%
	u16 v;  //整车电池总电压  单位0.1V  范围 0~750V
}PGN1536_Def;

typedef struct
{
	u16 v;  //电压需求  单位0.1V  范围 0~750V
	u16 i;  //电流需求  单位0.1A  -400A偏移量    范围  -400~0A
	u8  cc_cv;  //充电模式   0x01:恒压   0x02恒流
}PGN4096_Def;

typedef struct
{
	u16 v;  //电压测量值  单位0.1V  范围 0~750V
	u16 i;  //电流测量值  单位0.1A  -400A偏移量    范围  -400~0V
	u16 max_v_single;  //最高单体动力电池电压及其组号 1~12位电压 单位0.01V 范围0~24V  13~16位 电压所在组号 1偏移量  范围 1~16
	u8  SOC;  //当前电荷状态  单位1%  范围 0% ~ 100%
	u16 remain_t;  //估算剩余充电时间  单位分钟  范围0~600min
}PGN4352_Def;

typedef struct
{
	u8 SOC;  //单位1%
	u16 min_v_single;  //最低单体电压
	u16 max_v_single;  //最高单体电压
	u8  min_temp_single;  //最低电池组温度
	u8  max_temp_single;  //最高电池组温度
}PGN7168_Def;



#pragma pack(pop) //恢复对齐状态


#endif
