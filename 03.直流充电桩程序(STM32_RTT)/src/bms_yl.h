#ifndef BMS_YL_H_
#define BMS_YL_H_

#include "stm32f10x.h"

//银隆协议

#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐
//总电压报文
typedef struct
{
	u16 i;  //单位1A  偏移量 32767A
	u16 v;  //单位1V  偏移量  0V
	u8  SOC;  // 1%  电量百分比
}bms_yl_bbab_def;

//充电停止报文
typedef struct
{
	u8 onoff;  //关机命令： 等于1时，关机。默认为0x00.
}bms_yl_a8a8_def;

#pragma pack(pop) //恢复对齐状态

typedef struct
{
	//单体电池电压报文
	//第一维 电池箱
	//第二维 一条报文5条CAN消息
	//第三维 一条CAN消息8字节
	u8 msg_01A0[3][5][8];


	//电池信息 车辆VIN
	u8 msg_ACAF[4][8];
}bms_yl_info_def;

#endif
