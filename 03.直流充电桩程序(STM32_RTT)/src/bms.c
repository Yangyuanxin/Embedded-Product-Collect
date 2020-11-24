#include "RTL.h"
#include "bms.h"
#include "stm32f10x.h"
#include "block_fifo.h"
#include "string.h"
#include "evc.h"
#include "dbg.h"
#include "shk.h"
#include "io.h"
#include "measure.h"

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

bms_chg_info_def bms_chg_info;

bms_info_def bms_info;

OS_MUT can2_mutex;


u8 stop_charge = 0xff;  //停止充电标志  0xff:停止 0:运行
u8 bms_reset = 0;  //标志

/***********************************************************************/

/*
 * TIM4溢出中断处理程序(10ms)  用于BMS定时
 */
void TIM4_IRQHandler(void)
{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		bms_info.timer_tick++;
}




CanRxMsg CAN2_RxMessage;

OS_SEM CAN2_rx_sem;

u32 CAN2_rx_buf[(sizeof(b_fifo_typ) + sizeof(CanRxMsg) * 10 + 10 * 2 + 3) / 4];

/*
 * @brief  CAN2接收中断处理程序
 */
void CAN2_RX0_IRQHandler()
{
	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_Receive(CAN2, CAN_FIFO0, &CAN2_RxMessage);
		b_fifo_write(CAN2_rx_buf, (u8*)&CAN2_RxMessage, sizeof(CAN2_RxMessage));
		isr_sem_send(&CAN2_rx_sem);
	}
	else if (CAN_GetITStatus(CAN2, CAN_IT_FF0))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FF0);
	}
	else if (CAN_GetITStatus(CAN2, CAN_IT_FOV0))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FOV0);
	}
/*
	{
		int i;
		BMS_MSG("CAN2 RX: ");
		BMS_MSG("    %08x  ", CAN2_RxMessage.ExtId);
		for(i = 0; i < 8; i++)
		{
			BMS_MSG("%02x ", CAN2_RxMessage.Data[i]);
		}
		BMS_MSG("\n");
	}
//	*/
}


/*
 * 使能CAN2接收
 */
void CAN2_rx_en()
{
	CAN_FIFORelease(CAN2, CAN_FIFO0);
	CAN1->FA1R |= Bit(14);  //激活过滤器14
}

/*
 * 关闭CAN2接收
 */
void CAN2_rx_dis()
{
	CAN1->FA1R &= ~Bit(14);
}

/*
 * CAN2发送数据
 */
void CAN2_send(CanTxMsg *tx)
{
	CAN_Transmit(CAN2, tx);

/*	
		{
		int i;
		BMS_MSG("CAN2 TX: ");
		BMS_MSG("    %08x  ", tx->ExtId);
		for(i = 0; i < 8; i++)
		{
			BMS_MSG("%02x ", tx->Data[i]);
		}
		BMS_MSG("\n");
	}
	//	*/
}


/*
 * 设置CAN2 接收过滤器ID 低16位
 *
 */
void bms_set_filter_id(u16 id)
{
	//FilterId  BMS 使用第14组
    CAN1->sFilterRegister[14].FR1 = (((id >> 13) & 0x0007) << 16) | ((id << 3) |CAN_ID_EXT | CAN_RTR_DATA);
    //MaskId
    CAN1->sFilterRegister[14].FR2 = 0x0007FFFF;
}



//分包管理  当数据大于8字节时会用分包传输协议
#define MULT_PKG_LEN 1100
u8 mult_pkg_buf[MULT_PKG_LEN];  //多包数据
u8 mult_pkg_rx_en;  //当收到分包请示时置位   1:接收  0:不接收

mult_pkg_info_def mult_pkg_info =
{
		0,
		0,
		0,
		0,
		0,
		{0, 0, 0},
		mult_pkg_buf,
};


/*
 * @brief 当收到BMS请求发送多包数据时用于回应BMS 表示以准备好接收数据
 */
void tp_send_cts(u8 pkg_cnt, u8 id, u8 *pgn)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1CECF456;

	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = 17;  //控制字节
	TxMsg.Data[1] = pkg_cnt;  //可发送的数据包数目
	TxMsg.Data[2] = id;  //下一个将要发送的数据包编号

	TxMsg.Data[3] = 0xff;
	TxMsg.Data[4] = 0xff;

	TxMsg.Data[5] = pgn[0];
	TxMsg.Data[6] = pgn[1];
	TxMsg.Data[7] = pgn[2];

	CAN2_send(&TxMsg);
}

