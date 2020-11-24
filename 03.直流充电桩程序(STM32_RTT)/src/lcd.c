#include "RTL.h"
#include "stm32f10x.h"
#include "string.h"
#include "lcd.h"
#include "dbg.h"
#include "rtc.h"
#include "em.h"

//说明  DGUS中所有数据都是大端格式

#if 1
#define LCD_MSG dbg_msg
#define LCD_ERROR dbg_error
#else
#define LCD_MSG (void)
#define LCD_ERROR (void)
#endif





#define DGUS_HEADER 0xA55A

//DGUS屏通讯数据结构
typedef struct
{
	u16 header;    //帧头
	u8  f_lenth;   //帧长度 (指令+数据+CRC(如果有))
	u8  cmd;       //指令
	u8  data[1];      //数据
}dgus_fh;

typedef struct
{
	u16 var_addr;  //变量地址
	u8  data[1];      //数据
}var_w_fh;

typedef struct
{
	u16 var_addr;  //变量地址
	u8  data_len;  //数据长度
	u8  data[1];      //数据
}var_r_fh;

typedef struct
{
	u8  reg_addr;   //寄存器地址
	u8  data[1];      //数据
}rge_w_fh;

typedef struct
{
	u8  reg_addr;   //寄存器地址
	u8  data_len;  //数据长度
	u8  data[1];      //数据
}reg_r_fh;


typedef struct
{
	u16* var;                      //变量
	u8   var_lenth;                //var数组大小  单位字节
}page_var_t;


#define CMD_W_REG 0x80
#define CMD_R_REG 0x81
#define CMD_W_VAR 0x82
#define CMD_R_VAR 0x83







//等待DGUS屏返回数据的超时时间  单位ms
#define WAIT_TIME 100


#define LCD_RX_BUF_LEN 60
u8 lcd_rx_buf[LCD_RX_BUF_LEN];  //UART4接收缓存
u8 lcd_rx_len;

u8 dgus_buf[LCD_RX_BUF_LEN];
u8 dgus_buf_len;
//信号定义   分别为 接收到数据   数据发送完成    CRC校验成功
OS_SEM lcd_rx_sem, lcd_tx_sem, lcd_crc_ret_sem;

OS_SEM em_rx_sem;  //电表接收到数据

#define DMA1_CH4_EN()  DMA1_Channel4->CCR |= DMA_CCR1_EN  //开DMA
#define DMA1_CH5_EN()  DMA1_Channel5->CCR |= DMA_CCR1_EN;	//开DMA
#define DMA1_CH4_DIS() DMA1_Channel4->CCR &= (u16)(~DMA_CCR1_EN)  //关DMA
#define DMA1_CH5_DIS() DMA1_Channel5->CCR &= (u16)(~DMA_CCR1_EN);  //关DMA

void DMA2_CH3_RX_EN(void);

static void usart1_rx_dis(void);
static void usart1_rx_en(void);
unsigned short dgus_crc16(unsigned char* buf, unsigned short len);
/*
 * @brief  UART4空闲中断处理程序
 */
