/*
*********************************************************************************************************
*
*	模块名称 : DS18B20 驱动模块(1-wire 数字温度传感器）
*	文件名称 : bsp_ds18b20.c
*	版    本 : V1.0
*	说    明 : 用于H7-TOOL。 采用10us定时中断模式驱动. 可同步采集8通道.
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"

#define DS18B20_IRQ_HANDLE		TIM16_IRQHandler

#define TIM_HARD				TIM16
#define TIM_HARD_IRQn			TIM16_IRQn

#define DQ_DIR_OUTPUT(ch)       EIO_ConfigPort(ch, ES_GPIO_OUT)
#define DQ_DIR_INPUT(ch)        EIO_ConfigPort(ch, ES_GPIO_IN)

#define DQ_0(ch)	            EIO_SetOutLevel(ch, 0)
#define DQ_1(ch)		        EIO_SetOutLevel(ch, 1)

/* 判断DQ输入是否为低 */
#define DQ_IS_LOW(ch)	        (EIO_GetInputLevel(ch) == 0)

/*
    D0  PD14 PA15 PI0    - DIR PH8       CS2    
    D1  PD15 PA8 PH19    - DIR PG8       CS3
    
    D2  PE6  PD0 PB7     - DIR PD9       MOSI_1  (硬件SPI信号)
    D3  PE5  PD1 PH11    - DIR PG10      MISO_1  (硬件SPI信号)
    D4  PE4  PE7 PH12    - DIR PG12      CS1    (软件控制片选)
    D5  PE2  PE8 PI5     - DIR PG7       SCK     (硬件SPI信号)
    D6  PE9  PD3 PA0     - DIR PD10      MOSI_2
    D7  PE10 PI6         - DIR PI1       MISO_2  (第2路MISO) 
    D8  PE11 PD4  PI3    - DIR PG9       MISO_3  (第3路MISO)
    D9  PE12 PD5         - DIR PI12      MISO_4  (第4路MISO)
*/

static float s_DS18B20_TempReg[8];
static uint16_t s_err[8] = {0};
    
static void DS18B20_StartTimerIRQ(void);
static void DS18B20_StopTimerIRQ(void);

static uint8_t s_ch = 0;
static uint16_t s_status = 0;

/*
*********************************************************************************************************
*	函 数 名: DS18B20_ReadTempReg
*	功能说明: 读温度寄存器的值（原始数据）. 循环读，返回上次结果.
*	形    参: 无
*	返 回 值: 0表示失败，1表示OK
*********************************************************************************************************
*/
uint8_t DS18B20_ReadTemp(uint8_t _ch, float *_result)
{
    uint8_t re = 0;
    
    if (_ch > 7)
    {
        return re;
    }
    
    EIO_ConfigPort(_ch, ES_GPIO_OUT);    /* DQ方向输出 */
    
    s_err[_ch] = 1;
    s_ch = _ch;
    s_status = 0;
    
    /* 定时周期10us，频率100KHz */
	DS18B20_StartTimerIRQ();
    
    while (s_status != 10)
    {
        //bsp_Idle();
    }
      
    DS18B20_StopTimerIRQ();
    
    if (s_err[s_ch] == 1)
    {
        re = 0; /* 返回异常温度值 */
    }
    else
    {
        *_result = (float)s_DS18B20_TempReg[s_ch] / 16;
        re = 1;
    }

    return re;
}

/*
*********************************************************************************************************
*	函 数 名: DS18B20_CaculCRC
*	功能说明: CRC校验
*	形    参: _buf: 数据缓冲区
*			  _len : 数据长度
*	返 回 值: 校验值
*********************************************************************************************************
*/
uint8_t DS18B20_CaculCRC(uint8_t *_buf, uint8_t _len)
{
    uint8_t crc = 0, i, j;
	
    for (i = 0; i < _len; i++)
    {
        crc = crc ^ _buf[i];
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x01) 
			{
				crc = (crc >> 1) ^ 0x8C;
			}
            else
			{
				crc >>= 1;
			}
        }
    }
    return crc;
}

