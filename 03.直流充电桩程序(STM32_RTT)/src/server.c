#include "RTL.h"
#include "stm32f10x.h"
#include "server.h"
#include "dbg.h"
#include "udp.h"
#include "evc.h"
#include "bms.h"
#include "string.h"
#include "sys_cfg.h"
#include "em.h"
#include "delay.h"
#include "sflash.h"
#include "gprs.h"
#include "server.h"

#if 1
#define SERVER_MSG dbg_msg
#define SERVER_ERROR dbg_error
#else
#define SERVER_MSG (void)
#define SERVER_ERROR (void)
#endif


#define SERVER_EVT_RECV 0x01
#define SERVER_EVT_IDLE  0x02
#define SERVER_EVT_CHING  0x04
#define SERVER_EVT_CHEND  0x08

extern OS_TID server_task_id;


server_handler_def server_handler;

server_info_def server_info;


chg_info_def chg_info;  //充电结算数据

OS_SEM chgend_sem;

OS_MUT server_buf_mutex;

#define RECV_DATA_LEN 100
u16 recv_data_len;  //收到数据的长度
u8 recv_data[RECV_DATA_LEN];  //收到数据buf

#define SEND_DATA_LEN 100
u8 send_data[SEND_DATA_LEN];  //发送数据buf

u32 crc32(u8 *buf, u16 len);

int udp_send(u8 *buf, u16 len);

/*
 * @brief  接收到服务器数据处理函数  在UDP接收到数据时调用
 * @param  buf 数据
 * @param  len 数据字节数
 */
void server_recv_callback(u8 *buf, u16 len)
{
	if(len <= RECV_DATA_LEN)
	{
		com_header_def *temp = (com_header_def*)buf;
		os_mut_wait (&server_buf_mutex, 0xffff);  //防止数据在未被处理的情况下被覆盖
		if((len > 12) && (temp->fh == 0x5A) && (buf[temp->len - 1] == 0xA5) && (temp->id == sys_cfg.id))  //校验帧头帧尾  ID
		{
			u32 *temp_crc;
			temp_crc = (u32*)&buf[temp->len - 5];
			if(*temp_crc == crc32(buf, temp->len - 5))
			{
				server_info.status = 1;  //正常

				if(temp->cmd == 0)  //服务器响应空闲数据
				{
					server_info.rsp_cnt++;
				}
				else if(temp->cmd == 1)  //服务器响应充电中数据
				{
					server_info.rsp_cnt++;
				}
				else if(temp->cmd == 2)  //服务器响应充电结束数据
				{
					os_sem_send(&chgend_sem);
					server_info.rsp_cnt++;
					server_info.status = 1;  //正常
				}
				else
				{
					memcpy(recv_data, buf, len);
					recv_data_len = len;
					os_evt_set(SERVER_EVT_RECV, server_task_id);
					return;
				}
			}
		}
		os_mut_release(&server_buf_mutex);  //释放
	}
	//SERVER_MSG("%s %d\n", buf, len);
}


///*
// * @brief  等待服务器数据
// * @param  type 数据类型  0:空闲数据  1:充电数据  2:结算数据
// * @retval 0:成功  1:超时  2：本地校验错误  3:数据错误 说明发送的数据有错误
// */
//static int wait_rsp(u8 type)
//{
//	com_header_def *com = (com_header_def*)recv_data;
//	if(os_sem_wait(&recv_sem, 2000) != OS_R_TMO)  //收到服务器应答数据
//	{
//		if(recv_data_len < 13)
//		{
//			return 2;  //校验错误
//		}
//		if((recv_data[0] == 0x5A) && (recv_data[recv_data_len - 1] == 0xA5) && (com->len == recv_data_len))
//		{
//			u32 *temp_crc;
//			temp_crc = (u32*)&recv_data[recv_data_len - 5];
//			if(*temp_crc == crc32(recv_data, recv_data_len - 5))
//			{
//				if(recv_data[7] == type)
//				{
//					if(recv_data[8] == 0)  //检查应答结果
//					{
//						return 0;  //正确应答
//					}
//					else
//					{
//						return 3;  //数据错误
//					}
//				}
//			}
//		}
//		return 2;  //本地校验错误
//	}
//	else
//	{
//		return 1;  //超时
//	}
//}
/*
 * @brief  充电机空闲时发送 空闲数据报文
 * @retval 0:成功  1:超时
 */
