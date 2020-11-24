/**********************************************************
						作者：神秘藏宝室
						店铺：ILoveMCU.taobao.com

	本例程仅适用于在本店购买模块的用户使用，鄙视其它店铺的盗用行为
	版权所有，盗版必究！！！
	SIM800 GSM/GPRS模块
	https://item.taobao.com/item.htm?id=521599424969
	测试使用STM32最小系统：
	https://item.taobao.com/item.htm?id=523336310868
	STC12C5A60S2最小系统：
	https://item.taobao.com/item.htm?id=524701473371
	USB-TTL模块：
	https://item.taobao.com/item.htm?id=39481188174
	移动电源锂电池套装
	https://item.taobao.com/item.htm?id=530904849115
	移动场合供电用锂电池套装：
	https://item.taobao.com/item.htm?id=530904849115
	
	接线说明：
	STM32					SIM800C
	3.3V	------>	V_MCU
	GND		<----->	GND
	PA3		<------	T_TX
	PA2		------>	T_RX
	

	//用于调试可不接
	STM32					USB-TTL模块
	GND		------>	GND
	TX1		------>	RXD
***********************************************************/

#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "usart2.h"
#include "timer.h"
//#include "iostream"
//using namespace std;


//常量
#define Success 1U
#define Failure 0U

//定义变量
unsigned long  Time_Cont = 0;       //定时器计数器
unsigned int count = 0;

char phoneNumber[] = "1771922XXXX";		//替换成需要被拨打电话的号码
char msg[] = "ILoveMCU.taobao.com";		//短信内容	

char TCPServer[] = "47.106.156.87";		//TCP服务器地址
char Port[] = "80";						//端口	


void errorLog(int num);
//void phone(char *number);
unsigned int sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry);
unsigned int sendCommand_(char *Command, char *Response, unsigned long Timeout, unsigned char Retry);
unsigned int sendCommand_1(char *Command, char *Response, unsigned long Timeout, unsigned char Retry);
void Sys_Soft_Reset(void);
void open_lock(void);
void close_lock(void);
//void sendMessage(char *number,char *msg);

int main(void)
{	
	int aa,i;
	
	u16 led0pwmval=0;
	u8 dir=1;	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	//uart_init(115200);	 //串口初始化为115200
 //	LED_Init();			     //LED端口初始化
 	TIM3_PWM_Init(899,0);	 //不分频。PWM频率=72000000/900=80Khz
	
	
	
	
	
	delay_init();
	//NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 //串口初始化为9600
	USART2_Init(9600);	//串口2波特率9600
	u2_printf("AT\r\n");
	Init_LEDpin();
	Init_LEDpin_1();
	//cout<<"123456";
	
	
	printf("Welcome to use!\r\n");
	printf("ILoveMcu.taobao.com!\r\n");
	/*
	if (sendCommand("AT\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);
	delay_ms(10);

	if (sendCommand("AT+CPIN?\r\n", "READY", 1000, 10) == Success);
	else errorLog(2);
	delay_ms(10);


	if (sendCommand("AT+CGCLASS=\"B\"\r\n", "OK\r\n", 1000, 10) == Success);
	else errorLog(3);
	delay_ms(10);

	if (sendCommand("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK\r\n", 5000, 2) == Success);
	else errorLog(4);
	delay_ms(10);

	if (sendCommand("AT+CGATT=1\r\n","OK\r\n", 1000, 10) == Success);
	else errorLog(5);
	delay_ms(10);
	
	if(sendCommand("AT+CGATT?\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
		delay_ms(10);
	if(sendCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n","OK\r\n",1000,10) == Success);
		else errorLog(6);
		delay_ms(10);
			
		if(sendCommand("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n","OK\r\n",1000,10) == Success);
		else errorLog(7);
		delay_ms(10);

		if(sendCommand_("AT+SAPBR=1,1\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
		delay_ms(10);
	*/
	
	while(1)
	{
		
		GPIO_SetBits(GPIOB,GPIO_Pin_13);
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);
		GPIO_SetBits(GPIOB,GPIO_Pin_15);	
		TIM_SetCompare2(TIM3,200);		  
		/*for(i=0;i<30;i++)
    delay_ms(65535);
		GPIO_SetBits(GPIOB,GPIO_Pin_13);	
    GPIO_ResetBits(GPIOB,GPIO_Pin_14);
    for(i=0;i<30;i++)		*/
		delay_ms(1000);
		
		
	/*	
    printf("a_a_a_a_a_a_a_a\n");
		aa=web_flag();
			if(aa==1)
			{
			   printf("we need open the lock now!\n");
				GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			}
		else if(aa==0)
			printf("we don't need open the lock now!\n");
		else
			printf("web_error!\n");
		 if(sendCommand("AT+HTTPTERM\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
			delay_ms(3000);*/
		}
		//delay_ms(1000);	
	}
