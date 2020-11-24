#include "stm32f10x.h"
#include "RTL.h"
#include "dbg.h"
#include <string.h>
#include "sys_cfg.h"
#include "rtc.h"

#if 1
#define IC_MSG dbg_msg
#define IC_ERROR dbg_error
#else
#define IC_MSG (void)
#define IC_ERROR (void)
#endif

#define IC_RX_BUF_LEN 30
u8 ic_rx_buf[IC_RX_BUF_LEN];  //接收数据地址
u8 ic_rx_len, ic_tx_len;
u8 *ic_tx_buf;

OS_SEM ic_rx_sem, ic_tx_sem;

static void uart5_rx_dis(void);

void UART5_IRQHandler()
{
	u32 temp;

	if(USART_GetITStatus(UART5, USART_IT_RXNE))
	{
		if(ic_rx_len < IC_RX_BUF_LEN)
		{
			ic_rx_buf[ic_rx_len] = UART5->DR;
		}
		ic_rx_len++;
	}
	else if(USART_GetITStatus(UART5, USART_IT_IDLE))
	{
		uart5_rx_dis();
		USART_ITConfig(UART5,USART_IT_RXNE,DISABLE);
		isr_sem_send(&ic_rx_sem);
	}
	else if(USART_GetITStatus(UART5, USART_IT_TXE))
	{
		if(ic_tx_len > 0)
		{
			UART5->DR = *ic_tx_buf++;
			ic_tx_len--;
		}
		else
		{
			USART_ITConfig(UART5,USART_IT_TXE,DISABLE);
			USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
			//isr_sem_send(&ic_tx_sem);
		}
	}

	//清标志
    temp = UART5->SR;
    temp = UART5->DR;
}

/*
 * @brief  使能uart5接收
 */
static void uart5_rx_en()
{
	UART5->CR1 |= USART_Mode_Rx;  //使能接收
}

/*
 * @brief  关闭uart5接收
 */
static void uart5_rx_dis()
{
	UART5->CR1 &= ~USART_Mode_Rx;  //关闭接收
}

/**
  * @brief  UART5发送
  * @param  buf: 数据地址
  * @param  lenth: 数据长度
  * @retval 1 发送成功   0 失败
  */
static int uart5_send(u8* buf, u8 lenth)
{
	ic_rx_len = 0;
	ic_tx_buf = buf + 1;
	ic_tx_len = lenth - 1;

	uart5_rx_en();  //使能接收
	UART5->DR = *buf;  //发送一个字节
	USART_ITConfig(UART5,USART_IT_TXE,ENABLE);
	//os_sem_wait(&ic_tx_sem, 0xffff);
	return 1;
}

/*
 * @brief  UART5接收初始化
 */
void ic_rx_init()
{
	ic_rx_len = 0;
}











void ic_usart_send(u8* buf, u8 lenth)
{
	uart5_send(buf, lenth);
}


/*
 * @brief  计算校验
 * @param  buf 数据
 * @param  len 数据字节数
 * @retval 校验结果
 */
u8 verify(u8 *buf, u8 len)
{
    u8 i, BBC = 0;
    for (i = 0; i < len; i++)
    {
        BBC ^= buf[i];
    }
    return (u8)BBC;
}


/*
 * @brief  等待读卡器返回数据
 * @retval 1 成功   0 失败
 */
