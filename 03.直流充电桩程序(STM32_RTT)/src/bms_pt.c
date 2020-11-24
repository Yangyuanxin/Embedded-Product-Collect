#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "bms.h"
#include "bms_pt.h"
#include "bms_gb.h"
#include "string.h"
#include "evc.h"
#include "io.h"

#if 1
#define BMS_MSG dbg_msg
#else
#define BMS_MSG (void)
#endif

#if 1
#define BMS_ERROR dbg_error
#else
#define BMS_ERROR (void)
#endif

extern u8 stop_charge;


//记录收到的数据包的时间 用于判断超时
struct time_info_s
{
	u32 pgn256;
	u32 pgn512;
	u32 pgn4096;
}bms_pt_time_info;


/*
 * @brief 发送CRM报文(PGN256)
 * @param code 0x00 不能辨识   0x01 能辨识
 */
void bms_pt_send_crm(u8 code)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1801F4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = 0;  //充电机编号
	TxMsg.Data[1] = 1;  //充电头编号

	TxMsg.Data[2] = 0;  //充电机位置  0:户外   1:户内

	TxMsg.Data[3] = code;  //

	//充电机所在区域编码,ASCII码
	TxMsg.Data[4] = 0xff;
	TxMsg.Data[5] = 0xff;
	TxMsg.Data[6] = 0xff;
	TxMsg.Data[7] = 0xff;

	CAN2_send(&TxMsg);
}


/*
 * @brief 充电机发送时间同步信息报文(PGN1792)
 */
void bms_pt_send_cts()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1807F4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 7;
	memset(TxMsg.Data, 0xff, 7);  //无效数据
	CAN2_send(&TxMsg);
}

/*
 * @brief 充电机最大输出能力报文(PGN2048)
 */
void bms_pt_send_cml()
{
	CanTxMsg TxMsg;

	u16 max_v = 750 * 10;  //最高输出电压  单位0.1V 范围0~800V
	u16 min_v = 100 * 10;  //最低输出电压  单位0.1V 范围0~800V
	u16 max_i = (800 - 20 * evc_info.module_cnt) * 10;  //最高输出电流  单位0.1A -800A偏移  范围-800A ~ 0A

	TxMsg.ExtId = 0x1808F4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)max_v;
	TxMsg.Data[1] = (u8)(max_v >> 8);

	TxMsg.Data[2] = (u8)min_v;
	TxMsg.Data[3] = (u8)(min_v >> 8);

	TxMsg.Data[4] = (u8)max_i;
	TxMsg.Data[5] = (u8)(max_i >> 8);

	TxMsg.Data[6] = 0;  //脉冲放电电流（A） ：充电机能允许的放电电流，充电机若不具备放电功能应 设置为 0 数据分辨率：2A/位，0A 偏移量；数据范围：0~500A；
	TxMsg.Data[7] = 0;  //脉冲放电能持续时间（s） ：充电机若不具备放电功能应设置为 0	数据分辨率：1s/位，0s 偏移量；数据范围：0~255s；
	CAN2_send(&TxMsg);
}

/*
 * @brief 充电机输出准备就绪报文(PGN2560)
 */
void bms_pt_send_cro()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x140AF4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 1;

	TxMsg.Data[0] = 0x04;

	CAN2_send(&TxMsg);
}


/*
 * @brief 充电机充电状态报文(PGN4608)
 */
void bms_pt_send_ccs()
{
	CanTxMsg TxMsg;

	u16 v = evc_info.read.v;  //电压输出值  单位0.1V 范围0~800V
	u16 c = 8000 - (evc_info.read.i / 10);  //电流输出值  单位0.1A -800A偏移  范围-800A ~ 0A
	u16 t = bms_info.timer_tick / (100 * 60);  //累计充电时间  单位分钟  范围0~6000min

	TxMsg.ExtId = 0x1814F4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 6;

	TxMsg.Data[0] = (u8)v;
	TxMsg.Data[1] = (u8)(v >> 8);

	TxMsg.Data[2] = (u8)c;
	TxMsg.Data[3] = (u8)(c >> 8);

	TxMsg.Data[4] = (u8)t;
	TxMsg.Data[5] = (u8)(t >> 8);

	CAN2_send(&TxMsg);
}


/*
 * @brief 充电机中止充电报文(PGN6656)
 */
void bms_pt_send_cst()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x081AF4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 3;

	TxMsg.Data[0] = 0x03;
	TxMsg.Data[1] = 0x00;
	TxMsg.Data[2] = 0x00;

	CAN2_send(&TxMsg);
}