static int send_idle()
{
	int ret;
	com_header_def *com = (com_header_def*)send_data;
	u8 *p = com->data;
	com->fh = 0x5A;
	com->len = 12 + 1;
	com->id = sys_cfg.id;
	com->cmd = 0;  //表示空闲数据
	*((u32*)p) = crc32(send_data, com->len - 5);  //添加CRC
	p += 4;
	*p = 0xA5;  //帧尾

	server_handler.send(send_data, (u16)(p - send_data) + 1);

	server_info.send_cnt++;

	return 0;

//	ret = wait_rsp(0);
//
//	//SERVER_MSG("ret %d\n", ret);
//	return ret;
}



/*
 * @brief  充电机充电时发送 充电数据报文
 * @retval 0:成功  1:超时
 */
static int send_chging()
{
	int ret;
	u32 *crc;
	com_header_def *com = (com_header_def*)send_data;
	com_chging_def *p = (com_chging_def*)com->data;
	com->fh = 0x5A;
	com->len = 12 + 25;
	com->id = sys_cfg.id;
	com->cmd = 1;   //表示充电数据

	p->id = chg_info.id;  //ic卡号
	p->v = evc_info.read.v;  //电压
	p->i = evc_info.read.i / 10;  //电流
	p->kwh = em_info.kwh;  //消耗电量
	p->amount = em_info.sum;  //消耗金额
	p->time = (bms_info.timer_tick / 100) / 60;  //充电时长
	p->soc = bms_info.SOC;  //电池SOC
	p->start_time.y = chg_info.start_time.y;   //年
	p->start_time.m = chg_info.start_time.m;   //月
	p->start_time.d = chg_info.start_time.d;    //日
	p->start_time.h = chg_info.start_time.h;   //时
	p->start_time.mm = chg_info.start_time.mm;   //分
	p->start_time.s = chg_info.start_time.s;   //秒
	p->gun_id = chg_info.gun_id;
	p->max_s_v = bms_info.max_v_single;
	crc = (u32*)&send_data[32];
	*crc = crc32(send_data, 32);
	send_data[36] = 0xA5;
	server_handler.send(send_data, 37);

	server_info.send_cnt++;

	return 0;
//	ret = wait_rsp(1);
//	return ret;
	//SERVER_MSG("ret %d\n", ret);
}


/*
 * @brief  充电机充电完成时发送 充电结算数据报文
 * @retval 0:成功  1:超时
 */
static int send_chgend()
{
	int ret;
	u8 cnt = 0 ;
	u32 *crc;
	com_header_def *com = (com_header_def*)send_data;
	com_chgend_def *p = (com_chgend_def*)com->data;
	com->fh = 0x5A;
	com->len = 12 + 23;
	com->id = sys_cfg.id;
	com->cmd = 2;  //表示充电结算数据

	p->id = chg_info.id;  //ic卡号
	p->balances = chg_info.balances;  //余额
	p->kwh = em_info.kwh;  //消耗电量
	p->amount = em_info.sum;  //消耗金额
	p->time = (bms_info.timer_tick / 100) / 60;  //充电时长
	p->soc = bms_info.SOC;  //电池SOC
	p->start_time.y = chg_info.start_time.y;   //年
	p->start_time.m = chg_info.start_time.m;   //月
	p->start_time.d = chg_info.start_time.d;    //日
	p->start_time.h = chg_info.start_time.h;   //时
	p->start_time.mm = chg_info.start_time.mm;   //分
	p->start_time.s = chg_info.start_time.s;   //秒
	crc = (u32*)&send_data[30];
	*crc = crc32(send_data, 30);
	send_data[34] = 0xA5;

re_try:
	os_sem_init(&chgend_sem, 0);  //sem token  置零
	server_handler.send(send_data, 35);

	if(os_sem_wait(&chgend_sem, 3000) != OS_R_TMO)  //收到数据
	{
		return 0;
	}

	if(cnt < 3)
	{
		cnt++;
		goto re_try;
	}
	return 1;
}


