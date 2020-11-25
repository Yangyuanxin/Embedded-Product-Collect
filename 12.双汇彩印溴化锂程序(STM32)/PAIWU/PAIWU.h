#ifndef _PAIWU_H
#define _PAIWU_H

#ifdef _PAIWU_C
#define EXT_PAIWU
#else
#define EXT_PAIWU	extern
#endif	  


//定义排污阀运行 的各种状态
#define STATE_PAIWU_OPEN_EN			0	//允许打开排污阀
#define STATE_PAIWU_OPEN_ING		1	//正在打开排污阀
#define STATE_PAIWU_OPENED			2	//排污阀已经打开
#define STATE_PAIWU_CLOSE_EN		3	//排污阀允许关闭
#define STATE_PAIWU_CLOSE_ING		4	//排污阀正在关闭
#define STATE_PAIWU_CLOSED			5	//排污阀已经关闭
#define STATE_PAIWU_DELAY			6	//延时自锁到下次排污



EXT_PAIWU u32 paiwu_num;
EXT_PAIWU u32 process_delay;



#define TASK_PAIWU_STK_SIZE	512
EXT_PAIWU OS_STK TaskPAIWUStk[TASK_PAIWU_STK_SIZE];

EXT_PAIWU void TaskPAIWU(void *pdata);




#endif 




