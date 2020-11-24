#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "evc.h"
#include "string.h"
#include "io.h"

#if 1
#define EVC_MSG dbg_msg
#else
#define EVC_MSG (void)
#endif

#if 1
#define EVC_ERROR dbg_error
#else
#define EVC_ERROR (void)
#endif


#define ERR_PS(s) (((s >> 24) & 0xff) == 13)  //相序错误
#define ERR_OCP(s)  ((s & 0xff) == 1)  //DC 过流

#define EVC_ON(s) ((s >> 8) & 0x01)  //运行状态
#define EVC_OFF(s) (!((s >> 8) & 0x01))  //停止状态

#define CMD_ONOFF 	  Bit(1)
#define CMD_RESET 	  Bit(2)
#define CMD_MODE  	  Bit(3)
#define CMD_SAVE  	  Bit(4)
#define CMD_SET_LIMT  Bit(5)
#define CMD_SET_V_C	  Bit(6)


evc_info_def evc_info;

extern OS_TID evc_task_id;

CanRxMsg can1_rx_msg;

OS_SEM can1_rx_sem;

/*
 * 使能CAN1接收
 */
void CAN1_rx_en()
{
	CAN_FIFORelease(CAN1, CAN_FIFO0);
	CAN1->FA1R |= Bit(0);  //激活过滤器0
}

/*
 * 关闭CAN1接收
 */
void CAN1_rx_dis()
{
	CAN1->FA1R &= ~Bit(0);
}

/*
 * @brief  CAN1接收中断处理程序
 */
void CAN1_RX0_IRQHandler()
{
	CAN1_rx_dis();
	CAN_Receive(CAN1, CAN_FIFO0, &can1_rx_msg);

	isr_sem_send(&can1_rx_sem);
}


int can_send(CanTxMsg* tx)
{
	CAN_Transmit(CAN1, tx);
	return 1;
}

/*
 * @brief  配置模块   回应数据与发数据相同则成功
 * @param  tx: CAN消息
 * @retval 1 发送成功   0 失败
 */
int config_send(CanTxMsg* tx)
{
	u8 err_cnt = 3;
	int ret = 0;

	CAN1_rx_en();
	do{
		can_send(tx);
		if(os_sem_wait(&can1_rx_sem, 100) != OS_R_TMO)  //等待响应
		{
			//未超时
			u32 temp = can1_rx_msg.ExtId & (~0x0000ffff);
			temp |= HTONS((u16)can1_rx_msg.ExtId);
			if(tx->ExtId == temp)
			{
				if(memcmp(tx->Data, can1_rx_msg.Data, 8) == 0)  //比较收到的数据与发送的是否相同
				{
					ret = 1;
					break;
				}
			}
		}
		else
		{
			//超时
			//EVC_ERROR("config_send timeout\n");
		}
		os_dly_wait(50);
	}while(--err_cnt);

	CAN1_rx_dis();

	return ret;
}




/*
 * @brief  读取模块数据
 * @param  tx: CAN消息
 * @retval 1 发送成功收到应答信息   0 失败应答超时
 */
int read_info_send(CanTxMsg* tx)
{
	u8 err_cnt = 3;
	int ret = 0;

	CAN1_rx_en();
	do
	{
		can_send(tx);

		if(os_sem_wait(&can1_rx_sem, 100) != OS_R_TMO)  //等待响应
		{
			u32 temp = can1_rx_msg.ExtId & (~0x0000ffff);
			temp |= HTONS((u16)can1_rx_msg.ExtId);
			if(tx->ExtId == temp)
			{
				ret = 1;
				break;
			}
		}
		os_dly_wait(50);
	}while(--err_cnt);

	CAN1_rx_dis();

	return ret;
}

/*
 * @brief  配置模块输出电压 电流
 * @param  id: 模块id号
 * @param  v 单位0.1V
 * @param  c 单位0.01A
 * @retval 1 成功   0 失败
 */
int config_v_c(u8 id, u16 v, u16 c)
{
	CanTxMsg TxMsg;

	if(v > evc_info.max_v)
	{
		v = evc_info.max_v;
	}

	if(c > evc_info.max_i)
	{
		c = evc_info.max_i;
	}

	TxMsg.ExtId = 0x020200ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)(v >> 8);
	TxMsg.Data[1] = (u8)v;
	TxMsg.Data[2] = (u8)(c >> 8);
	TxMsg.Data[3] = (u8)c;
	memset(&TxMsg.Data[4], 0, 8 - 4);
	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}

/*
 * @brief  开启或关闭模块输出
 * @param  id: 模块id号
 * @param  on_off:  0x00 关闭   oxff 打开
 * @retval 1 成功   0 失败
 */
