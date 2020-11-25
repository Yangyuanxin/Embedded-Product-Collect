#include "bms.h"
#include "uart.h"
#include "error_code.h"
#include "gm_stdlib.h"
#include "command.h"
#include "log_service.h"
#include "gm_memory.h"
#include "utility.h"
#include "system_state.h"
#include "json.h"
#include "protocol.h"
#include "socket.h"
#include "gm_timer.h"
#include "config_service.h"

#define BMS_SEND_MAX_CNT 10 //读取BMS数据重复次数
#define BMS_SEND_INTERVAL 1 //读取BMS超时间隔 1s
#define BMS_INTERVAL_TIME 30 //间隔时间
#define BMS_INIT_TIME 2 //初始化 等待时间再去读取
#define BMS_TRANSPRENT_MAX_LEN 1024
#define BMS_MAX_VOLT 21



#define BMS_HEAD 0x3A
#define BMS_ADDR 0x16
#define BMS_END_1 0x0D
#define BMS_END_2 0x0A
#define BMS_DATA_DEFAULT 0x0B
#define BMS_MIN_LENTH 8


typedef enum 
{
    BMS_INIT = 0,
	BMS_CONFIG,
	BMS_SCAN,
	BMS_WAIT_ACK,
	BMS_STANDBY,
	BMS_SLEEP,
	BMS_ERROR,
	
	BMS_STATUS_MAX,
}GetBmsStatusEnum;


typedef enum 
{
    BMS_CMD_08,//电池组内部温度
	BMS_CMD_09,//电池总电压
	BMS_CMD_0A,//电池实时电流
	BMS_CMD_0D,//相对容量百分比
	BMS_CMD_0E,//绝对容量百分比
	BMS_CMD_0F,//剩余容量
	BMS_CMD_10,//满电容量
	BMS_CMD_17,//循环次数
	BMS_CMD_24,//1-7 节电池电压
	BMS_CMD_25,//8-14 节电池电压
	BMS_CMD_26,//5-21 节电池电压
	BMS_CMD_37,//关掉或者打开放电mos
	BMS_CMD_38,//电池休眠GPRS 模块指令
	BMS_CMD_55,//让电池进入shipmode

	BMS_CMD_MAX
}BmsCmdEnum;


typedef struct
{
	u8 cmd;      /* current status */
	u8 cmd_idx;  /*当前读取的指令位置*/
	char string[15];
}BmsCmdStruct;


static BmsCmdStruct s_bms_cmd[BMS_CMD_MAX] = 
{
	{0x08,BMS_CMD_08,"temprature"},
	{0x09,BMS_CMD_09,"voltage"},
	{0x0A,BMS_CMD_0A,"current"},
	{0x0D,BMS_CMD_0D,"rel_capacity"},
	{0x0E,BMS_CMD_0E,"abs_capacity"},
	{0x0F,BMS_CMD_0F,"capacity"},
	{0x10,BMS_CMD_10,"full_capacity"},
	{0x17,BMS_CMD_17,"circle"},
	{0x24,BMS_CMD_24,"vlist"},
	{0x25,BMS_CMD_25,"vlist"},
	{0x26,BMS_CMD_26,"vlist"},
	{0x37,BMS_CMD_37,"mos"},
	{0x38,BMS_CMD_38,"sleep"},
	{0x55,BMS_CMD_55,"shipmode"},
};



typedef struct
{
	u32 recv_valid;//有接收到有效值，用来判断是否上传字段
	float temprature;
	u16 voltage;
	s16 current;
	float rel_capacity;
	float abs_capacity;
	u16 capacity;
	u16 full_capacity;
	u16 circle;
	u16 vlist[BMS_MAX_VOLT];//1-BMS_MAX_VOLT voltage
	u8 mos;
	u8 sleep;
	u8 shipmode;
}BmsRecvData;


typedef struct
{
    GetBmsStatusEnum  status;      /* current status */
	kal_bool trans_once; //只获取一次状态，用于休眠时读取数据
    u8  cmd_index;  //当前读取的指令位置
    u32 time_clock;
	u8  send_cnt;//当前发送次数
	BmsRecvData RecvData;
}BmsType;

static BmsType s_bms;


