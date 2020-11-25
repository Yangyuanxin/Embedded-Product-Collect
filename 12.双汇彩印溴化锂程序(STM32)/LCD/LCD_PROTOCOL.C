/*
编写说明：
灯显数据帧定时召唤
时间数据帧定时召唤，时间帧还受菜单交互中的时间修改菜单触发召唤
其它帧由各自的交互菜单触发
*/

#define _LCD_PROTOCOL_C

#include "head.h"

#define PRO_CLR_FRAME_UNACK_FLAG()	(frame_status.status &= (~FRAME_UNACK_FLAG))
//#define 
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------
STRU_SEQ_CIRCLE_TIME seq_circle_time;		//循环召唤帧的循环时间计时
//
typedef struct 
{
	#define FRAME_UNACK_FLAG		0x80
	#define FRAME_OVERTIME_FLAG 	0x40		
	UINT8 status;	//D7为未确认标志，D6为超时标志，D3-D0 重传次数
	//
	UINT8 frame_type;	//发送的帧类型
	UINT16 frame_overtime_time;
}STRU_FRAME_STATUS;

STRU_FRAME_STATUS frame_status;			//帧交互状态，如果有未确认的帧，不到超时不能发送下一帧
//


//











#ifdef DEBUG_FIX_DEFAULT_PARA
const UINT8 fix_data[]=
{
	0x01,0x26,
	0x48,0x02,
	0x01,0x26,
	0x48,0x02,
	0x01,0x26,
	0x48,0x02,
	0x01,0x26,
	0x48,0x02,
	0x01,0x26,
	0x48,0x02,
	0x01,0x26,
	0x48,0x02,
	0x01,0x26,
	0x48,0x02,
	0x01,0x00,
	0x00,0x00
};
#endif
//
void InitLcdProtocol(void)
{
	//初始化规约运行变量
	seq_status = STA_SEQ_DEVICE_TYPE_FLAG			//查询设备类型
					| STA_SEQ_DISP_FLAG;
	//
	//local_addr = 0;
	valid_para_flag = 0;			//清所有可用标志
	led_disp_data = 0xffff;
	new_soe_num = 0;

	share_buf_use_type = 0;

	bkg_delay = 0;					//常亮
	yc_disp_mode = 0;				//显示二次值
	//

}



void TaskLcdProtocol(void *pdata)
{
	

}




//----------------------------------------------------------------
//液晶任务的数据
/*
设置液晶通讯任务的交互状态
该函数可以被其它应用程序调用
*/
void SetProRunStatus(UINT32 flag)
{
	seq_status |= flag;	
}

/*
清除液晶通讯任务的交互状态
该函数可以被其它应用程序调用
*/
void ResetProRunStatus(UINT32 flag)
{
	seq_status &= (~flag);
}


UINT32 GetProRunStatus(void)
{
	return seq_status;
}

/*
得到规约参数的召唤状态
*/
UINT32 GetParaStatus(void)
{
	return valid_para_flag;
}

void ResetParaStatus(UINT32 flag)
{
	valid_para_flag &= (~flag);
}
void SetParaStatus(UINT32 flag)
{
	valid_para_flag |= flag;
}





//-------------------------------------------------------------------------------------------------------------




//-------------------------------------------------------------------------------------------------------------
