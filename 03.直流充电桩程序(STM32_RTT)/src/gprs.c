#include "RTL.h"
#include "stm32f10x.h"
#include "string.h"
#include "dbg.h"
#include "rtc.h"
#include "server.h"
#include "gprs.h"

#if 1
#define GPRS_MSG dbg_msg
#define GPRS_ERROR dbg_error
#else
#define GPRS_MSG (void)
#define GPRS_ERROR (void)
#endif



#define GPRS_RX_BUF_LEN 100
u8 uart4_rx_buf[GPRS_RX_BUF_LEN];  //UART4接收缓存
u8 uart4_rx_len;

//信号定义   分别为 接收到数据   数据发送完成    CRC校验成功
OS_SEM uart4_rx_sem, uart4_tx_sem;

#define DMA2_CH3_EN()  DMA2_Channel3->CCR |= DMA_CCR1_EN  //开DMA
#define DMA2_CH5_EN()  DMA2_Channel5->CCR |= DMA_CCR1_EN;	//开DMA
#define DMA2_CH3_DIS() DMA2_Channel3->CCR &= (u16)(~DMA_CCR1_EN)  //关DMA
#define DMA2_CH5_DIS() DMA2_Channel5->CCR &= (u16)(~DMA_CCR1_EN);  //关DMA

void DMA2_CH3_RX_EN(void);

static void uart4_rx_dis(void);
static void uart4_rx_en(void);
/*
 * @brief  UART4空闲中断处理程序
 */
void UART4_IRQHandler()
{
	u32 temp;
	u16 len;

	uart4_rx_dis();  //关闭接收

	//清USART_IT_IDLE标志
    temp = UART4->SR;
    temp = UART4->DR;
    uart4_rx_len = GPRS_RX_BUF_LEN - DMA2_Channel3->CNDTR;  //本次收到的数据长度

    isr_sem_send(&uart4_rx_sem);
}


/*
 * @brief  DMA2_CH5发送完成中断处理程序
 */
void DMA2_Channel5_IRQHandler()
{
	//DMA_ClearITPendingBit(DMA2_IT_TC5);
	DMA2->IFCR = DMA2_IT_TC5;
	DMA2_CH5_DIS();
	isr_sem_send(&uart4_tx_sem);
}


/*
 * @brief  使能uart4接收
 */
static void uart4_rx_en()
{
	DMA2_Channel3->CNDTR = GPRS_RX_BUF_LEN;
	DMA2_CH3_EN();
	UART4->CR1 |= USART_Mode_Rx;  //使能接收
}

/*
 * @brief  关闭usart1接收
 */
static void uart4_rx_dis()
{
	UART4->CR1 &= ~USART_Mode_Rx;  //关闭接收
	DMA2_CH3_DIS();  //关DMA
}

/*
 * @brief  UART4接收初始化
 */
void UART4_rx_init()
{
	//UART4 DMA 接收数据地址设置
	DMA2_Channel3->CNDTR = GPRS_RX_BUF_LEN;
	DMA2_Channel3->CMAR = (u32)uart4_rx_buf;
	DMA2_CH3_EN();
}



u8 uart4_send_buf[50];
/**
  * @brief  UART4发送  超时时间50ms
  * @param  buf: 数据地址
  * @param  lenth: 数据长度
  * @retval 1 发送成功   0 失败
  */
static int UART4_send(u8* buf, u8 lenth)
{
	if(os_sem_wait(&uart4_tx_sem, 10) != OS_R_TMO)
	{
		DMA2_CH5_DIS();  //关DMA
		//LCD_MSG("-----------\n");
	}

	DMA2_Channel5->CMAR = (u32)buf;
	DMA2_Channel5->CNDTR = lenth;

	uart4_rx_en();  //使能接收

	DMA2_CH5_EN();

	return 0;
}



static int enter_at()
{
	uart4_send_buf[0] = '+';
	uart4_send_buf[1] = '+';
	uart4_send_buf[2] = '+';
	UART4_send(uart4_send_buf, 3);
	if(os_sem_wait(&uart4_rx_sem, 100) != OS_R_TMO)
	{
		if((uart4_rx_len == 1) && (uart4_rx_buf[0] == 'a'))
		{
			uart4_send_buf[0] = 'a';
			UART4_send(uart4_send_buf, 1);
			if(os_sem_wait(&uart4_rx_sem, 100) != OS_R_TMO)
			{
				if((uart4_rx_len == 7) && (uart4_rx_buf[2] == '+') && (uart4_rx_buf[3] == 'o') && (uart4_rx_buf[4] == 'k'))
				{
					return 0;
				}
			}
		}
	}
	return -1;
}