/*
 * @brief  回应服务器发来的数据报文
 * @param  type 数据类型  0x20:电价设置  0x40:读取数据
 * @param  code 0:无错误  1:数据错误
 */
static int response(u8 type, u8 code)
{
	u32 *crc;
	com_header_def *com = (com_header_def*)send_data;
	com->fh = 0x5A;
	com->len = 12 + 2;
	com->id = sys_cfg.id;

	send_data[7] = type;
	send_data[8] = code;

	crc = (u32*)&send_data[9];
	*crc = crc32(send_data, 9);
	send_data[13] = 0xA5;
	server_handler.send(send_data, 14);
	return 0;
}

#define FW_VER "1.0.0"
/*
 * @brief  发送服务器读取充电机信息
 */
static int send_read_info()
{
	u32 *crc;
	com_header_def *com = (com_header_def*)send_data;
	machine_info_def *info = (machine_info_def*)com->data;
	com->fh = 0x5A;
	com->len = 24;
	com->id = sys_cfg.id;
	com->cmd = 0x40;

	memcpy(&info->ver[0], FW_VER, 5);
	info->price = sys_cfg.e_price;
	info->kwh = em_info.kwh_cnt;

	crc = (u32*)&send_data[19];
	*crc = crc32(send_data, 19);
	send_data[23] = 0xA5;
	server_handler.send(send_data, 24);
	return 0;
}



/*
 * @brief  发送充电机故障信息
 * @param  err 错误代码
 */
static int send_err_info(u32 err)
{
	u32 *crc, *p;
	com_header_def *com = (com_header_def*)send_data;
	com->fh = 0x5A;
	com->len = 12 + 5;
	com->id = sys_cfg.id;

	send_data[7] = 0x80;

	p = (u32*)&send_data[8];
	*p = err;

	crc = (u32*)&send_data[com->len - 5];
	*crc = crc32(send_data, com->len - 5);
	send_data[com->len - 1] = 0xA5;
	server_handler.send(send_data, com->len);
	return 0;
}



void server_send_idle(void)
{
	chg_info.status = 1;  //充电机空闲
	os_evt_set(SERVER_EVT_IDLE, server_task_id);
}


void server_send_chging(void)
{
	os_evt_set(SERVER_EVT_CHING, server_task_id);
}


void server_send_chgend(void)
{
	os_evt_set(SERVER_EVT_CHEND, server_task_id);
}



u32 server_delay_base;
u32 m10_delay_base;  //10分钟定时时基

extern u8 ui_idle;  //当屏幕处于主界面(page2)时为0   在其它界面为1    防止当有人正在操作时开启充电
extern u8 ui_chg_id;   //远程开启充电的充电枪编号  后台发送充电指令时设置此值  0:无效    1:充电枪1  2:充电枪2   当不等于0时说明正在充电


