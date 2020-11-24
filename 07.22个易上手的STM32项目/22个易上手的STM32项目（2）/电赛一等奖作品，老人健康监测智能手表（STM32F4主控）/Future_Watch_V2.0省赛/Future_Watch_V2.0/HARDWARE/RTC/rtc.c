#include "rtc.h"
#include "led.h"
#include "delay.h"

Clock My_Clock;			//创建时间结构体
//等待RSF同步
//返回值:0,成功;1,失败;
u8 RTC_Wait_Synchro(void)
{ 
	u32 retry=0XFFFFF; 
	//关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	RTC->ISR&=~(1<<5);		//清除RSF位 
	while(retry&&((RTC->ISR&(1<<5))==0x00))//等待影子寄存器同步
	{
		retry--;
	}
    if(retry==0)return 1;	//同步失败 
	RTC->WPR=0xFF;			//使能RTC寄存器写保护  
	return 0;
}
//RTC进入初始化模式
//返回值:0,成功;1,失败;
u8 RTC_Init_Mode(void)
{ 
	u32 retry=0X10000; 
	if(RTC->ISR&(1<<6))return 0; 
	RTC->ISR|=1<<7;	//进入RTC初始化模式
	while(retry&&((RTC->ISR&(1<<6))==0x00))//等待进入RTC初始化模式成功
	{
		retry--;
	}
    if(retry==0)return 1;	//同步失败
	else return 0; 			//同步成功 
}
//RTC写入后备区域SRAM
//BKPx:后备区寄存器编号,范围:0~19
//data:要写入的数据,32位长度
void RTC_Write_BKR(u32 BKRx,u32 data)
{
	u32 temp=0; 
	temp=RTC_BASE+0x50+BKRx*4;   
	(*(u32*)temp)=data; 
}
//RTC读取后备区域SRAM
//BKPx:后备区寄存器编号,范围:0~19
//返回值:读取到的数据
u32 RTC_Read_BKR(u32 BKRx)
{
	u32 temp=0; 
	temp=RTC_BASE+0x50+BKRx*4;  
	return (*(u32*)temp);		//返回读取到的值
}