void USART1_IRQHandler()
{
	u32 temp;
	u16 len;
	dgus_fh *fh;
	//VAR_R_FH *var_r_fh;

	//int i;

	usart1_rx_dis();  //关闭接收

	//清USART_IT_IDLE标志
    temp = USART1->SR;
    temp = USART1->DR;

    fh = (dgus_fh*)lcd_rx_buf;
    len = LCD_RX_BUF_LEN - DMA1_Channel5->CNDTR;  //本次收到的数据长度

    if(lcd_rx_buf[0] == 1)  //电表modbu从机地址  定要设置为1
    {
    	lcd_rx_len = len;
    	isr_sem_send(&em_rx_sem);  //电表modbus收到数据
    }
    else if(fh->header == DGUS_HEADER)  //判断帧头
    {
    	if((len == 7) && (fh->f_lenth == 2))  //CRC应答帧长为7   f_lenth固定长度为2
    	{
			temp = *((u16*)&lcd_rx_buf[5]);  //当前帧CRC
			temp = HTONS(temp);
			if((temp == dgus_crc16(&lcd_rx_buf[3], 2)) && (lcd_rx_buf[4] == 0xff))  //返回正确的CRC校验应答  说明上次发送的数据正确接收
			{
				isr_sem_send(&lcd_crc_ret_sem);
			}
			else
			{
				LCD_MSG("crc error\n");
			}

			usart1_rx_en();
    	}
    	else  //其它帧
    	{
    		lcd_rx_len = len;

    		isr_sem_send(&lcd_rx_sem);
    	}
    }
    else
    {
    	int i;
    	LCD_ERROR("lcd error Frame\n");

    	for(i = 0; i < len; i++)
    	{
    		dbg_msg("%2x ", lcd_rx_buf[i]);
    	}
    	LCD_MSG("\n");

    	usart1_rx_en();
    }

    if(lcd_rx_buf[0] != 1)  //电表modbus地址
    {
    	fh->header = 0;
    }
}


/*
 * @brief  DMA1_CH4发送完成中断处理程序
 */
void DMA1_Channel4_IRQHandler()
{
	DMA1->IFCR = DMA1_IT_TC4;  //清标志
	DMA1_CH4_DIS();  //关发送DMA
	isr_sem_send(&lcd_tx_sem);
}


/*
 * @brief  使能uart1接收
 */
static void usart1_rx_en()
{
	DMA1_Channel5->CNDTR = LCD_RX_BUF_LEN;
	DMA1_CH5_EN();
	USART1->CR1 |= USART_Mode_Rx;  //使能接收
}

/*
 * @brief  关闭usart1接收
 */
static void usart1_rx_dis()
{
	USART1->CR1 &= ~USART_Mode_Rx;  //关闭接收
	DMA1_CH5_DIS();  //关接收DMA
}

/*
 * @brief  UART1接收初始化
 */
void lcd_rx_init()
{
	DMA1_Channel5->CNDTR = LCD_RX_BUF_LEN;
	DMA1_Channel5->CMAR = (u32)lcd_rx_buf;
}




/**
  * @brief  电表modbus等待一数据帧
  * @param  addr: 数据地址
  * @param  len: 数据长度
  * @retval 1 发送成功   0 失败
  */
int em_wait_frame(u8** addr, u16* len)
{
	if(os_sem_wait(&em_rx_sem, 200) != OS_R_TMO)
	{
		*len = lcd_rx_len;
		*addr = lcd_rx_buf;
		return 1;
	}
	return 0;
}

/**
  * @brief  电表modbus发送数据
  * @param  buf: 数据地址
  * @param  lenth: 数据长度
  * @retval 1 发送成功   0 失败
  */
void em_usart_send(u8* buf, u8 lenth)
{
	if(os_sem_wait(&lcd_tx_sem, 50) == OS_R_TMO)
	{
		DMA1_CH4_DIS();  //关DMA
	}

	DMA1_Channel4->CMAR = (u32)buf;
	DMA1_Channel4->CNDTR = lenth;

	os_sem_init(&em_rx_sem, 0);
	usart1_rx_en();  //使能接收

	DMA1_CH4_EN();  //发送
}








/**
  * @brief  发送  超时时间50ms
  * @param  buf: 数据地址
  * @param  lenth: 数据长度
  * @retval 1 发送成功   0 失败
  */
static int lcd_usart_send(u8* buf, u8 lenth)
{
	if(os_sem_wait(&lcd_tx_sem, 50) == OS_R_TMO)
	{
		DMA1_CH4_DIS();  //关DMA
		//LCD_MSG("-----------\n");
	}

	DMA1_Channel4->CMAR = (u32)buf;
	DMA1_Channel4->CNDTR = lenth;

	usart1_rx_en();  //使能接收

	DMA1_CH4_EN();  //发送

	return 0;
}