static void bms_vlot_list_to_jsonstr(char* json_str)
{
	u8 index = 0;
	JsonObject* p_vlot_list = NULL;
	char string[5];
	kal_bool is_good = KAL_FALSE;

	if (json_str == NULL)
	{
		return;
	}
	
	p_vlot_list = json_create();
	for (index=0; index<BMS_MAX_VOLT; index++)
	{
		if (s_bms.RecvData.vlist[index] != 0xFFFF)
		{
			is_good = KAL_TRUE;
			GM_memset(string, 0, sizeof(string));
			GM_sprintf(string, "v%d", index+1);
			json_add_int(p_vlot_list, string, s_bms.RecvData.vlist[index]);
		}
	}

	if (is_good == KAL_TRUE)
	{
		json_print_to_buffer(p_vlot_list, json_str, sizeof(json_str) - 1);
	}
	
	json_destroy(p_vlot_list);
}


static void bms_struct_to_jsonstr(char* json_str)
{
	u8 index = 0;
	JsonObject* p_bms_battery = NULL;
	char *p_list = NULL;
	kal_bool is_insert = KAL_FALSE;
	kal_bool is_good = KAL_FALSE;

	if (json_str == NULL)
	{
		return;
	}
	
	p_bms_battery = json_create();
	
	for (index=0; index<BMS_CMD_MAX; index++)
	{
		if ((s_bms.RecvData.recv_valid >> index) & 0x01)
		{
			is_good = KAL_TRUE;
			switch(index)
			{
				case BMS_CMD_08:
					json_add_double(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.temprature);
					break;

				case BMS_CMD_09:
					json_add_int(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.voltage);
					break;

				case BMS_CMD_0A:
					{
						s32 current = 0;

						current = s_bms.RecvData.current;
						if (GET_BIT15(current))
						{
							CLR_BIT15(current);
							SET_BIT31(current);
						}
						json_add_int(p_bms_battery, s_bms_cmd[index].string, current);
					}
					break;

				case BMS_CMD_0D:
					json_add_double(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.rel_capacity);
					break;

				case BMS_CMD_0E:
					json_add_double(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.abs_capacity);
					break;

				case BMS_CMD_0F:
					json_add_int(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.capacity);
					break;

				case BMS_CMD_10:
					json_add_int(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.full_capacity);
					break;

				case BMS_CMD_17:
					json_add_int(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.circle);
					break;
				
				case BMS_CMD_24:
				case BMS_CMD_25:
				case BMS_CMD_26:
					{
						if (is_insert == KAL_TRUE)break;
						is_insert = KAL_TRUE;
						p_list = GM_MemoryAlloc(12 * BMS_MAX_VOLT + 2);//,"v16":65535 {} 12 * BMS_MAX_VOLT + 2
						GM_memset(p_list, 0, 12 * BMS_MAX_VOLT + 2);
						bms_vlot_list_to_jsonstr(p_list);
						if (GM_strlen(p_list) == 0)
						{
							GM_MemoryFree(p_list);
							p_list = NULL;
							break;
						}
						json_add_string(p_bms_battery, s_bms_cmd[index].string, p_list);
						GM_MemoryFree(p_list);
						p_list = NULL;
					}
					break;

				case BMS_CMD_37:
					json_add_int(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.mos);
					break;

				case BMS_CMD_38:
					json_add_int(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.sleep);
					break;

				case BMS_CMD_55:
					json_add_int(p_bms_battery, s_bms_cmd[index].string, s_bms.RecvData.shipmode);
					break;

				default:
					break;

			}
			
		}
	}

	if(is_good == KAL_TRUE)
	{
		json_print_to_buffer(p_bms_battery, json_str, sizeof(json_str) - 1);
	}
	
	json_destroy(p_bms_battery);
}



GM_ERRCODE bms_battery_request_response(char *json_str)
{
	if (s_bms.status == BMS_SLEEP)
	{
		bms_create();
		s_bms.trans_once = KAL_TRUE;
		return GM_EMPTY_BUF;
	}
	
	bms_struct_to_jsonstr(json_str);
	if (GM_strlen(json_str) > 0)
	{
		protocol_send_transprent_msg(get_socket_by_accessid(SOCKET_INDEX_MAIN), json_str);
	}
	else
	{
		s_bms.trans_once = KAL_TRUE;
	}
	
	return GM_SUCCESS;
}


BmsCmdEnum bms_find_index_by_cmd(u8 cmd)
{
	u16 index = 0;

	for (index=0; index<BMS_CMD_MAX; index++)
	{
		if (s_bms_cmd[index].cmd == cmd)
		{
			return (BmsCmdEnum)s_bms_cmd[index].cmd_idx;
		}
	}

	return BMS_CMD_MAX;
}