/*
 * 通知发送者 多包数据接收完成
 */
void tp_send_EndofMsgAck()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1CECF456;

	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = 19;  //控制字节
	TxMsg.Data[1] = mult_pkg_info.len;  //总数据长度
	TxMsg.Data[2] = (u8)(mult_pkg_info.len >> 8);

	TxMsg.Data[3] = mult_pkg_info.pkg_cnt;  //数据数
	TxMsg.Data[4] = 0xff;

	TxMsg.Data[5] = mult_pkg_info.pgn[0];
	TxMsg.Data[6] = mult_pkg_info.pgn[1];
	TxMsg.Data[7] = mult_pkg_info.pgn[2];

	CAN2_send(&TxMsg);
}


u8 get_pgn(u32 id)
{
	return ((id >> 16) & 0x000000ff);
}

u8 get_pri(u32 id)
{
	return ((id >> 24) & 0x000000ff);
}




OS_SEM bms_start_sem;




u8 bms_test;
/*
 * @brief  BMS协议是否检测完成
 * @retval 0:正在检测 1:成功 2:失败
 */
u8 bms_test_isok()
{
	return bms_test;
}



/*
 * @brief           启动充电
 * @param start_type  启动方式   1:后台     2:刷卡    3:管理员
 * @param chg_type  充电方式         0:自动充满   1:按度数   2:时间    3:按金额
 * @param value  启动方式参数    电量0.01kwh   时间:1分钟       金额:0.01元
 * @retval 1 成功   0 失败
 */
int bms_start_charge(u8 start_type, u8 chg_type, u32 value)
{
	if(stop_charge != 0xff)
	{
		return -1;
	}
//	if(em_calc_init() != 0)
//	{
//		return -2;
//	}

	bms_chg_info.start_type = start_type;
	bms_chg_info.chg_type = chg_type;
	bms_chg_info.value = value;

	bms_test = 0;
	stop_charge = 0;  //
	os_sem_send(&bms_start_sem);
	return 0;
}

/*
 * @brief           停止充电
 * @param r  		停止原因  (定义见bms.h 中  bms_info_def   中的    stop_reason)
 */
void bms_stop_charge(u8 r)
{
	if(stop_charge == 0)
	{
		bms_info.stop_reason = r;
		stop_charge = 1;
		evc_out_onoff(0);  //关闭输出
	}
}

/*
 * @brief  检查充电是否完成
 * @retval 0 未停止  1:正在停止充电    0xff：完成停止 (接收完统计报文)
 */
u8 bms_charge_complete()
{
	return stop_charge;
}



extern bms_ptl_handler_def bms_gb_event;
extern bms_ptl_handler_def bms_xgb_event;
/*
 * bms任务
 */