int config_on_off(u8 id, u8 on_off)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x010400ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	TxMsg.Data[0] = on_off;
	memset(&TxMsg.Data[1], 0, 8 - 1);
	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}


/*
 * @brief  存储配置参数
 * @param  id: 模块id号
 * @param  save:  0x01 有效   0x00 取消
 * @retval 1 成功   0 失败
 */
int config_save_data(u8 id, u8 save)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x010200ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	TxMsg.Data[0] = save;
	memset(&TxMsg.Data[1], 0, 8 - 1);
	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}



/*
 * @brief  配置运行时间
 * @param  id: 模块id号
 * @param  run_t: 定时运行时间 小于6000s
 * @param  delay_t: 启动->运行延迟时间设定值 小于30s
 * @param  timeout_t: 模块通信超时停机功能时间  小于300s
 * @retval 1 成功   0 失败
 */
int config_run_time(u8 id, u32 run_t, u16 delay_t, u16 timeout_t)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x020300ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)(run_t >> 24);
	TxMsg.Data[1] = (u8)(run_t >> 16);
	TxMsg.Data[2] = (u8)(run_t >> 8);
	TxMsg.Data[3] = (u8)(run_t);

	TxMsg.Data[4] = (u8)(delay_t >> 8);
	TxMsg.Data[5] = (u8)(delay_t);

	TxMsg.Data[6] = (u8)(timeout_t >> 8);
	TxMsg.Data[7] = (u8)(timeout_t);

	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}


/*
 * @brief  配置通讯超时停机功能
 * @param  id: 模块id号
 * @param  s: 0:关闭   >0 使能
 * @retval 1 成功   0 失败
 */
int config_com_timeout(u8 id, u8 s)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x010100ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = 0x02;
	TxMsg.Data[1] = 0x02;
	TxMsg.Data[2] = 0x01;
	if(s)
	{
		TxMsg.Data[3] = 0x01;
	}
	else
	{
		TxMsg.Data[3] = 0x00;
	}

	TxMsg.Data[4] = 0;

	memset(&TxMsg.Data[5], 0, 8 - 5);

	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}


/*
 * @brief  复位模块
 * @param  id: 模块id号
 * @retval 1 成功   0 失败
 */
int reset_module(u8 id)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x010300ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	TxMsg.Data[0] = 1;
	memset(&TxMsg.Data[1], 0, 8 - 1);
	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}

/*
 * @brief  读取模块状态  具体状态定义见通信协议
 * @param  id: 模块id号
 * @retval 小于0xffffffff 成功   0xffffffff失败
 */
u32 read_status(u8 id)
{
	CanTxMsg TxMsg;
	u32 ret = 0;

	TxMsg.ExtId = 0x030300ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	memset(&TxMsg.Data[0], 0, 8);

	if(read_info_send(&TxMsg))
	{
		ret |= can1_rx_msg.Data[0];
		ret |= can1_rx_msg.Data[1] << 8;
		ret |= can1_rx_msg.Data[2] << 16;
		ret |= can1_rx_msg.Data[3] << 24;
	}
	else
	{
		ret = 0xffffffff;
	}
	return ret;
}

/*
 * @brief  读取输出电压电流
 * @param  id: 模块id号
 * @retval 小于0xffffffff 成功   0xffffffff失败  高两字节为电流  低两字节为电压   大端格式
 */
u32 read_v_c(u8 id)
{
	CanTxMsg TxMsg;
	u32 cv = 0;

	TxMsg.ExtId = 0x030200ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	memset(&TxMsg.Data[0], 0, 8);
	if(read_info_send(&TxMsg))
	{
		cv = *(u32*)can1_rx_msg.Data;
	}
	else
	{
		cv = 0xffffffff;
	}
	return cv;
}


/*
 * @brief  读取输出最大电压电流
 * @param  id: 模块id号
 * @retval 小于0xffffffff 成功   0xffffffff失败  高两字节为电流  低两字节为电压   大端格式
 */
u32 read_limt(u8 id)
{
	CanTxMsg TxMsg;
	u32 cv = 0;

	TxMsg.ExtId = 0x030100ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	memset(&TxMsg.Data[0], 0, 8);
	if(read_info_send(&TxMsg))
	{
		cv = *((u32*)(&can1_rx_msg.Data[4]));
	}
	else
	{
		cv = 0xffffffff;
	}
	return cv;
}


/*
 *
 */