void bms_transprent_callback(void)
{
	char *json_str = NULL;

	json_str = (char*)GM_MemoryAlloc(BMS_TRANSPRENT_MAX_LEN+1);
	if (json_str)
	{
		bms_battery_request_response(json_str);
		GM_MemoryFree((void *)json_str);
	}
}


GM_ERRCODE bms_check_next_status(BmsCmdEnum cmd_index)
{
	if (cmd_index == BMS_CMD_38)
	{
		s_bms.status = BMS_SLEEP;
		bms_destroy();
	}
	else if (s_bms.trans_once == KAL_TRUE && cmd_index == BMS_CMD_26)
	{
		s_bms.trans_once = KAL_FALSE;
		if (GM_SYSTEM_STATE_WORK == system_state_get_work_state())
		{
			s_bms.status = BMS_SLEEP;
			bms_destroy();
		}
		else
		{
			s_bms.status = BMS_SCAN;
			s_bms.cmd_index++;
		}
	}
	else if (cmd_index <= BMS_CMD_26)
	{
		s_bms.status = BMS_SCAN;
		s_bms.cmd_index++;
	}
	
	return GM_SUCCESS;
}




u8 bms_cmd_prase(char* p_cmd, u16 cmd_len)
{
	u8 cmd = 0;
	u8 len;
	u8 idx = 0;
	u8 list = 0;
	kal_bool is_upload = KAL_FALSE;
	BmsCmdEnum cmd_index;

	cmd = p_cmd[2];//命令
	cmd_index = bms_find_index_by_cmd(cmd);
	if (cmd_index >= BMS_CMD_MAX)
	{
		return 2;//命令不对
	}
	
	len = p_cmd[3] + BMS_MIN_LENTH;
	if ((p_cmd[len-1] != BMS_END_2) || (p_cmd[len-2] != BMS_END_1))
	{
		return 2;//结尾不对
	}

	LOG(DEBUG,"clock(%d) bms_cmd_prase cmd(%X) cmd_index(%X)", util_clock(), cmd, cmd_index);
	s_bms.RecvData.recv_valid |= (1 << cmd_index);
	idx = 4;
	switch(cmd_index)
	{
		case BMS_CMD_08:
			s_bms.RecvData.temprature = ((p_cmd[idx+1] << 8) + p_cmd[idx] - 2731) / 10.0f;
			break;

		case BMS_CMD_09:
			s_bms.RecvData.voltage = (p_cmd[idx+1] << 8) + p_cmd[idx];
			break;

		case BMS_CMD_0A:
			s_bms.RecvData.current = (p_cmd[idx+1] << 8) + p_cmd[idx];
			break;

		case BMS_CMD_0D:
			s_bms.RecvData.rel_capacity = ((p_cmd[idx+1] << 8) + p_cmd[idx]) / 100.0f;
			break;

		case BMS_CMD_0E:
			s_bms.RecvData.abs_capacity = ((p_cmd[idx+1] << 8) + p_cmd[idx]) / 100.0f;
			break;

		case BMS_CMD_0F:
			s_bms.RecvData.capacity = (p_cmd[idx+1] << 8) + p_cmd[idx];
			break;

		case BMS_CMD_10:
			s_bms.RecvData.full_capacity = (p_cmd[idx+1] << 8) + p_cmd[idx];
			break;

		case BMS_CMD_17:
			s_bms.RecvData.circle = (p_cmd[idx+1] << 8) + p_cmd[idx];
			break;

		case BMS_CMD_24:
			for (idx = 4; idx<p_cmd[3]+4;idx+=2)
			{
				list = (idx-4) / 2;
				s_bms.RecvData.vlist[list] = (p_cmd[idx+1] << 8) + p_cmd[idx];
			}
			break;

		case BMS_CMD_25:
			for (idx = 4; idx<p_cmd[3]+4;idx+=2)
			{
				list = (idx-4) / 2 + 7;
				s_bms.RecvData.vlist[list] = (p_cmd[idx+1] << 8) + p_cmd[idx];
			}
			break;

		case BMS_CMD_26:
			for (idx = 4; idx<p_cmd[3]+4;idx+=2)
			{
				list = (idx-4) / 2 + 14;
				s_bms.RecvData.vlist[list] = (p_cmd[idx+1] << 8) + p_cmd[idx];
			}
			break;

		case BMS_CMD_37:
			{
				kal_bool is_on = KAL_FALSE;
				
				config_service_get(CFG_IS_BMS_MOS_CUT_OFF, TYPE_BOOL, &is_on, sizeof(is_on));
				s_bms.RecvData.mos = (is_on == KAL_FALSE) ? 0 : 1;
				if (s_bms.status == BMS_CONFIG)
				{
					s_bms.status = BMS_SCAN;
					s_bms.cmd_index = BMS_CMD_08;
				}
				else
				{
					is_upload = KAL_TRUE;
				}
			}
			break;

		case BMS_CMD_38:
			s_bms.RecvData.sleep = 0x01;//p_cmd[idx];
			is_upload = KAL_TRUE;
			break;

		case BMS_CMD_55:
			s_bms.RecvData.shipmode = p_cmd[idx];
			is_upload = KAL_TRUE;
			break;

		default:
			break;
	}

	s_bms.time_clock = util_clock();
	if ((is_upload == KAL_TRUE) || (s_bms.trans_once == KAL_TRUE && cmd_index == BMS_CMD_26))
	{
		GM_StartTimer(GM_TIMER_BMS_TRANSPRENT, 1000, bms_transprent_callback);
	}
	bms_check_next_status(cmd_index);
	
	return len;
}


