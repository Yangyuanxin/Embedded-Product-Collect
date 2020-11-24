#ifndef RECORDER_H_
#define RECORDER_H_

#include "stm32f10x.h"

#pragma pack(push) //保存对齐状态
#pragma pack(1)//设定为1字节对齐
typedef struct
{
	unsigned short year;
	unsigned char  mon;
	unsigned char  day;
	unsigned char  hour;
	unsigned char  min;
	unsigned char  sec;
}record_time_def;

//充电历史记录
typedef struct
{
	record_time_def time;  //开始时间
	u32 sum;  //金额 0.01元
	u32 kwh;  //度数 0.01度
	u32 sec;  //时长  秒
	u32 card_id;  //IC卡ID
	//u8  id;  //充电枪ID
	u8  status;  //状态   1:正常  2:未结算
}record_def;

#pragma pack(pop) //恢复对齐状态


int recd_write(u16 addr, record_def* recd);
int recd_read(u16 addr, record_def* recd);
void recd_init(void);
u16 recd_get_cnt(void);
void recd_clr(void);
u16 recd_add(void);
int recd_read_new(u16 id, record_def* recd);


#endif /* ETH_H_ */