void dgus_switch_page(u8 page_id);
void ui_process(var_r_fh* _fh);
void Task_UpdatePAGE (void);
void update_page(void);

extern page_t* UI_data[];           //页面定义   lcd_ui.c
//LCD页面刷新时间 单位1ms
u32 poll_time = 0xffff;


/*
 * @brief  LCD驱动任务
 */
void task_lcd()
{
	u32 ret;
	dgus_fh *fh = (dgus_fh*)dgus_buf;
	var_r_fh *_var_r_fh;

	os_sem_init(&em_rx_sem, 0);

	os_sem_init(&lcd_rx_sem, 0);
	os_sem_init(&lcd_tx_sem, 1);
	os_sem_init(&lcd_crc_ret_sem, 0);

	poll_time = UI_data[0]->poll_time;  //logo页面

	lcd_rx_init();

	usart1_rx_en();

	em_read_kwh(&em_info.kwh_cnt);

	os_dly_wait(500);

	dgus_switch_page(0);

	while(1)
	{
		ret = os_sem_wait(&lcd_rx_sem, poll_time);   //等待按键命令

		if(ret == OS_R_TMO)                          //超时
		{
			update_page();                             //刷新显示
		}
		else                                         //收到
		{
			memcpy(dgus_buf, lcd_rx_buf, lcd_rx_len);  //保存数据

			if(dgus_crc16(&fh->cmd, fh->f_lenth - 2) != HTONS(*(u16*)&fh->data[fh->f_lenth -3]))  //CRC 错误
			{
				usart1_rx_en();                          //重新使能接收
				continue;
			}

			_var_r_fh = (var_r_fh*)fh->data;

			_var_r_fh->var_addr = HTONS(_var_r_fh->var_addr);  //转换为小端

			ui_process(_var_r_fh);			
		}
		usart1_rx_en();  //重新使能接收
	}
}


















//当前显示的页面ID号
static u8 Current_page;

u8 send_buf[200] =  {(u8)DGUS_HEADER, (u8)(DGUS_HEADER >> 8)};

/*
 * @brief      读取寄存器
 * @param reg  寄存器编号
 * @param buf  数据地址
 * @param len  数据长度
 */

static int dgus_read_reg(u8 reg, u8 *buf, u8 len)
{
	u16 crc;
	u8  err_cnt = 3;

	send_buf[2] = 5;  //长度
	send_buf[3] = CMD_R_REG;
	send_buf[4] = reg;  //地址
	send_buf[5] = len;
	crc = dgus_crc16(&send_buf[3], 3);  //计算CRC
	*(u16*)&send_buf[6] = HTONS(crc);

	do
	{
		lcd_usart_send(send_buf, 8);

		if (os_sem_wait(&lcd_rx_sem, WAIT_TIME) != OS_R_TMO) //等待返回数据
		{
			//这里返回的数据包括两个数据帧  第一个为发送给屏的读命令的CRC应答帧   后面一帧为数据帧
			//当有数据返回时说明发送的命令是正确的 可以把CRC应答帧忽略  只处理后面的数据帧

			reg_r_fh *temp = (reg_r_fh*)&lcd_rx_buf[11];
			crc = *(u16*)&lcd_rx_buf[lcd_rx_len - 2];
			crc = HTONS(crc);
			if(crc == dgus_crc16(&lcd_rx_buf[10], lcd_rx_len - 12))
			{
				if(temp->reg_addr == reg)
				{
					memcpy(buf, temp->data, len); //保存变量数据
					return 1;
				}
			}
		}
	}while(--err_cnt);

	LCD_ERROR("lcd Read_reg error!\n");
	return 0;
}


/*
 * @brief      写寄存器
 * @param reg  寄存器编号
 * @param buf  数据地址
 * @param len  数据长度
 */