/*
*********************************************************************************************************
*	函 数 名: DS18B20_IRQ_HANDLE
*	功能说明: 10us定时中断服务程序，读取DS18B20温度值
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DS18B20_IRQ_HANDLE(void)
{
	static uint16_t s_time = 0;
	static uint16_t s_call_ret = 0;
	static uint16_t s_write_value = 0;
	static uint8_t s_i;
	static uint8_t s_read;
	static uint8_t s_rx_buf[9];
	static uint8_t s_rx_len = 0;
	
	if (TIM_HARD->SR & TIM_IT_UPDATE)
	{
		TIM_HARD->SR = (uint16_t)~TIM_IT_UPDATE;
	}
	
	/*
		复位时序, 见DS18B20 page 15

		首先主机拉低DQ，持续最少 480us
		然后释放DQ，等待DQ被上拉电阻拉高，约 15-60us
		DS18B20 将驱动DQ为低 60-240us， 这个信号叫 presence pulse  (在位脉冲,表示DS18B20准备就绪 可以接受命令)
		如果主机检测到这个低应答信号，表示DS18B20复位成功
	*/
	s_time++;
	switch (s_status)
	{
	//		DS18B20_Reset(_ch);

	//		DS18B20_WriteByte(_ch, 0xcc);	/* 发命令 - 跳过ROM */
	//		DS18B20_WriteByte(_ch,0x44);	/* 发转换命令 */

	//		DS18B20_Reset(_ch);		/* 总线复位 */

	//		DS18B20_WriteByte(_ch, 0xcc);	/* 发命令 */
	//		DS18B20_WriteByte(_ch,0xbe);	/* 读温度 */

	//		temp1 = DS18B20_ReadByte(_ch);	/* 读温度值低字节 */
	//		temp2 = DS18B20_ReadByte(_ch);	/* 读温度值高字节 */

	//		return ((temp2 << 8) | temp1);	/* 返回16位寄存器值 */	
	
	case 0:
		s_status++;
		break;
	
	case 1:
		s_call_ret = s_status + 1;      /* 执行完毕返回状态 */
		s_status = 100;		            /* 	去执行DS18B20_Reset */
		break;

	case 2:
		s_call_ret = s_status + 1;
		s_write_value = 0xcc;
		s_status = 110;		/* DS18B20_WriteByte(_ch, 0xcc); */
		break;

	case 3:
		s_call_ret = s_status + 1;
		s_write_value = 0x44;
		s_status = 110;		/* DS18B20_WriteByte(_ch,0x44); */
		break;	
	
	case 4:
		s_call_ret = s_status + 1;
		s_status = 100;		/* 	DS18B20_Reset */
		break;	
	
	case 5:
		s_call_ret = s_status + 1;
		s_write_value = 0xcc;
		s_status = 110;		/* DS18B20_WriteByte(_ch, 0xcc); */
		break;

	case 6:
		s_call_ret = s_status + 1;
		s_write_value = 0xBE;
		s_status = 110;		/* DS18B20_WriteByte(_ch,0xbe); */
		break;	
	
	case 7:
		s_call_ret = s_status + 1;
		s_status = 120;		/* DS18B20_ReadByte(_ch); 读温度值 */
		break;	

	case 8:
		if (DS18B20_CaculCRC(s_rx_buf, 8) == s_rx_buf[8] && s_rx_buf[4] == 0x7F)
		{
            int16_t reg16;
            
            reg16 = (s_rx_buf[1] << 8) + s_rx_buf[0];
			s_DS18B20_TempReg[s_ch] = reg16;
            
            s_err[s_ch] = 0;		/* 读取成功清0 */
		}
        else
        {
            s_err[s_ch] = 1;
        }
        s_status++;
		break;
		
	case 9:
		/* 任务结束，关闭定时中断 */
		DS18B20_StopTimerIRQ();
		s_status++;
		break;
    
	case 10:    /* 执行一次结束,再此处等待 */
		;
		break;    

	case 99:	/* 异常处理 */
		s_err[s_ch] = 1;		/* 错误 */
        s_status = 9;	        /* 结束任务 */	
		break;	
		
	/************ DS18B20_Reset 总线复位子程序 ************/
	case 100:
        DQ_DIR_OUTPUT(s_ch);    /* DQ方向输出 */
		DQ_0(s_ch);				/* 拉低DQ */
		s_time = 0;
		s_status++;
		break;

	case 101:				    /* 延迟 520uS， 要求这个延迟大于 480us */
		if (s_time >= 52)
		{
            DQ_DIR_INPUT(s_ch);     /* DQ方向输入, 外部上拉会拉到高 */
			//DQ_1(s_ch);			    /* 释放DQ */
			
			s_time = 0;
			s_status++;
		}
		break;

	case 102:						/* 延时60us，等待总线电平归位 */
		if (s_time >= 6)	
		{
			s_time = 0;
			s_status++;
		}
		if (!DQ_IS_LOW(s_ch))	    /* 总线已经拉高 */	
		{
			s_time = 0;
			s_status++;
		}
		break;		
	
	case 103:						/* 等待DS18B20拉低总线 */
		if (DQ_IS_LOW(s_ch))		
		{
			s_time = 0;
			s_status++;
		}
		if (s_time > 8)			    /* 80us 没检测到应答，则认为DS18B20异常 */
		{
			s_status = 99;		    /* 错误处理 */
		}
		break;
		
	case 104:						/* 等待DS18B20释放总线 */
		if (!DQ_IS_LOW(s_ch))	
		{
			s_time = 0;
			s_status++;
		}
		if (s_time > 30)		/* 300us 没释放，则认为DS18B20异常 */
		{
			s_status = 99;		/* 错误处理 */
		}
		break;		

	case 105:						/* 复位成功，下面可以开始读数据了 */
		s_status = s_call_ret;
		break;	

	/************ DS18B20_WriteByte ************/
	//	for (i = 0; i < 8; i++)
	//	{
	//		DQ_0(_ch);
	//		bsp_DelayUS(2);

	//		if (_val & 0x01)
	//		{
	//			DQ_1(_ch);
	//		}
	//		else
	//		{
	//			DQ_0(_ch);
	//		}
	//		bsp_DelayUS(60);
	//		DQ_1(_ch);
	//		bsp_DelayUS(2);
	//		_val >>= 1;
	//	}	
	case 110:
		s_i = 0;
		s_status++;
		break;
	
	case 110 + 1:
        DQ_DIR_OUTPUT(s_ch);    /* DQ方向输出 */
		DQ_0(s_ch);			    /* 拉低DQ */
		bsp_DelayUS(2);
		if (s_write_value & 0x01)
		{
			DQ_1(s_ch);
		}
		else
		{
			DQ_0(s_ch);
		}
		s_time = 0;
		s_status++;
		break;	
	
	case 110 + 2:
		if (s_time >= 6)
		{
			DQ_1(s_ch);				
			s_write_value >>= 1;
			
			if (++s_i >= 8)
			{
				s_status++;		
			}
			else
			{
				s_status--;		
			}
		}
		break;
	
	case 110 + 3:
		s_status = s_call_ret;	
		break;
	
	/************ DS18B20_ReadByte ************/
	//	uint8_t i;
	//	uint8_t read = 0;

	//	for (i = 0; i < 8; i++)
	//	{
	//		read >>= 1;

	//		DQ_0(_ch);
	//		bsp_DelayUS(3);
	//		DQ_1(_ch);
	//		bsp_DelayUS(3);

	//		if (DQ_IS_LOW(_ch))
	//		{
	//			;
	//		}
	//		else
	//		{
	//			read |= 0x80;
	//		}
	//		bsp_DelayUS(60);
	//	}
	//	return read;
	
	case 120:
		s_rx_len = 0;
		s_status++;	
		break;
	
	case 120 + 1:
		s_i = 0;
		s_read = 0;
		s_status++;	
		break;

	case 120 + 2:
		s_read >>= 1;
        DQ_DIR_OUTPUT(s_ch);        /* DQ方向输出 */
		DQ_0(s_ch);
		bsp_DelayUS(3);
		//DQ_1(s_ch);
        DQ_DIR_INPUT(s_ch);         /* DQ方向输入 */
		bsp_DelayUS(3);
		if (DQ_IS_LOW(s_ch))
		{
			;
		}
		else
		{
			s_read |= 0x80;
		}		
		s_time = 0;
		s_status++;		
		break;
		
	case 120 + 3:
		if (s_time >= 6)		/* 延迟60us */
		{
			if (++s_i >= 8)
			{
				s_status++;	
			}
			else
			{
				s_status--;	
			}			
		}
		break;

	case 120 + 4:
		s_rx_buf[s_rx_len] = s_read;
		if (++s_rx_len >= 9)
		{
			s_status++;
		}
		else
		{
			s_status = 120 + 1;
		}
		break;
		
	case 120 + 5:
		//s_err[s_ch] = 0;		/* 读取成功清0 */
		s_status = s_call_ret;
		break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: DS18B20_StartTimerIRQ
*	功能说明: 启动TIM定时中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DS18B20_StartTimerIRQ(void)
{
	bsp_SetTIMforInt(TIM_HARD, 100*1000, 0, 0);
}

/*
*********************************************************************************************************
*	函 数 名: DS18B20_StopTimerIRQ
*	功能说明: 关闭TIM定时中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DS18B20_StopTimerIRQ(void)
{
	bsp_SetTIMforInt(TIM_HARD, 0, 0, 0);
}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/