u8 evc_is_err_status(u32 status)
{
	u8  dc_err = (u8)status;
	u8  pfc_err = (u8)(status >> 24);

	if((dc_err == 1) ||
	   (dc_err == 2) ||
	   (dc_err == 3))
	{
		return 1;
	}

	if((pfc_err == 1) ||
	   (pfc_err == 2) ||
	   (pfc_err == 3) ||
	   (pfc_err == 4) ||
	   (pfc_err == 5) ||
	   (pfc_err == 6) ||
	   (pfc_err == 7) ||
	   (pfc_err == 8) ||
	   (pfc_err == 9) ||
	   (pfc_err == 10) ||
	   (pfc_err == 11) ||
	   (pfc_err == 12) ||
	   (pfc_err == 13) ||
	   (pfc_err == 14) ||
	   (pfc_err == 15) ||
	   (pfc_err == 16) ||
	   (pfc_err == 33))
	{
		return 1;
	}
	return 0;
}



/******************  外部任务调用命令处理函数  ************************/


void cmd_onoff_handler()
{
	config_on_off(0, evc_info.config.onoff);  //配置每个模块
	os_dly_wait(50);
	config_on_off(0, evc_info.config.onoff);  //配置每个模块

	if(evc_info.config.onoff == 0)  //关闭
	{
		u8 i;
		io_runled_off();  //运行灯灭
		io_fun_off();  //风扇关
		evc_info.config.onoff = 0;
		evc_info.config.set_c = 0;
		evc_info.config.set_v = 0;

		for(i = 0; i < MAX_MODULE_CNT; i++)
		{
			evc_info.module_info[i].ocp_reset_cnt = 0;
		}
	}
	else
	{
		io_runled_on();    //运行灯亮
		io_fun_on();  //风扇开
	}
}


void cmd_set_v_c_handler()
{
	u16 c = (evc_info.config.set_c) / evc_info.module_cnt_no_err;  //电流平均

	config_v_c(0, evc_info.config.set_v, c);  //配置每个模块
	os_dly_wait(50);
	config_v_c(0, evc_info.config.set_v, c);
}
/******************************************/

/*
 * 重新设置电压电流
 */
static void re_set_v_c()
{
	os_evt_set(CMD_SET_V_C, evc_task_id);
}


u16 filter_calc(u8 id, u8 vi)
{
	u8 j, i;
	u16 value_buf[READ_SAMP_CNT], temp, count;
	u32  sum = 0;

	for(j = 0; j < READ_SAMP_CNT; j++)
	{
		if(vi == 0)
		{
			value_buf[j] = evc_info.module_info[id].v_temp[j];
		}
		else
		{
			value_buf[j] = evc_info.module_info[id].i_temp[j];
		}
	}

	for (j = 0; j < READ_SAMP_CNT - 1; j++) //排序  小到大
	{
		for (i = j + 1; i < READ_SAMP_CNT; i++)
		{
			if (value_buf[j] > value_buf[i])
			{
				temp = value_buf[i];
				value_buf[i] = value_buf[j];
				value_buf[j] = temp;
			}
		}
	}

#define N 1

	for (count = N; count < READ_SAMP_CNT - N; count++)
	{
		sum += value_buf[count];
	}

	return (u16)(sum / (READ_SAMP_CNT - N * 2));
}

void remove_old(u8 id)
{
	u8 j;
	for(j = 0; j < READ_SAMP_CNT - 1; j++)
	{
		evc_info.module_info[id].v_temp[j] = evc_info.module_info[id].v_temp[j + 1];
		evc_info.module_info[id].i_temp[j] = evc_info.module_info[id].i_temp[j + 1];
	}
}

/*
 * @brief    获取无故障模块总数
 * @retval   总数
 */
static u8 get_cnt()
{
	u8 i;
	u8 cnt = 0;
	for(i = 0; i < MAX_MODULE_CNT; i++)
	{
		if((evc_info.module_info[i].id != 0) && (evc_info.module_info[i].is_err == 0))
		{
			cnt++;
		}
	}
	return cnt;
}


/*
 * @brief    获取在线障模块总数
 * @retval   总数
 */
static u8 get_cnt_online()
{
	u8 i;
	u8 cnt = 0;
	for(i = 0; i < MAX_MODULE_CNT; i++)
	{
		if(evc_info.module_info[i].id != 0)
		{
			cnt++;
		}
	}
	return cnt;
}


OS_MUT evc_mutex;

static U64 stk_evc_scan[200];

/*
 * 循环扫描所有模块
 */