void task_server(void)
{
	u32 evt;
	com_header_def *recv_header = (com_header_def*)recv_data;

	os_sem_init(&chgend_sem, 0);  //标志在发送充电结束数据后收到服务器的正确响应

	os_mut_init (&server_buf_mutex);  //server buf 互斥锁



	server_handler.send = udp_send;
	//server_handler.send = gprs_send;
	server_handler.recv_callback = server_recv_callback;

	server_info.state = SERVER_SATUS_IDLE;

	delay_reset(&server_delay_base);

	delay_reset(&m10_delay_base);

	while(1)
	{
		evt = os_evt_wait_or(0xffff, 5000);  //等待事件
		if(evt == OS_R_EVT)  //收到数据
		{
			evt = os_evt_get();  //获取事件

			if(evt & SERVER_EVT_RECV)  //接收到服务器数据
			{
				if(recv_header->cmd == 0x20)  //设置电价
				{
					u16 temp = (recv_data[9] << 8) | recv_data[8];  //电价
					sys_cfg.e_price = temp;
					if(sys_cfg_save() == 0)
					{
						response(0x20, 0);  //回应服务器
					}
				}
				else if(recv_header->cmd == 0x40)  //读取充电机信息
				{
					send_read_info();
				}
//				else if(recv_header->cmd == 0x21)  //开启充电
//				{
//					u8 temp = recv_data[8];
//					if((!ui_idle) && (!ui_chg_id) && ((temp < 1) || (temp > 2)))  //充电枪编号必须为1 或  2
//					{
//						ui_chg_id = temp;
//						response(0x21, 0);  //回应服务器
//					}
//					else
//					{
//						response(0x21, 1);  //回应服务器
//					}
//				}
//				else if(recv_header->cmd == 0x22)  //关闭充电
//				{
//					bms_stop_charge();
//					response(0x21, 0);  //回应服务器
//				}
//				else if(recv_header->cmd == 0x23)  //远程开机
//				{
//					GPIO_SetBits(GPIOE, GPIO_Pin_2);
//					response(0x23, 0);  //回应服务器
//				}
//				else if(recv_header->cmd == 0x24)  //远程关机
//				{
//					GPIO_ResetBits(GPIOE, GPIO_Pin_2);
//					response(0x24, 0);  //回应服务器
//				}

				os_mut_release(&server_buf_mutex);  //释放
			}
			if(evt & SERVER_EVT_IDLE)  //发送空闲数据
			{
				server_info.state = SERVER_SATUS_IDLE;
			}
			if(evt & SERVER_EVT_CHING)
			{
				server_info.state = SERVER_SATUS_CHGING;
			}
			if(evt & SERVER_EVT_CHEND)
			{
				server_info.state = SERVER_SATUS_CHGEND;
			}
		}

/********************************************  检查与服务器连接状态  **************************************************/
		if(server_info.send_cnt > server_info.rsp_cnt)
		{
			if(server_info.send_cnt - server_info.rsp_cnt > 10)  //连续10次没收到服务器数据认为掉线
			{
				server_info.status = 0;  //标志离线
			}
		}

		if(delay_timeout_reload(&m10_delay_base, 1000 * 60 * 10) == 0)  //10分钟定时
		{
			server_info.send_cnt = 0;
			server_info.rsp_cnt = 0;
		}
/********************************************************************************************************************/


		switch(server_info.state)
		{
			case SERVER_SATUS_IDLE:  //发送空闲数据
			{
				if(delay_timeout_reload(&server_delay_base, 5000) == 0)  //5s定时时间到
				{
					send_idle();

					//SERVER_MSG("server_stauts %d  %d  %d\n", server_info.status, server_info.send_cnt, server_info.rsp_cnt);
				}
			}
			break;

			case SERVER_SATUS_CHGING:  //发送正在充电数据
			{
				if(delay_timeout_reload(&server_delay_base, 5000) == 0)  //5s定时时间到
				{
					send_chging();
				}
			}
			break;

			case SERVER_SATUS_CHGEND:  //发送充电结算数据
			{
				send_chgend();
				//os_dly_wait(500);
				//send_chgend();
//				if(!send_chgend())
//				{
//
//				}
				server_info.state = SERVER_SATUS_IDLE;
			}
			break;

			case SERVER_SATUS_ERR:  //发送错误信息
			{
				if(delay_timeout_reload(&server_delay_base, 10000) == 0)  //00s定时时间到
				{
					u32 err = 0;
					send_err_info(err);
				}
			}
			break;

			default: break;
		}
	}
}



#if 1

//两种方法计算结果是一致的  STM32用硬件算法   软件算法用于其它平台校验使用

/*
 * @brief  STM32内部硬件CRC32计算   注意打开CRC时钟！
 * @param  buf 数据
 * @param  len 数据字节数
 * @retval 校验结果
 */
u32 crc32(u8 *buf, u16 len)
{
	u16 index;
	CRC_ResetDR();        //复位CRC
	for(index = 0; index < len; index++)
	{
		CRC->DR = (u32)buf[index];
	}
	return (CRC->DR);
}
#else
/*
 * @brief  软件CRC32计算
 * @param  buf 数据
 * @param  len 数据字节数
 * @retval 校验结果
 */
u32 crc32(u8 *buf, u16 len)
{

#define POLY  0x04c11db7

    int i,j;
    u32 crc = 0xFFFFFFFF;
    for (j = 0; j < len; j++)
    {
        crc = crc ^ (*buf++);
        for (i = 0; i < 32; i++)
        {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ POLY;
            else
                crc <<= 1;
        }
    }
    return(crc);
}
#endif
