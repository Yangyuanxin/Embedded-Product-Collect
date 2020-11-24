#ifndef MEASURE_H_
#define MEASURE_H_

typedef struct
{
	s32 v;  //电压测量值  单位0.001V
	u32 i;  //电流测量值  单位0.001A
	u32 ad_v;
	u32 ad_i;
}meas_def;

extern meas_def meas_info;

void task_meas(void);

#endif