static int dgus_write_reg(u8 reg, u8 *buf, u8 len)
{
	u16 crc;
	u8  err_cnt = 3;

	send_buf[2] = 2 + len + 2;  //长度
	send_buf[3] = CMD_W_REG;
	send_buf[4] = reg;  //地址
	memcpy(&send_buf[5], buf, len);
	crc = dgus_crc16(&send_buf[3], 2 + len);  //计算CRC
	*(u16*)&send_buf[5 + len] = HTONS(crc);

	do
	{
		lcd_usart_send(send_buf, 7 + len);
		if(os_sem_wait(&lcd_crc_ret_sem, WAIT_TIME) != OS_R_TMO)  //CRC校验成功
		{
			return 1;
		}
	}while(--err_cnt);

	LCD_ERROR("lcd Write_reg error!\n");
	return 0;
}


int dgus_write_page(u8 page_id);

static u32 page_poll_cnt;  //从切换到当前页面开始计数轮询次数   poll_handler里可以检测pool_cnt  用于超时检测
/*
 * @brief          切换到指定页面
 * @param page_id  页面编号
 */
void dgus_switch_page(u8 page_id)
{
	page_t* next_page;
	u16 temp = HTONS(page_id);

	page_poll_cnt = 0;  //当前轮询次数清0

	next_page = UI_data[page_id];

	next_page->prev_id = Current_page;  //保存前一级页面id号

	if(next_page->page_init)  //初始化显示数据
	{
		next_page->page_init();
	}

	if(next_page->wo)  //切换之前先更新页面数据
	{
		dgus_write_page(page_id);
	}

	if(dgus_write_reg(0x03, (u8*)&temp, 2))  //切换成功
	{
		Current_page = page_id;
	}
	else
	{
		LCD_ERROR("lcd switch page error\n");
	}

	if(next_page->poll_handler != NULL)  //若页面定义了刷新事件
	{
		poll_time = next_page->poll_time;  //刷新时间

		//next_page->poll_handler(page_poll_cnt);  //切换完刷新一次页面

		if(poll_time == 0)
		{
			poll_time = 0xffff;
		}
	}
	else
	{
		poll_time = 0xffff;  //不刷新
	}
}

/*
 * @brief 触发一个键码  详见0x4F寄存器
 */
void dgus_trigger_key(u8 key)
{
	u8 temp[1];
	temp[0] = key;
	dgus_write_reg(0x4F, temp, 1);
}



/*
 * @brief          显示文本
 * @param addr     变量地址
 * @param code     字符串  中文为GBK
 * @retval 1 成功   0 失败
 */
int dgus_show_text(u16 addr, u8* str)
{
	u16 crc;
	u8  err_cnt = 3;
	u8  len = strlen((const char*)str);

	send_buf[2] = 5 + len + 2;  //在字符串后面加0x0000
	send_buf[3] = CMD_W_VAR;
	send_buf[4] = (u8)(addr >> 8);  //变量地址
	send_buf[5] = (u8)addr;

	memcpy(send_buf + 6, str, len);

	//在字符串后面加0x0000
	send_buf[6 + len] = 0;
	send_buf[6 + len + 1] = 0;
	len += 2;

	crc = dgus_crc16(send_buf + 3, len + 3);
	crc = HTONS(crc);
	*(u16*)&send_buf[len + 6] = crc;  //填充CRC

	//LCD_MSG("--- %d\n", page->wo_lenth);

	do
	{
		lcd_usart_send(send_buf, len + 8);  //发送数据

		if(os_sem_wait(&lcd_crc_ret_sem, WAIT_TIME) != OS_R_TMO) //CRC校验成功
		{
			return 1;
		}
	}while(--err_cnt);

	LCD_ERROR("lcd show_text error!\n");
	return 0;
}


/*
 * @brief          把页面的只写变量数据发到DGUS屏中
 * @param page_id  页面编号
 */