///////////////////////////////////////////////////////
void open_lock(void) //开锁成功网页
{
	
	
	
	
	
	
	
	
   do{
		if(sendCommand("AT+HTTPTERM\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
			delay_ms(30);
	if(sendCommand("AT+CGATT?\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
		delay_ms(10);
	if(sendCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n","OK\r\n",1000,10) == Success);
		else errorLog(6);
		delay_ms(10);
			
		if(sendCommand("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n","OK\r\n",1000,10) == Success);
		else errorLog(7);
		delay_ms(10);

		if(sendCommand_("AT+SAPBR=1,1\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
		delay_ms(10);
	
	 if(sendCommand("AT+HTTPINIT\r\n","OK\r\n",5000,2) == Success);
		else errorLog(11);
		delay_ms(10);
	if(sendCommand("AT+HTTPPARA=\"URL\",\"www.qyc2018.top/lock/open.php\"\r\n","OK",5000,2) == Success);
		else errorLog(12);
		delay_ms(20);
	 if(sendCommand_1("AT+HTTPACTION=0\r\n","HTTPACTION",20000,10) == Success);
		else errorLog(13);
		delay_ms(5000);
		
	if(sendCommand_("AT+HTTPREAD\r\n","HTTPREAD",20000,10) == Success);  // 读取数据
		else errorLog(14);
		delay_ms(4000);
	
	}while(strstr(USART2_RX_BUF,"lockopen") == NULL);

}
void close_lock(void) //关锁网页
{
	do{
		if(sendCommand("AT+HTTPTERM\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
			delay_ms(30);
	if(sendCommand("AT+CGATT?\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
		delay_ms(10);
	if(sendCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n","OK\r\n",1000,10) == Success);
		else errorLog(6);
		delay_ms(10);
			
		if(sendCommand("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n","OK\r\n",1000,10) == Success);
		else errorLog(7);
		delay_ms(10);

		if(sendCommand_("AT+SAPBR=1,1\r\n","OK\r\n",1000,10) == Success);
		else errorLog(8);
		delay_ms(10);
	
	 if(sendCommand("AT+HTTPINIT\r\n","OK\r\n",5000,2) == Success);
		else errorLog(11);
		delay_ms(10);
	if(sendCommand("AT+HTTPPARA=\"URL\",\"www.qyc2018.top/lock/close.php\"\r\n","OK",5000,2) == Success);
		else errorLog(12);
		delay_ms(20);
	 if(sendCommand_1("AT+HTTPACTION=0\r\n","HTTPACTION",20000,10) == Success);
		else errorLog(13);
		delay_ms(5000);
		
	if(sendCommand_("AT+HTTPREAD\r\n","HTTPREAD",20000,10) == Success);  // 读取数据
		else errorLog(14);
		delay_ms(4000);
	
	}while(strstr(USART2_RX_BUF,"lockclose") == NULL);
  
}
 int web_flag() //访问网页
{
//	if(sendCommand("AT+CGATT?\r\n","OK\r\n",1000,10) == Success);
//		else errorLog(8);
//		delay_ms(10);
//	if(sendCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n","OK\r\n",1000,10) == Success);
//		else errorLog(6);
//		delay_ms(10);
//			
//		if(sendCommand("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n","OK\r\n",1000,10) == Success);
//		else errorLog(7);
//		delay_ms(10);

//		if(sendCommand_("AT+SAPBR=1,1\r\n","OK\r\n",1000,10) == Success);
//		else errorLog(8);
//		delay_ms(10);
	
	 if(sendCommand("AT+HTTPINIT\r\n","OK\r\n",5000,2) == Success);
		else errorLog(11);
		delay_ms(10);
	if(sendCommand("AT+HTTPPARA=\"URL\",\"www.qyc2018.top/lock/flag.php\"\r\n","OK",5000,2) == Success);
		else errorLog(12);
		delay_ms(20);
	 if(sendCommand_1("AT+HTTPACTION=0\r\n","HTTPACTION",20000,10) == Success);
		else errorLog(13);
		delay_ms(5000);
		
	if(sendCommand_("AT+HTTPREAD\r\n","HTTPREAD",20000,10) == Success);  // 读取数据
		else errorLog(14);
		delay_ms(4000);
	 if (strstr(USART2_RX_BUF,"lockflag1") != NULL)
	 {
	    printf("web_success\n lockflag1\n");
		 return 1;
	 }
		else if(strstr(USART2_RX_BUF,"lockflag0") != NULL)
		{
		   printf("web_success\n lockflag0\n");
			return 0;
		}
		else
		{
		  printf("web_fail \n");
			printf("\n vlaue\n");
			printf(USART2_RX_BUF);
			delay_ms(500);
			return 2;
		}	
}
unsigned int sendCommand_1(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		u2_printf(Command); 		//发送GPRS指令
		
		printf("\r\n***************send****************\r\n");
		printf(Command);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			delay_ms(3000);
			Time_Cont += 100;
			while(strncmp(USART2_RX_BUF,USART2_RX_FLAG,200)<=0);
			if (strstr(USART2_RX_BUF, Response) != NULL)
			{				
				printf("\r\n***************receive1****************\r\n");
				printf(USART2_RX_BUF);
				USART2_CLR_Buf();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	printf("\r\n***************receive2****************\r\n");
	printf(USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}
unsigned int sendCommand_(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		u2_printf(Command); 		//发送GPRS指令
		
		printf("\r\n***************send****************\r\n");
		printf(Command);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			delay_ms(1000);
			Time_Cont += 100;
			if (strstr(USART2_RX_BUF, Response) != NULL)
			{				
				printf("\r\n***************receive1****************\r\n");
				printf(USART2_RX_BUF);
				//USART2_CLR_Buf();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	printf("\r\n***************receive2****************\r\n");
	printf(USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}


void errorLog(int num)
{
	printf("ERROR%d\r\n",num);
	while (1)
	{
		if (sendCommand("AT\r\n", "OK\r\n", 100, 10) == Success)
		{
			Sys_Soft_Reset();
		}
		delay_ms(200);
	}
}

void Sys_Soft_Reset(void)
{  
    SCB->AIRCR =0X05FA0000|(u32)0x04;      
}



unsigned int sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		u2_printf(Command); 		//发送GPRS指令
		
		printf("\r\n***************send****************\r\n");
		printf(Command);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			delay_ms(100);
			Time_Cont += 100;
			if (strstr(USART2_RX_BUF, Response) != NULL)
			{				
				printf("\r\n***************receive****************\r\n");
				printf(USART2_RX_BUF);
				USART2_CLR_Buf();
				return Success;
			}
			
		}
		Time_Cont = 0;
	}
	printf("\r\n***************receive****************\r\n");
	printf(USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}