void task_bms()
{
	u8 pgn, pri;
	CanRxMsg *BMS_rx_msg;
	b_fifo_data_typ *block;
	u8 temp;
	u8 v24;

	os_mut_init(&can2_mutex);

	os_sem_init(&bms_start_sem, 0);

	CAN2_rx_dis();
	bms_set_filter_id(0x56f4);  //CAN ID过滤


	while(1)
	{
		os_sem_wait(&bms_start_sem, 0xffff);  //等待外部启动充电命令
		memset(&bms_info, 0, sizeof(bms_info));

		evc_set_v_c(0, 0);

		v24 = 0;  //先用12V

restart:

		if(v24 == 0)
		{
			io_bms_12v_en();  //输出辅助电源
		}
		else
		{
			io_bms_24v_en();  //输出辅助电源
		}

		/********************************************** BMS协议检测 ********************************************/

		/***************  新国标协议  **************/

		if(v24 == 0)  //12V才检测
		{
			CAN2_rx_dis();
			os_sem_init(&CAN2_rx_sem, 0);
			b_fifo_init(CAN2_rx_buf, sizeof(CAN2_rx_buf));  //初始化CAN2接收缓冲区
			CAN2_rx_en();  //使能CAN接收

			temp = 25;
			while(temp--)
			{
				bms_xgb_send_chm();
				if(os_sem_wait(&CAN2_rx_sem, 250) != OS_R_TMO)  //收到数据
				{
					while(1)
					{
						block = b_fifo_get(CAN2_rx_buf);  //读取数据
						if(block != NULL)
						{
							BMS_rx_msg = (CanRxMsg*)block->data;
							if(get_pgn(BMS_rx_msg->ExtId) == 0x27)  //收到回应
							{
								bms_info.protocol = BMS_PROTOCOL_XGB;
								bms_info.event = &bms_xgb_event;
								BMS_MSG("BMS XGB\n");
								b_fifo_del(CAN2_rx_buf);  //删除数据
								goto start;
							}
							b_fifo_del(CAN2_rx_buf);  //删除数据
						}
						else
						{
							break;
						}
					}
				}
			}

			io_bms_12v_dis();
			os_dly_wait(5000);
			io_bms_12v_en();
		}

		/***************  国标协议  **************/

		CAN2_rx_dis();
		os_sem_init(&CAN2_rx_sem, 0);
		b_fifo_init(CAN2_rx_buf, sizeof(CAN2_rx_buf));  //初始化CAN2接收缓冲区
		CAN2_rx_en();  //使能CAN接收

		temp = 25;
		while(temp--)
		{
			bms_gb_send_crm(0);
			if(os_sem_wait(&CAN2_rx_sem, 250) != OS_R_TMO)  //收到数据
			{
				while(1)
				{
					block = b_fifo_get(CAN2_rx_buf);  //读取数据
					if(block != NULL)
					{
						BMS_rx_msg = (CanRxMsg*)block->data;
						if(get_pgn(BMS_rx_msg->ExtId) == PGN_60416)    //收到回应
						{
							bms_info.protocol = BMS_PROTOCOL_GB;
							bms_info.event = &bms_gb_event;
							BMS_MSG("BMS GB\n");
							b_fifo_del(CAN2_rx_buf);  //删除数据
							goto start;
						}
						b_fifo_del(CAN2_rx_buf);  //删除数据
					}
					else
					{
						break;
					}
				}
			}
		}



		/****************************************/
start:

		if(bms_info.protocol == 0)  //检测失败
		{
			io_bms_24v_dis();
			io_bms_12v_dis();
			if(v24 == 0)
			{
				v24 = 1;
				CAN2_rx_dis();
				os_dly_wait(5000);
				goto restart;
			}
			bms_test = 2;
			stop_charge = 0xff;
			continue;  //检测失败
		}
		else
		{
			bms_test = 1;  //检测成功
		}

		/******************************************************************************************************/

		io_bms_dc_out_en();  //先吸合继电器   检测电池电压

		bms_info.v12_v24 = v24;

		TIM_Cmd(TIM4, ENABLE);

reshk:  //重新握手

		bms_info.event->init();  //相应协议初始化

		while(1)
		{
			if(os_sem_wait(&CAN2_rx_sem, 10) != OS_R_TMO)
			{
				bms_info.timeout_cnt = 0;

				block = b_fifo_get(CAN2_rx_buf);

				if(block == NULL)
				{
					continue;
				}

				BMS_rx_msg = (CanRxMsg*)block->data;
				pgn = get_pgn(BMS_rx_msg->ExtId);
				pri = get_pri(BMS_rx_msg->ExtId);
				//BMS_MSG("PGN   %x\n", pgn);


				if(pgn == PGN_60416)  //can 多包(大于8字节)传输协议
				{
					if(pri != 0x1c)
					{
						goto del;
					}
					switch(BMS_rx_msg->Data[0])  //控制字节
					{
					case 0x10:  //请求发送
					{
						TP_CM_RTS_def *rts = (TP_CM_RTS_def*)&BMS_rx_msg->Data[0];
						u32 temp_pgn = 0;
						temp_pgn = rts->pgn[0] << 16;
						temp_pgn |= rts->pgn[1] << 8;
						temp_pgn |= rts->pgn[2];

						//PGN过滤
						if((temp_pgn == 0x000200) || (temp_pgn == 0x000600) || (temp_pgn == 0x001100) || (temp_pgn == 0x001500) || (temp_pgn == 0x001600) || (temp_pgn == 0x001700))
						{
							mult_pkg_info.len = rts->msg_lenth;
							mult_pkg_info.pkg_cnt = rts->pkg_cnt;
							mult_pkg_info.received_len = 0;
							mult_pkg_info.recv_pkg_id = 1;  //分包id从1开始
							mult_pkg_info.pgn[0] = rts->pgn[0];
							mult_pkg_info.pgn[1] = rts->pgn[1];
							mult_pkg_info.pgn[2] = rts->pgn[2];
							mult_pkg_info.pkg_rx_en = 0;
							if(mult_pkg_info.len <= MULT_PKG_LEN)  //有足够的存储空间
							{
								tp_send_cts(rts->pkg_cnt, 1, rts->pgn);  //准备接收BMS多包数据
								mult_pkg_info.pkg_rx_en = 1;  //全能接收
							}
						}
					}
					break;

					case 0xFF:  //放弃连接
						mult_pkg_info.pkg_rx_en = 0;
						break;

					default:
						break;
					}
				}
				else if((pgn == PGN_60160) && (mult_pkg_info.pkg_rx_en))  //多包数据帧
				{
					u16 len = mult_pkg_info.len - mult_pkg_info.received_len;  //剩余长度

					if(pri != 0x1c)  //检查报文优先级
					{
						goto del;
					}

					if(mult_pkg_info.recv_pkg_id != BMS_rx_msg->Data[0])  //检查分包id   若不是期望的id则丢弃
					{
						goto del;
					}

					if(mult_pkg_info.recv_pkg_id > mult_pkg_info.pkg_cnt)  //检查分包id合法性
					{
						goto del;
					}

					if(len > 7)
					{
						memcpy(&mult_pkg_info.buf[mult_pkg_info.received_len], &BMS_rx_msg->Data[1], 7);  //第一个字节为ID 后面7字节数据
						mult_pkg_info.received_len += 7;

						mult_pkg_info.recv_pkg_id++;  //接收下一个分包
					}
					else  //最后一个包
					{
						memcpy(&mult_pkg_info.buf[mult_pkg_info.received_len], &BMS_rx_msg->Data[1], len);
						mult_pkg_info.received_len += len;

						tp_send_EndofMsgAck();  //多包数据接收完成  通知发送者

						mult_pkg_info.pkg_rx_en = 0;  //接收完成

						bms_info.event->msg_handler(mult_pkg_info.pgn[1], mult_pkg_info.buf, mult_pkg_info.received_len);  //调用相应协议报文处理函数
					}
				}
				else  //其它单包报文
				{
					switch (pgn)
					//检查报文优先级
					{
					case PGN_9984: //BHM
					case PGN_4096: //BCL
					case PGN_7168: //BSD
					case PGN_4864: //BSM
						if (pri != 0x18)  //优先权6
						{
							goto del;
							//删除报文
						}
						break;

					case PGN_4352: //BCS
					case PGN_5376: //BMV
					case PGN_5632: //BMT
					case PGN_5888: //BSP
						if (pri != 0x1C)  //优先权7
						{
							goto del;
							//删除报文
						}
						break;

					case PGN_2304: //BRO
					case PGN_6400: //BST
						if (pri != 0x10)  //优先权4
						{
							goto del;
							//删除报文
						}
						break;

					case PGN_7680: //BEM
						if (pri != 0x08)   //优先权2
						{
							goto del;
							//删除报文
						}
						break;
					}
					bms_info.event->msg_handler(get_pgn(BMS_rx_msg->ExtId), BMS_rx_msg->Data, BMS_rx_msg->DLC);  //调用相应协议报文处理函数
				}
del:			b_fifo_del(CAN2_rx_buf);  //把处理完数据包从FIFO中删除
			}

			{
				u8 ret = bms_info.event->poll();
				if(ret == 0xff)  //
				{
					bms_reset = 0;
					stop_charge = 1;  //充电完成
					goto end;  //结束充电
				}
				else if(ret == 1)  //而要切换到24V辅助电源
				{
					BMS_MSG("bms 24\n");
					io_bms_12v_dis();  //断开辅助电源
					CAN2_rx_dis();
					os_sem_init(&CAN2_rx_sem, 0);
					b_fifo_init(CAN2_rx_buf, sizeof(CAN2_rx_buf));  //初始化CAN2接收缓冲区
					os_dly_wait(5000);
					io_bms_24v_en();  //开24V辅助电源
					bms_info.v12_v24 = 1;
					CAN2_rx_en();  //使能CAN接收
					goto reshk;  //重新握手通讯
				}
			}

			os_dly_wait(5);
		}

end:
		evc_out_onoff(0);  //关闭充电模块输出
		CAN2_rx_dis();  //停止接收报文
		TIM_Cmd(TIM4, DISABLE);  //关定时器
		temp = 0;
		while(evc_info.read.i > 100)  //小于1A才断开输出继电器
		{
			evc_out_onoff(0);  //关闭充电模块输出
			os_dly_wait(500);
			temp++;
			if(temp == 10)
			{
				break;
			}
		}

		io_bms_dc_out_dis();
		io_bms_24v_dis();  //关闭辅助电源
		io_bms_12v_dis();
		stop_charge = 0xff;  //充电完成
	}
}