int dgus_write_page(u8 page_id)
{
	u16 crc;
	u8  err_cnt = 3;

	page_t* page = UI_data[page_id];

	send_buf[2] = 5 + page->wo_len;
	send_buf[3] = CMD_W_VAR;
	send_buf[4] = page_id;  //页面ID
	send_buf[5] = 0;

	memcpy(send_buf + 6, page->wo, page->wo_len);
	crc = dgus_crc16(send_buf + 3, page->wo_len + 3);
	crc = HTONS(crc);
	*(u16*)&send_buf[page->wo_len + 6] = crc;  //填充CRC

	//LCD_MSG("--- %d\n", page->wo_lenth);

	do
	{
		lcd_usart_send(send_buf, page->wo_len + 8);  //发送数据

		if(os_sem_wait(&lcd_crc_ret_sem, WAIT_TIME) != OS_R_TMO) //CRC校验成功
		{
			return 1;
		}
	}while(--err_cnt);

	LCD_ERROR("lcd Write_page error! id:%d\n", page_id);
	return 0;
}


/*
 * @brief          初始化页面的只读变量数据  显示默认参数
 * 				      比如在屏幕上有一个输入框  但在切换到这个页面时想显示一个初始值   可以用此函数设置
 * @param page_id  页面编号
 */
int dgus_init_ro_var(u8 page_id)
{
	u16 crc;
	u8  err_cnt = 3;

	page_t* page = UI_data[page_id];

	send_buf[2] = 5 + page->ro_len;
	send_buf[3] = CMD_W_VAR;
	send_buf[4] = page_id;  //页面ID
	send_buf[5] = 0x80;  //只读

	memcpy(send_buf + 6, page->ro, page->ro_len);
	crc = dgus_crc16(send_buf + 3, page->ro_len + 3);
	crc = HTONS(crc);
	*(u16*)&send_buf[page->ro_len + 6] = crc;  //填充CRC

	//LCD_MSG("--- %d\n", page->wo_lenth);

	do
	{
		lcd_usart_send(send_buf, page->ro_len + 8);  //发送数据

		if(os_sem_wait(&lcd_crc_ret_sem, WAIT_TIME) != OS_R_TMO) //CRC校验成功
		{
			return 1;
		}
	}while(--err_cnt);

	LCD_ERROR("lcd Write_page error! id:%d\n", page_id);
	return 0;
}



/*
 * 刷新当前页面变量显示
 */
void dgus_write_current_page()
{
	dgus_write_page(Current_page);
}


/*
 * @brief          从DGUS屏中读取变量数据
 * @param page_id  页面编号
 */

int dgus_read_page(u8 page_id)
{
	u16 temp;
	dgus_fh  *data_fh;
	var_r_fh *var_fh;
	page_t* page = UI_data[page_id];
	u8  err_cnt = 3;

	send_buf[2] = 6;
	send_buf[3] = CMD_R_VAR;
	send_buf[4] = page_id;
	send_buf[5] = 0x80;  //只读
	send_buf[6] = page->ro_len / 2;
	temp = dgus_crc16(send_buf + 3, 4);
	*(u16*)&send_buf[7] = HTONS(temp);

	do
	{
		lcd_usart_send(send_buf, 9);

		if (os_sem_wait(&lcd_rx_sem, WAIT_TIME) != OS_R_TMO) //等待返回数据
		{
			//这里返回的数据包括两个数据帧  第一个为发送给屏的读命令的CRC应答帧   后面一帧为变量数据帧
			//当有数据返回时说明发送的命令是正确的 可以把CRC应答帧忽略  只处理后面的数据帧
			data_fh = (dgus_fh*)&lcd_rx_buf[7];  //忽略CRC应答帧   直接读数据帧
			temp = *((u16*)&data_fh->data[data_fh->f_lenth - 3]);  //数据帧CRC
			temp = HTONS(temp);
			if(temp == dgus_crc16(&lcd_rx_buf[10], data_fh->f_lenth - 2))  //返回正确的CRC校验应答  说明上次发送的数据正确接收
			{
				var_fh = (var_r_fh*)data_fh->data;

				memcpy(page->ro, var_fh->data, page->ro_len); //保存变量数据
				return 1;
			}
		}
	}while(--err_cnt);

	LCD_ERROR("lcd Read_page error! id:%d\n", page_id);
	return 0;
}


