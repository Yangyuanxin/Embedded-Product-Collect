#include "usart_2.h"
#include "includes.h"
u16 AT_RX_STA=0;                   //AT模式接收状态标志,B15:接收完成标志;B14:接收到'O',B13；
u16 DATA_RX_STA=0;                   //透传模式接收转态标志，
u8 Rxd_DataBuf[70];
u8 data_len = 0;			//数据长度
u16	OK_Sta = 0;
u8 SEND_AT_FLAG = 1;
/*
* 函数名称：Usart_2_Init；
* 功能说明：Usart2初始化；
* 入口参数：pclk1->PCLK1时钟频率最高36，bound->波特率；
* 返回值  ：无；
* 备注    ：无；
*/
void Usart_1_Init(u32 pclk1,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);   //得到USARTDIV
	mantissa=temp;                            //得到整数部分
	fraction=(temp-mantissa)*16;              //得到小数部分	 
	mantissa<<=4;
	mantissa+=fraction; 

	RCC->AHB1ENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APB2ENR|=1<<4;  	//使能串口1时钟 

	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	RCC->APB2RSTR|=1<<4;                     //复位；
	RCC->APB2RSTR&=~(1<<4);                  //停止复位；
	//波特率设置
 	USART1->BRR=mantissa; 	//波特率设置	 
	USART1->CR1&=~(1<<15); 	//设置OVER8=0 
	USART1->CR1|=1<<3;  	//串口发送使能 
	//使能接收中断 
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR1|=1<<5;    	//接收缓冲区非空中断使能	    	
	MY_NVIC_Init(3,3,USART1_IRQn,2);//组2，最低优先级 
	USART1->CR1|=1<<13;  	//串口使能
	
}
/*
* 函数名称：Usart_2_Send；
* 功能说明：Usart2串口发送；
* 入口参数：data->发送的数据；
* 返回值  ：无；
* 备注    ：无；
*/
void Usart_1_Send(u8 data)
{
	while((USART1->SR&0X40)==0);      //循环发送,直到发送完毕   
	USART1->DR = (u8)data;
}


void Set_Next(u8 x,u8 y)
{
//	UT_RX_STA |=  (1<<y);	//下一位置1
//	UT_RX_STA &= ~(1<<x);	//本位清0
}
/*
* 函数名称：USART2_IRQHandler；
* 功能说明：Usart2接收中断服务函数；
* 入口参数：无；
* 返回值  ：无；
* 备注    ：AT模式下以"OK"为结束标志，未加入检查到错误标志；
*           透传模式
*/
void USART1_IRQHandler(void)
{
	u8 rxd;	
	OSIntEnter();   
	if(USART1->SR&(1<<5))                                //接收到数据,清除标志位；
	{	
		rxd = USART1->DR;  						 //获取数据
		if(SEND_AT_FLAG == 0) 					//AT模式获取OK数据
		{                              
			AT_RX_STA++;						//通过他来判断O与K的间隔
			if((rxd == 'o')||(rxd == 'O'))
			{
				AT_RX_STA |= (1<<12);			//第十二位为 接收到O的标志位
				OK_Sta = AT_RX_STA;				//接到0
			}		
			else
			{
				if((rxd == 'k')||(rxd == 'K'))					//判断是否接到K
				{
					if(AT_RX_STA == (OK_Sta+1))	//OK是连续接收到的
					{
						AT_RX_STA |= 0x8000;	//成功标志	
						AT_RX_STA &= 0x8000;	//除了成功位其他都清0
						OK_Sta = 0;
					}
					else						//OK不是连续接收到的
					{
						AT_RX_STA = 0;			//技数位清0
					}
				}
			}
		}
		else
		{
			if((DATA_RX_STA & 0x8000) != 0)			//数据接收到了
			{
				Rxd_DataBuf[data_len]=rxd;			
				data_len++;
				if((rxd == '\r') || (rxd == '}'))		//数据接收结束
				{
					DATA_RX_STA = 0;
					DATA_RX_STA |=  0x4000;	//数据接收成功标志位
					data_len = 0;
				}
			}
			else
			{
				if((rxd == '=')|| (rxd == '{'))	//=后的都是数据
				{
					DATA_RX_STA = 0; 
					DATA_RX_STA |= 0x8000 ;		//开始接收数据 
				}
			}
		}
		USART1->SR&= ~(1<<5);
	}
	OSIntExit();  
}
	