void task_evc_scan()
{
	u8 t = 0;

	os_dly_wait(2000);

	while(1)
	{
		if((t++ % 10) == 0)  //扫描模块
		{
			u8 i;
			u8 set_vc_again = 0;
			u32 ret;

			for(i = 1; i < (MAX_MODULE_CNT + 1); i++)  //模块编号从1号开始
			{
				os_mut_wait (&evc_mutex, 0xffff);
				ret = read_status(i);  //读取模块状态
				os_mut_release (&evc_mutex);

				//EVC_MSG("status i %d %x\n", i, ret);

				if(ret != 0xffffffff)  //读取到状态
				{
					evc_info.module_info[i - 1].id = i;
					evc_info.module_info[i - 1].status = ret;
					evc_info.module_info[i - 1].link_off = 0;

					if(!evc_is_err_status(ret))  //没故障
					{
						evc_info.module_info[i - 1].is_err = 0;  //模块正常

						if(evc_info.config.onoff && EVC_OFF(ret))  //模块没有输出
						{
							os_mut_wait (&evc_mutex, 0xffff);
							config_v_c(i, 0, 0);  //电压电流先设置为0
							os_dly_wait(50);
							config_on_off(i, 0xff);  //开启模块
							set_vc_again = 1;
							os_mut_release (&evc_mutex);
						}
						else if((!evc_info.config.onoff) && EVC_ON(ret))  //模块没有关闭
						{
							os_mut_wait (&evc_mutex, 0xffff);
							config_on_off(i, 0);  //关闭模块
							os_mut_release (&evc_mutex);
						}

						if(evc_info.max_v == 0)  //读取最大输出参数
						{
							u32 temp;
							os_mut_wait (&evc_mutex, 0xffff);
							temp = read_limt(i);  //读最大输出电压电流
							os_mut_release (&evc_mutex);
							evc_info.max_v = HTONS((u16)temp);
							evc_info.max_i = HTONS((u16)(temp >> 16));
							
							evc_info.max_v /= 100;
							evc_info.max_v *= 100;  //去掉零头
							evc_info.max_i -= 100;  //减少1A

							if(evc_info.max_v == 5000)  //500V模块
							{
								evc_info.max_i = 3000;  //最大电流30A
							}
						}

						if(evc_info.module_info[i - 1].cfg_end == 0)  //还没设置参数
						{
							os_mut_wait (&evc_mutex, 0xffff);
							if(config_com_timeout(i, 1))  //开启通讯超时停机功能
							{
								if(config_run_time(i, 6000, 8, 30))  //超时时间30s
								{
									evc_info.module_info[i - 1].cfg_end = 1;
								}
							}
							os_mut_release (&evc_mutex);
						}
					}
					else  //模块有故障
					{
						evc_info.module_info[i - 1].is_err = 1;

						if(ERR_OCP(ret))  //DC 过流
						{
							os_mut_wait (&evc_mutex, 0xffff);
							reset_module(i);  //复位模块
							os_mut_release (&evc_mutex);
							set_vc_again = 1;
//							if(evc_info.config.onoff)  //输出状态
//							{
//								if(evc_info.module_info[i - 1].ocp_reset_cnt == 0)  //第一次复位
//								{
//									delay_reset(&evc_info.module_info[i - 1].ocp_reset_t);  //计时器复位
//									evc_info.module_info[i - 1].ocp_reset_cnt++;
//								}
//								else if(delay_timeout_reload(&evc_info.module_info[i - 1].ocp_reset_t, 1000 * 30) == 0)  //复位之后不到30S再次报警
//								{
//									evc_info.module_info[i - 1].ocp_reset_cnt++;
//								}
//							}
						}

						if(ERR_PS(ret))  //相序错误
						{
							evc_info.err_ps = 1;
						}
					}
				}
				else
				{
					if(evc_info.module_info[i - 1].id != 0)  //原来是正常的
					{
						evc_info.module_info[i - 1].link_off++;  //检测到一次离线
						if(evc_info.module_info[i - 1].link_off == 3)  //模块离线
						{
							set_vc_again = 1;
							memset(&evc_info.module_info[i - 1], 0, sizeof(module_info_def));
						}
					}
					else
					{
						//当前id号的模块不存在或通讯异常
						evc_info.module_info[i - 1].id = 0;
						evc_info.module_info[i - 1].status = 0xffffffff;  //通讯异常
					}
				}
			}

			evc_info.module_cnt = get_cnt_online();  //在线模块数

			evc_info.module_cnt_no_err = get_cnt();  //无故障模块总数

			if((t % 6) == 60)  //30S 设置一次电压电流
			{
				set_vc_again = 1;
			}

			if(set_vc_again)  //重新设置电压电流
			{
				set_vc_again = 0;
				if(evc_info.config.onoff)  //输出状态
				{
					re_set_v_c();
				}
			}
		}



		if ((evc_info.config.onoff == 0xff) || //开启输出状态
		   ((evc_info.config.onoff == 0) && ((evc_info.read.v != 0) || (evc_info.read.i != 0)))) //或已经关闭   但还有残余电压
		{
			u32 read_v = 0, read_i = 0;
			u16 temp_v, temp_i;
			u8 i, read_cnt = 0;

			//读取每个模块电压电流
			for (i = 1; i < (MAX_MODULE_CNT + 1); i++) //模块编号从1号开始
			{
				if ((evc_info.module_info[i - 1].id != 0) && (evc_info.module_info[i - 1].is_err == 0)) //模块正常
				{
					u32 temp;
					os_mut_wait(&evc_mutex, 0xffff);
					temp = read_v_c(i); //读电压电流
					os_mut_release(&evc_mutex);

					if (temp < 0xffffffff) //读取成功
					{
						temp_v = HTONS((u16)temp);
						temp_i = temp >> 16;
						temp_i = HTONS(temp_i);
						if (temp_v < 500) //低于50V
						{
							temp_v = 0;
						}
						if (temp_v == 0)
						{
							temp_i = 0;
						}

						{
							remove_old(i - 1);
							evc_info.module_info[i - 1].v_temp[READ_SAMP_CNT - 1] =	temp_v;
							evc_info.module_info[i - 1].i_temp[READ_SAMP_CNT - 1] =	temp_i;
							evc_info.module_info[i - 1].v = filter_calc(i - 1, 0); //电压滑动均值滤波
							evc_info.module_info[i - 1].i = filter_calc(i - 1, 1); //电流滑动均值滤波
						}
					}
					else
					{
						//EVC_MSG("ERR %d\n", i);
					}
				}
			}

			for(i = 1; i < (MAX_MODULE_CNT + 1); i++) //模块编号从1号开始
			{
				if((evc_info.module_info[i - 1].id != 0) && (evc_info.module_info[i - 1].is_err == 0)) //模块正常
				{
					if(read_v < evc_info.module_info[i - 1].v)  //电压取最大值
					{
						read_v = evc_info.module_info[i - 1].v;
					}

					read_i += evc_info.module_info[i - 1].i;
					read_cnt++;
				}
			}

			evc_info.read.v = read_v; //电压
			evc_info.read.i = read_i; //总电流
		}
		os_dly_wait(500);
	}
}


