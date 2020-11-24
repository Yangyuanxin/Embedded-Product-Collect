#ifndef BMS_PT_H_
#define BMS_PT_H_


#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐

typedef struct
{
	u16 limt_v_single;  //蓄电池模块最高允许充电电压  单位0.01V  范围 0~600V
	u16 limt_i;  //最高充电电流  单位0.1A  -800A偏移量    范围  -800~0V
	u8  max_Ah;  //最大允许充电容量  单位0.1Ah   范围 0~1000Ah ??????????????  可能有错误
	u16 limt_v;   //最高允许充电总电压  单位0.1V  范围 0~800V
	u8  limt_t;   //最高允许温度  单位度    -50度偏移   范围  -50度 ~ +200度
	u16 Ah;  //整车蓄电池估计剩余容量  单位1Ah  范围 0 ~ 1000Ah
	u16 v;  //整车电池总电压  单位0.1V  范围 0~780V
	u8  SOH;  //1%，0%偏移量； 数据范围：0~100%
}PT_PGN1536_Def;

typedef struct
{
	u16 v;  //电压测量值  单位0.1V  范围 0~750V
	u16 i;  //电流测量值  单位0.1A  -800A偏移量    范围  -800~0V
	u8  min_t;  //电池组最低温度 单位1度  -50度偏移
	u8  max_t;  //电池组最高温度 单位1度  -50度偏移
	u8  SOC;  //当前电荷状态  单位1%  范围 0% ~ 100%
	u16 remain_t;  //估算剩余充电时间  单位分钟  范围0~250min
}PT_PGN4352_Def;

#pragma pack(pop) //恢复对齐状态

#define PT_PGN_12288  0x30
#define PT_PGN_12544  0x31


#endif
