

#include "head.h"


#define _UART_C

#define RECE_BUF_LEN	256
#define SEND_BUF_LEN	256

typedef struct 
{
	UINT8 send_buffer[SEND_BUF_LEN];	//FIFO
	UINT8 rece_buffer[RECE_BUF_LEN];	//FIFO
	
	UINT8 send_head;				
	UINT8 send_end;
	
	UINT8 rece_head;
	UINT8 rece_end;
	
	UINT8 state;					//OPEN OR CLOSE
	UINT8 transmiting_flag;			//正在发送标志
}PORT;

PORT com[PORT_MAX];						//定义串口的资源


//串口配置
void UART_Configuration (void)  
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;						

	/* Enable USART clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3 
		                   |RCC_APB1Periph_UART4|RCC_APB1Periph_UART5, ENABLE);

	/* Configure USARTx_Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USARTx_Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	//USART1_TX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1_RX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	  	//USART2_TX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//USART2_RX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_TX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//USART3_RX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//USART4_TX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//USART4_RX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//USART_InitStructure.USART_Clock = USART_Clock_Disable;
	//USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	//USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	//USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	/* Configure the USARTx */ 
	USART_Init(USART1, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(USART1, USART_IT_TC);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(USART1, ENABLE);       
  
	//UART2------------------------------------------------------------------------------------
	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//USART_InitStructure.USART_Clock = USART_Clock_Disable;
	//USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	//USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	//USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	/* Configure the USARTx */ 
	USART_Init(USART2, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(USART2, ENABLE); 

	//UART3------------------------------------------------------------------------------------
	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//USART_InitStructure.USART_Clock = USART_Clock_Disable;
	//USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	//USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	//USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	/* Configure the USARTx */ 
	USART_Init(USART3, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(USART3, USART_IT_TC);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART3, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(USART3, ENABLE); 


	//UART4------------------------------------------------------------------------------------
	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(UART4, USART_IT_TC);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(UART4, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(UART4, ENABLE);

	//UART5------------------------------------------------------------------------------------
	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(UART5, USART_IT_TC);
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(UART5, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(UART5, ENABLE);
        
    open_port(COM1,9600,1,8,1,1);
    open_port(COM2,9600,1,8,1,1);
    open_port(COM3,9600,1,8,1,1);
	open_port(COM4,9600,1,8,1,1);
    open_port(COM5,9600,1,8,1,1);
}

/****************************************************************************************
** 函数名称: USART_SendChar
** 功能描述: UART1 发送字符
** 参    数: USARTx: 串口号,  Data: 待发送数据
** 返 回 值: None       
** 作　  者: 罗辉联
** 日  　期: 2008年1月7日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void USART_SendChar(USART_TypeDef* USARTx, u8* Data)
{
  /* Transmit Data */
  USARTx->DR = (Data[0] & (u16)0x01FF);

  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  
}



  
//UART DRIVER FOR APP BY GUOHUI 20090708
//---------------------------------------------------------------------------


UINT8 open_port(UINT8 port,UINT32 baud,UINT8 startbit,UINT8 databit,UINT8 stopbit,UINT8 parity)
{	
	//非法性检查	
	if (( port > (PORT_MAX-1) ) || (baud > 230400) || (databit > 8))
	{
		return FALSE;
	}
	if (com[port].state == TRUE)		//端口已经打开则直接返回
	{
		return FALSE;
	}
	
	
	//
	com[port].send_head = 0;
	com[port].send_end  = 0;
	com[port].rece_head = 0;
	com[port].rece_end  = 0;
	
	com[port].transmiting_flag = FALSE;

	if (port == COM1)					//打开串口0
	{		
			 	
	}
	else if (port == COM2)
	{
			
	}
		
	com[port].state = OPEN;
	
	//
	
	//
	return TRUE;
	
}
/*
数据发送函数，应用层调用此函数把数据放入发送FIFO缓冲区中
数据的发送，靠UART的中断进行。
*/
UINT8 send_data(UINT8 port,UINT8 *pdata, UINT8 length)
{
	UINT8 i;
	if (port > PORT_MAX)				//非法性检查
	{
		return 0;
	}
//	send_str(0,pdata);
	
	if (   (com[port].state == CLOSE)
		|| (length == 0) )
		//|| ((com[port].send_head + 1) == com[port].send_end) )	//发送缓冲区满.
	{
		return 0;
	} 
	
	for (i=0; i<length; i++)
	{
		com[port].send_buffer[(com[port].send_head++)] = pdata[i];
		if (com[port].send_head == com[port].send_end)
		{
			com[port].send_end++; 
		}
	}	
	
	if (com[port].transmiting_flag == 0)	
	{
		write_data(port);
		
		com[port].transmiting_flag = 1;
	}
	   	
	
	return i;
	
}

/**********************************************************************************************
函数名：UINT8 send_str(UINT8 port,UINT8 *pstr)
功能：API函数.应用层调用发送字符串的函数.
入口参数：串口号,字符串指针
出口参数：实际发送的字节数.
-------------------
编写日期:2005-09-30.GUOHUI
-------------------
***********************************************************************************************/
UINT8 send_str(UINT8 port,UINT8 *pstr)
{
	UINT8 i=0;
	
	while(pstr[i++] != '\0');
	
	return (send_data(port,pstr,i-1));	
}

/**********************************************************************************************
函数名：UINT8 get_uart_state(UINT8 port)
功能：API函数.应用层调用检查当前串口状态的函数.
入口参数：串口号
出口参数：串口当前状态，关闭或者是打开
-------------------
编写日期:2005-09-30.GUOHUI
-------------------
***********************************************************************************************/
UINT8 get_uart_state(UINT8 port)
{
	if (port > PORT_MAX)	
	{
		return CLOSE;
	}	
	return (com[port].state);
} 

/**********************************************************************************************
函数名：UINT8 get_rec_len(UINT8 port)
功能：API函数.应用层调用.得到底层接收缓冲区的可用字节数。
入口参数：串口号
出口参数：返回有多少个字节可用
-------------------
编写日期:2005-09-30.GUOHUI
-------------------
***********************************************************************************************/
UINT8 get_rec_len(UINT8 port)
{
	UINT8 temp;
	//
	if (port > PORT_MAX)	
	{
		return 0;
	}
	if (com[port].state == CLOSE)
	{
		return 0;
	}	
	
	temp = com[port].rece_head - com[port].rece_end;
	
	return temp;
	
}

/**********************************************************************************************
函数名：UINT8 procure_data(UINT8 * Desp,UINT8 port,UINT8 len)
功能：	API函数。应用层调用获得底层数据的函数。
入口参数：目的缓冲区首址，端口号，接收长度
出口参数：成功接收的字节数
-------------------
编写日期:2005-10-16.GUOHUI
------------------- 
注意：一般情况下，此函数应和UINT8 get_rec_len(UINT8 port)函数配套使用。
***********************************************************************************************/
UINT8 procure_data(UINT8 * Desp,UINT8 port,UINT8 len)
{
	UINT8 i;
	//非法性检查
	if ( (port > PORT_MAX) || (len == 0) )
	{
		return FALSE;
	}

	i = get_rec_len(port);

	if (i)
	{
		if (i < len) 
		{
			len = i;
		}
		for (i=0; i<len; i++)
		{
			*Desp++ = com[port].rece_buffer[com[port].rece_end++];
		}
	}
	
	return len;
	
}


/*
中断调用的函数，写硬件的发送寄存器
*/
UINT8 write_data(UINT8 port)
{	  
	
	if (com[port].send_head == com[port].send_end)	//如果没有数据可以发送
	{	
		if (port == COM1)
		{
			USART_ITConfig(USART1, USART_IT_TC, DISABLE);
		}
		else if (port == COM2)
		{
			USART_ITConfig(USART2, USART_IT_TC, DISABLE);
		}
		else if(port == COM3)
		{
			USART_ITConfig(USART3, USART_IT_TC, DISABLE);
		}
		else if(port == COM4)
		{
			USART_ITConfig(UART4, USART_IT_TC, DISABLE);
		}
		else if(port == COM5)
		{
			USART_ITConfig(UART5, USART_IT_TC, DISABLE);
		}

		com[port].transmiting_flag = 0;

		return FALSE;		
	}

	if (port == COM1)
	{
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	}
	else if (port == COM2)
	{
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	}
	else if (port == COM3)
	{
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
	}
	else if (port == COM4)
	{
		USART_ITConfig(UART4, USART_IT_TC, ENABLE);
	}
	else if (port == COM5)
	{
		USART_ITConfig(UART5, USART_IT_TC, ENABLE);
	}

	//
	
	//
	if (port == COM1)
	{
		USART_SendData(USART1, com[port].send_buffer[(com[port].send_end++)]);
	}
	else if (port == COM2)
	{		
		USART_SendData(USART2, com[port].send_buffer[(com[port].send_end++)]);

	}
	else if (port == COM3)
	{
		USART_SendData(USART3, com[port].send_buffer[(com[port].send_end++)]);
	}
	else if (port == COM4)
	{		
		USART_SendData(UART4, com[port].send_buffer[(com[port].send_end++)]);

	}
	else if (port == COM5)
	{
		USART_SendData(UART5, com[port].send_buffer[(com[port].send_end++)]);
	}


	//
	if (com[port].send_end == com[port].send_head)
	{		
		//com[port].transmiting_flag = FALSE;	
		
	}
	//
	
	//
	return TRUE;
	
}

void read_data(UINT8 port)
{  		
	//------------------------------------------------
	if (((com[port].rece_head+1) & 0xff) == com[port].rece_end)	//接收缓冲区已满,则返回
	{
		com[port].rece_end++;			
		//break;		
	}
	
	//------------------------------------------------
	if (port == COM1)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(USART1);
	}
	else if (port == COM2)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(USART2);
	}
	//-------------------------------------------------	 
	else if (port == COM3)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(USART3);
	}
	else if (port == COM4)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(UART4);
	}
	//-------------------------------------------------	 
	else if (port == COM5)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(UART5);
	}
}