/*
 * EVC模块控制任务
 */
void task_evc()
{
	u32 evt;

	memset(&evc_info, 0, sizeof(evc_info));

	os_sem_init(&can1_rx_sem, 0);
	os_mut_init (&evc_mutex);

	os_tsk_create_user(task_evc_scan, 3, stk_evc_scan, sizeof(stk_evc_scan));  //充电模块


	while(1)
	{
		evt = os_evt_wait_or(0xffff, 1000 * 10);  //等待事件
		if(evt == OS_R_EVT)  //执行外部命令
		{
			evt = os_evt_get();  //获取事件

			os_mut_wait (&evc_mutex, 1000 * 10);  //在task_evc_scan任务中也会向模块发数据，为防止两个任务冲突，使用互斥信号量

			if(evt & CMD_SET_V_C)  //设置电压电流
			{
				cmd_set_v_c_handler();
			}

			if(evt & CMD_ONOFF)  //开启或关闭输出
			{
				cmd_onoff_handler();
			}

			os_mut_release (&evc_mutex);
		}
	}
}












/**************************************   外部调用     ****************************/

/*
 * @brief  开启或关闭输出
 * @param  id: 模块id号
 * @param  on_off:  0x00 关闭   0xff 打开
 */
void evc_out_onoff(u8 onoff)
{
	evc_info.config.onoff = onoff;
	os_evt_set(CMD_ONOFF, evc_task_id);
}

/*
 * @brief  配置输出电压 电流
 * @param  v 单位0.1V 最高电压 不超过1200 V  当为0xffff时参数无效
 * @param  c 单位0.01A 最高电流 不超过55 A   当为0xffff时参数无效
 * @param  mode  模式  0x01恒压  0x02恒流
 */
void evc_set_v_c(u16 v, u16 c)
{
	if((v != 0) && (c != 0))
	{
		if((evc_info.config.set_v == v) && (evc_info.config.set_c == c))
		{
			return;
		}
	}

	if(v != 0xffff)
	{
		evc_info.config.set_v = v;
	}

	if(c != 0xffff)
	{
		evc_info.config.set_c = c;
	}

	os_evt_set(CMD_SET_V_C, evc_task_id);
}