/*
 * @brief 充电机统计数据报文(PGN7424)
 */
void bms_pt_send_csd()
{
	CanTxMsg TxMsg;

	u16 t = 0; //充电时间
	u16 kWh = 0;  //输出能量
	u8  id = 1;  //充电机编号

	TxMsg.ExtId = 0x181AF4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 5;

	TxMsg.Data[0] = (u8)t;
	TxMsg.Data[1] = (u8)(t >> 8);

	TxMsg.Data[2] = (u8)kWh;
	TxMsg.Data[3] = (u8)(kWh >> 8);

	TxMsg.Data[4] = (u8)id;

	CAN2_send(&TxMsg);
}

/*
 * 接收到报文数据处理
 */
void bms_pt_msg_handler(u8 pgn, u8* msg, u16 len)
{
	switch(pgn)
	{
	/************************** 充电握手阶段 ********************/
	case PGN_512:  //BRM BMS和车辆辨识报文
	{
		PGN512_Def *temp = (PGN512_Def*)msg;
		BMS_MSG("PGN512 type:%x V:%d Ah:%d\n", temp->battery_type, temp->V / 10, temp->Ah / 10);

		bms_info.battery_type = temp->battery_type;
		bms_info.Ah = temp->Ah;
		bms_info.V  = temp->V;

		bms_pt_send_crm(0x01);  //确认辨识
		bms_pt_time_info.pgn512 = bms_info.timer_tick;  //保存收到报文时间
	}
	break;


	case PT_PGN_12288:  //VBI 车辆和电池标识
	{
		msg[20] = 0;
		BMS_MSG("PT_PGN_12288 %c%c%c VIN:\n", msg[0], msg[1], msg[2]);
		bms_info.stage = BMS_stage_config;
		//for(int i = 3; i < 20; i++)
		//{
			//BMS_MSG("%x", msg->buf[i]);
		//}
	}
	break;
	/***********************************************************/




	/************************** 充电参数配置阶段 ****************/
	case PGN_1536:  //BCP 蓄电池充电参数报文
	{
		PT_PGN1536_Def *temp = (PT_PGN1536_Def*)msg;

		bms_info.limt_v = temp->limt_v;
		bms_info.limt_i = 8000 - temp->limt_i;
		BMS_MSG("PT_PGN1536 limt_v_single:%d limt_i:%d limt_v:%d V:%d Ah:%d SOC:%d\n", temp->limt_v_single / 100, (8000 - temp->limt_i)/10,temp->limt_v/10, temp->v / 10, temp->max_Ah, temp->SOH);

		bms_pt_send_cts();  //发送时间同步信息  与国标相同
		os_dly_wait(10);
		bms_pt_send_cml();  //充电机最大输出能力
		bms_info.stage = BMS_stage_config;
	}
	break;


	case PGN_2304:  //BRO 电池准备充电就绪报文
	{
		BMS_MSG("PGN_2304 %x\n", msg[0]);
		if(msg[0] == 0x04)  //准备完成  可以充电
		{
			bms_pt_send_cro();  // 充电机输出准备就绪
		}
		else  //不能充电
		{

		}
	}
	break;
	/***********************************************************/




	/************************** 充电阶段 ************************/
	case PGN_4096:  //BCL 电池充电需求报文
	{
		PGN4096_Def *temp = (PGN4096_Def*)msg;

		//BMS_MSG("PGN_4096 v:%d  i:%d  mode:%d\n", temp->v / 10, 800 - temp->i/10, temp->cc_cv);
		//evc_set_v_c(temp->v, (8000 - temp->i) * 10);  //设置EVC模块电压电流
		//请求电压加5V   2016.1.13
		evc_set_v_c((temp->v + 50), (8000 - temp->i) * 10);  //设置EVC模块电压电流

		bms_pt_send_ccs();  //发送充电机充电状态报文

		if(bms_info.stage != BMS_stage_end)
		{
			bms_info.stage = BMS_stage_charging;  //进入充电阶段
		}

		bms_pt_time_info.pgn4096 = bms_info.timer_tick;  //保存收到报文时间
	}
	break;

	case PGN_4352:  //BCS 电池充电总状态报文
	{
		PT_PGN4352_Def *temp = (PT_PGN4352_Def*)msg;
		bms_info.out_v = temp->v;
		bms_info.out_i = 8000 - temp->i;
		bms_info.SOC = temp->SOC;
		bms_info.remain_t = temp->remain_t;
		bms_info.max_temp = temp->max_t;
		//BMS_MSG("PGN_4352 v:%d i:%d mint:%d maxt:%d soc:%d remaintime:%d\n", temp->v/10,800-temp->i/10, temp->min_t - 50, temp->max_t-50,temp->SOC,temp->remain_t);
	}
	break;


	case PGN_4864:  //BSM 电池状态信息报文
	{
		//bms_info.max_v_single = ((msg->Data[1] << 8) | msg->Data[0]) & 0x0fff;  //最高蓄电池（最小测量单元）电压，数据分辨率：0.01V
		//bms_info.max_temp = msg->Data[3];  //最高单体温度
		//BMS_MSG("PGN_4864 max_v_single %d 10mv\n", bms_info.max_v_single);
	}
	break;

	case PGN_6400:  //BST BMS中止充电报文
	{
		BMS_MSG("PGN_6400---------------- %x\n", msg[0]);

		bms_pt_send_cst();  //充电机中止充电报文

		bms_info.stage = BMS_stage_end;
	}
	break;

	case PGN_5376:  //BMV 单体动力蓄电池状态信息报文
	{
		BMS_MSG("PGN_5376\n");
	}
	break;

	case PGN_5632:  //BMT 动力蓄电池温度报文
	{
		BMS_MSG("PGN_5632\n");
	}
	break;

	case PGN_5888:  //BSOC 蓄电池组荷电容量 SOC 值报文
	{
		BMS_MSG("PGN_5888\n");
	}
	break;

	/***********************************************************/




	/************************ 充电结束阶段 **********************/
	case PGN_7168:  //BSD BMS统计数据报文
	{
		PGN7168_Def *temp = (PGN7168_Def*)msg;
		bms_info.SOC = temp->SOC;
		bms_info.min_v_single = temp->min_v_single / 10;
		bms_info.max_v_single = temp->max_v_single / 10;
		bms_info.min_temp = temp->min_temp_single;
		bms_info.max_temp = temp->max_temp_single;

		if(bms_info.stage == BMS_stage_end)
		{
			BMS_MSG("PGN_7168  end charging\n");
			bms_pt_send_csd();  //充电机统计数据报文
		}
	}
	break;
	/***********************************************************/

	case PT_PGN_12544:  //电池生命帧报文
		break;


	case PGN_7680:  //BEM BMS错误报文
	{
		BMS_ERROR("PGN_7680 %02x %02x %02x %02x\n", msg[0], msg[1], msg[2] ,msg[3]);
		if(bms_info.err_id == 0)
		{
			bms_info.err_id = (msg[3] << 24) | (msg[2] << 16) | (msg[1] << 8) | msg[0];
		}
	}
	break;




	default:
		BMS_ERROR("bms_pt other msg %x\n", pgn);
		break;
	}
}