/*
 * @brief          写变量地址
 * @param len      长度
 */
int dgus_write_var(u16 addr, u8 len)
{
	u16 crc;
	u8  err_cnt = 3;

	send_buf[2] = 5 + len;
	send_buf[3] = CMD_W_VAR;
	send_buf[4] = (u8)(addr >> 8);
	send_buf[5] = (u8)addr;

	crc = dgus_crc16(send_buf + 3, len + 3);
	crc = HTONS(crc);
	*(u16*)&send_buf[len + 6] = crc;  //填充CRC

	do
	{
		lcd_usart_send(send_buf, len + 8);  //发送数据

		if(os_sem_wait(&lcd_crc_ret_sem, WAIT_TIME) != OS_R_TMO) //CRC校验成功
		{
			return 1;
		}
	}while(--err_cnt);

	LCD_ERROR("lcd Write_var error! addr:%x\n", addr);
	return 0;
}


/*
 * @brief          填充一个矩形
 * @param x1 y1    左上角坐标
 * @param x2 y2    右下角坐标
 * @param color    填充颜色
 */
void dgus_fill_rec(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	send_buf[6] = 0x00;  //矩形域填充指令
	send_buf[7] = 0x04;

	send_buf[8] = 0x00;  //填充一个矩形
	send_buf[9] = 0x01;

	send_buf[10] = (u8)(x1 >> 8);  //左上角坐标x
	send_buf[11] = (u8)x1;
	send_buf[12] = (u8)(y1 >> 8);  //右下角坐标y
	send_buf[13] = (u8)y1;

	send_buf[14] = (u8)(x2 >> 8);  //右下角坐标x
	send_buf[15] = (u8)x2;
	send_buf[16] = (u8)(y2 >> 8);  //右下角坐标y
	send_buf[17] = (u8)y2;

	send_buf[18] = (u8)(color >> 8);  //填充颜色
	send_buf[19] = (u8)color;

	send_buf[20] = 0xff;  //指令结束
	send_buf[21] = 0x00;

	dgus_write_var(0x0020, 16);
}


/*
 * @brief          屏幕亮度设置
 * @param onoff    0~8 级亮度
 */
void dgus_bl_config(u8 bl)
{
	u8 temp = 19 + (bl + 1) * 5;

	dgus_write_reg(0x01, &temp, 1);
}



/*
 * @brief 十进制转十六进制  dec2hex(15) == 0x15
 */
u8 dec2hex(u8 dec)
{
	u8 hex;
	hex = (dec % 10) | ((dec / 10) << 4);
	return hex;
}

/*
 * @brief 十六进制转十进制  hex2dec(0x15) == 15
 */
u8 hex2dec(u8 hex)
{
	u8 dec;
	dec = (hex >> 4) * 10 + (hex & 0x0f);
	return dec;
}

/*
 * @brief          设置rtc
 * @param onoff    0~8 级亮度
 */
void dgus_rtc_config(u8 YY, u8 MM, u8 DD, u8 h, u8 m)
{
	u8 temp[8];
	temp[0] = 0x5A;
	temp[1] = dec2hex(YY);
	temp[2] = dec2hex(MM);
	temp[3] = dec2hex(DD);
	temp[4] = 0;
	temp[5] = dec2hex(h);
	temp[6] = dec2hex(m);
	temp[7] = 0;

	dgus_write_reg(0x1F, temp, 8);
}


/*
 * @brief          读取rtc
 * @param time     TIME_type结构
 * @retval  1:成功  0:失败
 */
int dgus_rtc_read(time_t *time)
{
	dgus_time_t d_time;
	if(dgus_read_reg(0x20, (u8*)&d_time, 7))
	{
		time->year = hex2dec(d_time.YY) + 2000;
		time->mon  = hex2dec(d_time.MM);
		time->day  = hex2dec(d_time.DD);
		time->hour = hex2dec(d_time.h);
		time->min  = hex2dec(d_time.m);
		time->sec  = hex2dec(d_time.s);
		time->week = d_time.WW;
		return 1;
	}
	return 0;
}



