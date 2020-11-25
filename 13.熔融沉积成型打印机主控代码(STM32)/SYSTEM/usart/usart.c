#include "usart.h"


//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound)
{
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	#if EN_USART1_RX		  //如果使能了接收  
	NVIC_InitTypeDef NVIC_InitStructure;
	#endif
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);		//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10

   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口

#if EN_USART1_RX		  //如果使能了接收  
   //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
   
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
#endif
  USART_Cmd(USART1, ENABLE);                    //使能串口 
}

ring_buffer rx_buffer  =  { { 0 }, 0, 0 };

void store_char(unsigned char c)  //将接收到的数据存入缓冲区
{
  int i = (unsigned int)(rx_buffer.head + 1) % RX_BUFFER_SIZE;
  // if we should be storing the received character into the location  //如果我们应该存储的接收到的字符的位置刚好
  // just before the tail (meaning that the head would advance to the  //在尾端的前面（意味着头部将要进入尾端的
  // current location of the tail), we're about to overflow the buffer //当前位置），这样将会溢出缓冲区，因此我们
  // and so we don't write the character or advance the head.          //不该存入这个字符或使这个头前进
  if (i != rx_buffer.tail)  //缓冲区没有存满 
	{
    rx_buffer.buffer[rx_buffer.head] = c;
    rx_buffer.head = i;
  }
}

void checkRx(void)  //检测接收到的数据
{ 
	unsigned char c;
	unsigned int i;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{ 
		c = USART_ReceiveData(USART1);
		i = (unsigned int)(rx_buffer.head + 1) % RX_BUFFER_SIZE;
		// if we should be storing the received character into the location
		// just before the tail (meaning that the head would advance to the
		// current location of the tail), we're about to overflow the buffer
		// and so we don't write the character or advance the head.
		if (i != rx_buffer.tail) 
		{
			rx_buffer.buffer[rx_buffer.head] = c;
			rx_buffer.head = i;
		}
	}
}

unsigned int MYSERIAL_available(void)  //返回串口缓存区中数据的个数
{
   return (unsigned int)(RX_BUFFER_SIZE + rx_buffer.head - rx_buffer.tail) % RX_BUFFER_SIZE;
}

uint8_t MYSERIAL_read(void)  //按存入顺序逐个读取缓冲区的数据
{ 
	uint8_t c;
  // if the head isn't ahead of the tail, we don't have any characters //如果头不是在尾的前面，将收不到任何字符
  if (rx_buffer.head == rx_buffer.tail) 
	{
    return 0;
  } 
	else 
	{
    c = rx_buffer.buffer[rx_buffer.tail];
    rx_buffer.tail = (unsigned int)(rx_buffer.tail + 1) % RX_BUFFER_SIZE;
    return c;
  }
}
void MYSERIAL_flush(void)  //清空串口数据
{
  // RX
	// don't reverse this or there may be problems if the RX interrupt  //不要颠倒这个否则可能会有一些问题，如果接收中断
  // occurs after reading the value of rx_buffer_head but before writing  //发生在读取rx_buffer_head之后但在写入rx_buffer_tail之前
  // the value to rx_buffer_tail; the previous value of rx_buffer_head  //之前的rx_buffer_head值可能被写到rx_buffer_tail
  // may be written to rx_buffer_tail, making it appear as if the buffer  //使它呈现缓冲区是满的而非空的状态
  // were full, not empty.
  rx_buffer.head = rx_buffer.tail;
}		

//选择不同的方式支持printf函数，要包含stdio.h头文件

#if 1   //不需要选择使用MicroLIB	，就可支持printf函数

#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}

#elif 0   //使用microLib的方法支持printf函数
  
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}

#endif  

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	uint8_t rec;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d //0x0a结尾)
	{
		rec = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		store_char(rec);  		//将读取到的数据存入缓存区 
  } 
} 

#endif