//十进制转换为BCD码
//val:要转换的十进制数
//返回值:BCD码
u8 RTC_DEC2BCD(u8 val)
{
	u8 bcdhigh = 0; 
	while(val>=10)
	{
		bcdhigh++;
		val-=10;
	} 
	return ((u8)(bcdhigh<<4)|val);
}
//BCD码转换为十进制数据
//val:要转换的BCD码
//返回值:十进制数据
u8 RTC_BCD2DEC(u8 val)
{
	u8 temp=0;
	temp=(val>>4)*10;
	return (temp+(val&0X0F));
}
//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:AM/PM,0=AM/24H,1=PM.
//返回值:0,成功
//       1,进入初始化模式失败 
u8 RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	u32 temp=0;
	//关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	if(RTC_Init_Mode())return 1;//进入RTC初始化模式失败
	temp=(((u32)ampm&0X01)<<22)|((u32)RTC_DEC2BCD(hour)<<16)|((u32)RTC_DEC2BCD(min)<<8)|(RTC_DEC2BCD(sec));
	RTC->TR=temp;
	RTC->ISR&=~(1<<7);			//退出RTC初始化模式 
	return 0; 
}
//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:0,成功
//       1,进入初始化模式失败 
u8 RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	u32 temp=0;
 	//关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	if(RTC_Init_Mode())return 1;//进入RTC初始化模式失败
	temp=(((u32)week&0X07)<<13)|((u32)RTC_DEC2BCD(year)<<16)|((u32)RTC_DEC2BCD(month)<<8)|(RTC_DEC2BCD(date)); 
	RTC->DR=temp;
	RTC->ISR&=~(1<<7);			//退出RTC初始化模式 
	return 0; 
}
//获取RTC时间
//*hour,*min,*sec:小时,分钟,秒钟 
//*ampm:AM/PM,0=AM/24H,1=PM.
void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm)
{
	u32 temp=0;
 	while(RTC_Wait_Synchro());	//等待同步  	 
	temp=RTC->TR;
	*hour=RTC_BCD2DEC((temp>>16)&0X3F);
	*min=RTC_BCD2DEC((temp>>8)&0X7F);
	*sec=RTC_BCD2DEC(temp&0X7F);
	*ampm=temp>>22; 
}
//获取RTC日期
//*year,*mon,*date:年,月,日
//*week:星期
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week)
{
	u32 temp=0;
 	while(RTC_Wait_Synchro());	//等待同步  	 
	temp=RTC->DR;
	*year=RTC_BCD2DEC((temp>>16)&0XFF);
	*month=RTC_BCD2DEC((temp>>8)&0X1F);
	*date=RTC_BCD2DEC(temp&0X3F);
	*week=(temp>>13)&0X07; 
}
//RTC初始化
//返回值:0,初始化成功;
//       1,LSE开启失败;
//       2,进入初始化模式失败;
u8 RTC_Init(void)
{
	u16 retry=0X1FFF; 
	RCC->APB1ENR|=1<<28;			//使能电源接口时钟
	PWR->CR|=1<<8;					//后备区域访问使能(RTC+SRAM)
	if(RTC_Read_BKR(0)!=0X5050)		//是否第一次配置?
	{
		RCC->BDCR|=1<<0;			//LSE 开启  
		while(retry&&((RCC->BDCR&0X02)==0))//等待LSE准备好
		{
			retry--;
			delay_ms(5);
		}
		if(retry==0)return 1;		//LSE 开启失败. 
		RCC->BDCR|=1<<8;			//选择LSE,作为RTC的时钟
		RCC->BDCR|=1<<15;			//使能RTC时钟
 		//关闭RTC寄存器写保护
		RTC->WPR=0xCA;
		RTC->WPR=0x53; 
		if(RTC_Init_Mode())return 2;//进入RTC初始化模式
		RTC->PRER=0XFF;				//RTC同步分频系数(0~7FFF),必须先设置同步分频,再设置异步分频,Frtc=Fclks/((Sprec+1)*(Asprec+1))
		RTC->PRER|=0X7F<<16;		//RTC异步分频系数(1~0X7F)
		RTC->CR&=~(1<<6);			//RTC设置为,24小时格式
		RTC->ISR&=~(1<<7);			//退出RTC初始化模式
		RTC->WPR=0xFF;				//使能RTC寄存器写保护  
		RTC_Set_Time(23,59,56,0);	//设置时间
		RTC_Set_Date(14,5,5,1);		//设置日期
		RTC_Write_BKR(0,0X5050);	//标记已经初始化过了
	} 
	//RTC_Set_WakeUp(4,0);			//配置WAKE UP中断,1秒钟中断一次 
	return 0;
}

//周期性唤醒定时器设置
//wksel:000,RTC/16;001,RTC/8;010,RTC/4;011,RTC/2;
//      10x,ck_spre,1Hz;11x,1Hz,且cnt值增加2^16(即cnt+2^16)
//注意:RTC就是RTC的时钟频率,即RTCCLK!
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(u8 wksel,u16 cnt)
{ 
	//关闭RTC寄存器写保护
	RTC->WPR=0xCA; 
	RTC->WPR=0x53;
	RTC->CR&=~(1<<10);			//关闭WAKE UP
	while((RTC->ISR&0X04)==0);	//等待WAKE UP修改允许
	RTC->CR&=~(7<<0);			//清除原来的设置
	RTC->CR|=wksel&0X07;		//设置新的值
	RTC->WUTR=cnt;				//设置WAKE UP自动重装载寄存器值
	RTC->ISR&=~(1<<10);			//清除RTC WAKE UP的标志
	RTC->CR|=1<<14;				//开启WAKE UP 定时器中断
	RTC->CR|=1<<10;				//开启WAKE UP 定时器　
	RTC->WPR=0XFF;				//禁止修改RTC寄存器 　
	EXTI->PR=1<<22;  			//清除LINE22上的中断标志位  
	EXTI->IMR|=1<<22;			//开启line22上的中断 
	EXTI->RTSR|=1<<22;			//line22上事件上升降沿触发 
	MY_NVIC_Init(2,2,RTC_WKUP_IRQn,2); //抢占2，子优先级2，组2 
}


//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC->ISR&(1<<10))//WK_UP中断?
	{ 
		RTC->ISR&=~(1<<10);	//清除中断标志
		LED1=!LED1; 
	}   
	EXTI->PR|=1<<22;	//清除中断线22的中断标志 								
}
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//year,month,day：公历年月日 
//返回值：星期号(1~7,代表周1~周日)																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	temp2%=7;
	if(temp2==0)temp2=7;
	return temp2;
}	