/*
 * @brief   获取前一级页面ID  用于从当前页面返回前一级
 * @retval  页面ID
 */
u8 dgus_get_prev_id()
{
	return UI_data[Current_page]->prev_id;
}





/*
 *  UI 处理
 *
 *  _fh->var_addr DGUS屏数据变量地址定义
 *  当_fh->var_addr不为0xffff时_fh->var_addr表示如下:
 *  bit15 ~ bit8 (8bit) ---  page_id
 *  bit7 ~ bit0 (8bit) ----  变量在当前页面偏移  bit7为1时变量为只读  为0时为只写
 *
 *  当_fh->var_addr为0xffff时,表示为按键返回,_fh->data为按键码
 *  _fh->data[0]  为page id  _fh->data[1] 在当前页面偏移
 */
#define PAGE_ID(x)    ((u8)(x >> 8))
#define IS_BUTTON(x)  (x == 0xffff)

static void ui_process(var_r_fh* _fh)
{
	page_t *page;
	u8 next_page_id = 0xff;

	if(_fh->var_addr == 0xffff)  //按键命令
	{
		if(_fh->data[0] != Current_page)
		{
			//TODO 检查page id
			//当接收到两个以上一样的按键命令时，由于处理第一个按键命令时页面可能已经切换，再处理第二个时导致_fh->data[0] != Current_page
			//也有可能是CPU重启了,而DGUS屏没有重启造成 Current_page 与真实显示的page id不相等
			//此时从DGUS屏中读取当前显示的page id

			u16 temp;
			if(dgus_read_reg(0x03, (u8*)&temp, 2))
			{
				Current_page = HTONS(temp);
			}
//			else
//			{
//				return;
//			}
			return;
		}

		page = UI_data[Current_page];

		if(page->button_handler != NULL)
		{
			next_page_id = page->button_handler(_fh->data[1]);  //调用相应页面的按键处理函数
		}

		if(next_page_id == PAGE_ID_INVALID)
		{
			return;  //无需切换页面
		}

		dgus_switch_page(next_page_id);
	}
	else  //数据
	{
		 //  bit15 ~ bit8 (8bit) ---  page_id
		 //  bit7 ~ bit0 (8bit) ----  变量在当前页面偏移  bit7为1时变量为只读  为0时为只写

		page = UI_data[_fh->var_addr >> 8];

		if(page->vchange_handler)
		{
			u32 temp = *((u32*)&_fh->data[0]);

			page->vchange_handler(_fh->var_addr & 0x007f, temp);
		}
	}
}



/*
 * 页面刷新
 */
void update_page (void)
{
	page_t* page;

	page = UI_data[Current_page];

	if (page->poll_handler != NULL)
	{
		u8 next_page_id = page->poll_handler(page_poll_cnt);  //刷新页面显示   比如更新数据
		page_poll_cnt++;  //每调用一次 加1  //切换页面时清0

		if(next_page_id == PAGE_ID_INVALID)
		{
			return;  //无需切换页面
		}
		else
		{
			dgus_switch_page(next_page_id);
		}
	}
}













const unsigned char dgus_auchCRCHi[] =
{0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

const unsigned char dgus_auchCRCLo[] =
{0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40};


unsigned short dgus_crc16(unsigned char* buf, unsigned short len)
{
	unsigned char uchCRCHi = 0xff;
	unsigned char uchCRCLo = 0xff;
	unsigned uIndex;
	while(len--)
	{
		uIndex   = uchCRCHi ^ *buf++;
		uchCRCHi = uchCRCLo ^ dgus_auchCRCHi[uIndex];
		uchCRCLo = dgus_auchCRCLo[uIndex];
	}
	return (unsigned short)(uchCRCHi << 8 | uchCRCLo);
}