GM_ERRCODE bms_uart_receive(char* p_cmd, u16 cmd_len)
{
	u16 idx = 0;
	
	if (NULL == p_cmd || cmd_len < BMS_MIN_LENTH)
	{
		LOG(WARN, "clock(%d), bms_uart_receive receive error p_cmd(%d), cmd_len(%d)", util_clock(), p_cmd, cmd_len);
		return GM_PARAM_ERROR;
	}
	
	do
	{
		if (p_cmd[idx] == BMS_HEAD && p_cmd[idx+1] == BMS_ADDR)
		{
			idx += bms_cmd_prase(&p_cmd[idx], cmd_len-idx);
		}
		else
		{
			idx++;
		}
	}while (cmd_len-idx > BMS_MIN_LENTH);

	return GM_SUCCESS;
}




GM_ERRCODE bms_cmd_write_to_bms(u8 cmd, u8 *pdata, u16 len)
{
	u8 *bms_buff = NULL;
	u16 idx = 0;
	u8 index = 0;
	u16 check_sum = 0;
	
	bms_buff = GM_MemoryAlloc(len+8);
	if (NULL == bms_buff)
	{
		s_bms.status = BMS_ERROR;
		LOG(WARN, "clock(%d), bms_cmd_write_to_bms GM_MemoryAlloc (%d)!.", util_clock(), bms_buff);
		return GM_SYSTEM_ERROR;
	}
	GM_memset(bms_buff, 0x00, len+8);
	
	bms_buff[idx++] = BMS_HEAD;
	bms_buff[idx++] = BMS_ADDR;
	bms_buff[idx++] = cmd;
	
	if ((len > 0) && (NULL != pdata))
	{
		bms_buff[idx++] = len;
		for(index=0; index<len; index++)
		{
			bms_buff[idx++] = pdata[index];
		}
	}
	else
	{
		bms_buff[idx++] = 0x01;
		bms_buff[idx++] = BMS_DATA_DEFAULT;
	}
	
	for (index=1; index<idx; index++)
	{
		check_sum += bms_buff[index];
	}
	
	bms_buff[idx++] = check_sum & 0xFF;
	bms_buff[idx++] = (check_sum >> 8) & 0xFF;
	bms_buff[idx++] = BMS_END_1;
	bms_buff[idx++] = BMS_END_2;

	s_bms.send_cnt++;
	uart_write(GM_UART_BMS,bms_buff, idx);
	GM_MemoryFree(bms_buff);
	bms_buff = NULL;

	return GM_SUCCESS;
}



GM_ERRCODE bms_battery_mos_output_ctrl(u8 is_on)
{
	return bms_cmd_write_to_bms(s_bms_cmd[BMS_CMD_37].cmd, &is_on, 1);
}


GM_ERRCODE bms_init_proc(void)
{	
	if (GM_SYSTEM_STATE_WORK == system_state_get_work_state() || s_bms.trans_once == KAL_TRUE)
	{
		if (util_clock() - s_bms.time_clock >= BMS_INIT_TIME)
		{
			s_bms.time_clock = util_clock();
			s_bms.status = BMS_CONFIG;
		}
	}
	
	return GM_SUCCESS;
}

