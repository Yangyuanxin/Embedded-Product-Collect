#ifndef EM_H_
#define EM_H_

#include "stm32f10x.h"

typedef struct
{
	u32 kwh;    //0.01度  本次充电消耗电量  调用em_calc 会更新此数据
	u32 sum;    //0.01元  本次充电费用  调用 em_calc 会更新此数据
	u32 kwh_cnt;  //电表当前读数
	u8 err_cnt;   //出错次数
}em_info_def;

extern int em_read_kwh(u32 *kwh);
extern int em_link_isok(void);
int em_calc(void);
int em_calc_init(void);

extern em_info_def em_info;


#endif