static int cmd_at(u8* str)
{
	u8 len = strlen(str);
	memcpy(uart4_send_buf, str, len);
	UART4_send(uart4_send_buf, len);
	if(os_sem_wait(&uart4_rx_sem, 100) != OS_R_TMO)
	{
		if((uart4_rx_len == 6) && (uart4_rx_buf[2] == 'O') && (uart4_rx_buf[2] == 'K'))
		{
			return 0;
		}
	}
	return -1;
}


static int enter_entm()
{
	memcpy(uart4_send_buf, "AT+ENTM\n\r", 9);
	UART4_send(uart4_send_buf, 9);
	if(os_sem_wait(&uart4_rx_sem, 100) != OS_R_TMO)
	{
		if((uart4_rx_len == 6) && (uart4_rx_buf[2] == 'O') && (uart4_rx_buf[2] == 'K'))
		{
			return 0;
		}
	}
	return -1;
}


#define AT_CIPCFG "AT+CIPCFG=1,0,0,10,0,0\n\r"
#define AT_CIPPACK0 "AT+CIPPACK=0,,0\n\r"
#define AT_CIPPACK1 "AT+CIPPACK=1,,0\n\r"
#define AT_CIPSCONT "AT+CIPSCONT=1,UDP,103.224.249.102, 7777,1\n\r"
#define AT_CICOMAT "AT+CICOMAT=1\n\r"
#define AT_CINETAT "AT+CINETAT=0\n\r"
#define AT_CIMOD "AT+CIMOD=0\n\r"
#define AT_CSTT "AT+CSTT=UNINET,,\n\r"
#define AT_ATW "ATW\n\r"

static int gprs_cfg()
{
	int ret;
	ret = cmd_at(AT_CIPCFG);    //配置心跳包时间、串口打包时间等参数
	if(!ret)
		return -1;
	ret = cmd_at(AT_CIPPACK0);  //配置心跳包内容
	if(!ret)
		return -1;
	ret = cmd_at(AT_CIPPACK1);  //配置注册包内容
	if(!ret)
		return -1;
	ret = cmd_at(AT_CIPSCONT);  //配置连接信息
	if(!ret)
		return -1;
	ret = cmd_at(AT_CICOMAT);   //启用透传模式下串口 AT 指令功能
	if(!ret)
		return -1;
	ret = cmd_at(AT_CINETAT);   //禁用网络 AT 指令功能
	if(!ret)
		return -1;
	ret = cmd_at(AT_CSTT);      //配置模块工作模式  透传
	if(!ret)
		return -1;
	ret = cmd_at(AT_ATW);       //保存设置
	if(!ret)
		return -1;

	return 0;
}


int gprs_send(u8 *buf, u16 len)
{
	return UART4_send(buf, len);
}


/*
 * @brief  GPRS驱动任务
 */
void task_gprs()
{
	int i;
	os_sem_init(&uart4_rx_sem, 0);
	os_sem_init(&uart4_tx_sem, 0);

	UART4_rx_init();

	os_dly_wait(5000);

	uart4_rx_en();

	while(1)
	{
//		if(!enter_at())
//		{
//			gprs_cfg();
//		}

		if(os_sem_wait(&uart4_rx_sem, 5000) != OS_R_TMO)
		{
			if(server_handler.recv_callback != NULL)
			{
				server_handler.recv_callback(uart4_rx_buf, uart4_rx_len);  //收到服务器数据
			}
		}

		//os_dly_wait(0xffff);
//		os_sem_wait(&rx_sem, 0xffff);
//
//		for(i = 0; i < uart4_rx_len; i++)
//		{
//			GPRS_MSG("%02x  ", uart4_rx_buf[i]);
//		}
//
//
//		//GPRS_MSG("\n%s\n", uart4_rx_buf);
//		uart4_rx_en();

	}
}


///*
// * @brief          把页面的只写变量数据发到DGUS屏中
// * @param page_id  页面编号
// */
//int dgus_write_page(u8 page_id)
//{
//	u16 crc;
//	u8  err_cnt = 3;
//
//	page_t* page = UI_data[page_id];
//
//	send_buf[2] = 5 + page->wo_len;
//	send_buf[3] = CMD_W_VAR;
//	send_buf[4] = page_id;  //页面ID
//	send_buf[5] = 0;
//
//	memcpy(send_buf + 6, page->wo, page->wo_len);
//	crc = dgus_crc16(send_buf + 3, page->wo_len + 3);
//	crc = HTONS(crc);
//	*(u16*)&send_buf[page->wo_len + 6] = crc;  //填充CRC
//
//	//LCD_MSG("--- %d\n", page->wo_lenth);
//
//	do
//	{
//		UART4_send(send_buf, page->wo_len + 8);  //发送数据
//
//		if(os_sem_wait(&rx_sem, WAIT_TIME) != OS_R_TMO) //CRC校验成功
//		{
//			return 1;
//		}
//	}while(--err_cnt);
//
//	GPRS_ERROR("lcd Write_page error! id:%d\n", page_id);
//	return 0;
//}