int bms_pt_poll()
{
	switch(bms_info.stage)
	{
	case BMS_stage_handshake:

		if((bms_pt_time_info.pgn512 == 0) && ((bms_info.timer_tick - bms_pt_time_info.pgn256) > 25))  //BRM BMS和车辆辨识报文  250ms
		{
			bms_pt_send_crm(0);
			bms_pt_time_info.pgn256 = bms_info.timer_tick;  //保存发送时间
		}

		break;

	case BMS_stage_config:
		evc_set_v_c(0, 0);
		evc_out_onoff(0xff);
		io_bms_dc_out_en();
		break;

	case BMS_stage_charging:

		if((bms_info.timer_tick - bms_pt_time_info.pgn4096) > 10)  //BCL 电池充电需求报文  100ms没收到则超时  停止充电
		{
			//停止充电
			evc_out_onoff(0);  //关闭输出
			bms_pt_send_cst();  //充电机中止充电报文
			return 1;
		}

		break;

	case BMS_stage_end:  //收到BST BMS中止充电报文进入结束阶段
		return 1;

//		break;
	}

	
	if(stop_charge)
	{
		return 1;
	}

	return 0;
}

/*
 * CAN数据接收超时
 */
void bms_pt_timeout_handler()
{

}


/*
 * 初始化
 */
void bms_pt_init()
{
	memset(&bms_pt_time_info, 0, sizeof(bms_pt_time_info));

	bms_pt_send_crm(0);
	bms_pt_time_info.pgn256 = bms_info.timer_tick;  //保存发送时间
	bms_info.stage = BMS_stage_handshake;
}



/*
 * 定义相应处理函数
 */
bms_ptl_handler_def bms_pt_event =
{
		bms_pt_init,
		bms_pt_msg_handler,
		bms_pt_timeout_handler,
		bms_pt_poll,
};