int ic_wait_frame()
{
	if(os_sem_wait(&ic_rx_sem, 1000) != OS_R_TMO)
	{
		if((ic_rx_buf[0] == 0x02) && (ic_rx_buf[ic_rx_len - 1] == 0x03) && (verify(ic_rx_buf, ic_rx_len - 1) == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  检测刷卡器连接
 * @retval 1 成功   0 失败
 */
int ic_link_isok()
{
	u8 temp[10];
	u8 cnt = 3;

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x05;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x40;
	temp[5] = 0x00;  //等待时间
	temp[6] = 0x00;
	temp[7] = 0x00;
	temp[8] = verify(temp, 8);
	temp[9] = 0x03;  //END

	do
	{
		ic_usart_send(temp, 10);

		if(ic_wait_frame())
		{
			//收到数据
			return 1;
		}
		IC_MSG("--\n");
		cnt--;
	}while(cnt);
	return 0;
}


/*
 * @brief  搜索IC卡
 * @param  id 读到的IC卡号
 * @retval 0 成功   >0失败
 */
int ic_find(u32 *id)
{
	u8 temp[10];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x05;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x40;
	temp[5] = 0x00;  //等待时间
	temp[6] = 0x00;
	temp[7] = 0x00;
	temp[8] = verify(temp, 8);
	temp[9] = 0x03;  //END

	ic_usart_send(temp, 10);

	if(ic_wait_frame())
	{
		//收到数据  校验成功
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))  //找到IC卡
		{
			*id = *(u32*)&ic_rx_buf[7];  //ID号
			return 1;
		}
	}
	return 0;
}

/*
 * @brief  验证密钥
 * @param  ab  0x00密钥A  0x04密钥B
 * @param  sector  扇区号
 * @retval 1 成功   0 失败
 */
int ic_auth_keyAB(u8 ab, u8 sector)
{
	u8 temp[10];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x05;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x41;
	temp[5] = ab;    //密钥模式
	temp[6] = sector; //扇区号
	temp[7] = 0x00;
	temp[8] = verify(temp, 8);
	temp[9] = 0x03;  //END


	ic_usart_send(temp, 10);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  验证密钥
 * @param  sector  扇区号
 * @retval 1 成功   0 失败
 */
int ic_auth_key(u8 sector)
{
	return ic_auth_keyAB(0x04, sector);
}


const u8 key_a[6] = {0x50, 0x61, 0x77, 0x61, 0x72, 0x41};
const u8 key_b[6] = {0x50, 0x61, 0x77, 0x61, 0x72, 0x42};

/*
 * @brief  下载密钥到读卡器
 * @param  ab  0x00密钥A  0x04密钥B
 * @param  sector  扇区号
 * @retval 1 成功   0 失败
 */
int ic_download_key(u8 ab, u8* key, u8 sector)
{
	u8 temp[15];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x0A;
	temp[3] = 0xC0;  //命令
	temp[4] = 0x16;
	temp[5] = ab;    //密钥模式
	temp[6] = sector; //扇区号
	memcpy(&temp[7], key, 6);
	temp[13] = verify(temp, 13);
	temp[14] = 0x03;  //END


	ic_usart_send(temp, 15);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}





/*
 * @brief  读一块数据  16字节
 * @param  id IC卡号
 * @param  block  块号
 * @param  数据
 * @retval 1 成功   0 失败
 */
int ic_read_block(u8 block, u8 *buf)
{
	u8 temp[8];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x03;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x43;
	temp[5] = block; //扇区号
	temp[6] = verify(temp, 6);
	temp[7] = 0x03;  //END

	ic_usart_send(temp, 8);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			memcpy(buf, &ic_rx_buf[5], 16);
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  写一块数据  16字节
 * @param  id IC卡号
 * @param  block  块号
 * @param  数据
 * @retval 1 成功   0 失败
 */
int ic_write_block(u8 block, u8 *buf)
{
	u8 temp[24];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x13;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x44;
	temp[5] = block; //扇区号
	memcpy(&temp[6], buf, 16);
	temp[22] = verify(temp, 22);
	temp[23] = 0x03;  //END


	ic_usart_send(temp, 24);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}

/*
 * @brief  初始化值
 * @param  block  块号
 * @param  数据
 * @retval 1 成功   0 失败
 */
int ic_init_value(u8 block, u32 value)
{
	u8 temp[12];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x07;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x45;
	temp[5] = block; //扇区号
	temp[6] = (u8)(value >> 24);
	temp[7] = (u8)(value >> 16);
	temp[8] = (u8)(value >> 8);
	temp[9] = (u8)value;
	temp[10] = verify(temp, 10);
	temp[11] = 0x03;  //END

	ic_usart_send(temp, 12);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  读值
 * @param  block  块号
 * @param  value  数据
 * @retval 1 成功   0 失败
 */
int ic_read_value(u8 block, u32* value)
{
	u8 temp[8];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x03;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x46;
	temp[5] = block; //扇区号
	temp[6] = verify(temp, 6);
	temp[7] = 0x03;  //END


	ic_usart_send(temp, 8);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			*value = ic_rx_buf[5] << 24;
			*value |= ic_rx_buf[6] << 16;
			*value |= ic_rx_buf[7] << 8;
			*value |= ic_rx_buf[8];
			return 1;
		}
	}
	return 0;
}

/*
 * @brief  增值
 * @param  block  块号
 * @param  value  数据
 * @retval 1 成功   0 失败
 */
int ic_add_value(u8 block, u32 value)
{
	u8 temp[12];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x07;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x47;
	temp[5] = block; //扇区号
	temp[6] = (u8)(value >> 24);
	temp[7] = (u8)(value >> 16);
	temp[8] = (u8)(value >> 8);
	temp[9] = (u8)value;
	temp[10] = verify(temp, 10);
	temp[11] = 0x03;  //END


	ic_usart_send(temp, 12);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  减值
 * @param  block  块号
 * @param  value  数据
 * @retval 1 成功   0 失败
 */
int ic_sub_value(u8 block, u32 value)
{
	u8 temp[12];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x07;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x48;
	temp[5] = block; //扇区号
	temp[6] = (u8)(value >> 24);
	temp[7] = (u8)(value >> 16);
	temp[8] = (u8)(value >> 8);
	temp[9] = (u8)value;
	temp[10] = verify(temp, 10);
	temp[11] = 0x03;  //END


	ic_usart_send(temp, 12);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  停止卡
 * @retval 1 成功   0 失败
 */
int ic_halt()
{
	u8 temp[7];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //数据区长度
	temp[2] = 0x02;
	temp[3] = 0xC1;  //命令
	temp[4] = 0x4A;
	temp[5] = verify(temp, 5);
	temp[6] = 0x03;  //END


	ic_usart_send(temp, 7);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}





/*
 * @brief  读取余额
 * @retval 0 成功  <0 失败
 */
int ic_read_sum(u32 *sum)
{
	if(ic_auth_key(0))  //校验密钥
	{
		if(ic_read_value(1, sum))  //读取余额
		{
			return 0;
		}
	}
	return -1;
}


/*
 * @brief  扣费
 * @param  sum  扣费
 * @retval 0 成功  <0 失败
 */
int ic_pay(u32 sum)
{
	if(ic_auth_key(0))  //校验密钥
	{
		if(ic_sub_value(1, sum))  //扣费
		{
			return 0;
		}
	}
	return -1;
}

/*
 * @brief  判断IC卡是否锁定
 * @retval 1 已锁定   0 未锁定    <0失败
 */
int ic_is_locked()
{
	if(ic_auth_key(1))  //校验密钥
	{
		u8 temp[16];
		if(ic_read_block(4, temp) && (temp[0] != 0))  //块4第一字节不为0则为锁定状态
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return -1;
}


/*
 * @brief  锁定IC卡
 * @param  t  当前时间
 * @retval 0 成功  <0 失败
 */
int ic_lock_card(time_t *t)
{
	if(ic_auth_key(1))  //校验密钥
	{
		u8 temp[16];
		temp[0] = 0xff;  //标记锁定
		temp[1] = t->year;
		temp[2] = t->year >> 8;
		temp[3] = t->mon;
		temp[4] = t->day;
		temp[5] = t->hour;
		temp[6] = t->min;
		temp[7] = t->sec;
		memset(&temp[8], 0, 8);
		if(ic_write_block(4, temp))
		{
			return 0;
		}
		else
		{
			return -2;
		}
	}
	return -1;
}


/*
 * @brief  锁定IC卡
 * @param  t  当前时间
 * @retval 0 成功  <0 失败
 */
int ic_unlock_card(time_t *t)
{
	if(ic_auth_key(1))  //校验密钥
	{
		u8 temp[16];
		temp[0] = 0x00;  //清除锁定标记
		temp[1] = t->year;
		temp[2] = t->year >> 8;
		temp[3] = t->mon;
		temp[4] = t->day;
		temp[5] = t->hour;
		temp[6] = t->min;
		temp[7] = t->sec;
		memset(&temp[8], 0, 8);
		if(ic_write_block(4, temp))
		{
			return 0;
		}
		else
		{
			return -2;
		}
	}
	return -1;
}


/*
 * @brief  初始化
 */
void ic_init()
{
	//初始化信号量
	os_sem_init(&ic_rx_sem, 0);
	os_sem_init(&ic_tx_sem, 1);
	ic_rx_init();

	if(sys_cfg.ic_down_pswd != 0xAAAAAAAA)
	{
		if(ic_download_key(0x00, (u8*)key_a, 0) &&
		   ic_download_key(0x00, (u8*)key_a, 1) &&
		   ic_download_key(0x04, (u8*)key_b, 0) &&
		   ic_download_key(0x04, (u8*)key_b, 1))
		{
			sys_cfg.ic_down_pswd = 0xAAAAAAAA;  //标记已下载
			sys_cfg_save();
		}
	}
}


void task_ic()
{
	u8 cnt = 3;
	u32 value;
	//初始化信号量
	os_sem_init(&ic_rx_sem, 0);
	os_sem_init(&ic_tx_sem, 1);

	ic_rx_init();


//	if(sys_cfg.ic_down_pswd == 0xffffffff)  //没有下载过密钥
//	{
//		do
//		{
//			if(ic_download_key(0x00, 0))
//			{
//				sys_cfg.ic_down_pswd = 0;  //标记已经下载过
//				sys_cfg_save();
//				break;  //成功
//			}
//			cnt--;
//		}while(cnt);
//
//		if(cnt == 0)
//		{
//			//通讯失败
//			IC_ERROR("ic download key error!\n");
//			os_dly_wait(1000);
//		}
//	}

	while(1)
	{
		os_dly_wait(0xffff);
	}
}
