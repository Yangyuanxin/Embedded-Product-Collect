#include "sys.h"
#include "usart.h"	
#include "RS485.h"	


__IO uint16_t Rcv_Length = 0;
__IO uint16_t Rcv_Time_Ms = 0;
uint8_t CmdCode = 0;

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	RS485_Send_En();
	__nop();
	__nop();
	__nop();
	__nop();
	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;     

	__nop();
	__nop();
	__nop();
	__nop();
	RS485_Recv_En();	
	
	return ch;
}
#endif

//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound)
	{
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

	USART_Cmd(USART1, DISABLE);  //关闭串口1 
		
   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		USART_RX_BUF[Rcv_Length] = Res;
			
			if(USART_RX_STA == 0)
			{
				if(USART_RX_BUF[0] == HEAD_CODE)
				{
					USART_RX_STA = 0x01;
					Rcv_Length++;
				}
				else
				{
					Rcv_Length = 0;
				}
			}
			else
			{
					Rcv_Length++;
					
					Rcv_Time_Ms = 0;			
			}  		 
     }
} 	
void USART_Timer_Ms(void)
{
	if(USART_RX_STA == 0x01)
	{
		Rcv_Time_Ms++;
		if(Rcv_Time_Ms > 10)
		{
			USART_RX_STA = 0x02;
			Rcv_Time_Ms = 0;
		}
	}
	else
	{
		Rcv_Time_Ms = 0;
	}
}
void USART_Send_Char(uint8_t ch)
{
	RS485_Send_En();
	__nop();
	__nop();
	__nop();
	__nop();	
	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART_SendData(USART1,ch);
	
	__nop();
	__nop();
	__nop();
	__nop();
	RS485_Recv_En();
}
void USART_Send_Buf(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	
	for(i=0;i<len;i++)
	{
		USART_Send_Char(buf[i]);
//		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
//		USART_SendData(USART1,buf[i]);
	}
}
void USART_Send_String(char *s)
{
	while(*s)
	{
		USART_Send_Char(*s++);
	}
}