GM_ERRCODE bms_config_proc(void)
{
	kal_bool mos = KAL_FALSE;
	u8 is_on = 0;
	
	config_service_get(CFG_IS_BMS_MOS_CUT_OFF, TYPE_BOOL, &mos, sizeof(mos));
	is_on = (mos == KAL_FALSE) ? 0 : 1;
	bms_battery_mos_output_ctrl(is_on);

	return GM_SUCCESS;
}



GM_ERRCODE bms_scan_proc(void)
{
	if (s_bms.cmd_index <= BMS_CMD_26)
	{
		if (GM_SUCCESS == bms_cmd_write_to_bms(s_bms_cmd[s_bms.cmd_index].cmd,NULL,0))
		{
			s_bms.time_clock = util_clock();
			s_bms.status = BMS_WAIT_ACK;
		}
		else
		{
			s_bms.status = BMS_ERROR;
		}
	}
	else
	{
		s_bms.time_clock = util_clock();
		s_bms.status = BMS_STANDBY;
	}
	
	return GM_SUCCESS;
}


GM_ERRCODE bms_wait_ack_proc(void)
{
	if (util_clock() - s_bms.time_clock >= BMS_SEND_INTERVAL)
	{
		if (s_bms.send_cnt <= BMS_SEND_MAX_CNT)
		{
			s_bms.status = BMS_SCAN;
		}
		else if (s_bms.cmd_index == BMS_CMD_24 || s_bms.cmd_index == BMS_CMD_25 || s_bms.cmd_index == BMS_CMD_26)
		{//电池组不确定个数，允许不回复
			s_bms.status = BMS_SCAN;
			s_bms.cmd_index++;
		}
		else
		{
			LOG(WARN, "clock(%d) bms_wait_ack_proc bms no ack! index(%d).", util_clock(), s_bms.cmd_index);
			s_bms.status = BMS_ERROR;
		}
	}
	
	return GM_SUCCESS;
}

GM_ERRCODE bms_standby_proc(void)
{
	if (util_clock() - s_bms.time_clock >= BMS_INTERVAL_TIME)
	{
		LOG(INFO, "clock(%d) bms_standby_proc interval time to scan bms.", util_clock());
		s_bms.send_cnt = 0;
		s_bms.cmd_index = BMS_CMD_08;
		s_bms.status = BMS_SCAN;
	}
	
	return GM_SUCCESS;
}


GM_ERRCODE bms_error_proc(void)
{
	if (GM_SYSTEM_STATE_WORK == system_state_get_work_state() || s_bms.trans_once == KAL_TRUE)
	{
		bms_destroy();
		bms_create();
	}
	else
	{
		bms_destroy();
		s_bms.status = BMS_SLEEP;
	}

	return GM_SUCCESS;
}





GM_ERRCODE bms_timer_proc(void)
{
	switch(s_bms.status)
	{
		case BMS_INIT:
			bms_init_proc();
			break;
		case BMS_CONFIG:
			bms_config_proc();
			break;
		case BMS_SCAN:
			bms_scan_proc();
			break;
		case BMS_WAIT_ACK:
			bms_wait_ack_proc();
			break;
		case BMS_STANDBY:
			bms_standby_proc();
			break;
		case BMS_SLEEP:
			break;
		case BMS_ERROR:
			bms_error_proc();
			break;
		default:
			LOG(INFO,"clock(%d) bms_timer_proc bms status error status = %d!.",util_clock(), s_bms.status);
			s_bms.status = BMS_INIT;
			break;
	}
	
	return GM_SUCCESS;
}
	

GM_ERRCODE bms_create(void)
{	
	u8 index = 0;
	
	GM_memset(&s_bms, 0, sizeof(BmsType));
	s_bms.status = BMS_INIT;
	s_bms.time_clock = util_clock();
	for(index = 0; index<BMS_MAX_VOLT; index++)
	{
		s_bms.RecvData.vlist[index] = 0xFFFF;
	}
    uart_open_port(GM_UART_BMS, BAUD_RATE_LOW, 0);
    
    return GM_SUCCESS;
}


GM_ERRCODE bms_destroy(void)
{
	uart_close_port(GM_UART_BMS);
	s_bms.cmd_index = 0;
	s_bms.status = BMS_SLEEP;
	
    return GM_SUCCESS;
}


